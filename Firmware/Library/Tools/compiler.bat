   @echo off
   echo.

   if %1b == ECATb goto ECAT
   if %1b == EMCPRO_CEMASTERb goto EMCPRO_CEMASTER
   if %1b == EMCPRO1089b goto EMCPRO1089
   if %1b == ECAT-LTSb goto ECAT-LTS
   echo Invalid parameter.
   echo.
   echo usage : compiler [ECAT ^| ECAT-LTS ^| EMCPRO_CEMASTER ^| EMCPRO1089]
   echo.
   goto END

:ECAT
   Set ECATPROJ=C:\ecat
   echo Configuring to build ECAT...
   echo.
   echo Project directory has been set to %ECATPROJ%...
   echo.
   goto ECAT_ECAT-LTS

:ECAT-LTS
   Set ECATPROJ=C:\Ecat-LTS\Firmware
   echo Configuring to build ECAT LTS...
   echo.
   echo Project directory has been set to %ECATPROJ%...
   echo.
   goto ECAT_ECAT-LTS

:ECAT_ECAT-LTS
   Set ECATSOURCE=%ECATPROJ%\source
   Set ECATTOOLS=%ECATPROJ%\tools
   Set ECAT_SREC2BIN=%ECATPROJ%\S-record_conversion\srec2bin

   Set ECATLIB=%ECATPROJ%\library
   Set ECATLIBSOURCE=%ECATLIB%\source
   Set ECATLIBTOOLS=%ECATLIB%\tools
   
   Set ECATMRI=%ECATPROJ%\MRI
   Set MRI_RUNTIME_LIB=%ECATMRI%\lib\68000\mcc68kab.lib
   
   Set MAKEUTIL=%ECATPROJ%\tools\make.exe
   Set TOUCHUTIL=%ECATPROJ%\tools\touch.com

   REM ECAT uses the older Microtec run time library, but with the newer compiler
   Set XRAYMASTER=
   Set USR_MRI=
   Set PSS_ROOT=
   Set PSS_BSP=

   REM Use OLDER run time library and include files, but NEWER assembler and compiler
   Set mri_68k_bin=C:\MRI\mcc68k;C:\MRI\asm68k
   Set mri_68k_inc=%ECATMRI%\inc
   Set mri_68k_lib=%ECATMRI%\lib
   Set mri_68k_tmp=%TEMP%

   echo System is now configured for the OLD Microtec libraries,
   echo 	to build ECAT, ECAT LTS, and CE Series.
   echo.
   goto END

:EMCPRO_CEMASTER
   Set CMPLPROJ=C:\CMPLMSTR\Firmware
   echo Configuring to build EMCPro and CEMaster...
   echo.
   echo Project directory has been set to %CMPLPROJ%...
   echo.
   goto EMCPRO_CEMASTER_EMCPRO1089

:EMCPRO1089
   Set CMPLPROJ=C:\EMCPro1089\Firmware
   echo Configuring to build EMCPro1089...
   echo.
   echo Project directory has been set to %CMPLPROJ%...
   echo.
   goto EMCPRO_CEMASTER_EMCPRO1089

:EMCPRO_CEMASTER_EMCPRO1089
   REM EMCPRO_CEMASTER_EMCPRO1089 uses the newer Microtec run time library,
   REM along with the newer compiler (USR_MRI is used in place of all of the
   REM other MRI environment variables)

   set mri_68k_bin=
   set mri_68k_inc=
   set mri_68k_lib=
   set mri_68k_tmp=%TEMP%

   Set USR_MRI=C:\MRI\mcc68k
   Set PSS_ROOT=c:\pss68k
   Set PSS_BSP=%PSS_ROOT%\bsps\cmplmstr

   Set PSOSPROJ=c:\pss68k
   Set MRI_RUNTIME_LIB=
   Set MAKEUTIL=%CMPLPROJ%\make\nmake.exe
   Set TOUCHUTIL=
   echo System is now configured for the NEW Microtec libraries and for PSOS.
   echo.
   goto END

:END

