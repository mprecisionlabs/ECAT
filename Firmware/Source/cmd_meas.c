#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "tiocntl.h"

extern volatile ushort	PEAK_RELAYS;
extern FILE		*destination;
extern MODDATA	moddata[];
extern SRGDATA	srgdata;
extern CPLDATA  cpldata;
extern PKDATA	peakdata;

/* JMM - 05/30/95 - Set MEASURE relays here */
int	MEAS_BAY_pgm(P_PARAMT *params)
{
	uchar		bayaddr;
	ushort	commonrelay;
	P_PARAMT	tparam;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(!moddata[(char)(bayaddr)].modinfo.viMonitors)
		return(BAD_VALUE);

	/* DISABLE all measurement channels				*/
	/* Set up DEFAULTS and relays at this time	*/
	/* VMON = A/B	IMON = (1)							*/

	if(peakdata.peakaddr != bayaddr)
	{
		peakdata.peakaddr	= bayaddr;
		peakdata.vmonhi	= PEAK_VMONA;
		peakdata.vmonlo	= PEAK_VMONB;
		peakdata.imon		= PEAK_IMON1;
		PEAK_MeasureDisable();
	}
	tparam.idata = (((peakdata.vmonhi) << 4) & 0xf0) + ((peakdata.vmonlo) & 0x0f);
	MEAS_VCH_pgm(&tparam);
	tparam.idata = peakdata.imon;
	MEAS_ICH_pgm(&tparam);
	return(0);
}

int	MEAS_BAY_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",peakdata.peakaddr);
	return(0);
}

int	MEAS_ICH_pgm(P_PARAMT *params)
{
	uchar	ich;
	uchar	vchs;
	uint	tmprelay;
	int	returnval = 0;

	ich = (uchar)EXTRACT_INT(0);
	if((ich < 1) || (ich > 5) || (peakdata.peakaddr == GHOST_BOX))
		returnval = BAD_VALUE;

	if ( returnval == 0 )
	{
		/* Disable peak relays: peak caps disabled, 50 Ohm atten. in circuit. */
		PEAK_RELAYS = 0x0200;
		TPU_delay(10);	/* Relay settling time. */

		tmprelay = 0x00000000;
		moddata[(char)(peakdata.peakaddr)].relay &= ~(INST_IMASK);
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
		TPU_delay(10);

		if((moddata[(char)(peakdata.peakaddr)].modinfo.options & OPT_5CHINST) && (!peakdata.cheat))
		{
			switch(ich)
			{
				case PEAK_IMON1:
					tmprelay |= INST5_IMON1;
					break;
				case PEAK_IMON2:
					tmprelay |= INST5_IMON2;
					break;
				case PEAK_IMON3:
					tmprelay |= INST5_IMON3;
					break;
				case PEAK_IMON4:
					tmprelay |= INST5_IMON4;
					break;
				case PEAK_IMON5:
					tmprelay |= INST5_IMON5;
					break;
				default:
					returnval = BAD_VALUE;
			}
		}
		else				/* 3 channel board */
		{
			switch(ich)
			{
				case PEAK_IMON1:
                    /* Special case for E505B with output at internal CDN (2.5 kV waveform only):
                            connect IMON to channel 2 instead of channel 1 */
                    if ( ( moddata[(char)(peakdata.peakaddr)].modinfo.id == E505B_BOX )
                      && ( moddata[(char)(srgdata.address)].modinfo.id == E505B_BOX )
                      && ( cpldata.outaddr != GHOST_BOX ) )
                        tmprelay |= INST_IMON2;
                    else
                        tmprelay |= INST_IMON1;  // normal case
					break;
				case PEAK_IMON2:
					tmprelay |= INST_IMON2;
					break;
				case PEAK_IMON3:
					tmprelay |= INST_IMON3;
					break;
				case PEAK_IMON4:
					tmprelay |= INST_IMON4;
					break;
				case PEAK_IMON5:
					tmprelay |= INST_IMON5;
					break;
				default:
					returnval = BAD_VALUE;
			}
		}
	}

	if ( returnval == 0 )
	{
		peakdata.imon = ich;
		moddata[(char)(peakdata.peakaddr)].relay |= tmprelay;
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);

		/* Reenable peak relays: peak caps disabled, 50 Ohm out of circuit. */
		PEAK_RELAYS = 0x6200;
		TPU_delay(10);	/* Relay settling time. */
	}

	return( returnval );
}

int	MEAS_ICH_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",peakdata.imon);
	return(0);
}

int	MEAS_IPK_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d,%d",peakdata.ipeakpos,peakdata.ipeakneg);
	return(0);
}

/*
 *	Modification on 10/07/94 by James Murphy (JMM)
 *		This routine was altered so that the V_ENABLE relay would turn ON
 *		after the VMON channels have settled.  This should keep the
 *		instrumentation from showing a rail level signal (~14-15 volts)
 *
 *	Modification on 11/02/94 by James Murphy (JMM)
 *		This routine was altered to support the new 5-channel
 *		instrumentation board.  Since the 5-channel board is no longer
 *		a simple extension of a 3-channel board; special consideration
 *		had to be taken for setting the correct relay drivers.  Any unit
 *		with a 5-channel board must have a minimum of 4 voltage monitors
 *		set or else the board will be considered a 3-channel board
 *
 *	Modification on 12/13/94 by James Murphy (JMM)
 *		The serial EEPROM format has changed and a new option has been
 *		added for the 5 channel instrumentation board.  Therefore, a
 *		5-channel board can now use less than 4 voltage monitors.
 */
int	MEAS_VCH_pgm(P_PARAMT *params)
{
	uchar	vchhi;
	uchar	vchlo;
	uchar	vchs;
	uint	tmprelay;
	int	returnval = 0;

	vchhi = (uchar)((EXTRACT_INT(0) & 0xF0) >> 4);
	vchlo = (uchar) (EXTRACT_INT(0) & 0x0F);

	/* Channels cannot be the same */
	if((vchhi == vchlo) || (peakdata.peakaddr == GHOST_BOX))
	{
		returnval = BAD_VALUE;
	}
	else
	{
		if((vchhi < 1) || (vchlo < 1) || (vchhi > 5) || (vchlo > 5))
			returnval = BAD_VALUE;
	}

	if ( returnval == 0 )
	{
		/* Disable peak relays: peak caps disabled, 50 Ohm atten. in circuit. */
		PEAK_RELAYS = 0x0200;
		TPU_delay(10);	/* Relay settling time. */

		tmprelay = 0x00000000;
		moddata[(char)(peakdata.peakaddr)].relay &= ~(INST_VMASK);
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
		TPU_delay(10);

		if((moddata[(char)(peakdata.peakaddr)].modinfo.options & OPT_5CHINST) && (!peakdata.cheat))
		{
			switch(vchhi)
			{
				case PEAK_VMONA:
					tmprelay |= INST5_VMONHIA;
					break;
				case PEAK_VMONB:
					tmprelay |= INST5_VMONHIB;
					break;
				case PEAK_VMONC:
					tmprelay |= INST5_VMONHIC;
					break;
				case PEAK_VMOND:
					tmprelay |= INST5_VMONHID;
					break;
				case PEAK_VMONE:
					tmprelay |= INST5_VMONHIE;
					break;
				default:
					returnval = BAD_VALUE;
			}
			if ( returnval == 0 )
			{
				switch(vchlo)
				{
					case PEAK_VMONA:
						tmprelay |= INST5_VMONLOA;
						break;
					case PEAK_VMONB:
						tmprelay |= INST5_VMONLOB;
						break;
					case PEAK_VMONC:
						tmprelay |= INST5_VMONLOC;
						break;
					case PEAK_VMOND:
						tmprelay |= INST5_VMONLOD;
						break;
					case PEAK_VMONE:
						tmprelay |= INST5_VMONLOE;
						break;
					default:
						returnval = BAD_VALUE;
				}
			}
		}
		else				/* 3 channel board */
		{
			switch(vchhi)
			{
                case PEAK_VMONA:
                    /* Special case for E505B with output at internal CDN (2.5 kV waveform only):
                            connect VMON HI to channel C instead of channel A (LO is always B) */
                    if ( ( moddata[(char)(peakdata.peakaddr)].modinfo.id == E505B_BOX )
                      && ( moddata[(char)(srgdata.address)].modinfo.id == E505B_BOX )
                      && ( cpldata.outaddr != GHOST_BOX ) )
                        tmprelay |= INST_VMONHIC;
                    else
                        tmprelay |= INST_VMONHIA;  // normal case
					break;
				case PEAK_VMONB:
					tmprelay |= INST_VMONHIB;
					break;
				case PEAK_VMONC:
					tmprelay |= INST_VMONHIC;
					break;
				case PEAK_VMOND:
					tmprelay |= INST_VMONHID;
					break;
				case PEAK_VMONE:
					tmprelay |= INST_VMONHIE;
					break;
				default:
					returnval = BAD_VALUE;
			}
			if ( returnval == 0 )
			{
				switch(vchlo)
				{
					case PEAK_VMONA:
						tmprelay |= INST_VMONLOA;
						break;
					case PEAK_VMONB:
						tmprelay |= INST_VMONLOB;
						break;
					case PEAK_VMONC:
						tmprelay |= INST_VMONLOC;
						break;
					case PEAK_VMOND:
						tmprelay |= INST_VMONLOD;
						break;
					case PEAK_VMONE:
						tmprelay |= INST_VMONLOE;
						break;
					default:
						returnval = BAD_VALUE;
				}
			}
		}
	}

	if ( returnval == 0 )
	{
		peakdata.vmonhi = (uchar)vchhi;
		peakdata.vmonlo = (uchar)vchlo;
		moddata[(char)(peakdata.peakaddr)].relay |= tmprelay;
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);
		TPU_delay(10);
		moddata[(char)(peakdata.peakaddr)].relay |= INST_VENABLE;
		ECAT_WriteRelay0(peakdata.peakaddr,moddata[(char)(peakdata.peakaddr)].relay);

		/* Reenable peak relays: peak caps disabled, 50 Ohm out of circuit. */
		PEAK_RELAYS = 0x6200;
		TPU_delay(10);	/* Relay settling time. */
	}

	return( returnval );
}

int	MEAS_VCH_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d,%d",peakdata.vmonhi,peakdata.vmonlo);
	return(0);
}

int	MEAS_VPK_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d,%d",peakdata.vpeakpos,peakdata.vpeakneg);
	return(0);
}

/*[ÿ]
 *      Routine:	PEAK_MeasureDisable
 *       Module:	CMD_MEAS.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Shuts down ALL V/I monitor relays in ALL V/I capable
 *					modules.  This effectively shuts down the peak detector
 *					circuitry.
 * Error Checks:	None
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	None
 *      Globals:	[ ] moddata					- Holds ALL MODULE information
 *       Locals:	tvar							- Temporary bay address variable
 *      Returns:	0	- Success
 *    Revisions: 10/07/94 - James Murphy (JMM)
 *						Support was added to shut down the peak relays on
 *						the peak detector board since there will be NO
 *						inputs upon function termination.  This keeps any
 *						'floating' signals from reaching the VMON/IMON jacks
 *						on the front of the ECAT controller
 */
char	PEAK_MeasureDisable(void)
{
	char	tvar;

	for(tvar = 0; tvar < MAX_MODULES; tvar++)
		if(moddata[tvar].modinfo.viMonitors)
 		{
			moddata[tvar].relay &= ~(INST_VMASK | INST_IMASK);
			ECAT_WriteRelay0(tvar,moddata[tvar].relay);
		}
	PEAK_RELAYS = 0x0200;
	TPU_delay(10);	/* Relay settling time. */
	return(0);
}
/******* << END CMD_MEAS.C >> *******/
