   @echo off
   rem Configures environment variables to build ECAT and ECAT LTS systems

   echo.
   echo Configuring to build ECAT/ECAT LTS...
   echo.
   rem This assumes that %cd% is a drive root, not a sub-directory
   Set ECATPROJ=%cd%
   Set ECATSOURCE=%ECATPROJ%source
   Set ECATTOOLS=%ECATPROJ%tools
   Set ECAT_SREC2BIN=%ECATPROJ%S-record_conversion\srec2bin
   Set ECATLIB=%ECATPROJ%library
   Set ECATLIBSOURCE=%ECATLIB%\source
   Set ECATLIBTOOLS=%ECATLIB%\tools
   Set LIBMAKEFILE=%ECATLIBTOOLS%\makefile
   Set ECATMRI=%ECATPROJ%MRI
   Set ECATMRILIBS=%ECATMRI%\OlderLibraries
   Set ECATMRIBIN=%ECATPROJ%MRI\mcc68k\mcc68k
   Set ECATMRILNK=%ECATPROJ%MRI\asm68k\lnk68k
   Set ECATMRILIB=%ECATPROJ%MRI\asm68k\lib68k
   Set MRI_RUNTIME_LIB=%ECATMRILIBS%\lib\68000\mcc68kab.lib
   Set MAKEUTIL=%ECATPROJ%tools\make.exe
   Set MAKEFILE=%ECATPROJ%tools\makefile
   REM Reset environment variables used by EMCPro/CEMaster
   Set XRAYMASTER=
   Set USR_MRI=
   Set PSS_ROOT=
   Set PSS_BSP=
   REM ECAT uses OLDER run time library and include files, but NEWER assembler and compiler
   Set mri_68k_bin=%ECATPROJ%MRI\mcc68k;%ECATPROJ%MRI\asm68k
   Set mri_68k_inc=%ECATMRILIBS%\inc
   Set mri_68k_lib=%ECATMRILIBS%\lib
   Set mri_68k_tmp=%TEMP%
   echo System is now configured for the OLDER Microtec libraries,
   echo 	to build ECAT, ECAT LTS, and CE Series.
   echo.

