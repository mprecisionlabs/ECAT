#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "data.h"

extern FILE		*destination;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;

/*
 *      Routine:	DATA_CLAMP_pgm
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the CLAMP mode for DATA couplers.  The hardware is
 *					also set up this point
 * Error Checks:	The only parameter is the clamp selection which must fall
 *					within the range of 0 to 2
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] cpldata.clamp			- Holds the DATA coupler's CLAMP value
 *					[X] cpldata.outrelay		- Holds the COUPLING RELAY information
 *					[ ] cpldata.outaddr		- Holds the OUTPUT module address
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	clamp							- Temporary clamp value
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: N/A
 */
int	DATA_CLAMP_pgm(P_PARAMT *params)
{
	uchar	clamp;

	if(cpldata.outaddr > MAX_MODULES)
		return(BAD_VALUE);

	if(!moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
		return(BAD_VALUE);

	clamp = (uchar)(EXTRACT_INT(0));
	if(clamp > CLAMP_EXT)
		return(BAD_VALUE);

	cpldata.clamp = clamp;
	*cpldata.outrelay &= ~(RLY_INT20V);

	if(cpldata.clamp != CLAMP_INT220V)
		*cpldata.outrelay |= (RLY_INT20V);

	ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
	return(0);
}

/*
 *      Routine:	DATA_CLAMP_rpt
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current CLAMP value to <destination>
 *					(for DATA couplers only)
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] cpldata.clamp			- Holds the DATA coupler's CLAMP value
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 *    Revisions: N/A
 */
int	DATA_CLAMP_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.clamp);
	return(0);
}

/*
 *      Routine:	DATA_CMODE_pgm
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the COUPLING-MODE for DATA couplers. The hardware
 *					must be set at the time of the surge event (like the
 *					coupling and symmetric relays)
 * Error Checks:	The only parameter is the CMODE selection which is
 *					automatically scaled to fall within range (0 | 1).
 *					The CMODE can be changed regardless of the selected
 *					surge network (i.e. E508 does not use the CMODE selection)
 *					since it will not affect any modules that do not use it
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] cpldata.cmode			- Holds the DATA coupler's CMODE value
 *					[ ] cpldata.outaddr		- Holds the OUTPUT module address
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: N/A
 */
int	DATA_CMODE_pgm(P_PARAMT *params)
{
	if(cpldata.outaddr > MAX_MODULES)
		return(BAD_VALUE);

	if(!moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
		return(BAD_VALUE);

	cpldata.cmode = (uchar)(EXTRACT_INT(0) & 0x01);
	return(0);
}

/*
 *      Routine:	DATA_CMODE_rpt
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current COUPLING MODE value to <destination>
 *					(for DATA couplers only)
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] cpldata.cmode			- Holds the DATA coupler's CMODE value
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 *    Revisions: N/A
 */
int	DATA_CMODE_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.cmode);
	return(0);
}

/*
 *      Routine:	DATA_OUT_pgm
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Sets the OUTPUT enable for DATA couplers.  The hardware is
 *					also set up this point
 * Error Checks:	The only parameter is the output selection which is
 *					automatically scaled to fall within range (0 | 1)
 *  Assumptions:	None
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[X] cpldata.output		- Holds the DATA coupler's OUTPUT value
 *					[X] cpldata.outrelay		- Holds the COUPLING RELAY information
 *					[ ] cpldata.outaddr		- Holds the OUTPUT module address
 *					[ ] moddata					- Holds ALL MODULE information
 *       Locals:	None
 *      Returns:	0	- Success
 *					BV	- All errors
 *    Revisions: N/A
 */
int	DATA_OUT_pgm(P_PARAMT *params)
{
	if(cpldata.outaddr > MAX_MODULES)
		return(BAD_VALUE);

	if(!moddata[(char)(cpldata.outaddr)].modinfo.typePrimary & TYPE_CPLDATA)
		return(BAD_VALUE);

	cpldata.output = (uchar)(EXTRACT_INT(0) & 0x01);

	*cpldata.outrelay &= ~(RLY_DATAOUT | LED_VOLTAGE);
	if(cpldata.output)
		*cpldata.outrelay |= (RLY_DATAOUT | LED_VOLTAGE);

	ECAT_WriteRelay1(cpldata.outaddr,(*cpldata.outrelay >> 16));
	ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
	ECAT_TimerSequencer(cpldata.outaddr,(*cpldata.outrelay >> 24));
	return(0);
}

/*
 *      Routine:	DATA_OUT_rpt
 *       Module:	CMD_DATA.C
 * Date Created:	December 29, 1994
 *       Author:	James Murphy (JMM)
 *     Language:	Microtec Research Inc MCC68K 'C'
 *      Purpose:	Prints out the current OUTPUT value to <destination>
 *					(for DATA couplers only)
 * Error Checks:	None
 *  Assumptions:	<destination> is a valid FILE pointer
 *   Known Bugs:	None
 *   Parameters:	[ ] paraml					- Parser-defined parameter (required)
 *      Globals:	[ ] cpldata.output		- Holds the DATA coupler's OUTPUT value
 *					[ ] destination			- FILE pointer for output
 *      Returns:	0	- Success
 *    Revisions: N/A
 */
int	DATA_OUT_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.output);
	return(0);
}
/******* << END CMD_DATA.C >> *******/
