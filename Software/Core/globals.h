/**********************************************************************

GLOBALS.H -- a subset of the global data flags for the appliation.

History:	Starting version, for version 4.00a
			07/19/95	Add IS_CE_BOX for CE40/50 environment.			JFL
			08/30/95	Add CE_EP option.								JFL
			05/13/02	Ported to 32-bit (v5.0)							AAT

**********************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H
#ifndef DOGLOBALS
#define EXTDEF extern
#else
#define EXTDEF
#endif

// string compare helpers
#define IsSame(Str1, Str2)					(lstrcmp(Str1, Str2) == 0) 
#define IsSameNoCase(Str1, Str2)			(lstrcmpi(Str1, Str2) == 0) 

#define SWITCH(a)							{char *_p = a; if (0) {
#define CASE(x)								} else if (lstrcmp(_p, x) == 0) {
#define CASEi(x)							} else if (lstrcmpi(_p, x) == 0) {
#define DEFAULT								} else {
#define ENDSWITCH							}}
 
// Forward definition, so the log.h will have that number defined
#define MAX_DI_STR_LEN				39

#include "log.h"
#include "ilock.h"

// PQFWare specific structures
typedef struct PQF_LIST_ELEMENT_TAG
{
	BYTE	tp_action;
	UINT	tp_duration;		//Also an alias for the address in loop commands
	WORD	tp_start_angle;
	WORD	tp_RMS_limit;		//in thenths of an Amp
	WORD	tp_PEAK_limit;		//in tenths of an Amp.
	PQF_LIST_ELEMENT_TAG() {memset(this, 0, sizeof(PQF_LIST_ELEMENT_TAG));}
} PQFLIST, *LPPQFLIST;

// Stucture defines:
typedef struct _tagKtDeviceInfo 
{
	char	company_name[MAX_DI_STR_LEN + 1];
	char	machine_type[MAX_DI_STR_LEN + 1];
	DWORD	serial_number;
	char	firmware[MAX_DI_STR_LEN + 1];
	float	firm_rev;
	BOOL	three_phase;
	BOOL	eightKV;
	_tagKtDeviceInfo() {memset(this, 0, sizeof(_tagKtDeviceInfo));}
} kt_device_info, *pkt_device_info;

#define MODNAMESIZE  8
#define CALDATASIZE	 49
#define MAX_CPLTYPES 3
#define WAVENAMESIZE 50

// WARNING: In 16-bit app CALINFO structure was packed on 1-byte 
//			boundary, plus, all ints were 16-bit long, not 32. Make
//			sure we are accounting for that, since the old calibration
//			file is a straight memory dump of the structure.
/*
typedef struct CALINFORECORD_TAG
{
  unsigned long type;
  unsigned char id;
  char          name[MODNAMESIZE];
  unsigned long options;
  unsigned char vmeas;
  unsigned char imeas;
  unsigned long serial;
  unsigned int  cal_valid;
  unsigned int  cal[CALDATASIZE];
}CALINFO;
*/

#pragma pack(push)
#pragma pack(1)
typedef struct CALINFORECORD_TAG
{
	DWORD		type;
	BYTE		id;
	char		name[MODNAMESIZE];
	DWORD		options;
	BYTE		vmeas;
	BYTE		imeas;
	DWORD		serial;
	WORD		cal_valid;
	WORD		cal[CALDATASIZE];
} CALINFO;
#pragma pack(pop)

typedef struct WAVE_INFO_TAG
{ 
	int  mod_num;
	int  front;
	int  line_couple;
	int  io_couple;
	int  maxv;
	int  maxv_io;
	int  mint;
	int  mint_io;
	char wave_name[WAVENAMESIZE];
	int  waven;
} WAVEINFO;

typedef struct SRGNET_TAG
{
	int mod_num;		// index in calset
	int first_wave;		// index in wave_list
	int wave_cnt;		// number of waves
} SRGNET;

#define MAX_CAL_SETS		16

EXTDEF WAVEINFO wave_list[80];
EXTDEF CALINFO  calset[MAX_CAL_SETS];
EXTDEF SRGNET   srg_networks[16];
EXTDEF BOOL EFT_EXIST;
EXTDEF int  EFT_COUPLER;
EXTDEF BOOL EFT_THREE_PHASE;
EXTDEF BOOL EFT_2MHZ;
EXTDEF BOOL EFT_8KV;
EXTDEF BOOL EFT_CHIRP;
EXTDEF BOOL DC_SURGE;
EXTDEF BOOL DC_EFT;
EXTDEF BOOL SURGE_EXIST;
EXTDEF int  SURGE_COUPLER;
EXTDEF BOOL PQF_EXIST;
EXTDEF BOOL PQF_AUX;
EXTDEF int  AC_COUPLER;
EXTDEF int  IO_COUPLER;
EXTDEF int  EFT_MODULE;
EXTDEF BOOL SURGE_THREE_PHASE;
EXTDEF BOOL SIMULATION;
EXTDEF BOOL CALIBRATION;
EXTDEF BOOL POWER_UP;
EXTDEF BOOL EFT_SETUP;
EXTDEF BOOL RESET_REQUIRED;
EXTDEF LOG  Log;
EXTDEF ILOCK Ilock;
EXTDEF HWND	  hWndComm;		// handle to the communications	window.
EXTDEF HINSTANCE ghinst;

// CE40/50 additions

EXTDEF BOOL IS_CE_BOX;		// CExx box detected
EXTDEF BOOL CE_EP_OPTION;	// CE EP option enabled
EXTDEF LPSTR DEF_EXT;
EXTDEF LPSTR DEF_FILES;
EXTDEF char	fnamebuf[MAX_PATH];
EXTDEF LPSTR INI_NAME;
EXTDEF BOOL COMMERROR;
EXTDEF BOOL	ERR_AC_POWER; 
EXTDEF BOOL	ERR_AC_LIMIT; 
EXTDEF BOOL	BACK_FROM_LOCAL;
EXTDEF BOOL	AC_OPTION;
EXTDEF int  rms_data[10];
#define NOT_GHOST(x)  (calset[x].type != 0l)

#undef EXTDEF
#endif


