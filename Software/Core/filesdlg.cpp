//
// common dialog boxes for fileopen and filesave.
//

#include "stdafx.h"
#include <winbase.h>
#include "filesdlg.h"
#include "Globals.h"

#define MAXFILENAME 		(256) 	     /* maximum length of file pathname      */
#define MAXCUSTFILTER		(40)	     /* maximum size of custom dlg filter buffer */

int CommonDlgOperation( HWND hMainDlg, HANDLE ghinst, LPSTR fnamebuf, int mode )
{
static OPENFILENAME OFNameStruct;
int retval;		// function return value.
static char szDirName[MAX_PATH + 1];	// default directory name that comm dlg will look in.

static char	szFileName[MAXFILENAME];
static char	szFileTitle[MAXFILENAME];
char	szKeytekDir[] = ".";

	if( fnamebuf[0] == '\0') {	// we've got no name.
		// intialize all structure members to their appropriate values.
		memset( &OFNameStruct, 0, sizeof(OPENFILENAME) );
		szFileName[0] = '\0';
		szFileTitle[0] = '\0';
		szDirName[0] = '\0';
		GetCurrentDirectory(MAX_PATH, szDirName);
	}
		// fill in non-variant fields of OPENFILENAME struct.
	OFNameStruct.lStructSize	= sizeof(OPENFILENAME);
	OFNameStruct.hwndOwner		= hMainDlg;
	OFNameStruct.lpstrFilter	= DEF_FILES;		// default filename extension filter spec.
	OFNameStruct.lpstrCustomFilter	= NULL;
	OFNameStruct.nMaxCustFilter	= 0;
	OFNameStruct.nFilterIndex	= 1;
	OFNameStruct.lpstrFile		= szFileName;
	OFNameStruct.nMaxFile		= MAXFILENAME;
	OFNameStruct.lpstrInitialDir	= NULL;
	OFNameStruct.lpstrFileTitle	= szFileTitle;
	OFNameStruct.nMaxFileTitle	= MAXFILENAME;
	OFNameStruct.lpstrTitle		= NULL;
	OFNameStruct.lpstrDefExt	=DEF_EXT;
	OFNameStruct.lpstrInitialDir	= szDirName;
	OFNameStruct.hInstance	   	= (HINSTANCE)ghinst; 		//global

#ifdef need_customization   // if we need to customize this dialog at a later date:
	// here are the KeyTek customizations.
	OFNameStruct.lpfnHook	   	= FileOpenHookProc;
	OFNameStruct.Flags	   	= OFN_ENABLETEMPLATE | OFN_ENABLEHOOK;
	OFNameStruct.lpTemplateName 	= "MYTEMPLATE";
#endif  // end need_customization.

	switch ( mode ){
		case OPEN:
		OFNameStruct.Flags	= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

		if ( (retval = GetOpenFileName( &OFNameStruct ))!=0 )
			lstrcpy ( fnamebuf, OFNameStruct.lpstrFile);
		break;

		case SAVEAS:
		case SAVE:
		OFNameStruct.Flags	= OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

		if ((retval = GetSaveFileName( &OFNameStruct ))!=0)
			lstrcpy ( fnamebuf, OFNameStruct.lpstrFile);
		break;

		default:
		retval = 0;
		}
	return(retval);		// returns zero if "Canceled" or error occurs.
}
