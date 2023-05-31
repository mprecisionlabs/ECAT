#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "eft.h"
#include "tiocntl.h"
#include "data.h"

extern FILE		*destination;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern EFTDATA	eftdata;
extern uint		ghostrelay;
extern uchar		ecatmode;

EFTCHEAT	eftCheat;

char	EFT_ClearCouplerHardware(void)
{
	if(cpldata.outaddr > MAX_MODULES)
		return(-1);

	if(!(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLEFT))
		return(BAD_VALUE);

	if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_EFT)
	{
		if(moddata[(char)(cpldata.outaddr)].modinfo.id >= E422_BOX)
		{
			*cpldata.expansion &= ~(CPL_422MASK);
			ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
		}
		else
		{
			*cpldata.outrelay &= ~(CPL_412MASK);
			ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
		}
	}
	else
	{
		/* Standard AC Couplers (i.e. E551, E4554) */
		*cpldata.outrelay &= ~(CPL_CPLMASK | CPL_MODEMASK);
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_BAY_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT SOURCE box; Resets ALL the EFT modules
 *					in the system; Sets the ACTIVE LED in the appropriate
 *					module; and maps the TIO channels in said module. Note
 *					that the WHOLE <eftdata> structure is RESET via the call
 *					to EFT_ResetDisable()
 * Error Checks:	Parameter must be a valid BAY address and must indicate
 *					the address of an EFT module
 *					must be a valid BAY address
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata					- EFT data structure (ALL)
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	bayaddr						- Temporary bay address variable
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_BAY_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	if ( eftdata.eftflag )
	{
		bayaddr = (uchar)EXTRACT_INT(0);
		if(bayaddr >= MAX_MODULES)
			return(BAD_VALUE);

		if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_EFT))
			return(BAD_VALUE);

		/* This is a problem -- HOW to FIX? */
		EFT_ResetModules(0);

		eftdata.eftaddr = bayaddr;
		eftdata.relay   = &moddata[(char)(eftdata.eftaddr)].relay;
		*eftdata.relay |= LED_ACTIVE;
		ECAT_WriteRelays(eftdata.eftaddr,*eftdata.relay);

		TIO_EFTDisable();
		TIO_TOMap(eftdata.eftaddr,TO0_TOLCL0);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_BAY_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT SOURCE to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.eftaddr		- Holds the EFT source
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_BAY_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.eftaddr);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_CPL_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current COUPLING mode for EFT modules.  ALL lines
 *					can be coupled simultaneously.  The relay data is then
 *					set and stored for the couplers.  If the coupler is also
 *					an EFT module (E412) then it is handled differently
 * Error Checks:	The parameter is automatically adjusted so as to avoid
 *					any range errors
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] cpldata.cplhigh		- Holds the COUPLING information
 *					[X] cpldata.outrelay		- Holds the COUPLING RELAY information
 *					[ ] cpldata.outaddr		- Holds the OUTPUT module address
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	cplbits						- Temporary coupling variable
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_CPL_pgm(P_PARAMT *params)
{
	uchar	cplbits;

 	/* Determine the number of lines being coupled
		(zero implies no coupling; E422 behavior not yet determined) */
	cpldata.multihigh = 0;  

	if(eftdata.eftflag)
	{
		cplbits = EXTRACT_INT(0) & 0x1F;

		cpldata.cplhigh = (uchar)cplbits;
		if(cpldata.outaddr != GHOST_BOX)
		{
			if(moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_EFT)
			{
				if(moddata[(char)(cpldata.srcaddr)].modinfo.id >= E422_BOX)
				{
					*cpldata.expansion &= ~(CPL_422MASK);
					if(cplbits & CPL_L1BIT)
						*cpldata.expansion |= CPL_422L1;
					if(cplbits & CPL_L2BIT)
						*cpldata.expansion |= CPL_422L2;
					if(cplbits & CPL_L3BIT)
						*cpldata.expansion |= CPL_422L3;
					if(cplbits & CPL_NUBIT)
						*cpldata.expansion |= CPL_422NU;
					if(cplbits & CPL_PEBIT)
						*cpldata.expansion |= CPL_422PE;
					ECAT_WriteEXPRelay0(cpldata.outaddr,*cpldata.expansion);
				}
				else
				{
					*cpldata.outrelay &= ~(CPL_412MASK);
					if(cplbits & CPL_L1BIT)
					{	
						*cpldata.outrelay |= CPL_412L1;
						cpldata.multihigh++;
					}
					if(cplbits & CPL_L2BIT)
					{	
						*cpldata.outrelay |= CPL_412L2;
						cpldata.multihigh++;
					}
					if(cplbits & CPL_PEBIT)
					{
						*cpldata.outrelay |= CPL_412PE;
						cpldata.multihigh++;
					}
					ECAT_WriteRelay0(cpldata.outaddr,*cpldata.outrelay);
				}
			}
			else
			{
				*cpldata.outrelay &= ~(CPL_CPLMASK | CPL_MODEMASK);
				if(cplbits & CPL_L1BIT)
				{	
					*cpldata.outrelay |= CPL_EFTL1;
					cpldata.multihigh++;
				}
				if(cplbits & CPL_L2BIT)
				{
					*cpldata.outrelay |= CPL_EFTL2;
					cpldata.multihigh++;
				}
				if(cplbits & CPL_L3BIT)
				{
					*cpldata.outrelay |= CPL_EFTL3;
					cpldata.multihigh++;
				}
				if(cplbits & CPL_NUBIT)
				{
					*cpldata.outrelay |= CPL_EFTNU;
					cpldata.multihigh++;
				}
				if(cplbits & CPL_PEBIT)
				{
					*cpldata.outrelay |= CPL_EFTPE;
					cpldata.multihigh++;
				}
				ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
			}
		}
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_CPL_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT COUPLING to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] cpldata.cplhigh		- Holds the EFT COUPLING mode
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_CPL_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.cplhigh);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_DUR_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT DURATION parameter
 * Error Checks:	The parameter must be between EFT_MINDUR and EFT_MAXDUR.
 *					These limits apply to ALL EFT modules
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.duration		- Holds the EFT DURATION parameter
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_DUR_pgm(P_PARAMT *params)
{
	int maxdur;

	if ( eftdata.eftflag )
	{
		if(moddata[eftdata.eftaddr].modinfo.id == E421P_BOX)	/* JLR */
			maxdur = E421P_MaxDuration(eftdata.voltage,eftdata.frequency,(int)eftdata.period);
		else
			maxdur = EFT_MAXDUR;

		if((EXTRACT_INT(0) < EFT_MINDUR) || (EXTRACT_INT(0) > maxdur))
			return(BAD_VALUE);

		eftdata.duration = (ushort)EXTRACT_INT(0);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_DUR_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT DURATION to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.duration		- Holds the EFT DURATION parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_DUR_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.duration);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_FRQ_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT FREQUENCY parameter
 * Error Checks:	The parameter must be between EFT_MINFRQ and (EFT_MAXFRQ1000
 *					and EFT_MAXFRQ2000).  The upper range is distinguished by
 *					whether an EFT module is a 1 MHz or 2 MHz model
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.frequency	- Holds the current EFT FREQUENCY parameter
 *					[X] eftdata.relay			- Holds the current EFT relay information
 *					[ ] eftdata.eftaddr		- Holds the current EFT module address
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 03/23/93 - James Murphy (JMM)
 *						Support was added for 2 MHz capability.  The 2 MHz
 *						enable relay is RESET and then SET if needed
 *					03/17/94 - James Murphy (JMM)
 *						The limit checking was changed to include a 3 kV cap
 *						on 2 MHz bursts.  The new limits are as follows:
 *								  2 MHz - 3.0 kV Max
 *								  1 MHz - 4.4 kV Max
 *								250 kHz - 8.0 kV Max
 *					12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_FRQ_pgm(P_PARAMT *params)
{
	if ( eftdata.eftflag )
	{
		if((EXTRACT_INT(0) < EFT_MINFRQ) || (EXTRACT_INT(0) > EFT_MAXFRQ2000))
			return(BAD_VALUE);

		if(eftdata.eftaddr != GHOST_BOX)
		{
			if(!(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT2MHZ)
				&& (EXTRACT_INT(0) > EFT_MAXFRQ1000))
				return(BAD_VALUE);

			if((moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT8KV)
				&& (abs(eftdata.voltage) > EFT_MAXVLT4400) && (EXTRACT_INT(0) > EFT_MAXFRQ250))
				return(BAD_VALUE);

			if((EXTRACT_INT(0) > EFT_MAXFRQ1000) && (abs(eftdata.voltage) > EFT_MAXVLT3000))
				return(BAD_VALUE);
		}

		/* Store the frequency in EFTDATA structure */
		eftdata.frequency = EXTRACT_INT(0);

		/* SET the 2 MHz enable relay if frequency is less than 1 MHz */
		if((eftdata.frequency <= 1000000) || (eftdata.waveform))
			*eftdata.relay |= EFT_2MHZENABLE;
		else
			*eftdata.relay &= ~(EFT_2MHZENABLE);

		/* Actually SET the hardware */
		if(eftdata.eftaddr != GHOST_BOX)
			ECAT_WriteRelay1(eftdata.eftaddr,((*eftdata.relay) >> 16));
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_FRQ_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT FREQUENCY to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.frequency	- Holds the current EFT FREQUENCY parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_FRQ_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.frequency);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_MDE_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT mode (Continuous or Momentary)
 * Error Checks:	The parameter is automatically adjusted so as to
 *					make it either 0 (MOMENTARY) or 1 (CONTINUOUS)
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.mode			- Holds the current EFT MODE parameter
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 */
int	EFT_MDE_pgm(P_PARAMT *params)
{
	if ( eftdata.eftflag )
	{
		eftdata.mode = (uchar)(EXTRACT_INT(0) & 0x01);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_MDE_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT MODE to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.mode			- Holds the current EFT MODE parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_MDE_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.mode);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_OUT_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT OUTPUT parameter.  The parameters are
 *					defined as follows:
 *						0, 0/1	- Selects between OUTPUT1 and OUTPUT2
 *						1, bay	- Selects a COUPLER as output (uses OUTPUT2)
 *					A COUPLER is allowed to be selected so as to link the burst
 *					with the COUPLING modes.
 * Error Checks:	The first parameter must be a 0 or 1.  If 0 then the second
 *					parameter must be a 0 or 1.  If 1 then the second parameter
 *					must be a valid EFT COUPLER address.  The OUTPUT driver is
 *					RESET and then SET at this time
 *  Assumptions:	OUTPUT2 is used whenever the first parameter is 1
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.output		- Holds the current EFT OUTPUT parameter
 *					[X] eftdata.relay			- Holds the current EFT relay information
 *					[X] cpldata.outaddr		- Holds the current EFT OUTPUT module
 *					[X] cpldata.outrelay		- Holds the current OUTPUT relay information
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 10/25/94 - James Murphy (JMM)
 *						A change was made to support external couplers with EFT
 *						units with built-in couplers (i.e. E412 and E4551).
 *						If an external coupler is used with a hybrid EFT module,
 *						the output must go through OUTPUT1 instead of OUTPUT2.
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 *					02/13/95 - James Murphy (JMM)
 *						There was a problem with the error checking for EFT
 *						couplers when using mode=1.  This appeared when the
 *						'exists' parameter was deleted starting with version
 *						05.00 and greater.  The error has been corrected in
 *						version 05.00bc
 */
int	EFT_OUT_pgm(P_PARAMT *params)
{
	uchar	mode;
	uchar	addr;

	if(eftdata.eftflag)
	{
		mode = (uchar)(EXTRACT_INT(0));
		addr = (uchar)(EXTRACT_INT(1));

		if((mode > 1) || ((mode == 1) && (addr >= MAX_MODULES)))
			return(BAD_VALUE);

		if((mode == 1) && (!(moddata[(char)(addr)].modinfo.typePrimary & TYPE_CPLEFT)))
			return(BAD_VALUE);

		if(eftdata.eftaddr >= MAX_MODULES)
			return(BAD_VALUE);

		switch(mode)
		{
			case 0:	/* NO Coupler Connection (Front Panel) */
				if(cpldata.outaddr != GHOST_BOX)
				{
					if(cpldata.outaddr != cpldata.srcaddr)
						*cpldata.outrelay &= ~(LED_ACTIVE);
					ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
				}
				eftdata.output		= addr;
				cpldata.outaddr	= GHOST_BOX;
				cpldata.outrelay	= &ghostrelay;
				cpldata.expansion	= (uint *)&ghostrelay;
				break;
			case 1:	/* Coupler Connection (OUT2) */
				if(cpldata.outaddr != GHOST_BOX)
				{
					if((cpldata.outaddr != cpldata.srcaddr) && (eftdata.eftaddr != cpldata.srcaddr))
						*cpldata.outrelay &= ~(LED_ACTIVE);
					ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
				}
				cpldata.outaddr	= addr;
				cpldata.outrelay	= &moddata[(char)(cpldata.outaddr)].relay;
				cpldata.expansion	= &moddata[(char)(cpldata.outaddr)].expansion;

				/* If EFT/CPL Hybrid then output goes through OUT1 NOT OUT2 */
				if((eftdata.eftaddr != cpldata.outaddr) && (moddata[(char)(eftdata.eftaddr)].modinfo.typePrimary & TYPE_CPLEFT))
					eftdata.output = 0;
				else
					eftdata.output = 1;
				*cpldata.outrelay |= LED_ACTIVE;
				ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
				break;
		}

		/* Reset BOTH switches at this time */
		EFT_ClearCouplerHardware();
		*eftdata.relay &= ~(EFT_OUT1ENABLE | EFT_OUT2ENABLE);
		ECAT_WriteRelay1(eftdata.eftaddr,((*eftdata.relay) >> 16));
		TPU_write(CLAMP_TPU,0);									/* RD24 - Out2	*/

		if(eftdata.output)
			*eftdata.relay |= EFT_OUT2ENABLE;
		else
			*eftdata.relay |= EFT_OUT1ENABLE;
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_OUT_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT OUTPUT to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.output		- Holds the current EFT OUTPUT parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_OUT_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.output);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_PER_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT PERIOD parameter
 * Error Checks:	The parameter must be between EFT_MINPER and EFT_MAXPER.
 *					These limits apply to ALL EFT modules
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.period		- Holds the current EFT PERIOD parameter
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 */
int	EFT_PER_pgm(P_PARAMT *params)
{
	if ( eftdata.eftflag )
	{
		if((EXTRACT_INT(0) < EFT_MINPER) || (EXTRACT_INT(0) > EFT_MAXPER))
			return(BAD_VALUE);

		eftdata.period = (ushort)EXTRACT_INT(0);
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_PER_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT PERIOD to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.period		- Holds the current EFT PERIOD parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_PER_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.period);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_VLT_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT VOLTAGE parameter.  The POLARITY
 *					information is stored in <eftdata.relay> and both positive
 *					and negative relays are RESET in the module and then SET
 * Error Checks:	The parameter must be between EFT_MINVLT and (EFT_MAXVLT0
 *					and EFT_MAXVLT1).  The upper range is distinguished by
 *					whether an EFT module is an 8 kV or 4.4 kV model
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.voltage		- Holds the current EFT VOLTAGE parameter
 *					[X] cpldata.outrelay		- Holds the COUPLING RELAY information
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	absvoltage					- Temporary to hold ABSOLUTE VOLTAGE
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 03/16/94 - James Murphy (JMM)
 *						The limit checking was changed to include a 3 kV cap
 *						on 2 MHz bursts.  The new limits are as follows:
 *								  2 MHz - 3.0 kV Max
 *								  1 MHz - 4.4 kV Max
 *								250 kHz - 8.0 kV Max
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_VLT_pgm(P_PARAMT *params)
{
	uint	absvoltage;
	uchar	hvflag;

	if ( eftdata.eftflag )
	{
		absvoltage = ((EXTRACT_INT(0) < 0)?(-EXTRACT_INT(0)):(EXTRACT_INT(0)));
		if((absvoltage < EFT_MINVLT) || (absvoltage > EFT_MAXVLT8000))
			return(BAD_VALUE);

		if(eftdata.eftaddr != GHOST_BOX)
		{
			if(!(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT8KV)
				&& (absvoltage > EFT_MAXVLT4400))
				return(BAD_VALUE);

			if((moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFT8KV)
				&& (eftdata.frequency > EFT_MAXFRQ250) && (absvoltage > EFT_MAXVLT4400))
				return(BAD_VALUE);

			if((eftdata.frequency > EFT_MAXFRQ1000) && (absvoltage > EFT_MAXVLT3000))
				return(BAD_VALUE);
		}
		/*
		 *	If the HIGH Voltage is ON then it must be turned OFF before voltage
		 *	is adjusted for range switching
		 */
		hvflag = 0;
		if(eftdata.hvon)
		{
			EFT_HVoff();
			hvflag = 1;
		}

		*eftdata.relay &= ~(EFT_POSPOL | EFT_NEGPOL | EFT_ACCRISE | EFT_LOWRANGE | EFT_HIGHRANGE);
		ECAT_WriteRelays(eftdata.eftaddr,*eftdata.relay);

 		/* Activate EFT_ACCRISE relay if voltage is greater than 3500 for E411 or E412,
			regardless of coupling mode */
		/*
		if((absvoltage >= 3500) && (absvoltage <= 4400))
			eftdata.need_EFT_ACCRISE_relay = TRUE;
		else
			eftdata.need_EFT_ACCRISE_relay = FALSE;
		*/

		if((absvoltage > EFT_MAXVLT4400) || (eftCheat.forceHVRange))
		{
			*eftdata.relay |= EFT_HIGHRANGE;
			eftdata.voltage = EXTRACT_INT(0);
			eftdata.range   = 1;
		}
		else
		{
			*eftdata.relay |= EFT_LOWRANGE;
			eftdata.voltage = EXTRACT_INT(0);
			eftdata.range   = 0;
		}

		if(eftdata.voltage < 0)
			*eftdata.relay |= EFT_NEGPOL;
		else
			*eftdata.relay |= EFT_POSPOL;

		ECAT_WriteRelays(eftdata.eftaddr,*eftdata.relay);
		if(hvflag)
			EFT_HVon();
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_VLT_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT VOLTAGE to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.voltage		- Holds the current EFT VOLTAGE parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_VLT_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.voltage);
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_WAV_pgm
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the current EFT WAVEFORM parameter
 * Error Checks:	The parameter must be between EFT_MINWAV and EFT_MAXWAV.
 *					These limits apply to ALL EFT modules.  The chirp mode
 *					relay is RESET at this time and then SET
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] eftdata.waveform		- Holds the current EFT WAVEFORM parameter
 *					[X] eftdata.relay			- Holds the EFT relay information
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: 12/13/94 - James Murphy (JMM)
 *						The moddata data structure has been changed so all
 *						accesses have been modified for the new structure
 */
int	EFT_WAV_pgm(P_PARAMT *params)
{
	if ( eftdata.eftflag )
	{
		if((EXTRACT_INT(0) < EFT_MINWAV) || (EXTRACT_INT(0) > EFT_MAXWAV))
			return(BAD_VALUE);

		/* If NO-CHIRP option is set then CHIRP waves are NOT allowed */
		if(eftdata.eftaddr != GHOST_BOX)
			if(!(moddata[(char)(eftdata.eftaddr)].modinfo.options & OPT_EFTCHIRP)
				&& (EXTRACT_INT(0) > EFT_MINWAV))
				return(BAD_VALUE);

		eftdata.waveform = (uchar)EXTRACT_INT(0);

		if(eftdata.waveform)
			*eftdata.relay |= EFT_CHIRP;
		else
			*eftdata.relay &= ~(EFT_CHIRP);

		if((eftdata.frequency <= 1000000) || (eftdata.waveform))
			*eftdata.relay |= EFT_2MHZENABLE;
		else
			*eftdata.relay &= ~(EFT_2MHZENABLE);

		if(eftdata.eftaddr != GHOST_BOX)
			ECAT_WriteRelay1(eftdata.eftaddr,((*eftdata.relay) >> 16));
	}
	return(0);
}

/*[ÿ]
 *      Routine:	EFT_WAV_rpt
 *       Module:	CMD_EFT.C
 * Date Created:	February 15, 1993
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current EFT WAVEFORM to <destination>
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] eftdata.waveform		- Holds the current EFT WAVEFORM parameter
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 */
int	EFT_WAV_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",eftdata.waveform);
	return(0);
}

int	EFT_SET_pgm(P_PARAMT *params)
{
	char	errcode;

	if ( eftdata.eftflag )
	{
		errcode = EFT_StartSystem(0);
		if(errcode)
			EFT_StopSystem();
		fprintf(destination,"%d",errcode);
	}
	return(0);
}

int	EFT_SET_rpt(P_PARAMT *params)
{
	return(0);
}

int EFT_LCK_pgm(P_PARAMT* params)
{
	return SRG_LCK_pgm(params);
}

/**** SLOPE CALIBRATION CALCULATIONS ARE DONE HERE ****/

ushort	EFT_CalCompute(
CALINFO	*calptr,
int		user_voltage,
uchar	range)
{
	int hv_range_offset;
	int coupler_offset;
	int	lo;
	int	hi;
	int	pgmlo;
	int	pgmhi;
	float	m;
	float	b;
	int adjusted_voltage;
	int	dacval;

	if ( range == 0 )
		hv_range_offset = 0;  /* normal module */
	else if ( range == 1 )
		hv_range_offset = CAL_HV_OFFSET;  /* HV module (E421) */
	else
		return(0);

	/* Coupler cal data is in data block after Clamp cal data.
	   Coupler cal data is used if the Output is set to anything other than OUT1 or OUT2
	   (e.g. E412 built-in coupler, E411 with output redirected to external coupler) */
	if ( cpldata.outaddr != GHOST_BOX )
		coupler_offset = CAL_CPL_OFFSET;	/* output to coupler (including E412 & other modules w/ built-in coupler) */
	else
		coupler_offset = CAL_CLAMP_OFFSET;	/* output to clamp (OUT1 or OUT2) */


	lo = (int)(calptr->data[CAL_LLOOFF_CLAMP + hv_range_offset + coupler_offset]);
	hi = (int)(calptr->data[CAL_LHIOFF_CLAMP + hv_range_offset + coupler_offset]);

	if ( ( hi - lo ) == 0 )
		return(0);

	if(range)
	{
		pgmlo = 4401;
		pgmhi = 8000;
	}
	else
	{
		pgmlo = 250;
		pgmhi = 4400;
	}
	m = (float)(pgmhi - pgmlo) / (float)(hi - lo);
	b = ( (float)hi * m ) - (float)pgmhi;

	/* Use absolute value of voltage */
	adjusted_voltage = abs( user_voltage );

	/* Linear calibration adjustment */
	adjusted_voltage = m*(float)adjusted_voltage-b;

	/* Coupler multi-line adjustment - decrease output 6% for one or two lines coupled */
	if ( ( cpldata.multihigh == 1 ) || ( cpldata.multihigh == 2 ) )
		adjusted_voltage = ( adjusted_voltage * 100 ) / 106;

	/* Negative voltage adjustment - boost output 5% */
	if( user_voltage < 0 )
		adjusted_voltage = ( adjusted_voltage * 105 ) / 100;

	/* Convert to DAC bits */
	if(range)
		dacval = (int)(0.2560 * (float)adjusted_voltage);
	else
		dacval = (int)(0.4095 * (float)adjusted_voltage);
	
 	/* Clamp output to 2350 bits (which for 0.4095 bits/V is 5738 V,
		the min required to achieve 4400 V out) */
 	if(dacval > 2350)
		dacval = 2350;  // (2350 bits) / (0.4095 bits/V) = 5738 V

	return((ushort)(dacval));
}
/******* << END CMD_EFT.C >> *******/
