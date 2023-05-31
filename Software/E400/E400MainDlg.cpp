//////////////////////////////////////////////////////////////////////
// E400MainDlg.cpp: implementation of the CE400MainDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e400.h"
#include "E400App.h"
#include "E400MainDlg.h"
#include "..\core\message.h"
#include "..\core\globals.h"
#include "..\core\comm.h"
#include "..\core\kt_comm.h"
#include "..\core\commi.h"
#include "..\core\pps.h"
#include "..\core\filesdlg.h"
#include "..\core\pause.h"
#include "..\core\calinfo.h"

// Forward declarations
int Block_Changed(void);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CE400MainDlg::CE400MainDlg()
{
	chirp_hide = FALSE;
	Last_State = This_State = (LONG) -1;
	yBrush = NULL;
	Time_elapsed = 0;
	LastTick = 0;
	LastTest = 0;
	nRunMode = IDLE_MODE;		// initialize.
	PAUSE = FALSE;
	IL_PAUSE = FALSE;

	// For backward compatibility with dlgCtl class
	memset(Obj_Name, 0, sizeof(Obj_Name));
	created = FALSE;
}

//////////////////////////////////////////////////////////////////////
CE400MainDlg::~CE400MainDlg()
{
	// For backward compatibility with dlgCtl class
	for (int i=0; i<dlgcnt; i++)
	{
		if (dlglist[i] == this)
		{
			created=FALSE;
			dlglist[i]=0;
			dlgcnt--;
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// NOTE: Return 0 if the message has been processed, 1 for further 
//		 processing by the system
//////////////////////////////////////////////////////////////////////
int CE400MainDlg::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	BOOL rv;
	int i;
	DWORD tdword;
	HWND hWnd = GetHWND();

	switch (uiCtrlId)
	{
		case CMD_PAUSE :
			if (hwndCtl != NULL)    // Sent by Check_Interlocks()
				IL_PAUSE = TRUE;
			else           // Sent by 'retry' in Interlock dlg. proc.
			{
				if (COMMERROR)
				{
					COMMERROR=FALSE;
					PURGE();
					This_State=ECAT_Ilock();
					Ilock.Check_Interlocks(This_State);  // JFL add 7/25/95
				}

				if (RESET_REQUIRED)
				{
					SendMessage(hWndComm, KT_EFT_RESETALL,0, 0L);
					if (EUT_POWER==1)
						Message("Verify Power","Please Cycle the BLUE EUT button,\nand verify that AC power is present at the input to the EFT coupler");
					RESET_REQUIRED=FALSE;
				}//RESET_REQUIRED

				/* If PAUSE is TRUE, then Run mode is paused and the Run Pause dialog
				is active; don't restart EFT bursting (it will be restarted if
				the user pushes RUN in the Run Pause dialog to continue running). */
				if ( ( !PAUSE ) && ( (SET_READY) || (Run_time) ) )
				{
					PrintfDlg(hWnd, IDD_ACTION_TEXT, "Setting Test");
					CEcatApp::BeginWaitCursor();
					if (( rv=SendMessage(hWndComm, KT_EFT_SET, 0, 0L)) != 0)
					{//Bad Run
						CEcatApp::EndWaitCursor();
						Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
						IL_PAUSE=FALSE;
						break;
					}//Bad Run
					else
						SET_READY=TRUE;
					CEcatApp::EndWaitCursor();
				}

				// Don't start bursting if PAUSE is TRUE - see comment above.
				if ( ( !PAUSE ) && ( Run_time ) )
				{ 
					if (( rv=SendMessage(hWndComm, KT_EFT_ON, 0, 0L)) != 0)
					{//Bad Run
						Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
						IL_PAUSE = FALSE;
						break;
					}//Bad Run

					SET_READY=0;
					LastTick = GetTickCount();
				}//Run Time

				IL_PAUSE = FALSE;
			}//Pause ==False
			break;

		case IDD_RUN :
			// Make sure we are not already running:  ignore if so
			if (nRunMode == RUN_MODE) break;

			// Make sure the test time is valid
			char tmpBuf[MAX_PATH];
			GetDlgItemText(GetHWND(), IDD_TOTAL_TEST, tmpBuf, MAX_PATH);
			if (IsSame(tmpBuf, "Time: INVALID"))
			{
				Message("Invalid Time", "Total running time is invalid, please adjust one of the relevant parameters before running the test.");
				break;
			}

			// Test to make sure we can communicate with ECAT before starting
			This_State = ECAT_Ilock();
			if (This_State != ILOCK_OK)
			{
				Ilock.Check_Interlocks(This_State);
				break;
			}

			if (Log.log_on())
			{
				if (!Log.start_log())
					return 0;
				UpdateWindow(hWnd);

				if (!Log.log_on())//It must have been turned off in Start_log
					SendMessage(GetParent(), WM_COMMAND, IDM_LOFF, 0);
				else 
					EnableWindow(GetDlgItem(IDD_ECAT_COMM), TRUE);

			}

			// Disable menu
			SendMessage(GetParent(), WM_ENABLE_RUN, 0, (LPARAM) TRUE);
			PAUSE=TRUE;
			nRunMode = RUN_MODE;
			SendDlgItemMessage(hWnd, IDD_RUN, BM_SETCHECK, 1, 0L);
			EnableWindow(GetDlgItem(IDD_RAW_STOP), TRUE);
			EnableWindow(GetDlgItem(IDD_ECAT_EXIT), FALSE);
			
			// no break required, keep going!

		case IDD_RUN_PREP :
			tdword = SendMessage(Block_Window, CMD_BLOCK_GET_ORDER, 0, 0L);
			//The format of	time is: in Hex	6 digits Volt,Dur,repeat,Cpl,Pol
			for	(i = 0;	i < 8; i++) 
				Levels[i] =	0;
			for	(i = 8;	i > 0; i--) 
			{
				Levels[	(tdword &0x000F) - 1] = i-1;
				tdword /=16;
			}
			steps_taken	= 1;
			PrintfDlg(hWnd, IDD_TOTAL_N, "Tests: %ld of %ld", steps_taken, LastTest);
			changed_while_running=0;
			DurSet.Set_Update(1);
			if (!chirp_hide) 
				FreqSet.Set_Update(1);
			PerSet.Set_Update(1);
			PhaseSet.Set_Update(1);
			PolaritySet.Set_Update(1);
			VoltageSet.Set_Update(1);
			CoupleMode.Set_Update(1);

			if (!chirp_hide)
			{ 
				SendMessage(hWndComm, KT_EFT_CHIRP,	0, 0L);
				msecSet.Set_Update(0);
				FreqSet.Set_Start_Values();
				msecSet.Set_Update(1);
				DurSet.Set_Start_Values();
				msecSet.Set_Start_Values();
			}
			else
				SendMessage(hWndComm, KT_EFT_CHIRP,	1, 0L);

			// JFL 9/15/95 remove an error scenario with illegal :LI:AN
			// transmission.
			// JFL 10/26/95: Change the positioning of the tests and
			// init. calls, moving some back to position before 9/15 change;
			// broke the order and proper setting of "phase_mode" in KT_HIDE.

			if (Phase_Mode)
			{
				SendMessage(hWndComm,KT_PHASE_MODE,0,1L);
				if (CoupleMode.Get_Current_Values() == 0)
				{
					// Add error check case here:  incomplete selection.
					// Prevents :LI:AN illegal command error.  JFL 9/15/95
					Message("Error:  Incomplete Selection", "No coupling mode selected.\nLine sync not meaningful.");
					SendMessage(hWnd, WM_COMMAND, IDD_STOP, 0L);
					break;
				}
				PhaseSet.Set_Start_Values();
			}
			else if (DC_MODE)
				SendMessage(hWndComm,KT_PHASE_MODE,0,4);
			else if (EFT_COUPLER!=-1)
				SendMessage(hWndComm,KT_PHASE_MODE,0,0L);

			PerSet.Set_Start_Values();
			VoltageSet.Set_Start_Values();
			CoupleMode.Set_Start_Values();
			Repeat.Set_Start_Values();
			PolaritySet.Set_Start_Values();

			msecSet.Show_Run(1);
			DurSet.Show_Run(1);

			if (FreqSet.Get_Special_F8014())
			{
				// Switch point changed from 2KV to 4KV.		  JFL 2/23/96

				if (VoltageSet.Get_Current_Values() >= 4000)
					FreqSet.Set_Value(2500);
				else 
					FreqSet.Set_Value(5000);
			}

			if (!chirp_hide) 
				FreqSet.Show_Run(1);
			PhaseSet.Show_Run(1);
			PerSet.Show_Run(1);
			VoltageSet.Show_Run(1);
			CoupleMode.Show_Run(1);
			Repeat.Show_Run(1);
			PolaritySet.Show_Run(1);
			EnableWindow(Block_Window,0);
			Run_time =	Dur_Pair.Value() * Dur_mul;
			Wait_time=0;

			if (Log.log_on())
			{ 
				SaveText(Log.logfile());
				SendMessage(hWndComm,KT_EFT_LOG_START,0,0L);
			}

			if (!SET_READY)
			{
				PrintfDlg(hWnd, IDD_ACTION_TEXT, "Setting Test");
				CEcatApp::BeginWaitCursor();
				if (( rv=SendMessage(hWndComm, KT_EFT_SET, 0, 0L)) != 0)
				{//Bad Run
					CEcatApp::EndWaitCursor();
					Ilock.Check_Interlocks(ILOCK_EFT_BASE + rv);
					break;
				}//Bad Run
				CEcatApp::EndWaitCursor();
			}

			if (( rv=SendMessage(hWndComm, KT_EFT_ON, 0, 0L)) != 0)
			{//Bad Run
				Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
				break;
			}//Bad Run

			PrintfDlg(hWnd, IDD_ACTION_TEXT, "Run:");
			SET_READY=0;
			Time_elapsed=0;
			LastTick = GetTickCount();
			PAUSE=FALSE;
			break;

		case IDD_EUT_POWER:
			if (SIMULATION || (EFT_COUPLER!=-1))
			{ 
				changed_since_save=TRUE;
				if (EUT_POWER==0)
					SendMessage(hWndComm, KT_EUT_ON,	0, 0L);
				else
					SendMessage(hWndComm, KT_EUT_OFF,	0, 0L);

				if (SIMULATION)
				{
					if (EUT_POWER) 
						EUT_POWER=0;
					else 
						EUT_POWER=2;
					SendDlgItemMessage(hWnd, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
				}
			}
			break;

		case IDD_RAW_STOP:
			if (nRunMode ==	IDLE_MODE) return TRUE;
			rv=PAUSE;
			SendMessage(hWndComm, KT_EFT_OFF, 0, 0L);
			SET_READY = FALSE;				// JFL 7/24/95;  Add for 'Not set' bug
			PAUSE=TRUE;

			if (Do_Pause(hWnd))
			{
				PAUSE=rv;
				UpdateWindow(hWnd);
				if (Run_time)
				{
					if (!SET_READY)
					{
						PrintfDlg(hWnd, IDD_ACTION_TEXT, "Setting Test");
						CEcatApp::BeginWaitCursor();
						if (( rv=SendMessage(hWndComm, KT_EFT_SET, 0, 0L)) != 0)
						{//Bad Run
							CEcatApp::EndWaitCursor();
							Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
							break;
						}//Bad Run
						CEcatApp::EndWaitCursor();
					}

					if (( rv=SendMessage(hWndComm, KT_EFT_ON, 0, 0L)) != 0)
					{//Bad Run
						Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
						break;
					}//Bad Run
				}
				return TRUE;
			}
			PAUSE=rv;

		case IDD_STOP :
			SendMessage(hWndComm, KT_EFT_OFF, 0, 0L);
			PrintfDlg(hWnd, IDD_DURATION_TEXT, "Idle");
			PrintfDlg(hWnd, IDD_ACTION_TEXT, "Idle");
			msecSet.Show_Run(0);
			DurSet.Show_Run(0);
			FreqSet.Show_Run(0);
			PerSet.Show_Run(0);
			PhaseSet.Show_Run(0);
			VoltageSet.Show_Run(0);
			CoupleMode.Show_Run(0);
			Repeat.Show_Run(0);
			PolaritySet.Show_Run(0);
			EnableWindow(Block_Window,1);
			nRunMode = IDLE_MODE;
			SendDlgItemMessage(hWnd, IDD_RUN, BM_SETCHECK, 0, 0L);
			//SendDlgItemMessage(hWnd, IDD_STOP, BM_SETCHECK, 1, 0L);
			EnableWindow(GetDlgItem(IDD_RAW_STOP), FALSE);
			EnableWindow(GetDlgItem(IDD_ECAT_EXIT), TRUE);
			PrintfDlg(hWnd, IDD_TOTAL_N, "Tests:%ld", LastTest);
			PrintfDlg(hWnd, IDD_DURATION_TEXT, "Idle");

			/*  Removed -- JFL -- 8/8/95 -- never gets updated
			**		    PrintfDlg(hMainDlg, IDD_BETWEEN_TEXT, "Idle");
			*/
			DurSet.Set_Update(0);
			FreqSet.Set_Update(0);
			PhaseSet.Show_Run(0);
			PerSet.Set_Update(0);
			PhaseSet.Set_Update(0);
			VoltageSet.Set_Update(0);
			CoupleMode.Set_Update(0);
			EnableWindow(GetDlgItem(IDD_ECAT_COMM), FALSE);

			if (Log.log_on()) 
				Log.end_log();

			// Enable menu
			SendMessage(GetParent(), WM_ENABLE_RUN, 0, (LPARAM) FALSE);
			PAUSE = FALSE;
			IL_PAUSE = FALSE;
			COMMERROR=FALSE;
			PURGE();
			break;

		case IDD_RAW_STEP:
			rv=1;
			for (i = 8; (Levels[i] == 0) &&	(i > 0); i--);
			do  
			{
				switch (Levels[i])
				{
				case VBLOCK : //Voltage
					rv = VoltageSet.Set_Next_Step();
					if (rv) 
						VoltageSet.Set_Start_Values();
					if (FreqSet.Get_Special_F8014())
					{
						// Switch point changed from 2KV to 4KV.		  JFL 2/23/96
						if (VoltageSet.Get_Current_Values() >= 4000)
							FreqSet.Set_Value(2500);
						else 
							FreqSet.Set_Value(5000);
					}
					break;

				case DURBLOCK : //Duration
					rv = DurSet.Set_Next_Step();
					if (rv) 
						DurSet.Set_Start_Values();
					break;

				case FREQBLOCK : //Freq
					rv = FreqSet.Set_Next_Step();
					if (rv) 
						FreqSet.Set_Start_Values();
					break;

				case PERBLOCK : //Period
					if (Phase_Mode)
					{ 
						rv = PhaseSet.Set_Next_Step();
						if (rv) 
							PhaseSet.Set_Start_Values();
					}
					else
					{ 
						rv = PerSet.Set_Next_Step();
						if (rv) 
							PerSet.Set_Start_Values();
					}
					break;

				case RBLOCK : //Repeat
					rv = Repeat.Set_Next_Step();
					if (rv) 
						Repeat.Set_Start_Values();
					break;

				case CBLOCK ://Couple Mode
					rv = CoupleMode.Set_Next_Step();
					if (rv) 
						CoupleMode.Set_Start_Values();
					break;

				case POBLOCK : //Polarity 
					rv = PolaritySet.Set_Next_Step();
					if (rv) 
						PolaritySet.Set_Start_Values();
					break;
				} // switch Levels

				if (rv) i--;
			}
			while ((rv != 0) && (i >= 0));

			if (rv)	
			{
				nRunMode = IDLE_MODE;
				Log.loglpstr("Test Complete\n");
				SendMessage(hWnd, WM_COMMAND, IDD_STOP, 0L);
				//SendDlgItemMessage(hWnd, IDD_RUN, BM_SETCHECK, 0, 0L);
				//SendDlgItemMessage(hWnd, IDD_STOP, BM_SETCHECK, 1, 0L);
				//EnableWindow(GetDlgItem(IDD_STOP), FALSE);
				//EnableWindow(GetDlgItem(IDD_ECAT_EXIT), TRUE);
				MessageBox(hWnd, "The selected test has been completed. Press RUN button to repeat the test.", "Test Complete", MB_OK | MB_ICONINFORMATION);
				PrintfDlg(hWnd, IDD_ACTION_TEXT, "");
			}
			return (rv);

		case IDD_ECAT_COMM:
			Log.comment((LPSTR) NULL);
			break;

		case IDD_WAIT_CB :
			switch (SendDlgItemMessage(hWnd, IDD_WAIT_CB, CB_GETCURSEL,	0, 0L))
			{
			case 0 : 
				if (Wait_mul==1) 
					break;
				Wait_mul = 1;
				Wait_Pair.init(hWnd, IDD_WAIT_SB, IDD_WAIT_EB, 1, 360, 1, 0, 1, RGB(255, 255, 0), "");
				break;

			case 1 : 
				if (Wait_mul==60) 
					break;
				Wait_mul = 60;
				Wait_Pair.init(hWnd, IDD_WAIT_SB, IDD_WAIT_EB, 1, 240, 1, 0, 1, RGB(255, 255, 0), "");
				break;

			case 2 : 
				if (Wait_mul==3600) 
					break;
				Wait_mul = 3600;
				Wait_Pair.init(hWnd, IDD_WAIT_SB, IDD_WAIT_EB, 1, 24, 1, 0, 1, RGB(255, 255, 0), "");
				break;
			} //SendDlgItemMessage
			return TRUE;

		case IDD_WAITD_CB :
			switch (SendDlgItemMessage(hWnd, IDD_WAITD_CB, CB_GETCURSEL,	0, 0L))
			{
			case 0 : 
				if (Dur_mul==1) break;
				Dur_mul = 1;
				Dur_Pair.init(hWnd, IDD_WAITD_SB, IDD_WAITD_EB, 1, 360, 1, 0, 1, RGB(0, 255, 0), "");
				break;

			case 1 : 
				if (Dur_mul==60) break;
				Dur_mul = 60;
				Dur_Pair.init(hWnd, IDD_WAITD_SB, IDD_WAITD_EB, 1, 240, 1, 0, 1, RGB(0, 255, 0), "");
				break;

			case 2 : 
				if (Dur_mul==3600) break;
				Dur_mul = 3600;
				Dur_Pair.init(hWnd, IDD_WAITD_SB, IDD_WAITD_EB, 1, 24, 1, 0, 1, RGB(0, 255, 0), "");
				break;
			} //SendDlgItemMessage
			return TRUE;

		case IDD_WAIT_EB : 
			return (Wait_Pair.Scroll_Proc(WM_COMMAND, MAKEWPARAM(uiCtrlId, uiNotify), (LPARAM) hwndCtl));

		case IDD_WAITD_EB : 
			return (Dur_Pair.Scroll_Proc(WM_COMMAND, MAKEWPARAM(uiCtrlId, uiNotify), (LPARAM) hwndCtl));


		default:
			return CEcatMainDlg::OnCmdMsg(uiNotify, uiCtrlId, hwndCtl);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
BOOL CE400MainDlg::OnInitDialog(HWND hWnd)
{
	LONG j;
	static int error_cnt;

	// If the base class returns FALSE, exit - some kind of error
	if (!CEcatMainDlg::OnInitDialog(hWnd))
		return TRUE;

	// For backward compatibility with dlgCtl class
	hDlgChild = hWnd;
	strcpy(Obj_Name, theApp.GetAppName());
	error_cnt = 0;

	// Subclass buttons

	// EXIT
	UINT arrIds[MAX_BTN_STATES];
	arrIds[0] = IDD_ECAT_EXIT;
	m_btnExit.Subclass(hWnd, IDD_ECAT_EXIT, GetInst(), arrIds, 1);

	// STOP			
	arrIds[0] = IDD_RAW_STOP;
	//arrIds[1] = IDD_RAW_STOP_ON;
	m_btnStop.Subclass(hWnd, IDD_RAW_STOP, GetInst(), arrIds, 1);

	// RUN
	arrIds[0] = IDD_RUN;
	arrIds[1] = IDD_RUN_ON;
	m_btnRun.Subclass(hWnd, IDD_RUN, GetInst(), arrIds, 2);
	
	// EUT
	arrIds[0] = IDD_EUT_POWER;
	arrIds[1] = IDD_EUT_POWER_RDY;
	arrIds[2] = IDD_EUT_POWER_ON;
	m_btnEut.Subclass(hWnd, IDD_EUT_POWER, GetInst(), arrIds, 3);

	///////////////////////////////////////////////
	// Create the sets:
	// DUALS:	
	//	VoltageSet
	//  DurSet
	//  FreqSet
	//	PhaseSet
	//	PerSet
	//
	// POLARITY:
	//	PolaritySet
	///////////////////////////////////////////////

	// Get Pulse frame rect
	RECT rect, oldrect;
	HWND hPulse = GetDlgItem(IDC_MAINFRM_BASE);
	GetWindowRect(hPulse, &rect);
	ShowWindow(hPulse, SW_HIDE);
	S2C(hWnd, &rect);

	j = EFT_8KV ? 8000 : 4400;
	VoltageSet.Init(ghinst, hWnd, rect, CE400App::pvicon, 200, j, 1000, -1, 10, "Voltage", "V");
	PolaritySet.Init(ghinst,hWnd,VoltageSet,CE400App::pvicon,CE400App::mvicon,CE400App::vicon,"Polarity");
	rect.top += 2;

	DurSet.Init(ghinst, hWnd, rect, CE400App::duricon, 1, 20, 15, 0, 5, "Duration", "mS");
	rect.top += 2;
	j = (EFT_2MHZ) ? 2000 : 1000;

	FreqSet.Init(ghinst, hWnd, rect, CE400App::freqicon, 1000, 10000, 1000, 0, 10, "Frequency", "Hz");
	rect.top += 2;
	FreqSet.Set_Special_F8014(TRUE,EFT_CHIRP);

	oldrect=rect;
	PhaseSet.Init(ghinst, hWnd, oldrect, CE400App::phaseicon, 0, 360, 0, 0, 5,	"Phase", "dg");
	PhaseSet.ResetList("Sync:");
	PhaseSet.AddToList("Time");
	if (EFT_COUPLER != -1)	PhaseSet.AddToList("Line");
	if (DC_EFT) PhaseSet.AddToList("DC");
	PhaseSet.Notify(IDD_PHASE_MODE, (long)hWnd);
	PhaseSet.Hide(TRUE);

	PerSet.Init(ghinst, hWnd, rect, CE400App::pericon, 300,5000 , 300, -1, 5, "Period", "ms");
	rect.top += 2;
	PerSet.ResetList("Sync:");
	PerSet.AddToList("Time");
	if (EFT_COUPLER != -1) 
		PerSet.AddToList("Line");
	if (DC_EFT) 
		PerSet.AddToList("DC");
	PerSet.Notify(IDD_PHASE_MODE, (long)hWnd);

	/* PPS 421 */
	DurSet.Set_Max_Value(MaxDuration(0,VoltageSet.Get_Max_Value(),
	FreqSet.Get_Min_Value(),PerSet.Get_Min_Value()));
	/* PPS 421 */

	if (EFT_COUPLER==99) Choose_EFT_Coupler(hWnd);
	if (EFT_MODULE==99) Choose_EFT_Module(hWnd);

	AC_COUPLER = EFT_COUPLER;
	IO_COUPLER = -1;

	CoupleMode.Set_Both_Modes(FALSE);
	CoupleMode.Init(ghinst, hWnd, rect, CE400App::swicon, "Coupling",EFT_THREE_PHASE );

	rect.top  += 25;
	rect.left += 38;
	AcmeasSet.Init(ghinst, hWnd,rect,"AcMeasurements",EFT_COUPLER);

	rect.top += 2;
	msecSet.Init(ghinst, hWnd, DurSet,FreqSet,"Units");
	CoupleMode.Set_Message( KT_EFT_COUPLE_SETSTATE     ,hWndComm);
	PolaritySet.Set_Message( KT_EFT_VOLTAGE_SETSTATE    ,hWndComm);
	msecSet.Set_Message(KT_EFT_DURATION_SETSTATE,KT_EFT_FREQUENCY_SETSTATE,hWndComm);
	PerSet.Set_Message( KT_EFT_PERIOD_SETSTATE    ,hWndComm);
	PhaseSet.Set_Message( KT_PHASE_ANGLE_SETSTATE,hWndComm);

	Wait_Pair.init(hWnd, IDD_WAIT_SB, IDD_WAIT_EB, 5, 360, 10, 0, 1, RGB(255, 255, 0), "");

	Dur_Pair.init(hWnd, IDD_WAITD_SB, IDD_WAITD_EB, 1, 360, 1, 0, 1, RGB(0, 255, 0), "");

	// Fill in Time Between Test combo
	SendDlgItemMessage(hWnd, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Sec"));
	SendDlgItemMessage(hWnd, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Min"));
	SendDlgItemMessage(hWnd, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Hr"));
	SendDlgItemMessage(hWnd, IDD_WAIT_CB, CB_SETCURSEL,	0, 0L);
	SendMessage(hWnd,WM_COMMAND,MAKELONG(IDD_WAIT_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAIT_CB));

	// Fill in Duration Of Tests combo
	SendDlgItemMessage(hWnd, IDD_WAITD_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Sec"));
	SendDlgItemMessage(hWnd, IDD_WAITD_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Min"));
	SendDlgItemMessage(hWnd, IDD_WAITD_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Hr"));
	SendDlgItemMessage(hWnd, IDD_WAITD_CB, CB_SETCURSEL,	0, 0L);
	SendMessage(hWnd,WM_COMMAND,MAKELONG(IDD_WAITD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAITD_CB));

	// Hide Comm button
	EnableWindow(GetDlgItem(IDD_ECAT_COMM), FALSE);

	// Setup Order block window
	HWND hOrder = GetDlgItem(IDC_ORDER_BASE);
	GetWindowRect(hOrder, &rect);
	ShowWindow(hOrder, SW_HIDE);
	S2C(hWnd, &rect);
	Block_Window = CreateWindow("BlockClass", "BlockWindow", WS_CHILD, rect.left, rect.top, 
		rect.right - rect.left, rect.bottom - rect.top, hWnd, NULL, ghinst, 0L);

	SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::pvicon, CE400App::pvcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, DURBLOCK,	MAKELONG(CE400App::duricon,	CE400App::durcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, FREQBLOCK,	MAKELONG(CE400App::freqicon,	CE400App::freqcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, PERBLOCK,	MAKELONG(CE400App::pericon,	CE400App::percursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, CBLOCK,	MAKELONG(CE400App::swicon, CE400App::swcursor));
	SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);

	ShowWindow(Block_Window, SW_SHOWNORMAL);

	// Place REPEAT control below "ORDER" block 
	HWND hRepeat = GetDlgItem(IDC_REPEAT_BASE); 
	GetWindowRect(hRepeat, &rect);
	ShowWindow(hRepeat, SW_HIDE); 
	S2C(hWnd, &rect);
	Repeat.Init(ghinst, hWnd, rect, "Repeat");

	// Check the buttons
	SendDlgItemMessage(hWnd, IDD_RUN, BM_SETCHECK, 0, 0L);
	//SendDlgItemMessage(hWnd, IDD_STOP, BM_SETCHECK, 1, 0L);
	EnableWindow(GetDlgItem(IDD_RAW_STOP), FALSE);

	dcnt = DurSet.ReportSteps();
	fcnt = FreqSet.ReportSteps();
	phcnt=PhaseSet.ReportSteps();
	pcnt = PerSet.ReportSteps();
	vcnt = VoltageSet.ReportSteps();
	swcnt = CoupleMode.ReportSteps();
	repcnt= Repeat.ReportSteps();
	polcnt= PolaritySet.ReportSteps();

	// Prepare for the timer updates
	Wait_mul = 1;
	Dur_mul	= 1;
	nRunMode=IDLE_MODE;
	SET_READY=FALSE;
	LastTick = GetTickCount();
	change_shown=-2;
	changed_since_save=FALSE;

	// dlgCtl compatibility
	created = TRUE;
	
	CoupleMode.SetFrontName("E4xx Out 1" );
	if (EFT_COUPLER!=-1)
		CoupleMode.SetFrontOnly(0);
	else 
	{
		CoupleMode.SetFrontOnly(1);
		ShowWindow(GetDlgItem(IDD_EUT_POWER), SW_HIDE);
		ShowWindow(GetDlgItem(IDD_EUT_GB),    SW_HIDE);
	}

	// Update EUT power button
	if (/*(EFT_COUPLER!=-1) && */ (!SIMULATION))
	{
		SendMessage(hWndComm, KT_EFT_INIT,0, 0L);
		if (SendMessage(hWndComm, KT_EUT_QUERY,0, 0L))
			EUT_POWER=SendMessage(hWndComm, KT_EUT_QUERY,0, 0L);
		SendDlgItemMessage(hWnd, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
	}

	// make it a virgin start, or open the file, if supplied
	if (theApp.IsDdeFile())
	{
		strcpy(fnamebuf, theApp.GetDdeFileName());
		OpenFile();
	}
	else
		DoFileNew();

	// Start the timer
	if (!StartTimer(TIMER2, 500))
		MessageBox(GetActiveWindow(), "Not Enough Timers", "Not Enought Timers", MB_OK);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CE400MainDlg::OnTimer(UINT uiTimerId)
{
	if (uiTimerId != TIMER2)
		return;

	//DBGOUT("In OnTimer(). Line: %d. nRunMode: %d\n", __LINE__, nRunMode);

	BOOL rv;
	char gStr0[100];

	if (change_shown<0)
	{
		change_shown++;
		if (change_shown==0)
		{
			changed_since_save=0;
			change_shown=1;
		}
	}
	else 
		change_shown=changed_since_save;

	if (!SIMULATION)
	{
		Time_elapsed=SendMessage(hWndComm, KT_SYS_STATUS,0, 0L);
		This_State = LOWORD(Time_elapsed);

		if (HIWORD(Time_elapsed)!= EUT_POWER)
		{
			EUT_POWER=HIWORD(Time_elapsed);
			SendDlgItemMessage(GetHWND(), IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
		}

		AcmeasSet.UpdateNumbers();
		if (BACK_FROM_LOCAL)
		{
			BACK_FROM_LOCAL = FALSE;
			AcmeasSet.UpdateEutSource();
		}

		if (nRunMode==RUN_MODE)
		{
			if (This_State!=Last_State)
			{ 
				Ilock.Check_Interlocks(This_State);
				Last_State=This_State;
			}
		}
	}/*Real Stuff */

	if ( PAUSE || IL_PAUSE )
	{
		LastTick=GetTickCount();
		//DBGOUT("In OnTimer(). LastTick: %d. PAUSE: %d; IL_PAUSE: %d.\n", __LINE__, LastTick, PAUSE, IL_PAUSE);
		return;
	}

	Time_elapsed = GetTickCount();
	if (Time_elapsed < LastTick) LastTick =	0;
	//DBGOUT("In OnTimer(). Time_elapsed: %d.\n", __LINE__, Time_elapsed);

	LONG time = (Time_elapsed - LastTick);
	LastTick = Time_elapsed;
	Time_elapsed = time / 1000;
	//Seconds since time began
	LastTick -= (time % 1000);
	//Adjust for portions of seconds not used.
	if ((Time_elapsed) && (nRunMode==RUN_MODE))
	{
		if (Run_time)
		{
			Run_time-=Time_elapsed;
			if (Run_time<=0)
			{
				//Run Time < 0
				Run_time=0;
				Wait_time=(Wait_Pair.Value() * Wait_mul);
				SendMessage(hWndComm, KT_EFT_OFF, 0, 0L);
				SendMessage(GetHWND(), WM_COMMAND, IDD_RAW_STEP, 0L);
				PrintfDlg(GetHWND(), IDD_ACTION_TEXT, "Wait: %ld",Wait_time);
				Time_elapsed=0;
				LastTick=GetTickCount();
			}//Run Time <0
			else
			{
				//Update Run Action Text
				PrintfDlg(GetHWND(), IDD_ACTION_TEXT, "Run: %ld",Run_time);
			}//Update Run Action Text
		}//Run_Time =0
		else
		if ( !PAUSE && !IL_PAUSE )
		{
			//NOT PAUSE and NOT IL_PAUSE
			Wait_time-=Time_elapsed;
			if (Wait_time<=0 && SET_READY == TRUE)	// JFL: add SET_READY test for "not set" bug
			{//
				Wait_time=0;
				Run_time =	Dur_Pair.Value() * Dur_mul;
				SET_READY=FALSE;
				if (( rv=SendMessage(hWndComm, KT_EFT_ON, 0, 0L)) != 0)
				{//Bad Run
					Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
				}//Bad Run
				else
				{//Good Run
					PrintfDlg(GetHWND(), IDD_ACTION_TEXT, "Run: %ld",Run_time);
					steps_taken++;
					if (changed_while_running)
						PrintfDlg(GetHWND(),	IDD_TOTAL_N, "Tests: %ld of ???", steps_taken);
					else 
						PrintfDlg(GetHWND(),	IDD_TOTAL_N, "Tests: %ld of %ld", steps_taken, LastTest);
				}//Good Run
			}//
			else
			{//Update Wait Action Text
				PrintfDlg(GetHWND(), IDD_ACTION_TEXT, "Wait: %ld",Wait_time);
				if ((Wait_time<=SETUPTIME) && (!SET_READY))
				{
					PrintfDlg(GetHWND(), IDD_ACTION_TEXT, "Setting Test");
					CEcatApp::BeginWaitCursor();
					if (( rv=SendMessage(hWndComm, KT_EFT_SET, 0, 0L)) != 0)
					{//Bad Run 
						CEcatApp::EndWaitCursor();
						Ilock.Check_Interlocks(ILOCK_EFT_BASE+rv);
					}//Bad Run
					else  
						SET_READY=TRUE;
					CEcatApp::EndWaitCursor();
				}//EFT_SET
			}//Wait Action Text
		}//NOT PAUSE and NOT IL_PAUSE
	}/*Time Elapsed */


	if (Block_Changed()) 
		changed_since_save=TRUE;

	if (DurSet.changed()) 
	{
		changed_since_save=TRUE;
		dcnt = DurSet.ReportSteps();
	}

	if (PhaseSet.changed()) 
	{
		changed_since_save=TRUE;
		phcnt = PhaseSet.ReportSteps();
	}
	BOOL check_dur_limit = FALSE;

	if ((FreqSet.changed())|| msecSet.changed()) 
	{
		changed_since_save=TRUE;
		check_dur_limit = TRUE;

		fcnt = FreqSet.ReportSteps();

		if (FreqSet.Get_Chirp_F8014())
		{  
			chirp_hide=TRUE;
			DurSet.Hide(TRUE);
		}
		else if (chirp_hide)
		{  
			DurSet.Hide(FALSE);
			chirp_hide=FALSE;
		}
		if (FreqSet.Get_Special_F8014())
			msecSet.mSec_Only(1);
		else 
			msecSet.mSec_Only(0);

		if (msecSet.Value())//is pulses
		{
			float f = FreqSet.Get_Max_Value();

			if (f > 1000.0)
				VoltageSet.Set_Max_Value(3000.0);
			else if (!EFT_8KV || f > 250.0)
				VoltageSet.Set_Max_Value(4400.0);
			else if (EFT_8KV)
				VoltageSet.Set_Max_Value(8000.0);
		}
	} // FreqSet || msecSet

	if (PerSet.changed()) 
	{
		changed_since_save=TRUE;
		check_dur_limit = TRUE;
		pcnt = PerSet.ReportSteps();
	} // PerSet

	if (VoltageSet.changed()) 
	{
		changed_since_save=TRUE;
		check_dur_limit = TRUE;
		vcnt = VoltageSet.ReportSteps();
		if (FreqSet.Get_Special_F8014())
		{
			// Test changed to switch freq. at 4KV, not 2KV.  JFL 2/23/96
			if (VoltageSet.Get_Max_Value() >= 4000)
				FreqSet.Set_Value(2500);
			else 
				FreqSet.Set_Value(5000);
		}
	} // VoltageSet

	/* PPS 421 */
	if (check_dur_limit)
	{
		float max_dur =	MaxDuration(msecSet.Value(),VoltageSet.Get_Max_Value(),	FreqSet.Get_Min_Value(),PerSet.Get_Min_Value());
		DurSet.Set_Max_Value(max_dur);
	}
	/* end PPS 421 */

	if (CoupleMode.changed()) 
	{
		changed_since_save=TRUE;
		swcnt = CoupleMode.ReportSteps();
		if (CoupleMode.FrontPanel() && Phase_Mode)
		SendMessage(GetHWND(),IDD_PHASE_MODE,0,0);
	} // CoupleMode

	if (AcmeasSet.changed())
		changed_since_save=TRUE;

	if (Repeat.changed())
	{  
		repcnt=Repeat.ReportSteps();
		changed_since_save=TRUE;
		if (!Repeat.fixed())
		{  
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,MAKELONG(CE400App::repicon,CE400App::repcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, RBLOCK, 0);
		}
		else 
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);
	}

	double dTime = vcnt*swcnt*polcnt*repcnt*dcnt*fcnt*pcnt*phcnt;
	if (dTime > (double) 0x7FFFFFF)
		time = 0x7FFFFFF;
	else
		time = vcnt*swcnt*polcnt*repcnt*dcnt*fcnt*pcnt*phcnt;
	
	//Alternating Polarity
	if (nRunMode==IDLE_MODE)
	{
		/*Not in Run mode */
		if (time != LastTest)
			PrintfDlg(GetHWND(), IDD_TOTAL_N, "Tests:%ld", time);

		LastTest = time;
		total_steps=time;
		dTime *= (Wait_Pair.Value() * Wait_mul)+(Dur_Pair.Value()*Dur_mul);
		if (dTime > (double) 0x7FFFFFF)
		{
			time = 0x7FFFFFF;
			PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Time: INVALID");
			LastTime = time;
		}
		else
		{
			time *=	(Wait_Pair.Value() * Wait_mul)+(Dur_Pair.Value()*Dur_mul);
			if (time != LastTime)
			{
				Convert_Seconds(time, gStr0);
				PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Time: %s ", gStr0);
				LastTime = time;
			}
		}
	} 
	else
	{/* In run mode */
		if (time != LastTest) 
		{
			changed_while_running=TRUE;
			PrintfDlg(GetHWND(),	IDD_TOTAL_N, "Tests: %ld of ???", steps_taken);
		}
		
		if (!changed_while_running)
		{ 
			time-=steps_taken;
			time *=	(Wait_Pair.Value() * Wait_mul)+(Dur_Pair.Value()*Dur_mul);
			if (time != LastTime)
			{
				Convert_Seconds(time, gStr0);
				PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Left: %s ", gStr0);
				LastTime = time;
			}
		}
		else 
			PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Time:Changed ", gStr0);
	}/*Time report in run mode */

	if (vfix^VoltageSet.fixed())
	{
		vfix = VoltageSet.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, VBLOCK, vfix);
	}

	if (PolaritySet.changed())
	{   
		pol_type = PolaritySet.Value();
		switch (pol_type)
		{
		case 0 :       //Positive polarity
			//CoupleMode.Change_Icon(CE400App::swicon);
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::pvicon, CE400App::pvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;
		case 1 :       //Negative Polarity
			//CoupleMode.Change_Icon(CE400App::mswicon);
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::mvicon, CE400App::mvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;
		case 2 :       //Alternating Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::vicon,	CE400App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE400App::vpicon, CE400App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;
		case 3 :       //Alternating Polarity 5 Ea
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::vicon,	CE400App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE400App::vpicon, CE400App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;
		}

		polcnt=PolaritySet.ReportSteps();
	} // PolaritySet


	if (dfix^DurSet.fixed())
	{
		dfix = DurSet.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, DURBLOCK, dfix);
	}

	if (ffix^FreqSet.fixed())
	{
		ffix = FreqSet.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, FREQBLOCK, ffix);
	}

	if (pfix^PerSet.fixed())
	{
		pfix = PerSet.fixed();
		if (!Phase_Mode) 
			SendMessage(Block_Window, CMD_BLOCK_FIXED, PERBLOCK, pfix);
	}

	if (phfix^PhaseSet.fixed())
	{
		phfix = PhaseSet.fixed();
		if (Phase_Mode) 
			SendMessage(Block_Window, CMD_BLOCK_FIXED, PERBLOCK, phfix);
	}

	if (swfix^CoupleMode.fixed())
	{
		swfix = CoupleMode.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, CBLOCK, swfix);
	}
}

//////////////////////////////////////////////////////////////////////
void CE400MainDlg::DeleteContext()
{
	// Called to clear the data from DoFileNew and if open file fails
	Clear();
}

//////////////////////////////////////////////////////////////////////
void CE400MainDlg::PostCalibrate()
{
	AcmeasSet.Update(EFT_COUPLER);
}

//////////////////////////////////////////////////////////////////////
void CE400MainDlg::Clear()
{
	DurSet.Clear();
	FreqSet.Clear();
	PhaseSet.Clear();
	PerSet.Clear();
	VoltageSet.Clear();
	CoupleMode.Clear();
	Repeat.Clear();
	PolaritySet.Clear();
	msecSet.Clear();
	AcmeasSet.Clear();
	Tname.Clear();
	PhaseSet.Hide(TRUE);
	dcnt = DurSet.ReportSteps();
	fcnt = FreqSet.ReportSteps();
	phcnt=PhaseSet.ReportSteps();
	pcnt = PerSet.ReportSteps();
	vcnt = VoltageSet.ReportSteps();
	swcnt = CoupleMode.ReportSteps();
	repcnt= Repeat.ReportSteps();
	polcnt= PolaritySet.ReportSteps();
	Wait_Pair.init(GetHWND(), IDD_WAIT_SB, IDD_WAIT_EB, 5, 360, 10, 0, 1, RGB(255, 255, 0), "");
	Dur_Pair.init(GetHWND(), IDD_WAITD_SB, IDD_WAITD_EB, 1, 360, 1, 0, 1, RGB(0, 255, 0), "");
	SendDlgItemMessage(GetHWND(), IDD_WAITD_CB, CB_SETCURSEL,	0, 0L);
	SendDlgItemMessage(GetHWND(), IDD_WAIT_CB, CB_SETCURSEL,	0, 0L);
	SendMessage(GetHWND(), IDD_PHASE_MODE,0, 0L);

	SendMessage(Block_Window, CMD_BLOCK_FIXED, VBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, CBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, FREQBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, PERBLOCK, 1);
	SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);
	SendMessage(Block_Window,CMD_BLOCK_SET,POBLOCK,0L);

	EUT_POWER=0;
	SendDlgItemMessage(GetHWND(), IDD_EUT_POWER, BM_SETCHECK,EUT_POWER,0l);
	CoupleMode.Change_Icon(CE400App::swicon);
	Wait_mul	= 1;
	Dur_mul	= 1;
	DC_MODE=FALSE;
}

//////////////////////////////////////////////////////////////////////
int CE400MainDlg::SaveText(HANDLE hfile)
{
	char buffer[80];
	long templ;
	int i;
	Tname.GetText(buffer);
	if (*buffer)
		fhprintf(hfile,"%Fs:Name:%s\r\n",(LPSTR)Obj_Name,buffer);
	else
		fhprintf(hfile,"%Fs:Name:\r\n",(LPSTR)Obj_Name);

	CoupleMode.SaveText(hfile);
	VoltageSet.SaveText(hfile);
	PolaritySet.SaveText(hfile);
	msecSet.SaveText(hfile);
	FreqSet.SaveText(hfile);
	PerSet.SaveText(hfile);
	PhaseSet.SaveText(hfile);
	DurSet.SaveText(hfile);
	Repeat.SaveText(hfile);
	AcmeasSet.SaveText(hfile);

	fhprintf(hfile,"%Fs:Wait time  %d",(LPSTR)Obj_Name,(int)Wait_Pair.Value());
	switch (SendDlgItemMessage(GetHWND(), IDD_WAIT_CB, CB_GETCURSEL,	0, 0L))
	{
	case 0: 
		if (Wait_Pair.Value()>1) fhprintf(hfile," Seconds \r\n");
		else                     fhprintf(hfile," Second  \r\n");
		break;
	case 1: 
		if (Wait_Pair.Value()>1) fhprintf(hfile," Minutes \r\n");
		else                     fhprintf(hfile," Minute  \r\n");
		break;
	case 2 : 
		if (Wait_Pair.Value()>1) fhprintf(hfile," Hours \r\n");
		else                     fhprintf(hfile," Hour  \r\n");
		break;
	}

	fhprintf(hfile,"%Fs:Duration time  %d",(LPSTR)Obj_Name,(int)Dur_Pair.Value());
	switch (SendDlgItemMessage(GetHWND(), IDD_WAITD_CB, CB_GETCURSEL,	0, 0L))
	{
	case 0 : 
		if (Dur_Pair.Value()>1) fhprintf(hfile," Seconds \r\n");
		else                     fhprintf(hfile," Second  \r\n");
		break;
	case 1 : 
		if (Dur_Pair.Value()>1) fhprintf(hfile," Minutes \r\n");
		else                     fhprintf(hfile," Minute  \r\n");
		break;
	case 2 : 
		if (Dur_Pair.Value()>1) fhprintf(hfile," Hours \r\n");
		else                     fhprintf(hfile," Hour  \r\n");
		break;
	}

	if (EUT_POWER) 
		fhprintf(hfile,"%Fs:EUT power:ON \r\n",(LPSTR)Obj_Name);
	else 
		fhprintf(hfile,"%Fs:EUT power:OFF \r\n",(LPSTR)Obj_Name);

	if (Phase_Mode) 
		fhprintf(hfile,"%Fs:Phase Mode Phase\r\n",(LPSTR)Obj_Name);
	else if (DC_MODE)
		fhprintf(hfile,"%Fs:Phase Mode DC Period\r\n",(LPSTR)Obj_Name);
	else 
		fhprintf(hfile,"%Fs:Phase Mode Period\r\n",(LPSTR)Obj_Name);

	templ= SendMessage(Block_Window, CMD_BLOCK_GET_ORDER, 0, 0L);
	if (!templ) 
		fhprintf(hfile,"%Fs:Order:Fixed\r\n",(LPSTR)Obj_Name);
	else
	{
		int ar[16];
		for (i=0; i<16; i++)ar[i]=0;
		for (i=0; i<8; i++)
		{ 
			ar[(templ>>24)& 0x0F]=i+1;
			templ=(templ<<4)& 0x0FFFFFFF;
		}

		i=1;
		fhprintf(hfile,"%Fs:Order:",(LPSTR)Obj_Name);
		while (ar[i])
		{ 
			switch(ar[i])
			{  
			case VBLOCK:		fhprintf(hfile,"Voltage");		break;
			case FREQBLOCK:		fhprintf(hfile,"Frequency");	break;
			case DURBLOCK:		fhprintf(hfile,"Duration");		break;
			case PERBLOCK:		fhprintf(hfile,"Period");		break;
			case RBLOCK:		fhprintf(hfile,"Repeat");		break;
			case CBLOCK:		fhprintf(hfile,"Coupling");		break;
			case POBLOCK:		fhprintf(hfile,"Polarity");		break;
			default:			fhprintf(hfile,"Unknown?");		break;
			}//Switch

			i++; 
			if (ar[i]) 
				fhprintf(hfile,",");
		}//While

		fhprintf(hfile,"\r\n");
	}//Else

	return 1;
}

//////////////////////////////////////////////////////////////////////
int CE400MainDlg::Process_Line(LPSTR buffer)
{
	int tempi;
	char lbuff[80];
	if (_fstrstr(buffer,"Order:Fixed")==buffer)
		return 0;
	
	if (_fstrstr(buffer,"Order:")==buffer)
	{
		long n;
		long tv;
		LPSTR tok;
		LPSTR buf;
		n=1;
		tv=0;
		switch (PolaritySet.Value())
		{
		case 0 :       //Positive polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::pvicon, CE400App::pvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 1 :       //Negative Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::mvicon, CE400App::mvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 3:
		case 2 :       //Alternating Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE400App::vicon,	CE400App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE400App::vpicon, CE400App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;
		}

		if (!Repeat.fixed())
		{
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,MAKELONG(CE400App::repicon,CE400App::repcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, RBLOCK, 0);
		}
		else 
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);

		buf=buffer+6;
		tok=_fstrtok(buf,",\r");
		while(tok)
		{
			if (_fstrstr(tok,"Voltage")) tv|=(n<<((7-VBLOCK)*4));  
			else
			if (_fstrstr(tok,"Frequency"))   tv|=(n<<((7-FREQBLOCK)*4));
			else
			if (_fstrstr(tok,"Duration"))tv|=(n<<((7-DURBLOCK) *4));
			else
			if (_fstrstr(tok,"Period"))tv|=(n<<((7-PERBLOCK) *4));
			else
			if (_fstrstr(tok,"Repeat"))  tv|=(n<<((7-RBLOCK) *4));
			else
			if (_fstrstr(tok,"Coupling"))tv|=(n<<((7-CBLOCK) *4));
			else
			if (_fstrstr(tok,"Polarity"))tv|=(n<<((7-POBLOCK)*4));
			else 
			if (tok!=NULL)
				Message("Unknown Order!","Order Item:\n%Fs",tok);

			n++;
			tok=_fstrtok(NULL,",\r");
		} // while

		SendMessage(Block_Window, CMD_BLOCK_SET_ORDER, 0, tv);
		return 0;
	}
	else if (_fstrstr(buffer,"EUT power")==buffer)
	{
		if (_fstrstr(buffer,":ON")) 
		{
			SendMessage(hWndComm, KT_EUT_ON,	0, 0L);
			if (SIMULATION) 
				SendDlgItemMessage(GetHWND(), IDD_EUT_POWER, BM_SETCHECK,(EUT_POWER=2),0l);

		}
		else if (_fstrstr(buffer,":OFF"))
		{
			SendMessage(hWndComm, KT_EUT_OFF,	0, 0L);
			if (SIMULATION) 
				SendDlgItemMessage(GetHWND(), IDD_EUT_POWER, BM_SETCHECK,(EUT_POWER=0),0l);
		}
		return 0;
	}
	else if (_fstrstr(buffer,"Wait time")==buffer)
	{
		_fstrncpy(lbuff,buffer+10,80);
		tempi=0;
		if (_fstrstr(buffer,"Second")) 
			tempi=0;
		else if (_fstrstr(buffer,"Minute"))
			tempi=1;
		else if (_fstrstr(buffer,"Hour"))
			tempi=2;

		SendDlgItemMessage(GetHWND(), IDD_WAIT_CB, CB_SETCURSEL,tempi, 0L);
		SendMessage(GetHWND(),WM_COMMAND,MAKEWPARAM(IDD_WAIT_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAIT_CB));
		sscanf(lbuff,"%d",&tempi);
		Wait_Pair.Set_Value(tempi);
		return 0;
	}

	if (_fstrstr(buffer,"Duration time")==buffer)
	{
		_fstrncpy(lbuff,buffer+14,80);
		tempi=0;
		if (_fstrstr(buffer,"Second")) 
			tempi=0;
		else if (_fstrstr(buffer,"Minute"))
			tempi=1;
		else if (_fstrstr(buffer,"Hour"))
			tempi=2;
		SendDlgItemMessage(GetHWND(), IDD_WAITD_CB, CB_SETCURSEL,tempi, 0L);
		SendMessage(GetHWND(),WM_COMMAND,MAKEWPARAM(IDD_WAITD_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAITD_CB));
		sscanf(lbuff,"%d",&tempi);
		Dur_Pair.Set_Value(tempi);
		
		return 0;
	}

	if (_fstrstr(buffer,"Phase Mode")==buffer)
	{
		if (_fstrstr(buffer,"DC Period")) {Phase_Mode=FALSE; DC_MODE=TRUE;}
		else
		if (_fstrstr(buffer,"Period")) {Phase_Mode=FALSE; DC_MODE=FALSE;}
		else
		if (_fstrstr(buffer,"Phase"))Phase_Mode=TRUE;

		if (Phase_Mode)
			SendMessage(GetHWND(),IDD_PHASE_MODE,0,1);
		else
		if (DC_MODE)
			SendMessage(GetHWND(),IDD_PHASE_MODE,0,2);
		else
			SendMessage(GetHWND(),IDD_PHASE_MODE,0,0);

		return 0;
	}
	else if (_fstrstr(buffer,"Name:")==buffer)
	{
		Tname.SetText((LPSTR)buffer+5);
		return 0;
	}
	else 
	{ 
		Message("Unknow String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CE400MainDlg::LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Local window procedure
	switch (msg) 
	{
	case WM_DRAWITEM:
		// This case is necessary to make sure the bitmap buttons (EUT, Exit, Start, etc.) will draw themselves.
		// We are basically relaying the message back to the buttons (MFC's reflections idea)
		switch (wParam)
		{
		case IDD_RUN:
		case IDD_STOP:
		case IDD_RAW_STOP:
		case IDD_ECAT_EXIT:
		case IDD_EUT_POWER:
			SendItemMsg((int) wParam, msg, wParam, lParam);
			return TRUE;
		}
		break;

	case IDD_PHASE_MODE:
		switch (lParam)
		{
		case 0:
		case 2: //DC
			PhaseSet.Hide(TRUE);
			PerSet.Hide(FALSE);
			SendMessage(Block_Window, CMD_BLOCK_SET, PERBLOCK,	MAKELONG(CE400App::pericon,	CE400App::percursor));
			pfix = PerSet.fixed();
			SendMessage(Block_Window, CMD_BLOCK_FIXED, PERBLOCK, pfix);
			Phase_Mode=FALSE;
			if (lParam==2) 
			{
				DC_MODE=TRUE;
				PerSet.SetListN(2);
			}
			else 
			{
				DC_MODE=FALSE;
				PerSet.SetListN(0);
			}
		break;

		case 1:
			if (CoupleMode.FrontPanel())
			{
				long m = DC_MODE ? 2 : 0;
				SendMessage(GetHWND(),IDD_PHASE_MODE,0,m);
				break;
			}

			// Don't allow Phase sync if CE BOX and not EP option
			if (IS_CE_BOX && !CE_EP_OPTION)
			{ 
				Phase_Mode=FALSE;            // ##### added by Ken Merrithew
				CoupleMode.Set_Update(TRUE); // ##### added by Ken Merrithew
				SendMessage(GetHWND(),IDD_PHASE_MODE,0,0);
				break;                       
			}

			PerSet.Hide(TRUE);
			PhaseSet.Hide(FALSE);
			PhaseSet.SetListN(1);
			SendMessage(Block_Window, CMD_BLOCK_SET, PERBLOCK,	MAKELONG(CE400App::phaseicon,	CE400App::phasecursor));
			phfix = PhaseSet.fixed();
			SendMessage(Block_Window, CMD_BLOCK_FIXED, PERBLOCK, phfix);
			Phase_Mode=TRUE;
			// JFL 9/15/95 Force update of coupling info.
			CoupleMode.Set_Update(TRUE);
			break;

		} // switch (lParam)
		break;

	case WM_HSCROLL:
		if (created)
		{
			switch (GetWindowLong((HWND)lParam, GWL_ID))
			{
				case IDD_WAIT_SB :	return (Wait_Pair.Scroll_Proc(msg, wParam, lParam));
				case IDD_WAITD_SB : return (Dur_Pair.Scroll_Proc(msg, wParam, lParam));
			}
		}
		return TRUE;

	default:
		return CEcatMainDlg::LocalDlgProc(hWnd, msg, wParam, lParam);
	}

   return 0;
}

