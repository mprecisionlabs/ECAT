#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "tiocntl.h"
#include "module.h"
#include "data.h"
#include "math.h"

#define CALIBRATION_NORM    6000
#define E503A_ADJUSTMENT_a  0.5     // for E503A supplemental cal adjustment (empirically determined)
#define E503A_ADJUSTMENT_b  33      // for E503A supplemental cal adjustment (empirically determined)
#define E503A_ADJUSTMENT_c  3000    // for E503A supplemental cal adjustment (empirically determined)
#define E503A_ADJUSTMENT_d  0.35    // for E503A supplemental cal adjustment (empirically determined)
#define E503A_ADJUSTMENT_e  15      // for E503A supplemental cal adjustment (empirically determined)
#define E503A_ADJUSTMENT_f  8000    // for E503A supplemental cal adjustment (empirically determined)
#define E503A_CAL_VALUE_MIN 4000    // for E503A negative cal adjustment validation
#define E503A_CAL_VALUE_MAX 8000    // for E503A negative cal adjustment validation

#define E507_CAL_VALUE_MIN -1000    // for E507 voltage offset adjustment validation
#define E507_CAL_VALUE_MAX 1000     // for E507 voltage offset adjustment validation

#define PROPORTIONAL_ADJUSTMENT_DIVISOR 10  // proportional_adjustment is stored at percentage times ten
#define PROPORTIONAL_ADJUSTMENT_MAX     (10 * PROPORTIONAL_ADJUSTMENT_DIVISOR)  // max adjustment +/- 10.0%

// Bit definitions for E507/E52x HV supply DAC programming
#define E52xDAC_LOAD    (1 << 15)
#define E52xDAC_DATA_IN (1 << 14)
#define E52xDAC_CLOCK   (1 << 13)

extern volatile ushort		ldo_state;
extern volatile ushort		PEAK_LD5;
extern volatile CHGDATA	chgdata;

extern FILE		*destination;
extern MODDATA	moddata[];
extern SRGDATA	srgdata;
extern CPLDATA	cpldata;
extern uint		ghostrelay;
extern uchar		calibration;
extern P_PARAMT	paraml[3];

extern WAVDATA	wavdata[];
extern uchar		cplAdjust[][MAX_LMODES];

extern char	SRG_GetCouplerType(uchar);

SRGCHEAT	srgCheat;

/*
 *	Returns:
 *		 0 = Successful
 *		-1 = INVALID Coupler Module
 *		-2 = INVALID Coupling Settings
 */
char    SRG_VerifyCoupling(
                          uchar   cplhi,
                          uchar   cpllo)
{
    if ( cpldata.outaddr > MAX_MODULES )
        return(-1);

    /* No BITS can be DOUBLY set */
    if ( (cplhi & cpllo) != 0 )
        return(-2);

    /* Neither can be ZERO */
    if ( (cplhi == 0) || (cpllo == 0) )
        return(-2);

    /* No MULTIPLE bits in LOW coupling	*/
    if ( (cpllo != CPL_L1BIT) && (cpllo != CPL_L2BIT) && (cpllo != CPL_L3BIT) &&
         (cpllo != CPL_NUBIT) && (cpllo != CPL_PEBIT) )
        return(-2);

    /* HIGH cannot couple to PE */
    if ( cplhi & CPL_PEBIT )
        return(-2);

    /* If SINGLE-PHASE and NOT DATA coupler, LOW cannot couple to L1 */
    if ( !(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & (TYPE_3PHASE | TYPE_CPLDATA)) )
        if ( cpllo == CPL_L1BIT )
            return(-2);

    /* If SINGLE-PHASE and NOT DATA coupler, cannot couple to L3,PE lines */
    if ( !(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & (TYPE_3PHASE | TYPE_CPLDATA)) )
        if ( (cpllo == CPL_L3BIT) || (cpllo == CPL_NUBIT) || (cplhi & (CPL_L3BIT | CPL_NUBIT)) )
            return(-2);

    /* If E508_BOX then only THREE coupling modes are VALID */
    if ( (srgdata.address < MAX_MODULES) && (moddata[(char)(srgdata.address)].modinfo.id == E508_BOX) )
        if ( (cpllo != CPL_PEBIT) || (cplhi & (CPL_L3BIT | CPL_NUBIT)) )
            return(-2);

    /* If E505B_BOX then only ONE coupling mode is VALID (L1/L2) */
    if ( (srgdata.address < MAX_MODULES) && (moddata[(char)(srgdata.address)].modinfo.id == E505B_BOX) )
        if ( (cpllo != CPL_L2BIT) || (cplhi != CPL_L1BIT) )
            return(-2);

    return(0);
}

/*
 *	Returns:
 *		 0 = Successful
 *		-1 = INVALID Coupler Module
 *		-2 = INVALID Coupling Settings
 *		-4 = INVALID Surge network
 */
char	SRG_SetCouplerHardware(void)
{
	MODDATA	*modptr;
    uchar wavesave;

    if(srgdata.address >= MAX_MODULES)
		return(-4);

    modptr = &moddata[(char)(srgdata.address)];
	if(!(modptr->modinfo.typePrimary & TYPE_SRG))
		return(-4);

	if(SRG_VerifyCoupling(cpldata.cplhigh,cpldata.cpllow))
		return(-2);

    if ( moddata[cpldata.outaddr].modinfo.id == E505B_BOX )
        return 0;  // E505B doesn't use coupler relay drivers (because coupler is integrated with surge module)

    /* If the surge network is E501-like and the waveform is set for the 02/12 OHM AUTO wave,
        then the "real" waveform is chosen according to line coupling selections.
        This duplicates logic which serves the same purpose in SRG_SurgeStart(). */ 
    wavesave = srgdata.waveform;
    switch ( modptr->modinfo.id )
    {
        case E501_BOX:
        case E501A_BOX:
        case E501B_BOX:
        case E510_BOX:
        case E510A_BOX:
        case E521S_BOX:
        case E522S_BOX:
            if ( srgdata.waveform == E501_0212OHM )
                if ( (cpldata.cpllow & CPL_PEBIT) )
                    srgdata.waveform = E501_12OHM;
                else
                    srgdata.waveform = E501_02OHM;
            break;
    }

	/* High voltage couplers (i.e. E522C, E521C) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLHV)
	{
		*cpldata.outrelay &= ~(RLY_RD18 | RLY_RD19 | RLY_RD20 | RLY_RD21 | RLY_RD23 |
									  RLY_RD24 | RLY_RD25 | RLY_RD27 | RLY_RD28 | RLY_RD29);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		TPU_delay(100);
		switch(cpldata.cpllow)
		{
			case CPL_L1BIT:
				*cpldata.outrelay |= RLY_RD27;
				break;
			case CPL_L2BIT:
				*cpldata.outrelay |= RLY_RD20;
				break;
			case CPL_L3BIT:
				*cpldata.outrelay |= RLY_RD21;
				break;
			case CPL_NUBIT:
				*cpldata.outrelay |= RLY_RD25;
				break;
			case CPL_PEBIT:
				*cpldata.outrelay |= RLY_RD29;
				break;
		}
     ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
     TPU_delay(75);
		if(cpldata.cplhigh & CPL_L1BIT)
		{
			*cpldata.outrelay |= RLY_RD18;
			ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
			TPU_delay(75);
		}
		if(cpldata.cplhigh & CPL_L2BIT)
		{
			*cpldata.outrelay |= RLY_RD19;
			ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
			TPU_delay(75);
		}
		if(cpldata.cplhigh & CPL_L3BIT)
		{
			*cpldata.outrelay |= RLY_RD24;
			ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
			ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
			TPU_delay(75);
		}
		if(cpldata.cplhigh & CPL_NUBIT)
		{
			*cpldata.outrelay |= RLY_RD28;
			ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
			ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
			TPU_delay(75);
		}
		if(cpldata.multihigh==0)
		{
			switch(modptr->modinfo.id)
			{
				case E501_BOX:
				case E501A_BOX:
                case E501B_BOX:
				case E510_BOX:
				case E510A_BOX:
					if(srgdata.waveform == E501_02OHM)
						*cpldata.outrelay |= CPL_SRGCAPS;
					else
						if(srgdata.waveform == E501_12OHM && !(cpldata.cpllow & CPL_PEBIT))
							*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E521S_BOX:
				case E522S_BOX:
					if(srgdata.waveform == E501_02OHM)
						*cpldata.outrelay |= (CPL_SRGCAPS | RLY_RD23);
					else
						if(srgdata.waveform == E501_12OHM && !(cpldata.cpllow & CPL_PEBIT))
						{
							if((cpldata.cpllow & CPL_L1BIT) || (cpldata.cpllow & CPL_L2BIT))
								*cpldata.outrelay |= CPL_SRGCAPS;
							else
								*cpldata.outrelay |= RLY_RD23;
						}
					break;
				case E503_BOX:
                case E503A_BOX:
					if(!(cpldata.cpllow & CPL_PEBIT))
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E511_BOX:
					*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E505_BOX:
					if(srgdata.waveform == E505_2500V)
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E505A_BOX:
					if(srgdata.waveform == E505A_2500V)
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E506_BOX:
					if((srgdata.waveform == E506_2500V || srgdata.waveform == E506_5000V))
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
			}
		}
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		TPU_delay(200);
		return(0);
	}

	/* Standard AC Couplers (i.e. E551, E4554) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLLINE)
	{
		*cpldata.outrelay &= ~(CPL_CPLMASK);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		TPU_delay(200);
		*cpldata.outrelay |= CPL_SRGMODE;
		if(cpldata.cplhigh & CPL_L1BIT)
			*cpldata.outrelay |= CPL_SRGL1HI;
		if(cpldata.cplhigh & CPL_L2BIT)
			*cpldata.outrelay |= CPL_SRGL2HI;
		if(cpldata.cplhigh & CPL_L3BIT)
			*cpldata.outrelay |= CPL_SRGL3HI;
		if(cpldata.cplhigh & CPL_NUBIT)
			*cpldata.outrelay |= CPL_SRGNUHI;
		if(cpldata.cpllow & CPL_L2BIT)
			*cpldata.outrelay |= CPL_SRGL2LO;
		if(cpldata.cpllow & CPL_L3BIT)
			*cpldata.outrelay |= CPL_SRGL3LO;
		if(cpldata.cpllow & CPL_NUBIT)
			*cpldata.outrelay |= CPL_SRGNULO;
		if(cpldata.cpllow & CPL_PEBIT)
			*cpldata.outrelay |= CPL_SRGPELO;
		if(cpldata.multihigh==0)
		{
			switch(modptr->modinfo.id)
			{
				case E501_BOX:
				case E501A_BOX:
                case E501B_BOX:
				case E510_BOX:
				case E510A_BOX:
					if(srgdata.waveform == E501_02OHM)
						*cpldata.outrelay |= CPL_SRGCAPS;
					else
						if(srgdata.waveform == E501_12OHM && !(cpldata.cpllow & CPL_PEBIT))
							*cpldata.outrelay |= CPL_SRGCAPS;
					break;
                case E503_BOX:
                case E503A_BOX:
					if(!(cpldata.cpllow & CPL_PEBIT))
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E511_BOX:
					*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E505_BOX:
					if(srgdata.waveform == E505_2500V)
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
                case E505A_BOX:
					if(srgdata.waveform == E505A_2500V)
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
				case E506_BOX:
					if((srgdata.waveform == E506_2500V || srgdata.waveform == E506_5000V))
						*cpldata.outrelay |= CPL_SRGCAPS;
					break;
			}
		}
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		TPU_delay(200);
		return(0);
	}

	/* Data Couplers (i.e. E571) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
	{
		*cpldata.expansion = 0x00000000;
		*cpldata.outrelay &= ~(CPL_DATA_MASK0);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
		ECAT_WriteEXPRelay1(cpldata.outaddr,(*cpldata.expansion >> 16));
		TPU_delay(25);
		*cpldata.expansion = RLY_DATA_ENABLE;
		switch(modptr->modinfo.id)
		{
            case E502A_BOX:
            case E502B_BOX:
                *cpldata.expansion |= RLY_DATA_E502A;
            case E501_BOX:
            case E501A_BOX:
            case E501B_BOX:
            case E510_BOX:
            case E510A_BOX:
                if ( cpldata.cplhigh & CPL_L1BIT )
                {
                    *cpldata.expansion |= CPL_DATA_S1HI0;
                    if ( (cpldata.cmode) && (cpldata.multihigh!=0) )
                    {
                        if ( cpldata.multihigh == 2 )
                            *cpldata.expansion |= (RLY_RD13 | RLY_RD17);
                        if ( cpldata.multihigh == 3 )
                            *cpldata.expansion |=  RLY_RD17;
                    }
                }
                if ( cpldata.cplhigh & CPL_L2BIT )
                {
                    *cpldata.expansion |= CPL_DATA_S2HI0;
                    if ( (cpldata.cmode) && (cpldata.multihigh!=0) )
                    {
                        if ( cpldata.multihigh == 2 )
                            *cpldata.expansion |= (RLY_RD09 | RLY_RD12);
                        if ( cpldata.multihigh == 3 )
                            *cpldata.expansion |=  RLY_RD09;
                    }
                }
                if ( cpldata.cplhigh & CPL_L3BIT )
                {
                    *cpldata.expansion |= CPL_DATA_S3HI0;
                    if ( (cpldata.cmode) && (cpldata.multihigh!=0) )
                    {
                        if ( cpldata.multihigh == 2 )
                            *cpldata.expansion |= (RLY_RD06 | RLY_RD11);
                        if ( cpldata.multihigh == 3 )
                            *cpldata.expansion |=  RLY_RD06;
                    }
                }
                if ( cpldata.cplhigh & CPL_NUBIT )
                {
                    *cpldata.expansion |= CPL_DATA_S4HI0;
                    if ( (cpldata.cmode) && (cpldata.multihigh!=0) )
                    {
                        if ( cpldata.multihigh == 2 )
                            *cpldata.expansion |= (RLY_RD03 | RLY_RD05);
                        if ( cpldata.multihigh == 3 )
                            *cpldata.expansion |=  RLY_RD03;
                    }
                }
                if ( cpldata.cpllow & CPL_L1BIT )
                    *cpldata.outrelay |= CPL_DATA_S1LO0;
                if ( cpldata.cpllow & CPL_L2BIT )
                    *cpldata.outrelay |= CPL_DATA_S2LO0;
                if ( cpldata.cpllow & CPL_L3BIT )
                    *cpldata.outrelay |= CPL_DATA_S3LO0;
                if ( cpldata.cpllow & CPL_NUBIT )
                    *cpldata.outrelay |= CPL_DATA_S4LO0;
                if ( cpldata.cpllow & CPL_PEBIT )
                    *cpldata.outrelay |= CPL_DATA_PELO0;
                if ( (cpldata.cmode == CMODE_ASYMMETRIC) ||
                     ((cpldata.cmode == CMODE_SYMMETRIC) && (cpldata.multihigh==0)) )
                    *cpldata.expansion |= (RLY_RD18 | RLY_RD19 | RLY_RD20);
                break;
            case E508_BOX:
                *cpldata.outrelay |= (CPL_DATA_PELO0 | RLY_RD27 | RLY_RD28);
                if ( cpldata.cplhigh & CPL_L1BIT )
                {
                    *cpldata.expansion |= RLY_RD02;
                    *cpldata.outrelay &= ~(RLY_RD28);
                }
                if ( cpldata.cplhigh & CPL_L2BIT )
                {
                    *cpldata.expansion |= RLY_RD04;
                    *cpldata.outrelay &= ~(RLY_RD27);
                }
                break;
        }
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
		ECAT_WriteEXPRelay1(cpldata.outaddr,(*cpldata.expansion >> 16));
	}

    srgdata.waveform = wavesave;

	return(0);
}

char	SRG_ClearCouplerHardware(void)
{
	if(cpldata.outaddr > MAX_MODULES)
		return(-1);

    if ( moddata[cpldata.outaddr].modinfo.id == E505B_BOX )
        return 0;  // E505B doesn't use coupler relay drivers (because coupler is integrated with surge module)

    /* High voltage couplers (i.e. E522C, E521C) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLHV)
	{
		*cpldata.outrelay &= ~(RLY_RD18 | RLY_RD19 | RLY_RD20 | RLY_RD21 | RLY_RD22 |
									  RLY_RD23 | RLY_RD24 | RLY_RD25 | RLY_RD27 | RLY_RD28 | RLY_RD29);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		return(0);
	}

	/* Standard AC Couplers (i.e. E551, E4554) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLLINE)
	{
		*cpldata.outrelay &= ~(CPL_CPLMASK);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		return(0);
	}

	/* Data Couplers (i.e. E571) */
	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
	{
		*cpldata.expansion = 0x00000000;
		*cpldata.outrelay &= ~(CPL_DATA_MASK0);
		ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
		ECAT_WriteEXPRelay1(cpldata.outaddr,(*cpldata.expansion >> 16));
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
	}
	return(0);
}

void	SRG_ChargeMonitor(void)
{
	chgdata.monitor = 0;
	SRG_StopSystem();
}

void SRG_EarlyChargeCheck(void)
{
   /* For the E515 only:  turn on the Ignatron high voltage supply 5 seconds
      (EARLY_CHECK_TIME) before the end of the charge process */
   if((moddata[(char)(srgdata.address)].modinfo.id) == E515_BOX)
		*srgdata.relay |= RLY_RD28;
	ECAT_WriteRelays(srgdata.address,*srgdata.relay);
   /* Complete the charge process */
	chgdata.checker = TPU_delay_int(EARLY_CHECK_TIME,SRG_ChargeCheck);
}

void	SRG_ChargeCheck(void)
{
    chgdata.checker = 0;
	chgdata.monitor = TPU_delay_int(CHG_HOLDTIME,SRG_ChargeMonitor);
	chgdata.flag = CHG_READY;
	SRG_UpdateSystem();
	KEY_beep();

	/* For the E52x only: disconnect the Spelman supply (RD27) once the system is in 'discharge ready' state */
	switch ((moddata[(char)(srgdata.address)].modinfo.id))
	{
	case E521S_BOX:
	case E522S_BOX:
		*srgdata.relay &= ~RLY_RD27;
		ECAT_WriteRelays(srgdata.address, *srgdata.relay);
		TPU_delay(20);
		break;
	}
}

int	SRG_CHG_pgm(P_PARAMT *params)
{
	char	errcode;

	if ( srgdata.srgflag )
	{
		errcode = SRG_StartSystem();
		if(errcode)
			SRG_StopSystem();
		fprintf(destination,"%d",errcode);
	}
	return(0);
}

/*
 *      Routine:	SRG_CPL_pgm
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the COUPLING bits for SURGE functions. Also, checks
 *					for multiple high couple bits and saves the information.
 * Error Checks:	The following surge coupling rules must be followed:
 *						- At least ONE line must be coupled to HIGH and LOW surge
 *						- No line can be coupled simultaneously
 *						- No more than ONE line can be coupled to the LOW surge
 *						- The PE line CANNOT couple to the HIGH surge
 *  Assumptions:	This routine will set the coupling bits regardless of
 *					the current MODE. Parameters are automatically scaled to
 *					the required range (NO OVERRANGE ERRORS)
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] cpldata.cplhigh		- Holds the HIGH surge coupling
 *					[X] cpldata.cpllow		- Holds the LOW surge coupling
 *					[X] cpldata.multihigh	- Indicates how many lines are coupled HIGH
 *					[X] cpldata.outrelay		- Current relay state for current coupler module
 *       Locals:	cplhi							- Temporary variable
 *					cpllo							- Temporary variable
 *					tvar							- Temporary variable
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					12/29/94 - James Murphy (JMM)
 *						This function will now return an error if the current
 *						coupler is not a valid selection (i.e. GHOST_BOX).
 *						This is due to the need to check the coupler type
 *						for relay setting
 */
int	SRG_CPL_pgm(P_PARAMT *params)
{
	uchar	cplhi;
	uchar	cpllo;
	uchar	tvar;

	if ( srgdata.srgflag )
	{
		cplhi = EXTRACT_INT(0) & 0x1F;
		cpllo = EXTRACT_INT(1) & 0x1F;

		if ( SRG_VerifyCoupling(cplhi,cpllo) )
			return(BAD_VALUE);

		cpldata.cplhigh = cplhi;
		cpldata.cpllow  = cpllo;
		/*
		 *	Set the relay BITS at this time for HI and LO coupling.  Note that
		 *	L1 to LO is controlled via hardware so is does not need to be set
		 */
		tvar = 0;
		cpldata.multihigh = 0;
		if(cplhi & CPL_L1BIT)
			tvar++;
		if(cplhi & CPL_L2BIT)
			tvar++;
		if(cplhi & CPL_L3BIT)
			tvar++;
		if(cplhi & CPL_NUBIT)
			tvar++;
		if(tvar > 1)
			cpldata.multihigh = tvar;
	}
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_CPL_rpt
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current surge COUPLING to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] cpldata.cplhigh		- Holds the HIGH surge coupling
 *					[ ] cpldata.cpllow		- Holds the LOW surge coupling
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	SRG_CPL_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d,%d",cpldata.cplhigh,cpldata.cpllow);
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_DLY_pgm
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the inter-surge delay time
 * Error Checks:	Delay is checked for UNDERRANGE and OVERRANGE and surge
 *					network is checked for validity
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (standard)
 *      Globals:	[X] srgdata.delay			- Holds the current delay time
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The delay range is now verified via the SRG_MinimumDelay
 *						function which extracts the minimum delay from the
 *						new WAVDATA structure.  Also, this routine will return
 *						an error if no surge network has yet been chosen
 */
int	SRG_DLY_pgm(P_PARAMT *params)
{
	if ( srgdata.srgflag )
	{
		if(EXTRACT_INT(0) > MAX_SRGDELAY)
			return(BAD_VALUE);

		if(srgdata.address >= MAX_MODULES)
			return(BAD_VALUE);

		if(EXTRACT_INT(0) < SRG_MinimumDelay(srgdata.address,srgdata.waveform))
			return(BAD_VALUE);

		srgdata.delay = (ushort)EXTRACT_INT(0);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_DLY_rpt
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current surge DELAY to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] srgdata.delay			- Holds the current DELAY time
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	SRG_DLY_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",srgdata.delay);
	return(0);
}
/*
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					12/27/94 - James Murphy (JMM)
 *						Changing the NETWORK will reset the WAVEFORM to the
 *						default wave.  The WAVEFORM program routine will then
 *						be called which will ADJUST various other parameters
 *					04/14/95 - James Murphy (JMM)
 *						The delay functionality has been changed so that the
 *						minimum delay is RESET each time the network is
 *						changed since customer usage shows preference in using
 *						the minimum delay as much as possible
 *					05/23/95 - James Muprhy (JMM)
 *						yada,yada, RD26, etc.
 *					09/28/95 - James Murphy (JMM)
 *						A check has been added to verify voltage settings
 *						whenever the surge network is changed to ensure the
 *						legality of the surge voltage
 */
int	SRG_NET_pgm(P_PARAMT *params)
{
	uchar		bayaddr;
	short		maxVoltage;
	P_PARAMT	tparam;

	if(srgdata.srgflag)
	{
		bayaddr = (uchar)EXTRACT_INT(0);
		if(bayaddr >= MAX_MODULES)
			return(BAD_VALUE);

		if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_SRG))
			return(BAD_VALUE);

		if ( ( moddata[srgdata.address].modinfo.id == E522S_BOX )
          || ( moddata[srgdata.address].modinfo.id == E521S_BOX ) )
        {
			*srgdata.relay &= (INST_VMASK | INST_IMASK | RLY_RD26 | RLY_RD30);
        }
		else if ( ( moddata[srgdata.address].modinfo.id == E505B_BOX )
               && ( moddata[bayaddr].modinfo.id == E505B_BOX ) )
        {
            /* If switching from an E505B module TO an E505B module (SurgeWare may do this),
               then retain the state of RLY_RD23 (FP/CDN select) */
			*srgdata.relay &= (INST_VMASK | INST_IMASK | RLY_RD23);
        }
        else
        {
			*srgdata.relay &= (INST_VMASK | INST_IMASK);
        }

		if(srgdata.address != GHOST_BOX)
			ECAT_WriteRelays(srgdata.address,*srgdata.relay);

        // If switching to OR from an E505B module, shut OFF EUT power on "from" module
        // If switching from an E505B module TO an E505B module (SurgeWare may do this), then do nothing
        if ( ( ( moddata[bayaddr].modinfo.id == E505B_BOX ) || ( moddata[srgdata.address].modinfo.id == E505B_BOX ) )
          && ( moddata[bayaddr].modinfo.id != moddata[srgdata.address].modinfo.id ) )
        {
            blue_off();
        }

		srgdata.address	    = bayaddr;
		srgdata.relay		= &moddata[(char)(srgdata.address)].relay;
		srgdata.expansion	= &moddata[(char)(srgdata.address)].expansion;

		TIO_SRGDisable();
		TIO_TOMap(srgdata.address,TO0_TOLCL0);
		*srgdata.relay |= LED_ACTIVE;
		if(((moddata[(char)(srgdata.address)].modinfo.id) == E522S_BOX) ||
			((moddata[(char)(srgdata.address)].modinfo.id) == E521S_BOX))
			*srgdata.relay |= RLY_RD30;
		ECAT_WriteRelays(srgdata.address,*srgdata.relay);

		/*
		 *	If the current waveform and coupler selections are not valid then
		 *	RESET the waveform to ZERO (0) and call the SRG_WAV_PGM function
		 */
		if(!(SRG_OutputValid(cpldata.outaddr,srgdata.address,srgdata.waveform)))
		{
			tparam.idata = 0;
			if(SRG_WAV_pgm(&tparam))
				return(BAD_VALUE);
		}

		/*
		 *	Check the validity of the VOLTAGE parameter and adjust it
		 *	if it is invalid.  If in calibration mode, there is no
		 *	adjustment needed
		 */
		maxVoltage = SRG_MaximumVoltage(srgdata.address,srgdata.waveform);
		if((((abs)(srgdata.voltage)) > maxVoltage) && (!calibration))
		{
			tparam.idata = maxVoltage;
			SRG_VLT_pgm(&tparam);
		}

		/*
		 *	RESET the DELAY parameter so that the minimum delay is used
		 */
		tparam.idata = SRG_MinimumDelay(srgdata.address,srgdata.waveform);
		SRG_DLY_pgm(&tparam);
	}
	return(0);
}

int	SRG_NET_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",srgdata.address);
	return(0);
}

/*
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					12/29/94 - James Murphy (JMM)
 *						The coupling modes are now RESET whenever the OUTPUT
 *						is changed.  This is being done to avoid the complexity
 *						of having to check the coupling validity if another
 *						is different
 *					04/14/95 - James Murphy (JMM)
 *						The delay functionality has been changed so that the
 *						minimum delay is RESET each time the output is
 *						changed since customer usage shows preference in using
 *						the minimum delay as much as possible
 */
int	SRG_OUT_pgm(P_PARAMT *params)
{
	short	maxVoltage;
	uchar	bayaddr;
	uint	commonrelay;
	P_PARAMT	tparam[2];

	if ( srgdata.srgflag )
	{
		bayaddr = (uchar)EXTRACT_INT(0);
		if((bayaddr >= MAX_MODULES) && (bayaddr != GHOST_BOX))
			return(BAD_VALUE);

		if(bayaddr != GHOST_BOX)
			if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLSRG))
				return(BAD_VALUE);

		if(srgdata.waveform >= MAX_WAVEFORMS)
			return(BAD_VALUE);

        if(!(SRG_OutputValid(bayaddr,srgdata.address,srgdata.waveform)))
            return BAD_VALUE;

		if(cpldata.outaddr != GHOST_BOX)
		{
			if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
			{
				*cpldata.outrelay &= (INST_VMASK | INST_IMASK);
				*cpldata.expansion = 0x00000000;
				if(cpldata.output)
					*cpldata.outrelay |= (LED_ACTIVE | LED_VOLTAGE | RLY_DATAOUT);
				if(cpldata.clamp != CLAMP_INT220V)
					*cpldata.outrelay |= (RLY_INT20V);
				ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
				ECAT_WriteEXPRelay1(cpldata.outaddr,(*cpldata.expansion >> 16));
			}
			else
            {
				*cpldata.outrelay &= ~(LED_ACTIVE);
                // E505B doesn't use most coupler relay drivers (because coupler is integrated with surge module)
                if ( moddata[cpldata.outaddr].modinfo.id != E505B_BOX )
                    *cpldata.outrelay &= ~(CPL_CPLMASK);
            }
			commonrelay = *cpldata.outrelay;
			if(cpldata.outaddr == cpldata.srcaddr)
				commonrelay |= (*cpldata.srcrelay | LED_ACTIVE);
			ECAT_WriteRelays(cpldata.outaddr,commonrelay);
		}
		cpldata.outaddr	= GHOST_BOX;
		cpldata.outrelay	= &ghostrelay;
		if(bayaddr != GHOST_BOX)
		{
			cpldata.outaddr	= bayaddr;
			cpldata.outrelay	= &moddata[(char)(cpldata.outaddr)].relay;
			cpldata.expansion = &moddata[(char)(cpldata.outaddr)].expansion;
			if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
			{
				*cpldata.outrelay &= (INST_VMASK | INST_IMASK);
				*cpldata.expansion = 0x00000000;
				if(cpldata.output)
					*cpldata.outrelay |= (LED_ACTIVE | LED_VOLTAGE | RLY_DATAOUT);
				if(cpldata.clamp != CLAMP_INT220V)
					*cpldata.outrelay |= (RLY_INT20V);
				ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
				ECAT_WriteEXPRelay1(cpldata.outaddr,(*cpldata.expansion >> 16));
				if(moddata[(char)(srgdata.address)].modinfo.id == E508_BOX)
					cpldata.cmode = CMODE_SYMMETRIC;
			}
			commonrelay = *cpldata.outrelay |= LED_ACTIVE;
            // Select CDN side of E505B combined surge/CDN module
            if ( moddata[cpldata.outaddr].modinfo.id == E505B_BOX )
                commonrelay |= RLY_RD23;  // cpldata.outrelay IS srgdata.relay
			if(cpldata.outaddr == cpldata.srcaddr)
				commonrelay |= *cpldata.srcrelay;
			ECAT_WriteRelays(cpldata.outaddr,commonrelay);
		}
		/*
		 *	RESET the coupling modes to L1/L2 which is the default mode for
		 *	both LINE and DATA couplers
		 */
		if(SRG_VerifyCoupling(cpldata.cplhigh,cpldata.cpllow))
		{
			if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
			{
				tparam[0].idata = CPL_L1BIT;
				tparam[1].idata = CPL_PEBIT;
			}
			else
			{
				tparam[0].idata = CPL_L1BIT;
				tparam[1].idata = CPL_L2BIT;
			}
			SRG_CPL_pgm(tparam);
		}

		/*
		 *	Check the validity of the VOLTAGE parameter and adjust it
		 *	if it is invalid.  If in calibration mode, there is no
		 *	adjustment needed
		 */
		maxVoltage = SRG_MaximumVoltage(srgdata.address,srgdata.waveform);
		if((((abs)(srgdata.voltage)) > maxVoltage) && (!calibration))
		{
			tparam[0].idata = maxVoltage;
			SRG_VLT_pgm(tparam);
		}

		/*
		 *	RESET the DELAY parameter so that the minimum delay is used
		 */
		tparam[0].idata = SRG_MinimumDelay(srgdata.address,srgdata.waveform);
		SRG_DLY_pgm(tparam);
	}
	return(0);
}

int	SRG_OUT_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.outaddr);
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_VLT_pgm
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the surge VOLTAGE level
 * Error Checks:	The voltage is checked for UNDERRANGE and OVERRANGE in
 *					both positive and negative directions
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (standard)
 *      Globals:	[X] srgdata.voltage		- Holds the current surge VOLTAGE
 *					[ ] srgdata.waveform		- Holds the current surge waveform
 *					[ ] srgdata.address		- Holds the current surge network
 *					[ ] moddata					- Holds ALL MODULE information
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 03/29/93 - James Murphy (JMM)
 *						The voltage checking has been changed so that there
 *						is NO maximum voltage when in calibration mode
 *					06/03/93 - James Murphy (JMM)
 *						The maximum voltage checking has been modified to
 *						allow for modules and waveforms with differing
 *						maximum levels
 *					12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					12/16/94 - James Murphy (JMM)
 *						The routines for voltage translation and voltage
 *						maximum have changed so all references have been
 *						modified along with them.  Also, all voltage-related
 *						variables have been converted to integer values and
 *						the routine assumptions have been done away with
 *					12/30/94 - James Murphy (JMM)
 *						An error code is now generated if the surge network
 *						has not yet been chosen (i.e. GHOST_BOX)
 */
int	SRG_VLT_pgm(P_PARAMT *params)
{
	int	voltage;
	int	maxVoltage;

	if ( srgdata.srgflag )
	{
		if(srgdata.address >= MAX_MODULES)
			return(BAD_VALUE);

		voltage = EXTRACT_INT(0);
		if(!calibration)
			maxVoltage = SRG_MaximumVoltage(srgdata.address,srgdata.waveform);

		if((((abs)(voltage)) > maxVoltage) && (!calibration))
			return(BAD_VALUE);

		srgdata.voltage = voltage;
	}
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_VLT_rpt
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current surge VOLTAGE to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] srgdata.voltage		- Holds the current surge VOLTAGE
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	SRG_VLT_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",srgdata.voltage);
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_WAV_pgm
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the surge WAVEFORM.  This function can and will adjust
 *					the OUTPUT, VOLTAGE, and DELAY parameters in order to
 *					select the desired waveform
 * Error Checks:	Waveforms cannot be greater than MAX_WAVEFORMS and the
 *					number of waves in the desired network, Surge network
 *					must be VALID
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (standard)
 *      Globals:	[X] srgdata.waveform		- Current surge WAVEFORM
 *					[ ] moddata					- ALL MODULE information
 *					[ ] cpldata.E50812Paddr	- Address of the E508-12P
 *					[ ] cpldata.outaddr		- Address of the output module
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					12/13/94 - James Murphy (JMM)
 *						The checking for waveform numbers has been changed
 *						to support the new WAVDATA structure which tells
 *						how many waveforms every surge module has
 *					12/27/94 - James Murphy (JMM)
 *						The WAVEFORM command now controls the functions OUTPUT,
 *						VOLTAGE, and DELAY.  These parameters will be adjusted
 *						if need be depending on the network/wave/coupler
 *						combination
 *					01/30/95 - James Murphy (JMM)
 *						A check was added to ensure that the ACTIVE-LED on an
 *						E508-12P would toggle according to the selected
 *						E508 waveform
 *					04/14/95 - James Murphy (JMM)
 *						The delay functionality has been changed so that the
 *						minimum delay is RESET each time the waveform is
 *						changed since customer usage shows preference in using
 *						the minimum delay as much as possible
 */
int	SRG_WAV_pgm(P_PARAMT *params)
{
	uchar		srgId;
	uchar		wave;
	short		maxVoltage;
	char		tvar;
	P_PARAMT	tparam;

	if ( srgdata.srgflag )
	{
		wave = EXTRACT_INT(0);
		if(wave >= MAX_WAVEFORMS)
			return(BAD_VALUE);

		if(srgdata.address >= MAX_MODULES)
			return(BAD_VALUE);

		srgId = moddata[(char)(srgdata.address)].modinfo.id;
		if(wave > wavdata[(char)(srgId)].numWaves)
			return(BAD_VALUE);

		if((srgId == E508_BOX) && (cpldata.E50812Paddr >= MAX_MODULES) && (EXTRACT_INT(0) == E508_25A))
			return(BAD_VALUE);

		srgdata.waveform = (uchar)EXTRACT_INT(0);

		/*
		 *	Check the validity of the OUTPUT parameter and adjust it
		 *	if it is invalid.  The rule is to always select the FRONT PANEL
		 *	if it is valid, otherwise the first valid coupler is chosen
		 */
		if(!(SRG_OutputValid(cpldata.outaddr,srgdata.address,srgdata.waveform)))
		{
			if(SRG_OutputValid(255,srgdata.address,srgdata.waveform))
			{
				tparam.idata = 0xFF;
				SRG_OUT_pgm(&tparam);
			}
			else
			{
				for(tvar = 0; tvar < MAX_MODULES; tvar++)
					if(SRG_OutputValid(tvar,srgdata.address,srgdata.waveform))
					{
						tparam.idata = tvar;
						SRG_OUT_pgm(&tparam);
						break;
					}
			}
		}

		/*
		 *	Check the validity of the VOLTAGE parameter and adjust it
		 *	if it is invalid.  If in calibration mode, there is no
		 *	adjustment needed
		 */
		maxVoltage = SRG_MaximumVoltage(srgdata.address,srgdata.waveform);
		if((((abs)(srgdata.voltage)) > maxVoltage) && (!calibration))
		{
			tparam.idata = maxVoltage;
			SRG_VLT_pgm(&tparam);
		}

		/*
		 *	RESET the DELAY parameter so that the minimum delay is used
		 */
		tparam.idata = SRG_MinimumDelay(srgdata.address,srgdata.waveform);
		SRG_DLY_pgm(&tparam);

		/*
		 *	If the E508-12P and E508 modules are present then the LED_ACTIVE
		 *	light must be set on the E508-12P according to the selected waveform
		 */
		if((srgId == E508_BOX) && (cpldata.E50812Paddr < MAX_MODULES))
		{
			if(srgdata.waveform == E508_25A)
				moddata[(char)(cpldata.E50812Paddr)].relay |= LED_ACTIVE;
			else
				moddata[(char)(cpldata.E50812Paddr)].relay &= ~(LED_ACTIVE);
			ECAT_WriteRelays(cpldata.E50812Paddr,moddata[(char)(cpldata.E50812Paddr)].relay);
		}

        /*
         *  For the E505B, which has a built-in Coupler/Decoupler Network,
         *  route the LED_ACTIVE and LED_VOLTAGE lights to either the Direct
         *  output or the CDN output depending on user waveform selection
         *  (the output destination is fixed for each waveform).  Also activate
         *  the LED_ACTIVE light, which gets shut off during the call to
         *  SRG_OUT_pgm above when transitioning from 2.5 kV (CDN-only) waveform
         *  to one of the other two (Front Panel-only) waveforms.
         */
        if (srgId == E505B_BOX)
        {
            if ( ( srgdata.waveform == E505B_800V100A ) || ( srgdata.waveform == E505B_1500V ) )
            {
                blue_off();  // disable E505B EUT power when selecting a non-CDN waveform or another network
                moddata[(char)(srgdata.address)].relay &= ~RLY_RD23;
                moddata[(char)(srgdata.address)].relay |= LED_ACTIVE;
            }
            else if ( srgdata.waveform == E505B_2500V )
            {
                moddata[(char)(srgdata.address)].relay |= RLY_RD23;
            }
            ECAT_WriteRelays( srgdata.address, moddata[(char)(srgdata.address)].relay);
        }
	}
	return(0);
}

/*[ÿ]
 *      Routine:	SRG_WAV_rpt
 *       Module:	CMD_SRG.C
 * Date Created:	February 1, 1993
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current surge WAVEFORM to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] srgdata.waveform		- Holds the current surge WAVEFORM
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	SRG_WAV_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",srgdata.waveform);
	return(0);
}

/*
 *	Used to cheat out the EUT switch so it does not have to be cycled
 *	upon activation.  Used with POWER_USER command line parameter in
 *	SurgeWare
 */
int	SRG_CHEAT(P_PARAMT *params)
{
	if(!cpldata.eutrequest)
		return(BAD_VALUE);

	if(!cpldata.eutswitch)
		return(0);

	blue_on();
	return(0);
}

/* 
 *  Enables/disables EUT line check before test.
 * 
 *  Normal operation is line check ON (1), which corresponds to srgCheat.lineCheck OFF (0)
 */
int	SRG_LCK_pgm(P_PARAMT *params)
{
    uchar line_check_on = EXTRACT_INT(0);

    srgCheat.lineCheck = line_check_on ? 0 : 1;

    return 0;
}

/**** SLOPE CALIBRATION CALCULATIONS ARE DONE HERE ****/

/*
*   This function applies common for all SRG units calibration adjustments and applies them to the supplied cal 
*   factor and range offset if the data coupler is specified as the coupling type.
*/
void CAL_GetCommonSrgCalFactor(
	int raw_voltage_absolute,
	CALINFO* ci_wave,
	uchar wave,
	char cplType,
	int* calValue,
	uchar* rangeOff)
{
	*rangeOff = 0;
	*calValue = CALIBRATION_NORM;

	/* Retrieve calibration data point for this waveform/coupling mode/voltage */
	switch (cplType)
	{
	case CPL_TYPELINE:
	case CPL_TYPEHV:
		*calValue = ci_wave->data[(WAVE_OFFSET + (FP_LINE_SIZE * wave) + LINE_OFFSET)];
		break;
	case CPL_TYPEDATA:
		if (raw_voltage_absolute <= 625)
			*rangeOff = 0;
		else if (raw_voltage_absolute <= 850)
			*rangeOff = 1;
		else if (raw_voltage_absolute <= 1200)
			*rangeOff = 2;
		else if (raw_voltage_absolute <= 2000)
			*rangeOff = 3;
		else
			*rangeOff = 4;
		*calValue = ci_wave->data[(DATA_OFFSET + (DATA_SIZE * wave))];
		break;
	default:
		*calValue = ci_wave->data[(WAVE_OFFSET + (FP_LINE_SIZE * wave) + FP_OFFSET)];
		break;
	}
}

#define CPL_INVALID          (0)
#define CPL_4L_TO_PE         (25)
#define CPL_3L_TO_PE         (33)
#define CPL_2L_TO_PE         (50)
#define CPL_1L_TO_PE         (100)
#define CPL_3L_TO_L          (133)
#define CPL_2L_TO_L          (150)
#define CPL_1L_TO_L          (200)

/*
*   Calculates a value that identifies the coupling based on supplied HI-LO values.
*/
short CAL_GetCouplingValue(
	uchar cpl_hi,
	uchar cpl_lo)
{
	short lValue = CPL_INVALID;
	if (cpl_hi & CPL_L1BIT)
		lValue++;
	if (cpl_hi & CPL_L2BIT)
		lValue++;
	if (cpl_hi & CPL_L3BIT)
		lValue++;
	if (cpl_hi & CPL_NUBIT)
		lValue++;

	if (lValue != 0)
	{
		lValue = 100 / lValue;
		if (cpl_lo & CPL_PEBIT)
			lValue += 0;
		else
			lValue += 100;
	}

	return lValue;
}

#define E503x_RANGE_SPLIT          3000    // for E503x cal adjustment
#define E503x_POLARITY_OFFSET      (2)
#define E503x_RANGE_OFFSET         (1)

const uchar	E503Adjust[][2] =
{
	  E503x_CAL_POS_LO_FP_200A_v1,   E503x_CAL_POS_LO_FP_500A_v1,   /* 200A/500A POS LO */
	  E503x_CAL_POS_HI_FP_200A_v1,   E503x_CAL_POS_HI_FP_500A_v1,   /* 200A/500A POS HI */
	  E503x_CAL_NEG_LO_FP_200A_v1,   E503x_CAL_NEG_LO_FP_500A_v1,   /* 200A/500A NEG LO */
	  E503x_CAL_NEG_HI_FP_200A_v1,   E503x_CAL_NEG_HI_FP_500A_v1,   /* 200A/500A NEG HI */
};

#define E503x_CPL_POLARITY_OFFSET    (3)
#define E503x_CPL_L2N_OFFSET         (0)
#define E503x_CPL_L2PE_OFFSET        (1)
#define E503x_CPL_L2L_OFFSET         (2)

const uchar	E503CplAdjust[][2] =
{
	  E503x_CAL_POS_L2N_200A_v1,    E503x_CAL_POS_L2N_500A_v1,    /* 200A/500A POS L-N */
	  E503x_CAL_POS_L2PE_200A_v1,   E503x_CAL_POS_L2PE_500A_v1,   /* 200A/500A POS L-PE */
	  E503x_CAL_POS_L2L_200A_v1,    E503x_CAL_POS_L2L_500A_v1,    /* 200A/500A POS L-L */
	  E503x_CAL_NEG_L2N_200A_v1,    E503x_CAL_NEG_L2N_500A_v1,    /* 200A/500A NEG L-N */
	  E503x_CAL_NEG_L2PE_200A_v1,   E503x_CAL_NEG_L2PE_500A_v1,   /* 200A/500A NEG L-PE */
	  E503x_CAL_NEG_L2L_200A_v1,    E503x_CAL_NEG_L2L_500A_v1,    /* 200A/500A NEG L-L */
};


/*
*   This function applies calibration adjustments to E503x boxes - there are quite a few more factors then normal SRG units.
*/
void CAL_GetE503xCalFactor(
	int raw_voltage, 
	uchar cpl_hi, 
	uchar cpl_lo,
	CALINFO* ci_wave, 
	uchar wave, 
	char cplType,
	int* calValue,
	uchar* rangeOff)
{
	int raw_voltage_absolute = abs(raw_voltage);
	uchar offset = -1;
	ushort cal_datum = 0;
	uchar index = 0;
	short calRev = (short)ci_wave->data[CAL_REV_OFF];

	*rangeOff = 0;
	*calValue = CALIBRATION_NORM;

	if (wave > E503_500A)
		wave = E503_500A;

	/* Get the revision of the calibration data */
	switch (calRev)
	{
	default:
	case 0:
		if (raw_voltage < 0)
		{
			/* rev 0 Negative adjustments */
			switch (cplType)
			{
			case CPL_TYPELINE:
			case CPL_TYPEHV:
				if (srgdata.waveform == E503_500A)
					offset = E503A_CAL_NEG_LINE_500A;
				else  // assume srgdata.waveform == E503_200A
					offset = E503A_CAL_NEG_LINE_200A;
				break;
			case CPL_TYPEDATA: // there is no Data Coupler, fallback to FP
			default:  // front panel
				if (srgdata.waveform == E503_500A)
					offset = E503A_CAL_NEG_FP_500A;
				else  // assume srgdata.waveform == E503_200A
					offset = E503A_CAL_NEG_FP_200A;
				break;
			}

			if (offset >= 0)
			{
				cal_datum = abs((short)ci_wave->data[offset]);  // normalize to positive value

				/* Validate before committing to use - must be between 4000 V and 8000 V
					- this special cal value is only for negative program voltages,
						and is used in place of the generic cal value that all other modules use for both polarities */
				if ((cal_datum >= E503A_CAL_VALUE_MIN) && (cal_datum <= E503A_CAL_VALUE_MAX))
					*calValue = cal_datum;
			}
		}
		else
		{
			/* rev 0 Positive adjustments - common surge adjustments */
			CAL_GetCommonSrgCalFactor(
				raw_voltage_absolute,
				ci_wave,
				wave,
				cplType,
				calValue,
				rangeOff);
		}
		break;

	case 1:
		{
			/* rev 1 calibration adjustments */
			switch (cplType)
			{
			case CPL_TYPELINE:
			case CPL_TYPEHV:
				index = 0;
				if (raw_voltage < 0) // POS-NEG
					index += E503x_CPL_POLARITY_OFFSET;

				if ((cpl_hi & CPL_L1BIT) || (cpl_hi & CPL_L2BIT) || (cpl_hi & CPL_L3BIT))
				{

				}


				offset = E503CplAdjust[index][wave];
				break;

			case CPL_TYPEDATA: // there is no Data Coupler, fallback to FP
			default:  // front panel
				index = 0;
				if (raw_voltage < 0) // POS-NEG
					index += E503x_POLARITY_OFFSET;

				if (raw_voltage_absolute >= E503x_RANGE_SPLIT) // HI-LO
					index += E503x_RANGE_OFFSET;

				offset = E503Adjust[index][wave];
				break;
			}

			*calValue = abs((short)ci_wave->data[offset]);  // normalize to positive value
		}
		break;
	}
}

/* 
*       raw_voltage is the original voltage to be calibrated and otherwise adjusted.
*       adjusted_voltage is raw_voltage with some non-calibration adjustments already applied.
*       This function applies calibration adjustments to, and then returns, the further-modified adjusted_voltage.
*
*       If called from SRG_TranslateVoltage_precise(), then raw voltage and adjusted_voltage is function is operating on millivolts, not volts.                                                                                                         .
*/ 
int	CAL_GetCalibratedVoltage( int adjusted_voltage, int raw_voltage, int voltage_scaling,
                              uchar cpl_hi, uchar cpl_lo,
                              CALINFO *ci_wave, CALINFO *ci_cplr,
                              uchar wave, uchar netId, char cplType )
{
    uchar use_fixed_coupling_adjustments = TRUE;  // TRUE for all except E503A with available supplemental cal values
    int raw_voltage_absolute = abs(raw_voltage);
    int calibration_factor = CALIBRATION_NORM;  // default multiplier (results in no cal adjustment)
	short lValue;
	int calValue;
	int adjValue;
    int proportional_adjustment = 0;
	uchar rangeOff;
	uchar rangeIndex;
	uchar E503x_box = ((netId == E503_BOX) || (netId == E503A_BOX)) ? TRUE : FALSE;

	if(wave >= MAX_WAVEFORMS)
		wave = 0;

	if (E503x_box)
		CAL_GetE503xCalFactor(
			raw_voltage,
			cpl_hi,
			cpl_lo,
			ci_wave,
			wave,
			cplType,
			&calValue,
			&rangeOff);
	else
		CAL_GetCommonSrgCalFactor(
			raw_voltage_absolute,
			ci_wave,
			wave,
			cplType,
			&calValue,
			&rangeOff);

    /* Make coupling mode adjustments */
    adjValue = 100;  // default adjustment is no adjustment (100 %)

    if ( ( cplType == CPL_TYPELINE )
      || ( cplType == CPL_TYPEHV )
      || ( cplType == CPL_TYPEDATA ) )
    {
		lValue = CAL_GetCouplingValue(cpl_hi, cpl_lo);
        switch(lValue)
        {
            case CPL_4L_TO_PE: lValue = 0; break;  // 4 lines to PE
            case CPL_3L_TO_PE: lValue = 1; break;  // 3 lines to PE
            case CPL_2L_TO_PE: lValue = 2; break;  // 2 lines to PE
            case CPL_3L_TO_L:  lValue = 4; break;  // 3 lines to line
            case CPL_2L_TO_L:  lValue = 5; break;  // 2 lines to line
            case CPL_1L_TO_L:  lValue = 6; break;  // 1 line to line
            case CPL_1L_TO_PE:                     // 1 line to PE (default case)
            default:           lValue = 3; break;
        }
        if ( cplType == CPL_TYPEDATA )
        {
            switch ( netId )
            {
                case E501_BOX:
                    switch ( rangeOff )
                    {
                        case 3: calValue += 410;    break;
                        case 2: calValue += 520;    break;
                        case 1: calValue += 550;    break;
                        case 0: calValue += 530;    break;
                    }
                    break;
                case E501A_BOX:
                case E501B_BOX:
                    switch ( rangeOff )
                    {
                        case 3: calValue += 480;    break;
                        case 2: calValue += 660;    break;
                        case 1: calValue += 760;    break;
                        case 0: calValue += 700;    break;
                    }
                    break;
                case E510_BOX:
                    switch ( rangeOff )
                    {
                        case 3: calValue += 300;    break;
                        case 2: calValue += 400;    break;
                        case 1: calValue += 410;    break;
                        case 0: calValue += 400;    break;
                    }
                    break;
                case E510A_BOX:
                    switch ( rangeOff )
                    {
                        case 3: calValue += 480;    break;
                        case 2: calValue += 650;    break;
                        case 1: calValue += 720;    break;
                        case 0: calValue += 650;    break;
                    }
                    break;
                case E502A_BOX:
                case E502B_BOX:
                    switch ( rangeOff )
                    {
                        case 3: calValue += 610;    break;
                        case 2: calValue += 1180;   break;
                        case 1: calValue += 1940;   break;
                        case 0: calValue += 2620;   break;
                    }
                    break;
            }
        }
        else  // cplType != CPL_TYPEDATA
        {
            /* For E503x line coupling modes, adjust program voltage by percentage specified in module cal data
               (other modules use fixed values from cplAdjust[] table in ecat.c). */
            if (E503x_box)
            {
                uchar offset = -1;
                uchar multiplier = 1;

                switch(lValue)
                {
                    case  0:    // 4 lines to PE
                        multiplier = 3;  // 3 times 
                        offset = srgdata.waveform == E503_500A ? E503A_CAL_TWO_LINES_TO_PE_500A : E503A_CAL_TWO_LINES_TO_PE_200A;
                        break;
                    case  1:    // 3 lines to PE
                        offset = srgdata.waveform == E503_500A ? E503A_CAL_TWO_LINES_TO_PE_500A : E503A_CAL_TWO_LINES_TO_PE_200A;
                        multiplier = 2;
                        break;
                    case  2:    // 2 lines to PE
                        offset = srgdata.waveform == E503_500A ? E503A_CAL_TWO_LINES_TO_PE_500A : E503A_CAL_TWO_LINES_TO_PE_200A;
                        multiplier = 1;
                        break;
                    case 4:     // 3 lines to line
                    case 5:     // 2 lines to line
                    case 6:     // 1 line to line
                        offset = srgdata.waveform == E503_500A ? E503A_CAL_LINE_TO_LINE_500A : E503A_CAL_LINE_TO_LINE_200A;
                        multiplier = 1;
                        break;
                    default:    // includes 1 line to PE (no further adjustment required)
                        break;
                }

                if ( offset != -1 )
                {
                    /* proportional_adjustment is stored as a percentage with 1/10 % resolution,
                       but without the decimal (e.g. 1.5% is stored as "15").
                       Divide by ten (PROPORTIONAL_ADJUSTMENT_DIVISOR) when using value. */
                    proportional_adjustment = (short)ci_wave->data[offset] * multiplier;  // still in x10 form (see comment above) 

                    // Validate
                    if ( proportional_adjustment <= PROPORTIONAL_ADJUSTMENT_MAX )
                        use_fixed_coupling_adjustments = FALSE;  // valid
                    else
                        proportional_adjustment = 0;  // invalid
                }
                else  // offset == -1, so this adjustment doesn't apply
                {
                    proportional_adjustment = 0;
                }
            }
        }

        // All modules except E503x use fixed coupling adjustments from cplAdjust[] table in ecat.c
        if ( use_fixed_coupling_adjustments )
        {
            rangeIndex	= (uchar)((wavdata[(char)(netId)].wavinfo[(char)(wave)].rangeIndex[cplType]) + rangeOff);
            adjValue = (int)(cplAdjust[(char)(rangeIndex)][lValue]);
            calibration_factor = ( calValue * adjValue ) / 100;  // make coupling mode adjustment
        }
        else
        {
            proportional_adjustment += ( 100 * PROPORTIONAL_ADJUSTMENT_DIVISOR );  // convert from '% change' to 'fraction of 100.0' (will divide by 100.0 again below)
            calibration_factor = ( calValue * proportional_adjustment ) / ( 100 * PROPORTIONAL_ADJUSTMENT_DIVISOR );  // make coupling mode adjustment
        }
    }  // if ( ( cplType == CPL_TYPELINE ) || ( cplType == CPL_TYPEHV ) || ( cplType == CPL_TYPEDATA ) )
    else
    {
        calibration_factor = calValue;  // for Front Panel
    }

    /* Apply calibration factor */
    adjusted_voltage = ( adjusted_voltage * calibration_factor ) / CALIBRATION_NORM;

    /* Apply supplemental calibration adjustments */
	if (E503x_box)
    {
        /* For E503x, derate output at lower voltage settings (output:input ratio is higher at lower voltages).
           All coupling modes and both waveforms (200 A and 500 A) use the same compensation, which is the
           difference between two Gaussian curves (similar to a normal distribution curve).  This provides
           minimal compensation at higher voltages (both polarities), with increasing compensation down to
           about 1000 V, and then slightly decreasing compensation approaching zero.  (Use an online plotting tool to visualize.)
           Derating is as:  Vprogram = Vprogram - [( Vset * a * exp(-((Vset / b)^2) / c ) ) - ( Vset * d * exp(-((Vset / e)^2) / f ) )],
            where a = E503A_ADJUSTMENT_a; b = E503A_ADJUSTMENT_b; c = E503A_ADJUSTMENT_c;
            d = E503A_ADJUSTMENT_d; e = E503A_ADJUSTMENT_e; and f = E503A_ADJUSTMENT_f */
        adjusted_voltage -= (int)(raw_voltage * E503A_ADJUSTMENT_a * exp( -pow( raw_voltage / E503A_ADJUSTMENT_b, 2 ) / E503A_ADJUSTMENT_c ))
                            - (int)(raw_voltage * E503A_ADJUSTMENT_d * exp( -pow( raw_voltage / E503A_ADJUSTMENT_e, 2 ) / E503A_ADJUSTMENT_f ));
    }
    else if ( netId == E507_BOX )
    {
        short e507_offset_millivolts;
        float e507_offset_volts = 0.0;
        uchar raw_voltage_polarity = (raw_voltage == raw_voltage_absolute) ? POSITIVE : NEGATIVE;
        uchar adjusted_voltage_polarity;
        /* For the E507 add a sub-one-volt waveform-dependent offset to all voltages to improve peak current response.
           Because of voltage scaling in function SRG_TranslateVoltage_precise(), this will truncate to no change
           at all for most higher voltages. */
        if ( srgdata.waveform == E501_12OHM )
            e507_offset_millivolts = (short)(ci_wave->data[E507_CAL_OFFSET_12_OHM]);
        else
            e507_offset_millivolts = (short)(ci_wave->data[E507_CAL_OFFSET_02_OHM]);

        // Validate before committing to use - must be between 0 mV and 1000 mV
        if ( ( e507_offset_millivolts >= E507_CAL_VALUE_MIN ) && ( e507_offset_millivolts <= E507_CAL_VALUE_MAX ) )
            e507_offset_volts = (float)e507_offset_millivolts / 1000;
        adjusted_voltage += (int)(( voltage_scaling * e507_offset_volts ) * ( (adjusted_voltage >= 0) ? 1 : -1 ));

        // If offset caused a polarity reversal (possible only when programming 0 V), then set adjusted_voltage to zero.
        adjusted_voltage_polarity = (adjusted_voltage == abs( adjusted_voltage)) ? POSITIVE : NEGATIVE;
        if ( adjusted_voltage_polarity != raw_voltage_polarity )
            adjusted_voltage = 0;
    }

    /* Return calibrated voltage */
    return adjusted_voltage;
}

char	E507_HVProgram(
uchar	netaddr,
int		millivoltage,  // note MILLIVOLTS!
uchar	enableFlag)
{
    DAC_RESOLUTION dac_resolution = DR_16;

    uint    full_scale_millivolts = 500 * 1000;  // supply is 500 V max output for full scale input
    uint    full_scale_bits = (1 << dac_resolution) - 1;  // 16 bit unipolar DAC full scale is 65535 bits
    float   bits_per_millivolt = (float)full_scale_bits / (float)full_scale_millivolts;
    uint    bits_for_fifty_volts = 50000 * bits_per_millivolt;
	uint	dacval;
	char	errval;

	if(netaddr > MAX_MODULES)
		return -1;

    if ( moddata[(char)(netaddr)].modinfo.id != E507_BOX )
        return -1;

	dacval = (uint)(abs(millivoltage) * bits_per_millivolt);
	if ( dacval > full_scale_bits )
		dacval = bits_for_fifty_volts;  // mercury K5 can only handle current levels below about 50 V program level

	E52x_SetHVPS( netaddr, dacval, dac_resolution);

	if(enableFlag)
		*srgdata.expansion |= 0x0300;
	else
		*srgdata.expansion &= ~(0x0300);
	ECAT_WriteEXPRelay0((char)(srgdata.address),*srgdata.expansion);

	return 0;
}

char	E52x_HVProgram(
uchar	netaddr,
int		voltage,
uchar	enableFlag)
{
	uint	dacval;
	char	errval;

	if(netaddr > MAX_MODULES)
		return(-1);

	dacval = (uint)(abs((voltage * 4095) / 23000));
	if(dacval > 3916)
		dacval = 3916;

	if(enableFlag)
	{
		*srgdata.relay |= RLY_RD27;
		*srgdata.expansion |= 0x0300;
	}
	else
	{
		*srgdata.relay &= ~(RLY_RD27);
		*srgdata.expansion &= ~(0x0300);
	}
	ECAT_TimerSequencer(netaddr,(*srgdata.relay >> 24));
	ECAT_TimerSequencer(netaddr,(*srgdata.relay >> 24));
	TPU_delay(100);

	E52x_SetHVPS(netaddr, dacval, DR_12);
	TPU_delay(50);
	ECAT_WriteEXPRelay0((char)(srgdata.address),*srgdata.expansion);
	TPU_delay(40);
	errval = ECAT_HVProgram(1800,enableFlag);

	return(0);
}

/* This function is used to program auxiliary HV power supplies as used in
   E507 and E52x modules.  For E507 modules, the DAC that is being programmed
   is 16 bits (Linear Technology LTC1595), and for the E52x modules it's
   12 bits (Analog Devices DAC8043); they are functionally equivalent except
   for bit resolution.  In either case the board containing the DAC is
   installed in the module, and is connected via a dedicated cable to the
   module's Interface Board.  For E52x modules (12-bit DAC), the board is
   "E522 Power Supply Control PCB" (p/n 90-522-130-00).  There is a variant
   with a 16-bit DAC for E507 modules (name and part number TBD). */
void E52x_SetHVPS(uchar bay, uint dac_value, DAC_RESOLUTION dac_resolution)
{
    uint *dac_control_bits;
    int bit_index;
    ushort program_buffer;
    ushort next_bit_mask;  // masks next bit to be programmed

    if ( bay >= MAX_MODULES )
        return;

    // Only 12-bit and 16-bit serial DACs are supported
    if ( ( dac_resolution != DR_12 ) && ( dac_resolution != DR_16 ) )
        return;

    next_bit_mask = 1 << (dac_resolution - 1);

    program_buffer = dac_value;

    dac_control_bits = &(moddata[(char)(bay)].expansion);

    *dac_control_bits |= E52xDAC_LOAD;  // Load High
    *dac_control_bits &= ~(E52xDAC_CLOCK | E52xDAC_DATA_IN);  // Clock and Data In low
    ECAT_WriteExpansion(bay, *dac_control_bits);

    /* Program DAC MSb-first */

    // Write data to device's shift register
    for ( bit_index = 0; bit_index < dac_resolution; bit_index++ )
    {
        // Set next bit value
        if ( program_buffer & next_bit_mask )
            *dac_control_bits |= E52xDAC_DATA_IN;  // one
        else
            *dac_control_bits &= ~E52xDAC_DATA_IN;  // zero
        ECAT_WriteExpansion(bay, *dac_control_bits);

        // Write data to device's shift register
        asm(" NOP");
        asm(" NOP");
        *dac_control_bits |= 0x2000;  // Clock high
        ECAT_WriteExpansion(bay, *dac_control_bits);
        asm(" NOP");
        asm(" NOP");
        *dac_control_bits &= (~0x2000);  // Clock low
        ECAT_WriteExpansion(bay, *dac_control_bits);

        /* Shift buffer so next bit to be programmed is in mask (MSb) position,
           and mask valid buffer bits (e.g. LSb is not valid) */
        program_buffer = (program_buffer << 1) & ((1 << dac_resolution) - 2);
    }

    // Load device's shift register into DAC register (update DAC output)
    *dac_control_bits &= ~E52xDAC_LOAD;  // Load low
    ECAT_WriteExpansion(bay, *dac_control_bits);
    asm(" NOP");
    asm(" NOP");
    *dac_control_bits |= E52xDAC_LOAD;  // Load high
    ECAT_WriteExpansion(bay, *dac_control_bits);
    asm(" NOP");
    asm(" NOP");

    return;
}

/******* << END CMD_SRG.C >> *******/
