#ifndef PQF_DEFINED
#define PQF_DEFINED YES

#define NUM_PQF_LEVELS		11
#define PQF_LEVEL_OPEN      0
#define PQF_LEVEL_SHORT     1
#define PQF_LEVEL_40        2
#define PQF_LEVEL_50        3
#define PQF_LEVEL_70        4
#define PQF_LEVEL_80        5
#define PQF_LEVEL_90        6
#define PQF_LEVEL_100       7
#define PQF_LEVEL_110       8
#define PQF_LEVEL_120       9
#define PQF_LEVEL_150		10


// maximum length for columns in the log data.
#define PQF_COLUMN_LEN       12

#pragma pack(push)
#pragma pack(1)
// OLD 16-bit structure for compatibility with the old test plan files
/* ???????????????
  The usage below is a little tricky.  We'll have an array of PQFSTEPS,
  whose 0th element will be treated (sometimes) as a PQFHEADER, so the
  PQFSTEP structure should always be at least as large as PQFHEADER, lest
  PQFHEADER grow into the next PQFSTEP.
*/
typedef STRUCTURE tagPQFHEADER_16
{
    WORD  hdr_next;						/* forward thread  */
    WORD  hdr_prev;						/* backward thread */
    SHORT hdr_numsteps_allocated;
    SHORT hdr_numsteps;					/* number of steps in use (WARNING: not necessarily contiguous) */
    SHORT hdr_dummy_start;				/* needed to we can add last step's next step degrees to time */
    DWORD hdr_dwTotDegrees;
    WORD  hdr_line_voltage;
    WORD  hdr_line_freq;
    SHORT hdr_standby_level;			/* 0=0%-Open, 1=0%-Short, 2=40%, 3=50%, ... */
    BOOL  hdr_bAux;						/* TRUE if AUX is enabled to replace 50% */
	tagPQFHEADER_16() {memset(this, 0, sizeof(tagPQFHEADER_16));}
} PQFHEADER_16, FAR * LPPQFHEADER_16;

// New 32-bit header - used a standalone instance and not as part of the test plan list
typedef STRUCTURE tagPQFHEADER
{
    SHORT hdr_numsteps;					/* number of steps */
    DWORD hdr_dwTotDegrees;
    WORD  hdr_line_voltage;
    WORD  hdr_line_freq;
    SHORT hdr_standby_level;			/* 0=0%-Open, 1=0%-Short, 2=40%, 3=50%, ... */
    BOOL  hdr_bAux;						/* TRUE if AUX is enabled to replace 50% */
	tagPQFHEADER() {memset(this, 0, sizeof(tagPQFHEADER));}
} PQFHEADER, FAR * LPPQFHEADER;

#define PQFSTEP_NUM_SPARES 10
// OLD 16-bit structure for compatibility with the old test plan files
typedef STRUCTURE tagPQFSTEP_16
{
    WORD  step_next;				// forward thread
    WORD  step_prev;				// backward thread
    SHORT step_num;
    SHORT step_level;				// 0=0%-Open, 1=0%-Short, 2=40%, 3=50%, ... 
    SHORT step_start;				// degrees 
    SHORT step_duration;			// either of: seconds, seconds*100, cycles 
    SHORT step_duration_units;
    SHORT step_rms_ilim;			// times 10, so 20.1 represented as 201 
    SHORT step_peak_ilim;
    SHORT step_spares[PQFSTEP_NUM_SPARES];
	tagPQFSTEP_16() {memset(this, 0, sizeof(tagPQFSTEP_16));}
} PQFSTEP_16, *LPPQFSTEP_16;

// new 32-bit compliant structure ([aat] I made data members
// __int32 type. It's an MS specific type that ensures that the integer 
// will remain 32-bit long, even on 64-bit platforms - we are not porting
// this code to UNIX, right :-))
typedef STRUCTURE tagPQFSTEP
{
	BYTE step_junk1[4];
    __int32 step_num;
    __int32 step_level;					/* 0=0%-Open, 1=0%-Short, 2=40%, 3=50%, ... */
    __int32 step_start;					/* degrees */
    __int32 step_duration;				/* either of: seconds, seconds*100, cycles */
    __int32 step_duration_units;
    __int32 step_rms_ilim;				/* times 10, so 20.1 represented as 201 */
    __int32 step_peak_ilim;
    __int32 step_spares[PQFSTEP_NUM_SPARES];
	tagPQFSTEP() {memset(this, 0, sizeof(tagPQFSTEP));}
	tagPQFSTEP& operator = (PQFSTEP_16 &step16) 
	{
		memset(step_junk1, 0, sizeof(step_junk1));
		memset(step_spares, 0, sizeof(step_spares));
		step_num = step16.step_num;
		step_level = step16.step_level;
		step_start = step16.step_start;
		step_duration = step16.step_duration;
		step_duration_units = step16.step_duration_units;
		step_rms_ilim = step16.step_rms_ilim;
		step_peak_ilim = step16.step_peak_ilim;
		return *this;
	}
} PQFSTEP, FAR * LPPQFSTEP;

#pragma pack(pop)

#define PQF_NUM_DURATION_UNITS 3
#define PQF_DURATION_UNITS_MIN 0
#define PQF_DURATION_UNITS_SEC 1
#define PQF_DURATION_UNITS_CYC 2

/* below represents 50:59:59.99 @ freq=99 */
#define PQF_MAX_DURATION_DEGREES 3965759784L
#define PQF_DEGREES_PER_CYCLE 360L

GLOBALVARIABLE PQFHEADER pqf_lpStepListhdr;
GLOBALVARIABLE BOOL pqf_bEnableEdit;
GLOBALVARIABLE BOOL pqf_bEnableRun;
GLOBALVARIABLE FARPROC pqf_lpfnListSubProc GLOB_INIT0L;
GLOBALVARIABLE FARPROC pqf_lpfnListOldProc GLOB_INIT0L;
GLOBALVARIABLE SHORT pqf_dwCyclesPerSecond;
GLOBALVARIABLE DWORD pqf_dwDegreesPerSecond;
GLOBALVARIABLE DWORD pqf_dwDegreesPerMinute;
GLOBALVARIABLE DWORD pqf_dwDegreesPerHour;

GLOBALVARIABLE DRAWITEMSTRUCT DrawItemStruct;

#ifdef IS_MAIN_PROGRAM_FILE
GLOBALVARIABLE CHARACTER * pqf_duration_units[PQF_NUM_DURATION_UNITS] = {
    "m:s", "sec", "cyc",
};
GLOBALVARIABLE SHORT pqf_level[NUM_PQF_LEVELS] = {
    -1, 0, 40, 50, 70, 80, 90, 100, 110, 120, 150,
};
GLOBALVARIABLE CHARACTER pqf_level_entry[NUM_PQF_LEVELS][6] = { 
    "Open", "Short", "40", "50", "70", "80", "90", "100", "110", "120", "150"
};
GLOBALVARIABLE CHARACTER pqf_leveledit_display[NUM_PQF_LEVELS][7] = { 
    "Open", "Short", "    40", "    50", "    70", "    80", "    90", "  100", "  110", "  120", "  150"
};
GLOBALVARIABLE CHARACTER pqf_level_display[NUM_PQF_LEVELS][9] = { 
    "Open-0%", "Short-0%", "40%", "50%", "70%", "80%", "90%", "100%", "110%", "120%", "150%"
};
GLOBALVARIABLE CHARACTER pqf_level_list[NUM_PQF_LEVELS][6] = { 
    "Open", "Short", "  40", "  50", "  70", "  80", "  90", " 100", " 110", " 120", " 150"
};
#else
GLOBALVARIABLE CHARACTER * pqf_duration_units[PQF_NUM_DURATION_UNITS];
GLOBALVARIABLE SHORT pqf_level[NUM_PQF_LEVELS];
GLOBALVARIABLE CHARACTER pqf_level_entry[NUM_PQF_LEVELS][6]; 
GLOBALVARIABLE CHARACTER pqf_leveledit_display[NUM_PQF_LEVELS][7]; 
GLOBALVARIABLE CHARACTER pqf_level_display[NUM_PQF_LEVELS][9]; 
GLOBALVARIABLE CHARACTER pqf_level_list[NUM_PQF_LEVELS][6]; 
#endif

GLOBALVARIABLE CHARACTER pqf_dirname[COMMON_PATH_MAXLEN+1];
GLOBALVARIABLE CHARACTER pqf_filename[COMMON_PATH_MAXLEN+1];

typedef STRUCTURE tagPQFDIALOGSETUP
{
    BOOL pqfdlgsetup_bRunMode;
    BOOL pqfdlgsetup_bRun;
    BOOL pqfdlgsetup_bEdit;
} PQFDIALOGSETUP, FAR *LPPQFDIALOGSETUP;

#endif /* PQF_DEFINED */
