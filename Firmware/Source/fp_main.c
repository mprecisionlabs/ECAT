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
#include "ecat.h"
#include "intface.h"
#include "timer.h"
#include "tiocntl.h"
#include "rev.h"
#include "emc.h"

void	show_state_main(void);
void	show_state_rmsbad(void);

extern int			current_field;
extern uchar		calibration;
extern uchar		keyenable;
extern uchar		ecatmode;
extern uchar		(*func_manager)(char);
extern MODINFO	modinfo[];
extern SRGDATA	srgdata;
extern SRGCHEAT srgCheat;
extern CPLDATA	cpldata;
extern EFTDATA	eftdata;
extern EFTCHEAT	eftCheat;
extern PKDATA		peakdata;
extern DISDATA	display;
extern PQFDATA	pqfdata;
extern EMCDATA	emcdata;
extern ONE_FIELD	field_list[32];
extern WARM_UP	warmup;

uchar	noneTrack;

void	show_state_none(
char		*title,
char		*name,
uchar	backdis)
{
	int	i;
	char	temp[50];

	KEY_beep();
	LCD_gotoxy(((40-strlen(title))/2),1);
	LCD_puts(title);
	sprintf(temp,"There are no %s Modules",name);
	LCD_gotoxy(((40-strlen(temp))/2),3);
	LCD_puts(temp);
	LCD_gotoxy(8,4);
	LCD_puts("installed in this system");

	noneTrack = backdis;
	func_manager = none_funcs;
}

void	Show_State(
int	state)
{
	int	i;

	for(i = 0; i < 32; i++)
	{
		field_list[i].valid = 0;
		field_list[i].xNext = field_list[i].xPrev = 0xFF;
		field_list[i].yNext = field_list[i].yPrev = 0xFF;
	}
	keyenable = 0;
/*	peakdata.cheat = 0;*/
	switch(state)
	{
		case DISPLAY_LOCAL:
			switch(ecatmode)
			{
				case SRG_MODE:
					SRG_SurgeKill();
					srgCheat.lineCheck	 = 0;
					break;
				case EFT_MODE:
					EFT_StopSystem();
					eftCheat.forceHVRange = 0;
					eftCheat.lineCheck	 = 0;
					break;
				case PQF_MODE:
					/* Any Clean-UP Needed Here? */
					break;
				default:
					break;
			}
			ECAT_ModeChange(GHOST_MODE);
			show_state_main();
			break;
		case DISPLAY_SRG:
			if(srgdata.srgflag)
			{
				keyenable = 1;
				srgCheat.lineCheck	 = 0;
				ECAT_ModeChange(SRG_MODE);
				show_state_srg();
			}
			else
				show_state_none("Module ERROR","SURGE",DISPLAY_LOCAL);
			break;
		case DISPLAY_EFT:
			if(eftdata.eftflag)
			{
				keyenable = 1;
				eftCheat.forceHVRange = 0;
				eftCheat.lineCheck	 = 0;
				ECAT_ModeChange(EFT_MODE);
				show_state_eft();
			}
			else
				show_state_none("Module ERROR","EFT",DISPLAY_LOCAL);
			break;
		case DISPLAY_PQF:
			if(pqfdata.pqfflag)
			{
				show_state_pqfdemo();
				ECAT_ModeChange(PQF_MODE);
			}
			else
				show_state_none("Module ERROR","PQF",DISPLAY_LOCAL);
			break;
		case DISPLAY_ESD:
			show_state_none("Module ERROR","ESD",DISPLAY_LOCAL);
			break;
		case DISPLAY_MEASURE:
			if(peakdata.flag)
			{
				switch(ecatmode)
			   {
					case SRG_MODE:
						keyenable = 1;
						show_state_srgmeas();
						break;
					case EFT_MODE:
						keyenable = 1;
						show_state_eftmeas();
						break;
				}
			}
			else
				show_state_none("Measurement ERROR","V/I CAPABLE",display.previous);
			break;
		case DISPLAY_BAD_AC:
			show_state_bad("      The MAINS Signal is Missing\n\r      at the Inputs to the Coupler");
			break;
		case DISPLAY_BAD_BLUE:
			show_state_bad("      Please Enable the EUT power\n\r      EUT power is required    ");
			break;
		case DISPLAY_BAD_HV:
			show_state_bad("      The E522 HV is Missing\n\r      Please check the lower \n\r       AC input to the E522");
			break;
		case DISPLAY_BAD_CHARGE:
			show_state_bad("       The High Voltage Charge\n\r      Failed Tolerance Limits");
			break;
		case DISPLAY_BAD_RMS:
			show_state_rmsbad();
			break;
		case DISPLAY_BAD_FRQ:
			show_state_bad("       VOLTAGE/FREQUENCY Conflict\n\r       Verify the Following Limits\n\r");
			LCD_puts("           2 MHz - 3.0 kV Max\n\r           1 MHz - 4.4 kV Max\n\r         250 kHz - 8.0 kV Max\n\r");
			break;
		case DISPLAY_BAD_AUX:
			show_state_bad("      There was a PQF AUX fault\n\r      Please Check the Hardware");
			break;
		case DISPLAY_BAD_BURST:
			show_state_bad("     FREQUENCY/DURATION Combination\n\r     Is not Supported");
			break;
		case DISPLAY_CALSRG:
			if(calibration)
			{
				keyenable = 1;
				show_state_calsrg();
			}
			break;
		case DISPLAY_CALEFT:
			if(calibration)
			{
				keyenable = 1;
				show_state_caleft();
			}
			break;
		default:
			show_state_main();
			break;
	}
}

void	show_state_main(void)
{
	LCD_gotoxy(3,1);
	fprintf(DISPLAY,REV_MENU);
	LCD_gotoxy(3,5);
	LCD_puts("Choose Test Type:\n\r\n\r");
	if(calibration)
		LCD_puts("SURGE S/CAL EFT/B E/CAL ESD  _____ PQF");
	else
		LCD_puts("SURGE _____ EFT/B _____ ESD  _____ PQF");
	KEY_led(GO_LED,0);
	KEY_led(STOP_LED,1);

	func_manager = main_funcs;

/*********************************************
	Dummy menu bar
	LCD_puts("_____ _____ _____ _____ ____ _____ _____");
**********************************************/
}

void	FP_UpdateRemote(void)
{
	char	c;

	LCD_gotoxy(0,0);
	fprintf(DISPLAY,"\n\r\n\r\n\r\n\r               R E M O T E");
	LCD_gotoxy(39,7);
}

void	FP_UpdateWarmup(
ushort	time)
{
	LCD_gotoxy( 13, 3 );
	fprintf( DISPLAY, "Starting...%02d", (40-time) / 8 );

	if ( ( !warmup.started ) && ( time != 0 ) )
    {
#ifdef BETA_RELEASE
        size_t timestamp_length = strlen( __DATE__ ) + strlen( __TIME__ ) + 1;  /* add 1 for space between */
        int timestamp_x = ( DISPLAY_WIDTH - timestamp_length ) / 2;
        LCD_gotoxy( timestamp_x, 4 );
        fprintf( DISPLAY,__DATE__ " " __TIME__ );
#endif
        warmup.started = TRUE;
    }
}

void	show_state_bad(
char	*str)
{
	KEY_beep();
	LCD_gotoxy(14,1);
	switch(ecatmode)
	{
		case SRG_MODE:
			LCD_puts("SURGE ERROR\n\r\n\r");
			noneTrack = DISPLAY_SRG;
			break;
		case EFT_MODE:
			LCD_puts(" EFT ERROR\n\r\n\r");
			noneTrack = DISPLAY_EFT;
			break;
		case PQF_MODE:
			LCD_puts(" PQF ERROR\n\r\n\r");
			noneTrack = DISPLAY_PQF;
			break;
		case GHOST_MODE:
			if(emcdata.modeFlag)
			{
				LCD_puts(" EMC ERROR\n\r\n\r");
				noneTrack = DISPLAY_REMOTE;
			}
			break;
	}
	LCD_puts(str);
	func_manager = none_funcs;
}
