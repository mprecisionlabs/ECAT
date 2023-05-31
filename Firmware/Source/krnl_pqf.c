#include <stdio.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <mriext.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "serint.h"
#include "buffer.h"
#include "timer.h"
#include "parser.h"
#include "qsm.h"
#include "switches.h"
#include "display.h"
#include "key.h"
#include "duart.h"
#include "front.h"
#include "intface.h"
#include "vectors.h"
#include "pqf.h"
#include "ecat.h"
#include "tiocntl.h"
#include "period.h"

extern volatile uchar	PQF_softstartflag;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern EFTDATA	eftdata;
extern PQFDATA	pqfdata;
extern ACDATA		acdata;
extern PKDATA		peakdata;
extern P_PARAMT	paraml[3];
extern uchar		back_from_local;    /* JLR */
extern uchar		keyenable;

extern volatile uchar	AD_HIT;
extern volatile ushort	PEAK_RELAYS;
extern volatile uint	ilockstate;
extern volatile ushort	PEAK_LD5;
extern volatile ushort	TPU_rams[16][8];
extern volatile ulong	param1[];
extern volatile ulong	param2[];
extern volatile ulong	param3[];
extern volatile ulong	vectors[];
extern int	PQF_nominal_V;
extern int	PQF_running;
extern int	PQF_actual_V;
extern float	PQF_amps_rms;
extern int		PQF_amps_peak;
extern int		PQF_step_peak;
extern int		last_zero;
extern char	*err_msg;

volatile int	MAXRMS;
volatile int	NEXTMAXRMS;
extern volatile int	MAXPEAK;
extern volatile int	MINPEAK;
extern volatile unsigned int	LIMITHIT;
extern volatile unsigned int	VAL_LIMITHIT;
extern volatile unsigned int	STEP_LIMIT;
extern TPU_STRUCT	*TPU;
extern const P_ELEMENT	pqf_table[];
extern volatile ushort	ldo_state;
extern int adtotal_cnt;

/* For former NEW_PQF3.C section */
extern unsigned short AD_CH;
extern int slope_cnt;
extern int PUB_RMS_SUM;
extern int PUB_RMS_N;
extern int RMS_ZERO;
extern int LIN_RMS_ZERO;
extern int RMS_PEAKP;
extern int RMS_PEAKM;
extern unsigned short  *ADADD;
extern unsigned short  *ADCMPL;
extern  int 	ADRMS_trap;
extern  int   ADRMS_END;
extern volatile ushort	PITR;
uint     pitr_cnt;


void	do_remote();
void	PQF_init(uint);
interrupt void	PQF_help_Trap(void);
interrupt void	PQF_event_Trap(void);
interrupt void	PQF_zero_Trap(void);

typedef struct DETAIL_PQF_LIST_ELEMENT
{
	unsigned int	Switch_Duration;
	unsigned short	Relay_Values;
	unsigned int	User_Step_Num;
	unsigned char	Switch_Byte;
	unsigned char	time_not_cycles;
} DETAIL_LIST_ELEMENT;


/*
 *
 *	The action char has the following format:
 *
 *	  ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Event/Loop command 0=Event 1= Loop Command
 *	  ³ ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ Mode 0=Seconds in tenths,  1=Cycles
 *	  ³ ³     ÚÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÂ Tap/Type
 * msb E M x x t t t t             ³	0	= Open
 *											³	1	= Shorted
 *											³	2	= 40%
 *											³	3	= 50%
 *											³	4	= 70%
 *											³	5	= 80%
 *											³	6	= 90% 
 *											³	7	= 100%
 *											³	8	= 110%
 *											³	9	= 120%
 *											³	10	= 150%
 *											³Type
 *											³	0 - Branch to the step number in 'duration'.
 *											³	1 - Branch to the step number in 'duration','RMS_limit' times
 *											³	2 - Gosub to the step number in 'duration'.
 *											³	3 - Gosub to the step number in 'duration', 'RMS_limit' times
 *											³	4 - Return from the subroutine.
 *											ÀÄ	5 - Stop the program.
 */

void  Start_Zero(char sync_ch);

int	list_size;                                                                                                                                                 
LIST_RECORD	list_recs[MAXIMUM_LIST];
static DETAIL_LIST_ELEMENT	detail_list[255];
static unsigned long	return_stack[32];
static unsigned char	return_count;
static unsigned char	fill_busy;

unsigned int	Last_Period;				/* The period of one cycle */
unsigned int	Omni_Last_Zero_Time;		/* Last Zero crossing time */
unsigned int	Omni_Own_Clock;			/* Cumulative Degrees since time began		*/
int	Omni_Public_Edge;			/* The time that corresponds with Omni Public Clock */
int	Omni_Public_Clock;		/* Cumulative Degrees since time began		*/
unsigned int	Time_Margin;				/* The remainder of time not allocated to a 100th of a second */
unsigned int	Active_Step;
unsigned int	Int_Hit;
unsigned int	zero_cnt;
unsigned int	last_relay;
unsigned int	last_switch;
unsigned int	global_relay;
unsigned int	global_mode;
unsigned int	default_level;
int	global_error;
int	Next_Event;								/* Cumulative Degrees for the next event			 */
													/* or Hundredths of a second till the next event */

/* The Time/Degree Mode flag and Active/Waiting Mode flag
 *	T/D	A/W	Mode
 *	T		A		3
 *	T		W		2	; Don't care
 *	D		A		1
 *	D		W		0
 */
volatile unsigned char	mode_flag;
volatile unsigned char	inter_head;			/* The list of the next events to transpire	*/
volatile unsigned char	inter_tail;			/* The list of next events to transpire		*/
volatile unsigned char	inter_size;
volatile unsigned char	Zero_Running;
DETAIL_LIST_ELEMENT inter_list[256];		/* The list of next events to transpire		*/

extern volatile ushort	TPUCISR;
static int		lhappened;
static ushort	*ramch;
static ushort	ZERO_CISR_MASK;
static ushort	EVENT_CISR_MASK;
static ushort	HELP_CISR_MASK;
static ushort	EVENT_HSR_MASK;
static ushort	HELP_HSR_MASK;
static ushort	EVENT_HSR_VAL;
static ushort	HELP_HSR_VAL;
static ushort	ZERO_HSR_VAL;
static ushort	ZERO_HSR_MASK;
static ushort	*EVENT_HSR_ADDR;
static ushort	*HELP_HSR_ADDR;
static ushort	*ZERO_HSR_ADDR;

ushort	PQF_drawer;
uint	relay_state[16];

static long	PQFFREQ[4];
static int		PQF_sync_ch;
static int		PQF_event_ch;
static int		PQF_help_ch;
static int		maxdt;
static int		mindt;
int space;
volatile int	PQF_stop;

void	raw_pre_run(int start);

void	raw_relay(
uint	relay,
uint	value)
{
	int	newrelay;

	newrelay = relay_state[PQF_drawer];
	if(value)
		newrelay |= (value << relay);
	else
		newrelay &= ~((value | 0x01) << relay);

	if(relay_state[PQF_drawer] != newrelay)
	{
		if(relay < 16)
			ECAT_WriteRelay0(PQF_drawer,newrelay);
		else
			ECAT_WriteRelay1(PQF_drawer,(newrelay >> 16));
		relay_state[PQF_drawer] = newrelay;
	}
}

int	save_mem(unsigned char *from, int to, int size)
{
	int	i;
	unsigned short	*ip;
	unsigned short	ti;
	unsigned char	*cp;

	ip  = (unsigned short *)(0x4E000);
	ip += to;
	cp  = from;
	for(i = 0; i < size; i++)
	{
		ti  = *cp++;
		ti *= 257;
		*ip++ = ti;
	}
	return(i+to);
}

int	load_mem(unsigned char *to, int from, int size)
{
	int	i;
	unsigned short	*ip;
	unsigned short	ti;
	unsigned char	*cp;

	ip  = (unsigned short *)(0x4E000);
	ip += from;
	cp  = to;
	for(i = 0; i < size; i++)
	{
		ti = *ip++;
		ti = ti >> 8;
		ti = ti & 0xff;
		*cp++ = ti;
	}
	return(i+from);
}

void	raw_add_list(
int	listn,
int	offset,
P_PARAMT	*params)
{
	list_recs[listn].action			= EXTRACT_INT(offset+0);
	list_recs[listn].duration		= EXTRACT_INT(offset+1);
	list_recs[listn].start_angle	= EXTRACT_INT(offset+2);
	list_recs[listn].RMS_limit		= EXTRACT_INT(offset+3);
	list_recs[listn].PEAK_limit	= EXTRACT_INT(offset+4);
}

int	PQF_reset_list(P_PARAMT * params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		list_size = 0;
   }
	return(0);
}

int	PQF_set_list(P_PARAMT * params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		list_size++;
		if(list_size >= MAXIMUM_LIST)
			return(BADNUM);
		else
			raw_add_list(list_size-1,0,params);
   }
	return(0);
}

int	PQF_setn_list(P_PARAMT * params)
{
	int	listn;

	if(pqfdata.pqfflag)
	{
		do_remote();
		listn = EXTRACT_INT(0);
		if(list_size < listn)
			return(BADNUM);
		else
			raw_add_list(listn,1,params);
   }
	return(0);
}

void	raw_save()
{
	int	size;

	size = 4 + sizeof(LIST_RECORD) * (list_size + 1);
	save_mem((unsigned char *)&list_size,0,size);
}

int	PQF_save_list(P_PARAMT *params )
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		raw_save();
   }
	return(0);
}

void	raw_load()
{
	int	size;

	load_mem((unsigned char *)&list_size,0,4);
	size = 4 + sizeof(LIST_RECORD) * (list_size + 1);
	load_mem((unsigned char *)&list_size,0,size);
}

int	PQF_load_list(P_PARAMT* params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		raw_load();
	}
	return(0);
}

unsigned short	Get_A_Tap(
unsigned int	rv)
{
	switch(rv & A_MASK)
	{ 
		case A_ZERO:
			return(S_SHORT);
		case A_40:
			return(S_40);
		case A_50:
			return(S_50);
		case A_70:
			return(S_70);
		case A_80:
			return(S_80);
		case A_90:
			return(S_90);
		case A_110:
			return(S_110);
		case A_150:
			return(S_150);
		default:
			return(0xFF);
	}
}

unsigned int	Get_A_relay(
unsigned short	rv)
{
	switch(rv & 0x0F)
	{ 
		case S_SHORT:
			return(A_ZERO);
		case S_40:
			return(A_40);
		case S_50:
			return(A_50);
		case S_70:
			return(A_70);
		case S_80:
			return(A_80);
		case S_90:
			return(A_90);
		case S_110:
			return(A_110);
		case S_150:
			return(A_150);
		default:
			return(A_ZERO);
	}
}

unsigned short	Get_B_Tap(
unsigned int	rv)
{
	switch(rv & B_MASK)
	{ 
		case B_ZERO:
			return(S_SHORT);
		case B_40:
			return(S_40);
		case B_50:
			return(S_50);
		case B_70:
			return(S_70);
		case B_80:
			return(S_80);
		case B_90:
			return(S_90);
		case B_110:
			return(S_110);
		case B_120:
			return(S_120);
		default:
			return(0xFF);
	}
}

unsigned int	Get_B_relay(
unsigned short	rv)
{
	switch(rv & 0x0F)
	{
		case S_SHORT:
			return(B_ZERO);
		case S_40:
			return(B_40);
		case S_50:
			return(B_50);
		case S_70:
			return(B_70);
		case S_80:
			return(B_80);
		case S_90:
			return(B_90);
		case S_110:
			return(B_110);
		case S_120:
			return(B_120);
		default:
			return(B_ZERO);
	}
}

/*
 *	This routine will scan forward from the present point (inter_head_set)
 *	and determine what relays will be necessary.
 *	Rules:
 *		If there are only two relay taps the relay values should not be
 *		changed after the initial setup.
 *		At this time we will not try to fix 150 and 120% problems
 *		Scan ahead for 50 msec past the next step to determine
 *		the relays necessary.
 */
void	raw2_insert(
unsigned int	offset,
unsigned int	duration,
unsigned char	time_mode,
unsigned char	SCR)
{
	DETAIL_LIST_ELEMENT	*detail_lr;
	unsigned char	SCR_MASK;

	if(SCR)
		SCR_MASK = 0xFF;
	else
		SCR_MASK = (IG_100|IG_A|IG_B);

	detail_lr = &inter_list[inter_head];
	detail_lr->User_Step_Num	= offset;
	detail_lr->Relay_Values		= global_relay;
	detail_lr->Switch_Byte		= (list_recs[offset].Switch_Values & SCR_MASK);
	detail_lr->Switch_Duration	= duration;
	detail_lr->time_not_cycles	= time_mode;
	inter_head++;
	inter_size++;
}

void	raw3_insert(
unsigned int	offset,
unsigned int	duration,
unsigned char	time_mode,
unsigned char	SCR,
unsigned char	tap_byte,
unsigned int	relayv)
{
	DETAIL_LIST_ELEMENT	*detail_lr;
	unsigned char	SCR_MASK;

	if(SCR)
		SCR_MASK = 0xFF;
	else
		SCR_MASK = (IG_100|IG_A|IG_B);

	detail_lr = &inter_list[inter_head];
	detail_lr->User_Step_Num	= offset;
	detail_lr->Relay_Values		= relayv;
	detail_lr->Switch_Byte		= (tap_byte & SCR_MASK);
	detail_lr->Switch_Duration	= duration;
	detail_lr->time_not_cycles	= time_mode;
	inter_head++;
	inter_size++;
}

unsigned int	get_next(
int	offset)
{
	unsigned int	nextv;

	nextv = offset+1;
	if(nextv == list_size)
		return(offset);

	while(list_recs[nextv].action & 0x80) 
	{
		if(nextv == list_size)
			return(offset);
		switch(list_recs[nextv].action & 0x8F)
		{
			case DO_STOP:
				return(offset);
			case DO_GOSUBN:
			case DO_BRANCHN:
				if(list_recs[nextv].RMS_limit <= list_recs[nextv].PEAK_limit)
				{
					nextv++;
					break;
				}
			case DO_BRANCH:
				list_recs[nextv].PEAK_limit++;
			case DO_GOSUB:
				nextv = list_recs[nextv].duration;
				break;
			case DO_RETURN:
				nextv = return_stack[return_count-1]+1; 
				break;
		}
	}
	return(nextv);
}

void	raw_inter_insert(
unsigned int	offset)
{
	LIST_RECORD	*root_lr;
	LIST_RECORD	*next_lr;
	unsigned int	work_dur;
	unsigned char	scanv;
	int	temp;

	if(offset == -1)					/* End of Test */
		raw2_insert(offset,0,0,0);
	else
		if(global_mode == DO_FIXED_TAPS)			/* Global Mode = FIXED_TAPS	*/
		{
			root_lr	= &list_recs[offset];
			next_lr	= &list_recs[get_next(offset)];
			work_dur	= root_lr->duration;
			if((root_lr->action & 0x40) && (work_dur > 1))
			{
				raw2_insert(offset,(work_dur-1)*360,0,SCR_ON);
				work_dur = 1;
			}
			if((root_lr->action & 0x40) == 0)			/* Time Base Event		*/
			{
				if(work_dur < 5)
					work_dur = 20964 * work_dur / Last_Period;
				else
				{
					raw2_insert(offset,work_dur-3,1,SCR_ON);
					work_dur = 62892 / Last_Period;
				}
			}
			temp = next_lr->start_angle - root_lr->start_angle;
			if(temp < 0)
				temp += 360;
			work_dur = (work_dur * 360) + temp;
			if(work_dur > 720)
			{
				raw2_insert(offset,work_dur-360,0,SCR_ON);
				work_dur = 360;
			}
			raw2_insert(offset,work_dur,0,0);
		}													/* Global Mode = FIXED_TAPS	*/
		else
		{													/* Global mode = DO_LONG		*/
			unsigned char tapn1;
			unsigned char tapn2;
			unsigned int rt1;
			unsigned int rt2;
			unsigned int wd2;
			unsigned int last_relay;

			root_lr		= &list_recs[offset];
			next_lr		= &list_recs[get_next(offset)];
			work_dur		= root_lr->duration;
			last_relay	= inter_list[(inter_head-1) & 0xff].Relay_Values;
			if(((root_lr->action)& 15) == DO_100)
			{
				tapn1 = tapn2 = (SC_100 | IG_100 );
			}
			else
				if(((root_lr->action)& 15) == DO_OPEN)
				{
					tapn1 = tapn2 = 0;
				}
				else
					if((root_lr->action & 15) == Get_A_Tap(last_relay))
					{
						tapn1 = SC_A|IG_A;
						if((next_lr->action & 15) == DO_150)
						{
							if((root_lr->action & 15) == DO_150)
								tapn2 = tapn1;
							else
								tapn2 = IG_B|SC_B;
						}
						else
							tapn2 = tapn1;
					}
					else
					{
						tapn1 = SC_B|IG_B;
						if((next_lr->action & 15) == DO_120)
						{
							if((root_lr->action & 15) == DO_120)
								tapn2 = tapn1;
							else
								tapn2 = IG_A|SC_A;
						}
						else
							tapn2 = tapn1;
					}
			if(((next_lr->action & 15) == DO_100) || ((next_lr->action & 15) == DO_OPEN))
			{
				rt1 = rt2 = last_relay;
			}
			else
				if(tapn2 != tapn1)								/* Taps NOT SAME */
				{	/* Taps not same */
					if((next_lr->action & 15) == DO_120)	/* Next = 120 */
					{
						rt1  = Get_A_relay(root_lr->action & 15);
						rt1 |= Get_B_relay(root_lr->action & 15);
						rt2  = B_120 | Get_A_relay(root_lr->action & 15);
					}
					else												/* Next = 150 */
					{
						rt1  = Get_A_relay(root_lr->action & 15);
						rt1 |= Get_B_relay(root_lr->action & 15);
						rt2  = A_150 | Get_B_relay(root_lr->action & 15);
					}
				}
				else													/* Taps the SAME */
				{
					if(tapn1 & IG_A)
					{
						rt1  = last_relay & A_MASK;
						rt1 |= Get_B_relay(next_lr->action & 15);
						rt2  = rt1;
					}
					else
						if(tapn1 & IG_B)
						{
							rt1  = last_relay & B_MASK;
							rt1 |= Get_A_relay(next_lr->action & 15);
							rt2  = rt1;
						}
						else											/* OPEN or 100% */
						{
							rt1  = Get_A_relay(next_lr->action & 15);
							rt1 |= Get_B_relay(next_lr->action & 15);
							rt2  = rt1;
						}
				}

			wd2 = (work_dur / 2);
			work_dur -= wd2;
			if((root_lr->action & 0x40) == 0)				/* Time Based Event */
			{
				if(work_dur < 5)
					work_dur = 20964 * work_dur / Last_Period;
				else
				{
					raw3_insert(offset,work_dur-3,1,SCR_ON,tapn1,rt1);
					work_dur = 62892 / Last_Period;
				}
				raw3_insert(offset,work_dur*360,0,0,tapn1,rt1);
				if(wd2 < 5)
					wd2 = 20964 * wd2 / Last_Period;
				else
				{
					raw3_insert(offset,wd2-3,1,SCR_ON,tapn2,rt2);
					wd2 = 62892 / Last_Period;
				}
				temp = next_lr->start_angle-root_lr->start_angle;
				if(temp < 0)
					temp += 360;
				wd2 = (wd2 * 360) + temp;
				if(wd2 > 720)
				{
					raw3_insert(offset,wd2-360,0,SCR_ON,tapn2,rt2);
					wd2 = 360;
				}
				raw3_insert(offset,wd2,0,0,tapn2,rt2);
			}
			else													/* Degrees Based Event */
			{
				work_dur *= 360;
				if(work_dur > 720)
				{
					raw3_insert(offset,work_dur-360,0,SCR_ON,tapn1,rt1);
					raw3_insert(offset,360,0,0,tapn1,rt1);
				}
				else
					raw3_insert(offset,work_dur,0,0,tapn1,rt1);
				temp = next_lr->start_angle-root_lr->start_angle;
				if(temp < 0)
					temp += 360;
				wd2 = (wd2 * 360) + temp;
				if(wd2 > 720)
				{
					raw3_insert(offset,wd2-360,0,SCR_ON,tapn2,rt2);
					wd2 = 360;
				}
				raw3_insert(offset,wd2,0,0,tapn2,rt2);
			}
		}
}

int	add_inter()
{
	LIST_RECORD	*this_step;
	int	new_num = inter_list[(inter_head-1)& 0xff].User_Step_Num;

	if(list_recs[new_num].action & 0x80)
	{
		inter_head--;
		inter_size--;
	}
	else
		new_num++;

	do
	{ 
		if((new_num == list_size))
		{
			raw_inter_insert(-1); 
			return(END_LIST);
		}
		if((new_num > list_size) || (new_num < 0))
		{
			return BAD_TARGET;
		}
		this_step = &list_recs[new_num];
		switch(this_step->action & 0x8F)
		{
			case DO_STOP:
				raw_inter_insert(-1);
				return(END_LIST);
			case DO_BRANCHN:
				if(this_step->RMS_limit <= this_step->PEAK_limit)
				{
					new_num++;
					this_step->PEAK_limit = 0;
					break;
				}
				this_step->PEAK_limit++;
			case DO_BRANCH:
				new_num = this_step->duration;
				break;
			case DO_GOSUBN:
				if(this_step->RMS_limit <= this_step->PEAK_limit)
				{
					new_num++;
					this_step->PEAK_limit = 0;
					break;
				}
				this_step->PEAK_limit++;
			case DO_GOSUB:
				if(return_count > 31)
					return(STACK_OVER);
				return_stack[return_count++] = new_num;
				new_num = this_step->duration;
				break;
			case DO_RETURN:
				if(return_count == 0)
					return(BAD_RETURN);
				new_num = return_stack[--return_count] + 1; 
				break;
			case DO_SHORT:   
			case DO_OPEN:    
			case DO_40:      
			case DO_50:      
			case DO_70:      
			case DO_80:      
			case DO_90:      
			case DO_100:     
			case DO_110:     
			case DO_120:     
			case DO_150:
				raw_inter_insert(new_num);
				new_num++;
				break;
			default:
				return(BAD_ACTION);    
		}
	}
	while((this_step->action & 0x80) && (((inter_head+1)& 0xFF) != inter_tail));

	return(STEP_OK);
}

int	fill_inter(
unsigned short	first)
{
	int	rv = STEP_OK;

	fill_busy = 1;
	while((inter_size<200) && ((rv = add_inter()) == STEP_OK))
		;
	if(rv < 0)
	{
		global_error = rv;
		return(rv);
	}
	rv = STEP_OK;
	fill_busy = 0;
	return(rv);
}

void	Start_PQF(
uchar	event_ch)
{
	ushort	ut;
	ushort	i;
	uint		nrv;

	mode_flag = 0;
	PQF_event_ch = event_ch;
	PQF_help_ch = 9;
	for(i = 15; i > 9; i--)
		if(mode[i] == NONE)
		{
			PQF_help_ch = i;
			break;
		}
 	ut = 1 << event_ch;
	mode[PQF_help_ch] = ECAT_FREQ;
	EVENT_CISR_MASK = ~ut;

/*
 *	Here We must setup the latch and relay address
 *	WE must setup the Timer Sequencer here as well
 */
	if(default_level)
		ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
	else
		ECAT_TimerSequencer(PQF_drawer,000);

	/* Turn interrupt OFF! */
	TPU_rams[PQF_event_ch][0] = 0x113;
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);		/* High	*/
	TPU_delay(10);
	TPU_channel_init(PQF_event_ch,0x08, 2 , 0 , 3 , 0);		/* Low	*/
	TPU_delay(10);
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);		/* High	*/
	TPU_delay(10);

	nrv  = relay_state[PQF_drawer];
	nrv &= 0xFFFF00FF;
	nrv |= global_relay;
	relay_state[PQF_drawer] = nrv;

	ECAT_TimerSequencer(PQF_drawer,last_switch);
	mode[event_ch] = ECAT_FREQ;
	vectors[TPU_VECTOR_BASE+event_ch] = (long)PQF_event_Trap;

	ut = 1 << PQF_help_ch;
	HELP_CISR_MASK = ~ut;
	mode[PQF_help_ch] = ECAT_FREQ;
	vectors[TPU_VECTOR_BASE+PQF_help_ch] = (long)PQF_help_Trap;
/*
 *	Here we must setup the help_ch for:
 *		- High Priority
 *		- Interrrupt Enabled
 *		- Function = E
 *		- Matches and Pulses Scheduled
 *		- PAC for Toggle
 */
	TPU_rams[PQF_help_ch][0] = 0x8C;
	TPU_channel_init(PQF_help_ch,0x0E,0,0,2,1);

/*
 *	Here we must setup the event_ch for:
 *	Set up the register to something harmless 
 *		- Initially High Level
 *		- High Priority
 *		- Interrrupt Enabled
 *		- Function = E
 *		- Matches and Pulses Scheduled
 *		- PAC for Low Imediatly then High on Match
 */
	TPU_rams[PQF_event_ch][0] = 0x86;
	TPU_channel_init(PQF_event_ch,0x0E,0,0,2,1);

	HELP_HSR_MASK	= ~(3<<(2 * (PQF_help_ch % 8)));
	HELP_HSR_VAL	= 1 << (2 * (PQF_help_ch % 8));
	if(PQF_help_ch < 8)
		HELP_HSR_ADDR = &(TPU->HSSRR[1]);
	else
		HELP_HSR_ADDR = &(TPU->HSSRR[0]);

	EVENT_HSR_MASK	= ~(3 << (2 * (PQF_event_ch %8 )));
	EVENT_HSR_VAL	=   1 << (2 * (PQF_event_ch %8 ));
	if(PQF_event_ch < 8)
		EVENT_HSR_ADDR = &(TPU->HSSRR[1]);
	else
		EVENT_HSR_ADDR = &(TPU->HSSRR[0]);
}



/**************************************************************
				formerly NEW_PQF3.C
 **************************************************************/

/*
 *	dt, the time for the periodic interval
 *	as defined in the 68332 SIM handbook.
 *	p, a pointer to a function of type INTERRUPT!
 */

#define	MTIME_IRQ_LEVEL 	0x07		/* The IRQ level for periodic ints 		*/
#define	MTIME_VECTOR_BASE	0x40		/* The IRQ vector for periodic IRQ's	*/

void	PIT_init_long(
short	dt,
long		p)
{
	vectors[MTIME_VECTOR_BASE] = p;
	vectors[MTIME_VECTOR_BASE+256] = p;
	PICR = (MTIME_IRQ_LEVEL * 256) + (MTIME_VECTOR_BASE); 
	PITR = dt;
}

void	raw_adr(
int	i)
{
	int	*is;
	int	*id;
	int	AD_DRAWER;

	PIT_kill(0);
	AD_DRAWER = PQF_drawer;
	if(i == -1)
		return;

	AD_CH = MAX180_BIPP + i;
	ECAT_MAX180Write(AD_DRAWER,MAX180_BIPP+7);			/* Start Vin */
	ADADD = (ushort *)(ADC_CONTROL +(AD_DRAWER*0x1000));
	ADCMPL= (ushort *)(SERIAL_LATCH+(AD_DRAWER*0x1000));
	is = &ADRMS_trap;
	id = (int *)0x00C0100;
	while(is <= &ADRMS_END)
	{
		*id = *is;
		is++;
		id++;
	}
	slope_cnt	= 0;
	adtotal_cnt	= 0;
	LIMITHIT		= 0;
	PUB_RMS_SUM	= -1;
	PUB_RMS_N	= -1;
	RMS_ZERO		= 0;
	LIN_RMS_ZERO= 0;
	RMS_PEAKP	= -9999;
	RMS_PEAKM	= 9999;
	PIT_init_long(pitr_cnt,0xC0100);
}
/**************************************************************
				end of former NEW_PQF3.C
 **************************************************************/



int	raw_run(
int	startn)
{
	volatile int	i,ck;

	/* Delay for Soft Start relay? */
	while(!PQF_softstartflag)
		;

	fill_busy = 0;
	raw_relay(18,1);
	raw_relay(21,1);
	raw_relay(4,1);
	raw_relay(5,1);
	raw_relay(6,1);
	Active_Step = startn;
	LIMITHIT = 0;
	VAL_LIMITHIT = 0;
	STEP_LIMIT = 999;
	global_error = 0;
	if(default_level)
	{
		ECAT_TimerSequencer( PQF_drawer, ( SC_100 | IG_100 ) );
	}
	else
	{
		ECAT_TimerSequencer( PQF_drawer, 000 );
	}

	/* Turn interrupt off */
	TPU_rams[PQF_event_ch][0] = 0x113;
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);		/* High	*/
	TPU_delay(10);
	TPU_channel_init(PQF_event_ch,0x08, 2 , 0 , 3 , 0);		/* Low	*/
	TPU_delay(10);
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);		/* High	*/
	TPU_delay(210);
	list_recs[list_size].action = DO_STOP;
	raw_pre_run(startn);
	if(global_error)
	{
		return(global_error);
	}

	/* Zero out the counters for branch instructions */
	for(i = 0; i < list_size; i++)
		if((list_recs[i].action & 0x81) == 0x81)
			list_recs[i].PEAK_limit = 0;
	mode_flag = 0;
	return_count = 0;
	TPU_kill(PHASE_TPU);
	vectors[TPU_VECTOR_BASE+PHASE_TPU] = (long)TPU_trap;
	TPU_delay(200);
	TPU_kill(PHASE_TPU);
	Omni_Last_Zero_Time	= 0;
	Omni_Own_Clock			= 0;
	Omni_Public_Clock		= 0;
	Last_Period = 30000;
	if(!cpldata.eutrequest)
	{
		global_error = NO_EUTR;
		return(NO_EUTR);
	}

	if(!cpldata.eutblue)
	{
		global_error = NO_EUT;
		return(NO_EUT);
	}

	LIMITHIT	= 0;
	MAXPEAK	= 32000;
	MINPEAK	=-32000;
	Start_Zero(PHASE_TPU);
	TPU_delay(200);

	i = 0;
	TPU_delay(150);
	ck = Omni_Own_Clock;
	while(((ck == Omni_Own_Clock) || (Zero_Running != 1)) && (i < 10))
	{
		TPU_delay(10);
		i++;
	}
	if((Zero_Running != 1) || (ck == Omni_Own_Clock))
	{
		global_error = NO_AC;
		return(NO_AC);
	}

	inter_tail = 0;
	inter_head = 0;
	inter_size = 0;
	if(default_level)
		last_switch = IG_100;
	else
		last_switch = 0;
	if(list_recs[startn].start_angle > 90)
	{
		raw3_insert(startn,list_recs[startn].start_angle,0,0,last_switch,global_relay);
	}
	else
	{
		raw3_insert(startn,list_recs[startn].start_angle+360,0,last_switch,i,global_relay);
	}
	raw_inter_insert(startn);
	inter_list[(inter_head-1) & 0xff].Relay_Values = global_relay;
	i = fill_inter(1);
	if(i < 0)
	{
		global_error = i;
		return(i);
	}
	inter_list[255].Relay_Values = inter_list[0].Relay_Values;

	Start_PQF(SEQ_TPU);
	i = zero_cnt;
	TPU_delay(30);
	mode_flag	= 0;
	PQF_stop		= 1;
	Time_Margin	= 0;
	Next_Event	= 100;
	mode_flag	= mode_TIME | mode_WAIT;
	PQF_stop		= 0;
	MAXRMS		= list_recs[startn].calc_RMS;
	NEXTMAXRMS	= list_recs[startn].calc_RMS;
	MAXPEAK		= list_recs[startn].calc_PEAK;
	MINPEAK		= -MAXPEAK;
	raw_adr(3);
	KEY_led(GO_LED,1);
	KEY_led(STOP_LED,0);
	LIMITHIT		= 0;
	PQF_running	= TRUE;
	return(0);
}

int	PQF_run_list(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		if(!list_size)
			return;
		PQF_running = TRUE;
		LIMITHIT	= 0;
		MAXPEAK	= 30000;
		MINPEAK	= -MAXPEAK;
		last_zero= 0;
		raw_run(EXTRACT_INT(0));
		if(global_error)
		{
			PQF_running = FALSE;
			KEY_led(GO_LED,0);
			KEY_led(STOP_LED,1);
		}
		printf("%d",global_error);
		LIMITHIT = 0;
   }
	return(0);
}

void raw_stop()
{
	PQF_stop		= 1;
	PQF_running	= FALSE;
	inter_tail	= 0;
	inter_head	= 0;
	inter_size	= 0;

	TPU_kill(PQF_event_ch);
	TPU_kill(PQF_help_ch);
	vectors[TPU_VECTOR_BASE+PQF_event_ch]	= (long)TPU_trap;
	vectors[TPU_VECTOR_BASE+PQF_help_ch]	= (long)TPU_trap;

	ECAT_TimerSequencer(PQF_drawer,last_switch & (IG_100|IG_A|IG_B));
	/* Turn interrupt off */
	TPU_rams[PQF_event_ch][0] = 0x113;
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);			/* High	*/
	TPU_channel_init(PQF_event_ch,0x08, 2 , 0 , 3 , 0);			/* Low	*/
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);			/* High	*/
	TPU_delay(30);

	if(default_level)
		ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
	else
		ECAT_TimerSequencer(PQF_drawer,000);

	/* Turn interrupt off */
	TPU_rams[PQF_event_ch][0] = 0x113;
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);			/* High	*/
	TPU_channel_init(PQF_event_ch,0x08, 2 , 0 , 3 , 0);			/* Low	*/
	TPU_channel_init(PQF_event_ch,0x08, 1 , 0 , 3 , 0);			/* High	*/
	TPU_delay(200);

	TPU_kill(PQF_event_ch);
	TPU_kill(PQF_help_ch);

	KEY_led(GO_LED,0);
	KEY_led(STOP_LED,1);
	raw_adr(-1);
	if(default_level)
		last_switch = ( SC_100 | IG_100 );
	else
		last_switch = 0;
}

int   PQF_stop_list(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		raw_stop();
	}
	return(0);
}

void	PQF_init(
uint	drawer)
{
	unsigned int	nrv;
	float				f;

	inter_tail	 = 0;
	inter_head	 = 0;
	inter_size	 = 0;
	list_size	 = 0;
	PQF_stop		 = 1;
	adtotal_cnt	 = 0;
	Zero_Running = 0;
	default_level= 0;
	PQF_drawer	 = drawer;
	pqfdata.ep3phase = 0;

	/* Disable all measurement capable modules. */
	PEAK_MeasureDisable();

	if(pqfdata.ep3flag && (pqfdata.ep3box != GHOST_BOX))
	{
		default_level = 100;
		*pqfdata.ep3relay |= (LED_ACTIVE | RLY_RD22);
		ECAT_WriteRelays(pqfdata.ep3box,*pqfdata.ep3relay);
		TPU_delay(100);
		*pqfdata.ep3relay |= (RLY_RD19);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		TPU_delay(100);
		raw_relay(21,0);
		TPU_delay(100);
	}

	nrv = relay_state[PQF_drawer];
	ECAT_TimerSelReg(PQF_drawer,0x0003);
	last_relay  = (A_110 | B_110);
	last_switch = 0;
	if((cpldata.eutblue == ON) || (pqfdata.ep3flag))
	{
		default_level = 100;
		last_switch = ( SC_100 | IG_100);
		ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
	}
	else
		ECAT_TimerSequencer(PQF_drawer,000);

	nrv &= 0xFFFF00FF;
	nrv |= (last_relay & 0xFF00);
	ECAT_WriteRelay0(PQF_drawer,nrv);
	relay_state[PQF_drawer] = nrv;
	TPU_write(SEQ_TPU,1);				/* Set it High	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);				/* Set it Low	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);				/* Set it High	*/
	raw_relay(17,1);
	raw_relay(18,1);
	raw_relay(21,1);
	raw_relay(4,1);
	raw_relay(5,1);
	raw_relay(6,1);
	if(pqfdata.ep3flag)
	{
		TPU_delay(1000);
		f = TPU_ECAT_freq(PHASE_TPU);
		if((f > 65.0) || (f < 42.0))
			return;
		*pqfdata.ep3relay |= (RLY_RD23);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		TPU_delay(100);
		*pqfdata.ep3relay &= ~(RLY_RD22);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
}

/*
 *	This routine clocks the next TPU Event out at time TV
 *	If TV > 0x10000 we need to schedule intermediate events
 */
void	clock_pqf(
uint	RV,
uint	offset)
{
	ushort	nextt;

	TPU_rams[15][6] = RV;				/* This should put the value in $YFFFFC */
	if(offset < 32000)
	{
		TPU_rams[PQF_event_ch][1] = offset;
		TPU_rams[PQF_event_ch][2] = 0x00FC;				/* The reference address */
		*EVENT_HSR_ADDR &= EVENT_HSR_MASK;
		*EVENT_HSR_ADDR |= EVENT_HSR_VAL;
	}
	else
	{
		if(offset > 65000)
			nextt = 32000;
		else
			nextt = offset / 2;
		param1[PQF_help_ch] = offset - nextt;
		TPU_rams[PQF_help_ch][1] = nextt;
		TPU_rams[PQF_help_ch][2] = 0x00FC;				/* The reference address */
		*HELP_HSR_ADDR &= HELP_HSR_MASK;
		*HELP_HSR_ADDR |= HELP_HSR_VAL;
	}
}

/*This takes the time in value and fixes */
void	Fix_Edge(
ushort	value)
{
	uint	tempt;
	uint	tempd;
	uint	angle_adj;

	if(value <= Omni_Last_Zero_Time)
		tempt = (uint)value + 0x10000;
	else
		tempt = value;
	tempd = Omni_Own_Clock+(((tempt-Omni_Last_Zero_Time)*360)/Last_Period);
	if(tempd < Omni_Public_Clock)
	{
		angle_adj = 675;
		tempd += angle_adj;
	}
	while(tempd < Omni_Public_Clock)
		tempd += angle_adj;
	Omni_Public_Clock	= tempd;
	Omni_Public_Edge	= value;
}

interrupt void	PQF_help_Trap(void)
{
	ushort	nextt;

	Int_Hit = 1;
	if(param1[PQF_help_ch] < 32000)
	{
		TPU_rams[PQF_event_ch][1] = param1[PQF_help_ch];
		TPU_rams[PQF_event_ch][2] = (PQF_help_ch*16+10) & 0x0FF;
		*EVENT_HSR_ADDR &= EVENT_HSR_MASK;
		*EVENT_HSR_ADDR |= EVENT_HSR_VAL;
	}
	else
	{
		if(param1[PQF_help_ch] > 65000)
			nextt = 32000;
		else
			nextt = param1[PQF_help_ch] / 2;
		param1[PQF_help_ch] -= nextt;
		TPU_rams[PQF_help_ch][1] = nextt;
		TPU_rams[PQF_help_ch][2] = (PQF_help_ch*16+10) & 0x0FF;
		*HELP_HSR_ADDR &= HELP_HSR_MASK;
		*HELP_HSR_ADDR |= HELP_HSR_VAL;
	}
	TPUCISR &= HELP_CISR_MASK;
}

interrupt void	PQF_event_Trap(void)
{
	DETAIL_LIST_ELEMENT	*nle;
	uint	nrv	= relay_state[PQF_drawer];
	uint	delay	= inter_list[inter_tail].Switch_Duration;
	uchar	time	= inter_list[inter_tail].time_not_cycles;
	uint	trip_value;

	Int_Hit = 1;
	trip_value = (uint)TPU_rams[PQF_event_ch][5];
	TPUCISR &= EVENT_CISR_MASK;
	inter_tail++;
	inter_size--;
	nle = &inter_list[inter_tail];
	if(Active_Step >= 0)
	{
		NEXTMAXRMS = list_recs[Active_Step].calc_RMS;
		MAXPEAK = list_recs[Active_Step].calc_PEAK;
		MINPEAK =- MAXPEAK;
	}
 
	Active_Step = nle->User_Step_Num;
	nrv &= 0xFFFF00FF;
	nrv |= ((inter_list[((inter_tail-1)& 0xff)].Relay_Values) & 0xFF00);
 
	if((nle->User_Step_Num == -1) || (PQF_stop == 1))
	{
		mode_flag	= 0;
		Next_Event	= 0;
		Active_Step	= -1;
		PQF_stop		= 1;
		last_relay	= inter_list[(inter_tail -1) & 0xff].Relay_Values;
	}
	else
	{
		ECAT_WriteRelay0(PQF_drawer,nrv);
		relay_state[PQF_drawer] = nrv;
		last_switch = nle->Switch_Byte;
		ECAT_TimerSequencer(PQF_drawer,nle->Switch_Byte);
		if(time)
		{
			mode_flag	= 3;									/* mode_WAIT | mode_TIME */
			Next_Event	= delay;
			Time_Margin	= 0;
		}
		else
		{
			Next_Event += delay;
			mode_flag   = 0;
			if(Next_Event < Omni_Public_Clock)
			{
				clock_pqf(trip_value,200);
			}
			else
				if(Next_Event <= (Omni_Public_Clock + CLOCK_SAFETY))
					clock_pqf(Omni_Public_Edge,((Next_Event-Omni_Public_Clock)*Last_Period/360));
				else
				{
					mode_flag = mode_WAIT;
				}  
		}
		while((Omni_Own_Clock > 1000000) && (Next_Event > 1000000))
		{
			Next_Event			-= 1000000; 
			Omni_Own_Clock		-= 1000000;
			Omni_Public_Clock	-= 1000000;
		}
	}

	if((inter_size < 100) && (!fill_busy))
	{
		asm(" move.w #$2500,SR");							/* Enable TPU interrupts */
		fill_inter(0);
	}
}

interrupt void	PQF_zero_Trap(void)
{
	uint	temp;

	Int_Hit = 1;
	zero_cnt++;
	if(Zero_Running)
	{
		PQFFREQ[lhappened] = *ramch & 0xFFFF;
		if(lhappened == 1)
		{							/* Falling Edge */
			TPU_rams[PQF_sync_ch][0] = 0x0B;					/* Detect Rising edge */
			if(PQFFREQ[0] > PQFFREQ[1])
				PQFFREQ[1] += 0x10000;
			Last_Period = Omni_Last_Zero_Time;
			Omni_Last_Zero_Time = ((PQFFREQ[1] + PQFFREQ[0]) / 2) & 0xFFFF;
			if(Last_Period > Omni_Last_Zero_Time)
				Last_Period = 0x10000 + Omni_Last_Zero_Time - Last_Period;
			else
				Last_Period = Omni_Last_Zero_Time - Last_Period;
			if(Last_Period > maxdt)
				maxdt = Last_Period;
			if(Last_Period < mindt)
				mindt = Last_Period;
			Omni_Public_Clock = Omni_Own_Clock += 360;
			Omni_Public_Edge = Omni_Last_Zero_Time;
			Fix_Edge(PQFFREQ[1] & 0xFFFF);
			Zero_Running = 1;
			lhappened = 0;
			if((mode_flag & mode_WAIT) && (PQF_stop == 0))
			{
				if(mode_flag & mode_TIME)
				{
					temp = Last_Period + Time_Margin;
					Time_Margin = (temp % T10MSEC);
					Next_Event -= (temp/ T10MSEC);
					if(Next_Event < 0)
						Next_Event = 0;
					if(Next_Event <= (Last_Period / T10MSEC))
					{
						mode_flag &= ~mode_WAIT;
						Next_Event = 180;
						Omni_Own_Clock = 0;
						Omni_Public_Clock = 0;
						Omni_Public_Edge = Omni_Last_Zero_Time;
						Fix_Edge(PQFFREQ[1] & 0xFFFF);
						clock_pqf(Omni_Last_Zero_Time,Last_Period/2);
					}
				}
				else
				{
					if((Omni_Public_Clock+CLOCK_SAFETY) > Next_Event)
					{
						mode_flag &= ~mode_WAIT;
						clock_pqf(Omni_Public_Edge,((Next_Event-Omni_Public_Clock)*Last_Period/360));
					}
				}
			}
		}
		else
		{
			TPU_rams[PQF_sync_ch][0] = 0x07;					/* Detect Rising edge */
			if(Zero_Running == 1)
				Fix_Edge(PQFFREQ[0] & 0xFFFF);
			lhappened++;
		}
	}
	TPUCISR &= ZERO_CISR_MASK;
	*ZERO_HSR_ADDR &= ZERO_HSR_MASK;
	*ZERO_HSR_ADDR |= ZERO_HSR_VAL;
}

void	Start_Zero(
char	sync_ch)
{
	int		helper_ch;
	int		i;
  ushort	ut;
	volatile float	x;

	zero_cnt			= 0;
	PQF_sync_ch		= sync_ch;
	mode_flag		= 0;
	lhappened		= 0;
	Zero_Running	= 0;

	vectors[TPU_VECTOR_BASE+sync_ch] = (long)TPU_trap;
	TPU_kill(sync_ch);
	TPU_delay(100);

	ECAT_TimerSelReg(PQF_drawer,0x0003);
	x = 3.141;
	TPU_channel_init(sync_ch,0, 0 , 0 , 0 , 0);
	while(TPU_read(sync_ch) && (i < 100))
	{
		i++;
		x = log(x);
	}
	if(i >= 100)
		return;

	TPU_channel_init(sync_ch,0, 0 , 0 , 0 , 0);
	ut = 1 << sync_ch;
	ZERO_CISR_MASK	= ~ut;
	mode[sync_ch]	= OUT;										/* ECAT_FREQ */
	ZERO_HSR_MASK	= ~(3 << (2 * (PQF_sync_ch % 8)));
	ZERO_HSR_VAL	= 1 << (2 * (PQF_sync_ch % 8));
	if(PQF_sync_ch < 8)
		ZERO_HSR_ADDR = &(TPU->HSSRR[1]);
	else
		ZERO_HSR_ADDR = &(TPU->HSSRR[0]);

 /* Now to set up the input channel to be input capture on both edges */
	ramch = &TPU_rams[sync_ch][4];
	vectors[TPU_VECTOR_BASE+sync_ch] = (long)PQF_zero_Trap;
	TPU_rams[sync_ch][0] = 0x0B;						/* Detect rising edge	*/
	TPU_rams[sync_ch][1] = 0x0E;
	TPU_rams[sync_ch][2] = 0x00;						/* Count 1 Transition	*/
	Zero_Running = 2;
	TPU_channel_init(sync_ch,0x0A, 1 , 1 , 3 , 1);
}

void PQF_exit()
{
	TPU_kill(PQF_sync_ch);
	TPU_kill(PQF_event_ch);
	TPU_kill(PQF_help_ch);
	vectors[TPU_VECTOR_BASE+PQF_sync_ch]  = (long)TPU_trap;
	vectors[TPU_VECTOR_BASE+PQF_event_ch] = (long)TPU_trap;
	vectors[TPU_VECTOR_BASE+PQF_help_ch]  = (long)TPU_trap;
	ECAT_TimerSelReg(PQF_drawer,0x0000);
	raw_relay(4,0);
	raw_relay(5,0);
	raw_relay(6,0);
	raw_relay(7,0);
	/* TEST for EUT saving when exiting PQFland */
	raw_relay(29,1);
	TPU_delay(100);
	raw_relay(26,0);
	/* END Test */
	TIO_TISelect(PQF_drawer,TI3_SELECT,DISABLE);
	if(pqfdata.ep3flag && (pqfdata.ep3box < MAX_MODULES))
	{
		*pqfdata.ep3relay |= RLY_RD22;
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		TPU_delay(100);
		*pqfdata.ep3relay &= ~(RLY_RD19 | RLY_RD20 | RLY_RD21 | RLY_RD23 | RLY_RD24 | RLY_RD25);
		ECAT_WriteRelays(pqfdata.ep3box,*pqfdata.ep3relay);
		TPU_delay(100);
		*pqfdata.ep3relay &= ~(RLY_RD22);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
}

int	PQF_exit_pgm(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		PQF_exit();
		ECAT_ModeChange(GHOST_MODE);
		keyenable = 1;
	}
	return(0);
}

void	PQF_run()
{
	return;
}

void	raw_add_goto(
int	listn,
int	offset,
P_PARAMT	*params)
{
	do_remote();
	list_recs[listn].action			= 128;
	list_recs[listn].duration		= EXTRACT_INT(offset);
	list_recs[listn].start_angle	= 0;
	list_recs[listn].RMS_limit		= 0;
	list_recs[listn].PEAK_limit	= 0;
}

void	raw_add_tap(
int	listn,
int	orv,
int	offset,
P_PARAMT *params)
{
	uint	actionv;

	do_remote();
	actionv = 64 | DO_OPEN;
	switch(EXTRACT_INT(offset+0))
	{
		case -1:
			actionv = orv | DO_SHORT;
			break;
		case 0:
			actionv = orv | DO_OPEN;
			break;
		case 40:
			actionv = orv | DO_40;
			break;
		case 50:
			actionv = orv | DO_50;
			break;
		case 70:
			actionv = orv | DO_70;
			break;
		case 80:
			actionv = orv | DO_80;
			break;
		case 90:
			actionv = orv | DO_90;
			break;
		case 100:
			actionv = orv | DO_100;
			break;
		case 110:
			actionv = orv | DO_110;
			break;
		case 120:
			actionv = orv | DO_120;
			break;
		case 150:
			actionv = orv | DO_150;
			break;
	}
	list_recs[listn].action			= actionv;
	list_recs[listn].duration		= EXTRACT_INT(offset+1);
	list_recs[listn].start_angle	= EXTRACT_INT(offset+2);
	list_recs[listn].RMS_limit		= EXTRACT_INT(offset+3);
	list_recs[listn].PEAK_limit	= EXTRACT_INT(offset+4);
}

int	PQF_setc_tap(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		list_size++;
		if(list_size >= MAXIMUM_LIST)
		{
			printf("List is full \n");
			return(BADNUM);
		}
		else
			raw_add_tap(list_size-1,64,0,params);
   }
	return(0);
}

int	PQF_setnc_tap(P_PARAMT *params)
{
	int	listn;

	if(pqfdata.pqfflag)
	{
		do_remote();
		listn = EXTRACT_INT(0);
		if(list_size < listn)
		{
			printf("N is larger than the list \n");
			return(BADNUM);
		}
		else
			raw_add_tap(listn,64,1,params);
   }
	return(0);
}

int	PQF_sets_tap(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		list_size++;
		if(list_size >= MAXIMUM_LIST)
		{
			printf("List is full \n");
			return(BADNUM);
		}
		else
			raw_add_tap(list_size-1,0,0,params);
   }
	return(0);
}

int	PQF_setns_tap(P_PARAMT *params)
{
	int	listn;

	if(pqfdata.pqfflag)
	{
		do_remote();
		listn = EXTRACT_INT(0);
		if(list_size < listn)
		{
			printf("N is larger than the list \n");
			return(BADNUM);
		}
		else
			raw_add_tap(listn,0,1,params);
   }
	return(0);
}

int	PQF_set_goto(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		do_remote();
		list_size++;
		if(list_size >= MAXIMUM_LIST)
		{
			printf("List is full \n");
			return(BADNUM);
		}
		else
			raw_add_goto(list_size-1,0,params);
   }
	return(0);
}

int	PQF_setn_goto(P_PARAMT *params)
{
	int	listn;

	if(pqfdata.pqfflag)
	{
		do_remote();
		listn = EXTRACT_INT(0);
		if(list_size < listn)
		{
			printf("N is larger than the list \n");
			return(BADNUM);
		}
		else
			raw_add_goto(listn,1,params);
   }
	return(0);
}

int	long_step(
LIST_RECORD	lr)
{
	if(((lr.action & 64) && (lr.duration >= 6)) || (lr.duration >= 10))
		return(1);
	return(0);
}

void	raw_pre_run(
int	start)
{
	int	i,j;
	int	done;
	uchar	tapa;
	uchar	tapb;
 	uchar	too_many;
 	uchar	too_short;

	/* Zero out the loop counters */
	return_count = 0;
	for(i = 0; i < list_size; i++)
	{
		if(list_recs[i].action & 0x80)
			list_recs[i].PEAK_limit = 0;
	}
	tapa = 0xff;
	tapb = 0xff;
	too_many  = 0;
	too_short = 0;
	global_error = 0;
	i = start;
	if((i < list_size) && (list_recs[i].action & 0x80))
		i = get_next(i);

	global_relay = (Get_A_relay(list_recs[i].action & 15) | Get_B_relay(list_recs[i].action & 15));
	done = (list_size == 0);
	while(!done)
	{
		if((i >= list_size) || (list_recs[i].action == DO_STOP))
			done = TRUE;
		else
		{ /* Real step */
         /* If either calc_PEAK or calc_RMS is 0, then override its respective
               limit by setting it to an unattainably high value. */
			if(list_recs[i].PEAK_limit)
				list_recs[i].calc_PEAK = (int)list_recs[i].PEAK_limit * 16.384;
			else
				list_recs[i].calc_PEAK = 0x7000000;

         if ( list_recs[i].RMS_limit )
         {
            list_recs[i].calc_RMS  = (int)list_recs[i].RMS_limit * 1.6384;
            list_recs[i].calc_RMS *= list_recs[i].calc_RMS;
         }
         else
            list_recs[i].calc_RMS = 0x7000000;

			switch(list_recs[i].action & 15)
			{
				case DO_100:
				case DO_OPEN:
					break;
				case DO_SHORT:
				case DO_40:
				case DO_50:
				case DO_70:
				case DO_80:
				case DO_90:
				case DO_110:
					if(tapa == (list_recs[i].action & 15))
						break;
					if(tapb == (list_recs[i].action & 15))
						break;
					if(tapa == 0xff)
					{
						tapa = (list_recs[i].action & 15);
						break;
					}
					if(tapb == 0xff)
					{
						tapb = (list_recs[i].action & 15);
						break;
					}
					too_many = TRUE;
					break;
				case DO_120:
					if(tapb == DO_120)
						break;
					if(tapb == 0xFF)
					{
						tapb = DO_120;
						break;
					}
					if(tapa == 0xFF)
					{
						tapa = tapb;
						tapb = DO_120;
						break;
					}
					too_many = TRUE;
					break;
				case DO_150:
					if(tapa == DO_150)
						break;
					if(tapa == 0xFF)
					{
						tapa = DO_150;
						break;
					}
					if(tapb == 0xFF)
					{
						tapb = tapa;
						tapa = DO_150;
						break;
					}
					too_many = TRUE;
					break;
			}
			if(!long_step(list_recs[i]))
				too_short = TRUE;
		}
		if((list_recs[i+1].action == DO_BRANCH) && (list_recs[i+1].PEAK_limit >= 1))
			done = TRUE;
		j = get_next(i);
		if(j == i)
			done = TRUE;
		else
			i = j;
	}
	if(too_short && too_many)
	{
		printf("TOO short of step, or TOO many taps\n\r");
		global_mode = CANT_DO;
		global_error= TOOMANYTAPS;
		return;
	}

	switch(tapa)
	{
		case DO_SHORT:
			global_relay = A_ZERO;
			break;
		case DO_40:
			global_relay = A_40;
			break;
		case DO_50:
			global_relay = A_50;
			break;
		case DO_70:
			global_relay = A_70;
			break;
		case DO_80:
			global_relay = A_80;
			break;
		case DO_90:
			global_relay = A_90;
			break;
		case DO_110:
			global_relay = A_110;
			break;
		case DO_150:
			global_relay = A_150;
			break;
		default:
			global_relay = 0;
			break;
	}
	switch(tapb)
	{
		case DO_SHORT:
			global_relay |= B_ZERO;
			break;
		case DO_40:
			global_relay |= B_40;
			break;
		case DO_50:
			global_relay |= B_50;
			break;
		case DO_70:
			global_relay |= B_70;
			break;
		case DO_80:
			global_relay |= B_80;
			break;
		case DO_90:
			global_relay |= B_90;
			break;
		case DO_110:
			global_relay |= B_110;
			break;
		case DO_120:
			global_relay |= B_120;
			break;
	}

	if(too_short)
		global_mode = DO_FIXED_TAPS;
	else
	{
		global_mode = DO_LONG;
		return;
	}
	return_count = 0;

	for(i = 0; i < list_size; i++)
		if(list_recs[i].action & 0x80)
			list_recs[i].PEAK_limit = 0;
	i = start;
	if((i < list_size) && (list_recs[i].action & 0x80))
		i = get_next(i);
	done = (list_size == 0);

	while(!done)
	{
		if((list_recs[i].action & 15) == tapa)
			list_recs[i].Switch_Values = IG_A | SC_A;
		else
			if((list_recs[i].action & 15) == tapb)
				list_recs[i].Switch_Values = IG_B | SC_B;
			else
				if((list_recs[i].action & 15) == DO_100)
					list_recs[i].Switch_Values = IG_100 | SC_100;
				else
					list_recs[i].Switch_Values = 0;
		if((list_recs[i+1].action == DO_BRANCH) && (list_recs[i+1].PEAK_limit >= 1))
			done = TRUE;
		j = get_next(i);
		if(j == i)
			done = TRUE;
		else
			i = j;
	}
}

int PQF_error_rpt(P_PARAMT*params)
{
	int	ge;

	if(pqfdata.pqfflag)
	{
		ge = EXTRACT_INT(0);
		if(ge == global_error)
			global_error = 0;
		do_remote();

		if(ge >= 0)
			err_msg = "OK";
		else
			switch(ge)
			{
				case BAD_TARGET:
					err_msg = "Bad Goto Address";
					break;
				case BAD_RETURN:
					err_msg = "Bad Return Address";
					break;
				case STACK_OVER:
					err_msg = "Return Stack Overflow";
					break;
				case SETUP_BAD:
					err_msg = "List Setup Violation";
					break;
				case TOOMANYTAPS:
					err_msg = "List Has More Than 3 Taps";
					break;
				case BAD_ACTION:
					err_msg = "Internal ERROR";
					break;
				case NO_AC:
					err_msg = "No AC Present";
					break;
				case NO_EUT:
					err_msg = "EUT power is not enabled";
					break;
				case NO_EUTR:
					err_msg = "EUT power is not requested";
					break;
				case OVER_RMS:
				case OVER_PEAK:
				case OVER_TEMP:
				case SNUB_BUS:
				case IGBT_LIMIT:
					break;
				default:
					err_msg = "Unknown Error";
			}
		printf("%s",err_msg);
	}
	return(0);
}

void	raw_default(
int	i)
{
	default_level = i;
	if(PQF_stop)
	{
		if(default_level)
			ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
		else
			ECAT_TimerSequencer(PQF_drawer,000);
		TPU_write(SEQ_TPU,1);		/* Set it High	*/
		TPU_delay(10);
		TPU_write(SEQ_TPU,0);		/* Set it Low	*/
		TPU_delay(10);
		TPU_write(SEQ_TPU,1);		/* Set it High	*/
	}
}


int	PQF_status_rep(P_PARAMT *params)
{
	int	EUTV;
	uchar	statflag;

	if(pqfdata.pqfflag)
	{
		if(cpldata.eutrequest && cpldata.eutblue)
			EUTV = 2;
		else
			EUTV = cpldata.eutrequest;

		statflag = ECAT_StatusFlag();
		if(PQF_running)
		{
			if((Last_Period > 46603) || (Last_Period < 32263))
				printf("%d 0 0 %d %d %d 999 %d 1234",statflag,Active_Step,EUTV,global_error,PQF_actual_V);
			else
				printf("%d %d %d %d %d %d %d %d 1234",statflag,(int)(PQF_amps_rms*10.0),PQF_amps_peak,Active_Step,EUTV,global_error,
							20971520/Last_Period,PQF_actual_V);
		}
		else
			printf("%d 0 0 -1 %d %d 0 0 1234",statflag,EUTV,global_error);
   }
	return(0);
}

int	PQF_idle_pgm(P_PARAMT *params)
{
	if(pqfdata.pqfflag)
	{
		if((EXTRACT_INT(0) == 0) && (!pqfdata.ep3flag))
			raw_default(0);
		else
			if(EXTRACT_INT(0) == 100)
				raw_default(100);
			else
				return(BAD_VALUE);
   }
	return(0);
}

int	PQF_nominal_pgm(P_PARAMT *params)
{
	int	newv;

	if(pqfdata.pqfflag)
	{
		newv = EXTRACT_INT(0);
		if((newv >= 100) && (newv <= 240))
			PQF_nominal_V = newv;
		else
			return(BAD_VALUE);
   }
	return(0);
}

int raw_qualify(void)
{
	int	value1;
	int	cntr;
	int	iv;
	float	f;

	if(!PQF_stop)
		return(BAD_VALUE);

	if(peakdata.peakaddr < MAX_MODULES)
		ECAT_WriteRelay0(peakdata.peakaddr,0x0000);

	if(pqfdata.ep3flag)
	{
		*pqfdata.ep3relay &= ~(RLY_RD18);		/* Shut OFF Output Contactor */
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		TPU_delay(100);
	}
	ECAT_TimerSequencer(PQF_drawer,000);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low */
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(100);
	raw_relay(1,1);
	raw_relay(2,1);									/* Select Qualifer		*/
	raw_relay(4,1);									/* Monitors on Coax V	*/
	raw_relay(5,1);									/* Coax Voltage select	*/
	raw_relay(6,1);									/* Coax Current Select	*/
	raw_relay(3,1);									/* Line Qualifer Discharge	*/
	TPU_delay(5000);
	raw_relay(3,0);
	PEAK_RELAYS = 0x7A00;
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low	*/
	ECAT_TimerSequencer(PQF_drawer,SC_100);
	TPU_kill(PHASE_TPU);
	vectors[TPU_VECTOR_BASE+PHASE_TPU] = (long)TPU_trap;
	TPU_delay(100);
	f = TPU_ECAT_freq(PHASE_TPU);
	if((f > 65.0) || (f < 42.0))
	{
		if(peakdata.peakaddr < MAX_MODULES)
			ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
		return(-255);
	}
	TPU_kill(PHASE_TPU);
	TPU_delay(1);
	TPU_ECAT_phase(PHASE_TPU,SEQ_TPU,90,0,1,0x00);
	while(!TPU_ECAT_phase_done())
		;
	TPU_delay(1);
	AD_HIT = cntr = 0;
	while((AD_HIT == 0) && (cntr < 10000))
		cntr++;											/* Start measure cycle	*/
	value1 = DAC_read(5);
	if(value1 & 0x800)
		value1 |= 0xFFFFF000;
	value1 = abs(PEAK_IADJ*value1/4096);
	iv = value1;
	TPU_delay(100);
	PEAK_RELAYS = 0x6200;							/* DUMP the Peak CAPS	*/
	TPU_delay(10);
	ECAT_TimerSequencer(PQF_drawer,000);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low */
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(100);	/* NEW */
	raw_relay(3,1);									/* Line Qualifer Discharge */
	TPU_delay(4000);
	PEAK_RELAYS = 0x7A00;
	TPU_delay(1000);
	raw_relay(3,0);
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low */
	ECAT_TimerSequencer(PQF_drawer,SC_100);
	f = TPU_ECAT_freq(PHASE_TPU);
	if((f > 65.0) || (f < 42.0))
	{
		if(peakdata.peakaddr < MAX_MODULES)
			ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
		return(-255);
	}
	TPU_kill(PHASE_TPU);
	TPU_delay(1);
	TPU_ECAT_phase(PHASE_TPU,SEQ_TPU,270,0,1,0x00);

	while(!TPU_ECAT_phase_done())
		;
	PEAK_RELAYS = 0xFA00;							/* Enable NEGATIVE	*/
	TPU_delay(10);
	AD_HIT = cntr = 0;
	while((AD_HIT == 0) && (cntr < 10000))
		cntr++;											/* Start measure cycle	*/
	value1 = DAC_read(5);
	if(value1 & 0x800)
		value1 |= 0xFFFFF000;
	value1 = abs(PEAK_IADJ*value1/4096);

	iv = iv | ((value1 << 16) & 0xFFFF0000);
	TPU_delay(100);
	ECAT_TimerSequencer(PQF_drawer,000);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low */
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(100); /* NEW */
	raw_relay(2,0);
	TPU_delay(100); /* NEW */

	if(default_level)
		ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
	else
		ECAT_TimerSequencer(PQF_drawer,000);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);							/* Set it Low */
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);							/* Set it High */
	PEAK_RELAYS = 0x6200;							/* DUMP the Peak CAPS	*/

	if(pqfdata.ep3flag)
	{
		TPU_delay(100);
		*pqfdata.ep3relay |= RLY_RD18;			/* Turn ON Output Contactor */
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
	if(peakdata.peakaddr < MAX_MODULES)
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
	return(iv);
}

int	PQF_qualify_cmd(P_PARAMT *params)
{
	int	i;

	if(pqfdata.pqfflag)
	{
		i = raw_qualify();
		if(i > 0)
		{
			printf("%d %d",((i&0xffff)),((((i>>16) & 0xffff))));
			return(0);
		}
		else
			if((i == -255) || (i < 0))
			{
				paraml[0].idata = 0;
				root_EUT_pgm(paraml);
				printf("-1 -1");
			}
   }
	return(0);
}
