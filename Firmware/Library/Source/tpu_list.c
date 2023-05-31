#include <stdio.h>
#include "timer.h"
#include "switches.h"

#define SAFETIME	0x1000

extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile uchar	happened[16];
volatile long	LIST_TIMES[4];
static int	done_with_list_pulse;

/*
 *	Initialize()
 *	Set TPU out to the desired B4 event value;
 *	Set the Mode of the Time Sync Channel to LIST_SYNC
 *	Start Input Capture on Time Sync channel for both edges.
 *	set Happened[LIST_SYNC] = 0;
 *	param1[LIST_SYNC] = Phase in Degrees;
 *	param2[LIST_SYNC] = Time Delay in TPU counts for real pulse
 *	param3[LIST_SYNC] HIWORD = Helper Channel
 *	param3[LIST_SYNC] LOWORD = Output Channel 
 *	param1[LIST_HELPER] = listpointer
 *	param2[LIST_HELPER] = register address
 *	param3[LIST_HELPER] HIWORD = Sync Channel
 *	param3[LIST_HELPER] LOWORD = Output Channel
 *
 *	param1[LIST_OUTPUT] = listpointer
 *	param2[LIST_OUTPUT] = register address;
 *
 *	param3[LIST_OUTPUT] HIWORD = Sync Channel
 *	param3[LIST_OUTPUT] LOWORD = Helper Channel
 *	return; 
 */

void	TPU_ECAT_list_phase(
char	sync_ch,
char	out_ch,
int	phase,
LISTELEMENT	*listel,
ushort	*Reg_Addr)
{
	int	helper_ch;
	int	i;

	printf("Sync ch=%d out_ch=%d\n\r",sync_ch,out_ch);
	done_with_list_pulse = 0;
	do
		for(helper_ch = 15; ((mode[helper_ch]!=NONE) && (helper_ch > MAX_TPU_USED)); helper_ch--)
			;
	while(mode[helper_ch] != NONE);

	TPU_write(out_ch,0);				/* Set it LOW!	*/
	*Reg_Addr = listel[0].value;

	mode[sync_ch]	= LIST_SYNC;
	mode[helper_ch]= LIST_HELPER;

	param1[sync_ch] = (phase % 360);
	param2[sync_ch] = 0;							/* Predelay; Used for scratch */
	param3[sync_ch] = (helper_ch * 0x10000) | out_ch;

	param1[helper_ch] =(long)listel;
	param2[helper_ch] =(long)Reg_Addr;
	param3[helper_ch] =(sync_ch * 0x10000) | out_ch;
 
	param1[out_ch] = (long)listel;
	param2[out_ch] = (long)Reg_Addr;
	param3[out_ch] = (sync_ch * 0x10000) | helper_ch;

	happened[out_ch]	= 0;
	happened[sync_ch]	= 0;

	/* Now to set up the input channel to be input capture on both edges!	*/

	TPU_rams[sync_ch][0] = 0x07;	/* Detect rising edge		*/
	TPU_rams[sync_ch][1] = 0x0E;	/* Start link and count=0, Bank - 0E (unused) */
	TPU_rams[sync_ch][2] = 0x01;	/* Count 1 Transition		*/
											/* fun  HSR HSQ PRIO INT	*/
	TPU_channel_init(sync_ch,0x0A, 1 , 1 , 3 , 1);
	return;
}

void	process_list_Sync(
uchar	ch)
{
	long	P;
	long	PT;
	long	ADT;
	uchar	helper_ch;
	uchar	out_ch;

	if(happened[ch] < 4)
	{
		LIST_TIMES[happened[ch]] = TPU_rams[ch][4];
		happened[ch]++;
	}

	switch(happened[ch])
	{
		case 1:
			TPU_rams[ch][0] = 0x0F;					/* Detect either edge */
			TPU_channel_init(ch,0x0A,1,1,3,1);
		case 3:
		case 5:
			return;
		case 2:
			if(LIST_TIMES[0] > LIST_TIMES[1])
				LIST_TIMES[1] += 0x10000;
			LIST_TIMES[0] = (LIST_TIMES[1]+LIST_TIMES[0]) / 2;
			/* Surge time [0] now has the zero # 1 Time! */
			return;
		case 4:
			if(LIST_TIMES[2] > LIST_TIMES[3])
				LIST_TIMES[3] += 0x10000;
			LIST_TIMES[2] = (LIST_TIMES[3]+LIST_TIMES[2]) / 2;
			/* Surge time [2] now has the zero # 2 Time! */
			while(LIST_TIMES[2] < LIST_TIMES[0])
				LIST_TIMES[2] += 0x10000;
			while(LIST_TIMES[3] < LIST_TIMES[2])
				LIST_TIMES[3] += 0x10000;
			P	 = LIST_TIMES[2] - LIST_TIMES[0];
			PT  = LIST_TIMES[3] - LIST_TIMES[1];
			PT=(100*P)/PT;
			if((PT>105) || (PT<95))
        {
				happened[ch] = 0;
        	TPU_rams[ch][0] = 0x07;	/* Detect rising edge		*/
		    	TPU_channel_init(ch,0x0A,1,1,3,1);
   			return;
			}
			PT	 = param1[ch] * P / 360;
			ADT = PT;							/* -param2[ch]; */
			helper_ch = ((param3[ch] >> 16) & 255);
			out_ch = param3[ch] & 255;
			mode[out_ch] = LIST_OUT;
			while(ADT < ((LIST_TIMES[3]-LIST_TIMES[2])+SAFETIME))
				ADT += P;
			ADT = ADT + LIST_TIMES[2] - LIST_TIMES[3];
			param2[ch] = ADT / 2;
           ADT=ADT-(ADT/2);
			/* ADT now has the time from the last transition to the next LIST_HELPER event */
			/* Schedule Surge helper to happen at the next event */
			/* Use the OC function */
			TPU_rams[helper_ch][0] = 0x8C;
			TPU_rams[helper_ch][1] = ADT;
			TPU_rams[helper_ch][2] = ch * 0x10 + 0x08; /* Address of last tran in  ch */
			TPU_rams[helper_ch][3] = 0;
			TPU_channel_init(helper_ch,0x0E,1,0,3,1);
			TPU_kill(ch);
			return;
	}	/* End of case */ 
}

void	process_list_Help(
uchar	ch)
{
	uchar	out_ch;
	int	dlytime;
	int	sync_ch;

	/* Schedule LIST_OUT to happen at LIST_HELPER+param2[ch]; */
	sync_ch = (param3[ch] >> 16) & 255;
	out_ch  = param3[ch] & 255;
	dlytime = param2[sync_ch];

	TPU_rams[out_ch][0] = 0x86;	/* Positive edge */
	TPU_rams[out_ch][1] = dlytime;
	TPU_rams[out_ch][2] = ch * 0x10 + 0x0A;	/* Address of last tran in ch */
	TPU_rams[out_ch][3] = 0;
	mode[out_ch] = LIST_OUT;
	TPU_channel_init(out_ch,0x0E,1,0,3,1);
	TPU_kill(ch);
}

void	process_list_Out(
uchar	ch)
{
	LISTELEMENT	*listel;
	ushort	*addrp;
	uint		last_time;
	ushort	dlytime;

	listel = (LISTELEMENT *)param1[ch];
	/* ((LISTELEMENT *)param1[ch])++; */
	addrp = (ushort *)param2[ch];

	dlytime = listel[happened[ch]].wait_ticks;
	/* dlytime = listel[0].wait_ticks; */

	happened[ch]++;
	if(listel[happened[ch]].wait_ticks)
    *addrp = listel[happened[ch]].value;
	else	
   {done_with_list_pulse=1;
	 TPU_kill(ch);
	 return;
	 }

	if(dlytime)
	{ /*Schedule next! */
		TPU_rams[ch][0] = 0x86;	/* 0010000110; Force Pin low then High on match! */
		TPU_rams[ch][1] = dlytime;
		TPU_rams[ch][2] = ch * 0x10 + 0x08;	/* Address of last tran in  ch */
		TPU_rams[ch][3] = 0;
		TPU_channel_init(ch,0x0E,1,0,3,1);
	}
	else
	{
		done_with_list_pulse = 1;
		TPU_kill(ch);
	}
}

int	TPU_ECAT_list_phase_done(void)
{
	return(done_with_list_pulse!=0);
}
