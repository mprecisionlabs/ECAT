#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "front.h"
#include "module.h"
#include "ecat.h"
#include "intface.h"
#include "timer.h"
#include "eft.h"
#include "field.h"
#include "ehv.h"
#include "acrms.h"
#include "tiocntl.h"

extern uchar		keyenable;
extern uint	 	ghostrelay;
extern int			current_field;
extern uchar		calibration;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern EFTDATA	eftdata;
extern EFTCHEAT	eftCheat;
extern DISDATA	display;
extern EHVDATA	ehvdata;
extern ONE_FIELD	field_list[];
extern volatile ushort	ldo_state;
extern volatile ushort	TPU_rams[16][8];
extern TPU_STRUCT	*TPU;

ushort	EFT_412AngleFix;
uint		PostLSDelay;
uint		PreLSDelay;
uint		base_val;
uint		burst_set;
uint		burst_delay_ch;
uint		burst_clock;

const uint chirpp[] =
{
	16, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
	36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 51, 52, 53,
	55, 56, 57, 58, 60, 67, 74, 80, 92,104,120,132,144,160,172,184,200,0
};
const uint chirpn[] =
{
	275, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 
	2,3, 2, 2, 2, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0
}; 

const uint	freqs_sets[] =
{
	4000000, 4000000, 4000000, 1000000, 250000, 62500
};

const uint	freqs_clockv[] =
{
	SEQ_F_4M, SEQ_F_4M, SEQ_F_4M, SEQ_F_1M, SEQ_F_250K, SEQ_F_62K
};

void	EFT_refresh(void);

void	EFT_FindModules(void)
{
	char		baycnt;
	MODINFO	*modptr;
	
	eftdata.eftflag = 0;
	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if((modptr->typePrimary & TYPE_EFT) && (modptr->id < MAX_EFTBOXES))
			eftdata.eftflag = TRUE;
	}
}

void	EFT_ResetModules(
uchar	extent)
{
	char		tvar;
	MODINFO	*modptr;

	if(extent)
	{
		eftdata.relay		= &ghostrelay;
		eftdata.voltage	= EFT_MINVLT;
		eftdata.frequency	= EFT_MINFRQ;
		eftdata.unit		= 0;
		eftdata.duration	= 15;
		eftdata.period		= EFT_MINPER;
		eftdata.eftaddr	= GHOST_BOX;
		eftdata.output		= 0;
		eftdata.active		= 0;
		eftdata.setup		= 0;
		eftdata.mode		= 1;							 /* CON */
		eftdata.waveform	= 0;
		eftdata.range		= 0;
		cpldata.cplhigh	= 0;
	}

	for(tvar = MAX_MODULES-1; tvar >= 0; tvar--)		
	{
		modptr = &moddata[tvar].modinfo;
		if(modptr->typePrimary & TYPE_EFT)
		{
			if(modptr->id == E412_BOX)
				moddata[tvar].relay &= (CPL_412EUT | AC_IRANGE);
			else
				if(modptr->typePrimary & TYPE_CPLGEN)
					moddata[tvar].relay &= AC_IRANGE;
				else
					moddata[tvar].relay = 0x00000000;
			if(modptr->id >= E422_BOX)
				moddata[tvar].expansion &= CPL_422EUT;
			else 
				moddata[tvar].expansion &= 0x0000;
			ECAT_WriteRelays(tvar,moddata[tvar].relay);
			ECAT_WriteEXPRelay0(tvar,moddata[tvar].expansion);
			if(extent)
				eftdata.eftaddr = tvar;
		}
	}
}

uchar	EFT_StartSystem(
uchar	mode)
{
	MODDATA	*modptr;
	uint		time_dly;
	int		ox,oy;
	uchar		tvar;

	if(eftdata.eftaddr >= MAX_MODULES)
		return(ERR_SRGNET);

	modptr = &moddata[(char)(eftdata.eftaddr)];
	if(!(modptr->modinfo.typePrimary & TYPE_EFT))
		return(ERR_SRGNET);

	ox = LCD_getx();
	oy = LCD_gety();
	if(mode == 0)
	{
		if(eftdata.active)
			return(ERR_SRGIDLE);

		if(!eftCheat.lineCheck)
		{
			if(display.current == DISPLAY_EFT)
				tvar = ECAT_CheckEUT(PHASE_TPU,1);
			else
				tvar = ECAT_CheckEUT(PHASE_TPU,0);
			if(tvar)
				return(tvar);
		}
		if(display.current == DISPLAY_EFT)
		{
			if(field_list[current_field].left)
				(*field_list[current_field].left)(&field_list[current_field]);
			if(field_list[current_field].entered)
				(*field_list[current_field].entered)(&field_list[current_field]);
		}
		KEY_led(STOP_LED,0);
		KEY_led(GO_LED,1);
		keyenable = 0;
/*
 *	Set up the following EFT relays at this time:
 *		- POLARITY
 *		- OUTPUT
 *		- CHIRP / NON-CHIRP
 *		- RISETIME ACCELERATOR
 *		- COUPLING RELAYS (If Applicable)
 */
		TIO_TOMap(eftdata.eftaddr,TO0_TOLCL0);
		*eftdata.relay |= EFT_HIVENABLE;
		ECAT_WriteRelays(eftdata.eftaddr,(*eftdata.relay));
 		if(eftdata.output)
			TPU_write(CLAMP_TPU,1);						/* RD24 - OUT2 */
		else
			TPU_write(CLAMP_TPU,0);

		if(cpldata.outaddr != GHOST_BOX)
		{
			if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_EFT)
			{
				if(moddata[(char)(cpldata.srcaddr)].modinfo.id >= E422_BOX)
					ECAT_WriteEXPRelay0(cpldata.srcaddr,*cpldata.expansion);
				else
					ECAT_WriteRelay0(cpldata.outaddr,*cpldata.outrelay);
			}
			else
				ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		}

		if(!burst_set)
		{
			if(display.current == DISPLAY_EFT)
			{
				LCD_gotoxy(FLD_EFTINFOx,FLD_EFTINFOy);
				LCD_puts("Computing");
			}
			burst_set = 0;
			burst_delay_ch = -1;
			eftdata.active = 0;

			if(!eftdata.waveform)
				base_val = 0x40;				/* RLY_RD30	*/
			else
				base_val = 0x02;				/* RLY_RD25	*/

			/* Activate EFT_ACCRISE relay if coupling to more than one line */
			if ( cpldata.multihigh > 1 ) /*|| ( eftdata.need_EFT_ACCRISE_relay ) )*/
				*eftdata.relay |= EFT_ACCRISE;
			else
				*eftdata.relay &= ~EFT_ACCRISE;

			if(*eftdata.relay & EFT_ACCRISE)
				base_val |= 0x04;				/* RLY_RD26 */

			if(*eftdata.relay & EFT_LOWRANGE)
				base_val |= 0x10;				/* RLY_RD28 */
			else
				if(*eftdata.relay & EFT_HIGHRANGE)
					base_val |= 0x20;			/* RLY_RD29 */

/*
 * Modification: JMM (08/17/93)
 * If the EFT Module is an E412 then the LS angle is wrong because
 * of a hardware inconsistency; so it must be translated 180 degrees
 */
			if(moddata[(char)(eftdata.eftaddr)].modinfo.id == E412_BOX)
				EFT_412AngleFix = (cpldata.angle + 180) % 360;
			else
				EFT_412AngleFix = cpldata.angle;
 			if(eftdata.waveform)
			{
				PreLSDelay	= 2096;
				PostLSDelay	= 0;
				setup_chirp();
				burst_set = 2;
			}
			else
			{
				PreLSDelay	= (uint)(((float)(1)/(float)(eftdata.frequency))/TPU_TICK); 
				PostLSDelay	= (uint)(((float)(eftdata.duration)/TPU_TICK)/(float)(eftdata.frequency));
				setup_burst(eftdata.frequency,eftdata.duration);
				if(!burst_set)
				{
					if(display.current == DISPLAY_EFT)
						display.request = DISPLAY_BAD_BURST;
					return(ERR_EFTBURST);
				}
			}
		}

		EFT_HVon();
		if(display.current == DISPLAY_EFT)
		{
			LCD_gotoxy(FLD_EFTINFOx,FLD_EFTINFOy);
			LCD_puts("Charging ");
		}
		if(moddata[(char)(eftdata.eftaddr)].modinfo.id == E421P_BOX)
			TPU_delay(15000);
		else
			TPU_delay(5000);
		LCD_gotoxy(ox,oy);
		eftdata.setup = 1;
		return(0);
	} /* mode == 0 */

	/* Otherwise DO the actual bursting! */
	if(!eftdata.setup)
		return(ERR_SRGRDY);

	time_dly = eftdata.period * 2096;
	if(display.current == DISPLAY_EFT)
	{
		LCD_gotoxy(FLD_EFTINFOx,FLD_EFTINFOy);
		LCD_puts("Running  ");
	}
	if(eftdata.mode == 1)	/* CON */
	{
		eftdata.active = 1;
		burst_delay_ch = TPU_periodic_int(time_dly,DO_BURST);
	}
	else
	{
		eftdata.active = 1;
		DO_BURST();
		TPU_delay(200);
		EFT_StopSystem();
	}
	LCD_gotoxy(ox,oy);
	return(0);
}

uchar	EFT_StopSystem(void)
{
	int	ox,oy;

	ox = LCD_getx();
	oy = LCD_gety();
	KEY_led(GO_LED,0);
	KEY_led(STOP_LED,1);
	keyenable = 1;
	eftdata.active	= 0;
	eftdata.setup	= 0;
	burst_set = 0;
	if(burst_delay_ch != -1)
		TPU_kill(burst_delay_ch);
	burst_delay_ch = -1;
	if(eftdata.eftaddr != GHOST_BOX)
	{
		ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_RESET|base_val));
		TPU_toggle(SEQ_TPU,2);
	}
	if(display.current == DISPLAY_EFT)
	{
		LCD_gotoxy(FLD_EFTINFOx,FLD_EFTINFOy);
		LCD_puts("IDLE     ");
	}
	LCD_gotoxy(ox,oy);
	/*EFT_ClearCouplerHardware();*/
	EFT_HVoff();
}

void	setup_burst(
uint	f,
uint	n)
{
	int	local_f;
	int	pulsen;
	float	pulse_t;
	int	i;
	int	nt;
	uint	seqRamSize;

	if(eftdata.eftaddr == GHOST_BOX)
		return;

	if(moddata[eftdata.eftaddr].modinfo.id == E421P_BOX)
		seqRamSize = SEQ_RAM_SIZE1;
	else
		seqRamSize = SEQ_RAM_SIZE0;

	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_NORM|base_val));
	TPU_toggle(SEQ_TPU,2);

	/* freqs_sets[local_f] / f	= bits for 1 pulse */
	/*	32760 = RAM size */

	local_f = -1;
	for(i = 5;((i >= 0) && (local_f == -1)); i--)
		if(((freqs_sets[i] / f) > 2) && ((freqs_sets[i] % f) == 0))
			local_f = i;

	if((local_f >= 0) && ((freqs_sets[local_f]*((float)n/(float)f)) > seqRamSize))
		local_f = -1;

	if(local_f < 0)
	{
		for(i = 0; ((local_f == -1) && (i <= 5)); i++)
			if((freqs_sets[i]*((float)n / (float)f)) <= seqRamSize)
				local_f = i;
	}

	if(local_f < 0)
	{
		return;
	}

	seq_reset(base_val);
	fill_seq_ram(base_val|0x00);
	seq_reset(base_val|0x00);
	write_seq_ram(base_val);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x80));

	/* Starting RAM load */
	i = 0;
	pulsen = 0;
	while(pulsen < n)
	{
		pulsen++;												 /*pulse number         */ 
		pulse_t = (float)pulsen / (float)f;				 /*time of pulsen pulses*/
		nt = (int)(pulse_t * (float)freqs_sets[local_f]) - i;

		TPU_toggle(SEQ_TPU,nt);			/* nt = trans. time of a pulse       */
		i += nt;							  	/* i  = trans. time of pulsen pulses */

		write_seq_ram(base_val|0x80);
		ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x80));

		if(pulsen == n)
		{
			TPU_toggle(SEQ_TPU,1);
			ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x00));
		}
	}

	/* Done with RAM load	*/
	write_seq_ram(base_val|0x00);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x00));
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_WIPE|base_val|0x00));
	TPU_toggle(SEQ_TPU,5);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_WIPE|base_val|0x01));
	TPU_toggle(SEQ_TPU,10);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x01));
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x01));
	seq_reset(base_val|0x00);
	burst_clock = freqs_clockv[local_f];
	burst_set = 1;
}

void	seq_reset(
uchar	dat)
{
	ushort	tempv;

	if(eftdata.eftaddr == GHOST_BOX)
		return;

	tempv = (SEQ_F_TPU | SEQ_M_RESET | (dat & 0xff));		/* Reset Counter	*/
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
	TPU_toggle(SEQ_TPU,2);
}

void	write_seq_ram(
uchar	dat)
{
	ushort	tempv;

	if(eftdata.eftaddr == GHOST_BOX)
		return;

	tempv = (SEQ_F_TPU| SEQ_M_LRAW | (dat & 0xff));
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
	tempv = (SEQ_F_TPU| SEQ_M_INCAD | (dat & 0xff));
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
}

void	fill_seq_ram(
uchar	dat)
{
	ushort	tempv;

	if(eftdata.eftaddr == GHOST_BOX)
		return;

	seq_reset(dat);
	tempv = SEQ_F_TPU | SEQ_M_WIPE | (dat & 0xff);		/* AUTO WRITE mode	*/
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
	tempv = SEQ_F_4M | SEQ_M_WIPE | (dat & 0xff); 		/* AUTO WRITE @ 4Mhz	*/
	ECAT_TimerSequencer(eftdata.eftaddr,tempv);
	TPU_delay(100);
	seq_reset(dat);
}

void	setup_chirp(void)
{
	int	i,j;

	if(eftdata.eftaddr == GHOST_BOX)
		return;

	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_NORM|base_val));
	TPU_toggle(SEQ_TPU,2);
	seq_reset(base_val);
	fill_seq_ram(base_val|0x00);
	seq_reset(base_val|0x00);
	write_seq_ram(base_val);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x80));
	i = 0;
	TPU_toggle(SEQ_TPU,chirpp[i]-1);
	write_seq_ram(base_val|0x80);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x80));
 
	while(chirpn[i] != 0)
	{
		j = chirpn[i];
		while(j)
		{
			TPU_toggle(SEQ_TPU,chirpp[i]);
			write_seq_ram(base_val|0x80);
			ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x80));
			j--;
		}
		i++;
	}
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val));
	TPU_toggle(SEQ_TPU,1);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val));
	write_seq_ram(base_val|0x00);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x00));
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_WIPE|base_val|0x00));
	TPU_toggle(SEQ_TPU,5);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_WIPE|base_val|0x01));
	TPU_toggle(SEQ_TPU,10);
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x01));
	ECAT_TimerSequencer(eftdata.eftaddr,(SEQ_F_TPU|SEQ_M_INCAD|base_val|0x01));
	seq_reset(base_val|0x00);
	burst_clock = SEQ_F_4M;
	burst_set = 2;
}

void TPU_toggle(
uchar	ch,
int	repeat)
{
	uint	offset;
	uint	v1;
	uint	v2;
	uint	mask;
	volatile ushort *p2;
	volatile ushort *p1;

	mode[ch] = OUT;
	p2 = &(TPU_rams[ch][1]);
	TPU_channel_init(ch,0x08,2,0,3,0);
	offset= 1 - (ch / 8);
	mask	= ~(3 << ((ch % 8) * 2));
	v1 = (1) << ((ch % 8) * 2);
	v2 = (2) << ((ch % 8) * 2);
	p1 = &(TPU->HSSRR[offset]);
	for(repeat; repeat > 0; repeat--)
	{
		while((*p2) & 0x8000)
			;
		*p1 = (*p1 & mask) | v1;
		while(((*p2) & 0x8000) == 0)
			;
		*p1 = (*p1 & mask) | v2;
	}
}

void	def_proc(void)
{
	/* NULL function for TPU_one_shot (for now) */
}

void	EFT_LineSyncClean();

void	EFT_PostLineSync(void)
{
	TPU_delay_int(PostLSDelay,EFT_LineSyncClean);
}

void	EFT_LineSyncClean()
{
	TPU_write(SEQ_TPU,0);
	if(burst_set == 2)
		TPU_write(CHIRP_TPU,0);
}

void EFT_PostChirpLineSync(void)
{
	TPU_one_shot(SEQ_TPU,29000,1,EFT_PostLineSync);
}

void	DO_BURST(void)
{
	EFT_refresh();

	if(eftdata.active)
	{
		seq_reset(0);
		if(eftdata.eftaddr != GHOST_BOX)
			if((cpldata.phasemode) && (!eftCheat.lineCheck))
			{
				ECAT_TimerSequencer(eftdata.eftaddr,(burst_clock|SEQ_M_CST|base_val));
				if(burst_set == 2)		/* Chirp Linesync		*/
					TPU_ECAT_phase(PHASE_TPU,CHIRP_TPU,EFT_412AngleFix,PreLSDelay,1,EFT_PostChirpLineSync);
				else							/* Normal Linesync	*/
					TPU_ECAT_phase(PHASE_TPU,SEQ_TPU,EFT_412AngleFix,PreLSDelay,1,EFT_PostLineSync);
			}
			else
			{					/* Not Linesync */
				if(burst_set == 2)
					TPU_one_shot(CHIRP_TPU,29000,1,def_proc);
				ECAT_TimerSequencer(eftdata.eftaddr,(burst_clock|SEQ_M_CS|base_val));
			}
	}
}

void	EFT_HVoff(void)
{
	ECAT_HVProgram(0,0);
	if((moddata[eftdata.eftaddr].modinfo.id == E421P_BOX) &&
		(moddata[ehvdata.network].modinfo.id == EHV10_BOX))
		EHV_HVProgram(0,0);

  if((eftdata.eftaddr != cpldata.srcaddr) || (cpldata.eutblue == OFF))
		*eftdata.relay &= ~(LED_VOLTAGE);
  *eftdata.relay &= ~(EFT_HIVENABLE);
	if(eftdata.eftaddr != GHOST_BOX)
		ECAT_WriteRelay1(eftdata.eftaddr,((*eftdata.relay) >> 16));
	eftdata.hvon = 0;
	TPU_delay(100);
}

void	EFT_HVon(void)
{
	ushort	dacval;
	float    voltage;

	*eftdata.relay |= (LED_VOLTAGE | EFT_HIVENABLE);
	if(eftdata.eftaddr != GHOST_BOX)
		ECAT_WriteRelay1(eftdata.eftaddr,((*eftdata.relay) >> 16));
	ldo_state &= ~(0x0101);
	(*(ushort *)0x5D000) = ldo_state;
	if(!calibration)
	{
		dacval = 0;
		if(eftdata.eftaddr != GHOST_BOX)
			dacval = EFT_CalCompute(&moddata[(char)(eftdata.eftaddr)].modinfo.calinfo,eftdata.voltage,eftdata.range);
		voltage = (dacval * 10000) / 4095;
	}
	else
	{
		if(eftdata.range)
			voltage = (float)eftdata.voltage*2500.0/(1.6*6000);  /* 2500 * 1.6 */
		else
		   voltage = (float)eftdata.voltage*2500.0 /6000.0;
	}
	ECAT_HVProgram((int)voltage,1);
	if((moddata[eftdata.eftaddr].modinfo.id  == E421P_BOX) &&
		(moddata[ehvdata.network].modinfo.id == EHV10_BOX))
		EHV_HVProgram((int)voltage,1);

	eftdata.hvon = 1;
}

void EFT_refresh(void)
{
	if(cpldata.srcaddr != GHOST_BOX)
	{
		if(moddata[(char)(cpldata.srcaddr)].modinfo.id >= E422_BOX)
			ECAT_WriteEXPRelay0(cpldata.srcaddr,*cpldata.expansion);
	}
  ECAT_WriteRelays(eftdata.eftaddr,(*eftdata.relay));

	if(eftdata.output)
		TPU_write(CLAMP_TPU,1);						/* RD24 - OUT2 */
	else
		TPU_write(CLAMP_TPU,0);
}
