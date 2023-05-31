   @ECHO off

   rem Usage:  BUILD [FULL .or. RELEASE .or. LOAD]
   rem      where FULL indicates a full non-release build (displays "beta" development version letter);
   rem      RELEASE indicates a full release build (no "beta" version, e.g. v5.11, not v5.11r);
   rem      and LOAD loads an existing executable image into the ROM emulator only, and does not perform a build.
   rem      We no longer do partial builds (full build is sufficiently fast)

   echo.

   rem Set the root working directory (this directory) to the root of drive P:
   rem This works around file path length issues (mcc68k.exe doesn't work
   rem   if its full path length is greater than 59 characters)
   set MRI_DRIVE=P:
   echo Working directory %cd% is being aliased to drive %MRI_DRIVE%
   subst %MRI_DRIVE% %cd%
   pushd %MRI_DRIVE%

   set BUILDTYPE=NONE
   if %1b == FULLb set BUILDTYPE=FULL
   if %1b == fullb set BUILDTYPE=FULL
   if %1b == RELEASEb set BUILDTYPE=RELEASE
   if %1b == releaseb set BUILDTYPE=RELEASE
   if %1b == LOADb set BUILDTYPE=LOAD_ONLY
   if %1b == loadb set BUILDTYPE=LOAD_ONLY
   if %BUILDTYPE%b==NONEb goto SYNTAX

   rem Set up the environment for building ECAT/LTS code using the older MRI run
   rem   time library with the newer MRI compiler/assembler/linker tools.
   call .\compiler.bat

   if %BUILDTYPE%b==FULLb goto FULL_BUILD
   if %BUILDTYPE%b==RELEASEb goto RELEASE_BUILD
   if %BUILDTYPE%b==LOAD_ONLYb goto LOAD_ROM_EMULATOR
   goto SYNTAX

:FULL_BUILD
   rem Define BETA_RELEASE for the makefile
   set BETA_RELEASE=-DBETA_RELEASE
   goto BUILD

:RELEASE_BUILD
   rem Undefine BETA_RELEASE for the makefile
   set BETA_RELEASE=-UBETA_RELEASE
   goto BUILD

:BUILD
   pushd %ECATSOURCE%
   copy %ECATTOOLS%\rom.cmd %ECATSOURCE%
   rem Delete all object files (always a full build)
   if exist *.obj del *.obj
   set BUILDERR=
   call %MAKEUTIL% -f%MAKEFILE% all
   if %ERRORLEVEL% NEQ 0 set BUILDERR=YES
   del %ECATSOURCE%\rom.cmd
   popd

   rem Don't do anything further if Make returned an error
   if %BUILDERR%b==YESb goto ERROR

   rem Copy binary image to network directory for ROM emulator access
   call %ECATTOOLS%\copy_bins.bat

   rem Skip ROM emulator load
   goto END

:LOAD_ROM_EMULATOR
   rem Load binary image directly into locally-connected EmuTec PROMJet ROM/Flash emulator
   echo Loading ROM Emulator...
   pushd %ECATTOOLS%
   call copy_to_promjet.pjs
   popd
   goto END

:ERROR
   echo +++++++++++++++++++++++++++++++++++++++++++++
   echo ++++++++++++ !!! BUILD ERROR !!! ++++++++++++
   echo +++++++++++++++++++++++++++++++++++++++++++++
   goto END

:SYNTAX
    echo Usage : build-main.bat mode
    echo              where mode = [ FULL .or. RELEASE ]
    echo                    FULL is development build (development version letter is displayed)
    echo                    RELEASE is for official release
    echo.
    goto END

:END
   rem Return to original working directory and remove substituted drive M:
   popd
   subst %MRI_DRIVE% /d
   echo.

