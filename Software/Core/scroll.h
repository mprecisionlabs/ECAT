
#ifndef SCROLL_CLASS
#define SCROLL_CLASS
#include "dlgctl.h"
#include  "FILE_VER.H"
//************************************************************************ *//
//                   * The Scroll_Pair Class *                              //
//                   *************************                              //
//                                                                          //
//        This Obect  implements a paired Scroll Bar and Edit control       //
//        For use in Dialog boxes. THIS object will not operate with        //
//        stand alone controls.                                             //
//                                                                          //
//The following public methods and variables are avaiible for use:          //
//                                                                          //
//__________________________________________________________________________//
// Scroll_Pair();:The constructor, it takes no arguments                    //
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
// The following method initializes the class                               //
// void init(HWND hDlg, //The dialog handle                                 //
//           WORD id_sb,//The ID of the Scroll Bar                          //
//           WORD id_eb,//The ID of the Edit Box                            //
//           WORD start,//The start of the Scroll Pair  range.              //
//           WORD end,  //The end of the Scroll Pair  range.                //
//           WORD initv,//The initial value for the Scroll Pair.            //
//            int exp,  //Base 10 exponent used to map Scroll Bar units     //
//                      //to Values 0 =1 to 1, -1 Means 1 SB=10 EB units    //
//            int inc,  //How many Scroll Bar units to scroll for each press//
//           long color //If the Scroll bar class is SSCROLLBAR the it sets //
//                      //The color of the hash mark on the scroll bar      //
//                      //If the Class is not SSCROLLBAR then it is ignored.//
//                      //If the edit box iclass is SEDIT it sets the       //
//                      //outline color otherwise it is ignored             //   
//          );                                                              //
//                                                                          //
//                                                                          //
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
//The following procedure is used to process all of the Messages sent to the//
//parent Dialog proc from the controlls used with the Scroll Pair           //
//                                                                          //
//BOOL Scroll_Proc(WORD message,WORD wParam,LONG lParam);                   //
//                                                                          //
// The user of this class must include the following statments in their     //
// dialog proc to insure that the class recieves the desired messages       //
// The following example assumes that the programmer has two Scroll_Pair    //
// objects sp1 and sp2 and their Scroll Bar ID's are: SP1_SB_ID, SP2_SB_ID  //
// and the Edit box ID's are SBn_EB_ID etc...                               //
//                                                                          //
//	  case WM_HSCROLL:                                                       //
//    switch (GetWindowWord (HIWORD (lParam), GWW_ID))                      //
//    {                                                                     //
//		 case SP1_SB_ID: return(sp1.Scroll_Proc(message,wParam,lParam));      //
//		 case SP2_SB_ID: return(sp2.Scroll_Proc(message,wParam,lParam));      //
//     ....                                                                 //
//    }                                                                     //
//  case WM_COMMAND:                                                        //
//     switch (wParam)                                                      //
//    {                                                                     //
//		 case SP1_SB_ID: return(sp1.Scroll_Proc(message,wParam,lParam));      //
//		 case SP2_SB_ID: return(sp2.Scroll_Proc(message,wParam,lParam));      //
//		}                                                                     //
//                                                                          //
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
//The following method causes the scroll pair to be hidden and disabled     //
//void Hide(BOOL //True to Hide and False to show                           //
//         );                                                               //
//                                                                          //
//__________________________________________________________________________//
//This method returns the current value of the varible the dual scroll bar  //
//is controling / or maintaining                                            //
//LONG Value(void);                                                         //
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
//If the parent wants to be notified of each scroll bar change the the      //
//Parent would call Notify and pass the desired message value to the  Object//
//After that time the Scroll_Pair will send it's parrent a message every time//
//it is changed                                                             //
//void Notify(WORD msg);                                                    //
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
//The following function returns True when the value of the  Scroll Pair has//
// changed. It is a destructive READ. When you check to see if it is changed/
// the changed flag is cleared.
//BOOL changed();                                                  
//                                                                          //
//                                                                          //
//__________________________________________________________________________//
// The Object Destructor                                                    //
//~Scroll_Pair();                                                           //
//                                                                          //
//**************************************************************************//

class far Scroll_Pair
{
private:        //Private Variables are not availible to non class members.
	LONG div;
	LONG mul;
	LONG id_eb;    //The Edit Box ID
	LONG id_sb;    //The Scroll Bar ID
	LONG Notify_Message; //The message used to notify the parrent of changes
	LONG start;    //The legal range start value
	LONG end;      //The legal range end value
	LONG variable; //The current value
	BOOL Suspend;  //Used internally to determine the cause for an EB_ENCHANGED message
	BOOL On;       //The on /off flag
	HWND hDlg;     //The handle to the parent
	HPEN pen;      //The pen used with SSCROLLBAR and EDIT Classes
	char units[4];
	int  inc;      //The scroll increment
	BOOL ichanged; //The internal changed flag
public:

	void init(HWND ,LONG, LONG, LONG, LONG, LONG, int ,int, long, LPSTR);
	Scroll_Pair(){pen=NULL;};
	BOOL Scroll_Proc(UINT,WPARAM,LPARAM);
	void Hide(BOOL);
	void Set_Value(LONG val);
	void Set_Max_Value(LONG val);
	void Set_Min_Value(LONG val);
	void Set_Units(LPSTR);
	LONG Value(void){return variable;};
	void Notify(LONG msg){Notify_Message=msg;};
	~Scroll_Pair();
	BOOL changed(){if (ichanged) {ichanged=FALSE; return TRUE;} else return FALSE;};
};
#endif

