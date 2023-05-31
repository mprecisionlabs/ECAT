#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "tiocntl.h"

extern FILE		*destination;
extern MODDATA	moddata[];
extern CPLDATA	cpldata;
extern EHVDATA	ehvdata;
extern const WAVDATA	wavdata[];
extern const EHVINFO	ehvinfo[];


/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Calibration Data Offset
 *	Parameter 2 = Calibration Data
 */
int	BAY_CAL_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);
	if(EXTRACT_INT(1) >= CALINFOSIZE)
		return(BAD_VALUE);
	if(EXTRACT_INT(2) > 0x0000FFFF)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.calinfo.data[EXTRACT_INT(1)] = (ushort)EXTRACT_INT(2);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Calibration Data Offset
 *	If parameter 1 is -1 then ALL the calibration data will be sent
 *	in a hexadecimal format like the DUMP command
 */
int	BAY_CAL_rpt(P_PARAMT *params)
{
	uchar		bayaddr;
	char		tvar;
	ushort	*tmpptr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);
	if((EXTRACT_INT(1) >= CALINFOSIZE) && (EXTRACT_INT(1) != -1))
		return(BAD_VALUE);

	if(EXTRACT_INT(1) == -1)
	{
		for(tvar = 0; tvar < CALINFOSIZE; tvar++)
			fprintf(destination,"%04X",moddata[(char)(bayaddr)].modinfo.calinfo.data[tvar]);
	}
	else
		fprintf(destination,"%04X",moddata[(char)(bayaddr)].modinfo.calinfo.data[EXTRACT_INT(1)]);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_DFLT_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
 	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	MOD_DefaultModinfo(bayaddr);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Wave Number
 */
int	BAY_DLY_rpt(P_PARAMT *params)
{
	uchar		bayaddr;
	uchar		wave;
				
	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	wave = (uchar)EXTRACT_INT(1);
	if(wave >= MAX_WAVEFORMS)
		return(BAD_VALUE);

	if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_SRG))
		return(BAD_VALUE);

	fprintf(destination,"%d",SRG_MinimumDelay(bayaddr,wave));
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_DUMP_rpt(P_PARAMT *params)
{
	uchar		bayaddr;
	char		tvar;
	uchar		*tmpptr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	tmpptr = (uchar *)&moddata[(char)(bayaddr)].modinfo;
	for(tvar = 0; tvar < 26; tvar++)
		fprintf(destination,"%02X",tmpptr[tvar]);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Module Identification Data
 */
int	BAY_ID_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(EXTRACT_INT(1) > 0x00FF)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.id = (uchar)EXTRACT_INT(1);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_ID_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	fprintf(destination,"%d",moddata[(char)(bayaddr)].modinfo.id);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Encoded V/I Monitor Data
 */
int	BAY_MEAS_pgm(P_PARAMT *params)
{
	uchar	bayaddr;
	uchar	vibyte;

	bayaddr = (uchar)EXTRACT_INT(0);
	vibyte  = (uchar)EXTRACT_INT(1);

	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);
	if(((vibyte & 0x0F) > 0x05) || ((vibyte & 0xF0) > 0x50))
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.viMonitors = (uchar)vibyte;
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_MEAS_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	fprintf(destination,"%d",moddata[(char)(bayaddr)].modinfo.viMonitors);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Module Name
 */
int	BAY_NAME_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(strlen(EXTRACT_STRING(1)) > (MODNAMESIZE - 1))
		return(BAD_VALUE);

	strcpy(moddata[(char)(bayaddr)].modinfo.name,EXTRACT_STRING(1));
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_NAME_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);
	fprintf(destination,"%s",moddata[(char)(bayaddr)].modinfo.name);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Module Options
 */
int	BAY_OPTS_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.options = (uint)EXTRACT_INT(1);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_OPTS_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	fprintf(destination,"%d",moddata[(char)(bayaddr)].modinfo.options);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Module Serial Number
 */
int	BAY_SER_pgm(P_PARAMT *params)
{
	uchar		bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.serialNumber = (uint)EXTRACT_INT(1);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_SER_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	fprintf(destination,"%d",moddata[(char)(bayaddr)].modinfo.serialNumber);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Primary Module Type
 *	Parameter 2 = Secondary Module Type
 */
int	BAY_TYPE_pgm(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.typePrimary		= (uint)EXTRACT_INT(1);
	moddata[(char)(bayaddr)].modinfo.typeSecondary	= (uint)EXTRACT_INT(2);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_TYPE_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	fprintf(destination,"%d %d",moddata[(char)(bayaddr)].modinfo.typePrimary,
				moddata[(char)(bayaddr)].modinfo.typeSecondary);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_WRTE_pgm(P_PARAMT *params)
{
	uchar	bayaddr;
	char	tvar;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	moddata[(char)(bayaddr)].modinfo.calinfo.chksum = 0x0000;
	for(tvar = 0; tvar < CALINFOSIZE; tvar++)
		moddata[(char)(bayaddr)].modinfo.calinfo.chksum += moddata[(char)(bayaddr)].modinfo.calinfo.data[tvar];

	tvar = ECAT_9346Write(bayaddr,(void *)&moddata[(char)(bayaddr)].modinfo,sizeof(MODINFO));
	fprintf(destination,"%d",tvar);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 */
int	BAY_CHK_rpt(P_PARAMT *params)
{
	uchar	bayaddr;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

	if(ECAT_InterfaceCheck(bayaddr))
	{
		if(!moddata[(char)(bayaddr)].chksumflag)
			fputs("-2",destination);
		else
			if(!moddata[(char)(bayaddr)].calsumflag)
				fputs("-3",destination);
			else
				fputs("0",destination);
	}
	else
		fputs("-1",destination);
	return(0);
}

/*
 *	Parameter 0 = Module Address
 *	Parameter 1 = Waveform Number
 */
int	BAY_WAV_rpt(P_PARAMT *params)
{
	uchar	bayaddr;
	uchar	count;
	char	wavnum;
	uchar	minDelay;
	uchar	ehvId;
	short	maxVolts;

	bayaddr = (uchar)EXTRACT_INT(0);
	if(bayaddr >= MAX_MODULES)
		return(BAD_VALUE);

 	if(!(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_SRG))
		return(BAD_VALUE);

	if(EXTRACT_INT(1) > MAX_WAVEFORMS)
		return(BAD_VALUE);

	if(EXTRACT_INT(1) == 0)
	{
		count = wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].numWaves;
		switch(moddata[(char)(bayaddr)].modinfo.id)
		{
            case E502_BOX:
            case E502A_BOX:
            case E502B_BOX:
            case E502C_BOX:
            case E502H_BOX:
            case E502K_BOX:
                if ( !(moddata[(char)(bayaddr)].modinfo.options & OPT_SWE502) )
                    count--;
                break;
            case E508_BOX:
                if ( cpldata.E50812Paddr >= MAX_MODULES )
                    count--;
                break;
		}
		fprintf(destination,"%d",count);
	}
	else
	{
		wavnum = (char)(EXTRACT_INT(1) - 1);
		fprintf(destination,"%d ",wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].numWaves);
		fprintf(destination,"%d ",wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[wavnum].frontPanelFlag);
		for(count = 0; count < MAX_CPLTYPES; count++	)
			fprintf(destination,"%d ",wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[wavnum].cplFlag[(char)(count)]);
		for(count = 0; count < MAX_CPLTYPES; count++	)
			fprintf(destination,"%d ",wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[wavnum].maxVoltage[(char)(count)]);

		for(count = 0; count < MAX_CPLTYPES; count++)
		{
			minDelay = wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[wavnum].minDelay[(char)(count)];
			maxVolts = wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[(char)(wavnum)].maxVoltage[(char)(count)];
			maxVolts = SRG_TranslateVoltage(bayaddr,wavnum,(int)maxVolts);
			if((ehvdata.ehvflag) && (ehvdata.network < MAX_MODULES) && (moddata[(char)(bayaddr)].modinfo.options & OPT_EHVCAPABLE))
			{
				ehvId = moddata[(char)(ehvdata.network)].modinfo.id;
				if(maxVolts <= ehvinfo[(char)(ehvId)].maxVoltage)
				{
					minDelay = ((minDelay - 2 ) / ehvinfo[(char)(ehvId)].factor) + 2;
					if(minDelay < 3)
						minDelay = 3;
				}
			}
			fprintf(destination,"%d ",minDelay);
		}
		fprintf(destination,",%s",wavdata[(char)(moddata[(char)(bayaddr)].modinfo.id)].wavinfo[wavnum].name);
	}
	return(0);
}
/******* << END CMD_BAY.C >> *******/
