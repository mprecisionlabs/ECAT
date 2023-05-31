
Building the ECAT firmware application (including the KeyTek library)
---------------------------------------------------------------------

The ECAT firmware building tools are now set up to use the newer MRI compiler/
assembler/linker tools (v4.5R / v7.1J / v7.1J, respectively, from around 1996).
However, the application still builds using the _older_ MRI run time library and
its associated header files (circa 1990-1992).  In order to properly set up the
host system in the neccessary configuration, the ECAT building tools stored in
SourceSafe use one file which is local to the host system, COMPILER.BAT, which
MUST be located in the root directory (C:\COMPILER.BAT).  A functional example
is provided below, but it must be modified to represent the host's directory
structure; it is not stored in SourceSafe because it varies for each host.
COMPILER.BAT is called from the application building utilities BUILDLIB.BAT (for
the KeyTek library) and BUILDSRC.BAT (for the ECAT application itself), both of
which are archived in SourceSafe.  (Note also that the older MRI run time
library and header files are also archived in SourceSafe, in the project
$/ECAT Firmware/Current/MRI.)

Building the CE Series (CE40 and CE50) firmware (including KEYTEK.LIB)
----------------------------------------------------------------------

The CE Series firmware building tools are not yet set up to use the newer MRI
tools like the ECAT (see above).  They will probably have to be before any
further work can proceed on CE Series firmware, since the library (KEYTEK.LIB)
is shared with the ECAT and has therefore already been "upgraded" (in fact, the
CE Series tools actually use the KEYTEK.LIB in the ECAT project directory).  To
"upgrade" the CE Series tools to use the newer MRI compiler/assembler/linker,
do the following:
1. Make sure the host system is already set up to build the ECAT as described
   above (particularly, make sure C:\COMPILER.BAT is set up)
2. Modify the OLD section of COMPILER to include a new environment variable
   called CESERIESPROJ, and set it to the host CE Series project directory
3. Create a new file in the CE Series project directory called BUILDSRC.BAT,
   analogous to the file of the same name for ECAT, and modify as necessary to
   replace the existing files MAKEROM.BAT and MAKEBETA.BAT
4. Add the new BUILDSRC.BAT to SourceSafe, and remove MAKEROM.BAT and
   MAKEBETA.BAT
5. Modify the files MAKEROM., MAKEBETA., and ROM.CMD in a way similar to
   the way in which the ECAT files of the same name were modified (see ECAT
   project SourceSafe files MAKEROM. version 4, MAKEBETA version 5, and ROM.CMD
   version 4)
6. Update this file to indicate changes made to both the CE Series build tools
   and to C:\COMPILER.BAT



/********** START OF C:\COMPILER.BAT **********/
   @echo off

   if %1b == OLDb goto OLD
   if %1b == oldb goto OLD
   if %1b == NEWb goto NEW
   if %1b == newb goto NEW

   echo usage : compiler [OLD | NEW]
   goto END

:ECAT
   REM ECAT uses the older Microtec run time library, but with the newer compiler
   Set XRAYMASTER=
   Set USR_MRI=
   Set PSS_ROOT=
   Set PSS_BSP=

   REM Use OLDER run time library and include files, but NEWER assembler and compiler
   Set mri_68k_bin=C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\mcc68k;C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\asm68k
   Set mri_68k_inc=C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\mcc68k\inc
   Set mri_68k_lib=C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\mcc68k\lib
   Set mri_68k_tmp=%TEMP%

   Set USR_MRI=

   Set ECATPROJ=C:\"Documents and Settings"\"frank.dicesare"\Documents\ecat\firmware
   Set MRI_RUNTIME_LIB=C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\mcc68k\lib
   Set MAKEUTIL=%ECATPROJ%\source\maker.exe
   Set TOUCHUTIL=%ECATPROJ%\source\touch.com

   echo System is now configured for the OLD Microtec libraries,
   echo to build ECAT and CE Series

   goto END

:EMCPRO_CEMASTER
   Set CMPLPROJ=C:\"Documents and Settings"\"frank.dicesare"\Documents\CMPLMSTR\Firmware
   echo Configuring to build EMCPro and CEMaster...
   goto EMCPRO_CEMASTER_EMCPRO1089

:EMCPRO1089
   Set CMPLPROJ=C:\"Documents and Settings"\"frank.dicesare"\Documents\EMCPro1089\Firmware
   echo Configuring to build EMCPro1089...
   goto EMCPRO_CEMASTER_EMCPRO1089

:EMCPRO_CEMASTER_EMCPRO1089
   REM EMCPRO_CEMASTER_EMCPRO1089 uses the newer Microtec run time library,
   REM along with the newer compiler (USR_MRI is used in place of all of the
   REM other MRI environment variables)

   set mri_68k_bin=
   set mri_68k_inc=
   set mri_68k_lib=
   set mri_68k_tmp=%TEMP%

   Set USR_MRI=C:\"Documents and Settings"\"frank.dicesare"\Documents\MRI\mcc68k
   Set PSS_ROOT=c:\pss68k
   Set PSS_BSP=%PSS_ROOT%\bsps\cmplmstr

   Set PSOSPROJ=c:\pss68k
   Set MRI_RUNTIME_LIB=
   Set MAKEUTIL=%CMPLPROJ%\make\nmake.exe
   Set TOUCHUTIL=
   echo System is now configured for the NEW Microtec libraries and for PSOS

   goto END

:END
/********** END OF C:\COMPILER.BAT **********/
