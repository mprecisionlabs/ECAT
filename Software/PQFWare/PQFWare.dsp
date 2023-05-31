# Microsoft Developer Studio Project File - Name="PQFWare" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PQFWare - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PQFWare.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PQFWare.mak" CFG="PQFWare - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PQFWare - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PQFWare - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ECAT32/PQFWare", ERSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PQFWare - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Software Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "PQFWare - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PQFWare___Win32_Debug"
# PROP BASE Intermediate_Dir "PQFWare___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Software Debug/PQFWare.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PQFWare - Win32 Release"
# Name "PQFWare - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Core\about.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\cal.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\comi.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\comm.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\dlgctl.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\dlgutil.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\EcatApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\EcatMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\EcatMainWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\filesdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\ilock.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\kt_hide.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\kt_util.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\kt_util2.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\log.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\message.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\MultiStateBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\PqfApp.cpp
# End Source File
# Begin Source File

SOURCE=.\PQFGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\PqfMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PqfMainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PQFWare.cpp
# End Source File
# Begin Source File

SOURCE=.\PQFWare.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Core\tname.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Core\801bits.h
# End Source File
# Begin Source File

SOURCE=..\Core\about.h
# End Source File
# Begin Source File

SOURCE=..\Core\calinfo.h
# End Source File
# Begin Source File

SOURCE=..\Core\comm.h
# End Source File
# Begin Source File

SOURCE=..\Core\commi.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=..\Core\dlgctl.h
# End Source File
# Begin Source File

SOURCE=..\Core\dlgutil.h
# End Source File
# Begin Source File

SOURCE=..\Core\EcatApp.h
# End Source File
# Begin Source File

SOURCE=..\Core\EcatMainDlg.h
# End Source File
# Begin Source File

SOURCE=..\Core\EcatMainWnd.h
# End Source File
# Begin Source File

SOURCE=..\Core\filesdlg.h
# End Source File
# Begin Source File

SOURCE=..\Core\globals.h
# End Source File
# Begin Source File

SOURCE=..\Core\ilock.h
# End Source File
# Begin Source File

SOURCE=..\Core\kt_comm.h
# End Source File
# Begin Source File

SOURCE=..\Core\kt_hide.h
# End Source File
# Begin Source File

SOURCE=..\Core\kt_util.h
# End Source File
# Begin Source File

SOURCE=..\Core\kt_util2.h
# End Source File
# Begin Source File

SOURCE=..\Core\log.h
# End Source File
# Begin Source File

SOURCE=..\Core\message.h
# End Source File
# Begin Source File

SOURCE=..\Core\module.h
# End Source File
# Begin Source File

SOURCE=..\Core\MultiStateBtn.h
# End Source File
# Begin Source File

SOURCE=.\objects.h
# End Source File
# Begin Source File

SOURCE=.\pqf.h
# End Source File
# Begin Source File

SOURCE=.\PqfApp.h
# End Source File
# Begin Source File

SOURCE=.\PqfMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\PqfMainWnd.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\revstr.h
# End Source File
# Begin Source File

SOURCE=..\Core\rgbdefs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\test.h
# End Source File
# Begin Source File

SOURCE=.\testmgr.h
# End Source File
# Begin Source File

SOURCE=..\Core\tname.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\arr_down.ico
# End Source File
# Begin Source File

SOURCE=..\res\arr_up.ico
# End Source File
# Begin Source File

SOURCE=..\res\bdicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bdurcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bexpicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\Bhandico.ico
# End Source File
# Begin Source File

SOURCE=..\res\blicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bmvcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bmvicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bpcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bphcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bphicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bpicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bpolcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bpvcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bpvicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\brcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\brepeat.cur
# End Source File
# Begin Source File

SOURCE=..\res\brepicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bricon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bswcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bswicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bvcur.cur
# End Source File
# Begin Source File

SOURCE=..\res\bvicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\bvpicon.ico
# End Source File
# Begin Source File

SOURCE=..\res\com_lnk2.ico
# End Source File
# Begin Source File

SOURCE=..\res\cross.cur
# End Source File
# Begin Source File

SOURCE=..\res\dd_gnd_s.bmp
# End Source File
# Begin Source File

SOURCE=..\res\E400_app.ico
# End Source File
# Begin Source File

SOURCE=..\res\EFTgnd1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl1_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl1_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl1_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl2_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl2_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl2_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl3_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl3_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTl3_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTn1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTn3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EFTn6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EUTOFF1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\ex1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\frm_aux.bmp
# End Source File
# Begin Source File

SOURCE=..\res\frm_tot.bmp
# End Source File
# Begin Source File

SOURCE=..\res\idd_gnd_.bmp
# End Source File
# Begin Source File

SOURCE=..\res\idd_raw_.bmp
# End Source File
# Begin Source File

SOURCE=..\res\idd_run_.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PQF_app.ico
# End Source File
# Begin Source File

SOURCE=..\res\pwr_on.bmp
# End Source File
# Begin Source File

SOURCE=..\res\pwr_rdy.bmp
# End Source File
# Begin Source File

SOURCE=..\res\run1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\STOP1.bmp
# End Source File
# End Group
# End Target
# End Project
