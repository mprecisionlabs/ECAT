# Microsoft Developer Studio Project File - Name="e400" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=e400 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "e400.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "e400.mak" CFG="e400 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "e400 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "e400 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ECAT32/E400", CRSAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "e400 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"..\Software Release/E400.exe"

!ELSEIF  "$(CFG)" == "e400 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Software Debug/E400.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "e400 - Win32 Release"
# Name "e400 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Core\about.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\acmeas.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\block.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\btn.cpp
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

SOURCE=..\Core\cpl.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\dlgctl.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\dlgutil.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\duals.cpp
# End Source File
# Begin Source File

SOURCE=.\e400.rc
# End Source File
# Begin Source File

SOURCE=.\E400App.cpp
# End Source File
# Begin Source File

SOURCE=.\E400Main.cpp
# End Source File
# Begin Source File

SOURCE=.\E400MainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\E400MainWnd.cpp
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

SOURCE=..\Core\log.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\meas.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\message.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\msec.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\MultiStateBtn.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\pause.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\phase.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\polarity.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\pps.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\repeat.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\scroll.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\Core\tname.cpp
# End Source File
# Begin Source File

SOURCE=..\Core\wave.cpp
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

SOURCE=..\Core\acmeas.h
# End Source File
# Begin Source File

SOURCE=..\Core\block.h
# End Source File
# Begin Source File

SOURCE=..\Core\btn.h
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

SOURCE=..\Core\cpls.h
# End Source File
# Begin Source File

SOURCE=..\Core\dlgctl.h
# End Source File
# Begin Source File

SOURCE=..\Core\dlgutil.h
# End Source File
# Begin Source File

SOURCE=..\Core\duals.h
# End Source File
# Begin Source File

SOURCE=.\e400.h
# End Source File
# Begin Source File

SOURCE=.\E400App.h
# End Source File
# Begin Source File

SOURCE=.\E400MainDlg.h
# End Source File
# Begin Source File

SOURCE=.\E400MainWnd.h
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

SOURCE=..\Core\file_ver.h
# End Source File
# Begin Source File

SOURCE=..\Core\filedlg.h
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

SOURCE=..\Core\log.h
# End Source File
# Begin Source File

SOURCE=..\Core\meas.h
# End Source File
# Begin Source File

SOURCE=..\Core\message.h
# End Source File
# Begin Source File

SOURCE=..\Core\modes.h
# End Source File
# Begin Source File

SOURCE=..\Core\module.h
# End Source File
# Begin Source File

SOURCE=..\Core\msec.h
# End Source File
# Begin Source File

SOURCE=..\Core\MultiStateBtn.h
# End Source File
# Begin Source File

SOURCE=..\Core\pause.h
# End Source File
# Begin Source File

SOURCE=..\Core\phase.h
# End Source File
# Begin Source File

SOURCE=..\Core\polarity.h
# End Source File
# Begin Source File

SOURCE=..\Core\pps.h
# End Source File
# Begin Source File

SOURCE=..\Core\repeat.h
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

SOURCE=..\Core\scroll.h
# End Source File
# Begin Source File

SOURCE=..\Core\sim2.h
# End Source File
# Begin Source File

SOURCE=..\Core\simwaves.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\Core\tname.h
# End Source File
# Begin Source File

SOURCE=..\Core\usefdlg.h
# End Source File
# Begin Source File

SOURCE=..\Core\waves.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
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

SOURCE=..\res\bhandico.ico
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

SOURCE=..\res\dd_gnd_s.bmp
# End Source File
# Begin Source File

SOURCE=..\res\E400_app.ico
# End Source File
# Begin Source File

SOURCE=..\res\eftgnd1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl1_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl1_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl1_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl2_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl2_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl2_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl3_1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl3_3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftl3_6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftn1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftn3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\eftn6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\empty.ico
# End Source File
# Begin Source File

SOURCE=..\res\eutoff1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\ex1.bmp
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

SOURCE=..\res\pwr_on.bmp
# End Source File
# Begin Source File

SOURCE=..\res\pwr_rdy.bmp
# End Source File
# Begin Source File

SOURCE=..\res\run1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\stop1.bmp
# End Source File
# End Group
# End Target
# End Project
