/***********************************************************************
// Keytek Instrument Corp.
//
//	Private and public messages used by keytek devices for interprocess
// communication.
//
//	File: kt_comm.h header for kt_comm.c.
//			assigns WM_USER #define's from 1000 to 3000 for its own use.
//
// Jake Kirk
// Oct. 8, 1991
	History:
		13 Sep. 95		JFL			KT_SRG_PHASE_MODE added.
		22 Sept 97		FSDiC		Added KT_ILOCK_ABORT_RUN.
************************************************************************/
#ifndef __KT_COMM_H__
#define __KT_COMM_H__

//	Prototypes:
HWND init_kt_comm( HWND, HANDLE);

typedef struct tagPQFSTATUS
{
    __int32 pqfs_nIlock;
    WORD    pqfs_rms_iread;
    WORD    pqfs_peak_iread;
    SHORT   pqfs_step_number;
    BYTE    pqfs_eut_status; /*0 = Off 1=Enabled but not on  2=Fully On*/
    SHORT   pqfs_error;
    WORD    pqfs_line_freq;
    WORD    pqfs_line_voltage;
	tagPQFSTATUS() {memset(this, 0, sizeof(tagPQFSTATUS));}
} PQFSTATUS, *LPPQFSTATUS;

// BOGUS, defines that need a proper home, but are stuck here for now.
#define IDD_TEST_LED					999

// internal private messages for the communications window.
// from device => comm win; comm win => comm win.; and comm win => app win.


// application public messages for 801_5 apps.
// from app => communications win; communication win => communications win.
#define	 KT_DEVICE_STATUS				(WM_USER + 2001)	// to request/send status.
#define	 KT_DEVICE_INFO					(WM_USER + 2002)	// device ID & firmware revision.
#define  KT_EUT_ON						(WM_USER + 2003)
#define  KT_EUT_CHEAT					(WM_USER + 2004)
#define  KT_DEVICE_RESTORE				(WM_USER + 2005)
#define  KT_EUT_OFF						(WM_USER + 2006)
#define  KT_PHASE_ANGLE_SETSTATE		(WM_USER + 2008)
#define  KT_SRG_COUPLE_SETSTATE			(WM_USER + 2009)
#define  KT_SRG_OUTPUT_SETSTATE			(WM_USER + 2010)
#define  KT_SRG_NETWORK_SETSTATE		(WM_USER + 2011)
#define  KT_SRG_WAVE_SETSTATE			(WM_USER + 2012)
#define  KT_SRG_VOLTAGE_SETSTATE		(WM_USER + 2013)
#define  KT_SURGE						(WM_USER + 2014)
#define  KT_ABORT						(WM_USER + 2015)
#define  KT_CHARGE						(WM_USER + 2016)
#define  KT_INITIALIZE					(WM_USER + 2017)
#define  KT_MEAS_SETSTATE				(WM_USER + 2018)
#define  KT_EFT_COUPLE_SETSTATE			(WM_USER + 2019)
#define  KT_EFT_OUTPUT_SETSTATE			(WM_USER + 2020)
#define  KT_EFT_FREQUENCY_SETSTATE		(WM_USER + 2021)
#define  KT_EFT_DURATION_SETSTATE		(WM_USER + 2022)
#define  KT_EFT_VOLTAGE_SETSTATE		(WM_USER + 2023)
#define  KT_EFT_PERIOD_SETSTATE			(WM_USER + 2024)
#define  KT_EFT_INIT					(WM_USER + 2025)
#define  KT_EFT_ON						(WM_USER + 2026)
#define  KT_EFT_OFF						(WM_USER + 2027)
#define  KT_EFT_CHIRP					(WM_USER + 2028)
#define  KT_EFT_SET						(WM_USER + 2029)            
#define  KT_SRG_INIT					(WM_USER + 2030)
#define  KT_EUT_QUERY					(WM_USER + 2031)
#define  KT_SYS_STATUS					(WM_USER + 2032)
#define  KT_EFT_LOG_START				(WM_USER + 2033)
#define  KT_SRG_LOG_START				(WM_USER + 2034)
#define  KT_SRG_RESETALL				(WM_USER + 2035)
#define  KT_EFT_RESETALL				(WM_USER + 2036)
#define  KT_SRG_LOCAL_INIT				(WM_USER + 2037)
#define  KT_ECAT_EXIT					(WM_USER + 2038)
#define  KT_PHASE_MODE					(WM_USER + 2039)
#define  KT_CLAMP_STATE					(WM_USER + 2040)
#define  KT_DATA_ON						(WM_USER + 2041)
#define  KT_CMODE_STATE					(WM_USER + 2042)
#define  KT_SRG_OUTPUT_QUERY			(WM_USER + 2043)
#define  KT_DATA_ON_QUERY				(WM_USER + 2044)
#define  KT_CLAMP_QUERY					(WM_USER + 2045)
#define  KT_SRG_PHASE_MODE				(WM_USER + 2046)
#define  KT_ILOCK_ABORT_RUN				(WM_APP + 2047)

// PQFWare specific messages
#define  KT_PQF_IDLE					(WM_USER + 2048)
#define  KT_PQF_START					(WM_USER + 2049)
#define  KT_PQF_SETV					(WM_USER + 2050)
#define  KT_PQF_READ_DATA				(WM_USER + 2051)
#define  KT_PQF_STOP					(WM_USER + 2052)
#define  KT_PQF_RESET					(WM_USER + 2053)
#define  KT_PQF_STATUS					(WM_USER + 2054)
#define  KT_PQF_SEND_LIST				(WM_USER + 2055)
#define  KT_PQF_GET_ERROR				(WM_USER + 2056)
#define  KT_PQF_QUALIFY					(WM_USER + 2057)
#define  KT_AUX_ON						(WM_USER + 2058)
#define  KT_AUX_OFF						(WM_USER + 2059)
#define  KT_PQF_SET_PHASE				(WM_USER + 2060)
#define  KT_PQF_EXIT					(WM_USER + 2061)

#endif // __KT_COMM_H__
