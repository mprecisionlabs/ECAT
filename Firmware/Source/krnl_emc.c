#include <stdio.h>
#include <ctype.h>
#include <mriext.h>
#include "timer.h"
#include "switches.h"
#include "intface.h"
#include "tiocntl.h"
#include "module.h"
#include "front.h"
#include "ecat.h"
#include "emc.h"
#include "acrms.h"

extern FILE		*destination;
extern uchar		menukeyenable;
extern uchar		ecatmode;
extern EMCDATA	emcdata;
extern CPLDATA	cpldata;
extern ACDATA		acdata;
extern PQFDATA	pqfdata;
extern MODDATA	moddata[];
extern P_PARAMT	paraml[];

int	EMC_BYpass_pgm(P_PARAMT *params)
{
	uchar	mode;

	if(!emcdata.EP7xFlag)
		return(BAD_VALUE);

	mode = (uchar)(EXTRACT_INT(0) & 0x01);

	/* If same mode then no need to change anything */
	if(emcdata.EP7xBypass == mode)
		return(0);

	blue_off();
	TPU_delay(100);
	if(mode)
	{
		emcdata.EP7xRelay &= ~(RLY_RD20 | RLY_RD21 | RLY_RD23);
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
		TPU_delay(100);

		emcdata.EP7xRelay |= RLY_RD18;
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));

		paraml[0].idata = EMC_SRGEFT;
		EMC_MOde_pgm(paraml);
	}
	else
	{
		emcdata.EP7xRelay &= ~(RLY_RD18);
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
		TPU_delay(100);

		emcdata.EP7xRelay |= (RLY_RD20 | RLY_RD23);
		if(!emcdata.EP7xPhase)
			emcdata.EP7xRelay |= RLY_RD21;
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
	}
	emcdata.EP7xBypass = mode;
	return(0);
}

int	EMC_BYpass_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",emcdata.EP7xBypass);
	return(0);
}

int	EMC_PHase_pgm(P_PARAMT *params)
{
	uchar	mode;

	if(!emcdata.EP7xFlag)
		return(BAD_VALUE);

	mode = (uchar)(EXTRACT_INT(0) & 0x01);

	if(!(moddata[(char)(emcdata.EP7xAddr)].modinfo.typePrimary & TYPE_3PHASE) && (mode))
		return(BAD_VALUE);

	/* If same mode then no need to change anything */
	if(emcdata.EP7xPhase == mode)
		return(0);

	blue_off();
	TPU_delay(100);

	if(mode)
		emcdata.EP7xRelay &= ~(RLY_RD21);
	else
		emcdata.EP7xRelay |= RLY_RD21;
	ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));

	emcdata.EP7xPhase = mode;
	return(0);
}

int	EMC_PHase_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",emcdata.EP7xPhase);
	return(0);
}

int	EMC_EXit_pgm(P_PARAMT *params)
{
	menukeyenable = 1;
	if((emcdata.EOPxFlag) && (!emcdata.EP7xBypass))
	{
/*		blue_off();*/ /* 06/16/95 okay, jimp will check */
		emcdata.EOPxRelay &= ~(RLY_RD23);
		ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
		TPU_delay(100);
		emcdata.EOPxRelay |= RLY_RD22;
		ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
		TPU_delay(1000);
		emcdata.EOPxRelay |= RLY_RD23;
		ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
		emcdata.EOPxSoftStart = TRUE;
	}
	if (emcdata.EP7xFlag)
	{
		paraml[0].idata = EMC_SRGEFT;
		EMC_MOde_pgm(paraml);
	}
	return(0);
}

int	EMC_EUT_pgm(P_PARAMT *params)
{
 	if(emcdata.EP7xAddr == GHOST_BOX)
		return(BAD_VALUE);

	if(EXTRACT_INT(0))
		emcdata.EP7xRelay |= (RLY_RD20);
	else
		emcdata.EP7xRelay &= ~(RLY_RD20);
	ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));

	return(0);
}

int	EMC_REset_pgm(P_PARAMT *params)
{
	paraml[0].idata = 0;
	root_EUT_pgm(paraml);
	EMC_ResetModules(1);
	PQF_ResetModules(1);
	return(0);
}

int	EMC_MOde_pgm(P_PARAMT *params)
{
	if(!emcdata.EP7xFlag)
		return(BAD_VALUE);

	if(emcdata.EP7xAddr == GHOST_BOX)
		return(BAD_VALUE);

	switch(EXTRACT_INT(0))
	{
		case 0:	/* SRG/EFT/PQF	*/
			if(EMC_EP6x100Initialize(EXTRACT_INT(0)))
				return(BAD_VALUE);
			if(((ecatmode == SRG_MODE) || (ecatmode == EFT_MODE)) && (emcdata.EOPxFlag))
				EMC_EOPxInitialize();
			emcdata.mode = EMC_SRGEFT;

			/* When returning from ArbWare to emcdata.mode=EMC_SRGEFT, and
				the ecatmode is already either SRG_MODE or EFT_MODE, then
				reactivate the AC Option.  DON'T reactivate if ecatmode is
				either PQF_MODE or GHOST_MODE. FD 07Sep95 */
			if ( ( ecatmode == SRG_MODE ) || ( ecatmode == EFT_MODE ) )
			{
				RMS_ResetModules();
			}
			break;

		case 1:	/* Flicker		*/
		case 2:	/* Harmonics	*/
			RMS_Stop();
			acdata.acbox = GHOST_BOX;
			if(EMC_EP7xInitialize(EXTRACT_INT(0)))
				return(BAD_VALUE);
			emcdata.mode = EXTRACT_INT(0);
			break;

		default:
			return(BAD_VALUE);
	}
	if(emcdata.EP6xAddr < MAX_MODULES)
	{
		TIO_TIMap(emcdata.EP6xAddr,TILCL0_TI0);
		TIO_TISelect(emcdata.EP6xAddr,TI2_SELECT,ENABLE);
	}
	return(0);
}

void	EMC_FindModules(void)
{
	char		baycnt;
	MODINFO	*modptr;

	emcdata.EP7xFlag = 0;
	emcdata.EP6xFlag = 0;
	emcdata.ERIxFlag = 0;
	emcdata.EOPxFlag = 0;
	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if((modptr->typePrimary & TYPE_EMC) && (modptr->id < MAX_EMCBOXES))
		{
			switch(modptr->id)
			{
				case EP71_BOX:
				case EP72_BOX:
				case EP73_BOX:
				case EP91_BOX:
				case EP92_BOX:
				case EP93_BOX:
				case EP94_BOX:
					emcdata.EP7xFlag = 1;
					break;
				case ERI1_BOX:
				case ERI3_BOX:
					emcdata.ERIxFlag = 1;
					break;
			}
		}
		else
			if((modptr->typePrimary & TYPE_PQF) && (modptr->id < MAX_PQFBOXES))
				emcdata.EP6xFlag = TRUE;
			else
				if((modptr->typePrimary & TYPE_CLAMP) && (modptr->id < MAX_CLAMPBOXES))
					emcdata.EOPxFlag = 1;
	}
}

void	EMC_ResetModules(
uchar	extent)
{
	MODINFO	*modptr;
	char		baycnt;

	if(extent)
	{
		emcdata.mode		= EMC_SRGEFT;				/* Surge mode is default	*/
		emcdata.EP7xBypass= 0;							/* Default Bypass to OFF	*/
		emcdata.EP7xPhase	= 1;							/* Default to Single-phase	*/
		emcdata.EP7xAddr	= GHOST_BOX;
		emcdata.EP6xAddr	= GHOST_BOX;
		emcdata.ERIxAddr	= GHOST_BOX;
		emcdata.EOPxAddr	= GHOST_BOX;
		emcdata.EP7xRelay	= 0x00080000;
		emcdata.EP6xRelay	= 0x00000000;
		emcdata.ERIxRelay	= 0x00000000;
		emcdata.EOPxRelay	= 0x00000000;
		emcdata.EOPxSoftStart	= FALSE;
	}

	for(baycnt = MAX_MODULES-1; baycnt >= 0; baycnt--)
	{
		modptr = &moddata[baycnt].modinfo;
		if(modptr->typePrimary & TYPE_EMC)
		{
			ECAT_TimerSequencer(baycnt,0x0000);
			ECAT_TimerSequencer(baycnt,0x0000);
			ECAT_WriteRelay0(baycnt,0x0000);
			ECAT_WriteRelay1(baycnt,0x0000);
			ECAT_WriteEXPRelay0(baycnt,0x00000000);
			ECAT_WriteEXPRelay1(baycnt,0x00000000);
			if(extent)
				switch(modptr->id)
				{
					case EP71_BOX:
					case EP72_BOX:
					case EP73_BOX:
					case EP91_BOX:
					case EP92_BOX:
					case EP93_BOX:
					case EP94_BOX:
						emcdata.EP7xAddr = baycnt;
						emcdata.EP7xRelay |= RLY_RD19;
						ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
						break;
					case ERI1_BOX:
					case ERI3_BOX:
						emcdata.ERIxAddr = baycnt;
						break;
				}
		}
		else
			if(modptr->typePrimary & TYPE_PQF)
			{
				ECAT_TimerSequencer(baycnt,0x0000);
				ECAT_TimerSequencer(baycnt,0x0000);
				ECAT_WriteRelay0(baycnt,0x0000);
				ECAT_WriteRelay1(baycnt,0x0000);
				ECAT_WriteExpansion(baycnt,0x0000);
				if(extent)
					emcdata.EP6xAddr = baycnt;
			}
			else
				if(modptr->typePrimary & TYPE_CLAMP)
				{
					ECAT_TimerSequencer(baycnt,0x0000);
					ECAT_TimerSequencer(baycnt,0x0000);
					ECAT_WriteRelay0(baycnt,0x0000);
					ECAT_WriteRelay1(baycnt,0x0000);
					ECAT_WriteExpansion(baycnt,0x0000);
					if(extent)
						emcdata.EOPxAddr = baycnt;
				}
	}
}

char	EMC_EP6xAuxInitialize(
char	mode)
{
	if(emcdata.EP7xAddr == GHOST_BOX)
		return(-1);

/*
 *	RESET the PQF module so it can be set up correctly
 *	Reset only if the previous mode was SRG/EFT/PQF mode due to
 *	functional change on 04/26/94
 */
/**********************************
	if(emcdata.mode == EMC_SRGEFT)
	{
		paraml[0].idata = 0;
		EMC_REset_pgm(paraml);
	}
**********************************/

/*
 *	If switching to FLICKER mode, then the following relays in the
 *	EP61 module need to be turned ON at this point:
 *		RD27,RD30	- PQF Output relays
 */
	emcdata.ERIxRelay |= LED_ACTIVE;
	if(mode == EMC_FLICKER)
	{
		if(emcdata.EP6xFlag)
		{
			emcdata.EP6xRelay |= (RLY_RD09 | RLY_RD14 | RLY_RD24);
			ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
			TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
			ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
			TPU_write(1,1);		/* Set it High	*/
			TPU_write(1,0);		/* Set it Low	*/
			TPU_write(1,1);		/* Set it High	*/
			TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
			emcdata.EP6xRelay &= ~(RLY_RD02);
			ECAT_WriteRelay0(emcdata.EP6xAddr,emcdata.EP6xRelay);
			TPU_delay(100);
			emcdata.EP6xRelay |= (RLY_RD27 | RLY_RD30);
			TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
			ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
			TPU_write(1,1);		/* Set it High	*/
			TPU_write(1,0);		/* Set it Low	*/
			TPU_write(1,1);		/* Set it High	*/
			TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
			TPU_delay(100);
		}
		if(moddata[(char)(emcdata.ERIxAddr)].modinfo.id == ERI1_BOX)
			emcdata.ERIxRelay &= ~(RLY_RD26);
		else
			emcdata.ERIxRelay &= ~(RLY_RD19 | RLY_RD22);
		ECAT_WriteRelays(emcdata.ERIxAddr,emcdata.ERIxRelay);
		if((moddata[(char)(emcdata.EP7xAddr)].modinfo.id == EP71_BOX) ||
			(moddata[(char)(emcdata.EP7xAddr)].modinfo.id == EP72_BOX))
			emcdata.EP7xRelay &= ~(RLY_RD22);
		else
			emcdata.EP7xRelay &= ~(RLY_RD23);
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
	}
	else
	{
		/* Harmonics Mode */
		if(moddata[(char)(emcdata.ERIxAddr)].modinfo.id == ERI1_BOX)
			emcdata.ERIxRelay |= RLY_RD26;
		else
			emcdata.ERIxRelay |= (RLY_RD19 | RLY_RD22);
		ECAT_WriteRelays(emcdata.ERIxAddr,emcdata.ERIxRelay);
		if((moddata[(char)(emcdata.EP7xAddr)].modinfo.id == EP71_BOX) ||
			(moddata[(char)(emcdata.EP7xAddr)].modinfo.id == EP72_BOX))
			emcdata.EP7xRelay |= RLY_RD22;
		else
			emcdata.EP7xRelay |= RLY_RD23;
		ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
		TPU_delay(50);
		if(emcdata.EP6xFlag)
		{
			emcdata.EP6xRelay &= ~(RLY_RD27 | RLY_RD30);
			TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
			ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
			TPU_write(1,1);		/* Set it High	*/
			TPU_write(1,0);		/* Set it Low	*/
			TPU_write(1,1);		/* Set it High	*/
			TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
			emcdata.EP6xRelay &= ~(RLY_RD24);
			emcdata.EP6xRelay |= RLY_RD02;
		}
	}

/*
 *	Turn ON the following relays and WAIT for 100 msec:
 *		RD21			- Input to Transformer
 *		RD04			- Coax Voltage I/O Select
 *		RD05			- Coax Voltage Select
 *		RD06			- Coax Current Select
 */
/**** TEST JMM for JIMP on 07/27/94 (Do NOT set RD21) ****/
	if(emcdata.EP6xFlag)
	{
		emcdata.EP6xRelay |= (RLY_RD04 | RLY_RD05 | RLY_RD06);
		emcdata.EP6xRelay &= ~(RLY_RD21);
		ECAT_WriteRelay0(emcdata.EP6xAddr,emcdata.EP6xRelay);
		ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
		TPU_delay(100);

		/*
		 *	Turn ON the following relays:
		 *		RD17			- Active Module LED
		 *		RD9,RD14		- Select AUX / 50% Tap
		 *
		 * 	Note that these relays can be turned on simultaneously because
		 *	there will be NO AC present at this time
		 */
		/**** TEST JMM for JIMP on 09/12/94 (Do NOT set RD24) ****/
		emcdata.EP6xRelay |= (LED_ACTIVE | RLY_RD09 | RLY_RD14);
		ECAT_WriteRelay0(emcdata.EP6xAddr,emcdata.EP6xRelay);
		ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
		TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
		ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
		TPU_write(1,1);		/* Set it High	*/
		TPU_write(1,0);		/* Set it Low	*/
		TPU_write(1,1);		/* Set it High	*/
		TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
		TPU_delay(100);
		/*
		 *	Wait for ~100 msecs and then turn ON the following relays:
		 *		RD18			- EUT Input Contactor
		 */
		emcdata.EP6xRelay |= RLY_RD18;
		ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
	}
	emcdata.modeFlag = 1;
	return(0);
}

char	EMC_EP7xInitialize(
char	mode)
{
	if(emcdata.EP7xAddr == GHOST_BOX)
		return(-1);

	if(EMC_EP6xAuxInitialize(mode))
		return(-2);

/*
 *	Initialize the EP7x module
 *	Turn ON the following relays:
 *		RD17			- Active Module LED
 *		RD19			- Amplifier INPUT contactor
 *
 *	Note that the Amplifier INPUT contactor should already be ON
 *	at this point; but this will be left in just in case
 */
	emcdata.EP7xRelay |= (LED_ACTIVE | RLY_RD19);
	/* Check DIN1 == 0 then error! */

	ECAT_WriteRelays(emcdata.EP7xAddr,emcdata.EP7xRelay);
	emcdata.modeFlag = 1;

	if(pqfdata.ep3flag)
	{
		*pqfdata.ep3relay |=  (LED_ACTIVE | RLY_RD18);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
	if(emcdata.EOPxFlag)
	{
		emcdata.EOPxRelay &= ~(RLY_RD22);
		emcdata.EOPxRelay |= LED_ACTIVE;
		ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
	}
/*
 *	If running under FLICKER or HARMONICS mode, then the
 *	following relays need to be set in the default coupler:
 *		RD23,RD27	- Choke shorting relays (CPL_EMCMODE)
 *		RD17			- Active Module LED (LED_ACTIVE)
 */
	if(cpldata.srcaddr != GHOST_BOX)
	{
		*cpldata.srcrelay |= (CPL_EMCMODE | LED_ACTIVE);
		ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
	}
	return(0);
}

char	EMC_EP6x100Initialize(
char	mode)
{
/*
 *	Make sure that RD20 is OFF at this point to keep the
 *	PQF AUXILIARY input happy when we RESET the PQF module
 */
/****************************************
	emcdata.EP7xRelay &= ~(RLY_RD20);
	ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
****************************************/

/*
 *	RESET the PQF module so it can be set up correctly
 */
/**********************************
	paraml[0].idata = 0;
	EMC_REset_pgm(paraml);
**********************************/

/*
 *	ERIx Initialization
 *	Turn ON the following relays:
 *		RD17		- Active Module LED
 *		RD26		- Impedance bypass	(ERI1 ONLY)
 *		RD19,22	- Impedance bypass	(ERI3 and others)
 */
	if(moddata[(char)(emcdata.ERIxAddr)].modinfo.id == ERI1_BOX)
		emcdata.ERIxRelay |= (LED_ACTIVE | RLY_RD26);
	else
		emcdata.ERIxRelay |= (LED_ACTIVE | RLY_RD19 | RLY_RD22);
	ECAT_WriteRelays(emcdata.ERIxAddr,emcdata.ERIxRelay);

/*
 *	Turn ON the following relays:
 *		RD17			- Active Module LED
 *		RD18			- EUT Input Contactor
 */
	if(emcdata.EP6xFlag)
	{
		emcdata.EP6xRelay |= (LED_ACTIVE | RLY_RD18);
		ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
		TPU_delay(200);
	
		/*
		 *		Wait for ~200 msecs and then turn ON the following relays:
		 *			RD21,RD29	- PQF Output Enable
		 */
		emcdata.EP6xRelay |= (RLY_RD21 | RLY_RD29);
		TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
		ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
		ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
		TPU_write(1,1);		/* Set it High	*/
		TPU_write(1,0);		/* Set it Low	*/
		TPU_write(1,1);		/* Set it High	*/
		TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
	}
/*
 *	If running under SURGE/EFT mode, then the following relays
 *	need to be set in the default coupler:
 *		RD17			- Active Module LED
 */
	if(cpldata.srcaddr != GHOST_BOX)
	{
		*cpldata.srcrelay |= LED_ACTIVE;
 		ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
	}
	emcdata.modeFlag = 1;
 	return(0);
}

char	EMC_EOPxInitialize(void)
{
	return(0);
}
/******* << END KRNL_EMC.C >> *******/
