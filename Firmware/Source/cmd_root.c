#include <stdio.h>
#include <mriext.h>
#include "parser.h"
#include "switches.h"
#include "intface.h"
#include "ecat.h"
#include "tiocntl.h"
#include "front.h"
#include "eft.h"
#include "rev.h"
#include "acrms.h"
#include "emc.h"

typedef struct tag_bdmpinfo
{
	uchar		vioption;				/* V/I option information (VMON and IMON)	*/
	uchar		yearmonth;				/* Year/Month portion of serial number		*/
	ushort	tridigit;				/* Three digit section of serial number	*/
	char		name[MODNAMESIZE];	/* Actual Name of module (i.e. "E501")		*/
	ushort	modid;					/* The ID number of the module (internal)	*/
	uchar		modtype;					/* Type of system module						*/
	uchar		revision;				/* Hardware revision number of module		*/
	ushort	caldata[44];			/* Holds ALL module-dependent cal data		*/
	ushort	chksum;					/* Secondary checksum just for cal data	*/
} BDMPINFO;

BDMPINFO    bdmpdata;

extern FILE     *destination;
extern MODDATA  moddata[];
extern CPLDATA  cpldata;
extern SRGDATA  srgdata;
extern EFTDATA  eftdata;
extern ACDATA   acdata;
extern EMCDATA  emcdata;
extern PQFDATA  pqfdata;
extern uchar    ecatmode;

extern volatile CHGDATA chgdata;
extern volatile uint	ilockstate;
extern volatile ushort	ldo_state;
extern volatile ushort	PEAK_LD5;
extern volatile uchar	PQF_softstartflag;

extern P_PARAMT paraml[3];

static int baud;  /* the current serial port bit rate setting */

extern void	blue_off(void);

/*
 *	ROOT commands start here
 */

int root_CPL_pgm(P_PARAMT *params)
{
    uchar       bayaddr;
    P_PARAMT tparam;

    bayaddr = (uchar)EXTRACT_INT(0);
    if ( bayaddr > MAX_MODULES )
        return BAD_VALUE;

    if ( !(moddata[(char)(bayaddr)].modinfo.typePrimary & TYPE_CPLLINE) )
        return BAD_VALUE;

    /* Special cases for E505B, because internal CDN can only be used by this module:
       - E505B EUT source must be E505B
       - no other modules can use E505B source
    */
    if ( ecatmode == SRG_MODE )
    {
        if ( ( moddata[srgdata.address].modinfo.id == E505B_BOX )   // selected surge module
          || ( moddata[bayaddr].modinfo.id == E505B_BOX ) )         // requested EUT source module
        {
            if ( bayaddr != srgdata.address )
                return BAD_VALUE;
        }
    }

    if ( cpldata.srcaddr != bayaddr )
    {
        tparam.idata = 0;
        root_EUT_pgm(&tparam);
    }

    if ( cpldata.srcaddr != GHOST_BOX )
    {
        if ( ((ecatmode == EFT_MODE) && (eftdata.eftaddr != cpldata.srcaddr)) ||
             ((ecatmode != EFT_MODE) && (cpldata.outaddr != cpldata.srcaddr)) )
        {
            *cpldata.srcrelay &= ~(LED_ACTIVE);
            ECAT_WriteRelays(cpldata.srcaddr,*cpldata.srcrelay);
        }
    }
    cpldata.srcaddr =  bayaddr;
    cpldata.srcrelay    =  &moddata[bayaddr].relay;
    *cpldata.srcrelay   |= LED_ACTIVE;

    /* Special case for E505B:  for CDN waveform, if EUT is active, retain state of LED_VOLTAGE
       (E505B is combined surge/CDN module) */
    /*
    if ( ( moddata[bayaddr].modinfo.id == E505B_BOX ) && ( srgdata.waveform == E505B_2500V ) )
    {
        cpldata.eutswitch = !(read_pirq() & 0x20);
        if(cpldata.eutrequest && cpldata.eutswitch)
            *cpldata.srcrelay |= LED_VOLTAGE;
    }
    */
    ECAT_WriteRelays(bayaddr,*cpldata.srcrelay);

    RMS_SetNetwork(bayaddr);

    TIO_CPLDisable();
    TIO_SelectLinesync(cpldata.srcaddr);
    return 0;
}

int	root_CPL_rpt(P_PARAMT *params)
{
	if(cpldata.srcaddr != GHOST_BOX)
		fprintf(destination,"%d",cpldata.srcaddr);
	return(0);
}

int	root_EUT_pgm(P_PARAMT *params)
{
	/* PQF DEMO INSERTION */
	if(ecatmode == PQF_MODE)
	{
		cpldata.eutrequest = EXTRACT_INT(0) & 0x01;
		if(!cpldata.eutrequest)
		{
			ldo_state &= ~(0x2020);
			PEAK_LD5 = ldo_state;						/* Shut off EUT hardware	*/
			cpldata.eutblue = OFF;						/* Shut off BLUE light		*/
			PQF_softstartflag = TRUE;
		}
		return(0);
	}

	if((cpldata.srcaddr == GHOST_BOX) && (!emcdata.modeFlag))
		return(BAD_VALUE);

	cpldata.eutrequest = EXTRACT_INT(0) & 0x01;
	if(!cpldata.eutrequest)
		blue_off();
	return(0);
}

int	root_EUT_rpt(P_PARAMT *params)
{
	if(cpldata.eutrequest && cpldata.eutblue)
		fputs("2",destination);
	else
		fprintf(destination,"%d",cpldata.eutrequest);
	return(0);
}

/*
 *	COMMON commands start here
 */
int	star_IDN_rpt(P_PARAMT *params)
{
	fprintf(destination,"KeyTek Instrument,ECAT,9301444,%2s%2s",REV_MAJOR,REV_MINOR);
	return(0);
}

int	star_OPC_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",chgdata.flag);
	return(0);
}

int	star_RST_pgm(P_PARAMT *params)
{
	ECAT_ModeChange(GHOST_MODE);
	return(0);
}

int	star_TRG_pgm(P_PARAMT *params)
{
	char	errcode;

	if((ilockstate & 0x7C) != 0x7C)
	{
		fprintf(destination,"%d",ERR_SRGIL);
		return;
	}

	switch(EXTRACT_INT(0))
	{
		case 1:		/* SRG trigger	*/
			errcode = SRG_SurgeSystem();
			if(errcode)
			{
				SRG_StopSystem();
				fprintf(destination,"%d",errcode);
			}
			break;
		case 2:		/* EFT trigger	*/
			errcode = EFT_StartSystem(1);
			if(errcode)
				EFT_StopSystem();
			fprintf(destination,"%d",errcode);
			break;
		case 3:		/* PQF trigger	*/
			break;
		case 4:		/* ESD trigger	*/
			break;
		default:
			return(BAD_VALUE);
	}
	return(0);
}

int	root_ABT_pgm(P_PARAMT *params)
{
	switch(ecatmode)
	{
		case EFT_MODE:
			EFT_StopSystem();
			break;
		case SRG_MODE:
			chgdata.time = 0;
			SRG_StopSystem();
			break;
		case PQF_MODE:
		default:
			break;
	}
	return(0);
}

/*
 *	SYSTEM subsystem commands start here
 */
int	SYS_ILOK_rpt(P_PARAMT *params)
{
	if((ilockstate & 0x7C) == 0x7C)
		fputc('0',destination);
	else
		fputc('1',destination);
	return(0);
}

int	SYS_ITXT_rpt(P_PARAMT *params)
{
	char	faultmodule;

	if((ilockstate & 0x7C) != 0x7C)
		ReportInterlockError(ilockstate,destination);
	return(0);
}

int	SYS_JJAW_pgm(P_PARAMT *params)
{
	char	tvar;
	char	calcnt;
	uchar	bayaddr;
	int	rv;

	bayaddr = (uchar)EXTRACT_INT(0);
	switch(bayaddr)
	{
		case  0: case  1: case  2: case  3: case  4: case  5: case  6: case  7:
		case  8: case  9: case 10: case 11: case 12: case 13: case 14: case 15:
		case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
		case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
			rv = ECAT_9346Read(bayaddr,(void *)&moddata[(char)(bayaddr)].modinfo,sizeof(MODINFO));
			if((!rv) || (EXTRACT_INT(0) > 15))
			{
				if(EXTRACT_INT(0) > 15)
					tvar = EXTRACT_INT(0) - 16;
				else
					tvar = EXTRACT_INT(0);
				fprintf(destination,"\nSER  = %d\n",moddata[tvar].modinfo.serialNumber);
				fprintf(destination,"OPT  = %08X\n",moddata[tvar].modinfo.options);
				fprintf(destination,"VI   = %02X\n",moddata[tvar].modinfo.viMonitors);
				fprintf(destination,"NAME = %s\n",  moddata[tvar].modinfo.name);
				fprintf(destination,"ID   = %02X\n",moddata[tvar].modinfo.id);
				fprintf(destination,"TYPP = %08X\n",moddata[tvar].modinfo.typePrimary);
				fprintf(destination,"TYPS = %08X\n",moddata[tvar].modinfo.typeSecondary);
				fprintf(destination,"CSUM = %04X\n",moddata[tvar].modinfo.calinfo.chksum);
				for(calcnt = 0; calcnt < CALINFOSIZE; calcnt++)
				{
					fprintf(destination,"%5d ",moddata[tvar].modinfo.calinfo.data[calcnt]);
					if((calcnt % 11) == 10)
						fputc('\n',destination);
				}
			}
			else
				fputs("READ FAILED",destination);
			break;
		case 44:
			for(tvar = 0; tvar < MAX_MODULES; tvar++)
				fprintf(destination,"MOD %02d = %d\n",tvar,(int)ECAT_InterfaceCheck(tvar));
			break;
		default:
			show_csel();
	}
	return(0);
}

int	SYS_MEM_rpt(P_PARAMT *params)
{
	uint		addr;
	ushort	value;

	addr	= EXTRACT_INT(0);
	value	= (*(ushort *)(addr));
	fprintf(destination,"MEM[%08X] = %08X\n",(int)addr,(uint)value);
	return(0);
}

int	SYS_MEM_pgm(P_PARAMT *params)
{
	uint		addr;
	ushort	value;

	addr	= EXTRACT_INT(0);
	value	= EXTRACT_INT(1);
	(*(ushort *)(addr)) = value;
	return(0);
}

int	SYS_BAUD_rpt(P_PARAMT *params)
{
	fprintf(destination,"%d",baud);
	return(0);
}

void change_baud_rate()
{
    SCI_init(baud,1);
    return;
}

int	SYS_BAUD_pgm(P_PARAMT *params)
{
    int new_baud = EXTRACT_INT(0);
    switch ( new_baud )
    {
        case 1200:
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            baud = new_baud;
            // delay for 100 ms to allow command response to be sent before changing
            TPU_delay_int( TICKS_PER_SECOND / 10, change_baud_rate );
            break;
        default:
            return BAD_VALUE;
    }
	return(0);
}

int SYS_RLY_pgm(P_PARAMT *params)
{
	uchar	bay;
	uchar	state;
	uint	relay;
	uint	mask;

	bay	= EXTRACT_INT(0);
	relay	= EXTRACT_INT(1);
	state	= EXTRACT_INT(2);

	if((relay > 63) || ( bay > MAX_MODULES))
		return(BAD_VALUE);

	if(relay > 31)
	{
		relay = relay - 32;		/* Base off of ZERO */
		mask = 1 << relay;
		moddata[(char)(bay)].expansion &= (~mask);
		if(state)
			moddata[(char)(bay)].expansion |= mask;

		ECAT_WriteEXPRelay0(bay,*cpldata.expansion);
		ECAT_WriteEXPRelay1(bay,(*cpldata.expansion >> 16));
	}
	else
	{
		mask = 1 << relay;
		moddata[(char)(bay)].relay &= (~mask);
		if(state)
			moddata[(char)(bay)].relay |= mask;
		ECAT_WriteRelays(bay,moddata[bay].relay);
	}
	return(0);
}

int SYS_RLY_rpt(P_PARAMT *params)
{
	printf("Bay[%02d] = %08X\n",EXTRACT_INT(0),moddata[EXTRACT_INT(0)].relay);
	return(0);
}

int	SYS_BDMP_rpt(P_PARAMT *params)
{
	uchar	bayaddr;
	char	errval;
	uchar	tvar;
	uchar	*tmpptr;
	uchar	chksum;

	bayaddr = (uchar)EXTRACT_INT(0);
	errval = ECAT_9346Read(bayaddr,(void *)&bdmpdata,sizeof(BDMPINFO));
	switch(errval)
	{
		case 0:		/* EEPROM has been converted */
			fputs("ERR-0",destination);
			return(0);
			break;
		case -1:		/* Illegal Module choice	*/
			fputs("ERR-1",destination);
			return(0);
			break;
		case -2:		/* Module does not exist	*/
			fputs("ERR-2",destination);
			return(0);
			break;
		case -3:		/* Storage ERRORs (FATAL)	*/
		case -4:
		default:
			fputs("ERR-3",destination);
			return(0);
			break;
		case -5:		/* BAD checksum, CONVERT!!	*/
			errval = ECAT_9346Read2(bayaddr,(void *)&bdmpdata,sizeof(BDMPINFO),0);
			if(errval)
			{
				fputs("ERR-3",destination);
				return(0);
			}
			break;
	}
	chksum = 0;
	tmpptr = (uchar *)&bdmpdata;
	for(tvar = 0; tvar < sizeof(BDMPINFO); tvar++)
	{
		fprintf(destination,"%02X",tmpptr[tvar]);
		chksum += tmpptr[tvar];
	}
	fprintf(destination,"%02X",chksum);
	return(0);
}
/******* << END CMD_ROOT.C >> *******/
