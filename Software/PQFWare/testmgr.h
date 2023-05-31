#ifndef TESTMGR_DEFINED
#define TESTMGR_DEFINED

/**
***********************************************************************

During  operation you will send the following messages:

KT_START_PQF  Starts running the loaded PQF data.
              No Parameters: Returns 0 if successful.

KT_STOP_PQF   Stops running the loaded PQF data.
              No Parameters: Returns 0 if successful.

KT_HOLD_PQF   Pauses the running PQF list.
               No Parameters: Returns -1 if the systems is not running.
                              Returns the step number otherwise.

KT_RESET_PQF  Clears the PQF data list to zero
              No Parameters: Returns 0 if successful.

KT_READ_ILOCK Checks the interlock status of the system.
              This should be called periodically (2/Sec or so)
              No Parameters: Returns 0 if there are no open interlocks
              and returns a LPSTR to the interlock error message otherwise.

KT_ACTIVE_STEP Reports the current running step in the PQF list
               No Parameters: Returns -1 if the systems is not running.
                              Returns the step number otherwise.
           
KT_SET_PQF_LIST  Sets the PQF list to a specified value.
                 Parameters lParam=number of steps.
                            wParam=Global Handle to memory holding the
                                   list. You are responsible for unlocking and 
                                    freeing this.
                  Returns 0 on success 
                  Returns -1 on Communications failure.
                  Returns -2 on list too long.
                                 


The action char has the following format:

    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄEvent/Loop command 0=Event 1= Loop Command
    ³ ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄMode 0=Seconds in tenths,  1=Cycles
    ³ ³     ÚÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÂTap/Type
msb E M x x t t t t             ³   0 =Open
                                ³   1 =Shorted
                                ³   2 =40%
                                ³   3 =50%
                                ³   4 =70%
                                ³   5 =80%
                                ³   6 =90% 
                                ³   7 =100%
                                ³   8 =110%
                                ³   9 =120%
                                ³  10 =150%
                                ³ or Type
                                ³   0 Branch to the step number in 'duration'.
                                ³   1 Branch to the step number in 'duration','RMS_limit' times
                                ³   2 Gosub to the step number in 'duration'.
                                ³   3 Gosub to the step number in 'duration', 'RMS_limit' times
                                ÀÄ  4 Return from the subroutine.
                                    5 Stop.

*************************************************************************
**/
#include "..\core\kt_comm.h"

#define TIMER1							1
#define TESTMGR_EUT_OFF					0
#define TESTMGR_EUT_RDY					1
#define TESTMGR_EUT_ON					2

#define testmgr_bSimulate				SIMULATION
#define testmgr_bCalibrate				CALIBRATION

#define testmgr_hWndComm				hWndComm		/* handle to communication window */
#define testmgr_nIlock					This_State

#define TESTMGR_CALLER_RUNTEST			0
#define TESTMGR_CALLER_TIMER			1
#define TESTMGR_CALLER_EUTPOWER			2
#define TESTMGR_CALLER_INRUSH			3

GLOBALVARIABLE PQFSTATUS testmgr_PqfStatus;
GLOBALVARIABLE BOOL testmgr_bDebug GLOB_INITFALSE;
//GLOBALVARIABLE SHORT testmgr_eut_status; /* applies to the present button, NOT the new status */
#define testmgr_eut_status EUT_POWER
GLOBALVARIABLE BOOL testmgr_bGettingStatus GLOB_INITFALSE;
GLOBALVARIABLE BOOL testmgr_bShowStatus GLOB_INITTRUE;
GLOBALVARIABLE BOOL testmgr_bSeti GLOB_INITTRUE;
GLOBALVARIABLE BOOL testmgr_bEcatCommExit GLOB_INITFALSE; /* TRUE if user want to exit program, after getting ecat comm failure */
GLOBALVARIABLE WORD testmgr_rms_imax; /* times 10    <loword> */
GLOBALVARIABLE WORD testmgr_peak_imax; /* hiword */

#endif /* TESTMGR_DEFINED */
