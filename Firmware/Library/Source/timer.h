#ifndef __TPU_H				/* Avoid multiple inclusions */
#define __TPU_H

/*
 * Other includes
 */
							   
#include <shortdef.h>
#include <math.h>
#include <float.h>

/*
 * General definitions
 */

#define	MAX_TPU_USED		8		/* The number of the lowest channel used */

typedef volatile packed struct {
	ushort int	TMCR;
	ushort int	TTCR;
	ushort int	DSCR;
	ushort int	DSSR;
	ushort int	TICR;
	ushort int	CIER;
	ushort int	CFS[4];
	ushort int	HSQR[2];
	ushort int	HSSRR[2];
	ushort int	CPR[2];
	ushort int	CISR;
} TPU_STRUCT;

typedef enum tpu_modes
{
	NONE, IN, OUT, PWM, SPWM, SLAVE, FREQ, COUNT, TRIGOUTS, LONGTRIG,
	TRIGSYNC, TRIGOUTP,TRIGOUTM, ONE_SHOT, FGEN,REPEAT_INT_GEN,ECAT_FREQ,
	INT_GEN, CATCH, DELAY, DELAY_INT,REPEAT_DELAYINT,SURGE_SYNC,SURGE_HELPER,
	SURGE_OUTPUT,LIST_SYNC,LIST_OUT,LIST_HELPER,SYNC_CHANNEL
}	TPU_MODE;

extern  volatile TPU_MODE mode[16];
 
typedef void	(*PF)(void);

typedef struct listelement
{
	unsigned short extra;
  unsigned short	value;		/* Do not change this to a char!!! */
	unsigned short	wait_ticks;
} LISTELEMENT;

/*
 *	PRIVATE function definitions
 *
 *		TPU_trap 			-- Interrupt handler for all TPU functions
 *		TPU_channel_init	-- Used by various functions to setup a TPU channel
 */

interrupt void	TPU_trap(void);
void	TPU_channel_init(uchar,uchar,uchar,uchar,uchar,uchar);
/*
 *	PUBLIC function definitions
 *
 *		TPU_init		-- Initializes the TPU subsystem
 *		TPU_write	-- Forces a TPU output to the specified state
 *		TPU_read		-- Reads the state of a TPU channel
 *		TPU_kill		-- Shuts down a TPU channel
 *		TPU_set_pwm			-- Sets up a TPU channel as a PWM signal
 *		TPU_set_spwm		-- Sets up a two-channel PWM signal
 *		TPU_read_tcr1		-- Reads the TCR1 value using an unused TPU channel
 *		TPU_delay			-- Delays for a specified number of milliseconds
 *		TPU_delay_int		-- Delays then causes an interrupt?? (not sure)
 *		TPU_start_freq		-- Sets up a TPU channel to do frequency measurement
 *		TPU_read_freq		-- Reads the actual frequency (see previous)
 *		TPU_start_event	-- Sets up a TPU channel to do event counting
 *		TPU_read_event		-- Reads the actual number of events passed
 *		TPU_one_shot		-- Outputs a single pulse on a TPU channel
 *		TPU_interrupt		-- Sets up a TPU channel to interrupt on a trigger
 *		TPU_read_int_pin	-- Read the TPU channel that caused the interrupt
 *		TPU_periodic_int	-- Sets up a TPU channel to interrupt periodically
 *		TPU_phase_trigger	-- Sets up a TPU channel to cause a zero-crossing
 *									on it to cause a phase related event on another
 *									TPU channel.
 *		TPU_surge_phase	-- Causes a single transition to occur on an output 
 *									channel with reference to a ECAT sync signal on 
 *									the specified input channel. This function also
 *									takes a parameter that specifies the time delay ,
 *									(predelay) in the firing circuit.
 */

void		TPU_init(void);
void		TPU_write(uchar,uchar);
void		TPU_set_pwm(uchar,ushort,ushort);
void		TPU_set_spwm(uchar,ushort,ushort,ushort);
void		TPU_delay(int);
int		TPU_delay_int(int,void (*p)());
int		TPU_repeat_delayint(char,int,void (*p)());
void		TPU_start_freq(char,char);
void		TPU_start_event(uchar);
void		TPU_one_shot(uchar,ushort,ushort,void (*p)());
void		TPU_interrupt(uchar,short,void (*p)());
int		TPU_periodic_int(uint,void (*p)());
void		TPU_phase_trigger(uchar,uchar,uchar,uint,uint,uint,void p());
uchar	TPU_read_int_pin(uchar);
void		TPU_kill(uchar);
char		TPU_read(uchar);
int		TPU_read_tcr1(void);
long		TPU_read_event(uchar);
float	TPU_read_freq(char);
void		TPU_ECAT_phase(char,char,int,long,uchar,void (*p)());
int		TPU_ECAT_phase_done(void);
void		TPU_ECAT_list_phase(char,char,int,LISTELEMENT *,ushort *);
int		TPU_ECAT_list_phase_done(void);
void		process_Ecat_Freq(uchar);
float	   TPU_ECAT_freq(char sync_ch);
void     TPU_sync_start(char ch);

#endif							/* ifndef __TIMER.H */


