#include <stdio.h>
#include <float.h>
#include <ctype.h>
#include <mriext.h>
#include <string.h>
#include <math.h>
#include "serint.h"
#include "buffer.h"
#include "timer.h"
#include "parser.h"
#include "qsm.h"
#include "switches.h"
#include "intface.h"
#include "display.h"
#include "key.h"
#include "front.h"
#include "ecat.h"
#include "tiocntl.h"
#include "eft.h"
#include "pqf.h"
#include "acrms.h"
#include "emc.h"
#include "data.h"
   
void	MOD_DefaultModinfo(char	bayaddr);
void	MOD_ReadModinfo(char	bayaddr);

void	blue_on(void);
void	blue_off(void);

extern P_PARAMT	paraml[3];

const P_ELEMENT	common_table[] =
{
	{ "*IDN?",				star_IDN_rpt	},
	{ "*OPC?",				star_OPC_rpt	},
	{ "*RST",				star_RST_pgm	},
	{ "*TRG %d",			star_TRG_pgm	},
	{ "",0x00}
};

const P_ELEMENT	root_table[] =
{
	{ "ABort",				root_ABT_pgm	},
	{ "COupler %d",		    root_CPL_pgm	},
	{ "COupler?",			root_CPL_rpt	},
	{ "EUt %d",				root_EUT_pgm	},
	{ "EUt?",				root_EUT_rpt	},
	{ "LOADANDPRAY",		load_and_pray	},
	{ "",0x00}
};

const P_ELEMENT	bay_table[] =
{
	{ "CAlibrate %d %d %d",	BAY_CAL_pgm		},
	{ "CAlibrate? %d %d",	BAY_CAL_rpt		},
	{ "DEFault %d",			BAY_DFLT_pgm	},
	{ "DELay? %d %d",			BAY_DLY_rpt		},
	{ "DUmp? %d",				BAY_DUMP_rpt	},
	{ "ID %d %d",				BAY_ID_pgm		},
	{ "ID? %d",					BAY_ID_rpt		},
	{ "MEasure %d %d",		BAY_MEAS_pgm	},
	{ "MEasure? %d",			BAY_MEAS_rpt	},
	{ "NAme %d %s",			BAY_NAME_pgm	},
	{ "NAme? %d",				BAY_NAME_rpt	},
	{ "OPtion %d %d",			BAY_OPTS_pgm	},
	{ "OPtion? %d",			BAY_OPTS_rpt	},
	{ "SErial %d %d",			BAY_SER_pgm		},
	{ "SErial? %d",			BAY_SER_rpt		},
	{ "TYpe %d %d %d",		BAY_TYPE_pgm	},
	{ "TYpe? %d",				BAY_TYPE_rpt	},
	{ "UPdate %d",				BAY_WRTE_pgm	},
	{ "VAlid? %d",				BAY_CHK_rpt		},
	{ "WAveform? %d %d",		BAY_WAV_rpt		},
	{ "",0x00}
};

const	P_ELEMENT	eft_table[] =
{
	{ "BAy %d",				EFT_BAY_pgm		},
	{ "BAy?",				EFT_BAY_rpt		},
	{ "COupling %d",		EFT_CPL_pgm		},
	{ "COupling?",			EFT_CPL_rpt		},
	{ "DUration %d",		EFT_DUR_pgm		},
	{ "DUration?",			EFT_DUR_rpt		},
	{ "FRequency %d",		EFT_FRQ_pgm		},
	{ "FRequency?",		    EFT_FRQ_rpt		},
	{ "MOde %d",			EFT_MDE_pgm		},
	{ "MOde?",				EFT_MDE_rpt		},
	{ "OUtput %d %d",		EFT_OUT_pgm		},
	{ "OUtput?",			EFT_OUT_rpt		},
	{ "PEriod %d",			EFT_PER_pgm		},
	{ "PEriod?",			EFT_PER_rpt		},
	{ "SEt",				EFT_SET_pgm		},
	{ "SEt?",				EFT_SET_rpt		},
	{ "VOltage %d",		    EFT_VLT_pgm		},
	{ "VOltage?",			EFT_VLT_rpt		},
	{ "WAveform %d",		EFT_WAV_pgm		},
	{ "WAveform?",			EFT_WAV_rpt		},
    { "LInecheck %d",       EFT_LCK_pgm     },
	{ "",0x00},
};

const	P_ELEMENT	linesync_table[] =
{
	{ "ANgle %d",			LI_ANG_pgm		},
	{ "ANgle?",				LI_ANG_rpt		},
	{ "MOde %d",			LI_MODE_pgm		},
	{ "MOde?",				LI_MODE_rpt		},
	{ "",0x00}
};

const P_ELEMENT	measure_table[] =
{
	{ "BAy %d",				MEAS_BAY_pgm	},
	{ "BAy?",				MEAS_BAY_rpt	},
	{ "IMon %d",			MEAS_ICH_pgm	},
	{ "IMon?",				MEAS_ICH_rpt	},
	{ "IPeak?",				MEAS_IPK_rpt	},
	{ "VMon %d",			MEAS_VCH_pgm	},
	{ "VMon?",				MEAS_VCH_rpt	},
	{ "VPeak?",				MEAS_VPK_rpt	},
	{ "",0x00}
};

const P_ELEMENT	pqf_table[] =
{
	{ "ERror? %d",							PQF_error_rpt		},
	{ "EXit",								PQF_exit_pgm		},
	{ "IDle %d",							PQF_idle_pgm		},
	{ "LOad",								PQF_load_list		},
	{ "NOminal %d",				  		PQF_nominal_pgm   },
	{ "PHase %d",							PQF_phase_pgm		},
	{ "QUalify ",							PQF_qualify_cmd   },
	{ "REset",								PQF_reset_list		},
	{ "RUn %i",								PQF_run_list		},
	{ "SAve",								PQF_save_list		},
	{ "SET %d %d %d %d %d",				PQF_set_list		},
	{ "SET GOTO %d",						PQF_set_goto		},
	{ "SET TAPC %d %d %d %d %d",		PQF_setc_tap		},
	{ "SET TAPS %d %d %d %d %d",		PQF_sets_tap		},
	{ "SETN %d %d %d %d %d %d",		PQF_setn_list		},
	{ "SETN GOTO %d",						PQF_setn_goto		},
	{ "SETN TAPC %d %d %d %d %d %d",	PQF_setnc_tap		},
	{ "SETN TAPS %d %d %d %d %d %d",	PQF_setns_tap		},
	{ "STAtus?",                 		PQF_status_rep		},
	{ "STOp",								PQF_stop_list		},
	{ "",0x00}
};

const P_ELEMENT	surge_table[] =
{
	{ "CHarge",				SRG_CHG_pgm		},
	{ "CHEAT",  			SRG_CHEAT		},
	{ "COupling %d %d",	    SRG_CPL_pgm		},
	{ "COupling?",			SRG_CPL_rpt		},
	{ "DElay %d",			SRG_DLY_pgm		},
	{ "DElay?",				SRG_DLY_rpt		},
	{ "NEtwork %d",		    SRG_NET_pgm		},
	{ "NEtwork?",			SRG_NET_rpt		},
	{ "OUTput %d",			SRG_OUT_pgm		},
	{ "OUTput?",			SRG_OUT_rpt		},
	{ "VOltage %d",		    SRG_VLT_pgm		},
	{ "VOltage?",			SRG_VLT_rpt		},
	{ "WAveform %d",		SRG_WAV_pgm		},
	{ "WAveform?",			SRG_WAV_rpt		},
    { "LInecheck %d",       SRG_LCK_pgm     },
	{ "",0x00}
};

const P_ELEMENT	system_table[] =
{
	{ "DUmp? %d",			SYS_BDMP_rpt		},
	{ "ILock?",				SYS_ILOK_rpt		},
	{ "IText?",				SYS_ITXT_rpt		},
	{ "RLy %d %d %d",		SYS_RLY_pgm			},
	{ "RLy? %d %d",		    SYS_RLY_rpt			},
	{ "MEM? %d",			SYS_MEM_rpt			},
	{ "MEM %d %d",			SYS_MEM_pgm			},
	{ "JABBERJAW %d",		SYS_JJAW_pgm		},
	{ "LOADANDPRAY",		load_and_pray		},
    { "BAUD %d",            SYS_BAUD_pgm        },
    { "BAUD?",              SYS_BAUD_rpt        },
	{ "",0x00}
};

const P_ELEMENT	ac_table[] =
{
	{ "Status?",			RMS_StatusRep	},
	{ "PLIM %d",			RMS_PLIM_pgm	},
	{ "RMIN %d",			RMS_SMIN_pgm	},
	{ "RMAX %d",			RMS_SMAX_pgm	},
	{ "LIM?",				RMS_LIM_rpt		},
	{ "PIT %d",				RMS_PITR_pgm	},
	{ "",0x00}
};

const P_ELEMENT	emc_table[] =
{
	{ "BYpass %d",			EMC_BYpass_pgm	},
	{ "BYpass?",			EMC_BYpass_rpt	},
	{ "EUt %d",				EMC_EUT_pgm		},
	{ "MOde %d",			EMC_MOde_pgm	},
	{ "PHase %d",			EMC_PHase_pgm	},
	{ "PHase?",				EMC_PHase_rpt	},
	{ "REset",				EMC_REset_pgm	},
	{ "EXit",				EMC_EXit_pgm	},
	{ "",0x00}
};

const P_ELEMENT	data_table[] =
{
	{ "CLamp %d",			DATA_CLAMP_pgm	},
	{ "CLamp?",				DATA_CLAMP_rpt	},
	{ "CMode %d",			DATA_CMODE_pgm	},
	{ "CMode?",				DATA_CMODE_rpt	},
	{ "OUtput %d",			DATA_OUT_pgm	},
	{ "OUtput?",			DATA_OUT_rpt	},
	{ "",0x00}
};

const PE_LIST	xpib_table[] =
{
	{ "*",				common_table	},
	{ "",					root_table	},
	{ ":AC ",			ac_table		},
	{ ":AMP ",			emc_table		},
	{ ":BAY ",			bay_table		},
	{ ":DAta ",			data_table		},
	{ ":EFT ",			eft_table		},
	{ ":LInesync ",	linesync_table	    },
	{ ":MEasure ",		measure_table	},
	{ ":PQF ",			pqf_table		},
	{ ":SRG ",			surge_table		},
	{ ":SYstem ",		system_table	},
	{ "",0x00}
};

const WAVDATA	wavdata[MAX_SRGBOXES] =
{
	/* E501 Module */
	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x01, 0x01, 0x01 }, { 6600, 6600, 4400 }, { 12, 12, 12 }, { 1, 2, 3 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, { 8, 9, 0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, { 0, 0, 0 },    0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E502 Module */
	{ 3, {  { "6 kV, 0.5/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 6600, 0, 0 }, { 18, 0, 0 }, { 0, 0, 0 }, 1000 },
	        { "6 kV, 10/700 Exponential",   0x01, { 0x00, 0x00, 0x00 }, { 6600, 0, 0 }, { 18, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "5 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 5500, 0, 0 }, { 18, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E503 Module */
	{ 2, {  { "100 kHz Ring Wave, 200A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 9, 9, 0 }, { 10, 11, 0 }, 1000 },
	        { "100 kHz Ring Wave, 500A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 9, 9, 0 }, { 12, 13, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E511 Module */
	{ 1, {  { "1.2/50 8/20 Comb",           0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 14, 15, 0 }, 1000 },
	        { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E506 Module */
	{ 4, {  { "800V,  2 us/10 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 880,     0, 0 }, { 16,  0, 0 }, {  0,  0, 0 }, 2125 },
	        { "1.5kV, 2 us/10 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1650,    0, 0 }, { 16,  0, 0 }, {  0,  0, 0 }, 1133 },
            { "2.5 kV, 2 us/10 us, 500A",   0x01, { 0x01, 0x01, 0x00 }, { 2750, 2750, 0 }, { 16, 16, 0 }, { 16, 17, 0 }, 2400 },
            { "5.0 kV, 2 us/10 us, 500A",   0x01, { 0x01, 0x01, 0x00 }, { 5500, 5500, 0 }, { 16, 16, 0 }, { 18, 19, 0 }, 1200 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E505 Module */
	{ 4, {  { "1.5 kV, 10 us/160 us, 200A", 0x01, { 0x00, 0x00, 0x00 }, { 1650,    0, 0 }, { 18,  0, 0 }, {  0,  0, 0 }, 4000 },
	        { "800 V,  10 us/560 us, 100A", 0x01, { 0x00, 0x00, 0x00 }, { 880,     0, 0 }, { 18,  0, 0 }, {  0,  0, 0 }, 7500 },
            { "800 V,  10 us/560 us, 200A", 0x01, { 0x00, 0x00, 0x00 }, { 880,     0, 0 }, { 24,  0, 0 }, {  0,  0, 0 }, 7500 },
            { "2.5 kV, 2 us/10 us,  1kA",   0x01, { 0x01, 0x01, 0x00 }, { 2750, 2750, 0 }, { 24, 24, 0 }, { 20, 21, 0 }, 2400 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

    /* E510 Module */
	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x01, 0x01, 0x01 }, { 10100, 10100, 4400 }, { 18, 18, 12 }, { 22, 23, 24 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x01, 0x01, 0x00 }, { 10100, 10100,    0 }, { 18, 18,  0 }, { 29, 30,  0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x01, { 0x01, 0x01, 0x00 }, { 10100, 10100,    0 }, { 18, 18,  0 }, {  0,  0,  0 },    0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E502H Module */
	{ 3, {  { "7 kV,  0.5/700 Exponential", 0x01, { 0x00, 0x00, 0x00 }, { 7350, 0, 0 }, { 24, 0, 0 }, { 0, 0, 0 }, 1000 },
	        { "7 kV,  10/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 7350, 0, 0 }, { 24, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "6 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 6300, 0, 0 }, { 24, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E504 Module */
	{ 2, {  { "6 kV, 1.2 us/50 us, 500A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 31, 32, 0 }, 1000 },
	        { "6 kV, 1.2 us/50 us, 750A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 33, 34, 0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E513 Module */
 	{ 5, {  { " 0.1 kV / 1 usec @ 3 kV",    0x01, { 0x00, 0x00, 0x00 }, { 3000, 0, 0 }, { 9, 0, 0 }, { 0, 0, 0 }, 1900 },
	        { " 0.5 kV / 1 usec @ 3 kV",    0x01, { 0x00, 0x00, 0x00 }, { 3000, 0, 0 }, { 9, 0, 0 }, { 0, 0, 0 }, 1900 },
            { " 1.0 kV / 1 usec @ 3 kV",    0x01, { 0x00, 0x00, 0x00 }, { 3000, 0, 0 }, { 9, 0, 0 }, { 0, 0, 0 }, 1900 },
            { " 5.0 kV / 1 usec @ 3 kV",    0x01, { 0x00, 0x00, 0x00 }, { 3000, 0, 0 }, { 9, 0, 0 }, { 0, 0, 0 }, 1900 },
            { "10.0 kV / 1 usec @ 3 kV",    0x01, { 0x00, 0x00, 0x00 }, { 3000, 0, 0 }, { 9, 0, 0 }, { 0, 0, 0 }, 1900 } } },
																  
	/* E514 Module */
 	{ 4, {  { "1.5 kV, 10 us/1000 us,   15A",   0x01, { 0x00, 0x00, 0x00 }, { 1650, 0, 0 }, { 20, 0, 0 }, { 0, 0, 0 }, 1000 },
	        { "1.5 kV,  1 kV/1 us ramp, 60A",   0x01, { 0x00, 0x00, 0x00 }, { 1650, 0, 0 }, { 20, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "1.0 kV, 10 us/1000 us,   100A",  0x01, { 0x00, 0x00, 0x00 }, { 1100, 0, 0 }, { 59, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "1.5kV,   1 kV/1 us ramp, 250A",  0x01, { 0x00, 0x00, 0x00 }, { 1650, 0, 0 }, { 59, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E522 Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x00, { 0x00, 0x01, 0x00 }, { 0, 20200, 0 }, { 0, 45, 0 }, { 0, 35, 0 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x00, { 0x00, 0x01, 0x00 }, { 0, 20200, 0 }, { 0, 45, 0 }, { 0, 36, 0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x00, { 0x00, 0x01, 0x00 }, { 0, 20200, 0 }, { 0, 45, 0 }, { 0,  0, 0 },    0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E502K Module */
	{ 3, {  { "6 kV, 1.2/50  Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 6600, 0, 0 }, { 6,  0, 0 }, { 0, 0, 0 }, 1000 },
	        { "6 kV, 10/700 Exponential",   0x01, { 0x00, 0x00, 0x00 }, { 6600, 0, 0 }, { 18, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "5 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 5500, 0, 0 }, { 18, 0, 0 }, { 0, 0, 0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E501A Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x01, 0x01, 0x01 }, { 6600, 6600, 4400 }, { 12, 12, 12 }, { 37, 38, 39 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, { 44, 45,  0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, {  0,  0,  0 },    0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 } } },

	/* E510A Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x01, 0x01, 0x01 }, { 10100, 10100, 4400 }, { 18, 18, 12 }, { 46, 47, 48 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x01, 0x01, 0x00 }, { 10100, 10100,    0 }, { 18, 18,  0 }, { 53, 54,  0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x01, { 0x01, 0x01, 0x00 }, { 10100, 10100, 0 }, { 18, 18, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },     { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },     { 0, 0, 0 }, 0 } } },

	/* E508 Module */
	{ 2, { { "1 kV, 10 us/360 us, 100A",    0x01, { 0x00, 0x00, 0x01 }, { 1100, 0, 1100 }, {  50, 0, 50 }, { 0, 0, 55 }, 1500 },
	        { "1 kV, 10 us/360 us, 25A",    0x01, { 0x00, 0x00, 0x00 }, { 1100, 0,    0 }, { 150, 0,  0 }, { 0, 0,  0 }, 1500 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E502A Module */
	{ 3, {  { "6 kV, 0.5/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 6600, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
	        { "6 kV, 10/700 Exponential",   0x01, { 0x00, 0x00, 0x01 }, { 6600, 0, 4400 }, { 18, 0, 18 }, { 0, 0, 60 }, 1000 },
            { "5 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 5500, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E504A Module */
 	{ 3, {  { "6 kV, 1.2 us/50 us, 500A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 65, 66, 0 }, 1000 },
	        { "6 kV, 1.2 us/50 us, 750A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 67, 68, 0 }, 1000 },
            { "6 kV, 1.2 us/50 us 125A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 69, 70, 0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E509 Module */
 	{ 3, {  { "600 V,  10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 660, 0, 0 }, { 32, 0, 0 }, { 0, 0, 0 }, 10000 },
	        { "1.0 kV, 10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1100,0, 0 }, { 32, 0, 0 }, { 0, 0, 0 }, 6000 },
            { "1.5 kV, 10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1650,0, 0 }, { 32, 0, 0 }, { 0, 0, 0 }, 4000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 } } },

	/* E521 Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x00, { 0x00, 0x01, 0x00 }, { 0, 20200, 0 }, { 0, 45, 0 }, { 0, 71, 0 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x00, { 0x00, 0x01, 0x00 }, { 0, 20200, 0 }, { 0, 45, 0 }, { 0, 72, 0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x00, { 0x00, 0x01, 0x00 }, { 0, 22000, 0 }, { 0, 45, 0 }, { 0,  0, 0 },    0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E502B Module */
	{ 3, {  { "6 kV, 0.5/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 7000, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
	        { "6 kV, 10/700 Exponential",   0x01, { 0x00, 0x00, 0x01 }, { 7000, 0, 4400 }, { 18, 0, 18 }, { 0, 0, 60 }, 1000 },
            { "5 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 5500, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E509A Module */
 	{ 3, {  { "600 V,  10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 600, 0, 0 }, { 40, 0, 0 }, { 0, 0, 0 }, 10000 },
	        { "1.0 kV, 10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1000,0, 0 }, { 40, 0, 0 }, { 0, 0, 0 },  6000 },
            { "1.5 kV, 10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1500,0, 0 }, { 40, 0, 0 }, { 0, 0, 0 },  4000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 } } },

	/* E501B Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x01, 0x01, 0x01 }, { 6600, 6600, 4400 }, { 12, 12, 12 }, { 73, 74, 75 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, { 80, 81,  0 }, 1000 },
            { "1.2/50, 8/20 Comb Auto 2/12 Ohm",0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600,    0 }, { 12, 12,  0 }, {  0,  0,  0 },    0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 } } },

	/* E504B Module */
 	{ 3, {  { "6 kV, 1.2 us/50 us, 500A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 65, 66, 0 }, 1000 },
	        { "6 kV, 1.2 us/50 us, 750A",   0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 67, 68, 0 }, 1000 },
            { "6 kV, 1.2 us/50 us 125A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 12, 12, 0 }, { 69, 70, 0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E506_4W Module */
	{ 4, {  { "800V,  2 us/10 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 800,  0, 0 }, { 16, 0, 0 }, { 0, 0, 0 }, 2125 },
	        { "1.5kV, 2 us/10 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1500, 0, 0 }, { 16, 0, 0 }, { 0, 0, 0 }, 1133 },
            { "2.5 kV, 2 us/10 us, 500A",   0x01, { 0x00, 0x00, 0x00 }, { 2500, 0, 0 }, { 16, 0, 0 }, { 0, 0, 0 }, 2400 },
            { "5.0 kV, 2 us/10 us, 500A",   0x01, { 0x00, 0x00, 0x00 }, { 5000, 0, 0 }, { 16, 0, 0 }, { 0, 0, 0 }, 1200 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E515 Module */
	{ 2, {  { "2.0 kV, 10 us/250 us",   0x01, { 0x00, 0x00, 0x00 }, { 2000, 0, 0 }, {  63, 0, 0 }, { 0, 0, 0 }, 1000 },
	        { "4.0 kV, 10 us/250 us",   0x01, { 0x00, 0x00, 0x00 }, { 4000, 0, 0 }, { 126, 0, 0 }, { 0, 0, 0 }, 1000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E505A Module */
	{ 3, {  { "1.5 kV, 10 us/160 us, 200A", 0x01, { 0x00, 0x00, 0x00 }, { 1500,    0, 0 }, { 20,  0, 0 }, {  0,  0, 0 }, 4000 },
	        { "800 V,  10 us/560 us, 100A", 0x01, { 0x00, 0x00, 0x00 }, { 800,     0, 0 }, { 20,  0, 0 }, {  0,  0, 0 }, 7500 },
            { "2.5 kV, 2 us/10 us,  1kA",   0x01, { 0x01, 0x01, 0x00 }, { 2500, 2500, 0 }, { 20, 20, 0 }, { 20, 21, 0 }, 2400 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E518 Module */
 	{ 3, {  { "600 V,  10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 600,  0, 0 }, { 50, 0, 0 }, { 0, 0, 0 }, 10000 },
	        { "1.0 kV, 10 us/1000 us, 100A",    0x01, { 0x00, 0x00, 0x00 }, { 1000, 0, 0 }, { 50, 0, 0 }, { 0, 0, 0 },  6000 },
            { "2.0 kV, 10 us/1000 us, 200A",    0x01, { 0x00, 0x00, 0x00 }, { 2000, 0, 0 }, { 90, 0, 0 }, { 0, 0, 0 },  3000 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E505B Module */
	{ 3, {  { "2.5 kV, 2 us/10 us,  1kA",   0x00, { 0x01, 0x00, 0x00 }, { 2500, 0, 0 }, { 20, 0, 0 }, { 20, 0, 0 }, 2400 },
        	{ "1.5 kV, 10 us/160 us, 200A", 0x01, { 0x00, 0x00, 0x00 }, { 1500, 0, 0 }, { 20, 0, 0 }, {  0, 0, 0 }, 4000 },
	        { "800 V,  10 us/560 us, 100A", 0x01, { 0x00, 0x00, 0x00 }, {  800, 0, 0 }, { 20, 0, 0 }, {  0, 0, 0 }, 7500 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

    /* E502C Module */
    { 3, {  { "6 kV, 0.5/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 7000, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
            { "6 kV, 10/700 Exponential",   0x01, { 0x00, 0x00, 0x00 }, { 7000, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
            { "5 kV, 100/700 Exponential",  0x01, { 0x00, 0x00, 0x00 }, { 5500, 0,    0 }, { 18, 0,  0 }, { 0, 0,  0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E503A Module */
	{ 2, {  { "100 kHz Ring Wave, 200A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 3, 3, 0 }, { 82, 83, 0 }, 1000 },
	        { "100 kHz Ring Wave, 500A",    0x01, { 0x01, 0x01, 0x00 }, { 6600, 6600, 0 }, { 3, 3, 0 }, { 84, 85, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 0 } } },

	/* E507 Module */
 	{ 3, {  { "1.2/50, 8/20 Comb 2 Ohm",        0x01, { 0x00, 0x00, 0x00 }, { 440, 0, 0 }, { 3, 0, 0 }, { 0, 0, 0 }, 1000 },
	        { "1.2/50, 8/20 Comb 12 Ohm",       0x01, { 0x00, 0x00, 0x00 }, { 440, 0, 0 }, { 3, 0, 0 }, { 0, 0, 0 }, 1000 },
            { "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },  // no Auto 2/12, it's only req'd for CDN
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 },
        	{ "", 0x00, { 0x00, 0x00, 0x00 }, { 0, 0, 0 }, { 0, 0, 0 },   { 0, 0, 0 }, 0 } } },
};

/* The cplAdjust table consists of records whose 7 members are calibration
   factors for the different coupling modes.  Record members are defined as
   follows, where index is the array index into each record (note that HV front
   panel output values are always 100):
                                        |
   For LINE (line coupler outp) records | For DATA (data coupler outp) records
   ------------------------------------ | --------------------------------------
      Record   Coupling                 |    Record   Program
      Index      Mode                   |    Index    Voltage
      ------   --------                 |    ------   -------
        0      4 Lines to PE            |      0      less than or equal to 625
        1      3 Lines to PE            |      1      626 to 850
        2      2 Lines to PE            |      2      851 to 1200
        3      1 Line to PE             |      3      1201 to 2000
        4      3 Lines to Line          |      4      greater than 2000
        5      2 Lines to Line          |
        6      1 Line to Line           |
*/
const uchar	cplAdjust[][MAX_LMODES] =
{
	  0,   0,   0,   0,   0,   0,   0,		/* 000, Used for ALL UNUSED modes*/
	103, 101, 100, 100,  99,  99,  99,		/* 001, E501,  WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 002, E501,  WAV-0, HV			*/
	102, 101, 100, 100,  96,  96,  96,		/* 003, E501,  WAV-0, DATA (1)	*/
	102, 101, 100, 100,  95,  95,  96,		/* 004, E501,  WAV-0, DATA (2)	*/
	102, 101, 100, 100,  96,  96,  96,		/* 005, E501,  WAV-0, DATA (3)	*/
	103, 102, 101, 100,  96,  96,  97,		/* 006, E501,  WAV-0, DATA (4)	*/
	106, 104, 102, 100,  99,  99,  98,		/* 007, E501,  WAV-0, DATA (5)	*/
	109, 106, 103, 100,  99,  99,  99,		/* 008, E501,  WAV-1, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 009, E501,  WAV-1, HV			*/
	106, 104, 102, 100,  98,  98,  98,		/* 010, E503,  WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 011, E503,  WAV-0, HV			*/
	103, 102, 101, 100,  97,  97,  97,		/* 012, E503,  WAV-1, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 013, E503,  WAV-1, HV			*/
	102, 101, 100, 100, 100, 100, 100,		/* 014, E511,  WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 015, E511,  WAV-0, HV			*/
	100, 100, 100, 100, 100, 100, 100,		/* 016, E506,  WAV-2, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 017, E506,  WAV-2, HV			*/
	100, 100, 100, 100, 100, 100, 100,		/* 018, E506,  WAV-3, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 019, E506,  WAV-3, HV			*/
	100, 100, 100, 100, 100, 100, 100,		/* 020, E505/E505A/E505B, WAV-3, LINE	*/
	100, 100, 100, 100, 100, 100, 100,		/* 021, E505/E505A/E505B, WAV-3, HV		*/
	104, 101, 100, 100, 100, 101, 100,		/* 022, E510,  WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 023, E510,  WAV-0, HV			*/
	101, 100,  99, 100,  96,  96,  97,		/* 024, E510,  WAV-0, DATA (1)	*/
	101, 100,  99, 100,  96,  96,  96,		/* 025, E510,  WAV-0, DATA (2)	*/
	101, 100,  99, 100,  96,  96,  96,		/* 026, E510,  WAV-0, DATA (3)	*/
	102, 101, 100, 100,  96,  96,  97,		/* 027, E510,  WAV-0, DATA (4)	*/
	104, 102, 101, 100,  99,  98,  98,		/* 028, E510,  WAV-0, DATA (5)	*/
	108, 106, 103, 100, 100, 100, 100,		/* 029, E510,  WAV-1, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 030, E510,  WAV-1, HV			*/
	115, 110, 105, 100, 103, 103,  98,		/* 031, E504,  WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 032, E504,  WAV-0, HV			*/
	112, 106, 104, 100, 104, 103,  98,		/* 033, E504,  WAV-1, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 034, E504,  WAV-1, HV			*/
	104, 101, 100, 100, 100, 101, 100,		/* 035, E522,  WAV-0, HV			*/
	108, 106, 103, 100, 100, 100, 100,		/* 036, E522,  WAV-1, HV			*/
	105, 103, 101, 100, 102, 101, 101,		/* 037, E501A, WAV-0, LINE	      */
	100, 100, 100, 100, 100, 100, 100,		/* 038, E501A, WAV-0, HV	      */
	 99,  98,  98, 100,  95,  94,  96,		/* 039, E501A, WAV-0, DATA (1)   */
	 99,  98,  97, 100,  95,  95,  96,		/* 040, E501A, WAV-0, DATA (2)   */
	100,  99,  98, 100,  95,  95,  96,		/* 041, E501A, WAV-0, DATA (3)   */
	102, 101,  99, 100,  97,  96,  97,		/* 042, E501A, WAV-0, DATA (4)   */
	105, 103, 101, 100,  99,  99,  98,		/* 043, E501A, WAV-0, DATA (5)   */
	108, 105, 102, 100, 100,  99,  98,		/* 044, E501A, WAV-1, LINE	      */
	100, 100, 100, 100, 100, 100, 100,		/* 045, E501A, WAV-1, HV	      */
	104, 102, 101, 100, 100, 100, 100,		/* 046, E510A, WAV-0, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 047, E510A, WAV-0, HV			*/
	103, 101, 100, 100,  95,  95,  96,		/* 048, E510A, WAV-0, DATA (1)	*/
	102, 101,  99, 100,  95,  94,  95,		/* 049, E510A, WAV-0, DATA (2)	*/
	103, 102, 100, 100,  96,  96,  96,		/* 050, E510A, WAV-0, DATA (3)	*/
	104, 103, 101, 100,  97,  97,  97,		/* 051, E510A, WAV-0, DATA (4)	*/
	107, 105, 102, 100, 100,  99,  98,		/* 052, E510A, WAV-0, DATA (5)	*/
	109, 105, 103, 100, 100, 100, 100,		/* 053, E510A, WAV-1, LINE			*/
	100, 100, 100, 100, 100, 100, 100,		/* 054, E510A, WAV-1, HV			*/
	100, 100, 100, 100, 100, 100, 100,		/* 055, E508,  WAV-0, DATA (1)	*/
	100, 100, 100, 100, 100, 100, 100,		/* 056, E508,  WAV-0, DATA (2)	*/
	100, 100, 100, 100, 100, 100, 100,		/* 057, E508,  WAV-0, DATA (3)	*/
	100, 100, 100, 100, 100, 100, 100,		/* 058, E508,  WAV-0, DATA (4)	*/
	100, 100, 100, 100, 100, 100, 100,		/* 059, E508,  WAV-0, DATA (5)	*/
	115, 110, 105, 100,  95,  95,  94,		/* 060, E502A/E502B, WAV-0, DATA (1)*/
	116, 111, 105, 100,  95,  95,  94,		/* 061, E502A/E502B, WAV-0, DATA (2)*/
	120, 114, 108, 100,  96,  95,  95,		/* 062, E502A/E502B, WAV-0, DATA (3)*/
	121, 114, 108, 100,  96,  95,  94,		/* 063, E502A/E502B, WAV-0, DATA (4)*/
	123, 116, 108, 100,  97,  96,  95,		/* 064, E502A/E502B, WAV-0, DATA (5)*/
	115, 110, 105, 100, 103, 103,  98,		/* 065, E504A/E504B, WAV-0, LINE	*/
	100, 100, 100, 100, 100, 100, 100,		/* 066, E504A/E504B, WAV-0, HV  	*/
	112, 106, 104, 100, 104, 103,  98,		/* 067, E504A/E504B, WAV-1, LINE	*/
	100, 100, 100, 100, 100, 100, 100,		/* 068, E504A/E504B, WAV-1, HV  	*/
	100, 100, 100, 100, 100, 100, 100,		/* 069, E504A/E504B, WAV-2, LINE	*/
	100, 100, 100, 100, 100, 100, 100,		/* 070, E504A/E504B, WAV-2, HV  	*/
	100, 100, 100, 100, 100, 100, 100,		/* 071, E521,  WAV-0, HV			*/
	100, 100, 100, 100, 100, 100, 100,		/* 072, E521,  WAV-1, HV			*/
	 98, 100, 100, 100,  96, 100, 100,		/* 073, E501B, WAV-0, LINE	        */
	100, 100, 100, 100, 100, 100, 100,		/* 074, E501B, WAV-0, HV	        */
	100, 100, 100, 100, 100, 100, 100,		/* 075, E501B, WAV-0, DATA (1)      */
	100, 100, 100, 100, 100, 100, 100,		/* 076, E501B, WAV-0, DATA (2)      */
	100, 100, 100, 100, 100, 100, 100,		/* 077, E501B, WAV-0, DATA (3)      */
	100, 100, 100, 100, 100, 100, 100,		/* 078, E501B, WAV-0, DATA (4)      */
	100, 100, 100, 100, 100, 100, 100,		/* 079, E501B, WAV-0, DATA (5)      */
	107, 105, 103, 100, 100, 100, 100,		/* 080, E501B, WAV-1, LINE	        */
	100, 100, 100, 100, 100, 100, 100,		/* 081, E501B, WAV-1, HV	        */
    104, 103, 101, 100,  98,  98,  98,		/* 082, E503A, WAV-0, LINE			*/
    100, 100, 100, 100, 100, 100, 100,		/* 083, E503A, WAV-0, HV			*/
    100, 100, 100, 100, 100, 100, 100,		/* 084, E503A, WAV-1, LINE			*/
    100, 100, 100, 100, 100, 100, 100,		/* 085, E503A, WAV-1, HV			*/
};

MODDATA	moddata[MAX_MODULES];
SRGDATA	srgdata;
CPLDATA	cpldata;
EFTDATA	eftdata;
DISDATA	display;
WARM_UP	warmup;
PKDATA	peakdata;
EHVDATA	ehvdata;
ACDATA	acdata;
EMCDATA	emcdata;

FILE	 	*destination;
uchar	ecatmode;
uchar	saveft;
uchar	calibration;
uchar	front_interlock;
uchar	back_from_local;
uint	 	ghostrelay;
static 	char PIT_runner_channel;

extern uint	burst_delay_ch;

volatile CHGDATA	chgdata;
volatile uint		ilockstate;
volatile ushort	ldo_state;

extern uchar	contrast;
extern volatile ulong	vectors[];
extern volatile ushort	DUART_OPR_SET;
extern volatile ushort	DUART_OPR_CLR;
extern volatile ushort	DISABLE_DUART_LOC;
extern volatile ushort	PEAK_LD5;
extern volatile ushort	PEAK_RELAYS;
extern volatile ushort	RAMBAR;
extern volatile ushort	RAMMCR;
extern void interrupt	spurious(void);
extern int	PQF_running;
extern uchar	menukeyenable;

/* PQF DEMO BEG */
PQFDATA	pqfdata;
volatile uchar	PQF_softstartflag;
volatile uchar	EP3_softstartflag;
volatile uint		EP3_RD22StartFlag;
volatile uint		PQF_softstartchannel;
volatile uint		EP3_softstartchannel;
volatile uint		EP3_RD22StartChannel;
void		PQF_SoftStartTrap(void);
void		EP3_SoftStartTrap(void);
void		EP3_RD22StartTrap(void);
void		do_PQF_main(void (*p)());
void		PQF_init(uint);
void		show_state_pqfdemo(void);
uchar	demo_funcs(char);
void		raw_relay(uint,uint);
void		PQF_exit();
/**********************************
void		ilock_opened();
void		ilock_closed();
**********************************/
void		PIT_runner2(void);
void		PIT_runner_kill(void);
void	   PIT_runner_init(void);
void	   PIT_runner(void);

extern uint	pitr_cnt;	  /* time for periodic interval to call AD_TRAP */
extern int		new_eut;
/* PQF DEMO END */

void	EMC_EOPxSoftStartTrap(void);

void	DUART_disable(void)
{
	short	i;
	DISABLE_DUART_LOC = i;
}
void	DUART_enable(void)
{
	short	i;
	i = DISABLE_DUART_LOC;
}

char	ECAT_WriteRelays(
uchar	bay,
uint		relaydata)
{
	if(bay > 15)
		return(-1);

	ECAT_WriteRelay0(bay,relaydata);
	ECAT_WriteRelay1(bay,(relaydata >> 16));
	ECAT_TimerSequencer(bay,(relaydata >> 24));
	ECAT_TimerSequencer(bay,(relaydata >> 24));
	return(0);
}

void	ResetHardware(void)
{
	int	tvar;

	/* Shut down appropriate DACS */
	DAC_set12(0);
	DAC_set(1,0);
	DAC_set(2,0);

	/* Shut down all TPU channels */
	for(tvar = 0; tvar < 4; tvar++)
	{
		TPU_read(tvar*2);
		TPU_write(tvar*2+1,0);
	}

	/* Shut off ALL relays on all interface boards */
	for(tvar = 0; tvar < 16; tvar++)
	{
		moddata[tvar].relay = 0x00000000;
		ECAT_WriteRelay0(tvar,0x0000);
		ECAT_WriteRelay1(tvar,0x0000);
		ECAT_TimerSequencer(tvar,0x0000);
		ECAT_TimerSequencer(tvar,0x0000);
		TIO_TOMap(tvar,TO0_TOLCL0);
		TPU_write(1,1);		/* Set it High	*/
		TPU_write(1,0);		/* Set it Low	*/
		TPU_write(1,1);		/* Set it High	*/
		TIO_TOMap(tvar,TO0_DISABLE);
	}
	TIO_Reset();

/*
 *	Modification on 10/07/94 by James Murphy (JMM)
 *		The DEFAULT value for the peak relays was changed from 0x6200
 *		to 0x0200.  This change keeps the VNOT_50 and INOT_50 relays
 *		from turning ON.  This 'solves' the floating input problem.
 *		A related change was made in the CMD_MEAS.C module 
 */
	PEAK_RELAYS = 0x0200;					/* Set up Peak board		*/
	ldo_state	= 0x0101;
	PEAK_LD5		= ldo_state;				/* Disable HV supply		*/
}

void	ECAT_ModeChange(
uchar	mode)
{
	P_PARAMT tparam;

	/* If from exiting PQF mode. */
	if(ecatmode == PQF_MODE && mode != PQF_MODE)
	{
		PQF_exit();
		PIT_runner_init();
	}

	/* Enable line sync for the active source coupler (except for PQF mode). */
	if((cpldata.srcaddr != GHOST_BOX) && (mode != PQF_MODE))
		TIO_TISelect(cpldata.srcaddr,TI3_SELECT,ENABLE);

	switch(mode)
	{
		case SRG_MODE:
			if(ecatmode != SRG_MODE)
			{
				ecatmode = SRG_MODE;
				if(SRG_VerifyCoupling(cpldata.cplhigh,cpldata.cpllow))
				{
					cpldata.cplhigh = CPL_L1BIT;
					cpldata.cpllow  = CPL_L2BIT;
				}
				if(moddata[(char)(cpldata.srcaddr)].modinfo.typePrimary & TYPE_PQF)
				{
					PQF_ResetModules(1);
					CPL_ResetModules(0);
				}
				PEAK_ResetModules(0);
				SRG_ResetModules(0);
			}

         /* Shut off coupling mode relays (will get turned on as required during test)
            - don't do for E505B combined Surge/Coupler module (it doesn't use coupler relay drivers at all) */
  			if ( (cpldata.srcaddr != GHOST_BOX) && ( moddata[(char)(cpldata.srcaddr)].modinfo.id != E505B_BOX ) )
			{
                *cpldata.srcrelay &= ~CPL_MODEMASK;
				ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
				ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
				ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
			}

			/* If the AC Option isn't activated, do that now.  This sets up the
				coupler(s) to do RMS measurements.  Measurements actually begin
				when the AC is turned ON using the blue EUT button (if it is
				already ON, then measurements start next time through main()).
				FD 07Sep95 */
			if ( !acdata.measure_on )
			{
				RMS_ResetModules();
			}

			/* If ArbWare exits improperly, entering SurgeWare reenables keybd. */
			menukeyenable = 1;

			break;

		case EFT_MODE:
			if(ecatmode != EFT_MODE)
			{
				ecatmode = EFT_MODE;
				PEAK_ResetModules(0);
				EFT_ResetModules(0);
			}

            /* Shut off coupling mode relays (should remain OFF for EFT) */
			if(cpldata.srcaddr != GHOST_BOX)
			{
				*cpldata.srcrelay &= ~CPL_MODEMASK;
				ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
				ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
				ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
			}

			/* If the AC Option isn't activated, do that now.  This sets up the
				coupler(s) to do RMS measurements.  Measurements actually begin
				when the AC is turned ON using the blue EUT button (if it is
				already ON, then measurements start next time through main()).
				FD 07Sep95 */
			if ( !acdata.measure_on )
			{
				RMS_ResetModules();
			}

			/* If ArbWare exits improperly, entering BurstWare reenables keybd. */
			menukeyenable = 1;

			break;

		case PQF_MODE:
			if(ecatmode != PQF_MODE)
			{
				ecatmode = PQF_MODE;
				PIT_runner_kill();
				if(emcdata.EOPxFlag)
				{
					emcdata.EOPxRelay &= ~(RLY_RD22);
					ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
				}

            /* Turn on coupling mode relays */
				if(cpldata.srcaddr != GHOST_BOX)
				{
   				*cpldata.srcrelay |= CPL_PQFMODE;
					ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
					ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
					ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
				}

				if(cpldata.srcaddr != GHOST_BOX)
					TIO_TISelect(cpldata.srcaddr,TI3_SELECT,DISABLE);

				SRG_ResetModules(0);
				EFT_ResetModules(0);
				PEAK_ResetModules(0);

				/* Deactivate the AC option (only used for Surge and EFT).
					FD 07Sep95 */
				RMS_Stop();

				if(!emcdata.modeFlag)
					CPL_ResetModules(0);
				acdata.acbox = GHOST_BOX;
				PQF_init(pqfdata.pqfbox);
			}

			/* If ArbWare exits improperly, entering PQFWare reenables keybd. */
			menukeyenable = 1;

			break;

		case GHOST_MODE:
			ecatmode = GHOST_MODE;
			SRG_ResetModules(0);
			EFT_ResetModules(0);
			PEAK_ResetModules(0);
			/*****
			Do not RESET - Testing fix, 08-01-95, JMM
			if(!emcdata.modeFlag)
				PQF_ResetModules(1);
			*****/
			break;

		default:
			break;
	}

	/* Set peak relay states such that the peak caps are grounded (the peak
		detector is disabled) and the 50 Ohm terminating resistors are out
		of the VI monitor circuit (no 50 Ohm termination).  The only
		situation which requires the 50 Ohm terminators is when the
		measurement board is changed, in order to prevent a false extremely
		high output on the VI monitor BNC outputs.  FD 12Sep95 */
	PEAK_RELAYS = 0x6200;

}

const char	*strstr(
const char	*s1,
const char	*s2)
{
	const char	*cp;
	const char	*cps1;
	const char	*cps2;

	if(!s1 || !s2)
		return(NULL);

	cp = s1;
	while(*cp)
	{
		while((*cp) && (*cp != *s2))
			cp++;
		if(*cp == *s2)
		{
			cps1 = cp;
			cps2 = s2;
			while((*cps1) && (*cps2) && (*cps1 == *cps2))
			{
				cps1++; cps2++;
			}
			if(*cps2 == 0)
				return(cp);
		}
		else
			return(NULL);
		cp++;
	}
	return(NULL);
}

void	main(void)
{
	char	c = ' ';
	char	sbuf[80];
	char	*c_ptr;
	int	error;
	int	tvar;

	INIT_CCS();
	vectors[24] = (long)spurious;
	saveft	= 0;
	sbuf[0]	= 0;
	destination  = stdout;
	ecatmode     = GHOST_MODE;
	warmup.state = 0;
	warmup.time  = 0;
    warmup.started = FALSE;
	ilockstate   = 0x007C;
	warmup.powerup  = 1;
	front_interlock = 0;
	calibration 	 = 0;
	back_from_local = 0;
	menukeyenable	 = 1;
	cpldata.eutblue = 0;
	cpldata.eutswitch	 = !(read_pirq() & 0x20);
	cpldata.eutrequest = 0;
    RAMBAR = 0x0C00;
    RAMMCR = 0;
	PQF_softstartflag = TRUE;
	EP3_softstartflag = TRUE;
	EP3_RD22StartFlag = TRUE;
	pitr_cnt = 0x02;					/* PQF stuff (AD_TRAP period = 244 us) */

	switch((read_sw3()>>1) & 3)
	{
		case 0:
			SCI_init(1200,1);
			break;
		case 1:
			SCI_init(2400,1);
			break;
		case 2:
			SCI_init(9600,1);
			break;
		case 3:
			SCI_init(19200,1);
			break;
	}

	TPU_init();												/* Set up timer functions	*/
	INT_disable();											/* Enable TPU interrupts	*/

	QSM_init();												/* Set up A/Ds and D/As		*/
	KEY_reset();											/* Set up the keyboard		*/

	INT_enable();											/* Enable ALL interrupts	*/
	TPU_delay(150);
	DUART_enable();										/* Enable the PEAK board	*/

	ResetHardware();										/* RESET ALL Modules			*/
	fopen("DISPLAY","w");
	setbuf(DISPLAY,NULL);

	PIT_runner_init();

    warmup.state = FALSE;
	contrast = 30;											/* Setup display screen		*/
	DAC_set(DISPLAY_DAC,contrast);
	display.current = DISPLAY_LOCAL;
	display.request = DISPLAY_LOCAL;
	display.previous= DISPLAY_NOTHING;
	init_front(display.current);

	while(!warmup.state)
		;
	ResetHardware();										/* RESET ALL Modules			*/

	DUART_OPR_CLR = 0x00C1;								/* RESET the backplane		*/
	DUART_OPR_SET = 0x00E1;								/* Operate the backplane	*/

	SRG_InitSurgeDetected();							/* Reset flag */
	peakdata.flag = FALSE;								/* Reset flag */
	for(tvar = 0; tvar < MAX_MODULES; tvar++)
		MOD_ReadModinfo(tvar);

	RMS_Init();

	SRG_FindModules();
	EHV_FindModules();
	CPL_FindModules();
	EFT_FindModules();
	PQF_FindModules();
	EMC_FindModules();

	CPL_ResetModules(1);
	SRG_ResetModules(1);
	EHV_ResetModules(1);
	EFT_ResetModules(1);
	PQF_ResetModules(1);
	EMC_ResetModules(1);
	PEAK_ResetModules(1);
	RMS_ResetModules();

	PQF_running = FALSE;

    while(1)
	{
        if((ecatmode == PQF_MODE) && (pqfdata.pqfbox != GHOST_BOX)) 
			do_PQF_main(PIT_runner2);

		if(acdata.acbox != GHOST_BOX)
			RMS_Main();

		if(front_interlock)
		{
 			if(display.current < DISPLAY_COVER_INT)
			{
				display.previous = display.current;
				if(ecatmode == PQF_MODE)
					raw_stop();
				/*ilock_opened();*/
			}
			display.request = front_interlock;
		}
		else
			if(display.current >= DISPLAY_COVER_INT)
			{
				display.request = display.previous;
				/*ilock_closed();*/
			}

		if(display.current != display.request)
			init_front(display.current = display.request);
		if(KEY_hit())
		{
			c = KEY_getchar();
			if(ecatmode == PQF_MODE && pqfdata.pqfbox != GHOST_BOX)
				demo_funcs(c);
			else
				parse_front(c);
		}
		while(fcavail(stdin))
		{
			c = toupper(getchar());
            if(c == '\r')
				c = '\n';
			c_ptr = sbuf + strlen(sbuf);
			if(c == 0x08)  /* backspace */
			{
				c_ptr--;
				*(c_ptr) = 0;
				_OUTCHR(' ');
				_OUTCHR(0x08);
			}
			else
				*(c_ptr++) = c;
			*c_ptr = 0x00;
			if(c == 05)  /* ENQ */
			{
				destination = stdout;
				fputc('[',destination);
				if(ecatmode == PQF_MODE)
				  PQF_status_rep(paraml);
				else
				  RMS_StatusRep(paraml);
				fputc(']',destination);
				c_ptr = sbuf;
				*c_ptr=0;
			}
			if(c == 04)  /* ETX */
			{
				destination = stdout;
				fputc('[',destination);
				PQF_stop_list(paraml);
				fputc(']',destination);
				c_ptr = sbuf;
				*c_ptr = 0;
			}
			else
				if(c == 24)		/* CAN (cancel) */
				{
					sbuf[0] = '\0';
					c_ptr = sbuf;
					destination = stdout;
					fputc('\n',destination);
					fputc('[',destination);
					fputc(']',destination);
				}
				else
					if(c == '\n')
					{
						if(strlen(sbuf) > 3)
						{
							destination = stdout;
							fputc('\n',destination);
							fputc('[',destination);
							tvar = 1;							/* Go to REMOTE Mode */
							if ( strstr( sbuf,"SRG:" ) )
							{
								if ( ( ecatmode != SRG_MODE ) && ( srgdata.srgflag ) )
								{
									ECAT_ModeChange(SRG_MODE);
								}
							}
							else if ( strstr( sbuf,"EFT:") )
							{
								if ( ( ecatmode != EFT_MODE ) && ( eftdata.eftflag ) )
								{
									ECAT_ModeChange(EFT_MODE);
								}
							}
							else if ( strstr( sbuf,"PQF:" ) )
							{
								if ( ( ecatmode != PQF_MODE ) && ( pqfdata.pqfflag ) )
								{
									ECAT_ModeChange(PQF_MODE);
								}
							}
							else if (strstr(sbuf,"AMP:"))
							{
								if ( emcdata.EP7xFlag )
								{
									tvar = 0;			/* Do NOT go to REMOTE */
									menukeyenable = 0;
								}
							}
							error = parse(sbuf,xpib_table);
							if(error)
								report_errors(destination,error);
							if(tvar)
							{
								display.request = DISPLAY_REMOTE;
								if(display.current != DISPLAY_REMOTE)
									back_from_local = TRUE;
							}
							fputc(']',destination);
							fputc('\n',destination);
						}
						sbuf[0] = '\0';
						c_ptr = sbuf;
					}
		}
	}
}

void	EMC_EOPxSoftStartTrap(void)
{
	/* Turn ON softstart relay */
	if(emcdata.EOPxAddr < MAX_MODULES)
	{
		emcdata.EOPxRelay |= RLY_RD23;
		ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
	}
	emcdata.EOPxSoftStart = TRUE;
}

void	EP3_RD22StartTrap(void)
{
	if(pqfdata.ep3flag)
	{
		*pqfdata.ep3relay &= ~(RLY_RD22);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
	EP3_RD22StartFlag = TRUE;
	EP3_RD22StartChannel = 0;
}

void	EP3_SoftStartTrap(void)
{
	if(pqfdata.ep3flag)
	{
		switch(pqfdata.ep3phase)
		{
			case 0: /* L1 */
				*pqfdata.ep3relay |= RLY_RD23;
				break;
			case 1: /* L2 */
				*pqfdata.ep3relay |= RLY_RD24;
				break;
			case 2: /* L3 */
				*pqfdata.ep3relay |= RLY_RD25;
				break;
		}
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
		ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
		EP3_RD22StartFlag = FALSE;
		EP3_RD22StartChannel = TPU_delay_int(209643,EP3_RD22StartTrap);
	}
	EP3_softstartflag = TRUE;
	EP3_softstartchannel = 0;
}

void	PQF_SoftStartTrap(void)
{
	/* The PQF Soft Start Relay (RD01) must be turned on 4 seconds after EUT */
	if(cpldata.eutblue == ON)
	{
		if(ecatmode == PQF_MODE)
			raw_relay(1,1);
		else
			if(emcdata.modeFlag && emcdata.EP6xFlag && emcdata.mode != EMC_HARMONICS)
			{
				emcdata.EP6xRelay |= RLY_RD01;
				ECAT_WriteRelay0(emcdata.EP6xAddr,emcdata.EP6xRelay);
			}
		PQF_softstartflag = TRUE;
	}
	PQF_softstartchannel = 0;
}

void	new_eut_trap()
{
	new_eut = 0;
}

void PIT_runner2(void)
{
	ilockstate = ilock_stat() | ((~(cpldata.eutblue) & 0x0001) & cpldata.eutrequest);
	if((ilockstate & 0x7C) != 0x7C)
	{
		if(!(ilockstate & 0x40))
			front_interlock = DISPLAY_COVER_INT;
	}
	else
		front_interlock = 0;

	if(((!(read_pirq() & 0x20)) != cpldata.eutswitch) && ((ilockstate & 0x7C)==0x7C))
	{
		cpldata.eutswitch = !(read_pirq() & 0x20);
		if(cpldata.eutrequest && cpldata.eutswitch)
			blue_on();
		else		/* Switch went from ON to OFF */
			blue_off();
	}
	if(!warmup.state)
	{
		if(warmup.time >= 40)				/* 8 TICK/SEC * 5 SECS */
		{
			warmup.state = TRUE;
			display.current = DISPLAY_NOTHING;
		}
		else
			if((warmup.time % 8) == 0)
				FP_UpdateWarmup(warmup.time);
		warmup.time++;
	}
	if(chgdata.time >= 0)
	{
		chgdata.time--;
		if((chgdata.time % 8) == 0)
			SRG_UpdateCharge(chgdata.time);
	}
	if(acdata.acbox != GHOST_BOX)
	{
		if(acdata.time > 0)
			acdata.time--;
		acdata.checker++;
	}
}

void	PIT_runner_init(void)
{
	PIT_runner2();
	PIT_runner_channel = TPU_repeat_delayint(0,PIT_RUNNER_PERIOD,PIT_runner);
}

void	PIT_runner(void)
{
	PIT_runner2();
	TPU_repeat_delayint(PIT_runner_channel,PIT_RUNNER_PERIOD,PIT_runner);
}

void	PIT_runner_kill(void)
{
	if(PIT_runner_channel)
	{
		TPU_kill(PIT_runner_channel);
		PIT_runner_channel = 0;
	}
}

void	MOD_DefaultCALInfo(
char	bayaddr)
{
	char	tvar;

	if(bayaddr >= MAX_MODULES)
		return;

	moddata[bayaddr].modinfo.calinfo.chksum = 0x0000;
	moddata[bayaddr].calsumflag = FALSE;
	for(tvar = 0; tvar < CALINFOSIZE; tvar++)
	{
		moddata[bayaddr].modinfo.calinfo.data[tvar] = 0x0000;
		moddata[bayaddr].modinfo.calinfo.chksum += 0x0000;
	}
}

void	MOD_ReadModinfo(
char	bayaddr)
{
	ushort	check;
	char		tvar;

	if(bayaddr >= MAX_MODULES)
		return;

 	moddata[bayaddr].chksumflag = TRUE;			/* for any ERRORS found below	*/
	moddata[bayaddr].calsumflag = TRUE;

	if(ECAT_9346Read(bayaddr,(void *)&moddata[bayaddr].modinfo,sizeof(MODINFO)))
		MOD_DefaultModinfo(bayaddr);
	else
	{
		check = 0x0000;
		if(moddata[tvar].modinfo.viMonitors)
			peakdata.flag = TRUE;
		for(tvar = 0; tvar < CALINFOSIZE; tvar++)
			check += moddata[bayaddr].modinfo.calinfo.data[tvar];
		if(moddata[bayaddr].modinfo.calinfo.chksum != check)
			MOD_DefaultCALInfo(bayaddr);
	}
}

void	MOD_DefaultModinfo(
char	bayaddr)
{
	char	tvar;
	MODDATA	*modptr;

	if(bayaddr >= MAX_MODULES)
		return;

	modptr = &moddata[bayaddr];
	modptr->chksumflag = FALSE;

	modptr->modinfo.viMonitors		= 0x00;
	modptr->modinfo.serialNumber	= 0x00;
	modptr->modinfo.options			= 0x00000000;
	modptr->modinfo.id				= GHOST_BOX;
	modptr->modinfo.typePrimary	= TYPE_GHOST;
	modptr->modinfo.typeSecondary	= TYPE_GHOST;
	modptr->modinfo.calinfo.chksum= 0x0000;
	strcpy(modptr->modinfo.name,"E000 ");
	for(tvar = 0; tvar < CALINFOSIZE; tvar++)
	{
		modptr->modinfo.calinfo.data[tvar] = 0x0000;
		modptr->modinfo.calinfo.chksum += 0x0000;
	}
}

void	CPL_FindModules(void)
{
	char		baycnt;
	MODINFO	*modptr;
	
	cpldata.cplflag		= 0;
	acdata.flag				= 0;
	cpldata.E50812Paddr	= 0xFF;

	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if ( ( ( modptr->typePrimary & TYPE_CPLGEN ) && ( modptr->id < MAX_CPLBOXES ) )
          || ( ( modptr->typePrimary & TYPE_SRG ) && ( modptr->id == E505B_BOX ) ) )
		{
			cpldata.cplflag = TRUE;
			if(modptr->options & OPT_ACRMS)
				acdata.flag = TRUE;
		}
		if((modptr->typePrimary & TYPE_CPLSRG) && !(modptr->typePrimary & (TYPE_CPLLINE | TYPE_CPLDATA)))
			cpldata.E50812Paddr = baycnt;
	}
}

void	PQF_FindModules(void)
{
	char		baycnt;
	MODINFO	*modptr;
	
	pqfdata.pqfflag = 0;
	pqfdata.ep3flag = 0;
	for(baycnt = 0; baycnt < MAX_MODULES; baycnt++)
	{
		modptr = &moddata[baycnt].modinfo;
		if((modptr->typePrimary & TYPE_PQF) && (modptr->id < MAX_PQFBOXES))
		{
			if(modptr->id == EP3_BOX)
				pqfdata.ep3flag = TRUE;
			else
				pqfdata.pqfflag = TRUE;
		}
	}
}

void	PEAK_ResetModules(
uchar	extent)
{
	char	tvar;

	if(extent)
	{
		peakdata.imon		= PEAK_IMON1;
		peakdata.vmonhi	= PEAK_VMONA;
		peakdata.vmonlo	= PEAK_VMONB;
		peakdata.peakaddr	= GHOST_BOX;
		peakdata.menuset	= 0;
		peakdata.cheat		= 0;
		PEAK_MeasureDisable();
	}
	for(tvar = MAX_MODULES-1; tvar >= 0; tvar--)
		if(moddata[tvar].modinfo.viMonitors)
		{
			moddata[tvar].relay &= ~(INST_VMASK | INST_IMASK);
			ECAT_WriteRelay0(tvar,0x0000);
 			if(extent)
				peakdata.peakaddr = tvar;
		}
	/* JMM - 05/30/95 - Set MEASURE relays here */
	paraml[0].idata = (((peakdata.vmonhi) << 4) & 0xf0) + ((peakdata.vmonlo) & 0x0f);
	MEAS_VCH_pgm(paraml);
	paraml[0].idata = peakdata.imon;
	MEAS_ICH_pgm(paraml);
}

void	CPL_ResetModules(
uchar	extent)
{
	char	tvar;

	if(extent)
	{
		cpldata.outaddr	= GHOST_BOX;
		cpldata.srcaddr	= GHOST_BOX;
		cpldata.srcrelay	= &ghostrelay;
		cpldata.outrelay	= &ghostrelay;
		cpldata.expansion	= &ghostrelay;
		cpldata.cplhigh	= CPL_L1BIT;
		cpldata.cpllow		= CPL_L2BIT;
		cpldata.multihigh	= 0;
		cpldata.phasemode	= 0;
		cpldata.angle		= 0;
		cpldata.eutrequest= 0;

		cpldata.cmode		= 0;
		cpldata.clamp		= 0;
		cpldata.output		= 0;
	}

    TIO_CPLDisable();
    for ( tvar = MAX_MODULES-1; tvar >= 0; tvar-- )
    {
        if ( moddata[tvar].modinfo.typePrimary & TYPE_CPLGEN )
        {
            /* FIX THIS LATER JMM-VERIFY-01/05/95 */
            if ( !moddata[tvar].modinfo.typePrimary & TYPE_CPLDATA )
                moddata[tvar].relay &= AC_IRANGE;
            moddata[tvar].expansion = 0x00000000;
            ECAT_WriteRelays(tvar,moddata[tvar].relay);
            ECAT_WriteEXPRelay0(tvar,0x00000000);
            ECAT_WriteEXPRelay1(tvar,0x00000000);
            if ( extent )
            {
                if ( moddata[tvar].modinfo.typePrimary & TYPE_CPLLINE )
                {
                    cpldata.srcaddr = tvar;
                    cpldata.srcrelay = &moddata[tvar].relay;
                    cpldata.expansion = &moddata[tvar].expansion;
                }
            }
        }
    }
	ldo_state &= ~(0x2020);
	PEAK_LD5 = ldo_state;
	cpldata.eutblue = OFF;
	RMS_ResetModules();
}

void	PQF_ResetModules(
uchar	extent)
{
	char	tvar;

	if(extent)
	{
		pqfdata.pqfbox		= GHOST_BOX;
		pqfdata.ep3box		= GHOST_BOX;
		pqfdata.ep3relay	= &ghostrelay;
	}

	TIO_PQFDisable();
	for(tvar = MAX_MODULES-1; tvar >= 0; tvar--)
		if(moddata[tvar].modinfo.typePrimary & TYPE_PQF)
		{
			TIO_TOMap(tvar,TO0_TOLCL0);
			ECAT_TimerSequencer(tvar,0x0000);
			TPU_write(1,1);								/* Set it High		*/
			TPU_write(1,0);								/* Set it Low		*/
			TPU_write(1,1);								/* Set it High		*/
			TIO_TOMap(tvar,TO0_DISABLE);
			ECAT_WriteRelay0(tvar,0x0000);
			ECAT_WriteRelay1(tvar,0x0000);
			ECAT_WriteEXPRelay0(tvar,0x00000000);
			ECAT_WriteEXPRelay1(tvar,0x00000000);
			moddata[tvar].relay = 0x00000000;
			moddata[tvar].expansion = 0x00000000;
			if(extent)
			{
				if(moddata[tvar].modinfo.id == EP3_BOX)
				{
					pqfdata.ep3box		= tvar;
					pqfdata.ep3relay	= &moddata[tvar].relay;
					*pqfdata.ep3relay |=  (LED_ACTIVE | RLY_RD18);
					ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
				}
				else
					pqfdata.pqfbox = tvar;
			}
		}
}

void	blue_on(void)
{
    /* Special case for E505B (combined surge/coupler/decoupler module):
       EUT power is disabled in hardware for non-coupled waveforms, so don't turn on light for those waveforms */
    if ( ( moddata[(cpldata.srcaddr)].modinfo.id == E505B_BOX ) && ( moddata[(srgdata.address)].modinfo.id == E505B_BOX ) )
    {
        // (cpldata.srcaddr = srgdata.address) for E505B
        char cplType;
        if((cplType = SRG_GetCouplerType(cpldata.srcaddr)) < 0)
            return;  // error - didn't find cplType
        if ( !wavdata[moddata[srgdata.address].modinfo.id].wavinfo[srgdata.waveform].cplFlag[cplType] )
            return;  // waveform is non-coupling, so EUT power is disabled in hardware (don't turn on blue EUT button light)
    }

	ldo_state |= 0x2020;
	PEAK_LD5 = ldo_state;
	cpldata.eutblue = ON;
	new_eut = 1;
	TPU_delay_int(314572,new_eut_trap);
	/* if((ecatmode != PQF_MODE || emcdata.modeFlag) && (cpldata.srcaddr != GHOST_BOX)) ---removed PQF restriction 2007 Oct 16  FSD */
    if(cpldata.srcaddr != GHOST_BOX)
	{
        *cpldata.srcrelay |= LED_VOLTAGE;
		if(moddata[(char)(cpldata.srcaddr)].modinfo.typePrimary & TYPE_EFT)
		{
			if(moddata[(char)(cpldata.srcaddr)].modinfo.id >= E422_BOX)
			{
				*cpldata.expansion |= CPL_422EUT;
				ECAT_WriteEXPRelay0(cpldata.srcaddr,*cpldata.expansion);
			}
			else
            {
				*cpldata.srcrelay  |= CPL_412EUT;
            }
		}
		else if ( moddata[(char)(cpldata.srcaddr)].modinfo.id != E505B_BOX )
        {
            // for all non-EFT modules except E505B (in which this relay, RD26, is used for a separate Surge function)
            *cpldata.srcrelay |= CPL_EUTENABLE;
        }

		ECAT_WriteRelay0(cpldata.srcaddr,*cpldata.srcrelay);
		ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
	}
	if(emcdata.modeFlag)
	{
		if(emcdata.ERIxAddr < MAX_MODULES)
		{
			emcdata.ERIxRelay |= LED_VOLTAGE;
			ECAT_WriteRelay1(emcdata.ERIxAddr,(emcdata.ERIxRelay >> 16));
		}
		if(emcdata.EP7xAddr < MAX_MODULES)
		{
			emcdata.EP7xRelay |= LED_VOLTAGE;
			ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
		}
		if(emcdata.EOPxAddr < MAX_MODULES)
		{
			emcdata.EOPxRelay |= LED_VOLTAGE;
			ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
			/* Set up background timer to turn on SoftStart relay after 1 sec */
			emcdata.EOPxSoftStart	= FALSE;
			emcdata.EOPxSoftStartCh	= TPU_delay_int(2096436,EMC_EOPxSoftStartTrap);
		}
		if((emcdata.EP6xFlag) && (ecatmode == PQF_MODE))
		{
			emcdata.EP6xRelay |= LED_VOLTAGE;
			ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
		}
	}
	if(pqfdata.ep3flag)
	{
		*pqfdata.ep3relay |= (LED_VOLTAGE | RLY_RD18 | RLY_RD22);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
	}
	if(ecatmode == PQF_MODE)
	{
		PQF_softstartflag = FALSE;
		PQF_softstartchannel = TPU_delay_int(8385744,PQF_SoftStartTrap);
		if(pqfdata.ep3flag)
		{
			EP3_softstartflag = FALSE;
			EP3_softstartchannel = TPU_delay_int(2096436,EP3_SoftStartTrap);
		}
	}
}

void	blue_off(void)
{
	ldo_state &= ~(0x2020);
	PEAK_LD5 = ldo_state;
	cpldata.eutblue = OFF;
    /* if((ecatmode != PQF_MODE || emcdata.modeFlag) && (cpldata.srcaddr != GHOST_BOX)) ---removed PQF restriction 2007 Oct 16  FSD */
	if(cpldata.srcaddr != GHOST_BOX)
	{
		if(moddata[(char)(cpldata.srcaddr)].modinfo.typePrimary & TYPE_EFT)
		{
			if((eftdata.eftaddr != cpldata.srcaddr) || (!eftdata.hvon))
				*cpldata.srcrelay &= ~(LED_VOLTAGE);
			if(moddata[(char)(cpldata.srcaddr)].modinfo.id >= E422_BOX)
			{
				*cpldata.expansion &= ~(CPL_422EUT);
				ECAT_WriteEXPRelay0(cpldata.srcaddr,*cpldata.expansion);
			}
			else
            {
				*cpldata.srcrelay &= ~(CPL_412EUT);
            }
		}
		else 
        {
            *cpldata.srcrelay &= ~(LED_VOLTAGE);
            if ( moddata[(char)(cpldata.srcaddr)].modinfo.id != E505B_BOX )
            {
                // for all non-EFT modules except E505B (in which this relay, RD26, is used for a separate Surge function)
                *cpldata.srcrelay &= ~(CPL_EUTENABLE);
            }
        }

		ECAT_WriteRelay0(cpldata.srcaddr,*cpldata.srcrelay);
		ECAT_WriteRelay1(cpldata.srcaddr,(*cpldata.srcrelay >> 16));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
		ECAT_TimerSequencer(cpldata.srcaddr,(*cpldata.srcrelay >> 24));
	}
	if(ecatmode == PQF_MODE)
	{
		raw_relay(1,0);
		PQF_softstartflag = TRUE;
		if(PQF_softstartchannel)
			TPU_kill(PQF_softstartchannel);
	}
	if(emcdata.modeFlag)
	{
		if(emcdata.ERIxAddr < MAX_MODULES)
		{
			emcdata.ERIxRelay &= ~(LED_VOLTAGE);
			ECAT_WriteRelay1(emcdata.ERIxAddr,(emcdata.ERIxRelay >> 16));
		}
		if(emcdata.EP7xAddr < MAX_MODULES)
		{
			emcdata.EP7xRelay &= ~(LED_VOLTAGE);
			ECAT_WriteRelay1(emcdata.EP7xAddr,(emcdata.EP7xRelay >> 16));
		}
		if(emcdata.EOPxAddr < MAX_MODULES)
		{
			/* Shut OFF LED and SoftStart Relay */
			emcdata.EOPxRelay &= ~(LED_VOLTAGE | RLY_RD23);
			ECAT_WriteRelay1(emcdata.EOPxAddr,(emcdata.EOPxRelay >> 16));
		}
		if((ecatmode != PQF_MODE) && (emcdata.EP6xFlag))
		{
			emcdata.EP6xRelay &= ~(LED_VOLTAGE | RLY_RD27 | RLY_RD25 | RLY_RD30 | RLY_RD01);
			ECAT_WriteRelay0(emcdata.EP6xAddr,emcdata.EP6xRelay);
			ECAT_WriteRelay1(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 16));
			TIO_TOMap(emcdata.EP6xAddr,TO0_TOLCL0);
			ECAT_TimerSequencer(emcdata.EP6xAddr,(emcdata.EP6xRelay >> 24));
			TPU_write(1,1);		/* Set it High	*/
			TPU_write(1,0);		/* Set it Low	*/
			TPU_write(1,1);		/* Set it High	*/
			TIO_TOMap(emcdata.EP6xAddr,TO0_DISABLE);
			PQF_softstartflag = TRUE;
			if(PQF_softstartchannel)
				TPU_kill(PQF_softstartchannel);
		}
	}
	if(pqfdata.ep3flag)
	{
		EP3_softstartflag = TRUE;
		if(EP3_softstartchannel)
			TPU_kill(EP3_softstartchannel);
		EP3_RD22StartFlag = TRUE;
		if(EP3_RD22StartChannel)
			TPU_kill(EP3_RD22StartChannel);
		*pqfdata.ep3relay &= ~(LED_VOLTAGE | RLY_RD23 | RLY_RD24 | RLY_RD25);
		ECAT_WriteRelay1(pqfdata.ep3box,(*pqfdata.ep3relay >> 16));
		ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
		ECAT_TimerSequencer(pqfdata.ep3box,(*pqfdata.ep3relay >> 24));
	}
}

uchar	ECAT_StatusFlag(void)
{
	uchar	statflag;

	statflag = 0x00;
	if((ilockstate & 0x7C) != 0x7C)
		statflag |= 0x01;
	if(warmup.powerup)
	{
		statflag |= 0x02;
		warmup.powerup = 0;
	}
	if(eftdata.setup)
		statflag |= 0x04;

	if(back_from_local)
	{
		back_from_local = FALSE;
		statflag |= 0x20;
	}
	return(statflag);
}

/* Set/reset bit 2 (LDO2) in the 'Local Digital Output' register
   on the Peak Detector board (90-100-130-00).
   This bit is easy to physically access on the Interface Overlay
   board (90-100-100-00), which is the board with the FiberCom
   serial port.  It is the outboard side of R3, which itself is at
   the upper corner of the PCB, on the side toward the outside of
   the system.  Hardware documentation refers to this as "XV1 Relay",
   which is part of a never-used interface to the MiniZap. */
void debug_bit_set()
{
	ldo_state |= 0x0404;
	PEAK_LD5 = ldo_state;
    return;
}

void debug_bit_reset()
{
    ldo_state &= ~(0x0404);
	PEAK_LD5 = ldo_state;
    return;
}
    
void debug_bit_pulse( int duration )
{
    debug_bit_set();
	TPU_delay( duration );  // duration is in ms
    debug_bit_reset();
    return;
}
