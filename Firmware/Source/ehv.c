#include <stdio.h>
#include <ctype.h>
#include <mriext.h>
#include <math.h>
#include "timer.h"
#include "intface.h"
#include "ecat.h"
#include "module.h"
#include "parser.h"
#include "ehv.h"

extern MODDATA	moddata[];
extern EHVDATA	ehvdata;

const EHVINFO ehvinfo[MAX_EHVBOXES] =
{ { 2200, 3 }, { 10000, 3 }, { 2200, 5 } };

void	EHV_FindModules(void)
{
	char		baycnt;
	MODINFO	*modptr;
	
	ehvdata.ehvflag = FALSE;
	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if((modptr->typePrimary & TYPE_EHV) && (modptr->id < MAX_EHVBOXES))
			ehvdata.ehvflag = TRUE;
	}
}

void	EHV_ResetModules(
uchar	extent)
{
	char	baycnt;

	if(extent)
		ehvdata.network = GHOST_BOX;

	for(baycnt = MAX_MODULES-1; baycnt >= 0; baycnt--)
		if(moddata[baycnt].modinfo.typePrimary & TYPE_EHV)
		{
			if(extent)
				ehvdata.network = baycnt;
			moddata[baycnt].expansion = 0x0300;				/* Disable Both HVPS	*/
			moddata[baycnt].relay = 0x00000000;
			ECAT_WriteRelays(baycnt,moddata[baycnt].relay);
			ECAT_WriteEXPRelay0(baycnt,moddata[baycnt].expansion);
		}
	/* Reset the EHV power supply */
	EHV_HVProgram(0,0);
}

/*
 *	Enable power supply and set HV power supply in EHV network
 *
 *      Returns:	 0	- Success
 *					-1	- Illegal bay choice
 *					-2 - Interface board does not exist
 *					-3	- DAC Overflow
 */
char	EHV_HVProgram(
int		voltage,
uchar	enableFlag)
{
	ushort	ehvId;
	uint		dacval;
	char		errval;

	if(ehvdata.network >= MAX_MODULES)
		return(-1);

	ehvId = moddata[(char)(ehvdata.network)].modinfo.id;
	if(ehvId >= MAX_EHVBOXES)
		return(-2);

	errval = 0;
	if((dacval > 0) && (enableFlag))
	{
		dacval = (uint)(abs(voltage) * 4095) / (uint)(ehvinfo[(char)(ehvId)].maxVoltage);
		if(dacval > 4095)
		{
			dacval = 4095;
			errval = -3;
		}
		moddata[(char)(ehvdata.network)].expansion |= 0x0001;
		ECAT_WriteEXPRelay0((char)(ehvdata.network),moddata[(char)(ehvdata.network)].expansion);
		TPU_delay(40);
		if(ehvId == EHV10_BOX)
			moddata[(char)(ehvdata.network)].expansion &= ~(0x0300);
		else
			moddata[(char)(ehvdata.network)].expansion &= ~(0x0100);
		ECAT_WriteEXPRelay0((char)(ehvdata.network),moddata[(char)(ehvdata.network)].expansion);
		E52x_SetHVPS((int)ehvdata.network, dacval, DR_12);
	}
	else
	{
		E52x_SetHVPS(ehvdata.network, 0, DR_12);
		moddata[(char)(ehvdata.network)].expansion = 0x0300;
		ECAT_WriteEXPRelay0((char)(ehvdata.network),moddata[(char)(ehvdata.network)].expansion);
		TPU_delay(40);
	}
	return(errval);
}
/******* << END EHV.C >> *******/
