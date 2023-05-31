//////////////////////////////////////////////////////////////////////
// E500MainDlg.cpp: implementation of the CE500MainDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e500.h"
#include "E500App.h"
#include "E500MainDlg.h"
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
static int limitv;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CE500MainDlg::CE500MainDlg()
{
	chirp_hide = FALSE;
	Last_State = (LONG) ILOCK_OK;
	This_State = (LONG) ILOCK_OK;
	yBrush = NULL;
	Time_elapsed = 0;
	LastTick = 0;
	LastTest = 0;
	nRunMode = IDLE_MODE;		// initialize.
	PAUSE = FALSE;
	IL_PAUSE = FALSE;
	IS_E52x = FALSE;

	// For backward compatibility with dlgCtl class
	memset(Obj_Name, 0, sizeof(Obj_Name));
	created = FALSE;
	mv = 0;
}

//////////////////////////////////////////////////////////////////////
CE500MainDlg::~CE500MainDlg()
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
int CE500MainDlg::OnCmdMsg(UINT uiNotify, UINT uiCtrlId, HWND hwndCtl)
{
	BOOL rv;
	HWND hMainDlg = GetHWND();
	LONG time;
	char gStr0[100];
	int i;

	switch (uiCtrlId)
	{
		case CMD_PAUSE:
			if (hwndCtl != NULL)
				IL_PAUSE = TRUE;
			else
			{
				if (COMMERROR)
				{
					COMMERROR=FALSE;
					PURGE();
					This_State=ECAT_Ilock();
					Ilock.Check_Interlocks(This_State);  // make sure get error dlg.
				}

				if (RESET_REQUIRED)
				{
					SendMessage(hWndComm, KT_SRG_RESETALL,0, 0L);
					if (EUT_POWER==1)
						if (!theApp.POWER_USER)
							Message("Verify Power","Please Cycle the BLUE EUT button,\nand verify that power is present at the input to the surge coupler");
						else
						{
							//Power User
							SendMessage(hWndComm, KT_EUT_ON,	0, 0L);
							SendMessage(hMainDlg,WM_COMMAND,IDD_SET_POWER,0L);
							for (int i=0; ((i<5) && (EUT_POWER!=2)); i++)
							EUT_POWER=SendMessage(hWndComm, KT_EUT_QUERY,0, 0L);
							SendDlgItemMessage(hMainDlg, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
						}

					RESET_REQUIRED=FALSE;
					charge_failed=FALSE;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State:");
					Actual_Charge_Mode=MODE_UNKNOWN;
				}//Reset Required

				IL_PAUSE = FALSE;

				if (nRunMode!=IDLE_MODE)
				{
					int t;

					Desired_Charge_Mode=MODE_DONE;
					charge_failed=FALSE;
					Actual_Charge_Mode=MODE_UNKNOWN;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: ");
					nRunMode=WAIT_MODE;
					if (SIMULATION) 
						t=4;
					else
						t=(wave_list[WaveMode.Get_Current_Values()].mint);

					if (t>PauseWait) 
						PauseWait=t;
				}
			}//Pause = False
			break;

		case IDD_SET_POWER:
			rv = MessageBox(NULL,"About to apply EUT power to the \nDevice under test. Have you verified that it is safe to do so?",
								"POWER WARNING",MB_YESNO|MB_TASKMODAL|MB_ICONHAND);
			if (rv!=IDYES) break;
			if (ECAT_Ilock()!=ILOCK_OK)
			{
				MessageBox(NULL,"The Interlock System is open Please correct this\nand try to enable EUT power again.",
								"WARNING",MB_TASKMODAL|MB_ICONHAND);
				break;
			}

			if (SendMessage(hWndComm, KT_EUT_CHEAT,0, 0L)==-1)
				MessageBox(NULL,"The EUT power switch is not depressed!\nThe switch must be depressed to enable\nEUT power.",
								"WARNING",MB_TASKMODAL|MB_ICONHAND);
			break;

		case IDD_RUN:
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

			if ((dcnt*vcnt*swcnt*wacnt*polcnt*repcnt)==0)
			{
				Message("Invalid Mode","One or More of the selected lists has\nno entries.\nPlease correct this");
				return 0;
			}

			if (!CoupleMode.setvalid())
			{
				Message("Invalid Mode","The Coupling Mode(s) selected\nare invalid. Please correct this");
				return 0;
			}
			WaveMode.Copy_Net_Name(gStr0,99);

			// Modify test: include E521 and E522 -- JFL 3/4/96
			if (_fstrstr(gStr0,"E522") || _fstrstr(gStr0,"E521")) 
				IS_E52x=TRUE;
			else 
				IS_E52x=FALSE;

			if (CoupleMode.FrontPanel() && IS_E52x)
			{
				Message("Invalid Mode","The E522 Can only Couple out of the E522 Coupler\nPlease change the coupling 'mode' field to \nanother mode");
				return 0;
			}
			Desired_Charge_Mode=MODE_DONE;
			PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: ");
			charge_failed=FALSE;

			if (Log.log_on())
			{
				if (!Log.start_log())
				{
					//Log Failed
					return 0;
				}
				UpdateWindow(hMainDlg);

				if (!Log.log_on())//It must have been turned off in Start_log
				{
					HMENU hMenu,phMenu;

					hMenu=GetMenu(hMainDlg);
					phMenu=GetSubMenu(hMenu,1);
					ModifyMenu(hMenu,1,MF_BYPOSITION|MF_POPUP,(UINT) phMenu,"Log(Off)");
					DrawMenuBar(hMainDlg);
				}
				else
					EnableWindow(GetDlgItem(IDD_ECAT_COMM), TRUE);			
			}

			// Disable menu
			SendMessage(GetParent(), WM_ENABLE_RUN, 0, (LPARAM) TRUE);
			nRunMode = RUN_MODE;
			EnableMenuItem(GetMenu(hMainDlg),1,MF_BYPOSITION|MF_GRAYED);
			DrawMenuBar(hMainDlg);

			SendDlgItemMessage(hMainDlg, IDD_RUN, BM_SETCHECK, 1, 0L);
			EnableWindow(GetDlgItem(IDD_RAW_STOP), TRUE);
			//SendDlgItemMessage(hMainDlg, IDD_STOP, BM_SETCHECK, 0, 0L);
			EnableWindow(GetDlgItem(IDD_ECAT_EXIT), FALSE);
			PauseWait =	0;
			// end of IDD_RUN - no break required!

		case IDD_RUN_PREP:
			if ((theApp.POWER_USER) && (EUT_POWER==1))
				SendMessage(hMainDlg,WM_COMMAND,IDD_SET_POWER,0L);

			Ilock.Check_Interlocks( Last_State);
			SendMessage(hWndComm, KT_INITIALIZE,	0, 0L);
			time = SendMessage(Block_Window, CMD_BLOCK_GET_ORDER, 0, 0L);
			//The format of	time is: in Hex	6 digits Volt,Dur,repeat,Cpl,Pol
			for	(i = 0;	i < 8; i++) 
				Levels[i] =	0;

			for	(i = 8;	i > 0; i--)
			{
				Levels[(time &0x000F) - 1] = i-1;
				time /=16;
			}
			steps_taken	= 0;
			changed_while_running=0;

			PolaritySet.Set_Update(1);
			VoltageSet.Set_Update(1);
			CoupleMode.Set_Update(1);
			WaveMode.Set_Update(1);
			MeasSet.Set_Update(1);
			PhaseSet.Set_Update(1);

			VoltageSet.Set_Start_Values();
			WaveMode.Set_Start_Values();
			CoupleMode.Set_Start_Values();
			Repeat.Set_Start_Values();
			PolaritySet.Set_Start_Values();
			PhaseSet.Set_Start_Values();
			MeasSet.Setup_Values(CoupleMode.Get_Current_Values());

			PhaseSet.Show_Run(1);
			VoltageSet.Show_Run(1);
			CoupleMode.Show_Run(1);
			WaveMode.Show_Run(1);
			Repeat.Show_Run(1);
			PolaritySet.Show_Run(1);
			MeasSet.Show_Run(1);

			EnableWindow(Block_Window,0);

			if (SIMULATION) 
				PauseWait=5;
			else 
				PauseWait=wave_list[WaveMode.Get_Current_Values()].mint;

			PrintfDlg(hMainDlg, IDD_BETWEEN_TEXT, "Left:%ld ", PauseWait);
			nRunMode=WAIT_MODE;

			LastTick = GetTickCount();
			if (Log.log_on())
			{
				SaveText(Log.logfile());
				SendMessage(hWndComm,KT_SRG_LOG_START,0,0L);
			}
			break;
			// end of IDD_RUN_PREP

		case IDD_EUT_POWER:
			if (SIMULATION || (SURGE_COUPLER!=-1))
			{
				changed_since_save=TRUE;
				if (EUT_POWER)
					SendMessage(hWndComm, KT_EUT_OFF,0, 0L);
				else
					SendMessage(hWndComm, KT_EUT_ON, 0, 0L);

				if (SIMULATION)
				{
					if (EUT_POWER) 
						EUT_POWER=0;
					else 
						EUT_POWER=2;

					SendDlgItemMessage(hDlgChild, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
				}
			}
			break; // end of IDD_EUT_POWER

		case IDD_RAW_STOP:
			if ((nRunMode==IDLE_MODE) || (nRunMode==DELAY_MODE))
			return TRUE;

			rv=PAUSE;
			PAUSE=TRUE;
			if (Do_Pause(GetParent()))
			{
				PAUSE=rv;
				UpdateWindow(hMainDlg);
				return TRUE;
			}
			PAUSE=rv;

			//VictorGinzburg 04/22/97
			Actual_Charge_Mode=MODE_DONE;
			Desired_Charge_Mode=MODE_DONE;
			PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Aborted");
			/////////////////////////

		case IDD_STOP:
			Last_State = ILOCK_OK;
			Desired_Charge_Mode=MODE_JUSTSTOPPED;
			nRunMode=IDLE_MODE;
			SendMessage(hWndComm, KT_ABORT,	0, 0L);

			PhaseSet.Show_Run(0);
			VoltageSet.Show_Run(0);
			CoupleMode.Show_Run(0);
			WaveMode.Show_Run(0);
			Repeat.Show_Run(0);
			PolaritySet.Show_Run(0);
			MeasSet.Show_Run(0);
			EnableWindow(Block_Window,1);
			EnableWindow(GetDlgItem(IDD_ECAT_COMM), FALSE);
			if (Log.log_on()) 
				Log.end_log();

			SendDlgItemMessage(hMainDlg, IDD_RUN, BM_SETCHECK, 0, 0L);
			//SendDlgItemMessage(hMainDlg, IDD_STOP, BM_SETCHECK, 1, 0L);
			EnableWindow(GetDlgItem(IDD_RAW_STOP), FALSE);
			EnableWindow(GetDlgItem(IDD_ECAT_EXIT), TRUE);

			PrintfDlg(hMainDlg, IDD_TOTAL_N, "Tests:%ld", LastTest);
			PrintfDlg(hMainDlg, IDD_DURATION_TEXT, "Idle");
			PrintfDlg(hMainDlg, IDD_BETWEEN_TEXT, "Idle");

			PhaseSet.Set_Update(0);
			VoltageSet.Set_Update(0);
			CoupleMode.Set_Update(1);
			WaveMode.Set_Update(1);

			// Enable menu
			SendMessage(GetParent(), WM_ENABLE_RUN, 0, (LPARAM) FALSE);
			COMMERROR=FALSE;
			PURGE();
			PAUSE = FALSE;
			IL_PAUSE = FALSE;
			break;
			// end of IDD_STOP

		case IDD_RAW_STEP:
			steps_taken++;
			rv=1;
			for (i = 5; (Levels[i] == 0) &&	(i > 0); i--);
			do
			{
				switch (Levels[i])
				{
				case VBLOCK : //Voltage
					rv = VoltageSet.Set_Next_Step();
					if (rv) VoltageSet.Set_Start_Values();
					break;

				case PHBLOCK : //Duration
					rv = PhaseSet.Set_Next_Step();
					if (rv) PhaseSet.Set_Start_Values();
					break;

				case WBLOCK : rv=WaveMode.Set_Next_Step();
					if (rv) WaveMode.Set_Start_Values();
					break;
				
				case RBLOCK : //Repeat
					rv = Repeat.Set_Next_Step();
					if (rv) Repeat.Set_Start_Values();
					break;
				
				case CBLOCK ://Couple Mode
					rv = CoupleMode.Set_Next_Step();
					if (rv) CoupleMode.Set_Start_Values();
					MeasSet.Setup_Values(CoupleMode.Get_Current_Values());
					break;
				
				case POBLOCK : //Polarity
					rv = PolaritySet.Set_Next_Step();
					if (rv) 
						PolaritySet.Set_Start_Values();
					break;
				} // switch (Levels)

				if (rv) i--;
			}
			while ((rv != 0) && (i >= 0));

			if (changed_while_running)
				PrintfDlg(hMainDlg, IDD_TOTAL_N, "Tests: %ld of ???", steps_taken);
			else
				PrintfDlg(hMainDlg, IDD_TOTAL_N, "Tests: %ld of %ld", steps_taken, LastTest);

			VoltageSet.Refresh_Values();
			WaveMode.Refresh_Values();
			CoupleMode.Refresh_Values();
			PolaritySet.Refresh_Values();
			PhaseSet.Refresh_Values();
			MeasSet.Refresh_Values();

			if (rv)
			{
				//nRunMode = DELAY_MODE;
				nRunMode=IDLE_MODE;
				Desired_Charge_Mode=MODE_JUSTSTOPPED;

				Log.loglpstr("Test Complete\n");
				SendMessage(hMainDlg, WM_COMMAND, IDD_STOP, 0L);
				//SendDlgItemMessage(hMainDlg, IDD_RUN, BM_SETCHECK, 0, 0L);
				//SendDlgItemMessage(hMainDlg, IDD_STOP, BM_SETCHECK, 1, 0L);
				//EnableWindow(GetDlgItem(IDD_ECAT_EXIT), TRUE);
				//EnableWindow(GetDlgItem(IDD_RAW_STOP), FALSE);

				//VictorGinzburg 04/22/97
				Actual_Charge_Mode=MODE_DONE;
				Desired_Charge_Mode=MODE_DONE;
				PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Discharged");
				/////////////////////////

				MessageBox(hMainDlg, "The selected test has been completed. Press RUN button to repeat the test.", "Test Complete", MB_OK | MB_ICONINFORMATION);
			}
			return (rv);
			// end of IDD_RAW_STEP

		case IDD_ECAT_COMM:
			Log.comment((LPSTR) NULL);
			break;

		case IDD_WAIT_CB:
			switch (SendDlgItemMessage(hMainDlg, IDD_WAIT_CB, CB_GETCURSEL,	0, 0L))
			{
			case 0:
				if (Wait_mul==1) break;
				Wait_mul = 1;
				Wait_Pair.init(hMainDlg, IDD_WAIT_SB, IDD_WAIT_EB, Old_Min_Time, 360,Old_Min_Time , 0, 1, RGB(255, 255, 0), "");
				break;
			
			case 1:
				if (Wait_mul==60) break;
				Wait_mul = 60;
				Wait_Pair.init(hMainDlg, IDD_WAIT_SB, IDD_WAIT_EB, 1, 240, 1, 0, 1, RGB(255, 255, 0), "");
				break;

			case 2 :
				if (Wait_mul==3600) break;
				Wait_mul = 3600;
				Wait_Pair.init(hMainDlg, IDD_WAIT_SB, IDD_WAIT_EB, 1, 24, 1, 0, 1, RGB(255, 255, 0), "");
				break;
			}
			return TRUE;
			// end of IDD_WAIT_CB

		case IDD_WAIT_EB:
			return (Wait_Pair.Scroll_Proc(WM_COMMAND, MAKEWPARAM(uiCtrlId, uiNotify), (LPARAM) hwndCtl));

		default:
			return CEcatMainDlg::OnCmdMsg(uiNotify, uiCtrlId, hwndCtl);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
long CE500MainDlg::GetAllowCouple()
{
	long allow_cpl = 0;

	if (SURGE_COUPLER !=-1) allow_cpl |= TYPE_CPLAC;
	if (IO_COUPLER    !=-1) allow_cpl |= TYPE_CPLIO;

	allow_cpl &= WaveMode.AllowCouple();
	return allow_cpl;
}

//////////////////////////////////////////////////////////////////////
BOOL CE500MainDlg::OnInitDialog(HWND hWnd)
{
	LONG j;
	static int error_cnt;

	if (!CEcatMainDlg::OnInitDialog(hWnd))
		return TRUE;

	// For backward compatibility with dlgCtl class
	hDlgChild = hWnd;
	strcpy(Obj_Name, "E500");
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

	HWND hMainDlg = hWnd;

	// Get Pulse frame rect
	RECT rect;
	HWND hPulse = GetDlgItem(IDC_MAINFRM_BASE);
	GetWindowRect(hPulse, &rect);
	ShowWindow(hPulse, SW_HIDE);
	S2C(hWnd, &rect);

	j = 6600;
	VoltageSet.Init(ghinst,hMainDlg,rect,CE500App::pvicon,0,j,1000,-1,10,"Voltage","V");
	PolaritySet.Init(ghinst,hMainDlg,VoltageSet,CE500App::pvicon,CE500App::mvicon,CE500App::vicon,"Polarity");
	rect.top += 2;

	WaveMode.Init(ghinst, hMainDlg, rect, CE500App::waicon, "Waveform");
	rect.top += 2;

	if (SURGE_COUPLER==99) 
		Choose_Coupler(hMainDlg);
	AC_COUPLER = SURGE_COUPLER;

	if (!(calset[IO_COUPLER].type & TYPE_CPLSRG))
		IO_COUPLER = -1;
	
	CoupleMode.Set_Both_Modes(TRUE);
	CoupleMode.Init(ghinst, hMainDlg, rect, CE500App::swicon, "Coupling",SURGE_THREE_PHASE );
	rect.top += 2;

	PhaseSet.Init(ghinst,hMainDlg,rect,CE500App::phicon,0,360,0,0,5,"Phase","dg");
	PhaseSet.three_phase(SURGE_THREE_PHASE);
	PhaseSet.dc_possible(DC_SURGE);
	rect.top += 24;

	// Place measurement dialog in place
	rect.left -= 2;
	MeasSet.Init(ghinst,hMainDlg,rect,"Measurements");
	PhaseSet.Set_Message( KT_PHASE_ANGLE_SETSTATE,KT_SRG_PHASE_MODE,hWndComm);
	CoupleMode.Set_Message( KT_SRG_COUPLE_SETSTATE, hWndComm);
	WaveMode.Set_Message( KT_SRG_WAVE_SETSTATE, hWndComm);
	PolaritySet.Set_Message( KT_SRG_VOLTAGE_SETSTATE, hWndComm);

	MeasSet.Set_Message(KT_MEAS_SETSTATE,hWndComm);

	rect.top  += 12;
	rect.left += 48;
	AcmeasSet.Init(ghinst,hMainDlg,rect,"AC Measurements",SURGE_COUPLER);
	//if (!AC_OPTION) MeasSet.MoveBy(0,20);

	if (SIMULATION) 
		Wait_Pair.init(hMainDlg, IDD_WAIT_SB, IDD_WAIT_EB, 5, 360, 5, 0, 1, RGB(255, 255, 0), "");
	else  
		Wait_Pair.init(hMainDlg, IDD_WAIT_SB, IDD_WAIT_EB, 20, 360, 20, 0, 1, RGB(255, 255, 0), "");

	SendDlgItemMessage(hMainDlg, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Sec"));
	SendDlgItemMessage(hMainDlg, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Min"));
	SendDlgItemMessage(hMainDlg, IDD_WAIT_CB, CB_ADDSTRING,	0, (LONG) ((LPSTR) "Hr"));
	SendDlgItemMessage(hMainDlg, IDD_WAIT_CB, CB_SETCURSEL,	0, 0L);
	SendMessage(hWnd,WM_COMMAND,MAKELONG(IDD_WAIT_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAIT_CB));

	EnableWindow(GetDlgItem(IDD_ECAT_COMM), FALSE);

	// Setup Order block window
	HWND hOrder = GetDlgItem(IDC_ORDER_BASE);
	GetWindowRect(hOrder, &rect);
	ShowWindow(hOrder, SW_HIDE);
	S2C(hMainDlg, &rect);
	Block_Window = CreateWindow("BlockClass", "BlockWindow", WS_CHILD, rect.left, rect.top, 
		rect.right - rect.left, rect.bottom - rect.top, hMainDlg, NULL, ghinst, 0L);

	SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK, MAKELONG(CE500App::pvicon,CE500App::pvcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, PHBLOCK,MAKELONG(CE500App::phicon,CE500App::phcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, WBLOCK, MAKELONG(CE500App::waicon,CE500App::wacursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, CBLOCK, MAKELONG(CE500App::swicon,CE500App::swcursor));
	SendMessage(Block_Window, CMD_BLOCK_SET, RBLOCK,0L);
	ShowWindow (Block_Window, SW_SHOWNORMAL);

	// Place REPEAT control below "ORDER" block 
	HWND hRepeat = GetDlgItem(IDC_REPEAT_BASE); 
	GetWindowRect(hRepeat, &rect);
	ShowWindow(hRepeat, SW_HIDE); 
	S2C(hMainDlg, &rect);
	Repeat.Init(ghinst, hMainDlg, rect, "Repeat");

	SendDlgItemMessage(hMainDlg, IDD_RUN,  BM_SETCHECK, 0, 0L);
	EnableWindow(GetDlgItem(IDD_RAW_STOP), FALSE);
	//SendDlgItemMessage(hMainDlg, IDD_STOP, BM_SETCHECK, 1, 0L);

	dcnt  = PhaseSet.ReportSteps();
	vcnt  = VoltageSet.ReportSteps();
	swcnt = CoupleMode.ReportSteps();
	wacnt = WaveMode.ReportSteps();
	repcnt= Repeat.ReportSteps();
	polcnt= PolaritySet.ReportSteps();

	PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Discharged");
	CoupleMode.SetFrontName("E501" );

	// Prepare for the timer updates
	LastTest = 0;
	Wait_mul = 1;
	nRunMode=IDLE_MODE;
	created = TRUE;
	SET_READY=FALSE;
	LastTick = GetTickCount();
	change_shown=-4;
	changed_since_save=FALSE;
	PAUSE = FALSE;
	IL_PAUSE = FALSE;

	CoupleMode.SetAllowCouple(GetAllowCouple());
	CoupleMode.Set5IOLines(WaveMode.FiveChanIo());

	if (SURGE_COUPLER==-1)
	{
		ShowWindow(GetDlgItem(IDD_EUT_POWER), SW_HIDE);
		ShowWindow(GetDlgItem(IDD_EUT_GB),    SW_HIDE);
	}
	PhaseSet.Random_only(TRUE);

	if ((SURGE_COUPLER!=-1) && (!SIMULATION))
	{
		SendMessage(hWndComm, KT_SRG_INIT,0, 0L);
		EUT_POWER=SendMessage(hWndComm, KT_EUT_QUERY,0, 0L);
		SendDlgItemMessage(hMainDlg, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
	}

	// make it a virgin start, or open the file, if supplied
	if (theApp.IsDdeFile())
	{
		strcpy(fnamebuf, theApp.GetDdeFileName());
		OpenFile();
	}
	else
		DoFileNew();

	Last_State = ILOCK_OK;
	This_State = ECAT_Ilock();
	//DBGOUT("FIRST: Last_State: %d; This_State: %d\n", Last_State, This_State);

	// Kick off the timer
	if (!StartTimer(TIMER2, 500))
		MessageBox(GetParent(), "Not Enough Timers", "Not Enought Timers", MB_OK);

	Actual_Charge_Mode=MODE_UNKNOWN;
	Desired_Charge_Mode=MODE_DONE;

	return TRUE;
}

BOOL CALLBACK LimitProc(HWND hMainDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		switch(limitv)
		{
		case 1:
			SetDlgItemText(hMainDlg,IDD_LIMIT_TEXT,"Peak Positive Voltage");
			break;

		case 2:
			SetDlgItemText(hMainDlg,IDD_LIMIT_TEXT,"Peak Negative Voltage");
			break;

		case 4:
			SetDlgItemText(hMainDlg,IDD_LIMIT_TEXT,"Peak Positive Current");
			break;

		case 8:
			SetDlgItemText(hMainDlg,IDD_LIMIT_TEXT,"Peak Negative Current");
			break;

		default: 
			SetDlgItemText(hMainDlg,IDD_LIMIT_TEXT,"Multiple Items");
			break;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUT_RUN:
			Log.loglpstr("User continued test after failed measurment\n");  
			EndDialog(hMainDlg, TRUE);
			break;

		case IDC_BUT_STOP:
			Log.loglpstr("User ABORTED test after failed measurment\n");  
			EndDialog(hMainDlg, FALSE);
			break;
		}
	}
	return (FALSE);
}

//////////////////////////////////////////////////////////////////////
void CE500MainDlg::OnTimer(UINT uiTimerId)
{
	if (uiTimerId != TIMER2)
		return;

	//DBGOUT("Last_State: %d; This_State: %d\n", Last_State, This_State);

	BOOL rv = FALSE;
	int	results[4];
	char gStr0[100];

	HWND hMainDlg = GetHWND();
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
			if ((EUT_POWER==0) && (theApp.POWER_USER))
				PostMessage(hMainDlg,WM_COMMAND,IDD_SET_POWER,0L);

			EUT_POWER = HIWORD(Time_elapsed);
			SendDlgItemMessage(hMainDlg, IDD_EUT_POWER, BM_SETCHECK, EUT_POWER, 0L);
		}
		AcmeasSet.UpdateNumbers();
		if (BACK_FROM_LOCAL)
		{
			BACK_FROM_LOCAL = FALSE;
			AcmeasSet.UpdateEutSource();
		}

	}//Not simulation
	else 
		AcmeasSet.UpdateNumbers();

	if ((SIMULATION))
	{
		if ( (PAUSE) || (IL_PAUSE) || (nRunMode==IDLE_MODE) || (Actual_Charge_Mode==MODE_UNKNOWN))
		{
			Actual_Charge_Mode=MODE_DONE;
			//VictorGinzburg 04/24/97
			//PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Discharged");
			/////////////////////////
		}
		else if (Desired_Charge_Mode==MODE_CHARGEING)
		{
			Actual_Charge_Mode=MODE_CHARGED;
			PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Charged");
		}
	}//Simulation

	//	if ((SIMULATION))
	//      switch (ECAT_OPC())
	//	{
	//
	//       case OPC_IDLE:
	//         if ((Desired_Charge_Mode==MODE_CHARGEING) && (!charge_failed))
	//         {  charge_failed=SendMessage(hWndComm, KT_CHARGE,	0, 0L);
	//            if (charge_failed)
	//            {
	//               Ilock.Check_Interlocks(ILOCK_SURGE_BASE+charge_failed);
	//               charge_failed=TRUE;
	//            }
	//            else
	//            {  charge_failed=FALSE;
	//               Desired_Charge_Mode=MODE_CHARGEING;
	//               Actual_Charge_Mode=MODE_UNKNOWN;
	//               PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: ");
	//            }
	//         }
	//         break;
	//       case OPC_CHARGING:
	//         if (Actual_Charge_Mode!=MODE_CHARGEING)
	//         {  Actual_Charge_Mode=MODE_CHARGEING;
	//            PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Charging");
	//         }
	//         break;
	//       case OPC_DELAY:
	//         if (Actual_Charge_Mode!=MODE_DELAY)
	//         {  Actual_Charge_Mode=MODE_DELAY;
	//            PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Waiting");
	//         }
	//         break;
	//      }//ECAT_OPC
	//      }/*Simulation Stuff */


	if ((nRunMode!=IDLE_MODE) && (!SIMULATION))
	{ // real stuff
		switch (ECAT_OPC())
		{
		case OPC_READY:
			if (Actual_Charge_Mode != MODE_CHARGED)
			{
				Actual_Charge_Mode   = MODE_CHARGED;
				PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Charged");
			}
			break;

		case OPC_IDLE:
			if (Actual_Charge_Mode != MODE_DONE)
			{
				Actual_Charge_Mode   = MODE_DONE;
				//VictorGinzburg 04/24/97
				//PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Discharged");
				/////////////////////////
			}
			if ( (PAUSE) || (IL_PAUSE) || (nRunMode == IDLE_MODE)) break;

			if ((Desired_Charge_Mode == MODE_CHARGEING) && (!charge_failed))
			{
				charge_failed=SendMessage(hWndComm, KT_CHARGE, 0, 0L);
				if (charge_failed)
				{
					Ilock.Check_Interlocks(ILOCK_SURGE_BASE+charge_failed);
					charge_failed=TRUE;
				}
				else
				{
					charge_failed=FALSE;
					Desired_Charge_Mode = MODE_CHARGEING;
					Actual_Charge_Mode  = MODE_UNKNOWN;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: ");
				}
			}
			break;

		case OPC_CHARGING:
			if (Actual_Charge_Mode != MODE_CHARGEING)
			{
				Actual_Charge_Mode   = MODE_CHARGEING;
				PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Charging");
			}
			break;

		case OPC_DELAY:
			if (Actual_Charge_Mode != MODE_DELAY)
			{
				Actual_Charge_Mode   = MODE_DELAY;
				PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Waiting");
			}
			break;
		}//ECAT_OPC


		if (This_State!=Last_State)
		{
			Ilock.Check_Interlocks(This_State);
			Last_State=This_State;
		}
	}/*Real Stuff */


	if ( (PAUSE) || (IL_PAUSE) )
	{
		LastTick=GetTickCount();
		return;
	}

	Time_elapsed = GetTickCount();
	if (Time_elapsed < LastTick) 
		LastTick = 0;

	LONG time = (Time_elapsed - LastTick);
	LastTick = Time_elapsed;
	Time_elapsed = time / 1000;
	//Seconds since time began
	LastTick -= (time % 1000);

	//Adjust for portions of seconds not used.
	if (Time_elapsed)
	{
		int t_time;

		switch (nRunMode)
		{
		case WAIT_MODE:
			PauseWait -= Time_elapsed;
			if (SIMULATION) 
				t_time=4;
			else if (IS_E52x) 
				t_time=36;
			else
				t_time=(wave_list[WaveMode.Get_Current_Values()].mint+2);

			if ((PauseWait<t_time) && (Actual_Charge_Mode==MODE_DONE) && (!charge_failed))
			{
				Desired_Charge_Mode=MODE_CHARGEING;
				if (SIMULATION)
				{
					Actual_Charge_Mode=MODE_CHARGEING;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State:Charging");
				}
				else
				{
					Actual_Charge_Mode=MODE_UNKNOWN;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State:");
				}
			}

			if (PauseWait >0)
			{
				PrintfDlg(hMainDlg, IDD_BETWEEN_TEXT, "Left:%ld ", PauseWait);
				break;
			}
			if (Actual_Charge_Mode == MODE_CHARGED)
				Desired_Charge_Mode =  MODE_SURGE; //The only way to get here is T=0
			nRunMode = RUN_MODE;

		case RUN_MODE:
		case STEP_MODE:
			if ((Actual_Charge_Mode==MODE_CHARGED))
			{
				charge_failed=SendMessage(hWndComm, KT_SURGE,	0, (long)results);
				if (charge_failed)
				{
					Ilock.Check_Interlocks(ILOCK_SURGE_BASE+charge_failed);
					Actual_Charge_Mode=MODE_UNKNOWN;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: ");
				}
				else
				{
					Desired_Charge_Mode=MODE_DONE;
					Actual_Charge_Mode=MODE_UNKNOWN;
					limitv=MeasSet.New_Numbers(results);

					if (limitv)
					{
						rv=PAUSE;
						PAUSE=TRUE;
						if (DialogBox(ghinst, MAKEINTRESOURCE(IDD_LIMIT), hMainDlg, (DLGPROC) LimitProc))
						{
							PAUSE=rv;
						}
						else
						{
							PAUSE=rv;
							SendMessage(hMainDlg,WM_COMMAND,IDD_STOP,0L);
							return;
						}
					}//Limitv
				}//Else Not Charge Failed

				if (SIMULATION)
				{
					Actual_Charge_Mode=MODE_DONE;
					Desired_Charge_Mode=MODE_DONE;
					PrintfDlg(hMainDlg, IDD_ACTION_TEXT, "State: Discharged");
				}
			} // MODE_CHARGED
			else
			{
				//VictorGinzburg 04/24/97
				//PrintfDlg(hMainDlg,	IDD_BETWEEN_TEXT, "CHARGING");
				/////////////////////////
				break;
			}

			if ((nRunMode == RUN_MODE) && (!charge_failed))
			{
				SendMessage(hMainDlg,WM_COMMAND,IDD_RAW_STEP,0L);
				if (nRunMode==IDLE_MODE || nRunMode==DELAY_MODE)
					break;
				PauseWait =	Wait_Pair.Value() * Wait_mul;
				PrintfDlg(hMainDlg, IDD_BETWEEN_TEXT, "Left:%ld ", PauseWait);
				nRunMode=WAIT_MODE;
			}
			break;

		case DELAY_MODE:
		case IDLE_MODE:
			break;
		}/*Switch */
	}/*Time Elapsed */

	if (Block_Changed())
		changed_since_save=TRUE;

	if (PhaseSet.changed())
	{
		changed_since_save=TRUE;
		dcnt = PhaseSet.ReportSteps();
	}

	if (VoltageSet.changed())
	{
		changed_since_save=TRUE;
		vcnt = VoltageSet.ReportSteps();
	}

	BOOL change_limits = FALSE;

	if (CoupleMode.changed())
	{
		change_limits = TRUE;
		changed_since_save=TRUE;
		swcnt = CoupleMode.ReportSteps();

		// The following test duplicated here (from WaveMode tests, below)
		// to catch selection of FrontPanel output for modules that
		// don't support it.    JFL 11/20/95

		if (!WaveMode.AllowFront() && CoupleMode.front())
		{
			//Set to Not Front Only!
			CoupleMode.Clear();
			CoupleMode.Process_Line("Fixed:");
		}

		if (CoupleMode.CoupleType() == TYPE_CPLAC)
		{
			PhaseSet.LineRef(TRUE);
			PhaseSet.Random_only(FALSE);
		}
		else
		{
			PhaseSet.LineRef(FALSE);
			PhaseSet.Random_only(TRUE);
		}
	}

	if (WaveMode.changed())
	{
		change_limits = TRUE;
		changed_since_save=TRUE;
		wacnt = WaveMode.ReportSteps();
		if (wacnt>1)
			CoupleMode.SetFrontName("E50x");
		else
		{
			WaveMode.Copy_Net_Name(gStr0,99);
			CoupleMode.SetFrontName(gStr0);
		}

		CoupleMode.Set5IOLines(WaveMode.FiveChanIo());
		CoupleMode.SetAllowCouple(GetAllowCouple());

		if (!WaveMode.AllowFront() && CoupleMode.front())
		{
			//Set to Not Front Only!
			CoupleMode.Clear();
			CoupleMode.Process_Line("Fixed:");
		}
	}

	if (change_limits)
	{
		long cpl_type = CoupleMode.CoupleType();

		VoltageSet.Set_Max_Value((float)WaveMode.MaxVoltage(cpl_type));
		int mt = WaveMode.MinTime(cpl_type);

		if (mt != Old_Min_Time)
		{
			Old_Min_Time = SIMULATION ? 5 : mt;
			if (Wait_mul==1)
				Wait_Pair.Set_Min_Value(Old_Min_Time);
		}
	}
	if (MeasSet.changed())
		changed_since_save=TRUE;
	if (AcmeasSet.changed())
		changed_since_save=TRUE;

	if (Repeat.changed())
	{
		repcnt=Repeat.ReportSteps();
		changed_since_save=TRUE;
		if (!Repeat.fixed())
		{
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,MAKELONG(CE500App::repicon,CE500App::repcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, RBLOCK, 0);
		}
		else 
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);
	}

	double dTime = dcnt*vcnt*swcnt*wacnt*polcnt*repcnt;
	if (dTime > (double) 0x7FFFFFF)
		time = 0x7FFFFFF;
	else
		time = dcnt*vcnt*swcnt*wacnt*polcnt*repcnt;

	//Alternating Polarity
	if ((nRunMode==IDLE_MODE) || (nRunMode==DELAY_MODE))
	{
		/*Not in Run mode */
		if (time != LastTest)
			PrintfDlg(hMainDlg, IDD_TOTAL_N, "Tests:%ld", time);

		LastTest   =  time;
		total_steps=  time;

		dTime *= (Wait_Pair.Value() * Wait_mul);
		if (dTime > (double) 0x7FFFFFF)
		{
			time = 0x7FFFFFF;
			PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Time: INVALID");
			LastTime = time;
		}
		else
		{
			time *=	(Wait_Pair.Value() * Wait_mul);
			if (time != LastTime)
			{
				Convert_Seconds(time, gStr0);
				PrintfDlg(GetHWND(), IDD_TOTAL_TEST, "Time: %s ", gStr0);
				LastTime = time;
			}
		}
	}
	else
	{
		/* In run mode */
		if (time != LastTest)
		{
			changed_while_running=TRUE;
			PrintfDlg(hMainDlg, IDD_TOTAL_N, "Tests: %ld of ???", steps_taken);
		}

		if (!changed_while_running)
		{
			time -= steps_taken;
			time *= (Wait_Pair.Value() * Wait_mul);

			if (time != LastTime)
			{
				Convert_Seconds(time, gStr0);
				PrintfDlg(hMainDlg, IDD_TOTAL_TEST, "Left: %s ", gStr0);
				LastTime = time;
			}
		}
		else
			PrintfDlg(hMainDlg, IDD_TOTAL_TEST, "Time:Changed ", gStr0);
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
			CoupleMode.Change_Icon(CE500App::swicon);
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::pvicon, CE500App::pvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 1 :       //Negative Polarity
			CoupleMode.Change_Icon(CE500App::mswicon);
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::mvicon, CE500App::mvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 2 :       //Alternating Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::vicon,	CE500App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE500App::vpicon, CE500App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;

		case 3 :       //Alternating Polarity 5 Ea
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::vicon,	CE500App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE500App::vpicon, CE500App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;
		}
		polcnt=PolaritySet.ReportSteps();
	}

	if (dfix^PhaseSet.fixed())
	{
		dfix = PhaseSet.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, PHBLOCK, dfix);
	}

	if (swfix^CoupleMode.fixed())
	{
		swfix = CoupleMode.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, CBLOCK, swfix);
	}

	if (wafix^WaveMode.fixed())
	{
		wafix = WaveMode.fixed();
		SendMessage(Block_Window, CMD_BLOCK_FIXED, WBLOCK, wafix);
	}
}

//////////////////////////////////////////////////////////////////////
void CE500MainDlg::DeleteContext()
{
	Clear();

	//JLR,10/25/95 - added {} after else to set old voltage limit
	//               only if error reading file
	if (mv != 0)
		VoltageSet.Set_Max_Value((float) mv);
}

//////////////////////////////////////////////////////////////////////
void CE500MainDlg::BeforeFileParse()
{
	mv = VoltageSet.Get_Upper_Limit();
	VoltageSet.Set_Max_Value(25000);
	PhaseSet.LineRef(TRUE);
	PhaseSet.Random_only(FALSE);

	long allow_cpl = 0;
	if (SURGE_COUPLER !=-1) allow_cpl |= TYPE_CPLAC;
	if (IO_COUPLER    !=-1) allow_cpl |= TYPE_CPLIO;
	CoupleMode.SetAllowCouple(allow_cpl);
}

//////////////////////////////////////////////////////////////////////
void CE500MainDlg::PostCalibrate()
{
	AcmeasSet.Update(SURGE_COUPLER);

	// Show or hide meas set
	/*
	if (ac_opt && !AC_OPTION) 
		MeasSet.MoveBy(0,20);
	if (!ac_opt && AC_OPTION) 
		MeasSet.MoveBy(0,-20);
	*/
}

//////////////////////////////////////////////////////////////////////
void CE500MainDlg::Clear()
{
	VoltageSet.Clear();
	PhaseSet.Clear();
	CoupleMode.Clear();
	WaveMode.Clear();
	Repeat.Clear();
	PolaritySet.Clear();
	MeasSet.Clear();
	AcmeasSet.Clear();

	SendDlgItemMessage(hDlgChild, IDD_WAIT_CB, CB_SETCURSEL,0, 0L);
	SendMessage(hDlgChild,WM_COMMAND,MAKELONG(IDD_WAIT_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAIT_CB));
	Tname.Clear();

	SendMessage(Block_Window, CMD_BLOCK_FIXED, VBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, WBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, CBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_FIXED, PHBLOCK, 1);
	SendMessage(Block_Window, CMD_BLOCK_SET,RBLOCK,0L);
	SendMessage(Block_Window, CMD_BLOCK_SET,POBLOCK,0L);

	EUT_POWER=0;

	PhaseSet.Random_only(TRUE);
	PhaseSet.LineRef(FALSE);
	SendDlgItemMessage(hDlgChild, IDD_EUT_POWER, BM_SETCHECK,EUT_POWER,0l);
	if (SIMULATION)
		Wait_Pair.init(hDlgChild, IDD_WAIT_SB, IDD_WAIT_EB, 5, 360, 5, 0, 1, RGB(255, 255, 0), "");
	else
		Wait_Pair.init(hDlgChild, IDD_WAIT_SB, IDD_WAIT_EB, 20, 360, 20, 0, 1, RGB(255, 255, 0), "");

	CoupleMode.Change_Icon(CE500App::swicon);

	dcnt  = PhaseSet.ReportSteps();
	vcnt  = VoltageSet.ReportSteps();
	swcnt = CoupleMode.ReportSteps();
	wacnt = WaveMode.ReportSteps();
	repcnt= Repeat.ReportSteps();
	polcnt= PolaritySet.ReportSteps();

	Wait_mul = 1;
}

//////////////////////////////////////////////////////////////////////
int CE500MainDlg::SaveText(HANDLE hfile)
{
	char buffer[80];
	long templ;
	int i;

	Tname.GetText(buffer);
	if (*buffer)
		fhprintf(hfile,"%Fs:Name:%s\r\n",(LPSTR)Obj_Name,buffer);
	else
		fhprintf(hfile,"%Fs:Name:\r\n",(LPSTR)Obj_Name);

	WaveMode.SaveText(hfile);
	CoupleMode.SaveText(hfile);
	VoltageSet.SaveText(hfile);
	Repeat.SaveText(hfile);
	PolaritySet.SaveText(hfile);
	MeasSet.SaveText(hfile);
	AcmeasSet.SaveText(hfile);
	PhaseSet.SaveText(hfile);

	fhprintf(hfile,"%Fs:Wait time  %d",(LPSTR)Obj_Name,(int)Wait_Pair.Value());
	switch (SendDlgItemMessage(hDlgChild, IDD_WAIT_CB, CB_GETCURSEL,0, 0L))
	{
	case 0 : 
		if (Wait_Pair.Value()>1) fhprintf(hfile," Seconds \r\n");
		else                     fhprintf(hfile," Second  \r\n");
		break;

	case 1 : 
		if (Wait_Pair.Value()>1) fhprintf(hfile," Minutes \r\n");
		else                     fhprintf(hfile," Minute  \r\n");
		break;

	case 2 :
		if (Wait_Pair.Value()>1) fhprintf(hfile," Hours \r\n");
		else                     fhprintf(hfile," Hour  \r\n");
		break;
	}

	if (EUT_POWER) 
		fhprintf(hfile,"%Fs:EUT power:ON \r\n",(LPSTR)Obj_Name);
	else 
		fhprintf(hfile,"%Fs:EUT power:OFF \r\n",(LPSTR)Obj_Name);

	templ= SendMessage(Block_Window, CMD_BLOCK_GET_ORDER, 0, 0L);
	if (!templ)
		fhprintf(hfile,"%Fs:Order:Fixed\r\n",(LPSTR)Obj_Name);
	else
	{  
		int ar[16];
		for (i=0; i<16; i++)
			ar[i]=0;

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
			case VBLOCK: 
				fhprintf(hfile,"Voltage");
				break;

			case PHBLOCK:
				fhprintf(hfile,"Phase");
				break;

			case WBLOCK:
				fhprintf(hfile,"Waveform");
				break;

			case RBLOCK:
				fhprintf(hfile,"Repeat");
				break;

			case CBLOCK:
				fhprintf(hfile,"Coupling");
				break;

			case POBLOCK:
				fhprintf(hfile,"Polarity");
				break;

			default:
				fhprintf(hfile,"Unknown?");
			}//Switch

			i++; 
			if (ar[i]) fhprintf(hfile,",");
		}//While

		fhprintf(hfile,"\r\n");
	}//Else

	return 1;
}

//////////////////////////////////////////////////////////////////////
int CE500MainDlg::Process_Line(LPSTR buffer)
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
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::pvicon, CE500App::pvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 1 :       //Negative Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::mvicon, CE500App::mvcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	0L);
			break;

		case 3:
		case 2 :       //Alternating Polarity
			SendMessage(Block_Window, CMD_BLOCK_SET, VBLOCK,	MAKELONG(CE500App::vicon,	CE500App::vcursor));
			SendMessage(Block_Window, CMD_BLOCK_SET, POBLOCK,	MAKELONG(CE500App::vpicon, CE500App::vpcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, POBLOCK, 0);
			break;
		} // switch 

		if (!Repeat.fixed())
		{  
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,MAKELONG(CE500App::repicon,CE500App::repcursor));
			SendMessage(Block_Window, CMD_BLOCK_FIXED, RBLOCK, 0);
		}
		else
			SendMessage(Block_Window,CMD_BLOCK_SET,RBLOCK,0L);

		buf=buffer+6;
		tok=_fstrtok(buf,",\r");
		while(tok)
		{
			if (_fstrstr(tok,"Voltage")) 
				tv|=(n<<((7-VBLOCK)*4));  
			else if (_fstrstr(tok,"Phase"))   
				tv|=(n<<((7-PHBLOCK)*4));
			else if (_fstrstr(tok,"Waveform"))
				tv|=(n<<((7-WBLOCK) *4));
			else if (_fstrstr(tok,"Repeat"))  
				tv|=(n<<((7-RBLOCK) *4));
			else if (_fstrstr(tok,"Coupling"))
				tv|=(n<<((7-CBLOCK) *4));
			else if (_fstrstr(tok,"Polarity"))
				tv|=(n<<((7-POBLOCK)*4));
			else if (tok!=NULL)
				Message("Unknown Order!","Order Item:\n%Fs",tok);
			n++;
			tok=_fstrtok(NULL,",\r");
		}
		SendMessage(Block_Window, CMD_BLOCK_SET_ORDER, 0, tv);
		return 0;
	} // if "Order:"
	else if (_fstrstr(buffer,"Phase")==buffer)
		return 0;
	else if (_fstrstr(buffer,"EUT power")==buffer)
	{
		if (_fstrstr(buffer,":ON") && (SURGE_COUPLER!=-1)) 
		{  
			SendMessage(hWndComm, KT_EUT_ON, 0, 0L);
			if (SIMULATION) 
				SendDlgItemMessage(hDlgChild, IDD_EUT_POWER, BM_SETCHECK,(EUT_POWER=2),0l);

		}
		else if (_fstrstr(buffer,":OFF"))
		{  
			if (SURGE_COUPLER!=-1)
				SendMessage(hWndComm, KT_EUT_OFF,	0, 0L);

			if (SIMULATION) 
				SendDlgItemMessage(hDlgChild, IDD_EUT_POWER, BM_SETCHECK,(EUT_POWER=0),0l);
		}
		return 0;
	}
	else if (_fstrstr(buffer,"Wait time")==buffer)
	{
		_fstrncpy(lbuff,buffer+10,80);
		sscanf(lbuff,"%d",&tempi);
		Wait_Pair.Set_Value(tempi);
		tempi=0;
		if (_fstrstr(buffer,"Second")) tempi=0;
		else if (_fstrstr(buffer,"Minute"))tempi=1;
		else if (_fstrstr(buffer,"Hour"))tempi=2;
		SendDlgItemMessage(hDlgChild, IDD_WAIT_CB, CB_SETCURSEL,tempi, 0L);
		SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_WAIT_CB,CBN_SELCHANGE),(LPARAM) GetDlgItem(IDD_WAIT_CB));
		sscanf(lbuff,"%d",&tempi);
		Wait_Pair.Set_Value(tempi);
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
LRESULT CALLBACK CE500MainDlg::LocalDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		break;

	case WM_HSCROLL:
		if (created)
		{
			switch (GetWindowLong((HWND)lParam, GWL_ID))
			{
				case IDD_WAIT_SB :	return (Wait_Pair.Scroll_Proc(msg, wParam, lParam));
				default:
					break;
			}
		}
		return TRUE;

	default:
		return CEcatMainDlg::LocalDlgProc(hWnd, msg, wParam, lParam);
	}

   return 0;
}



