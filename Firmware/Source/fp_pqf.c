#include <shortdef.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "parser.h"
#include "front.h"
#include "switches.h"
#include "intface.h"
#include "timer.h"
#include "qsm.h"
#include "vectors.h"
#include "pqf.h"
#include "ecat.h"
#include "key.h"

/*
 *	Function Prototypes
 */
int		raw_run(int);
int		raw_qualify(void);
void		raw_load(void);
void		raw_save(void);
void		raw_stop(void);
void		raw_default(int);
void		do_remote();
void		display_list(void);
uchar	demo_funcs(char);
extern void	raw_relay(uint,uint);
extern uchar	(*func_manager)(char);
extern int		root_EUT_pgm(P_PARAMT *);

int EP3_SwitchPhase(uchar	phase);

/*
 *	Variable Declarations
 */
int		new_eut;
int		last_zero;
int		PQF_running;
int		PQF_amps_peak;
int		PQF_step_peak;
int		PQF_nominal_V;
int		PQF_actual_V;
char		*err_msg;
char		limit_buff[80];
float	PQF_amps_rms;

static char	display_buff[80];
static int		PQF_display_mode;
static int		PQF_power;
static int		PQF_current_step;
static int		active_field;
static int		field_type;
static int		field_dirty;
static float	PQF_pf;

const int	field_start[6]	= { 4,  9, 14, 23, 29, 35 };
const int	field_end[6]	= { 6, 11, 19, 27, 32, 37 };
char	*ep3char[3];
/*int	EP3_JIMPphase;*/

/*
 *	External Variables
 */
extern int		global_error;
extern int		NEXTMAXRMS;
extern int		MAXRMS;
extern int		slope_cnt;
extern int		adtotal_cnt;
extern int		IN_RMS_SUM;
extern int		PUB_RMS_SUM;
extern int		PUB_RMS_N;
extern int		RMS_ZERO;
extern int		LIN_RMS_ZERO;
extern int		RMS_PEAKP;
extern int		RMS_PEAKM;
extern int		list_size;
extern uint	Active_Step;
extern uint	zero_cnt;
extern uchar	front_interlock;
extern ushort	AD_CH;
extern ushort	PQF_drawer;
extern DISDATA		display;
extern P_PARAMT		paraml[3];
extern CPLDATA		cpldata;
extern PQFDATA		pqfdata;
extern LIST_RECORD	list_recs[MAXIMUM_LIST];

extern volatile ulong	vectors[];
extern volatile int		RMSCNT;
extern volatile int		default_level;
extern volatile uint	LIMITHIT;
extern volatile int		VAL_LIMITHIT;
extern volatile uint	STEP_LIMIT;

#define NO_FIELD			(0)
#define INT_FIELD		(1)
#define FLOAT_FIELD		(2)
#define KEY_F1			'a'
#define KEY_F2			'b'
#define KEY_F3			'c'
#define KEY_F4			'd'
#define KEY_F5			'e'
#define KEY_F6			'f'
#define KEY_F7			'g'
#define KEY_GO			'G'
#define KEY_STOP			'X'
#define KEY_STORE		'S'
#define KEY_RECALL		'R'
#define KEY_MENU			'M'
#define KEY_CLEAR		'C'
#define KEY_RIGHT		'>'
#define KEY_LEFT			'<'
#define KEY_UP			'^'
#define KEY_DOWN			'v'
#define KEY_ENTER		(13)

int	PQF_phase_pgm(P_PARAMT *params)
{
	uchar	phase;

	if(pqfdata.pqfflag)
	{
		phase = (uchar )EXTRACT_INT(0) - 1;
		if(!pqfdata.ep3flag || (phase > 2) || PQF_running)
			return(BAD_VALUE);

		if(EP3_SwitchPhase(phase))
			return(BAD_VALUE);
	}
	return(0);
}

int	EP3_SwitchPhase(
uchar	phase)
{
	uint	cntr;
	int	xv;
	float	f;

	if(phase > 3 || (!pqfdata.ep3flag))
		return(-1);

	/* If same phase do nothing */
	if(pqfdata.ep3phase == phase)
		return(0);

	pqfdata.ep3phase = phase;

	if(display.current == DISPLAY_PQF)
	{
		xv = LCD_getx();
		LCD_gotoxy(22,4);
		fprintf(DISPLAY,"%2s",ep3char[pqfdata.ep3phase]);
		LCD_gotoxy(xv,2);
	}

	/* Turn ON Bypass Contactor */
	*pqfdata.ep3relay |= RLY_RD22;
	ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	TPU_delay(100);

	/* RD26 OFF (0%), RD21 OFF (Input Tap), RD01 (SoftStart) */
	raw_relay(1,0);
	raw_relay(21,0);
	ECAT_TimerSequencer(PQF_drawer,000);
	TPU_write(SEQ_TPU,1);		/* Set it High	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);		/* Set it Low	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);		/* Set it High	*/
	TPU_delay(100);

	/* Shut OFF Current EP3 Phase */
	*pqfdata.ep3relay &= ~(EP3_L1 | EP3_L2 | EP3_L3 | RLY_RD23 | RLY_RD24 | RLY_RD25);
	ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
	ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
	TPU_delay(100);

	/* Turn ON the NEW EP3 Phase Input */
	switch(phase)
	{
		case 0: /* L1 */
			*pqfdata.ep3relay |= EP3_L1;
			break;
		case 1: /* L2 */
			*pqfdata.ep3relay |= EP3_L2;
			break;
		case 2: /* L3 */
			*pqfdata.ep3relay |= EP3_L3;
			break;
	}
	ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	TPU_delay(600);

	TPU_kill(PHASE_TPU);
	vectors[TPU_VECTOR_BASE+PHASE_TPU] = (long)TPU_trap;
	TPU_delay(100);
	f = TPU_ECAT_freq(PHASE_TPU);
	if((f > 65.0) || (f < 42.0))
		return(-1);

	TPU_ECAT_phase(PHASE_TPU,SEQ_TPU,90,0,1,0x00);
	cntr = 0;
	while(!TPU_ECAT_phase_done() && (cntr < 300000))
		cntr++;
	if(cntr >= 300000)
		return(-1);

	/* RD21 ON (Input Tap) */
	raw_relay(21,1);
	TPU_delay(1000);

	switch(phase)
	{
		case 0: /* L1 */
			*pqfdata.ep3relay |= RLY_RD23;
			break;
		case 1: /* L2 */
			*pqfdata.ep3relay |= RLY_RD24;
			break;
		case 2: /* L3 */
			*pqfdata.ep3relay |= RLY_RD25;
			break;
	}
	ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
	ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
	TPU_delay(100);

	/* RD26 ON (100%) */
	ECAT_TimerSequencer(PQF_drawer,( SC_100 | IG_100 ));
	TPU_write(SEQ_TPU,1);		/* Set it High	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,0);		/* Set it Low	*/
	TPU_delay(10);
	TPU_write(SEQ_TPU,1);		/* Set it High	*/
	TPU_delay(1000);

	/* Turn on RD01 (SoftStart) */
	raw_relay(1,1);
	TPU_delay(1000);

	/* Turn OFF Bypass Contactor */
	*pqfdata.ep3relay &= ~(RLY_RD22);
	ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	return(0);
}

void	cleanup_field()
{
	LIST_RECORD	*ar;
	long	iv;
	int	ox;
	float	fv;

	ox = LCD_getx();
	if(!field_dirty)
		return;
	if((PQF_current_step > list_size) || (list_size == 0))
		return;
	if(field_type == INT_FIELD)
		iv = atol(display_buff);
	else
		if(field_type == FLOAT_FIELD)
			fv = atof(display_buff);
		else
			return;

	ar = &list_recs[PQF_current_step-1];
	if(ar->action & 128)
	{
		if(iv > 0)
			ar->duration = (iv-1);
		else
		{
			LCD_gotoxy(field_start[active_field],3);
			sprintf(display_buff,"%6ld",(ar->duration+1));
			LCD_puts(display_buff);
			LCD_gotoxy(ox,3);
		}
	}
	else
	{
		switch(active_field)
		{
			case 1:
				if((iv >= 0) && (iv <= 359))
					ar->start_angle = iv;
				else
				{
					LCD_gotoxy(field_start[active_field],3);
					sprintf(display_buff,"%3d",ar->start_angle);
					LCD_puts(display_buff);
					LCD_gotoxy(ox,3);
				}
				break;
			case 2:
				if(ar->action & DO_CYCLES)
				{
					if(iv >= 0)
						ar->duration = iv;
					else
					{
						LCD_gotoxy(field_start[active_field],3);
						sprintf(display_buff,"%6ld",ar->duration);
						LCD_puts(display_buff);
						LCD_gotoxy(ox,3);
					}
				}
				else
				{
					if(fv >= 0.0)
						ar->duration = (long)(fv*100.0);
					else
					{
						LCD_gotoxy(field_start[active_field],3);
						sprintf(display_buff,"%6.2f",((float)ar->duration/100.0));
						LCD_puts(display_buff);
						LCD_gotoxy(ox,3);
					}
				}
				break;
			case 4:
				if((fv >= 0.0) && (fv <= 32.0))
					ar->RMS_limit = (int)(fv*10.0);
				else
				{
					LCD_gotoxy(field_start[active_field],3);
					sprintf(display_buff,"%4.1f",(float)ar->RMS_limit/10.0);
					LCD_puts(display_buff);
					LCD_gotoxy(ox,3);
				}
				break;
			case 5:
				if((iv >= 0) && (iv <= 999))
					ar->PEAK_limit = iv;
				else
				{
					LCD_gotoxy(field_start[active_field],3);
					sprintf(display_buff,"%3d",ar->PEAK_limit);
					LCD_puts(display_buff);
					LCD_gotoxy(ox,3);
				}
				break;
		}
	}
	field_dirty = FALSE;
}

void	change_field(
int	nf)
{
	LIST_RECORD	*ar;

	LCD_cursor(0);
	if((active_field >= 0) && (active_field <= 5))
		LCD_normal(field_start[active_field],3,field_end[active_field],3);
	if((nf >= 0) && (nf <= 5))
	{
		active_field = nf;
		LCD_hilight(field_start[active_field],3,field_end[active_field],3);
	}
	field_type = NO_FIELD;

	if((list_size) && (PQF_current_step))
	{
		ar = &list_recs[PQF_current_step-1];
		if(ar->action & 128)
		{														/* Not an action field */
			switch(ar->action)
			{
				case DO_RETURN:
					LCD_cursor(0);
					field_type = NO_FIELD;
					break;
				case DO_GOSUB:
				case DO_GOSUBN:
				case DO_BRANCHN:
				case DO_BRANCH:
					if(nf == 3)
					{
						field_type = INT_FIELD;
						sprintf(display_buff,"%6ld",(ar->duration+1));
						LCD_gotoxy(field_end[active_field],3);
						LCD_cursor(1);
						break;
					}
					else
					{
						LCD_cursor(0);
						field_type = NO_FIELD;
						break;
					}
			}
		}
		else
		{
			switch(active_field)
			{
				case 0:	/* Tap Field */
				case 3:	/* Cycles/Sec field */
					LCD_cursor(0);
					field_type = NO_FIELD;
					break;
				case 1:	/* Start angle field */
					field_type = INT_FIELD;
					sprintf(display_buff,"%3d",ar->start_angle);
					LCD_gotoxy(field_end[active_field],3);
					LCD_cursor(1);
					break;
				case 2:	/* Duration field */
					if(ar->action & DO_CYCLES)
					{
						field_type = INT_FIELD;
						sprintf(display_buff,"%6ld",ar->duration);
					}
					else
					{
						field_type = FLOAT_FIELD;
						sprintf(display_buff,"%6.2f",((float)ar->duration/100.0));
					}
					LCD_gotoxy(field_end[active_field],3);
					LCD_cursor(1);
					break;
				case 4:	/* RMS field */
					field_type = FLOAT_FIELD;
					sprintf(display_buff,"%4.1f",((float)ar->RMS_limit/10.0));
					LCD_gotoxy(field_end[active_field],3);
					LCD_cursor(1);
					break;
				case 5:	/* Peak Field */
					field_type = INT_FIELD;
					sprintf(display_buff,"%3d",ar->PEAK_limit);
					LCD_gotoxy(field_end[active_field],3);
					LCD_cursor(1);
					break;
			}
		}
	}
	field_dirty = FALSE;
}

void	SetUp_Display(
int	dv)
{
	char	buffer[100];
	int	ox,oy;

	if(dv == PQF_display_mode)
		return;

	switch(dv)
	{
		case PQF_DISPLAY_NORMAL:
			LCD_reset();
			LCD_gotoxy(0,0);
			LCD_puts("STP  %  START   STEP   CYCLE   ILIMIT  \n\r");
			LCD_puts(" #  LVL  ANG  DURATION /SEC   RMS  PEAK\n\r");
			LCD_puts("                                       \n\r");
			LCD_puts("              Start                    \n\r");
			LCD_puts("              End                      \n\r");
			LCD_puts("Nominal 120V                           \n\r");
			LCD_puts("                                       \n\r");
			LCD_puts(" INS   DEL  MISC   ON   OFF  SETUP     \n\r");
			LCD_line(21,0,21,39,1);
			LCD_line(45,0,45,39,1);
			LCD_line(81,0,81,39,1);
			LCD_line(135,0,135,39,1);
			LCD_line(171,0,171,39,1);
			LCD_line(204,8,204,39,1);
			LCD_line(239,0,239,39,1);
			LCD_line(0,15,239,15,1);
			LCD_line(0,39,239,39,1);
			LCD_cursor(0);
			PQF_display_mode = PQF_DISPLAY_NORMAL;
			if(list_size != 0)
				display_list();
		case PQF_DISPLAY_NUMBERS:
			ox = LCD_getx();
			oy = LCD_gety();
			LCD_gotoxy(0,5);
			if(PQF_running)
			{
				if(!default_level)
					sprintf(buffer,"Nominal %3dV Idle:Open Actual %3dV   ",PQF_nominal_V,PQF_actual_V);
				else
					sprintf(buffer,"Nominal %3dV Idle:100%% Actual %3dV   ",PQF_nominal_V,PQF_actual_V);
				LCD_puts(buffer);
				LCD_gotoxy(0,6);
				sprintf(buffer,"RMS %5.1fA Peak %3dA  StepMAX %3dA",PQF_amps_rms,PQF_amps_peak,PQF_step_peak);
				LCD_puts(buffer);
			}
			else
			{
				if(!default_level)
					sprintf(buffer,"Nominal %3dV  Idle:Open                ",PQF_nominal_V);
				else
					sprintf(buffer,"Nominal %3dV  Idle:100%%                ",PQF_nominal_V);
				LCD_puts(buffer);
				LCD_gotoxy(0,6);
				sprintf(buffer,"                                       ");
				LCD_puts(buffer);
			}
			LCD_gotoxy(ox,oy);
			break;
		case PQF_DISPLAY_SETUP:
			PQF_display_mode = PQF_DISPLAY_SETUP;
			LCD_reset();
			LCD_gotoxy(14,0);
			LCD_puts("PQF SETUP\n\r\n\r");
			fprintf(DISPLAY,"Nominal Line Voltage: %3d V\n\r",PQF_nominal_V);
			LCD_puts("Standby Level       : ");
			if(!default_level)
				LCD_puts("Open");
			else
				LCD_puts("100%");
			ep3char[0] = "L1";
			ep3char[1] = "L2";
			ep3char[2] = "L3";
			if(pqfdata.ep3flag)
			{
				fprintf(DISPLAY,"\n\rInterrupt Phase     : %2s\n\r\n\r",ep3char[pqfdata.ep3phase]);
				LCD_puts("Standby     Interrupt Phase  Qual\n\r");
				LCD_puts("Open  100%  L1    L2    L3   Line  Done");
			}
			else
			{
				LCD_puts("\n\r\n\r\n\rStandby                      Qual\n\r");
				LCD_puts("Open  100%                   Line  Done");
			}
			LCD_cursor(1);
			LCD_hilight(22,2,25,2);
			LCD_gotoxy(22,2);
			break;
		case PQF_DISPLAY_ERROR:
			PQF_display_mode = dv;
			LCD_reset();
			LCD_gotoxy(0,2);
			LCD_puts("   ERROR: \n\r");
			LCD_puts(err_msg);
			LCD_gotoxy(0,7);
			LCD_puts("                             Abort");
			LCD_cursor(0);
			break;
	case PQF_DISPLAY_QUALIFY:
			PQF_display_mode = dv;
			LCD_reset();
			LCD_gotoxy(0,2);
			LCD_puts("   Please Wait Qualifying Line Inrush\n\r");
			LCD_cursor(0);
			break;
	case PQF_REMOTE:
			PQF_display_mode = dv;
			LCD_reset();
			FP_UpdateRemote();
			LCD_cursor(0);
			break;
	case PQF_STARTUP:
			PQF_display_mode = dv;
			LCD_reset();
			LCD_gotoxy(0,2);
			LCD_puts("         Starting System\n\r");
			LCD_cursor(0);
			break;
	case PQF_DISPLAY_STORE:
	case PQF_DISPLAY_RECALL:
			PQF_display_mode = dv;
			LCD_reset();
			LCD_gotoxy(0,3);
			if(dv == PQF_DISPLAY_RECALL)
				LCD_puts("   Recall From (0..9)?\n\r");
			else
				LCD_puts("   Store to  (0..9)?\n\r");
			LCD_gotoxy(0,7);
			LCD_puts("                             Done");
			LCD_cursor(0);
			break;
	}
}


void	show_one_raw(
int	number,
int	row)
{
	char	buffer[50];
	uchar	action;

	action = list_recs[number-1].action;
	LCD_gotoxy(0,row);
	sprintf(buffer,"%3d ",number);
	LCD_puts(buffer);
	switch(action & (~DO_CYCLES))
	{
		case DO_BRANCHN:
		case DO_BRANCH:
			sprintf(buffer,"          GOTO    %6ld           ",list_recs[number-1].duration+1);
			LCD_puts(buffer);
			break;
		case DO_GOSUBN:
		case DO_GOSUB:
			sprintf(buffer,"          GOSUB   %6ld           ",list_recs[number-1].duration+1);
			LCD_puts(buffer);
			break;
		case DO_RETURN:
			LCD_puts("          RETURN                    \n\r");
			break;
		case DO_PAUSE:
			LCD_puts("          PAUSE                     \n\r");
 			break;
		case DO_STOP:
			LCD_puts("          STOP                      \n\r");
			break;
		case DO_SHORT:	LCD_puts("SHT  "); break;
		case DO_OPEN:	LCD_puts("OPN  "); break;
		case DO_40:		LCD_puts(" 40  "); break;
		case DO_50:		LCD_puts(" 50  "); break;
		case DO_70:		LCD_puts(" 70  "); break;
		case DO_80:		LCD_puts(" 80  "); break;
		case DO_90:		LCD_puts(" 90  "); break;
		case DO_100:	LCD_puts("100  "); break;
		case DO_110:	LCD_puts("110  "); break;
		case DO_120:	LCD_puts("120  "); break;
		case DO_150:	LCD_puts("150  "); break;
	}
	if((action & 128) == 0)
	{								/* We have a real step */
		if(action & DO_CYCLES)
			sprintf(buffer,"%3d  %6ld   Cycle %4.1f  %3d",list_recs[number-1].start_angle,
						list_recs[number-1].duration,(float)(list_recs[number-1].RMS_limit/10.0),
						list_recs[number-1].PEAK_limit);
		else
			sprintf(buffer,"%3d  %6.2f   Secs  %4.1f  %3d",list_recs[number-1].start_angle,
						(float)(list_recs[number-1].duration/100.0),(float)(list_recs[number-1].RMS_limit/10.0),
						list_recs[number-1].PEAK_limit);
		buffer[39] = 0;
		LCD_puts(buffer);
	}
}

void	display_list()
{
	int	ox,oy;

	ox = LCD_getx();
	oy = LCD_gety();

	if(PQF_current_step == 0)
	{
		LCD_gotoxy(0,2);
		LCD_puts("                                       \n\r");
		LCD_puts("              Start                    \n\r");
		change_field(2);
	}
	else
	{
		if(PQF_current_step == 1)
		{
			LCD_gotoxy(0,2);
			LCD_puts("              Start                    \n\r");
		}
		else
			show_one_raw(PQF_current_step-1,2);
		show_one_raw(PQF_current_step,3);
		if(list_recs[PQF_current_step-1].action & 128)
		{
			if((list_recs[PQF_current_step-1].action == DO_RETURN) ||
				(list_recs[PQF_current_step-1].action==DO_RETURN))
				change_field(2);
			else
				if(active_field != 2)
					change_field(3);
		}
		else
			LCD_hilight(field_start[active_field],3,field_end[active_field],3);
	}
	if(PQF_current_step == list_size)
	{
		LCD_gotoxy(0,4);
		LCD_puts("              End                      \n\r");
	}
	else
		show_one_raw(PQF_current_step+1,4);
	LCD_gotoxy(ox,oy);
}


void	DO_insert()
{
	int	icpy;
	int	i;

	cleanup_field();
	if(!list_size)
	{
		list_recs[0].duration = 30;
		list_recs[0].action = DO_100 | DO_CYCLES;
		list_recs[0].start_angle = 0;
		list_recs[0].RMS_limit = 100;
		list_recs[0].PEAK_limit = 30;
		PQF_current_step = 1;
		list_size++;
		change_field(0);
		display_list();
		LCD_cursor(1);
	}							/* First list element		*/
	else
	{							/* Not the First element	*/
		if(list_size != PQF_current_step)
		{
			for(i = list_size; i >= PQF_current_step; i--)
			{
				list_recs[i].duration	= list_recs[i-1].duration;
				list_recs[i].action		= list_recs[i-1].action;
				list_recs[i].start_angle= list_recs[i-1].start_angle;
				list_recs[i].RMS_limit	= list_recs[i-1].RMS_limit;
				list_recs[i].PEAK_limit	= list_recs[i-1].PEAK_limit;
			}
		}
		if(PQF_current_step > 0)
			icpy = PQF_current_step-1;
		else
			icpy = 0;
		list_recs[PQF_current_step].duration   = list_recs[icpy].duration;
		list_recs[PQF_current_step].action		= list_recs[icpy].action;
		list_recs[PQF_current_step].start_angle= list_recs[icpy].start_angle;
		list_recs[PQF_current_step].RMS_limit	= list_recs[icpy].RMS_limit;
		list_recs[PQF_current_step].PEAK_limit	= list_recs[icpy].PEAK_limit;
		list_size++;
		PQF_current_step++;
		display_list();
	}							/* Not the First Element	*/
}


void	DO_delete()
{
	int	i;

	LCD_normal(field_start[active_field],3,field_end[active_field],3);
	if(!list_size)
		return;
	if(PQF_current_step == 0)
	{
		PQF_current_step++;
		display_list();
		change_field(active_field);
		return;
	}
	if(list_size != 1)
	{
		for(i = PQF_current_step-1; i < list_size; i++)
		{
			list_recs[i].duration	= list_recs[i+1].duration;
			list_recs[i].action		= list_recs[i+1].action;
			list_recs[i].start_angle= list_recs[i+1].start_angle;
			list_recs[i].RMS_limit	= list_recs[i+1].RMS_limit;
			list_recs[i].PEAK_limit	= list_recs[i+1].PEAK_limit;
		}
	}
	list_size--;
	if(PQF_current_step > list_size)
		PQF_current_step = list_size;
	display_list();
}

void	DO_GOTO_KEY()
{
	DO_insert();
	list_recs[PQF_current_step-1].action	= DO_BRANCH;
	list_recs[PQF_current_step-1].duration	= 0;
	display_list();
	change_field(3);
}

void	DO_MISC_KEY()
{
	DO_insert();
	list_recs[PQF_current_step-1].action	= DO_BRANCH;
	list_recs[PQF_current_step-1].duration	= 0;
	display_list();
	change_field(2);
}

void	DO_PQFSETUP()
{
	int	newv;

	if(PQF_display_mode == PQF_DISPLAY_NORMAL)
	{
		cleanup_field();
		SetUp_Display(PQF_DISPLAY_SETUP);
	}
	else
	{
		newv = atoi(display_buff);
		if((newv >= 100) && (newv <= 240))
			PQF_nominal_V = newv;
		SetUp_Display(PQF_DISPLAY_NORMAL);
	}
}

void	DO_STORE(
char	c)
{
	cleanup_field();
	raw_save();
}

void	DO_RECALL(
char	c)
{
	raw_load();
	field_dirty = FALSE;
	PQF_current_step = 0;
	display_list();
	change_field(active_field);
}

void	DO_GO()
{
	cleanup_field();
	if(!list_size)
		return;
	PQF_running = TRUE;
	SetUp_Display(PQF_STARTUP);
	last_zero = 0;
	raw_run(0);
	if(global_error)
	{
		switch(global_error)
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
				err_msg = "No AC present";
				break;
			case NO_EUT:
				err_msg = "EUT Power is NOT Enabled\n\r           Cycle the BLUE Button";
				break;
			case NO_EUTR:
				err_msg = "EUT Power is NOT Requested\n\r           ABORT then Select ON (F4)";
				break;
			default:
				err_msg = "Unknown Error";
		}
		SetUp_Display(PQF_DISPLAY_ERROR);
		PQF_running = FALSE;
		KEY_led(GO_LED,0);
		KEY_led(STOP_LED,1);
	}
	else
		SetUp_Display(PQF_DISPLAY_NORMAL);
	LIMITHIT = 0;
}

void	DO_STOP_KEY()
{
	active_field = 0;
	change_field(0);
	SetUp_Display(PQF_DISPLAY_NUMBERS);
	if(PQF_running)
		raw_stop();
	PQF_running = FALSE;
}

void	Process_Normal_Field(
char	c)
{
	int	xv;

	switch(c)
	{
		case KEY_UP:
			cleanup_field();
			if(PQF_current_step > 0)
			{
				PQF_current_step--;
				change_field(active_field);
				display_list();
			}
			break;
		case KEY_DOWN:
			cleanup_field();
			if(PQF_current_step < list_size)
			{
				PQF_current_step++;
				display_list();
				change_field(active_field);
			}
			break;
		case KEY_LEFT:
			if(field_type == NO_FIELD)
			{
				if(list_size)
				{
					if((list_recs[PQF_current_step-1].action & 128) == 0)
					{
						cleanup_field();
						change_field((active_field+5) % 6);
					}
					else
					{
						if((list_recs[PQF_current_step-1].action==DO_RETURN) ||
							(list_recs[PQF_current_step-1].action==DO_STOP) ||
							(list_recs[PQF_current_step-1].action==DO_PAUSE))
							return;
						change_field(3);
					}
				}
			}
			else
			{
				xv = LCD_getx();
				xv--;
				if(xv < field_start[active_field])
				{
					if((list_recs[PQF_current_step-1].action & 128) == 0)
					{
						cleanup_field();
						change_field((active_field+5) % 6);
					}
					else
						if(active_field == 3)
						{
							cleanup_field();
							change_field(2);
						}
						else
							LCD_gotoxy(field_start[active_field],3);
				}
				else
					LCD_gotoxy(xv,3);
 			}
			break;
		case KEY_RIGHT:
			if(field_type == NO_FIELD)
			{
				if(list_size)
				{
					if((list_recs[PQF_current_step-1].action & 128) == 0)
					{
						cleanup_field();
						change_field((active_field+1) % 6);
					}
					else
					{
						if((list_recs[PQF_current_step-1].action==DO_RETURN) ||
							(list_recs[PQF_current_step-1].action==DO_STOP)  ||
							(list_recs[PQF_current_step-1].action==DO_PAUSE))
							return;
						change_field(3);
					}
				}
			}
			else
			{
				xv = LCD_getx();
				xv++;
				if(xv > field_end[active_field])
				{
					if((list_recs[PQF_current_step-1].action & 128) == 0)
					{
						cleanup_field();
						change_field((active_field+1) % 6);
					}
					else
					{
						cleanup_field();
						change_field(2);
					}
				}
				else
					LCD_gotoxy(xv,3);
			}
			break;
		case KEY_ENTER:
			cleanup_field();
			if(!list_size)
				return;
			if(list_recs[PQF_current_step-1].action & 128)
			{
				if(active_field == 3)
					return;
				switch(list_recs[PQF_current_step-1].action)
				{
					case DO_BRANCH:
						list_recs[PQF_current_step-1].action = DO_STOP;
						break;
					case DO_BRANCHN:
						list_recs[PQF_current_step-1].action = DO_GOSUB;
						break;
					case DO_GOSUB:
					case DO_GOSUBN:
						list_recs[PQF_current_step-1].action = DO_RETURN;
						break;
					case DO_RETURN:
						list_recs[PQF_current_step-1].action = DO_PAUSE;
						break;
					case DO_PAUSE:
						list_recs[PQF_current_step-1].action = DO_STOP;
						break;
					case DO_STOP:
						list_recs[PQF_current_step-1].action = DO_BRANCH;
						break;
 				}
				show_one_raw(PQF_current_step,3);
				return;
			} 
			else
				if(active_field == 0)
				{
					if((list_recs[PQF_current_step-1].action & 0x0f) == DO_150)
						list_recs[PQF_current_step-1].action &= 0xF0;
					else
						list_recs[PQF_current_step-1].action += 1;
					show_one_raw(PQF_current_step,3);
				}
				else
					if(active_field == 3)
					{
						list_recs[PQF_current_step-1].action ^= DO_CYCLES;
						if((list_recs[PQF_current_step-1].action & DO_CYCLES) == 0)
							list_recs[PQF_current_step-1].duration %= 10000;
						show_one_raw(PQF_current_step,3);
					}
			break;
		case 'C':
			if(field_type == NO_FIELD)
				break;
			xv = field_end[active_field] - field_start[active_field];
			LCD_gotoxy(field_start[active_field],3);
			for(xv = xv; xv >= 0; xv--)
			{
				display_buff[xv]=' ';
				LCD_putc(' ');
			}
			field_dirty = TRUE;
			LCD_gotoxy(field_start[active_field],3);
			break;
		case '.':
			if(field_type != FLOAT_FIELD)
				break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(field_type == NO_FIELD)
				break;
			xv = LCD_getx();
			LCD_putc(c);
			if(((xv - field_start[active_field]) >= 0) &&
				(xv <= field_end[active_field]))
			{
				display_buff[xv-field_start[active_field]] = c;
				display_buff[(field_end[active_field]-field_start[active_field])+1] = 0;
			}
			field_dirty = TRUE;
			xv++;
			if(xv > field_end[active_field])
				xv = field_end[active_field];
			LCD_gotoxy(xv,3);
	}
}

void	Process_Setup_Field(
char	c)
{
	int	xv;

	switch(c)
	{
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_RIGHT:
 			xv = LCD_getx();
			if(xv < 25)
				LCD_gotoxy(xv+1,2);
			break;
		case KEY_LEFT:
			xv = LCD_getx();
			if(xv > 22)
				LCD_gotoxy(xv-1,2);
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
			xv = LCD_getx();
			xv = xv - 22;
			if((xv >= 0) && (xv <= 2))
			{
				display_buff[xv] = c;
				LCD_putc(c);
				if(xv == 25)
					LCD_gotoxy(25,2);
			}
			break;
		case 'C':
			LCD_gotoxy(22,2);
			LCD_puts("   ");
			LCD_gotoxy(22,2);
			display_buff[0] = ' ';
			display_buff[1] = ' ';
			display_buff[2] = ' ';
			display_buff[3] = 0;
			break;
	}
}

void	show_qualify_results(
int	iv)
{
	char	buff[80];

	LCD_reset();
	LCD_gotoxy(0,1);
	if(iv > 0)
	{
		sprintf(buff,"Peak Positive Current: %dA\n\r", ((iv & 0xffff)));
		LCD_puts(buff);
		sprintf(buff,"Peak Negative Current: %dA\n\r",(((iv>>16) & 0xffff)));
		LCD_puts(buff);
	}
	else
	{
	 	paraml[0].idata = 0;
	 	root_EUT_pgm(paraml);
		if(iv == -255)
			LCD_puts("  Error NO AC present\n\r");
		else
			LCD_puts("  Error Qualifying the Line\n\r");
	}
	LCD_gotoxy(0,7);
	LCD_puts("                             Done");
	LCD_cursor(0);
}

void	Process_Char(
char	c)
{
	int	ox,oy;

	switch(PQF_display_mode)
	{
		case PQF_DISPLAY_ERROR:
			if(c == 'f')
				SetUp_Display(PQF_DISPLAY_NORMAL);
			break;
		case PQF_REMOTE:
			break;
		case PQF_DISPLAY_STORE:
			SetUp_Display(PQF_DISPLAY_NORMAL);
			if((c >= '0') && (c <= '9'))
				DO_STORE(c);
			break;
		case PQF_DISPLAY_RECALL:
			SetUp_Display(PQF_DISPLAY_NORMAL);
			if((c >= '0') && (c <= '9'))
				DO_RECALL(c);
			break;
		case PQF_DISPLAY_NORMAL:
			if((PQF_running) && (c != KEY_STOP))
				return;
			else
				switch(c)
				{
					case KEY_F1:
						DO_insert();
						break;
					case KEY_F2:
						DO_delete();
						break;
					case KEY_F3:
						DO_MISC_KEY();
						break;
					case KEY_F4:				/* EUT ON */
						raw_relay(18,1);
						raw_relay(16,1);
						raw_relay(17,1);
						paraml[0].idata = 1;
						root_EUT_pgm(paraml);
						break;
					case KEY_F5:
						raw_relay(18,0);
						raw_relay(16,0);
						paraml[0].idata = 0;
						root_EUT_pgm(paraml);
						break;
					case KEY_F6:
						DO_PQFSETUP();
						break;
					case KEY_STORE:
						cleanup_field();
						DO_STORE('0');
						break;
					case KEY_RECALL:
						DO_RECALL('0');
						break;
					case KEY_GO:
						DO_GO();
						break;
					case KEY_STOP:
						DO_STOP_KEY();
						break;
					case KEY_MENU:
						raw_stop();
						display.request = DISPLAY_LOCAL;
						break;
					default:
						Process_Normal_Field(c);
				}
			break;
		case PQF_DISPLAY_QUALIFY:
			switch(c)
			{
				case KEY_F6:
				case KEY_MENU:
					SetUp_Display(PQF_DISPLAY_SETUP);
					break;
			}
			break;
		case PQF_DISPLAY_SETUP:
			switch(c)
			{
				case KEY_F1:
					if(pqfdata.ep3flag) /* TAKE OUT LATER (JMM) 03/13/95 */
					{
/************************************************************
						EP3_JIMPphase = (EP3_JIMPphase + 90) % 360;
						ox = LCD_getx();
						oy = LCD_gety();
						LCD_gotoxy(0,7);
						fprintf(DISPLAY,"%3dd",EP3_JIMPphase);
						LCD_gotoxy(ox,oy);
						KEY_beep();
************************************************************/
					}
					else
					{
						raw_default(0);
						PQF_display_mode = -1;
						SetUp_Display(PQF_DISPLAY_SETUP);
					}
					break;
				case KEY_F2:
					if(pqfdata.ep3flag)
						KEY_beep();
					else
					{
						raw_default(100);
						PQF_display_mode = -1;
						SetUp_Display(PQF_DISPLAY_SETUP);
					}
					break;
				case KEY_F3: /* EP3 - L1 */
					EP3_SwitchPhase(0);
					break;
				case KEY_F4: /* EP3 - L2 */
					EP3_SwitchPhase(1);
					break;
				case KEY_F5: /* EP3 - L3 */
					EP3_SwitchPhase(2);
					break;
				case KEY_F6:
					SetUp_Display(PQF_DISPLAY_QUALIFY);
					if(!cpldata.eutblue)
					{
						raw_relay(2,1);
						raw_relay(18,1);
						raw_relay(16,1);
						raw_relay(17,1);
						paraml[0].idata = 1;
						root_EUT_pgm(paraml);
						LCD_gotoxy(0,2);
						LCD_puts("Please Cycle the blue button and:     \n\r");
						LCD_gotoxy(0,6);                        
						LCD_puts("                              Try      \n\r");
						LCD_puts("                             Again     \n\r");
					}
					else
					{
						show_qualify_results(raw_qualify());
						raw_relay(2,0);
						raw_relay(3,0);
						while(KEY_hit())
							KEY_getchar();
					}
					break;
				case KEY_STORE:
				case KEY_RECALL:
				case KEY_GO:
				case KEY_STOP:
					break;
				case KEY_MENU:
				case KEY_F7:
					DO_PQFSETUP();
					break;
				default:
					Process_Setup_Field(c);
			}
			break;
	}
}

void	startup()
{
	PQF_display_mode	= PQF_DISPLAY_NONE;
	PQF_running			= FALSE;
	PQF_amps_rms		= 0.0;
	PQF_amps_peak		= 0;
	PQF_step_peak		= 0;
	PQF_pf				= 0.99;
	PQF_nominal_V		= 120;
	PQF_actual_V		= 0;
	PQF_power			= 0;
	PQF_current_step	= 0;
	active_field		= 0;
	field_type = NO_FIELD;
	SetUp_Display(PQF_DISPLAY_NORMAL);
	KEY_led(STOP_LED,1);
}

void	show_state_pqfdemo(void)
{
	startup();
}

uchar	demo_funcs(
char	c)
{
	Process_Char(c);
}

void	do_PQF_main(
void	(*p)())
{
	float	f;
	int	i;
	int	dinbyte;
	static	err_cnt;
	static	last_step;

	if(!front_interlock)
	{
		dinbyte = ECAT_9346KernelRead(PQF_drawer);
		if((!(dinbyte & 0x100)) && (!new_eut))
		{	/* Snub Bus */
			err_cnt++;
			if(err_cnt > 10)
			{									  
				raw_relay(18,0);
				raw_stop();
				err_msg = "Snub BUS voltage exceeded";
				global_error = SNUB_BUS;
				if(PQF_display_mode != PQF_REMOTE)
					SetUp_Display(PQF_DISPLAY_ERROR);
				PQF_running = FALSE;
				paraml[0].idata = 0;
				root_EUT_pgm(paraml);
				raw_relay(1,0);	/* Here for soft start fix? (JMM - 07/21/93) */
			}
			return;
		} /* Snub Bus */
		else
			if((dinbyte & 0x200))
			{ /* OVER Current */
				err_cnt++;
				if(err_cnt > 5)
				{
					raw_relay(18,0);
					raw_stop();
					global_error = IGBT_LIMIT;
					err_msg = "Over current Fault";
					if(PQF_display_mode != PQF_REMOTE)
						SetUp_Display(PQF_DISPLAY_ERROR);
					PQF_running = FALSE;
					paraml[0].idata = 0;
					root_EUT_pgm(paraml);
					raw_relay(1,0);	/* Here for soft start fix? (JMM - 07/21/93) */
				}
				return;
			} /* OVER Current */
			else
				if(dinbyte & 0x400)
				{ /* PQF Over Temp */
					err_cnt++;
					if(err_cnt > 10)
					{
						raw_relay(18,0);
						raw_stop();
						global_error = OVER_TEMP;
						err_msg = "Over Temperature Fault";
						if(PQF_display_mode != PQF_REMOTE)
							SetUp_Display(PQF_DISPLAY_ERROR);
						PQF_running = FALSE;
						paraml[0].idata = 0;
						root_EUT_pgm(paraml);
						raw_relay(1,0);	/* Here for soft start fix? (JMM - 07/21/93) */
					}
					return;
				} /* PQF Over Temp */
				else
					err_cnt = 0;
	} /* Not front interlock */
	if(!PQF_running) 
	{
		adtotal_cnt += 1; 
		if(adtotal_cnt < 2000)
			return;
	}
	else
	{
		if(NEXTMAXRMS > MAXRMS)
			MAXRMS = NEXTMAXRMS;
		else
			if(NEXTMAXRMS < MAXRMS)
			{
				RMSCNT = 0;
				MAXRMS = NEXTMAXRMS;
			}
		if(RMSCNT > 2)
		{
			f = (double)PUB_RMS_SUM / (double)PUB_RMS_N;
			if(f > MAXRMS)
			{
				LIMITHIT = 3;
				VAL_LIMITHIT = PUB_RMS_SUM / PUB_RMS_N;
				STEP_LIMIT = Active_Step;
				PQF_amps_rms = sqrt(f) / 16.384;
			}
		}
		if(LIMITHIT && PQF_running)
		{
			if(RMS_PEAKP > RMS_PEAKM)
			{
				if(VAL_LIMITHIT > RMS_PEAKP)
					PQF_amps_peak = (int)(VAL_LIMITHIT / 16.384);
				else
					PQF_amps_peak = (int)(RMS_PEAKP / 16.384);
			}
			else
			{
				if(VAL_LIMITHIT > RMS_PEAKM)
					PQF_amps_peak = (int)(VAL_LIMITHIT / 16.384);
				else
					PQF_amps_peak = (int)(RMS_PEAKM / 16.384);
			}   
			raw_stop();
			switch(LIMITHIT)
			{
				case 1:
					sprintf(limit_buff,"+Peak of %d exceeded\n\r     Step %d limit",PQF_amps_peak,STEP_LIMIT+1);
					global_error = OVER_PEAK;
					break;
				case 2:
					sprintf(limit_buff,"-Peak of %d exceeded\n\r     Step %d limit",PQF_amps_peak,STEP_LIMIT+1);
					global_error = OVER_PEAK;
					break;
				case 3:
					sprintf(limit_buff,"RMS of %3.1f exceeded\n\r     Step %d limit",PQF_amps_rms,STEP_LIMIT+1);
					global_error = OVER_RMS;
					break;
				default:
					sprintf(limit_buff,"Unknown Limit Error");
					break;
			}
			err_msg = limit_buff;
			if(PQF_display_mode != PQF_REMOTE)
				SetUp_Display(PQF_DISPLAY_ERROR);
			PQF_running = FALSE;
			paraml[0].idata = 0;
			root_EUT_pgm(paraml);
			raw_relay(1,0);			/* Here for soft start fix? (JMM - 07/21/93) */
			return;
		}
		if(adtotal_cnt < 1000)
			return;
	}
	p();	/* Yeah, SO? */

	if(cpldata.eutblue == ON)
		raw_relay(16,1);
	else
	{
		raw_relay(16,0);
		raw_relay(1,0);			/* Here for soft start fix? (JMM - 07/21/93) */
	}

	if(!PQF_running)
	{
		adtotal_cnt = 0;
		return;
	}
	if(Active_Step == -1)
	{
		raw_stop();
		return;
	}
	if(front_interlock)
		return;

	if(last_zero == zero_cnt)
	{ /* No zero crossing */
		raw_stop();
		err_msg = "Input AC Lost";
		global_error = NO_AC;
		if(PQF_display_mode != PQF_REMOTE)
			SetUp_Display(PQF_DISPLAY_ERROR);
		PQF_running=FALSE;
		return;           
	} /* PQF lost zero */
	last_zero = zero_cnt;

	f = (double)IN_RMS_SUM / (double)PUB_RMS_N;
	PQF_actual_V = (int)sqrt(f) * 0.1953;

	f = (double)PUB_RMS_SUM / (double)PUB_RMS_N;
	PQF_amps_rms = sqrt(f) / 16.384;

	if(RMS_PEAKP > RMS_PEAKM)
		PQF_amps_peak = (int)(RMS_PEAKP / 16.384);
	else
		PQF_amps_peak = (int)(RMS_PEAKM / 16.384);
	RMS_PEAKP = -9999;
	RMS_PEAKM = 9999;
	PQF_power = 0;
	PQF_pf = 0;
	adtotal_cnt = 0;
	if(last_step != Active_Step)
	{
		last_step = Active_Step;
		PQF_step_peak = 0;
	}
	if(PQF_amps_peak > PQF_step_peak)
		PQF_step_peak = PQF_amps_peak;

	if(PQF_display_mode != PQF_REMOTE)
	{
		SetUp_Display(PQF_DISPLAY_NUMBERS);
		i = Active_Step;
		if((i < list_size) && (i >= 0))
		{
			PQF_current_step = Active_Step + 1;
			display_list();
		}
	}
	return;
}

void	do_remote(void)
{
	if(PQF_display_mode != PQF_REMOTE)
	{
		SetUp_Display(PQF_REMOTE);
		raw_relay(16,1);
	}
}

void	PQF_after_ilock()
{
	DO_STOP_KEY();
}
