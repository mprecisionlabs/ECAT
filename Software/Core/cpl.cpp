/*--------------------------------------------------------------------

  CPL.CPP -- Coupler dialog box control.

  History:	Base is V4.00C version.
		5/18/95 	JFL	Don't show DATA & AUX buttons in 'Create';
							(needs more work on coupling modes)
		6/1/95   JFL	Don't set output to front panel (in ECAT)
							when illegal mode selected.
		7/13/95	JFL	Treat NEW_LIST like LIST in setvalid().
		9/20/95	JFL	Redo 6/1/95 change in Refresh_Values:
							  broke front panel selection
		10/10/95 JLR   in Set5IOLines() for MODE_ALL set same mode, 
							(was set to ALL_IO).
		10/10/95 JLR   recheck & redisplay Invalid Mode when coupler changed.
		10/10/95	JLR   when select NEW LIST, not only show LIST instead, but
  						assign MODE_LIST to mode.
		10/10/95 JLR	in SaveText() for ALL list fixed "A/O" to	"I/O".
		10/11/95	JLR	in Clear() update selected_coupler, because combo box
							gets updated. 
     10/11/95 JLR   in Clear() - clear the list box of coupling modes
		10/12/95	JLR	Show_Valid() added and called for coupling buttons,
							Set5IOLines(), and Process_Line() instead Update_State().
----------------------------------------------------------------------*/

#include "stdafx.h"
#include "CPLS.h"
#include "801BITS.H"     
#include "kt_comm.h"
#include "comm.h"
#include "globals.h"
#include "message.h"
#include "module.h"
                 
#define HICOLOR RGB(255,255,0)
#define LOCOLOR RGB(255,0,255)
#define HITEXTCOLOR RGB(0,0,0)

#define MODE_FRONT      0
#define MODE_FIXED      1
#define MODE_LIST       2
#define MODE_NEW_LIST   3
#define MODE_STAND_AC   4
#define MODE_ALL_AC     5
#define MODE_STAND_IO   6
#define MODE_ALL_IO     7
#define MODE_ALL        8

const char *DATA_NAMES[]  = {"Off","On",0};
const char *CMODE_NAMES[] = {"Asymm","Symm",0};
const char *CLAMP_NAMES[] = {"Int 20V","Int 220V","Ext",0};
const int MAX_LB_BUF_LEN = 20;

const LONG ALL_5IO_COUPLES[] =
{
	  BIT_DATACPL | BIT_S1 | BITH_S2,
	  BIT_DATACPL | BIT_S1 | BITH_S3,
	  BIT_DATACPL | BIT_S1 | BITH_S4,
	  BIT_DATACPL | BIT_S1 | BITH_S2 | BITH_S3,
	  BIT_DATACPL | BIT_S1 | BITH_S2 | BITH_S4,
	  BIT_DATACPL | BIT_S1 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_S1 | BITH_S2 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_S2 | BITH_S1,
	  BIT_DATACPL | BIT_S2 | BITH_S3,
	  BIT_DATACPL | BIT_S2 | BITH_S4,
	  BIT_DATACPL | BIT_S2 | BITH_S1 | BITH_S3,
	  BIT_DATACPL | BIT_S2 | BITH_S1 | BITH_S4,
	  BIT_DATACPL | BIT_S2 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_S2 | BITH_S1 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_S3 | BITH_S1,
	  BIT_DATACPL | BIT_S3 | BITH_S2,
	  BIT_DATACPL | BIT_S3 | BITH_S4,
	  BIT_DATACPL | BIT_S3 | BITH_S1 | BITH_S2,
	  BIT_DATACPL | BIT_S3 | BITH_S1 | BITH_S4,
	  BIT_DATACPL | BIT_S3 | BITH_S2 | BITH_S4,
	  BIT_DATACPL | BIT_S3 | BITH_S1 | BITH_S2 | BITH_S4,
	  BIT_DATACPL | BIT_S4 | BITH_S1,
	  BIT_DATACPL | BIT_S4 | BITH_S2,
	  BIT_DATACPL | BIT_S4 | BITH_S3,
	  BIT_DATACPL | BIT_S4 | BITH_S1 | BITH_S2,
	  BIT_DATACPL | BIT_S4 | BITH_S1 | BITH_S3,
	  BIT_DATACPL | BIT_S4 | BITH_S2 | BITH_S3,
	  BIT_DATACPL | BIT_S4 | BITH_S1 | BITH_S2 | BITH_S3,
	  BIT_DATACPL | BIT_G  | BITH_S1,
	  BIT_DATACPL | BIT_G  | BITH_S2,
	  BIT_DATACPL | BIT_G  | BITH_S3,
	  BIT_DATACPL | BIT_G  | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S2,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S3,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S2 | BITH_S3,
	  BIT_DATACPL | BIT_G  | BITH_S2 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S2 | BITH_S3 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S2 | BITH_S4,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S2 | BITH_S3,
	  BIT_DATACPL | BIT_G  | BITH_S1 | BITH_S2 | BITH_S3 | BITH_S4,
	  0
};

const LONG STAND_5IO_COUPLES[] =					//	SURGE
{
    BIT_DATACPL | BIT_G |(256*(BIT_S1)),
    BIT_DATACPL | BIT_G |(256*(BIT_S2)),
    BIT_DATACPL | BIT_G |(256*(BIT_S3)),
    BIT_DATACPL | BIT_G |(256*(BIT_S4)),
    BIT_DATACPL | BIT_G |(256*(BIT_S1|BIT_S2|BIT_S3|BIT_S4)),
	  0
};

const LONG STAND_IO_COUPLES[] =					//	SURGE
{
    BIT_DATACPL | BIT_G |(256*(BIT_S1)),
    BIT_DATACPL | BIT_G |(256*(BIT_S2)),
    BIT_DATACPL | BIT_G |(256*(BIT_S1|BIT_S2)),
	  0
};

const LONG *ALL_IO_COUPLES = STAND_IO_COUPLES;	//	SURGE

const LONG STAND_3PHASE_COUPLES[] =					//	SURGE
{
    BIT_GND|(256*(BIT_L1)),
    BIT_GND|(256*(BIT_L2)),
    BIT_GND|(256*(BIT_L3)),
    BIT_GND|(256*(BIT_N)),
    BIT_GND|(256*(BIT_N|BIT_L1|BIT_L2|BIT_L3)),
    BIT_N  |(256*(BIT_L1)),
	 BIT_N  |(256*(BIT_L2)),
    BIT_N  |(256*(BIT_L3)),
    BIT_N  |(256*(BIT_L1|BIT_L2|BIT_L3)),
    BIT_L1 |(256*(BIT_L2)),
    BIT_L1 |(256*(BIT_L3)),
    BIT_L2 |(256*(BIT_L3)),
	  0
};

const LONG ALL_2WIRE_COUPLES[] =							// SURGE
{
    BIT_L2 |(256*(BIT_L1)),
    BIT_GND|(256*(BIT_L1)),
    BIT_GND|(256*(BIT_L2)),
    BIT_GND|(256*(BIT_L1|BIT_L2)),
	  0
};

const LONG *STAND_2WIRE_COUPLES = ALL_2WIRE_COUPLES;

const LONG ALL_505B_COUPLES[] =							// E505B special case
{
    BIT_L2 |(256*(BIT_L1)),
	  0
};

const LONG *STAND_505B_COUPLES = ALL_505B_COUPLES;

const LONG ALL_3PHASE_COUPLES[] =							//	SURGE
{
   BIT_GND|(256*(0 | BIT_L1)),
   BIT_GND|(256*(0 | BIT_L2)),
   BIT_GND|(256*(0 | BIT_L3)),
   BIT_GND|(256*(0 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L2)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L3)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L2 | BIT_L3)),
   BIT_GND|(256*(0 | BIT_L2 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L3 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L2 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L3 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L2 | BIT_L3)),
   BIT_GND|(256*(0 | BIT_L2 | BIT_L3 | BIT_N)),
   BIT_GND|(256*(0 | BIT_L1 | BIT_L2 | BIT_L3 | BIT_N)),

	BIT_N|(256*(0 | BIT_L1)),
   BIT_N|(256*(0 | BIT_L2)),
   BIT_N|(256*(0 | BIT_L3)),
   BIT_N|(256*(0 | BIT_L1 | BIT_L2)),
   BIT_N|(256*(0 | BIT_L1 | BIT_L3)),
   BIT_N|(256*(0 | BIT_L2 | BIT_L3)),
   BIT_N|(256*(0 | BIT_L1 | BIT_L2 | BIT_L3)),

   BIT_L1|(256*(0 | BIT_L2)),
   BIT_L1|(256*(0 | BIT_L3)),
   BIT_L1|(256*(0 | BIT_N)),
   BIT_L1|(256*(0 | BIT_L2 | BIT_L3)),
   BIT_L1|(256*(0 | BIT_L2 | BIT_N)),
   BIT_L1|(256*(0 | BIT_L3 | BIT_N)),
   BIT_L1|(256*(0 | BIT_L2 | BIT_L3 | BIT_N)),

   BIT_L2|(256*(0 | BIT_L1)),
   BIT_L2|(256*(0 | BIT_L3)),
   BIT_L2|(256*(0 | BIT_N)),
	BIT_L2|(256*(0 | BIT_L1 | BIT_L3)),
   BIT_L2|(256*(0 | BIT_L1 | BIT_N)),
   BIT_L2|(256*(0 | BIT_L3 | BIT_N)),
   BIT_L2|(256*(0 | BIT_L1 | BIT_L3 | BIT_N)),

   BIT_L3|(256*(0 | BIT_L1)),
   BIT_L3|(256*(0 | BIT_L2)),
   BIT_L3|(256*(0 | BIT_N)),
   BIT_L3|(256*(0 | BIT_L1 | BIT_L2)),
   BIT_L3|(256*(0 | BIT_L1 | BIT_N)),
   BIT_L3|(256*(0 | BIT_L2 | BIT_N)),
   BIT_L3|(256*(0 | BIT_L1 | BIT_L2 | BIT_N)),
	 0
};

const LONG STAND_EFTCOUPLES[] =							// SURGE
{
	256*BIT_L1,
	256*BIT_L2,
	256*BIT_GND,
	0
};

const LONG STAND_3EFTCOUPLES[] =						 
{
	256*BIT_L1,
	256*BIT_L2,
	256*BIT_L3,
	256*BIT_N,
	256*BIT_GND,
	0
};

const LONG ALL_EFTCOUPLES[] =							
{
	256*(BIT_L1),
	256*(BIT_L1 | BIT_L2),
	256*(BIT_L1 | BIT_GND),
	256*(BIT_L1 | BIT_L2 | BIT_GND),
	256*(BIT_L2),
	256*(BIT_L2 | BIT_GND),
	256*(BIT_GND),
	0
};

const LONG ALL_3EFTCOUPLES[] =							
{
	256*(BIT_L1),
	256*(BIT_L1 | BIT_L2),
	256*(BIT_L1 | BIT_L3),
	256*(BIT_L1 | BIT_N),
	256*(BIT_L1 | BIT_GND),
	256*(BIT_L1 | BIT_L2 | BIT_L3),
	256*(BIT_L1 | BIT_L2 | BIT_N),
	256*(BIT_L1 | BIT_L3 | BIT_N),
	256*(BIT_L1 | BIT_L2 | BIT_GND),
	256*(BIT_L1 | BIT_L3 | BIT_GND),
	256*(BIT_L1 | BIT_N  | BIT_GND),
	256*(BIT_L1 | BIT_L2 | BIT_L3  | BIT_N),
	256*(BIT_L1 | BIT_L2 | BIT_L3  | BIT_GND),
	256*(BIT_L1 | BIT_L2 | BIT_N   | BIT_GND),
	256*(BIT_L1 | BIT_L3 | BIT_N   | BIT_GND),
	256*(BIT_L1 | BIT_L2 | BIT_L3  | BIT_N   | BIT_GND),

	256*(BIT_L2),
	256*(BIT_L2 | BIT_L3),
	256*(BIT_L2 | BIT_N),
	256*(BIT_L2 | BIT_GND),
	256*(BIT_L2 | BIT_L3 | BIT_N),
	256*(BIT_L2 | BIT_L3 | BIT_GND),
	256*(BIT_L2 | BIT_N  | BIT_GND),
	256*(BIT_L2 | BIT_L3 | BIT_N   | BIT_GND),

	256*(BIT_L3),
	256*(BIT_L3 | BIT_N),
	256*(BIT_L3 | BIT_GND),
	256*(BIT_L3 | BIT_N  | BIT_GND),

	256*(BIT_N),
	256*(BIT_N  | BIT_GND),

	256*(BIT_GND),
	0
};

long __declspec(dllexport) CALLBACK SubListProc(HWND hwnd,UINT message, UINT wPAram, LONG lParam);

COUPLE::COUPLE()
{
	running=0;
	Update_Values=FALSE;
	Update_Window=NULL;
	Update_Message=0;
	Front_Only=0;
	Last_Couple = 0;
	Last_Data_Couple = BIT_DATACPL;
	Five_Chan_Io = TRUE;
	strcpy(front_name,"Front");
}

long COUPLE::CoupleType()
{
	switch (mode)
	{
	case MODE_FRONT:
		return 0;

	case MODE_FIXED:
		return selected_coupler;

	case MODE_STAND_AC:
	case MODE_ALL_AC:  
		return TYPE_CPLAC;

	case MODE_STAND_IO:
	case MODE_ALL_IO:  
		return TYPE_CPLIO;

	case MODE_ALL:  
		return TYPE_CPLAC | TYPE_CPLIO;

	case MODE_NEW_LIST:  
	case MODE_LIST:  
	default:
		{
			int cnt=( SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
			if (cnt == 0) return allow_couple;

			long cp_mode = 0;
			for (int i=0; i<cnt; i++)
			{
				LONG cpl=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i+1,0L);
				if (cpl & BIT_DATACPL) 
					cp_mode |= TYPE_CPLIO;
				else
					cp_mode |= TYPE_CPLAC;
			}

			return cp_mode;
		}
	}
}

inline BOOL IsE505B(int bay)
{
	return (calset[bay].id == E505B_BOXT);
}

void COUPLE::Init(HANDLE hInstance,HWND hwnd, RECT& rect, HICON hicon,LPSTR text, BOOL ThreePhase)
{
	icon=hicon;
	Three_Phase=ThreePhase;

	allow_couple   = 0;

	if (AC_COUPLER != -1) 
	for (int i = 0; i < 80,wave_list[i].mod_num!=-1; i++)
	{
		if (wave_list[i].line_couple)
		{
			allow_couple |= TYPE_CPLAC;
			break;
		}
	}

	io_net  = -1;
	if (IO_COUPLER != -1)
	{
		for (int i = 0; i < 80,wave_list[i].mod_num!=-1; i++)
		{
			if (wave_list[i].io_couple)
			{
				io_net  = wave_list[i].mod_num;
				io_wave = wave_list[i].waven;
				break;
			}
		}
		if (io_net == -1) 
			IO_COUPLER = -1;
		else  
			allow_couple |= TYPE_CPLIO;
	}

	if (surge_app)
		create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_SRG_COUPLE));
	else 
		create(hInstance,hwnd,rect,MAKEINTRESOURCE(IDD_EFT_COUPLE));

	SetWindowLong(GetDlgItem(hDlgChild,IDD_ICON),4,(long)hicon);
	ShowWindow(hDlgChild,SW_SHOWNORMAL);
	ShowWindow(GetDlgItem(hDlgChild,IDD_FRONT_LAB),SW_HIDE);
	SetDlgItemText(hDlgChild,IDD_WIN_TEXT,text);
	strcpy(Obj_Name,text);
	Captured=FALSE;
	created=TRUE;
}

int ValidCoupleMode(LONG set_cpl,BOOL Five_Chan_Io,HWND hDlg)
{
	BOOL data = set_cpl & BIT_DATACPL;
	if (data) 
	{
		set_cpl &= ~BIT_DATACPL;
		set_cpl &= ~BIT_CMODEON;
	}

	if ((data && Five_Chan_Io) || (!data && SURGE_THREE_PHASE))
	{
		if ((set_cpl & 255)==0) return 0;

		if (((set_cpl & 255)==BIT_GND)|
		((set_cpl & 255)==BIT_N)  |
		((set_cpl & 255)==BIT_L1) |
		((set_cpl & 255)==BIT_L2) |
		((set_cpl & 255)==BIT_L3))  // Low mode is ok !
		{
			if (set_cpl >>8) 			// At least one mode hi
			return 1;
		}

		return 0;
	}
	else
	{
		int rv  = SendDlgItemMessage(hDlg,IDD_COUPLER,CB_GETCURSEL,0,0L);
		int bay = SendDlgItemMessage(hDlg,IDD_COUPLER,CB_GETITEMDATA,rv,0L); 
        if ( IsE505B( bay ) )  // special case, must be L1 vs. L2
        {
            if (set_cpl == (BIT_L2 | (256*BIT_L1)))
                return 1;
            else
                return 0;
        }
		if (set_cpl==(BIT_GND |(256*BIT_L1))) return 1;
		else if (set_cpl==(BIT_GND |(256*BIT_L2))) return 1;
		else if (set_cpl==(BIT_GND |(256*(BIT_L1|BIT_L2)))) return 1;
		else if (!data)
		    if (set_cpl==(BIT_L2 | (256*BIT_L1))) return 1;

		return 0;
	}
}

int COUPLE::valid_mode(LONG set_cpl)
{
	if (!surge_app) return 1;

	if(set_cpl & BIT_DATACPL)
	{
		if ((allow_couple & TYPE_CPLIO) == 0) 
			return 0;
	}
	else
	{
		if ((allow_couple & TYPE_CPLAC) == 0) 
			return 0;
	}
	return ::ValidCoupleMode(set_cpl,Five_Chan_Io, hDlgChild);
}

long COUPLE::SelCouplerType()
{
	if (SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETCOUNT,0,0L))
	{
		int rv  = SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETCURSEL,0,0L);
		int bay = SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETITEMDATA,rv,0L); 

		if (calset[bay].type & TYPE_CPLIO) return TYPE_CPLIO;
		if (calset[bay].type & TYPE_CPLAC) return TYPE_CPLAC;
	}

	return 0;
}

void  COUPLE::Set5IOLines(BOOL state)
{
	Five_Chan_Io = state;

	switch (mode)
	{
	case MODE_FIXED:
		Show_Valid();						// JLR, 10/12/95
		break;

	case MODE_LIST:
		InvalidateRect(GetDlgItem(hDlgChild,IDD_LIST),NULL,TRUE);
		break;

	//JLR, 10/10/95  fix: ALL list turned into ALL IO list,after network changed 

	case MODE_ALL:				// JLR, 10/10/95
	case MODE_STAND_IO:
	case MODE_ALL_IO:
		Set_Mode(mode);
		break;

	default:
		break;
	}
}

void  COUPLE::SetAllowCouple(long state)
{
	allow_couple = state;

	if (state)
	{
		if (selected_coupler == TYPE_CPLIO)
		{
			if (!(state & TYPE_CPLIO)) 
				SelectCoupler(TYPE_CPLAC);
		}
		else
		{
			if (!(state & TYPE_CPLAC)) 
				SelectCoupler(TYPE_CPLIO);
		}
		InitModeComboBox();

		if (mode == MODE_LIST)
		{
			InvalidateRect(GetDlgItem(hDlgChild,IDD_LIST),NULL,TRUE);
		}
		else
		{
			if (!(state & TYPE_CPLAC))
			if (mode == MODE_STAND_AC || mode == MODE_ALL_AC || mode == MODE_ALL)
				Set_Mode(MODE_NEW_LIST);

			if (!(state & TYPE_CPLIO))
			{
				if (mode == MODE_STAND_IO || mode == MODE_ALL_IO || mode == MODE_ALL)
				Set_Mode(MODE_NEW_LIST);
			}
		}
	}

	SetFrontOnly(!allow_couple);
}

void  COUPLE::SetFrontOnly(long only)
{
	Front_Only=only;
	if (Front_Only) 
	{
		Set_Mode(MODE_FRONT);
		ShowWindow(GetDlgItem(hDlgChild,IDD_FRONT_LAB),SW_SHOWNORMAL);
	}
	else 
		ShowWindow(GetDlgItem(hDlgChild,IDD_FRONT_LAB),SW_HIDE);
	
	EnableWindow(GetDlgItem(hDlgChild,IDD_CB),!Front_Only);
}


void COUPLE::Change_Icon(HICON hicon)
{ 
	icon=hicon;
	SendMessage(GetDlgItem(hDlgChild,IDD_ICON), STM_SETICON, (WPARAM)hicon, 0);
}


void COUPLE::Next_State(LONG Bit_MASK,LONG &cpl)
{
	if (!surge_app)				// EFT
	{  
		cpl ^=Bit_MASK*256;
		return;
	}

	if (cpl & Bit_MASK) 
		cpl = (cpl & ~Bit_MASK)|(Bit_MASK*256);
	else if (cpl & (Bit_MASK*256))
		cpl=cpl & ~(Bit_MASK*256);
	else
		cpl |= Bit_MASK;

	if (Bit_MASK==BIT_GND) 
		cpl &= (~(BIT_GND*256)); 
}


void COUPLE::Set_Button_State(HWND hDlg,LONG IDD_MASK,LONG BIT_MASK)
{
	LONG sv;
	LONG cpl = IDD_MASK >= IDD_S1 && IDD_MASK <= IDD_G ? Last_Data_Couple : Last_Couple;

	if (cpl & BIT_MASK) 
		sv=1;
	else if (cpl & (BIT_MASK*256)) 
		sv=2;
	else 
		sv=0;

	SendDlgItemMessage(hDlg,IDD_MASK,BM_SETCHECK,sv,0l);
}


LONG COUPLE::ReportSteps(void)
{
	if (mode > MODE_FIXED) 
		return (SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
	else 
		return 1;
}


void COUPLE::InitCouplerComboBox()
{
	int rv;

	if (allow_couple & TYPE_CPLAC)
	{
		rv = SendDlgItemMessage(hDlgChild, IDD_COUPLER, CB_ADDSTRING,0, (LONG)((LPSTR)calset[AC_COUPLER].name));
		SendDlgItemMessage(hDlgChild, IDD_COUPLER, CB_SETITEMDATA,	rv, (LONG)AC_COUPLER);
	}

	if (allow_couple & TYPE_CPLIO)
	{
		rv = SendDlgItemMessage(hDlgChild, IDD_COUPLER, CB_ADDSTRING,0, (LONG)((LPSTR)calset[IO_COUPLER].name));
		SendDlgItemMessage(hDlgChild, IDD_COUPLER, CB_SETITEMDATA,	rv, (LONG)IO_COUPLER);
	}

}

void COUPLE::InitModeComboBox()
{
	int m = 0;
	if (SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCOUNT,0,0L))
	{
		m  = SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCURSEL,0,0L);
		m  = SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETITEMDATA,m,0L);
	}

	SendDlgItemMessage(hDlgChild,IDD_CB,CB_RESETCONTENT,0,0L);
	int i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)front_name));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_FRONT);

	i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Fixed"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_FIXED);

	i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"List"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_LIST);

	i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"New List"));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_NEW_LIST);

	if (allow_couple & TYPE_CPLAC)
	{
		i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Standard AC"));
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_STAND_AC);

		i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"All  AC"));
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_ALL_AC);
	}

	if (allow_couple & TYPE_CPLIO)
	{
		i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"Standard I/O"));
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_STAND_IO);

		i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"All  I/O"));
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_ALL_IO);
	}

	if ((allow_couple & TYPE_CPLAC) && (allow_couple & TYPE_CPLIO))
	{
		i = SendDlgItemMessage(hDlgChild,IDD_CB,CB_ADDSTRING,0,(LONG)((LPSTR)"All"));
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETITEMDATA,i, (LONG)MODE_ALL);
	}

	if (!Set_Mode(m,FALSE)) 
		Set_Mode(MODE_FRONT,FALSE);
}

void COUPLE::InitCoupleControls()
{
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_START);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_END);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,0,0L);

	InitCouplerComboBox();
	InitModeComboBox();

	if (surge_app)
	{
		clamp_btn.Create(hDlgChild,IDD_AUX,CLAMP_NAMES,IDD_AUX_TEXT);
		data_btn.Create(hDlgChild, IDD_DATABTN,DATA_NAMES,IDD_DATA_TEXT);
		cmode_btn.Create(hDlgChild,IDD_CMODEBTN,CMODE_NAMES);

		//  Default:  do not show the aux & data buttons
		//	 (this needs more complicated logic to hide, show)

		clamp_btn.Show(FALSE);
		data_btn.Show(FALSE);
	}

	ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_LIST),  SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_HIDE);

	ShowWindow(GetDlgItem(hDlgChild,IDD_GND),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_N),  SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L1), SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L2), SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L3), SW_HIDE);

	if (surge_app)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_S1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S2), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S4), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_G),  SW_HIDE);
		cmode_btn.Show(FALSE);
	}

	ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_SHOWNORMAL);

	if (SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETCOUNT,0,0L))
		SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_SETCURSEL,0,0L);

	selected_coupler = SelCouplerType();

	ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER_TEXT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER), SW_HIDE);

	if (IO_COUPLER != -1)
	{
		PrintfEcatOutAtIO();
		data_btn  = (unsigned int)SendMessage(hWndComm,KT_DATA_ON_QUERY,0,0);
		clamp_btn = (unsigned int)SendMessage(hWndComm,KT_CLAMP_QUERY,0,0L);
		clamp_btn.Show(TRUE);
		data_btn.Show(TRUE);
	}
}

void COUPLE::CouplerChanged()
{
//JLR, 10/10/95 restructured code, because was doing stuff only for MODE_FIXED
//				  any way.
 
	if (mode != MODE_FIXED) return;

	if (selected_coupler & TYPE_CPLIO)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_GND),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_L1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_L2), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_N), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_L3),SW_HIDE);

		cmode_btn.Show(TRUE);

		ShowWindow(GetDlgItem(hDlgChild,IDD_S1),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S2),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_G), SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S3),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S4),SW_SHOWNORMAL);
	}
	else
	{
		if (surge_app)
		{
			ShowWindow(GetDlgItem(hDlgChild,IDD_S1),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_S2),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_S3),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_S4),SW_HIDE);
			ShowWindow(GetDlgItem(hDlgChild,IDD_G), SW_HIDE);
			cmode_btn.Show(FALSE);
		}														

		ShowWindow(GetDlgItem(hDlgChild,IDD_L1),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_L2),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_GND),SW_SHOWNORMAL);
		if (Three_Phase)
		{
			ShowWindow(GetDlgItem(hDlgChild,IDD_N),SW_SHOWNORMAL);
			ShowWindow(GetDlgItem(hDlgChild,IDD_L3),SW_SHOWNORMAL);
		}
	}

	//JLR, 10/10/95 code below added to fix: "Invalid Mode" was displayed or 
	// 					  hidden incorrectly when Coupler selection changed

	Show_Valid();			// JLR, 10/12/95
}

void COUPLE::SetModeFront()
{
	ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_LIST),SW_HIDE);

	ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER_TEXT),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER), SW_HIDE);
	if (surge_app)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_S1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S2), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S4), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_G),  SW_HIDE);
		cmode_btn.Show(FALSE);
	}

	ShowWindow(GetDlgItem(hDlgChild,IDD_GND),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_N),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L1),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L2),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L3),SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_INVALID),SW_HIDE);
}

void COUPLE::ShowIoControls()
{
    if (!surge_app) return;

	  int sw = TRUE;

	  if (!(allow_couple & TYPE_CPLIO)) sw = FALSE;
	  else
	  if (mode == MODE_FRONT || mode == MODE_STAND_AC || mode == MODE_ALL_AC) 
	     sw = FALSE;
	  else
	  if (mode == MODE_FIXED && !(selected_coupler & TYPE_CPLIO))
	     sw = FALSE;
			 
	  clamp_btn.Show(sw);
	  data_btn.Show(sw);
}

void COUPLE::SetModeList(HWND hDlg)
{
	ShowWindow(GetDlgItem(hDlgChild,IDD_GND),   SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_N),     SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L1),    SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L2),    SW_HIDE);
	ShowWindow(GetDlgItem(hDlgChild,IDD_L3),    SW_HIDE);

	ShowWindow(GetDlgItem(hDlgChild,IDD_INVALID),SW_HIDE);

	if (surge_app)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_S1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S2), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_S4), SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_G),  SW_HIDE);
		cmode_btn.Show(FALSE);

		ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER_TEXT),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_COUPLER),     SW_SHOWNORMAL);
	}

	if (mode == MODE_LIST || mode == MODE_NEW_LIST)
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_SHOWNORMAL);
		if (mode == MODE_NEW_LIST)
		{
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_START);
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_END);
		}
	}
	else
	{
		ShowWindow(GetDlgItem(hDlgChild,IDD_INSERT),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_DELETE),SW_HIDE);

		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_START);

		int rv  = SendDlgItemMessage(hDlg,IDD_COUPLER,CB_GETCURSEL,0,0L);
		int bay = SendDlgItemMessage(hDlg,IDD_COUPLER,CB_GETITEMDATA,rv,0L); 
        
        const LONG *cpls;
		LONG  mask = 0;
		switch (mode)
		{
		case MODE_STAND_IO:
			cpls = Five_Chan_Io ? STAND_5IO_COUPLES : STAND_IO_COUPLES;
			if (cmode_btn == CMODE_SYMM)	
				mask = BIT_CMODEON;
			break;

		case MODE_ALL_IO:
			cpls = Five_Chan_Io ? ALL_5IO_COUPLES : ALL_IO_COUPLES;
			if (cmode_btn == CMODE_SYMM)	
				mask = BIT_CMODEON;
			break;

		case MODE_STAND_AC:
			if (surge_app)
            {
                if (Three_Phase)
                    cpls = STAND_3PHASE_COUPLES;
                else if (IsE505B( bay ))
                    cpls = STAND_505B_COUPLES;
                else
                    cpls = STAND_2WIRE_COUPLES;
            }
			else
				cpls = Three_Phase ? STAND_3EFTCOUPLES : STAND_EFTCOUPLES;
			break;

        case MODE_ALL_AC:
		case MODE_ALL:
		default:
			if (surge_app)
            {
                if (Three_Phase)
                    cpls = ALL_3PHASE_COUPLES;
                else if (IsE505B( bay ))
                    cpls = ALL_505B_COUPLES;
                else
                    cpls = ALL_2WIRE_COUPLES;
            }
			else
				cpls = Three_Phase ? ALL_3EFTCOUPLES   : ALL_EFTCOUPLES;
			break;
		}

		while (*cpls)
		{
			SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)(*cpls | mask));
			cpls++;
		}
		if (mode == MODE_ALL)
		{
			if (cmode_btn == CMODE_SYMM)	
				mask = BIT_CMODEON;
			cpls = Five_Chan_Io ? ALL_5IO_COUPLES : ALL_IO_COUPLES;
			while (*cpls)
			{
				SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)(*cpls | mask));
				cpls++;
			}
		}

		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_END);
	}

	if (mode != MODE_LIST)
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,0,0L);

	ShowWindow(GetDlgItem(hDlgChild,IDD_LIST),  SW_SHOWNORMAL);
}

void COUPLE::PrintfEcatOutAtIO()
{
	if (IO_COUPLER == -1) return;

	if (IO_COUPLER != SendMessage(hWndComm,KT_SRG_OUTPUT_QUERY,0,0))
	{
		SendMessage(hWndComm,KT_SRG_NETWORK_SETSTATE,((LONG)io_net & 0x0F),0L);
		SendMessage(hWndComm,KT_SRG_WAVE_SETSTATE,((LONG)io_wave & 0x0ff),0L);
		SendMessage(hWndComm,KT_SRG_OUTPUT_SETSTATE,IO_COUPLER,0L);
	}
}

#define DIS  (*((LPDRAWITEMSTRUCT)(lParam)))
#define MIS  (*((LPMEASUREITEMSTRUCT)(lParam)))

LONG COUPLE::show_cpl_display(LONG cp,LONG mask,LPARAM lParam, char *cl)
{ 
	POINT cp1,cp2;
	LONG rv;
	char chs[10];
	memset(chs, 0, sizeof(chs));
	memset(chs, ' ', 5);

	GetCurrentPositionEx(DIS.hDC,&cp1);
	rv = cp1.x;

	COLORREF oldTxCol;
	UINT uiOldMode = SetBkMode(DIS.hDC, TRANSPARENT);
	HBRUSH hBrush = NULL;
	if (strchr(cl,'y'))								//symm,asymm
	{
		oldTxCol = SetTextColor(DIS.hDC,RGB(0,0,0));
		hBrush = CreateSolidBrush(RGB(192,192,192));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(DIS.hDC, hBrush);
		Rectangle(DIS.hDC,cp1.x-1,cp1.y,cp1.x+5*tmW,cp1.y+tmH-2);
		TextOut(DIS.hDC,0,0,cl,strlen(cl));
		SelectObject(DIS.hDC, hOldBrush);
		
		GetCurrentPositionEx(DIS.hDC,&cp2);
		MoveToEx(DIS.hDC,cp1.x+5*tmW+1,cp2.y,NULL);
		SetTextColor(DIS.hDC,oldTxCol);
	}
	else
	{
		strcpy(&chs[1],cl);
		if (cp & mask)
		{  
			hBrush = CreateSolidBrush(HICOLOR);
			oldTxCol = SetTextColor(DIS.hDC,HITEXTCOLOR);
			strcat(chs,"- ");
		}
		else if (cp & (256*mask))
		{  
			hBrush = CreateSolidBrush(LOCOLOR);
			oldTxCol = SetTextColor(DIS.hDC,RGB(0,0,0));
			strcat(chs,"+ ");
		}
		else 
		{  
			hBrush = CreateSolidBrush(GetBkColor(DIS.hDC));
			oldTxCol = SetTextColor(DIS.hDC,RGB(0,0,0));
			strcat(chs,"  ");
		}

		HBRUSH hOldBrush = (HBRUSH) SelectObject(DIS.hDC, hBrush);
		local_W_x=(int)(4.5*(float)tmW);
		Rectangle(DIS.hDC,cp1.x-1,cp1.y,cp1.x+4*tmW+1,cp1.y+tmH-2);
		SelectObject(DIS.hDC, hOldBrush);
		TextOut(DIS.hDC,0,0,chs,strlen(chs));
		
		GetCurrentPositionEx(DIS.hDC,&cp2);
		MoveToEx(DIS.hDC,cp1.x+4*tmW+1,cp2.y,NULL);
		TextOut(DIS.hDC,0,0,"   ",3);
		SetTextColor(DIS.hDC,oldTxCol);
	}

	if (hBrush != NULL)
		DeleteObject(hBrush);

	SetBkMode(DIS.hDC, uiOldMode);
	return rv;
}

void COUPLE::MesureListItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMIS)
{
	if (!tmH)
	{ 
		TEXTMETRIC tm;
		HDC hdc=GetDC(hDlg);
		GetTextMetrics(hdc,&tm);
		tmH=tm.tmHeight;
		ReleaseDC(hDlg,hdc);
	}

	lpMIS->itemHeight=tmH+2; 
}

BOOL COUPLE::DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{  
	POINT cp2;
	char buff[20];
	int  i;
	LONG cp;
	POINT ptOld;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			TEXTMETRIC tm;
			HDC hdc=GetDC(hDlg);
			GetTextMetrics(hdc,&tm);
			tmH=tm.tmHeight;
			tmW=tm.tmAveCharWidth;
			ReleaseDC(hDlg,hdc);

			hDlgChild = hDlg;

			newlpfn=(LPFN)SubListProc;
			oldlistlpfn=(LPFN) GetWindowLong(GetDlgItem(hDlg,IDD_LIST),GWL_WNDPROC);
			SetWindowLong(GetDlgItem(hDlg,IDD_LIST),GWL_WNDPROC,(LONG)newlpfn);

			// Subclass buttons

			// L1
			UINT arrIds[MAX_BTN_STATES];
			arrIds[0] = IDD_L1;
			arrIds[1] = IDD_L1_NEG;
			arrIds[2] = IDD_L1_POS;
			btnL1.Subclass(hDlg, IDD_L1, ghinst, arrIds, 3);

			// L2
			arrIds[0] = IDD_L2;
			arrIds[1] = IDD_L2_NEG;
			arrIds[2] = IDD_L2_POS;
			btnL2.Subclass(hDlg, IDD_L2, ghinst, arrIds, 3);

			// L3
			arrIds[0] = IDD_L3;
			arrIds[1] = IDD_L3_NEG;
			arrIds[2] = IDD_L3_POS;
			btnL3.Subclass(hDlg, IDD_L3, ghinst, arrIds, 3);
			
			// PE
			arrIds[0] = IDD_GND;
			arrIds[1] = IDD_GND_NEG;
			arrIds[2] = IDD_GND_POS;
			btnPE.Subclass(hDlg, IDD_GND, ghinst, arrIds, 3);

			// N
			arrIds[0] = IDD_N;
			arrIds[1] = IDD_N_NEG;
			arrIds[2] = IDD_N_POS;
			btnN.Subclass(hDlg, IDD_N, ghinst, arrIds, 3);

			// S1
			arrIds[0] = IDD_S1;
			arrIds[1] = IDD_S1_NEG;
			arrIds[2] = IDD_S1_POS;
			btnS1.Subclass(hDlg, IDD_S1, ghinst, arrIds, 3);

			// S2
			arrIds[0] = IDD_S2;
			arrIds[1] = IDD_S2_NEG;
			arrIds[2] = IDD_S2_POS;
			btnS2.Subclass(hDlg, IDD_S2, ghinst, arrIds, 3);

			// S3
			arrIds[0] = IDD_S3;
			arrIds[1] = IDD_S3_NEG;
			arrIds[2] = IDD_S3_POS;
			btnS3.Subclass(hDlg, IDD_S3, ghinst, arrIds, 3);

			// S4
			arrIds[0] = IDD_S4;
			arrIds[1] = IDD_S4_NEG;
			arrIds[2] = IDD_S4_POS;
			btnS4.Subclass(hDlg, IDD_S4, ghinst, arrIds, 3);

			// G
			arrIds[0] = IDD_G;
			arrIds[1] = IDD_G_NEG;
			arrIds[2] = IDD_G_POS;
			btnG.Subclass(hDlg, IDD_G, ghinst, arrIds, 3);

			InitCoupleControls();

			mode = MODE_FRONT;
		}
		return FALSE;

	case WM_DRAWITEM:

		switch (wParam)
		{
		case IDD_S1:
		case IDD_S2:
		case IDD_S3:
		case IDD_S4:
		case IDD_G:
		case IDD_L1:
		case IDD_L2:
		case IDD_L3:
		case IDD_GND:
		case IDD_N:
			SendDlgItemMessage(hDlg, (int) wParam, message, wParam, lParam);
			return TRUE;

		case IDD_LIST:
			cp = DIS.itemData;
			_itoa(DIS.itemID,buff,10);
			for (i=0; buff[i]; i++); 

			MoveToEx(DIS.hDC,DIS.rcItem.left,DIS.rcItem.top+1,&ptOld);
			SetTextAlign(DIS.hDC,TA_UPDATECP);

			if (cp == CPLIST_START) 
				TextOut(DIS.hDC,0,0,"Start of List",13);
			else if (cp == CPLIST_END)
				TextOut(DIS.hDC,0,0,"End of List",11);
			else
			{
				int arrow_pos;

				_itoa(DIS.itemID,buff,10);

				for (i=0; buff[i]; i++);
				while (i<5)  
					buff[i++]=' ';
				buff[i]=0;

				SelectObject(DIS.hDC, GetStockObject(HOLLOW_BRUSH));

				TextOut(DIS.hDC,0,0,(LPSTR)buff, 5);

				if (cp & BIT_DATACPL)
				{
					local_1_x=show_cpl_display(cp,BIT_S1,lParam,"S1");
					GetCurrentPositionEx(DIS.hDC,&cp2);
					local_W_x=cp2.x-local_1_x;

					local_2_x=show_cpl_display(cp,BIT_S2,lParam,"S2");
					local_3_x=show_cpl_display(cp,BIT_S3,lParam,"S3");
					local_4_x=show_cpl_display(cp,BIT_S4,lParam,"S4");
					local_5_x=show_cpl_display(cp,BIT_G, lParam,"G  ");
					arrow_pos = 36;

					if (cp & BIT_CMODEON)
						local_s_x=show_cpl_display(cp,BIT_CMODEON, lParam,"Symm ");
					else
						local_s_x=show_cpl_display(cp,BIT_CMODEON, lParam,"Asym ");
				}
				else
				{
					local_s_x = 0;  
					local_1_x=show_cpl_display(cp,BIT_L1,lParam,"L1");
					GetCurrentPositionEx(DIS.hDC,&cp2);
					local_W_x=cp2.x-local_1_x;

					if (Three_Phase)
					{ 
						local_2_x=show_cpl_display(cp,BIT_L2, lParam,"L2");
						local_3_x=show_cpl_display(cp,BIT_L3, lParam,"L3");
						local_4_x=show_cpl_display(cp,BIT_N,  lParam,"N  ");
						local_5_x=show_cpl_display(cp,BIT_GND,lParam,"PE");
						arrow_pos = 30;
					}
					else
					{
						local_3_x =	local_4_x = 0;
						local_2_x=show_cpl_display(cp,BIT_L2, lParam,"L2");
						local_5_x=show_cpl_display(cp,BIT_GND,lParam,"PE");
						arrow_pos = 20;
					}
				}
				MoveToEx(DIS.hDC,local_1_x+arrow_pos*tmW-2,cp2.y,NULL);

				if (!show && !valid_mode(cp))				// show == show run
				{
					SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
					SetTextColor(DIS.hDC,RGB(255,0,0));
					TextOut(DIS.hDC,0,0,"INVALID",7);
					SetTextColor(DIS.hDC,RGB(0,0,0));
				}
				else if ((DIS.itemState & ODS_SELECTED) && show)
				{  
					SetBkColor(DIS.hDC,RGB(255,255,0));
					SetTextColor(DIS.hDC,RGB(0,0,0));
					TextOut(DIS.hDC,0,0," <--- ",6);
					SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
				}
				else
				{  
					SetBkColor(DIS.hDC,GetSysColor(COLOR_WINDOW));
					TextOut(DIS.hDC,0,0,"            ",12);
				}
			}
			return TRUE;

		default:
			break;
		}


	case WM_MEASUREITEM: 
		if ((UINT) wParam != IDD_LIST) return FALSE;
		MesureListItem(hDlg, (LPMEASUREITEMSTRUCT) (lParam));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDD_INSERT:
			{
				cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
				if (CPLIST_END != SendDlgItemMessage(hDlg,IDD_LIST,LB_GETITEMDATA,cp+1,0L))
					cp++;

				LONG cpl = selected_coupler & TYPE_CPLIO ? Last_Data_Couple : Last_Couple;
				SendDlgItemMessage(hDlg,IDD_LIST,LB_INSERTSTRING,cp+1,(LONG)cpl);
				SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX, cp+1,0L);
				ichanged=TRUE;
			}
			return TRUE;

		case IDD_DELETE:
			cp=SendDlgItemMessage(hDlg,IDD_LIST,LB_GETTOPINDEX,0,0L);
			if (CPLIST_END != SendDlgItemMessage(hDlg,IDD_LIST,LB_GETITEMDATA,cp+1,0L))
				SendDlgItemMessage(hDlg,IDD_LIST,LB_DELETESTRING,cp+1,0L);
			SendDlgItemMessage(hDlg,IDD_LIST,LB_SETTOPINDEX,cp,0L);
			ichanged=TRUE;
			return TRUE;

		case  IDD_GND:
			Next_State(BIT_GND,Last_Couple);
			Set_Button_State(hDlg,IDD_GND,BIT_GND);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_N:
			Next_State(BIT_N,Last_Couple);
			Set_Button_State(hDlg,IDD_N,BIT_N);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_L1:
			Next_State(BIT_L1,Last_Couple);
			Set_Button_State(hDlg,IDD_L1,BIT_L1);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_L2:
			Next_State(BIT_L2,Last_Couple);
			Set_Button_State(hDlg,IDD_L2,BIT_L2);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_L3:
			Next_State(BIT_L3,Last_Couple);
			Set_Button_State(hDlg,IDD_L3,BIT_L3);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_S1:
			Next_State(BIT_S1,Last_Data_Couple);
			Set_Button_State(hDlg,IDD_S1,BIT_S1);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_S2:
			Next_State(BIT_S2,Last_Data_Couple);
			Set_Button_State(hDlg,IDD_S2,BIT_S2);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_S3:
			Next_State(BIT_S3,Last_Data_Couple);
			Set_Button_State(hDlg,IDD_S3,BIT_S3);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_S4:
			Next_State(BIT_S4,Last_Data_Couple);
			Set_Button_State(hDlg,IDD_S4,BIT_S4);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case  IDD_G:
			Next_State(BIT_G,Last_Data_Couple);
			Set_Button_State(hDlg,IDD_G,BIT_G);
			Show_Valid();									// JLR, 10/12/95
			ichanged=TRUE;
			return TRUE;

		case IDD_CMODEBTN:
			{
				++cmode_btn;
				Last_Data_Couple ^= BIT_CMODEON;
				Show_Valid();									// JLR, 10/12/95
				ichanged=TRUE;
			}
			return TRUE;

		case IDD_COUPLER:
			selected_coupler = SelCouplerType();
			if (!(allow_couple)) return TRUE;

			if (selected_coupler == TYPE_CPLIO)
			{
				if (!(allow_couple & TYPE_CPLIO))
				{
					SelectCoupler(TYPE_CPLAC);				//select back
					return TRUE;
				}
			}
			else if (!(allow_couple & TYPE_CPLAC))
			{
				SelectCoupler(TYPE_CPLIO);			  //select back
				return TRUE;
			}
			CouplerChanged();

			ichanged=TRUE;
			return TRUE;

		case IDD_DATABTN:
			{
				PrintfEcatOutAtIO();
				unsigned int state = ++data_btn;
				SendMessage(Update_Window,KT_DATA_ON,state,0L);
				ichanged=TRUE;
			}
			return TRUE;

		case IDD_AUX:
			{
				PrintfEcatOutAtIO();
				unsigned int state = ++clamp_btn;
				SendMessage(Update_Window,KT_CLAMP_STATE,state,0L);
				ichanged=TRUE;
			}
			return TRUE;

		case IDD_CB:
			switch (HIWORD(wParam))
			{ 
			case CBN_SELCHANGE:
				ichanged=TRUE;
				i = SendDlgItemMessage(hDlg,IDD_CB,CB_GETCURSEL,0,0L);
				mode  = SendDlgItemMessage(hDlg,IDD_CB,CB_GETITEMDATA,i,0L);

				switch(mode)
				{ 
				case MODE_FRONT: 
					SetModeFront();
					return TRUE;

				case MODE_FIXED:
					if (surge_app)
					{
						ShowWindow(GetDlgItem(hDlg,IDD_COUPLER_TEXT),SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg,IDD_COUPLER),     SW_SHOWNORMAL);
					}
					ShowWindow(GetDlgItem(hDlg,IDD_INSERT), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg,IDD_DELETE), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg,IDD_LIST),   SW_HIDE);
					ShowWindow(GetDlgItem(hDlg,IDD_INVALID),SW_SHOWNORMAL);

					selected_coupler = SelCouplerType();
					CouplerChanged();
					return TRUE;

				//JLR 10/10/95 - fix: FRONT mode loaded from the test file instead of
				// 					  the list created as new list
				case MODE_NEW_LIST:
					SetModeList(hDlg);
					Set_Mode(MODE_LIST,FALSE);
					mode = MODE_LIST;			 //JLR, 10/10/95 
					Change_Ok=TRUE;
					return TRUE;

				case MODE_LIST:
					SetModeList(hDlg);
					Change_Ok=TRUE;
					return TRUE;

				case MODE_STAND_AC:
				case MODE_ALL_AC:
				case MODE_STAND_IO:
				case MODE_ALL_IO:
				case MODE_ALL:
					SetModeList(hDlg);
					Change_Ok=FALSE;
					return TRUE;
				} // switch(mode)

			case CBN_SETFOCUS:
			case CBN_KILLFOCUS:
			case CBN_DROPDOWN:
			case CBN_DBLCLK:
				return TRUE;
			}
			break;
		}
	} // switch (message)

	return FALSE;
}
#undef DIS
#undef MIS

long __declspec(dllexport) CALLBACK SubListProc(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	long rv;
	COUPLE FAR *lpCOUPLE=(COUPLE FAR *)GetWindowLong(GetParent(hwnd),DWL_USER);
	if (lpCOUPLE!=NULL)
	{
		rv=((*lpCOUPLE).process(hwnd,message,wParam,lParam));
		return rv;
	}
	else 
		return DefWindowProc(hwnd,message,wParam,lParam);
}

long COUPLE::process(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN: 
		{ 
			if (!Change_Ok) return 0;

			int first;
			RECT middle;
			first=SendMessage(hwnd,LB_GETTOPINDEX,0,0L);
			SendMessage(hwnd,LB_GETITEMRECT,first+1,(LONG)((LPRECT)&middle));

			if (HIWORD(lParam)<middle.top) HitRow=first;
			else if (HIWORD(lParam)>middle.bottom) HitRow=first+2;
			else HitRow=first+1;

			if ((LOWORD(lParam) > local_1_x) && (LOWORD(lParam)<(local_1_x+local_W_x)))
				Hit_x=local_1_x;	 
			else if ((LOWORD(lParam) > local_2_x) && (LOWORD(lParam)<(local_2_x+local_W_x)))
				Hit_x=local_2_x;
			else if ((LOWORD(lParam) > local_3_x) && (LOWORD(lParam)<(local_3_x+local_W_x)))
				Hit_x=local_3_x;
			else if ((LOWORD(lParam) > local_4_x) && (LOWORD(lParam)<(local_4_x+local_W_x)))
				Hit_x=local_4_x;	 
			else if ((LOWORD(lParam) > local_5_x) && (LOWORD(lParam)<(local_5_x+local_W_x)))
				Hit_x=local_5_x;
			else if ((LOWORD(lParam) > local_s_x) && (LOWORD(lParam)<(local_s_x+local_W_x)))
				Hit_x=local_s_x;
			else
			{  
				HitRow=-1; 
				MessageBeep(0);
			}

			SetCapture(hwnd);
			Captured=TRUE;
		}

		break;

	case WM_LBUTTONUP:
		if (Captured)
		{ 
			RECT middle;
			Captured=FALSE;
			ReleaseCapture();
			SendMessage(hwnd,LB_GETITEMRECT,HitRow,(long)((LPRECT)&middle));

			if (HitRow != -1 &&
				(HIWORD(lParam)>=middle.top) &&
				(HIWORD(lParam)<=middle.bottom) &&
				(LOWORD(lParam)>=Hit_x) &&
				(LOWORD(lParam)<=(Hit_x+local_W_x)))
			{
				long item_value=SendMessage(hwnd,LB_GETITEMDATA,HitRow,0L);

				if (item_value == CPLIST_START || item_value == CPLIST_END)
				{   
					MessageBeep(0); 
					break; 
				}

				BOOL data_cpl = item_value & BIT_DATACPL ? TRUE : FALSE; 

				if (Hit_x == local_1_x) HitButton =  BIT_L1; else
				if (Hit_x == local_2_x) HitButton =  BIT_L2; else
				if (Hit_x == local_3_x) HitButton =  BIT_L3; else
				if (Hit_x == local_4_x) HitButton =  BIT_N;  else
				if (Hit_x == local_5_x) HitButton =  BIT_GND;  
				if (Hit_x == local_s_x) HitButton =  BIT_CMODEON;  

				if (!surge_app)						 
					item_value^=(HitButton*256);
				else
				{ 
					if (HitButton == BIT_N || HitButton == BIT_L3)
					{
					
						if (!data_cpl && !Three_Phase)     //!3 phase, L3,N 
						{  
							MessageBeep(0); 
							break; 
						}					
					}

					if (HitButton==BIT_CMODEON) 
					{
						item_value ^= HitButton;
					}
					else
					{

						if (item_value & HitButton) 
						{
							item_value|=HitButton*256; 
							item_value &=~HitButton;
						}
						else if (item_value & (HitButton*256))
							item_value &=~(HitButton*256);
						else 
							item_value|=HitButton;

						if (HitButton==BIT_GND) 
							item_value &=~(HitButton*256);
					}
				}
				
				SendMessage(hwnd,LB_SETITEMDATA,HitRow,item_value);
				InvalidateRect(hwnd,(LPRECT)&middle,0);
			}
			else 
				MessageBeep(0);
		}
		break;

	case WM_LBUTTONDBLCLK:
		break;

	default:
		return (oldlistlpfn(hwnd,message,wParam,lParam));
	}

	return 0;
}

 
void COUPLE::Set_Start_Values()
{ 
	if (mode>MODE_FIXED)
	{
		Current_Couple_Pos=1;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_Couple_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_Couple_Pos-1,0L);
	}

	Update_State();
}

BOOL COUPLE::Set_Next_Step()
{
	if (mode>MODE_FIXED)
	{
		long next=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,Current_Couple_Pos+1,0L);
		if (next==CPLIST_END) return TRUE;

		Current_Couple_Pos++;
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETCURSEL,Current_Couple_Pos,0L);
		SendDlgItemMessage(hDlgChild,IDD_LIST,LB_SETTOPINDEX,Current_Couple_Pos-1,0L);
		Update_State();
		return FALSE;
	}
	else 
	{ 
		Update_State();
		return TRUE;
	}
}

LONG COUPLE::Get_Current_Values()
{ 
	if (mode>MODE_FIXED) 
		return (LONG)SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,Current_Couple_Pos,0L);
	else if (selected_coupler & TYPE_CPLIO)
		return Last_Data_Couple;
	else
		return Last_Couple;
}

// JLR, 10/12/95  Show_Valid() added to show or hide "Invalid Mode"	   
//					 previously called for that Update_State() also sent 
//					 command messages to ECAT, that caused error messages back. 	

void COUPLE::Show_Valid()
{
	if (mode != MODE_FIXED) return;

	LONG set_cpl = Get_Current_Values();

	if (!valid_mode(set_cpl))
		SetDlgItemText(hDlgChild,IDD_INVALID,"Invalid Mode");
	else
		SetDlgItemText(hDlgChild,IDD_INVALID,"");
}

void COUPLE::Update_State()
{
	Show_Valid();										// JLR, 10/12/95

	if ((Update_Window)&& (Update_Values))
		Refresh_Values(); 
}

void COUPLE::Refresh_Values()
{
	LONG set_cpl =Get_Current_Values();

	if (surge_app)
	{
		if ((mode != MODE_FRONT) && (valid_mode(set_cpl)))
		{ 
			if (set_cpl & BIT_DATACPL)
			{
				SendMessage(Update_Window,KT_SRG_OUTPUT_SETSTATE,IO_COUPLER,0L);
				int state = set_cpl & BIT_CMODEON ? 1 : 0;
				SendMessage(Update_Window,KT_CMODE_STATE,state,0L);
			}
			else
				SendMessage(Update_Window,KT_SRG_OUTPUT_SETSTATE,AC_COUPLER,0L);

		SendMessage(Update_Window,Update_Message,0,(long)set_cpl);
		}
		else if (mode == MODE_FRONT)
			// Following condition changed so errors don't always set to front panel
			// JFL 9/20/95
			SendMessage(Update_Window,KT_SRG_OUTPUT_SETSTATE,0xFF,0);
	}
	else
	{//EFT
		if (mode!= MODE_FRONT) 
			SendMessage(Update_Window,Update_Message,0,(long)set_cpl);
		else 
			SendMessage(Update_Window,Update_Message,0,-1);
	}//EFT
}

void COUPLE::Show_Run(BOOL state)
{
	LONG csel;
	char buff [20];
	show=state;
	running=state;
	InvalidateRect(GetDlgItem(hDlgChild,IDD_LIST),NULL,0);
	if (state)
	{
		clamp_btn.Enable(FALSE);
		data_btn.Enable(FALSE);

		EnableWindow(GetDlgItem(hDlgChild,IDD_COUPLER	),FALSE);
		EnableWindow(GetDlgItem(hDlgChild,IDD_DELETE  ),FALSE);
		EnableWindow(GetDlgItem(hDlgChild,IDD_INSERT  ),FALSE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_HIDE);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_SHOWNORMAL);
		csel=SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCURSEL,0,0L);
		SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETLBTEXT,(LONG)csel,(LONG)((LPSTR)buff));
		PrintfDlg(hDlgChild,IDD_MODE_LAB,"%s",buff);
	}
	else
	{ 
		clamp_btn.Enable(TRUE);
		data_btn.Enable(TRUE);

		EnableWindow(GetDlgItem(hDlgChild,IDD_COUPLER	),TRUE);
		EnableWindow(GetDlgItem(hDlgChild,IDD_DELETE  ),TRUE);
		EnableWindow(GetDlgItem(hDlgChild,IDD_INSERT  ),TRUE);

		ShowWindow(GetDlgItem(hDlgChild,IDD_CB),SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlgChild,IDD_MODE_LAB),SW_HIDE);
	}
}

void COUPLE::SetFrontName(LPSTR fn)
{ 
	int cs;

	strcpy(front_name,fn);
	SendDlgItemMessage(hDlgChild,IDD_CB,WM_SETREDRAW,FALSE,0);

	cs=SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCURSEL,0,0);
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_INSERTSTRING,0,(long)(LPSTR(front_name)));
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_DELETESTRING,1,0L);
	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,cs,0);
	SendDlgItemMessage(hDlgChild,IDD_CB,WM_SETREDRAW,TRUE,0);
	if (cs==0)
	{
		InvalidateRect(GetDlgItem(hDlgChild,IDD_CB),NULL,FALSE);
		UpdateWindow(GetDlgItem(hDlgChild,IDD_CB));
	}
}

void COUPLE::Clear(void)
{
	// JLR,10/11/95 clear the list box of coupling modes

	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_RESETCONTENT,0,0L);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_START);
	SendDlgItemMessage(hDlgChild,IDD_LIST,LB_ADDSTRING,0,(LONG)CPLIST_END);

	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,0,0L);
	if (SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETCOUNT,0,0L))
		SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_SETCURSEL,0,0L);

	// JLR,10/11/95 fix: selected_coupler stayed E571, even thow copler combo box
	//					    now has	AC coupler selected. As a result AC coupler was
	//						 selected for E508 in some cases.						  
	selected_coupler = SelCouplerType();			// JLR, 10/11/95
	mode=MODE_FRONT;

	SetModeFront();
	ichanged=TRUE;
}

BOOL COUPLE::setvalid()
{
	int n;
	LONG cv;
	switch (mode)
	{
	case MODE_FRONT: 
	case MODE_ALL_AC:
	case MODE_ALL_IO:
	case MODE_ALL:
	case MODE_STAND_AC: 
	case MODE_STAND_IO: 
		return TRUE;
	
	case MODE_FIXED:
		return valid_mode(Get_Current_Values());

	case MODE_LIST:
	case MODE_NEW_LIST:
		n=ReportSteps(); 
		if (n<=0) 
			return FALSE;
		for (n=n; n>0; n--)
		{
			cv=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,n,0L);
			if (!valid_mode(cv)) 
				return FALSE;
		}
		return 1;
	}
	return 0;
}

BOOL COUPLE::Set_Mode(int nmode,BOOL reset)
{ 
	int cnt = SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETCOUNT,0,0L);
    int i;

	for (i = 0; i < cnt; i++)
	{
		if (nmode == SendDlgItemMessage(hDlgChild,IDD_CB,CB_GETITEMDATA,i,0L))
			break;
	}
	
	if (i >= cnt)	
		return FALSE;

	SendDlgItemMessage(hDlgChild,IDD_CB,CB_SETCURSEL,i,0L);
	if (reset)
		SendMessage(hDlgChild,WM_COMMAND,MAKEWPARAM(IDD_CB,CBN_SELCHANGE),(LPARAM)GetDlgItem(hDlgChild,IDD_CB));

	return TRUE;
}

LPSTR Cpl_String(long cv)
{
	static char buffer[50];
	int i;
	buffer[0]=0;
	buffer[1]=0;
	if (LONG(cv) & BIT_DATACPL)
	{
		i=(int)((cv>>8)&255);
		if (i & BIT_S1)  strcat(buffer,",S1+");
		if (i & BIT_S2)  strcat(buffer,",S2+");
		if (i & BIT_S3)  strcat(buffer,",S3+");
		if (i & BIT_S4)  strcat(buffer,",S4+");
		if (i & BIT_G )  strcat(buffer,", G+");
		i=(int)((cv)&255);
		if (i & BIT_S1)  strcat(buffer,",S1-");
		if (i & BIT_S2)  strcat(buffer,",S2-");
		if (i & BIT_S3)  strcat(buffer,",S3-");
		if (i & BIT_S4)  strcat(buffer,",S4-");
		if (i & BIT_G )  strcat(buffer,", G-");
		
		if (cv & BIT_CMODEON) 
			strcat(buffer,",Symm");
		else
			strcat(buffer,",Asymm");
	}
	else
	{
		i=(int)((cv>>8)&255);
		if (i & BIT_L1)  strcat(buffer,",L1+");
		if (i & BIT_L2)  strcat(buffer,",L2+");
		if (i & BIT_L3)  strcat(buffer,",L3+");
		if (i & BIT_N)   strcat(buffer,", N+");
		if (i & BIT_GND) strcat(buffer,",PE+");
		i=(int)((cv)&255);
		if (i & BIT_L1)  strcat(buffer,",L1-");
		if (i & BIT_L2)  strcat(buffer,",L2-");
		if (i & BIT_L3)  strcat(buffer,",L3-");
		if (i & BIT_N)   strcat(buffer,", N-");
		if (i & BIT_GND) strcat(buffer,",PE-");
	}
	buffer[0]=' ';

	return buffer;
}		  

int COUPLE::SaveText(HANDLE hfile)
{ 
	int  i,n;
	long cc;
	char cmode[20];
	char clamp[20];
	char data[20];

	if (mode != MODE_FRONT)
	{
	if (selected_coupler & TYPE_CPLIO)
	fhprintf(hfile,"%Fs:Coupler:IO",(LPSTR)Obj_Name);
	else fhprintf(hfile,"%Fs:Coupler:AC",(LPSTR)Obj_Name);

	if (IO_COUPLER == -1)
	fhprintf(hfile,"\r\n",(LPSTR)Obj_Name);
	else
	{
	clamp_btn.GetText(clamp);
	data_btn.GetText(data);

	fhprintf(hfile," ,AuxClamp:%s, Data:%s\r\n",clamp,data);

	cmode_btn.GetText(cmode);
	}
	}
	switch (mode)
	{
	case MODE_FRONT:
		fhprintf(hfile,"%Fs:Front\r\n",(LPSTR)Obj_Name);
		break;

	case MODE_FIXED:
	{
		LONG cpl = selected_coupler & TYPE_CPLIO ? Last_Data_Couple : Last_Couple;
		fhprintf(hfile,"%Fs:Fixed:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)Cpl_String(cpl));
		break;
	}

	case MODE_LIST:
		fhprintf(hfile,"%Fs:List\r\n",(LPSTR)Obj_Name);
		n=( SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETCOUNT,0,0L)-2);
		for (i=0; i<n; i++)
		{
			cc=SendDlgItemMessage(hDlgChild,IDD_LIST,LB_GETITEMDATA,i+1,0L);
			fhprintf(hfile,"%Fs:Step:%Fs\r\n",(LPSTR)Obj_Name,(LPSTR)Cpl_String(cc));
		}
		break;

	case MODE_ALL_AC:
		fhprintf(hfile,"%Fs:All\r\n",(LPSTR)Obj_Name);
		break;

	case MODE_ALL_IO:
		fhprintf(hfile,"%Fs:All I/O,%s\r\n",(LPSTR)Obj_Name,cmode);
		break;

	//JLR 10/10/95 - fix: ALL AC list loaded from the test file instead of ALL list

	case MODE_ALL:
		fhprintf(hfile,"%Fs:All AC+I/O,%s\r\n",(LPSTR)Obj_Name,cmode);//JLR,10/10/95
		break;

	case MODE_STAND_AC:
		fhprintf(hfile,"%Fs:Standard\r\n",(LPSTR)Obj_Name);
		break;

	case MODE_STAND_IO:
		fhprintf(hfile,"%Fs:Standard I/O,%s\r\n",(LPSTR)Obj_Name,cmode);
	}

	return 1;
}

long Get_Couple(LPSTR cb, BOOL three_phase)
{  
	long tb;
	tb=0;
	if (_fstrstr(cb,"L1+")) tb|= (BIT_L1 <<8);
	if (_fstrstr(cb,"L2+")) tb|= (BIT_L2 <<8);
	if (_fstrstr(cb,"PE+")) tb|= (BIT_GND<<8);

	if (three_phase)
	{
		if (_fstrstr(cb," N+")) tb|=  (BIT_N<<8);
		if (_fstrstr(cb,"L3+")) tb|= (BIT_L3<<8);
	}

	if (_fstrstr(cb,"L1-")) tb|=(BIT_L1);
	if (_fstrstr(cb,"L2-")) tb|=(BIT_L2);
	if (_fstrstr(cb,"PE-")) tb|=(BIT_GND);

	if (three_phase)
	{
		if (_fstrstr(cb,"L3-")) tb|=(BIT_L3);
		if (_fstrstr(cb," N-")) tb|=(BIT_N);
	}

	if (tb) return tb;

	if (_fstrstr(cb,"S1+")) tb|= (BIT_S1<<8);
	if (_fstrstr(cb,"S2+")) tb|= (BIT_S2<<8);
	if (_fstrstr(cb,"G+"))  tb|= (BIT_G <<8);
	if (three_phase)
	{
		if (_fstrstr(cb,"S3+")) tb|= (BIT_S3<<8);
		if (_fstrstr(cb,"S4+")) tb|= (BIT_S4<<8);
	}

	if (_fstrstr(cb," G-")) tb|= BIT_G;
	if (_fstrstr(cb,"S1-")) tb|= BIT_S1;
	if (_fstrstr(cb,"S2-")) tb|= BIT_S2;
	if (three_phase)
	{
		if (_fstrstr(cb,"S3-")) tb|= BIT_S3;
		if (_fstrstr(cb,"S4-")) tb|= BIT_S4;
	}

	if (_fstrstr(cb,"Symm")) tb|= BIT_CMODEON;
	if (tb) tb |=	BIT_DATACPL;
	
	return tb;
}


void COUPLE::SelectCoupler(unsigned long type)
{
	if (!surge_app) return;

	if (type == TYPE_CPLAC && !(allow_couple & TYPE_CPLAC)) return;
	if (type == TYPE_CPLIO && !(allow_couple & TYPE_CPLIO)) return;

	int cnt = SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_GETCOUNT,0,0L);
	if (cnt == 2)
	{
		if (type == TYPE_CPLAC)
		{
			SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_SETCURSEL,0,0L);
			selected_coupler = type;
		}
		else if (type == TYPE_CPLIO)
		{
			SendDlgItemMessage(hDlgChild,IDD_COUPLER,CB_SETCURSEL,1,0L);
			selected_coupler = type;
		}
	}

	CouplerChanged();
	return;
}               

int COUPLE::Process_Line(LPSTR buffer)
{
	if (_fstrstr(buffer,"Coupler")==buffer)
	{
		if (_fstrstr(buffer,"AC"))
		{		
			if (allow_couple & TYPE_CPLAC)
				SelectCoupler(TYPE_CPLAC);
			else 
				Message("Can't Set Couple Mode","Unable to Couple to AC Coupler\n");
		}
		else if (_fstrstr(buffer,"IO"))
		{
			if (allow_couple & TYPE_CPLIO)
				SelectCoupler(TYPE_CPLIO);
			else 
				Message("Can't Set Couple Mode","Unable to Couple to IO Coupler\n");
		}

		if (IO_COUPLER != -1)
		{
			if (_fstrstr(buffer,"220")) clamp_btn = CLAMP_220V; else
			if (_fstrstr(buffer,"20"))  clamp_btn = CLAMP_20V; else
			if (_fstrstr(buffer,"Ext")) clamp_btn = CLAMP_EXT; 

			if (_fstrstr(buffer,"Off")) data_btn = DATA_OFF; else
			if (_fstrstr(buffer,"On"))  data_btn = DATA_ON;
		}

		return 0;
	}

	if (_fstrstr(buffer,"Front")==buffer) 
	{
		Set_Mode(MODE_FRONT);
		return 0;
	}

	if (allow_couple == 0) return 0;

	if (_fstrstr(buffer,"Fixed")==buffer)
	{
		Set_Mode(MODE_FIXED);

		if (selected_coupler & TYPE_CPLIO)
		{
			Last_Data_Couple=Get_Couple(buffer+6,Five_Chan_Io);
			Set_Button_State(hDlgChild,IDD_S1,BIT_S1);
			Set_Button_State(hDlgChild,IDD_S2,BIT_S2);
			Set_Button_State(hDlgChild,IDD_S3,BIT_S3);
			Set_Button_State(hDlgChild,IDD_S4,BIT_S4);
			Set_Button_State(hDlgChild,IDD_G, BIT_G);

			if (Last_Data_Couple & BIT_CMODEON)
				cmode_btn = CMODE_SYMM; 
			else
				cmode_btn = CMODE_ASYMM;
		}
		else
		{
			Last_Couple=Get_Couple(buffer+6,Three_Phase);
			Set_Button_State(hDlgChild,IDD_L1, BIT_L1);
			Set_Button_State(hDlgChild,IDD_L2, BIT_L2);
			Set_Button_State(hDlgChild,IDD_L3, BIT_L3);
			Set_Button_State(hDlgChild,IDD_N,  BIT_N);
			Set_Button_State(hDlgChild,IDD_GND,BIT_GND);
		}

		Show_Valid();											// JLR, 10/12/95
		return 0;
	}

	if (IO_COUPLER != -1 && _fstrstr(buffer,"Symm"))
	{
		Last_Data_Couple |= BIT_CMODEON;
		cmode_btn = CMODE_SYMM;
	}
	else if (IO_COUPLER != -1 && _fstrstr(buffer,"Asymm"))
	{
		Last_Data_Couple &= ~BIT_CMODEON;
		cmode_btn = CMODE_ASYMM;
	}

	if (_fstrstr(buffer,"List")==buffer)
		Set_Mode(MODE_NEW_LIST);
	else if (_fstrstr(buffer,"All I/O")==buffer)
		Set_Mode(MODE_ALL_IO);
	else if (_fstrstr(buffer,"All AC+I/O")==buffer)
		Set_Mode(MODE_ALL);
	else if (_fstrstr(buffer,"All")==buffer)
		Set_Mode(MODE_ALL_AC);
	else if (_fstrstr(buffer,"Standard I/O")==buffer)
		Set_Mode(MODE_STAND_IO);
	else if (_fstrstr(buffer,"Standard")==buffer)
		Set_Mode(MODE_STAND_AC);
	else if (_fstrstr(buffer,"Step")==buffer)
	{
		if (selected_coupler & TYPE_CPLIO)
			Last_Data_Couple=Get_Couple(buffer+5,Five_Chan_Io);
		else	
			Last_Couple=Get_Couple(buffer+5,Three_Phase);

		SendMessage(hDlgChild,WM_COMMAND,IDD_INSERT,0L);
	}
	else
	{
		Message("Unknown String to Parse!","Unknown:\n%Fs",buffer);
		return 1;
	}

	return 0;
}


 
