#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	TPU_rami[16][4];

/*
 *	Uses TPU Synchronized PWM  (p3-43 to p3-56)
 *	CH is programmed in mode 2 and CH+1 in mode 0
 *	This function will setup a two channel PWM with user defined:
 *		period
 *		on time
 *		dt (dead time)
 *	Starting at Channel ch.
 *
 *
 *				旼컴컴컴컴컴 period 컴컴�
 *				쳐컴 on 컴컴�				�
 *				�			�dt�			�dt�
 *				�			�	�			�	�
 *				旼컴컴컴커					旼컴컴컴커
 *	CH		컴켸        읕컴컴컴컴컴컴켸        읕컴컴컴�
 *								旼컴컴컴커						旼�
 *	CH+1	컴컴컴컴컴컴컴켸			읕컴컴컴컴컴컴컴컴�
 *
 */

void	TPU_set_spwm(
uchar	ch,				/* Channel number */
ushort	period,		/* Period			*/
ushort	on,			/* On time			*/
ushort	dt)			/* Dead time		*/
{
	if(mode[ch] != SPWM)
	{									/* init SPWM */
		mode[ch]   = SPWM;
		mode[ch+1] = SLAVE;
		TPU_channel_init(ch,0x00,0,0,0,0);		/* Turn the channel off		*/
		TPU_channel_init(ch+1,0x00,0,0,0,0);	/* Turn the channel off		*/
		TPU_rams[ch][0]	= 0x0090;				/* Set up the PAC for init */
      TPU_rams[ch+1][0] = 0x0090;				/* Set up the PAC for init */

/* Write the channel on time (on - dt) and period int TPU RAM */
		TPU_rami[ch][1] = ((uint)(on - dt)) * 0x10000 + period;
/*
 *	The next line sets:
 *		start_LINK_CHANNEL = CH+1
 *		LINK_CHANNEL_COUNT = 1
 *		REF_ADDR offset to LASTRISE  (16 * ch)
 */
		TPU_rams[ch][4] = ((((ch+1)*16)+1)*256)+(ch*16);
		TPU_rams[ch][5] = 0;											/* CH delay to 0	*/

/* Write the on time (period -(on+dt)) and period into TPU ram for ch+1 */

		TPU_rami[ch+1][1] = ((uint)(period-(on+dt)))* 0x10000 + period;
		TPU_rams[ch+1][4] = (ch*16);	/* Set up the reference addr for ch+1	*/
		TPU_rams[ch+1][5] = on;			/* Set delay to ch+1 from rise of ch	*/
		TPU_channel_init(ch,0x07,2,2,2,0);
		TPU_channel_init(ch+1,0x07,2,0,2,0);
	}	/* init SPWM */
	else
	{	/* Update parameters only */
		TPU_rami[ch][1]	= ((uint)(on-dt))* 0x10000 + period;
		TPU_rami[ch+1][1] = ((uint)(period-(on+dt))) * 0x10000 + period;
		TPU_rams[ch+1][5] = on;
	}
}
