#include <stdio.h>
#include <ctype.h>
#include <mriext.h>
#include "timer.h"
#include "switches.h"
#include "intface.h"
#include "tiocntl.h"
#include "front.h"
#include "ecat.h"
#include "ehv.h"
#include "data.h"

extern FILE    *destination;
extern uchar      calibration;
extern uint    ghostrelay;
extern P_PARAMT   paraml[3];

extern const WAVDATA wavdata[];

extern MODDATA moddata[];
extern SRGDATA srgdata;
extern SRGCHEAT	srgCheat;
extern CPLDATA cpldata;
extern DISDATA display;
extern PKDATA  peakdata;
extern EHVDATA ehvdata;
extern EHVINFO ehvinfo[];
extern volatile uchar   AD_HIT;
extern volatile ushort  PEAK_LD5;
extern volatile ushort  PEAK_RELAYS;
extern volatile ushort  ldo_state;

extern volatile CHGDATA chgdata;

int   E510_Happened;
void  SRG_HVKill(void);

/*
 * (JMM - 02/25/94)
 * Experimental stuff for PEAK detector work
 *
 * New stuff added and modified by FD, Aug 95.
 */
extern volatile ushort  QSM_rx[];
static short   vpeaktmp;
static short   ipeaktmp;
static uchar   surge_detected;
static uchar   SRG_peakwindow;
static uchar   SRG_peaktrigger;
void           SRG_PeakWindow(void);
void           SRG_PeakTrigger(void);

void  SRG_InitSurgeDetected( void )
{
	surge_detected = FALSE;
}

uchar SRG_GetSurgeDetected( void )
{
	return ( surge_detected );
}

void  SRG_PeakWindow(void)
{
	SRG_peakwindow = 0;
}

void  SRG_PeakTrigger(void)
{
	SRG_peaktrigger = 0;
}

char  SRG_ReadPeakDetector( uchar polarity )
{
	/* This function:  1) sets the peak relays for either
		positive or negative peak measurement; 2) reads
		both the voltage and current peaks for that
		polarity; 3) converts the peak readings from 12 bit
		to 16 bit values; 4) returns values by redefining
		the global static variables vpeaktmp and ipeaktmp.

		NOTE:  the peak relays must be activated at least
		10 milliseconds before this function is called, in
		order to allow the relays to turn on. */

	ushort   cntr;
	char     returnval = TRUE;

	/* Enable POSITIVE or NEGATIVE voltage and current peak relays. */
	if ( polarity == POSITIVE )
	{
		PEAK_RELAYS = 0x7A00;
	}
	else
	{
		PEAK_RELAYS = 0xFA00;
	}

	if ( returnval )
	{
		/* Wait for end of active A/D converter measurement cycle. */
		AD_HIT = cntr = 0;
		while(AD_HIT == 0)
		{
			if(++cntr >= 10000)
				returnval = FALSE;
		}
	}

	if ( returnval )
	{
		/* Now wait for one FULL A/D converter measurement cycle. */
		AD_HIT = cntr = 0;
		while(AD_HIT == 0)
		{
			if(++cntr >= 10000)
				returnval = FALSE;
		}
	}

	if ( returnval )
	{
		/* Read voltage peak (ch - 4) and current peak (ch - 5). */
		vpeaktmp = (QSM_rx[5] & 0x0FFF);
		ipeaktmp = (QSM_rx[6] & 0x0FFF);

		/* Convert values from 12 bits to 16 bits. */
		if(vpeaktmp & 0x0800)
			vpeaktmp |= 0xF000;

		if(ipeaktmp & 0x0800)
			ipeaktmp |= 0xF000;
	}

	return( returnval );
}


char  SRG_DetectSurgeStart( int v_ambient, int i_ambient )
{
	char  returnval = FALSE;

	if ( ( (abs(vpeaktmp)) > (SURGE_PEAK_THRESHOLD + abs(v_ambient)) )
		||
		  ( (abs(ipeaktmp)) > (SURGE_PEAK_THRESHOLD + abs(i_ambient)) ) )
	{
		returnval = TRUE;
	}

	return( returnval );
}


void  SRG_ResetModules(
uchar extent)
{
	char  tvar;

	if(extent)
	{
		srgdata.relay     = &ghostrelay;
		srgdata.expansion = &ghostrelay;
		srgdata.voltage   = 0;
		srgdata.waveform  = 0;
		srgdata.address   = GHOST_BOX;
		srgdata.delay     = MIN_SRGDELAY;
		cpldata.outaddr   = GHOST_BOX;
		cpldata.phasemode = 0;
		cpldata.angle     = 0;
	}

	chgdata.flag = CHG_IDLE;
	chgdata.time = -1;

	TIO_SRGDisable();
	for(tvar = MAX_MODULES-1; tvar >= 0; tvar--)
    {
		if(moddata[tvar].modinfo.typePrimary & TYPE_SRG)
        {
            if ( extent )
                srgdata.address = tvar;
            if ( ( moddata[tvar].modinfo.id == E522S_BOX )
              || ( moddata[tvar].modinfo.id == E521S_BOX ) )
            {
                moddata[tvar].relay = RLY_RD26;
                E52x_HVProgram(tvar,0,0);
            }
            else 
            {
                moddata[tvar].relay = 0x00000000;

                // Shut off HV supply for E507
                if ( moddata[tvar].modinfo.id == E507_BOX )
                    E507_HVProgram( tvar, 0, 0 );
            }
            ECAT_WriteRelays(tvar,moddata[tvar].relay);
        

            // Shut off E505B EUT power when resetting modules
            if ( ( moddata[tvar].modinfo.id == E505B_BOX ) && ( tvar == cpldata.outaddr ) )
                blue_off();
        
        }
    }

	if(srgdata.address != GHOST_BOX)
		srgdata.delay = SRG_MinimumDelay(srgdata.address,srgdata.waveform);
}

void  SRG_FindModules(void)
{
	char     baycnt;
	MODINFO  *modptr;
	
	srgdata.srgflag = 0;
	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if((modptr->typePrimary & TYPE_SRG) && (modptr->id < MAX_SRGBOXES))
			srgdata.srgflag = TRUE;
	}
}

/*
 * Returns:
 *     0          - Successful (AC or DC Okay, or no need to check)
 *    ERR_SRGBLUE - NO BLUE button
 *    ERR_SRGDC   - NO DC (DC mode)
 *    ERR_SRGAC   - NO AC (AC mode)
 */
uchar ECAT_CheckEUT(
uchar phaseChannel,
uchar setdisplay)
{
	short freq;
	uchar errvalue;

/*
 * If there is NO SOURCE or OUTPUT module selected then there is NO need
 * to verify the EUT source
 */
	if((cpldata.srcaddr >= MAX_MODULES) || (cpldata.outaddr >= MAX_MODULES))
		return(0);

/*
 * IF !EUTREQUEST
 *    If the EUT is not requested then the EUT need not be enabled
 *    only if the phase mode is RANDOM or DC
 *    If the phase mode is in the L1 to L3 range, then the EUT must be
 *    enabled and valid
 * ELSE
 *    If the EUT is being requested then the EUT must be enabled and
 *    VALID regardless of the mode
 *    The EUT is considered valid if the DC option board is installed
 *    and the EUT button is enabled.  There is no method for checking
 *    for an actual DC signal.  This could potentially be dangerous!
 *    Otherwise, the EUT is considered valid if the AC frequency falls
 *    within certain limits, currently 40 to 70 Hertz
 */
	errvalue = 0;
	if(!cpldata.eutrequest)
	{
		if(cpldata.phasemode)
			errvalue = ERR_SRGBLUE;
	}
	else
	{
		if(!cpldata.eutblue)
			errvalue = ERR_SRGBLUE;
		else
		{
#ifndef HACK_400Hz  /* Don't check for AC if hack is defined. */
            /* Try reading frequency up to two times to get a valid in-range reading */
            uchar try = 1;
            uchar max_tries = 3;
            freq = 0;
            do {
                freq = (short)TPU_ECAT_freq(phaseChannel);
            } while ( ( try++ < max_tries ) && ( ( freq < 40 ) || ( freq > 70 ) ) );

            /* Fail after two tries */
			if ( try > max_tries )
			{
				if((!cpldata.phasemode) && (moddata[(char)(cpldata.srcaddr)].modinfo.options & OPT_DC_SURGE))
				{
					if(!((ECAT_ReadDINByte(cpldata.srcaddr)) & 0x02))
						errvalue = ERR_SRGAC;
				}
				else
                {
					errvalue = ERR_SRGAC;
                }
			}
#endif
		}
	}
	if(setdisplay)
	{
		switch(errvalue)
		{
			case ERR_SRGBLUE:
				display.request = DISPLAY_BAD_BLUE;
				break;
			case ERR_SRGAC:
				display.request = DISPLAY_BAD_AC;
				break;
		}
	}
	return(errvalue);
}

char  SRG_SurgeStart(void)
{
    MODDATA  *modptr;
    uint     tickcount;
    uchar    wavesave;
    uchar    tvar;

    if ( srgdata.address >= MAX_MODULES )
        return(ERR_SRGNET);

    modptr = &moddata[(char)(srgdata.address)];
    if ( !(modptr->modinfo.typePrimary & TYPE_SRG) )
        return(ERR_SRGNET);
/*
 * If the EUT power needs to be involved then make sure there is AC
 * present before proceeding any further
 */
	if ( !srgCheat.lineCheck )
	{
		if ( display.current == DISPLAY_SRG )
			tvar = ECAT_CheckEUT(PHASE_TPU,1);
		else
			tvar = ECAT_CheckEUT(PHASE_TPU,0);
		if ( tvar )
		{
			DUART_enable();
			INT_enable();
			SRG_StopSystem();
			return(tvar);
		}
	}
/*
 * If the surge network is an E501 and the waveform is set for the
 * 02/12 OHM AUTO wave then the "real" waveform is chosen according
 * to line coupling selections
 * Modification on 05/11/94 by James Murphy (JMM)
 *    An E501A module has been added so the automatic waveform code
 *    has been changed to support this
 * Modification on 06/07/94 by James Murphy (JMM)
 *    An E510A module has been added so the automatic waveform code
 *    has been changed to support this
 * Modification on 12/12/94 by James Murphy (JMM)
 *    AUTO waveform support for the E522 was never instituted.
 *    Support has been added at this time
 * Modification on 05/18/95 by James Murphy (JMM)
 *    An E521 modules has been added so the automatic waveform code
 *    has been changed to support this
 * Modification on 09/18/97 by Frank Snyder Di Cesare (FSDiC)
 *    The E501B module has been added so the automatic waveform code
 *    has been changed to support this.
 * Modification on 11/10/14 by Frank Snyder Di Cesare (FSD)
 *    The E507 module has been added so the automatic waveform code
 *    has been changed to support this.
 */
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

/*
 * The following configures general parameters for the surge process.
 * The PHASE path is programmed; the HV supply current limiting is
 * programmed to maximum; and the peak detector is initialized.
 * Modification on 10/07/94 by James Murphy (JMM)
 *    The DEFAULT value for the peak relays was changed from 0x6200
 *    to 0x0200.  This change keeps the VNOT_50 and INOT_50 relays
 *    from turning ON.  This 'solves' the floating input problem.
 *    A related change was made in the CMD_MEAS.C module as well.
 */
/*
    The peak relay DEFAULT value was changed back to 0x6200 from 0x0200.
    The reason for changing it to 0x0200 in the first place was to prevent
    excessive readings on the VI monitor outputs upon changing measurement
    modes.  This is now handled in the measurement mode change functions
    themselves.  Also, the peak relays are now initialized at the
    beginning of SRG_SurgeTrigger().  FD 12Sep95
*/
    TIO_SRGDisable();
    TIO_TOMap(srgdata.address,TO0_TOLCL0);
    TIO_TIMap(srgdata.address,TILCL0_TI0);
    TIO_TISelect(srgdata.address,TI1_SELECT,1);     /* Enable K5 Sync Line  */
    TIO_TISelect(srgdata.address,TI2_SELECT,1);     /* Enable K6 Sync Line  */

/*
 * The following turns ON/OFF network specific relays for routing
 * the surge (backplane/front panel) and changing waveform impedances
 * Modification on 06/10/94 by James Murphy (JMM)
 *    Support was added for the E510A module.  This network has a
 *    different design so it cannot be lumped in with the standard
 *    E501/E510 modules
 * Modification on 12/27/95 by James Murphy (JMM)
 *    Support was added for the E502A module.  This network is a
 *    standard E502 with the ability to couple to the new DATA couplers
 * Modification on 07/23/97 by Frank Snyder Di Cesare (FSD)
 *    Support was added for the E502B module.  This network is functionally
 *    equivalent to an E502A; the distinction is made because the E502B
 *    supports the IEC 10x700 (same as the E502A) as well as the FCC 9x720
 *    waveforms (both are within the operating spec for the 10x700 waveform).
 * Modification on 09/18/97 by Frank Snyder Di Cesare (FSDiC)
 *    Support was added for the E501B module.  This network is functionally
 *    equivalent to an E501A; the distinction is made because the E501B
 *    supports updated and/or additional standards which earlier E501 variants
 *    do not.
 * Modification on 11/10/14 by Frank Snyder Di Cesare (FSD)
 *    Support was added for the E507 module, whose behavior is very similar to
 *    the E510A but with a much lower voltage output.
 */
    *srgdata.relay |= LED_VOLTAGE;
    tickcount = TICKS_PER_SECOND * (SRG_MinimumDelay(srgdata.address,srgdata.waveform)-2);

    switch ( modptr->modinfo.id )
    {
        case E501_BOX:
        case E501A_BOX:
        case E501B_BOX:
        case E510_BOX:
            if ( modptr->modinfo.id == E510_BOX )
                *srgdata.relay |= (RLY_RD30);

            if ( modptr->modinfo.id == E501B_BOX )
                *srgdata.relay &= ~(RLY_RD31 | RLY_RD22);  /* Shut off K15/K16 & K4 */
            else
                *srgdata.relay &= ~(RLY_RD21 | RLY_RD22);  /* Shut off K3 & K4 */
            if ( srgdata.waveform == E501_02OHM )
            {
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
                if ( ( (cpldata.multihigh == 0) && (cpldata.outaddr != GHOST_BOX) )
                     || ( cpldata.outaddr == GHOST_BOX ) )
                {
                    if ( modptr->modinfo.id == E501B_BOX )
                        *srgdata.relay |= RLY_RD31;   /* Turn ON  K3          */
                    else
                        *srgdata.relay |= RLY_RD21;   /* Turn ON  K3          */
                }
                if ( (modptr->modinfo.id == E501A_BOX)
                  || (modptr->modinfo.id == E501B_BOX) )
                {
                    if ( cpldata.outaddr != GHOST_BOX )
                    {
                        if ( modptr->modinfo.id == E501B_BOX )
                            *srgdata.relay |= RLY_RD21;      /* Turn ON  K4A         */
                        else
                            *srgdata.relay |= RLY_RD31;      /* Turn ON  K4A         */
                    }
                    if ( (cpldata.multihigh == 0) && (cpldata.outaddr != GHOST_BOX) &&
                         (cpldata.cpllow & CPL_PEBIT) &&
                         (!moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA) )
                    {
                        if ( modptr->modinfo.id == E501B_BOX )
                            *srgdata.relay &= ~(RLY_RD31);   /* Turn OFF K3          */
                        else
                            *srgdata.relay &= ~(RLY_RD21);   /* Turn OFF K3          */
                    }
                }
                if ( (cpldata.multihigh != 0) && !(cpldata.cpllow & CPL_PEBIT) &&
                     (cpldata.outaddr != GHOST_BOX) )
                {
                    if ( modptr->modinfo.id == E501B_BOX )
                        *srgdata.relay |= RLY_RD31;         /* Turn ON  K3          */
                    else
                        *srgdata.relay |= RLY_RD21;         /* Turn ON  K3          */
                }
            }
            else  // srgdata.waveform == E501_12OHM
            {
				if ( modptr->modinfo.id == E501B_BOX )
				{	
					*srgdata.relay |= RLY_RD31;  // Turn ON K15/K16 for all coupling and non-coupling modes for 12 Ohms for E501B
				}
				else
				{
					if ( cpldata.outaddr == GHOST_BOX )
						*srgdata.relay |= RLY_RD21;  // Turn ON K3
				}
            }
            break;

        case E507_BOX:
            // 2 Ohm/12 Ohm relay RLY_RD22
            if (srgdata.waveform == E501_02OHM)
                *srgdata.relay |= RLY_RD22;  // Turn ON K4
            else
                *srgdata.relay &= ~RLY_RD22;  // Turn OFF K4
            // RLY_RD21 ON for Direct output and 2 Ohm Line-to-Line coupling, otherwise OFF
            if ( cpldata.outaddr != GHOST_BOX )
            {
                if ( (srgdata.waveform == E501_02OHM) && (!(cpldata.cpllow & CPL_PEBIT)) )
                    *srgdata.relay |= RLY_RD21;  // Turn ON K3
                else
                    *srgdata.relay &= ~RLY_RD21;  // Turn ON K3
            }
            else  // Direct output
            {
                *srgdata.relay |= RLY_RD21;  // Turn ON K3
            }
            break;

        case E510A_BOX:
            if ( srgdata.waveform == E501_02OHM )
                *srgdata.relay |= RLY_RD30;            /* Turn ON  K7          */
            if ( cpldata.outaddr != GHOST_BOX )
            {
                if ( (srgdata.waveform == E501_02OHM) && (!(cpldata.cpllow & CPL_PEBIT)) )
                    *srgdata.relay |= RLY_RD22;         /* Turn ON K4           */
                if ( (moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA) &&
                     (cpldata.multihigh==0) && (cpldata.cpllow & CPL_PEBIT) )
                    *srgdata.relay |= RLY_RD22;         /* Turn ON K4           */
                if ( srgdata.waveform == E501_02OHM )
                    if ( (cpldata.cplhigh & (CPL_L1BIT | CPL_L2BIT | CPL_L3BIT | CPL_NUBIT))
                         != (CPL_L1BIT | CPL_L2BIT | CPL_L3BIT | CPL_NUBIT) )
                        *srgdata.relay |= RLY_RD31;      /* Turn ON  K6          */
            }
            else
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
            break;
        case E522S_BOX:
        case E521S_BOX:
            if ( srgdata.waveform == E501_02OHM )
                *srgdata.relay |= (RLY_RD22);          /* Turn ON  K4          */
            else
                *srgdata.relay &= ~(RLY_RD22);         /* Turn OFF K4          */
            break;
        case E502_BOX:
        case E502H_BOX:
        case E502A_BOX:
        case E502B_BOX:
        case E502C_BOX:
            *srgdata.relay &= ~(RLY_RD22 | RLY_RD21); /* Turn OFF K3 and K4   */
            switch ( srgdata.waveform )
            {
                case E502_100700:
                    *srgdata.relay |= RLY_RD22;         /* Turn ON  K4          */
                case E502_10700:
                    *srgdata.relay |= RLY_RD21;         /* Turn ON  K3          */
                    break;
            }
            break;
        case E502K_BOX:
            *srgdata.relay &= ~(RLY_RD27 | RLY_RD28);
            if ( srgdata.waveform == E502K_1250US )
                *srgdata.relay |= RLY_RD28;            /* Turn ON  K12/K13     */
            else
                *srgdata.relay |= RLY_RD27;            /* Turn ON  K10/K11     */
            break;
        case E511_BOX:
            if ( cpldata.outaddr != GHOST_BOX )
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
            else
                *srgdata.relay &= ~(RLY_RD22);         /* Turn OFF K4          */
            if ( cpldata.outaddr != GHOST_BOX &&
                 (cpldata.multihigh != 0) && (cpldata.cpllow & CPL_PEBIT) )
                *srgdata.relay &= ~(RLY_RD21);         /* Turn OFF K3          */
            else
                *srgdata.relay |= RLY_RD21;            /* Turn ON  K3          */
            break;
        case E503_BOX:
            if ( srgdata.waveform == E503_500A )
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
            else  // srgdata.waveform == E503_200A
                *srgdata.relay &= ~(RLY_RD22);         /* Turn OFF K4          */
            break;
        case E503A_BOX:
            // E503A box only; these relays are what differentiate an E503A from an E503
            *srgdata.relay &= ~(RLY_RD28);  // default state (activate for 200A at lower voltages)
            *srgdata.relay &= ~(RLY_RD31);  // default state (activate for 500A at higher voltages
            if ( srgdata.waveform == E503_500A )
            {
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
                if ( abs(srgdata.voltage) <= 2250 )
                    *srgdata.relay |= RLY_RD31;
            }
            else  // srgdata.waveform == E503_200A
            {
                *srgdata.relay &= ~(RLY_RD22);         /* Turn OFF K4          */
                if ( abs(srgdata.voltage) <= 2750 )
                    *srgdata.relay |= RLY_RD28;
            }
            break;
        case E506_BOX:
            *srgdata.relay &= ~(RLY_RD21 | RLY_RD22 | RLY_RD28 | RLY_RD29 | RLY_RD30);
            switch ( srgdata.waveform )
            {
                case E506_800V:
                    *srgdata.relay |= RLY_RD28;
                case E506_1500V:
                    *srgdata.relay |= (RLY_RD29 | RLY_RD30);
                    break;
                case E506_2500V:
                    *srgdata.relay |= RLY_RD21;
                    if ( cpldata.outaddr != GHOST_BOX )
                        *srgdata.relay |= RLY_RD22;
                    else
                        *srgdata.relay |= (RLY_RD29 | RLY_RD30);
                    break;
                case E506_5000V:
                    if ( cpldata.outaddr != GHOST_BOX )
                        *srgdata.relay |= (RLY_RD22 | RLY_RD29);
                    else
                        *srgdata.relay |= (RLY_RD29 | RLY_RD30);
                    break;
            }
            break;
        case E506_4W_BOX:
            *srgdata.relay &= ~(RLY_RD27 | RLY_RD28);
            switch ( srgdata.waveform )
            {
                case E506_800V:
                    *srgdata.relay |= RLY_RD27;  /* K10, K13, K16, & K19 */
                case E506_1500V:
                    /* No action */
                    break;
                case E506_2500V:
                    *srgdata.relay |= (RLY_RD27 | RLY_RD28);
                    break;
                case E506_5000V:
                    *srgdata.relay |= RLY_RD28;  /* K11, K14, K17, & K20 */
                    break;
                default:
                    break;
            }
            break;
        case E505_BOX:
            *srgdata.relay &= ~(RLY_RD21 | RLY_RD22 | RLY_RD27 | RLY_RD28);
            switch ( srgdata.waveform )
            {
                case E505_1500V:
                    *srgdata.relay |= RLY_RD21;
                    break;
                case E505_800V200A:
                    *srgdata.relay |= RLY_RD27;
                case E505_800V100A:
                    *srgdata.relay |= RLY_RD28;
                    break;
                case E505_2500V:
                    *srgdata.relay |= RLY_RD22;
                    break;
            }
            break;
        case E505A_BOX:
            *srgdata.relay &= ~(RLY_RD21 | RLY_RD22 | RLY_RD28);
            switch ( srgdata.waveform )
            {
                case E505A_1500V:
                    *srgdata.relay |= RLY_RD21;
                    break;
                case E505A_800V100A:
                    *srgdata.relay |= RLY_RD28;
                    break;
                case E505A_2500V:
                    *srgdata.relay |= RLY_RD22;
                    break;
            }
            break;
        case E505B_BOX:
            /* 
                RLY_RD21    RLY_RD22    Function
                --------    --------    --------
                ON          OFF         Output through K5A for 1.5 kV waveform
                OFF         ON          Output through K5B for 800 V waveform
                OFF         OFF         Output through K5 for 2.5 kV waveform
             
                RLY_RD27    RLY_RD28    Function
                --------    --------    --------
                ON          OFF         Select 1.5 kV waveform for Direct output
                OFF         ON          Select 800 V waveform for Direct output
                OFF         OFF         No Direct output (use for 2.5 kV waveform output to INTERNAL Coupler/Decoupler Network)
            */
            *srgdata.relay &= ~(RLY_RD21 | RLY_RD22 | RLY_RD27 | RLY_RD28);
            switch ( srgdata.waveform )
            {
                case E505B_1500V:
                    *srgdata.relay |= (RLY_RD21 | RLY_RD27);  /* RLY_RD21 - use K5A; RLY_RD27 - select 1.5 kV */
                    break;
                case E505B_800V100A:
                    *srgdata.relay |= (RLY_RD22 | RLY_RD28);
                    break;
                case E505B_2500V:
                default:
                    /* no action */
                    break;
            }
            break;
        case E504_BOX:
        case E504A_BOX:
        case E504B_BOX:
            *srgdata.relay &= ~(RLY_RD21 | RLY_RD27);    /* Turn OFF K3/K9    */
            if ( srgdata.waveform == E504_750A )
                *srgdata.relay |= (RLY_RD21 | RLY_RD27);  /* Turn ON  K3/K9    */
            else
                if ( srgdata.waveform == E504_500A )
                *srgdata.relay |= RLY_RD27;            /* Turn ON  K9       */
            if ( cpldata.outaddr == GHOST_BOX )
                *srgdata.relay |= RLY_RD22;               /* Turn ON  K4       */
            break;
        case E513_BOX:
            switch ( srgdata.waveform )
            {
                case E513_P1KVUS:
                    if ( (srgdata.voltage <= 200) && (srgdata.voltage >= -200) )
                        *srgdata.relay |= RLY_RD30;
                    break;
                case E513_P5KVUS:
                    *srgdata.relay |= RLY_RD26;
                    break;
                case E513_1KVUS:
                    *srgdata.relay |= RLY_RD23;
                    break;
                case E513_5KVUS:
                    *srgdata.relay |= RLY_RD28;
                    break;
                case E513_10KVUS:
                    break;
            }
            break;
        case E514_BOX:
            switch ( srgdata.waveform )
            {
                case E514_015A:
                    *srgdata.relay |= (RLY_RD30 | RLY_RD31);
                    break;
                case E514_060A:
                    *srgdata.relay |= (RLY_RD29 | RLY_RD31);
                    break;
                case E514_100A:
                    *srgdata.relay |= RLY_RD26;
                    break;
                case E514_250A:
                    *srgdata.relay |= RLY_RD27;
                    break;
            }
            break;
        case E509_BOX:
        case E509A_BOX:
            switch ( srgdata.waveform )
            {
                case E509_600V:
                    *srgdata.relay |= RLY_RD28;
                    break;
                case E509_1000V:
                    *srgdata.relay |= RLY_RD27;
                    break;
            }
            break;
        case E518_BOX:
            switch ( srgdata.waveform )
            {
                case E518_600V:
                    *srgdata.relay |= RLY_RD28;
                    break;
            }
            break;
        case E508_BOX:
        case E515_BOX:
            break;
        default:
            return(ERR_SRGNET);
    }
    ECAT_WriteRelays(srgdata.address,*srgdata.relay);

    TPU_delay(200);

/*
 * The surge DUMP relay is opened at this point to allow the HIGH
 * VOLTAGE to charge the surge capacitor(s)
 * Modification on 06/10/94 by James Murphy (JMM)
 *    The E510A requires an additonal DUMP relay for voltage reasons.
 *    Support has been added to turn this relay (K9 / RLY_RD29) ON along
 *    with the standard DUMP relay (K8, RLY_RD18)
 * Modification on 01/31/95 by James Murphy (JMM)
 *    Support for the E508/E508-12P has been added
 */
    switch ( modptr->modinfo.id )
    {
        case E502K_BOX:
            if ( srgdata.waveform == E502K_1250US )
                *srgdata.relay |= RLY_RD29;            /* Turn ON  K9          */
            else
                *srgdata.relay |= RLY_RD18;            /* Turn ON  K8          */
            break;
        case E505_BOX:
            *srgdata.relay |= (RLY_RD31 | RLY_RD18);  /* Turn ON  K8 / K9     */
            break;
        case E505A_BOX:
        case E505B_BOX:
            *srgdata.relay |= RLY_RD18;               /* Turn ON  K8          */
            break;
        case E510A_BOX:
        case E521S_BOX:
        case E522S_BOX:
            *srgdata.relay |= (RLY_RD18 | RLY_RD29);
            break;
        case E508_BOX:
            *srgdata.relay |= RLY_RD18;               /* Turn ON  K8 (E508)   */
            if ( (srgdata.waveform == E508_25A) && (cpldata.E50812Paddr < MAX_MODULES) )
            {
                *srgdata.relay |= (RLY_RD21 | RLY_RD28);
                moddata[(char)(cpldata.E50812Paddr)].relay |= (LED_VOLTAGE | RLY_RD18);
                TIO_SRGDisable();
                TIO_TOMap(cpldata.E50812Paddr,TO0_TOLCL0);
                TIO_TIMap(cpldata.E50812Paddr,TILCL0_TI0);
                ECAT_WriteRelays(cpldata.E50812Paddr,moddata[(char)(cpldata.E50812Paddr)].relay);
                TPU_write(3,1);
            }
            break;
        default:
            *srgdata.relay |= RLY_RD18;               /* Turn ON  K8          */
    }
    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
    ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
    TPU_delay(200);

/*
 * The POLARITY relays are selected.  CAUTION must be taken with
 * these so as to ensure both do not get turned on simultaneously
 * Modification on 06/10/94 by James Murphy (JMM)
 *    The E510A module uses RLY_RD19 for both POSITIVE and
 *    NEGATIVE charging.  The charging relay setup has been changed
 *    to support this
 * Modification on 05/23/95 by James Murphy (JMM)
 *    The E52x series modules use the polarity settings on the output
 *    and not the input.  Therefore, they will be set later.
 * The E507 uses RD19 to charge, and sets output-side polarity relays 
 *    (RLY_RD26 and RLY_RD27) post-charge, similar to the E510A. 
 */
    *srgdata.relay &= ~(RLY_RD19 | RLY_RD20); /* Turn OFF K1/K2 */
    switch ( modptr->modinfo.id )
    {
        case E507_BOX:
        case E510A_BOX:
            *srgdata.relay |= RLY_RD19;      /* Turn ON K1 */
            break;
        case E515_BOX:
            *srgdata.relay |= RLY_RD19;      /* Turn ON K1A, K1B, & K1C */
            if ( srgdata.voltage < 0 )
                *srgdata.relay |= RLY_RD20;   /* Turn ON K10 (Imon XFMR polarity) */
            break;
        default:
            if ( srgdata.voltage >= 0 )
                *srgdata.relay |= RLY_RD19;   /* Turn ON K1 */
            else
                *srgdata.relay |= RLY_RD20;   /* Turn ON K2 */
            break;
    }
    ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
    TPU_delay(200);

/*
 * Enable the HIGH VOLTAGE supply and set it to the correct value.
 * Also, a TPU monitoring channel is set up so as to shut off the
 * HIGH VOLTAGE within 10 seconds of turn on.
 */
    SRG_HVProgram(srgdata.address,srgdata.waveform,srgdata.voltage,1);

    /* For the E515 only:  turn on the Ignatron high voltage supply 5 seconds
       (EARLY_CHECK_TIME) before the end of the charge process.  For all other
       modules, just set interrupt following the whole charge time (tickcount). */
    if ( modptr->modinfo.id == E515_BOX )
        chgdata.checker = TPU_delay_int((tickcount-EARLY_CHECK_TIME),SRG_EarlyChargeCheck);
    else
        chgdata.checker = TPU_delay_int(tickcount,SRG_ChargeCheck);

    srgdata.waveform = wavesave;

    return(0);
}

void  SRG_E510LineSync(void)
{
	*srgdata.relay |= RLY_RD28;
	ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
	ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
	E510_Happened = 1;
}

char  SRG_SurgeTrigger(void)
{
    int i;  /**** temporary ****/
    MODDATA  *modptr;
    char     tvar;
    uchar    dataCoupler;
    ushort   K5delay;
    ushort   K6delay;
    int      v1,b0,b1;
    int      tvar1,tvar2;

    /* Vars for Peak Detector measurements. */
    uchar    peaktpuchan;
    int      peakcnt;
    int      vpeakpos[30];
    int      vpeakneg[30];
    int      ipeakpos[30];
    int      ipeakneg[30];
    int      vpeakpos_sumxy;
    int      vpeakneg_sumxy;
    int      ipeakpos_sumxy;
    int      ipeakneg_sumxy;
    int      xpeakxxx_sumxi;
    int      xpeakxxx_sumxx;
    int      vpeakpos_sumyi;
    int      vpeakneg_sumyi;
    int      ipeakpos_sumyi;
    int      ipeakneg_sumyi;
    int      vpeakpos_ambient;
    int      vpeakneg_ambient;
    int      ipeakpos_ambient;
    int      ipeakneg_ambient;
    uchar    surge_start;

    if ( srgdata.address >= MAX_MODULES )
        return(ERR_SRGNET);

    modptr = &moddata[(char)(srgdata.address)];
    if ( !(modptr->modinfo.typePrimary & TYPE_SRG) )
        return(ERR_SRGNET);

/*
 * The polarity relays are turned off at this point and then the HIGH
 * VOLTAGE is shut off and disabled.  Also, the discharge relay section
 * is done (what function does this serve??)
 */
    if ( chgdata.monitor )
        TPU_kill(chgdata.monitor);                   /* Turn OFF HV watch    */
    chgdata.monitor = 0;

    switch ( modptr->modinfo.id )
    {
        case E521S_BOX:
        case E522S_BOX:
            *srgdata.relay |= RLY_RD23;
            ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
            TPU_delay(200);
            *srgdata.relay |= RLY_RD24;
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(200);
            break;
        case E515_BOX:
            /* For E515, RD20 is the Imon polarity relay, and must maintain its
               present state until after the surge occurs. */
            *srgdata.relay &= ~RLY_RD19;                 /* Turn OFF K1A/K1B/K1C */
            break;
        default:
            *srgdata.relay &= ~(RLY_RD19 | RLY_RD20);    /* Turn OFF K1/K2       */
            break;
    }

    if ( (modptr->modinfo.id == E506_BOX) && (srgdata.voltage < 0) )
        *srgdata.relay |= RLY_RD23;

    ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));

    /*
     * If the output is heading for a coupler/decoupler then the
     * coupling relays must be set at this time.
     */
    dataCoupler = 0;
    if ( cpldata.outaddr != GHOST_BOX )
    {
        if ( moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA )
            dataCoupler = 1;
        SRG_SetCouplerHardware();
    }

    /* If this is an E501B and output is to a coupler, then set the undershoot clamp relays */
    if ( modptr->modinfo.id == E501B_BOX )
    {
        *srgdata.relay &= ~(RLY_RD27 | RLY_RD28); /* Reset both clamp relays (OFF) */
        if ( cpldata.outaddr != GHOST_BOX )
        {
            if ( srgdata.voltage >= 0 )
                *srgdata.relay |= RLY_RD27;   /* Turn ON relay for positive clamp */
            else
                *srgdata.relay |= RLY_RD28;   /* Turn ON relay for negative clamp */
        }
    }
    ECAT_WriteRelays(srgdata.address,*srgdata.relay);

    peaktpuchan = 2;                                /* K5 Sync Line   */
    switch ( modptr->modinfo.id )
    {
        case E513_BOX:
            peaktpuchan = 4;                          /* K6 Sync Line   */
            TPU_delay(2000);
            break;
        case E514_BOX:
            if ( (srgdata.waveform == E514_015A) || (srgdata.waveform == E514_060A) )
                peaktpuchan = 4;                       /* K6 Sync Line   */
            TPU_delay(600);
            break;
        case E507_BOX:
            TPU_delay(200);  // no line sync; not sure why more than RLY_RD19/K1 relay closure time is necessary
            break;
        default:
            TPU_delay(1100);
            break;
    }

    /*
     * Prepare for the surge by setting up the peak detector relays and
     * shutting down all interrupts (except TPU) and disabling the backplane
     * communications.  THIS IS VERY IMPORTANT as it "protects" the firmware
     * and peak detector GAL from the SURGE NOISE!!!
     * JMM - yada, shut down high voltage now except for E52x series
     * JMM - 05/30/95 - 10 ms delay after setting PEAK relays
     */
    /*
     * Also, calculate ambient positive and negative voltage and current peak
     * levels, to be used later on in the function for surge wave detection.
     * FD - 31 July 95
     */
    PEAK_RELAYS = 0x7A00;
    TPU_delay(10); /* Allow the peak relays time to activate. */
    TPU_delay(25); /* Allow the peak detectors to detect the peaks of any
                            ambient voltage or current, such as coupled AC (worst
                            case is 40 Hz AC, with a period of 25 ms). */
    vpeakpos_ambient = 0;
    vpeakneg_ambient = 0;
    ipeakpos_ambient = 0;
    ipeakneg_ambient = 0;
    vpeaktmp = 0;
    ipeaktmp = 0;

    /* For ambient peak levels, read MAX_AMBIENT_SAMPLE samples of
        both positive and negative voltage and current peaks, and then
        use the average for each category. */
    for ( tvar = 1; tvar <= MAX_AMBIENT_SAMPLE; tvar++ )
    {
        if ( !SRG_ReadPeakDetector( POSITIVE ) )        /* Read Vpos & Ipos */
            return(ERR_SRGREAD);
        vpeakpos_ambient += vpeaktmp/MAX_AMBIENT_SAMPLE;   /* Calc Vpos avg */
        ipeakpos_ambient += ipeaktmp/MAX_AMBIENT_SAMPLE;   /* Calc Ipos avg */

        if ( !SRG_ReadPeakDetector( NEGATIVE ) )        /* Read Vneg & Ineg */
            return(ERR_SRGREAD);
        vpeakneg_ambient += vpeaktmp/MAX_AMBIENT_SAMPLE;   /* Calc Vneg avg */
        ipeakneg_ambient += ipeaktmp/MAX_AMBIENT_SAMPLE;   /* Calc Ineg avg */
    }
    /* DUMP the Peak Caps now, then reset the peak detector circuit
        as soon as the surge is triggered (or immediately before, in
        the case of solid state relays).  FD 29Dec95 05Mar96*/
    PEAK_RELAYS = 0x6200;

    /* Start peak detector circuit (open Peak Cap dump relays) for all modules
        using solid state relays (peak detector circuit will be restarted after
        the K5 surge relay has been triggered for modules with electromechanical
        relays, so that intermediate relay noise cannot collect in the peak
        detector circuitry).  First wait for 10 ms for the peak detector to finish
        discharging (it was dumped immediately above here).  FD 05Mar96 */
    switch ( modptr->modinfo.id )
    {
        /* All modules with solid state relays go here */
        case E522S_BOX:
        case E521S_BOX:
            TPU_delay(10); /* Allow the peak relays time to discharge. */
            PEAK_RELAYS = 0x7A00;
            TPU_delay(10); /* Allow the peak relays time to activate. */
            break;

            /* All modules with electromechanical relays default to here */
        default:
            /* Do nothing - peak detectors will be turned on just before surge
            occurs. */
            break;
    }

    DUART_disable();
    INT_disable();
    if ( (modptr->modinfo.id != E522S_BOX) && (modptr->modinfo.id != E521S_BOX) )
        SRG_HVKill();


    /*
     * The surge happens here! Various relays are turned ON/OFF depending
     *    on the network/waveforms used. Also, phase-based surge is computed
     * and accomplished at this point
     * Modification on 06/10/94 by James Murphy (JMM)
     *    Support has been added for the E510A module. Note that there are
     *    major differences between the old E510 and the new E510A
     * Modification on 01/06/95 by James Murphy (JMM)
     *    Support has been added for DATA coupler types
     * Modification on 01/25/95 by James Murphy (JMM)
     *    Support has been added for the E508 surge network
     * Modification on 05/22/95 by James Murphy (JMM)
     *    The E522 support code has been rewritten and an
     *    E521 has also been added
     */
    switch ( modptr->modinfo.id )
    {
        case E522S_BOX:
        case E521S_BOX:
            K5delay = 0; /* No delay for solid state relays */
			if ( !srgCheat.lineCheck )
			{
				if ( display.current == DISPLAY_SRG )
					tvar = ECAT_CheckEUT(PHASE_TPU,1);
				else
					tvar = ECAT_CheckEUT(PHASE_TPU,0);
				if ( tvar )
				{
					DUART_enable();
					INT_enable();
					SRG_StopSystem();
					return(tvar);
				}
			}
            if ( cpldata.phasemode && !dataCoupler && !srgCheat.lineCheck )
            {
                TPU_ECAT_phase(PHASE_TPU,5,cpldata.angle,K5delay,1,0x00);
                while ( !TPU_ECAT_phase_done() )
                    ;
                TPU_write(5,0);      /* Trigger OFF */
            }
            else
                TPU_write(5,1);      /* Trigger ON  */
            break;

        case E507_BOX:
            K5delay = 0; /* No delay required because module does not have line sync capability */

            if ( !srgCheat.lineCheck )
            {
                if ( display.current == DISPLAY_SRG )
                    tvar = ECAT_CheckEUT(PHASE_TPU,1);
                else
                    tvar = ECAT_CheckEUT(PHASE_TPU,0);
                if ( tvar )
                {
                    DUART_enable();
                    INT_enable();
                    SRG_StopSystem();
                    return(tvar);
                }
            }

            /* Set output-side polarity relays.  This module has no dedicated
               output connect relays; the polarity relays act as Direct
               (front panel) connect relays, and this module has no CDN
               capability. */

            if ( srgdata.voltage < 0 )
            {
                *srgdata.relay &= ~RLY_RD26;  // Turn OFF K10
                *srgdata.relay |= RLY_RD27;  // Turn ON K11
            }
            else
            {
                *srgdata.relay |= RLY_RD26;  // Turn ON K10
                *srgdata.relay &= ~RLY_RD27;  // Turn OFF K11
            }
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(200);

            /* Fire using mercury relay for lower voltages and SCR for higher voltages.
                This mercury relay is much smaller than in other modules and can't
                withstand the current for higher voltages, and the SCR on-resistance
                losses increase at lower voltages. */
            if ( abs(srgdata.voltage) < 50 )
            {
                *srgdata.relay |= RLY_RD31;
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            }
            else
            {
                TPU_write(5,1);
            }
            break;

        case E510A_BOX:
            if ( cpldata.outaddr != GHOST_BOX )
            {
                K5delay = modptr->modinfo.calinfo.data[CAL_K5OFFSET] * 4;
                K6delay = modptr->modinfo.calinfo.data[CAL_K6OFFSET] * 4;
                if ( (abs(srgdata.voltage)) > 1000 )
                    K5delay -= (((abs(srgdata.voltage) - 1000) * 1650) / 5000);
				if ( !srgCheat.lineCheck )
				{
					if ( display.current == DISPLAY_SRG )
						tvar = ECAT_CheckEUT(PHASE_TPU,1);
					else
						tvar = ECAT_CheckEUT(PHASE_TPU,0);
					if ( tvar )
					{
						DUART_enable();
						INT_enable();
						SRG_StopSystem();
						return(tvar);
					}
				}
                if ( cpldata.phasemode && !dataCoupler && !srgCheat.lineCheck )
                {
                    /* Is this OKAY to use TPU-7 for NEGATIVE surges? */
                    if ( srgdata.voltage < 0 )
                        TPU_ECAT_phase(PHASE_TPU,7,0,K6delay,1,SRG_E510LineSync);
                    else
                        TPU_ECAT_phase(PHASE_TPU,3,0,K6delay,1,0x00);
                    while ( !TPU_ECAT_phase_done() )
                        ;
                    TPU_delay(20);          /* Wait for relay to finish */

                    TPU_ECAT_phase(PHASE_TPU,5,cpldata.angle,K5delay,1,0x00);
                    while ( !TPU_ECAT_phase_done() )
                        ;
                }
                else
                {
                    if ( srgdata.voltage < 0 )
                    {
                        *srgdata.relay |= RLY_RD28;      /* Turn ON  K13         */
                        ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                        ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                    }
                    else
                        TPU_write(3,1);                  /* Turn ON  K12         */
                    TPU_delay(50);                      /* Relay settling...    */
                    TPU_write(5,1);                     /* Turn on  K5!!!       */
                }
            }
            else
            {
                if ( srgdata.voltage < 0 )
                    *srgdata.relay |= RLY_RD27;         /* Turn ON  K11         */
                else
                    *srgdata.relay |= RLY_RD26;         /* Turn ON  K10         */
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                TPU_delay(75);
                TPU_write(5,1);                        /* Turn ON  K5!!!       */
            }
            break;
        case E508_BOX:
            if ( (srgdata.waveform == E508_25A) && (cpldata.E50812Paddr < MAX_MODULES) )
            {
                moddata[(char)(cpldata.E50812Paddr)].relay |= (RLY_RD26 | RLY_RD23);
                ECAT_WriteRelay1(cpldata.E50812Paddr,(moddata[(char)(cpldata.E50812Paddr)].relay >> 16));
                ECAT_TimerSequencer(cpldata.E50812Paddr,(moddata[(char)(cpldata.E50812Paddr)].relay >> 24));
                ECAT_TimerSequencer(cpldata.E50812Paddr,(moddata[(char)(cpldata.E50812Paddr)].relay >> 24));
            }
            else
            {
                if ( (cpldata.outaddr != GHOST_BOX) && (dataCoupler) )
                    *srgdata.relay |= (RLY_RD27 | RLY_RD28 | RLY_RD22);
                else
                    *srgdata.relay |= (RLY_RD26);
            }
            ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(50);                         /* Relay settling...    */
            TPU_write(5,1);                        /* Turn on  K5!!!       */
            /* TPU_delay(20); ***Removed 12 Aug 97 FSDiC */
            break;
        case E510_BOX:
            if ( srgdata.voltage < 0 )
                TPU_delay(900);
        case E501_BOX:
        case E501A_BOX:
        case E501B_BOX:
        case E503_BOX:
        case E503A_BOX:
        case E504_BOX:
        case E504A_BOX:
        case E504B_BOX:
        case E505_BOX:
        case E505A_BOX:
        case E505B_BOX:
        case E506_BOX:
        case E506_4W_BOX:
        case E511_BOX:
            if ( cpldata.outaddr != GHOST_BOX )  // output to coupler
            {
                if ( (modptr->modinfo.id == E505_BOX) && (srgdata.waveform != E505_2500V) )
                    return(ERR_SRGNET);
                if ( (modptr->modinfo.id == E505A_BOX) && (srgdata.waveform != E505A_2500V) )
                    return(ERR_SRGNET);
                if ( (modptr->modinfo.id == E505B_BOX) && (srgdata.waveform != E505B_2500V) )
                    return(ERR_SRGNET);
                if ( (modptr->modinfo.id == E506_BOX) && (srgdata.waveform != E506_2500V) &&
                     (srgdata.waveform != E506_5000V) )
                    return(ERR_SRGNET);
                if ( modptr->modinfo.id == E506_4W_BOX )
                    return(ERR_SRGNET);
                K5delay = modptr->modinfo.calinfo.data[CAL_K5OFFSET] * 4;
                K6delay = modptr->modinfo.calinfo.data[CAL_K6OFFSET] * 4;
                if ( (abs(srgdata.voltage)) > 1000 )
                    K5delay -= (((abs(srgdata.voltage)-1000) * 1650)/5000);
                *srgdata.relay &= ~(RLY_RD26);         /* Turn OFF K11         */
                if ( (modptr->modinfo.id != E505_BOX)
                     && (modptr->modinfo.id != E504A_BOX)
                     && (modptr->modinfo.id != E504B_BOX)
                     && (modptr->modinfo.id != E501B_BOX) )
                {
                    *srgdata.relay &= ~(RLY_RD27);      /* Turn OFF K12         */
                }
				ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
				if ( !srgCheat.lineCheck )
				{
					if ( display.current == DISPLAY_SRG )
						tvar = ECAT_CheckEUT(PHASE_TPU,1);
					else
						tvar = ECAT_CheckEUT(PHASE_TPU,0);
					if ( tvar )
					{
						DUART_enable();
						INT_enable();
						SRG_StopSystem();
						return(tvar);
					}
				}
                if ( cpldata.phasemode && !dataCoupler && !srgCheat.lineCheck )
                {
                    TPU_ECAT_phase(PHASE_TPU,3,0,K6delay,1,0x00);
                    while ( !TPU_ECAT_phase_done() )
                        ;
                    TPU_delay(20);          /* Wait for relay to finish */

                    if ( modptr->modinfo.id == E510_BOX )
                    {
                        E510_Happened = 0;
                        TPU_ECAT_phase(PHASE_TPU,8,cpldata.angle,K5delay,1,SRG_E510LineSync);
                        while ( !E510_Happened )
                            ;
                        TPU_delay(32);
                        TPU_write(5,1);                  /* Turn on  K5!!!       */
                        /* TPU_delay(10); ***Removed 12 Aug 97 FSDiC */
                    }
                    else
                        TPU_ECAT_phase(PHASE_TPU,5,cpldata.angle,K5delay,1,0x00);
                    while ( !TPU_ECAT_phase_done() )
                        ;
                }
                else
                {
                    TPU_write(3,1);                     /* Turn ON  K6/K7       */
                    TPU_delay(50);                      /* Relay settling...    */
                    if ( modptr->modinfo.id == E510_BOX )
                    {
                        *srgdata.relay |= RLY_RD28;
                        ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                        ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                        TPU_delay(32);
                    }
                    TPU_write(5,1);                     /* Turn on  K5!!!       */
                    /* if(modptr->modinfo.id == E510_BOX)
                        TPU_delay(10); ***Removed 12 Aug 97 FSDiC */
                }
            }
            else  // cpldata.outaddr == GHOST_BOX (DIRECT output)
            {
                switch ( modptr->modinfo.id )
                {
                    case E501_BOX:
                    case E501A_BOX:
                    case E503_BOX:
                    case E503A_BOX:
                    case E510_BOX:
                    case E511_BOX:
                        *srgdata.relay |= RLY_RD27;      /* Turn on K12          */
                    case E501B_BOX:
                    case E504_BOX:
                    case E504A_BOX:
                    case E504B_BOX:
                    case E505_BOX:
                    case E505A_BOX:
                    case E505B_BOX:
                        *srgdata.relay |= RLY_RD26;      /* Turn on K11          */
                        break;
                    case E506_BOX:
                        if ( (srgdata.waveform == E506_800V) ||(srgdata.waveform == E506_1500V) )
                            *srgdata.relay |= RLY_RD27;   /* Turn on K3           */
                        else
                            *srgdata.relay |= RLY_RD26;   /* Turn on K13          */
                        break;
                    case E506_4W_BOX:
                        *srgdata.relay |= (RLY_RD30 | RLY_RD31);   /* Turn on K3, K12, K15, K18, & K21 */
                        break;
                }
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                TPU_delay(50);
                if ( modptr->modinfo.id == E510_BOX )
                {
                    *srgdata.relay |= RLY_RD28;
                    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                    TPU_delay(32);
                }
                TPU_write(5,1);
            }
            break;
        case E513_BOX:
            switch ( srgdata.waveform )
            {
                case E513_P1KVUS:
                    tvar1 = RLY_RD21;
                    tvar2 = RLY_RD22;
                    break;
                case E513_P5KVUS:
                case E513_1KVUS:
                    tvar1 = RLY_RD27;
                    tvar2 = RLY_RD24;
                    break;
                case E513_5KVUS:
                case E513_10KVUS:
                    tvar1 = RLY_RD29;
                    tvar2 = RLY_RD25;
                    break;
            }
            *srgdata.relay |= tvar1;
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_WriteRelays(srgdata.address,*srgdata.relay);
            TPU_delay(75);
            *srgdata.relay |= tvar2;
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_WriteRelays(srgdata.address,*srgdata.relay);
            if ( tvar2 & RLY_RD25 )
                TPU_write(5,1);
            if ( tvar2 & RLY_RD24 )
                TPU_write(3,1);
            /* Delay for certain waveforms so that surge occurs during peak
               detection time window (peak detectors look for peak for a 20 ms
               period then shut off).  The 0.1 kV/us waveform is different than
               the other waveforms because its coil is powered by a 100V supply.
               The other waveforms are powered by 24V, resulting in a longer coil
               activation time. */
            switch ( srgdata.waveform )
            {
                case E513_P5KVUS:
                case E513_1KVUS:
                case E513_5KVUS:
                case E513_10KVUS:
                    TPU_delay(30);
                    break;
                case E513_P1KVUS:
                default:
                    break;
            }
            /* TPU_delay(30); ***Removed 12 Aug 97 FSDiC */
            break;
        case E502A_BOX:
        case E502B_BOX:
        case E502C_BOX:
            if ( (cpldata.outaddr != GHOST_BOX) && (dataCoupler) )
            {
                *srgdata.relay |= (RLY_RD26 | RLY_RD27);
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                TPU_delay(50);                      /* Relay settling...    */
                TPU_write(5,1);                     /* Turn on  K5!!!       */
                /* TPU_delay(10); ***Removed 12 Aug 97 FSDiC */
                break;
            }
            /* If going through the front panel then just FALL through */
        case E502_BOX:
        case E502H_BOX:
            TPU_write(3,1);                           /* Turn ON  K6/K7       */
            TPU_delay(50);                            /* Relay settling...    */
            TPU_write(5,1);                           /* Turn on  K5!!!       */
            /* TPU_delay(20); ***Removed 12 Aug 97 FSDiC */
            break;
        case E502K_BOX:
            if ( srgdata.waveform == E502K_1250US )
            {
                *srgdata.relay |= RLY_RD26;            /* Turn ON  K7          */
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
                ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            }
            else
            {
                *srgdata.relay |= RLY_RD22;            /* Turn ON  K4          */
                ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
            }
            TPU_write(3,1);                           /* Turn ON  K6          */
            TPU_delay(50);                            /* Relay settling...    */
            if ( srgdata.waveform == E502K_1250US )
            {
                *srgdata.relay |= RLY_RD21;            /* Turn ON  K3!!!       */
                ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
            }
            else
                TPU_write(5,1);                        /* Turn ON  K5!!!       */
            /* TPU_delay(20); ***Removed 12 Aug 97 FSDiC */
            break;
        case E514_BOX:
            switch ( srgdata.waveform )
            {
                case E514_015A:
                case E514_060A:
                    *srgdata.relay |= (RLY_RD28 | RLY_RD22);
                    ECAT_WriteRelays(srgdata.address,*srgdata.relay);
                    TPU_delay(200);
                    TPU_write(3,1);                     /* K6 ON (RD24)         */
                    break;
                case E514_100A:
                case E514_250A:
                    *srgdata.relay |= (RLY_RD28 | RLY_RD21);
                    ECAT_WriteRelays(srgdata.address,*srgdata.relay);
                    TPU_delay(200);
                    TPU_write(5,1);                     /* K5 ON (RD25)         */
                    break;
            }
            break;
        case E509_BOX:
        case E509A_BOX:
            *srgdata.relay |= RLY_RD26;               /* Turn ON  K6/K7       */
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(50);                            /* Relay settling...    */
            TPU_write(5,1);                           /* Turn on  K5!!!       */
            /* TPU_delay(20); ***Removed 12 Aug 97 FSDiC */
            break;
        case E518_BOX:
            *srgdata.relay |= RLY_RD26;               /* Turn ON  K6/K7       */
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(50);                            /* Relay settling...    */
            TPU_write(5,1);                           /* Turn on  K5!!!       */
            /* TPU_delay(20); ***Removed 12 Aug 97 FSDiC */
            break;
        case E515_BOX:
            if ( srgdata.voltage < 0 )
                *srgdata.relay |= RLY_RD27;   /* Turn ON  K4 */
            else
                *srgdata.relay |= RLY_RD26;   /* Turn ON  K3 */
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
            TPU_delay(200);
            TPU_write(5,1);                  /* Turn ON  K5!!! (RD25)      */
            /* Delay so that surge occurs during the peak detection time window
               (peak detectors look for peak for a 20 ms period then shut off). */
            TPU_delay(25);
            break;
        default:
            return(ERR_SRGNET);
    }

    /*
     * Surge cleanup happens here. First a delay is performed to wait for
     * the surge relay (K5) to close (this may change later). Then the
     * backplane and interrupts that were shutdown are re-enabled for peak
     * measurements.
     * Both K5 and K6 sync pulses are used for synchronizing on the
     * surge depending on the network.  The TIO channels used for each
     * are as follows:
     *       K5 Sync -> TIO_2 (TIOLCL_1)
     *       K6 Sync -> TIO_4 (TIOLCL_2)
     */
    DUART_enable();
    INT_enable();

    SRG_peakwindow = 1;
    SRG_peaktrigger= 1;
    vpeakpos_sumxy = 0;
    vpeakneg_sumxy = 0;
    ipeakpos_sumxy = 0;
    ipeakneg_sumxy = 0;
    xpeakxxx_sumxi = 0;
    xpeakxxx_sumxx = 0;
    vpeakpos_sumyi = 0;
    vpeakneg_sumyi = 0;
    ipeakpos_sumyi = 0;
    ipeakneg_sumyi = 0;
    peakdata.vpeakpos = 0x8000;
    peakdata.ipeakpos = 0x8000;
    peakdata.vpeakneg = 0x7FFF;
    peakdata.ipeakneg = 0x7FFF;
    vpeaktmp = 0;
    ipeaktmp = 0;
    surge_start = FALSE;

    /* Detect the start of the surge using the peak detectors. */
    TPU_delay_int(63000,SRG_PeakTrigger);           /* 30 ms measure window */

    /* Start peak detector circuit (open Peak Cap dump relays) for all modules
        using electromechanical relays (peak detector circuit was restarted prior
        to surge for modules with solid state relays).  Note that opening the
        relays and delaying 10 ms at this point means that peak detection does not
        begin until 10 ms after the start of the 30 ms detection window.  This
        should not be a problem, since the surge seems to occur at about 20 to
        25 ms after the start of the 30 ms window.  FD 29Dec95 05Mar96 */
    switch ( modptr->modinfo.id )
    {
        /* All modules with solid state relays go here */
        case E522S_BOX:
        case E521S_BOX:
            /* Do nothing - peak detectors are already on. */
            break;

            /* All modules with electromechanical relays default to here */
        default:
            PEAK_RELAYS = 0x7A00;
            TPU_delay(10); /* Allow the peak relays time to activate. */
            break;
    }

    while ( SRG_peaktrigger && !surge_start )
    {
        /* Check for voltage OR current surge on positive peak detectors. */
        if ( !SRG_ReadPeakDetector( POSITIVE ) )
            return(ERR_SRGREAD);

        if ( SRG_DetectSurgeStart( vpeakpos_ambient, ipeakpos_ambient ) )
        {
            surge_start = TRUE;
            continue;
        }

        /* Check for voltage OR current surge on negative peak detectors. */
        if ( !SRG_ReadPeakDetector( NEGATIVE ) )
            return(ERR_SRGREAD);

        if ( SRG_DetectSurgeStart( vpeakneg_ambient, ipeakneg_ambient ) )
        {
            surge_start = TRUE;
            continue;
        }
    }

    /* Reset flag if surge start was not detected above in the time frame
        allowed.  This flag is used denote on the LCD screen that the surge
        peak data MAY not be valid. */
    surge_detected = ( (surge_start == TRUE) ? TRUE : FALSE );

    /* Record surge peak data. */
    TPU_delay_int(30000,SRG_PeakWindow);
    peakcnt = -1;
    while ( SRG_peakwindow && ( peakcnt++ < 30 ) )
    {
        /* Read positive voltage and current peaks from peak detectors. */
        if ( !SRG_ReadPeakDetector( POSITIVE ) )
            return(ERR_SRGREAD);

        vpeakpos[peakcnt] = vpeaktmp;
        ipeakpos[peakcnt] = ipeaktmp;

        /* Read negative voltage and current peaks from peak detectors. */
        if ( !SRG_ReadPeakDetector( NEGATIVE ) )
            return(ERR_SRGREAD);

        vpeakneg[peakcnt] = vpeaktmp;
        ipeakneg[peakcnt] = ipeaktmp;
    }

    /* DUMP the Peak Caps.  The peak relay DEFAULT value was changed to
        0x6200 from 0x0200, in order to remove the 50 Ohm terminations from
        the circuit.  Previously, they were used to prevent excessive
        readings on the VI monitor outputs upon changing measurement
        modes.  This is now handled in the measurement mode change functions
        themselves.  FD 12Sep95 */
    PEAK_RELAYS = 0x6200;

/*
 * Other cleanup work is performed at this time in case the SURGE noise
 * caused any problems with the system.  Also, all relays that may be ON
 * at this time are shut off!
 */

    TPU_delay(200);                                 // Slight delay
    KEY_reset();                                    // Reset keyboard
    TPU_write(5,0);                                 // Turn OFF K5
    TPU_write(3,0);                                 // Turn OFF K6

    *srgdata.relay &= ~(RLY_RD18);                  // Turn OFF K8 (Dump)
    switch ( modptr->modinfo.id )
    {
        case E502K_BOX:
            *srgdata.relay &= ~(RLY_RD21);          // Turn OFF K3 (Trigger)
            break;
        case E505_BOX:
            *srgdata.relay &= ~(RLY_RD31);          // Turn OFF EXTRA K8
            break;
        case E507_BOX:
            /* Shut off RLY_RD31 K7 LV Fire relay (higher levels use RLY_RD25/"K5"),
               and also the polarity (default output connect) relays. */
            *srgdata.relay &= ~(RLY_RD31 | RLY_RD26 | RLY_RD27);
            break;
        case E522S_BOX:
        case E521S_BOX:
            *srgdata.relay &= ~(RLY_RD19 | RLY_RD20 | RLY_RD24 | RLY_RD27);
            E52x_HVProgram(srgdata.address,0,0);
            break;
        default:
            break;
    }
    ECAT_WriteRelay1(srgdata.address,(*srgdata.relay >> 16));
    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
    ECAT_TimerSequencer(srgdata.address,(*srgdata.relay >> 24));
    TPU_delay(200);                                 /* Slight delay         */

    for ( tvar = 0; tvar < 16; tvar++ )                /* Reset TIO channels   */
        TIO_Refresh(tvar);

/*
 * The peaks are returned at this time if they need to be else
 * all ZEROs are returned.  Peaks are also translated at this time
 * Modification on 01/08/95 by James Murphy (JMM)
 *    Support was added for the new DATA couplers which have a
 *    different scale factor than standard couplers
 */
    if ( peakdata.peakaddr != GHOST_BOX )
    {
        tvar1 = tvar2 = 2047;               /* Default V/I Scale Factors     */
        if ( moddata[(char)(peakdata.peakaddr)].modinfo.typePrimary & TYPE_CPLGEN )
        {
            switch ( moddata[(char)(peakdata.peakaddr)].modinfo.id )
            {
                case E522C_BOX:
                case E521C_BOX:
                    tvar1 = tvar2 = 1023;               /* V/I Scale Factors    */
                    break;
                case E571_BOX:
                    tvar2 = 20470;                      /* Current Scale Factor */
                    break;
                default:
                    break;
            }
        }
        else
        {
            switch ( moddata[(char)(peakdata.peakaddr)].modinfo.id )
            {
                case E513_BOX:
                case E514_BOX:
                case E508_BOX:
                case E509_BOX:
                case E518_BOX:
                case E509A_BOX:
                    tvar2 = 20470;                      /* Current Scale Factor */
                    break;
                case E506_4W_BOX:
                    tvar2 = 16376;                      /* Current Scale Factor */
                    break;
                case E505A_BOX:
                case E505B_BOX:
                    tvar2 = 8188;                       /* Current Scale Factor */
                default:
                    break;
            }
        }
/*printf("\n");*/
        for ( tvar = 0; tvar < peakcnt; tvar++ )
        {
            peakdata.vpeakpos = 10000 * vpeakpos[tvar] / tvar1;
            peakdata.vpeakneg = 10000 * vpeakneg[tvar] / tvar1;
            peakdata.ipeakpos =  5000 * ipeakpos[tvar] / tvar2;
            peakdata.ipeakneg =  5000 * ipeakneg[tvar] / tvar2;
            vpeakpos_sumyi += (int)peakdata.vpeakpos;
            vpeakneg_sumyi += (int)peakdata.vpeakneg;
            ipeakpos_sumyi += (int)peakdata.ipeakpos;
            ipeakneg_sumyi += (int)peakdata.ipeakneg;
            xpeakxxx_sumxi += (tvar+1);
            xpeakxxx_sumxx += ((tvar+1) * (tvar+1));
            vpeakpos_sumxy += ((int)peakdata.vpeakpos * (tvar+1));
            vpeakneg_sumxy += ((int)peakdata.vpeakneg * (tvar+1));
            ipeakpos_sumxy += ((int)peakdata.ipeakpos * (tvar+1));
            ipeakneg_sumxy += ((int)peakdata.ipeakneg * (tvar+1));
/*printf("cnt=%2d, ADC_V+ = %4d, ADC_V- = %4d\n",tvar,peakdata.vpeakpos,peakdata.vpeakneg);*/
        }
        b1 = ((peakcnt * vpeakpos_sumxy) - (xpeakxxx_sumxi * vpeakpos_sumyi)) /
             ((peakcnt * xpeakxxx_sumxx) - (xpeakxxx_sumxi * xpeakxxx_sumxi));
        b0 = (vpeakpos_sumyi / peakcnt) - (b1 * (xpeakxxx_sumxi / peakcnt));
        peakdata.vpeakpos = b0 + b1;
        b1 = ((peakcnt * vpeakneg_sumxy) - (xpeakxxx_sumxi * vpeakneg_sumyi)) /
             ((peakcnt * xpeakxxx_sumxx) - (xpeakxxx_sumxi * xpeakxxx_sumxi));
        b0 = (vpeakneg_sumyi / peakcnt) - (b1 * (xpeakxxx_sumxi / peakcnt));
        peakdata.vpeakneg = b0 + b1;
        b1 = ((peakcnt * ipeakpos_sumxy) - (xpeakxxx_sumxi * ipeakpos_sumyi)) /
             ((peakcnt * xpeakxxx_sumxx) - (xpeakxxx_sumxi * xpeakxxx_sumxi));
        b0 = (ipeakpos_sumyi / peakcnt) - (b1 * (xpeakxxx_sumxi / peakcnt));
        peakdata.ipeakpos = b0 + b1;
        b1 = ((peakcnt * ipeakneg_sumxy) - (xpeakxxx_sumxi * ipeakneg_sumyi)) /
             ((peakcnt * xpeakxxx_sumxx) - (xpeakxxx_sumxi * xpeakxxx_sumxi));
        b0 = (ipeakneg_sumyi / peakcnt) - (b1 * (xpeakxxx_sumxi / peakcnt));
        peakdata.ipeakneg = b0 + b1;
        fprintf(destination,"0 %+6d %+6d %+6d %+6d",peakdata.vpeakpos,peakdata.vpeakneg,
                peakdata.ipeakpos,peakdata.ipeakneg);
    }
    else
        if ( display.current != DISPLAY_SRG )
        fputs("0 0 0 0 0",destination);
    return(0);
}

char  SRG_SurgeKill(void)
{
	int   ox,oy;
    uint reserve_mask = 0x00;

	DUART_enable();                                 /* ENABLE backplane     */
	INT_enable();                                   /* ENABLE interrupts    */
	SRG_HVKill();

	if(chgdata.checker)
		TPU_kill(chgdata.checker);                   /* Turn OFF HV charger  */
	if(chgdata.monitor)
		TPU_kill(chgdata.monitor);                   /* Turn OFF HV watchdog */
	chgdata.checker    = 0;
	chgdata.monitor    = 0;

	TPU_write(5,0);                                 /* Turn OFF K5          */
	TPU_write(3,0);                                 /* Turn OFF K6          */

	/* Clear ALL the coupler relays to avoid 24V drains in certain modules */
	SRG_ClearCouplerHardware();

   /* If this is an E501B, clear the undershoot clamp relays. */
   if ( ( moddata[(char)(srgdata.address)].modinfo.id ) == E501B_BOX )
   {
      *srgdata.relay &= ~(RLY_RD27 | RLY_RD28); /* Turn OFF both clamp relays*/
   	ECAT_WriteRelays(srgdata.address,*srgdata.relay);
   }

	if(((moddata[(char)(srgdata.address)].modinfo.id) == E522S_BOX) ||
		((moddata[(char)(srgdata.address)].modinfo.id) == E521S_BOX))
		reserve_mask |= (LED_ACTIVE | INST_VMASK | INST_IMASK | RLY_RD26 | RLY_RD30);
	else
		reserve_mask |= (LED_ACTIVE | INST_VMASK | INST_IMASK);
    if ( moddata[srgdata.address].modinfo.id == E505B_BOX)
    {
        reserve_mask |= RLY_RD23;  // don't reset E505B module LED steering relay
        if ( srgdata.waveform == E505B_2500V )
        {
            // if E505B CDN waveform & EUT power is on, retain LED_VOLTAGE state (E505B is combined surge/CDN module)
            cpldata.eutswitch = !(read_pirq() & 0x20);
            if ( cpldata.eutrequest && cpldata.eutswitch )
                reserve_mask |= LED_VOLTAGE;
        }
    }

    *srgdata.relay &= reserve_mask;

	/* Shut off ALL SURGE relays except INST(RD00-RD15) and LED_ACTIVE (and RD23 for E505B only) */
	if((srgdata.address < MAX_MODULES) && (moddata[(char)(srgdata.address)].modinfo.typePrimary & TYPE_SRG))
		ECAT_WriteRelays(srgdata.address,*srgdata.relay);

	/* Clean up E508-12P if exists */
	if(cpldata.E50812Paddr < MAX_MODULES)
	{
		moddata[(char)(cpldata.E50812Paddr)].relay &= LED_ACTIVE;
		ECAT_WriteRelays(cpldata.E50812Paddr,moddata[(char)(cpldata.E50812Paddr)].relay);
	}

	return(0);
}

void  SRG_HVKill(void)
{
	ECAT_HVProgram(0,0);
	if(ehvdata.ehvflag)
		EHV_HVProgram(0,0);
	if ( ( (moddata[(char)(srgdata.address)].modinfo.id ) == E522S_BOX )
      || ( (moddata[(char)(srgdata.address)].modinfo.id ) == E521S_BOX ) )
        E52x_HVProgram( srgdata.address, 0, 0 );
    if ( (moddata[(char)(srgdata.address)].modinfo.id ) == E507_BOX )
        E507_HVProgram( srgdata.address, 0, 0 );
}

/*
 * This will return the maximum USER voltage allowable for each
 * waveform for each coupler for each network
 * Parameters:
 *    netaddr  - SURGE network
 *    wave     - SURGE waveform
 */
int   SRG_MaximumVoltage(
uchar netaddr,
uchar wave)
{
	char  cplType;
	uchar netId;
	int   maxVolts;

	maxVolts = 0;
	if((netaddr >= MAX_MODULES) || (wave >= MAX_WAVEFORMS))
		return(maxVolts);

	cplType = 0;
	if(cpldata.outaddr != GHOST_BOX)
	{
		if((cplType = SRG_GetCouplerType(cpldata.outaddr)) < 0)
			return(maxVolts);
	}

	netId = moddata[(char)(netaddr)].modinfo.id;
	if((wave >= wavdata[(char)(netId)].numWaves) || (netId >= MAX_SRGBOXES))
		return(maxVolts);
	maxVolts = (int)wavdata[(char)(netId)].wavinfo[(char)(wave)].maxVoltage[cplType];
	return(maxVolts);
}

/*
 * The voltage to power supply  is calculated according to the
 * nominal factor & calibration data.  Any errors will simply return
 * a voltage translation of ZERO (0) for safety purposes
 *    netaddr       - SURGE network
 *    wave          - SURGE waveform
 *    raw_voltage   - voltage to translate
 *  
 * This function is called directly by modules (E507 at this time) whose 
 * very low program voltages require greater precision than the whole integer 
 * precision provided by SRG_TranslateVoltage().  However, for consistency 
 * and to eliminate duplication, that function calls this one to perform 
 * calibration adjustments. 
 *  
 * This function returns adjusted_voltage IN MILLIVOLTS! 
 */
int   SRG_TranslateVoltage_precise( uchar netaddr, uchar wave, int raw_voltage )
{ 
    int adjusted_voltage;
    int scaled_voltage;
    int voltage_scale;
	MODDATA  *modptr;
	uchar    netId;

	if((netaddr >= MAX_MODULES) || (wave >= MAX_WAVEFORMS))
		return(0);

	modptr = &moddata[(char)(netaddr)];
	netId  = modptr->modinfo.id;
	if((wave >= wavdata[(char)(netId)].numWaves) || (netId >= MAX_SRGBOXES))
		return(0);

    /* Scale raw_voltage as required to increase accuracy of calculations.
        If raw_voltage < 10 V, multiply by 100.
        If 10 V <= raw_voltage < 100 V, multiply by 10.
        Otherwise (raw_voltage >= 100 V) don't scale. */
    if ( abs(raw_voltage) < 10 )
        voltage_scale = 100;    // 100:1
    else if ( abs(raw_voltage) < 100 )
        voltage_scale = 10;     // 10:1
    else
        voltage_scale = 1;      // 1:1 (no scaling)

    scaled_voltage = raw_voltage * voltage_scale;
/*
 * The voltage needs to be translated to the actual desired voltage
 * which is module dependent.  This is what the voltage factor is
 * used for.  Most modules will have a scale factor of 1, however,
 * a few modules use this translation. Note that the translation is
 * done with factors in the thousands allowing a complete integer
 * calculation to avoid floating point roundoff errors
 */
	adjusted_voltage = scaled_voltage * (int)(wavdata[(char)(netId)].wavinfo[(char)(wave)].voltFactor);
	adjusted_voltage /= 1000;

/*
 * This step will take the translated voltage and translate it using the
 * CALIBRATION data.  Once again this is wholly an integer calculation 
 * (exception:  the E503A special adjustment is floating point).
 * Note that all calibration factors are based off of a value of 6000
 * volts regardless of the actual module's maximum voltage. There is NO
 * translation done if in calibration mode
 */
	if(!calibration)
	{
        adjusted_voltage = CAL_GetCalibratedVoltage( adjusted_voltage, raw_voltage, voltage_scale,
                                                     cpldata.cplhigh,cpldata.cpllow,
                                                     &(modptr->modinfo.calinfo),0x00, wave,netId,
                                                     SRG_GetCouplerType(cpldata.outaddr) );
	}

	return adjusted_voltage * ( 1000 / voltage_scale );  // return adjusted_voltage in millivolts
}

/*
 * The voltage to power supply  is calculated according to the
 * nominal factor & calibration data.  Any errors will simply return
 * a voltage translation of ZERO (0) for safety purposes
 *    netaddr  - SURGE network
 *    wave     - SURGE waveform
 *    voltage  - voltage to translate
 */
int   SRG_TranslateVoltage( uchar netaddr, uchar wave, int raw_voltage )
{ 
    // raw_voltage input is volts, adjusted_voltage output is millivolts
    int adjusted_voltage = SRG_TranslateVoltage_precise( netaddr, wave, raw_voltage );

    // convert to volts before returning
	return adjusted_voltage / 1000;
}

/*
 * The HVPS is set in this routine for surge functions. The voltage
 * will be translated via supply factor, calibration data, and EHV
 * factor. The DAC will NOT be programmed in case of errors and an
 * error code will be returned.
 * Parameters:
 *    netaddr  - SURGE network
 *    wave     - SURGE waveform
 *    voltage  - Desired voltage
 *
 * Returns:
 * Returns:  0 - Success
 *          -1 - Illegal bay choice
 *          -2 - Interface board does not exist
 *          -3 - OVERFLOW
 */
char  SRG_HVProgram(
uchar netaddr,
uchar wave,
int      voltage,
uchar enableFlag)
{
	MODDATA  *modptr;
	char     errval;

	if((netaddr >= MAX_MODULES) || (wave >= MAX_WAVEFORMS))
		return(-1);

	errval = 0;
	modptr = &moddata[(char)(netaddr)];
	if(!(modptr->modinfo.typePrimary & TYPE_SRG) || (wave >= wavdata[(char)(modptr->modinfo.id)].numWaves))
		return(-2);

	if((abs(voltage)) > (SRG_MaximumVoltage(srgdata.address,srgdata.waveform)) && (!calibration))
		voltage = 0;

	if ( (modptr->modinfo.id == E522S_BOX) || (modptr->modinfo.id == E521S_BOX) )
	{
        voltage = SRG_TranslateVoltage(netaddr,wave,voltage);
		E52x_HVProgram( netaddr,voltage,enableFlag );
	}
    else if (modptr->modinfo.id == E507_BOX)
    {
        // SRG_TranslateVoltage_precise returns millivolts, E507_HVProgram takes millivolts for input
        voltage = SRG_TranslateVoltage_precise(netaddr,wave,voltage);
        E507_HVProgram( netaddr,voltage,enableFlag );
    }
    else 
	{
        voltage = SRG_TranslateVoltage(netaddr,wave,voltage);
		errval = ECAT_HVProgram( voltage,enableFlag );
		if ( ( ehvdata.ehvflag ) && ( modptr->modinfo.options & OPT_EHVCAPABLE ) )
			EHV_HVProgram( voltage,enableFlag );
	}
	return(errval);
}

uchar SRG_MinimumDelay(
uchar netaddr,
uchar wave)
{
	ushort   minDelay;
	short    maxVolts;
	char     cplType;
	uchar    netId;
	uchar    ehvId;
	MODDATA  *modptr;

	minDelay = MIN_SRGDELAY;

	if((netaddr >= MAX_MODULES) || (wave >= MAX_WAVEFORMS))
		return(minDelay);

	modptr = &moddata[(char)(netaddr)];
	netId  = modptr->modinfo.id;

	if(wave >= wavdata[(char)(netId)].numWaves)
		return(minDelay);

	if(cpldata.outaddr != GHOST_BOX)
	{
		if((cplType = SRG_GetCouplerType(cpldata.outaddr)) < 0)
			return(MIN_SRGDELAY);
	}
	else
		cplType = 0;

	minDelay = wavdata[(char)(netId)].wavinfo[(char)(wave)].minDelay[cplType];
	maxVolts = wavdata[(char)(netId)].wavinfo[(char)(wave)].maxVoltage[cplType];
	maxVolts = SRG_TranslateVoltage(netaddr,wave,(int)maxVolts);

	if((ehvdata.ehvflag) && (ehvdata.network < MAX_MODULES) && (modptr->modinfo.options & OPT_EHVCAPABLE))
	{
		ehvId = moddata[(char)(ehvdata.network)].modinfo.id;
		if ( maxVolts <= ehvinfo[(char)(ehvId)].maxVoltage )
		{
			minDelay = ((minDelay - 2 ) / ehvinfo[(char)(ehvId)].factor) + 2;
			if(minDelay < 3)
				minDelay = 3;
		}
	}

    if ( ( netId == E510A_BOX )  && ( ( abs( srgdata.voltage ) ) <= 6000 ) )
        minDelay = 10;

	if ( ( ( netId == E521S_BOX ) || ( netId == E522S_BOX ) ) && ( ( abs( srgdata.voltage ) ) <= 10000 ) )
		minDelay = 30;

	return(minDelay);
}

/*
 * 0  = LINE Coupler (Standard)
 * 1  = HV Coupler   (E52xC)
 * 2  = DATA Coupler (E571)
 * -1 = NOT a Coupler
 */
char  SRG_GetCouplerType(
uchar bayaddr)
{
	char  cplType;

	if(bayaddr >= MAX_MODULES)
		return(-1);

	if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLGEN))
		return(-1);
	if(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLLINE)
		cplType = CPL_TYPELINE;
	if(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLHV)
		cplType = CPL_TYPEHV;
	if(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLDATA)
		cplType = CPL_TYPEDATA;
	return(cplType);
}

/*
 * 1 = OKAY
 * 0 = BAD Output Selection
 */
uchar SRG_OutputValid(
uchar cpladdr,
uchar netaddr,
uchar wave)
{
	char     cplType;
	uchar    surge_id;

	if(netaddr >= MAX_MODULES)
		return 0;

	surge_id = moddata[(char)(netaddr)].modinfo.id;
	if(wave >= wavdata[(char)(surge_id)].numWaves)
		return 0;
	if((surge_id == E508_BOX) && (cpldata.E50812Paddr >= MAX_MODULES) && (wave == E508_25A))
		return 0;

	if(cpladdr == GHOST_BOX)
	{
		if(!(wavdata[(char)(surge_id)].wavinfo[(char)(wave)].frontPanelFlag))
			return 0;
	}
	else
	{
		if((cplType = SRG_GetCouplerType(cpladdr)) < 0)
			return 0;
		if(!(wavdata[(char)(surge_id)].wavinfo[(char)(wave)].cplFlag[cplType]))
			return 0;

        // E505B output must be E505B, and non-E505B_BOX output cannot be E505B
        if ( surge_id == E505B_BOX )
        {
            if ( cpladdr != netaddr )
                return 0;
        }
        else  // surge network is NOT E505B_BOX
        {
            if( moddata[(char)(cpladdr)].modinfo.id == E505B_BOX )
                return 0;
        }
	}
	return 1;
}

/* 
 * Determine validity of EUT (AC line) source module 
 *  
 * 1 = OKAY
 * 0 = BAD Output Selection
 */
uchar SRG_SourceValid( uchar source_address, uchar surge_address )
{
	if ( ( surge_address >= MAX_MODULES ) || ( source_address >= MAX_MODULES ) )
		return 0;

    // E505B source must be E505B, and non-E505B_BOX source cannot be E505B
    if ( moddata[(char)(surge_address)].modinfo.id == E505B_BOX )
    {
        if ( source_address != surge_address )
            return 0;
    }
    else  // surge network is NOT E505B_BOX
    {
        if( moddata[(char)(source_address)].modinfo.id == E505B_BOX )
            return 0;
    }

	return 1;
}
/******* << END KRNL_SRG.C >> *******/
