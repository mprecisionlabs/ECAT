#ifndef __ECAT_H              /* Avoid multiple inclusions */
#define __ECAT_H

#include <shortdef.h>
#include "parser.h"
#include "module.h"

/* Hack special for single customer (Sysgration) - not supported in standard
	firmware releases.  When defined here, it disables line sync checking, and
   also converts all phase mode requests to RANDOM. ---FSD 04 Oct 96--- */
#undef HACK_400Hz

#define DISPLAY_WIDTH   (40)

#define  GHOST_MODE     (0x00)
#define  SRG_MODE       (0x01)
#define  EFT_MODE       (0x02)
#define  PQF_MODE       (0x03)
#define  AMP_MODE       (0x04)

#define  OFF            (0)
#define  ON             (1)
#define  NEGATIVE       (0)
#define  POSITIVE       (1)
#define  TRIG_TPU       (2)
#define  PHASE_TPU      (6)
#define  TPU_TICK       ((float)(477E-9))

#define  TICKS_PER_SECOND   (2097152)
#define  CHG_HOLDTIME       (20964361)     /* Hold time before DUMPING HV   */
#define  EARLY_CHECK_TIME   (TICKS_PER_SECOND * 5)  /* 5 sec */

#define  PIT_RUNNER_PERIOD (235850)

#define  INT_disable()  (asm(" move.w #$2500,SR"))
#define  INT_enable()   (asm(" move.w #$2100,SR"))

#define  PEAK_VADJ      (20400)
#define  PEAK_IADJ      (10200)

#define  MAX_AMBIENT_SAMPLE   (10)  /* # samples in peak det ambient meas. */
#define  SURGE_PEAK_THRESHOLD (30)  /* Surge detection lvl, above ambient
													peak level. For most surge networks,
													4.88V/b * 30 bits=146.4V */

#define  SRG_MAXNRMVLT  (6600)
#define  SRG_MAXCALVLT  (10000)
#define  MAX_FP_VOLTAGE (11000)
#define  MAX_CPLTYPES   (0x03)      /* 0=CPL, 1=HVCPL, 2=DATA              */
#define  MAX_LMODES     (0x07)      /* 1/4L 1/3L 1/2L 1L 1-1/3L 1-1/2L 2L  */
#define  MAX_DATARANGES (0x05)

#define LS_RNDMODE      (0x00)
#define LS_L1MODE    (0x01)
#define LS_L2MODE    (0x02)
#define LS_L3MODE    (0x03)

#define  ERR_SRGIDLE    (0x01)
#define  ERR_SRGNET     (0x02)
#define  ERR_SRGAC      (0x03)
#define  ERR_SRGBLUE    (0x04)
#define  ERR_SRGRDY     (0x05)
#define  ERR_SRGIL      (0x06)
#define  ERR_SRGREAD    (0x07)
#define  ERR_SRGDC      (0x08)
#define  ERR_SRGHV      (0x09)
#define  ERR_EFTBURST   (0x10)

/*
 * Generic relay equates
 */
#define  RLY_RD00       (0x00000001)
#define  RLY_RD01       (0x00000002)
#define  RLY_RD02       (0x00000004)
#define  RLY_RD03       (0x00000008)
#define  RLY_RD04       (0x00000010)
#define  RLY_RD05       (0x00000020)
#define  RLY_RD06       (0x00000040)
#define  RLY_RD07       (0x00000080)
#define  RLY_RD08       (0x00000100)
#define  RLY_RD09       (0x00000200)
#define  RLY_RD10       (0x00000400)
#define  RLY_RD11       (0x00000800)
#define  RLY_RD12       (0x00001000)
#define  RLY_RD13       (0x00002000)
#define  RLY_RD14       (0x00004000)
#define  RLY_RD15       (0x00008000)
#define  RLY_RD16       (0x00010000)
#define  RLY_RD17       (0x00020000)
#define  RLY_RD18       (0x00040000)
#define  RLY_RD19       (0x00080000)
#define  RLY_RD20       (0x00100000)
#define  RLY_RD21       (0x00200000)
#define  RLY_RD22       (0x00400000)
#define  RLY_RD23       (0x00800000)
#define  RLY_RD24       (0x01000000)
#define  RLY_RD25       (0x02000000)
#define  RLY_RD26       (0x04000000)
#define  RLY_RD27       (0x08000000)
#define  RLY_RD28       (0x10000000)
#define  RLY_RD29       (0x20000000)
#define  RLY_RD30       (0x40000000)
#define  RLY_RD31       (0x80000000)

#define  LED_ACTIVE     RLY_RD17
#define  LED_VOLTAGE    RLY_RD16

/*
 * Relay equates for COUPLER modules
 */
#define  CPL_EUTENABLE  (0x04000000)   /* EUT enable/disable selection */

#define  CPL_MODEMASK   (RLY_RD23 | RLY_RD27)   /* Mask for coupler mode selections */
#define  CPL_SRGMODE    (RLY_RD27)              /* RD23 OFF, RD27 ON */
#define  CPL_EFTMODE    (0x00000000)            /* RD23 OFF, RD27 OFF */
#define  CPL_PQFMODE    (CPL_MODEMASK)          /* RD23 ON, RD27 ON */
#define  CPL_EMCMODE    (CPL_MODEMASK)          /* RD23 ON, RD27 ON */          

#define  CPL_CPLMASK    (0x33FC0000)
#define  CPL_SRGL1HI    (0x00040000)   /* SURGE coupling modes */
#define  CPL_SRGL2HI    (0x00080000)
#define  CPL_SRGL3HI    (0x10000000)
#define  CPL_SRGNUHI    (0x01000000)
#define  CPL_SRGL1LO    (0x00000000)   /* NO relay driver for this line */
#define  CPL_SRGL2LO    (0x00100000)
#define  CPL_SRGL3LO    (0x20000000)
#define  CPL_SRGNULO    (0x02000000)
#define  CPL_SRGPELO    (0x00200000)
#define  CPL_SRGCAPS    (0x00400000)
#define  CPL_SRGCHKS    (0x00800000)
#define  CPL_REFRLY0    (0x40000000)
#define  CPL_REFRLY1    (0x80000000)

#define  CPL_EFTL1      (0x00040000)   /* EFT coupling modes            */
#define  CPL_EFTL2      (0x10000000)
#define  CPL_EFTL3      (0x00080000)
#define  CPL_EFTNU      (0x01000000)
#define  CPL_EFTPE      (0x02000000)
#define  CPL_EFTMASK    (0x130C0000)

#define  INST_VMONHIA   (0x00000001)   /* Instrumentation board channels*/
#define  INST_VMONHIB   (0x00000002)
#define  INST_VMONHIC   (0x00000004)
#define  INST_VMONHID   (0x00000008)
#define  INST_VMONHIE   (0x00000010)
#define  INST_VMONLOA   (0x00000020)
#define  INST_VMONLOB   (0x00000040)
#define  INST_VMONLOC   (0x00000080)
#define  INST_VMONLOD   (0x00000100)
#define  INST_VMONLOE   (0x00000200)
#define  INST_IMON1     (0x00000400)
#define  INST_IMON2     (0x00000800)
#define  INST_IMON3     (0x00001000)
#define  INST_IMON4     (0x00001000)           /*temp*/
#define  INST_IMON5     (0x00001000)           /*temp*/
#define  INST_VENABLE   (0x00008000)
#define  INST_VMASK     (0x000083FF)
#define  INST_IMASK     (0x00001C00)

#define  INST5_VMONHIA  (0x00000000)   /* 5-ch Instrumentation Channels */
#define  INST5_VMONHIB  (0x00000001)
#define  INST5_VMONHIC  (0x00000002)
#define  INST5_VMONHID  (0x00000003)
#define  INST5_VMONHIE  (0x00000000)
#define  INST5_VMONLOA  (0x00000000)
#define  INST5_VMONLOB  (0x00000020)
#define  INST5_VMONLOC  (0x00000040)
#define  INST5_VMONLOD  (0x00000060)
#define  INST5_VMONLOE  (0x00000080)
#define  INST5_VENABLE  (0x00008000)
#define  INST5_IMON1    (0x00000400)
#define  INST5_IMON2    (0x00000800)
#define  INST5_IMON3    (0x00000C00)
#define  INST5_IMON4    (0x00000000)
#define  INST5_IMON5    (0x00001000)

/*#define   INST_IMON4     (0x00002000)*/
/*#define   INST_IMON5     (0x00004000)*/
/*#define   INST_VENABLE   (0x00008000)*/
/*#define   INST_VMASK     (0x000083FF)*/
/*#define   INST_IMASK     (0x00007C00)*/

/* Other defines?? */
#define  CPL_L1BIT      (0x01)
#define  CPL_L2BIT      (0x02)
#define  CPL_L3BIT      (0x04)
#define  CPL_NUBIT      (0x08)
#define  CPL_PEBIT      (0x10)

#define  PEAK_VMONA     (0x01)         /* Instrumentation choices       */
#define  PEAK_VMONB     (0x02)
#define  PEAK_VMONC     (0x03)
#define  PEAK_VMOND     (0x04)
#define  PEAK_VMONE     (0x05)
#define  PEAK_IMON1     (0x01)
#define  PEAK_IMON2     (0x02)
#define  PEAK_IMON3     (0x03)
#define  PEAK_IMON4     (0x04)
#define  PEAK_IMON5     (0x05)

typedef enum dac_resolution
{
	DR_12 = 12,
    DR_16 = 16
}	DAC_RESOLUTION;  // used for programming E507 and E52x HV supplies


typedef  struct tag_srgdata
{
	uint     *relay;
	uint     *expansion;
	uchar    srgflag;
	uchar    waveform;
	uchar    address;
	uchar    delay;
	int      voltage;
} SRGDATA;

typedef struct tag_srgcheat
{
	uchar lineCheck;                 /* Disables verification of AC signal  */
} SRGCHEAT;

typedef struct tag_eftdata
{
	uchar    eftflag;
	uchar    eftaddr;
	ushort   duration;
	ushort   period;
	uint     *relay;
	int      voltage;
	uint     frequency;
	uchar    unit;
	uchar    waveform;
	uchar    mode;
	uchar    output;
	uchar    active;
	uchar    setup;
	uchar    range;
	uchar    hvon;
    /*uchar    need_EFT_ACCRISE_relay;*/
} EFTDATA;

typedef struct tag_eftcheat
{
	uchar lineCheck;                 /* Disables verification of AC signal  */
	uchar forceHVRange;              /* Forces the HV range at any voltage  */
} EFTCHEAT;

typedef struct tag_cpldata
{
	uchar    cplflag;                      /* Do ANY coupler modules exist  */
	uchar    srcaddr;                      /* Module for EUT SRC functions  */
	uchar    outaddr;                      /* Module for OUTPUT functions   */
	uchar    cplhigh;                      /* HIGH coupling modes           */
	uchar    cpllow;                       /* LOW coupling modes            */
	uchar    multihigh;                    /* Indicates multiple HIGH bits  */
	uchar    phasemode;                    /* Phase reference (RND,Lx,DC)   */
	ushort   angle;                        /* Angle for linesync work       */
	uint     *srcrelay;                    /* Relays for EUT SRC modules    */
	uint     *outrelay;                    /* Relays for OUTPUT modules     */
	uint     *expansion;                   /* Module expansion connector    */
	uchar    eutrequest;                   /* EUT power has been REQUESTED  */
	uchar    eutswitch;                    /* Physical state of EUT SWITCH  */
	uchar    eutblue;                      /* Physical state of EUT relay   */
	uchar    cmode;
	uchar    output;
	uchar    clamp;
	uchar    E50812Paddr;                  /* E508-12P address if installed */
} CPLDATA;

typedef struct tag_pkdata
{
	uchar flag;
	uchar peakaddr;
	uchar vmonhi;
	uchar vmonlo;
	uchar imon;
	short vpeakpos;
	short vpeakneg;
	short ipeakpos;
	short ipeakneg;
	uchar menuset;
	uchar cheat;                           /* Run 5-chan in 3-chan mode     */
} PKDATA;

typedef struct tag_aclinedata          /* AC line (L1,L2,L3) data */
{
	int    peakm;                          /* Peak minus over period  */
	int    peakp;                          /* Peak plus  over period  */
	float  rms;                     /* rms        over period  */
} ACLINEDATA;

typedef struct tag_acdata
{
	uchar  flag;
	uchar  acbox;           /* network for RMS measurements              */
	uchar  measure_on;      /* ADS_TRAP is active                        */
	uchar  line;            /* AC lime displayed (0=surge,1=L1,2=L2,3=L3)*/
	uchar  time;            /* time to display ac peaks & RMS            */
	char   error;           /* 0 / ERR_AC_INPUT / ERR_AC_LIMIT           */
	ushort pit_cnt;         /* PIT count to run ADS_TRAP                 */
	uint   cycle_cnt;       
	uint   checker;         /* timer to check AC current                 */
	float  peak_lim;        /* peak     limit in amps                    */
	float  rmsmin_lim;      /* rms  min limit in amps                    */
	float  rmsmax_lim;      /* rms  max limit in amps                    */

	ACLINEDATA ldata[3];    /* structures for AC lines                   */

} ACDATA;

typedef struct tag_wavinfo
{
	char     *name;                        /* Name of Waveform              */
	uchar    frontPanelFlag;               /* Front Panel Output Ability    */
	uchar    cplFlag[MAX_CPLTYPES];        /* Coupler Type Ability          */
	short    maxVoltage[MAX_CPLTYPES];     /* Maximum Voltage per Waveform  */
	uchar    minDelay[MAX_CPLTYPES];       /* Minimum Delay per Waveform    */
	uchar    rangeIndex[MAX_CPLTYPES];     /* Index into RANGE array        */
	ushort   voltFactor;                   /* ???What is this really for?   */
} WAVINFO;

typedef struct tag_wavdata
{
	uchar    numWaves;                  /* Number of Waves in Network       */
	WAVINFO  wavinfo[MAX_WAVEFORMS];    /* Individual Waveform Information  */
} WAVDATA;

typedef struct tag_warm_up
{
	uint  time;
	uchar state;
	uchar powerup;
	uchar started;
} WARM_UP;

typedef struct tag_disdata
{
	uchar previous;
	uchar current;
	uchar request;
} DISDATA;

typedef struct tag_chgdata
{
	uchar    flag;
	short    time;
	uchar    monitor;
	uchar    checker;
} CHGDATA;

typedef struct
{
	uchar pqfbox;
	uchar pqfflag;
	uchar ep3flag;
	uchar ep3box;
	uchar ep3phase;
	uint  *ep3relay;
} PQFDATA;

typedef  struct tag_ehvdata
{
	uchar    ehvflag;
	uchar    network;
} EHVDATA;

typedef  struct tag_ehvinfo
{
	short    maxVoltage;
	ushort   factor;
} EHVINFO;

/*
 * SURGE-specific routines
 */
void     SRG_InitSurgeDetected( void );
uchar    SRG_GetSurgeDetected( void );
uchar    SRG_OutputValid(uchar,uchar,uchar);
uchar    SRG_SourceValid( uchar source_address, uchar surge_address );
uchar    SRG_MinimumDelay(uchar,uchar);
int      SRG_MaximumVoltage(uchar,uchar);
int      SRG_MaxVoltageToSupply(uchar,uchar);
int	CAL_GetCalibratedVoltage( int adjusted_voltage, int raw_voltage, int voltage_scaling,
                              uchar cpl_hi, uchar cpl_lo,
                              CALINFO *ci_wave, CALINFO *ci_cplr,
                              uchar wave, uchar netId, char cplType );
void     SRG_ChargeMonitor(void);
void     SRG_EarlyChargeCheck(void);
void     SRG_ChargeCheck(void);
void     SRG_FindModules(void);
void     SRG_ResetModules(uchar);
char     SRG_GetCouplerType(uchar);
char     SRG_SurgeKill(void);
char     SRG_SurgeStart(void);
char     SRG_SurgeTrigger(void);
char     SRG_VerifyCoupling(uchar,uchar);
char     SRG_SetCouplerHardware(void);
char     SRG_ClearCouplerHardware(void);
char     SRG_HVProgram(uchar,uchar,int,uchar);
char     E507_HVProgram(uchar,int,uchar);
char     E52x_HVProgram(uchar,int,uchar);
void     E52x_SetHVPS(uchar bay, uint dac_value, DAC_RESOLUTION dac_resolution);

uchar    ECAT_CheckEUT(uchar,uchar);
uchar    ECAT_StatusFlag(void);
void     CPL_FindModules(void);
void     EFT_FindModules(void);
void     PQF_FindModules(void);
void     CPL_ResetModules(uchar);
void     EFT_ResetModules(uchar);
void     PQF_ResetModules(uchar);
void     PEAK_ResetModules(uchar);
char     PEAK_MeasureDisable(void);

int   star_IDN_rpt(P_PARAMT *);
int   star_OPC_rpt(P_PARAMT *);
int   star_RST_pgm(P_PARAMT *);
int   star_TRG_pgm(P_PARAMT *);

int   SRG_CHEAT(P_PARAMT *);
int   SRG_CHG_pgm(P_PARAMT *);
int   SRG_CPL_pgm(P_PARAMT *);
int   SRG_CPL_rpt(P_PARAMT *);
int   SRG_DLY_pgm(P_PARAMT *);
int   SRG_DLY_rpt(P_PARAMT *);
int   SRG_NET_pgm(P_PARAMT *);
int   SRG_NET_rpt(P_PARAMT *);
int   SRG_OUT_pgm(P_PARAMT *);
int   SRG_OUT_rpt(P_PARAMT *);
int   SRG_VLT_pgm(P_PARAMT *);
int   SRG_VLT_rpt(P_PARAMT *);
int   SRG_WAV_pgm(P_PARAMT *);
int   SRG_WAV_rpt(P_PARAMT *);
int	  SRG_LCK_pgm(P_PARAMT *params);

int   root_ABT_pgm(P_PARAMT *);
int   root_CPL_pgm(P_PARAMT *);
int   root_CPL_rpt(P_PARAMT *);
int   root_EUT_pgm(P_PARAMT *);
int   root_EUT_rpt(P_PARAMT *);

int   BAY_DFLT_pgm(P_PARAMT *);
int   BAY_DUMP_rpt(P_PARAMT *);
int   BAY_MEAS_pgm(P_PARAMT *);
int   BAY_MEAS_rpt(P_PARAMT *);
int   BAY_NAME_pgm(P_PARAMT *);
int   BAY_NAME_rpt(P_PARAMT *);
int   BAY_OPTS_pgm(P_PARAMT *);
int   BAY_OPTS_rpt(P_PARAMT *);
int   BAY_WRTE_pgm(P_PARAMT *);
int   BAY_TYPE_pgm(P_PARAMT *);
int   BAY_TYPE_rpt(P_PARAMT *);
int   BAY_SER_pgm(P_PARAMT *);
int   BAY_SER_rpt(P_PARAMT *);
int   BAY_CHK_rpt(P_PARAMT *);
int   BAY_CAL_pgm(P_PARAMT *);
int   BAY_CAL_rpt(P_PARAMT *);
int   BAY_ID_pgm(P_PARAMT *);
int   BAY_ID_rpt(P_PARAMT *);
int   BAY_WAV_rpt(P_PARAMT *);
int   BAY_DLY_rpt(P_PARAMT *);

int   DATA_CLAMP_pgm(P_PARAMT *);
int   DATA_CLAMP_rpt(P_PARAMT *);
int   DATA_CMODE_pgm(P_PARAMT *);
int   DATA_CMODE_rpt(P_PARAMT *);
int   DATA_OUT_pgm(P_PARAMT *);
int   DATA_OUT_rpt(P_PARAMT *);

int   EFT_BAY_pgm(P_PARAMT *);
int   EFT_BAY_rpt(P_PARAMT *);
int   EFT_CPL_pgm(P_PARAMT *);
int   EFT_CPL_rpt(P_PARAMT *);
int   EFT_DUR_pgm(P_PARAMT *);
int   EFT_DUR_rpt(P_PARAMT *);
int   EFT_FRQ_pgm(P_PARAMT *);
int   EFT_FRQ_rpt(P_PARAMT *);
int   EFT_MDE_pgm(P_PARAMT *);
int   EFT_MDE_rpt(P_PARAMT *);
int   EFT_OUT_pgm(P_PARAMT *);
int   EFT_OUT_rpt(P_PARAMT *);
int   EFT_PER_pgm(P_PARAMT *);
int   EFT_PER_rpt(P_PARAMT *);
int   EFT_SET_pgm(P_PARAMT *);
int   EFT_SET_rpt(P_PARAMT *);
int   EFT_VLT_pgm(P_PARAMT *);
int   EFT_VLT_rpt(P_PARAMT *);
int   EFT_WAV_pgm(P_PARAMT *);
int   EFT_WAV_rpt(P_PARAMT *);
int	  EFT_LCK_pgm(P_PARAMT *params);

int   MEAS_BAY_pgm(P_PARAMT *);
int   MEAS_BAY_rpt(P_PARAMT *);
int   MEAS_ICH_pgm(P_PARAMT *);
int   MEAS_ICH_rpt(P_PARAMT *);
int   MEAS_IPK_rpt(P_PARAMT *);
int   MEAS_VCH_pgm(P_PARAMT *);
int   MEAS_VCH_rpt(P_PARAMT *);
int   MEAS_VPK_rpt(P_PARAMT *);

int   LI_MODE_pgm(P_PARAMT *);
int   LI_MODE_rpt(P_PARAMT *);
int   LI_ANG_pgm(P_PARAMT *);
int   LI_ANG_rpt(P_PARAMT *);
int   LI_REF_pgm(P_PARAMT *);
int   LI_REF_rpt(P_PARAMT *);

int   SYS_BDMP_rpt(P_PARAMT *);
int   SYS_ILOK_rpt(P_PARAMT *);
int   SYS_ITXT_rpt(P_PARAMT *);
int   SYS_JJAW_pgm(P_PARAMT *);
int   SYS_MEM_rpt(P_PARAMT *);
int   SYS_MEM_pgm(P_PARAMT *);
int	  SYS_BAUD_rpt(P_PARAMT *);
int	  SYS_BAUD_pgm(P_PARAMT *);
int   SYS_RLY_pgm(P_PARAMT *);
int   SYS_RLY_rpt(P_PARAMT *);
int   load_and_pray(P_PARAMT *);

int   PQF_exit_pgm(P_PARAMT *);
int   PQF_load_list(P_PARAMT *);
int   PQF_save_list(P_PARAMT *);
int   PQF_reset_list(P_PARAMT *);
int   PQF_set_list(P_PARAMT *);
int   PQF_setn_list(P_PARAMT *);
int   PQF_run_list(P_PARAMT *);
int   PQF_stop_list(P_PARAMT *);
int   PQF_setc_tap(P_PARAMT *);
int   PQF_setnc_tap(P_PARAMT *);
int   PQF_sets_tap(P_PARAMT *);
int   PQF_setns_tap(P_PARAMT *);
int   PQF_set_goto(P_PARAMT *);
int   PQF_setn_goto(P_PARAMT *);
int   PQF_error_rpt(P_PARAMT *);
int   PQF_qualify_cmd(P_PARAMT *);
int   PQF_phase_pgm(P_PARAMT *);

void debug_bit_set();
void debug_bit_reset();
void debug_bit_pulse( int duration );

#endif                        /* ifndef ECAT_H */
