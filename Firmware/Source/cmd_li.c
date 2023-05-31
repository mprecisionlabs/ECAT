#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "eft.h"

extern FILE    *destination;
extern CPLDATA cpldata;
extern MODDATA moddata[];

int   LI_ANG_pgm(P_PARAMT *params)
{
	if(cpldata.outaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(!(moddata[(char)cpldata.outaddr].modinfo.typePrimary & TYPE_CPLGEN))
		return(BAD_VALUE);

	if((EXTRACT_INT(0) < 0) || (EXTRACT_INT(0) > 360))
		return(BAD_VALUE);

	cpldata.angle = (ushort)EXTRACT_INT(0);
	return(0);
}

int   LI_ANG_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.angle);
	return(0);
}

/*
 * Parameter 0: Phase Mode (Random, L1, L2, L3, DC)
 * Modification by James Murphy (JMM) on 06/12/95
 * The DC mode has been eliminated. DC is now simply a variant
 * of the RANDOM mode
 */
int   LI_MODE_pgm(P_PARAMT *params)
{
	if(cpldata.outaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(!(moddata[(char)cpldata.outaddr].modinfo.typePrimary & TYPE_CPLGEN))
		return(BAD_VALUE);

	if(moddata[(char)cpldata.outaddr].modinfo.typePrimary & TYPE_CPLDATA)
		return(BAD_VALUE);
	
/* If 400 Hz hack, then phase mode is always RANDOM, regardless of what 
	display indicates. */
#ifndef HACK_400Hz
	cpldata.phasemode = (uchar)(EXTRACT_INT(0) & 0x03);
#else
	cpldata.phasemode = (uchar)0;
#endif
	
	*cpldata.outrelay  &= ~(CPL_REFRLY0 | CPL_REFRLY1);
	*cpldata.expansion &= ~(CPL_422REF0 | CPL_422REF1);
	switch(cpldata.phasemode)
	{
		case LS_RNDMODE:  /* Random Reference  */
		case LS_L1MODE:   /* L1 Reference      */
		case 4:           /* 'OLD' DC Mode     */
				break;
		case LS_L2MODE:   /* L2 Reference      */
			*cpldata.outrelay  |= CPL_REFRLY0;
			*cpldata.expansion |= CPL_422REF0;
			break;
		case LS_L3MODE:   /* L3 Reference      */
			*cpldata.outrelay  |= CPL_REFRLY1;
			*cpldata.expansion |= CPL_422REF1;
			break;
		default:
			return(BAD_VALUE);
	}
	if(cpldata.outaddr != GHOST_BOX)
		ECAT_WriteRelays(cpldata.outaddr,*cpldata.outrelay);
	return(0);
}

int   LI_MODE_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",cpldata.phasemode);
	return(0);
}
/******* << END CMD_LI.C >> *******/
