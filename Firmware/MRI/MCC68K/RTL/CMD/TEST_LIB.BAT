@echo off

:########################################################################
:# THIS INFORMATION IS PROPRIETARY TO					#
:# MICROTEC RESEARCH, INC.						#
:#----------------------------------------------------------------------#
:# Copyright (c) 1993, 1995 Microtec Research, Inc.			#
:# All rights reserved							#
:########################################################################
:The software source code contained in this file and in the related
:header files is the property of Microtec Research, Inc. and may only be
:used under the terms and conditions defined in the Microtec Research
:license agreement. You may modify this code as needed but you must retain
:the Microtec Research, Inc. copyright notices, including this statement.
:Some restrictions may apply when using this software with non-Microtec
:Research software.  In all cases, Microtec Research, Inc. reserves all rights.

:#      %W% %G%

if not "%1" == "" goto no_instructions
:show_instructions

echo test_lib {library_name} [ -cmdfile {file name} ]
echo          [ -output {file name} ] [ -testsrcfile {file name ...} ]
echo.
echo The batch file "test_lib" compiles the library test suite in ..\TEST and
echo generates .ABS files for each test. It also generates an XRAY Debugger
echo include file "XRAY.INC" which executes the .ABS files and generates a .OUT
echo file for each test. The batch file "summary" can then be used to generate
echo an easily readable test summary.
echo.
echo {library_name}: Name of the library which is to be tested.  This is also
echo                 the name of the configuration file used for the library
echo                 build process.
echo.
echo -cmdfile:       The name of the linker command file.  The default is 
echo                 MCC68K.CMD.  It must be located in ..\LIB.
echo.
echo -output:        The argument to this option is the name of a file
echo                 which will be used to record the results of the test.
echo.
echo -testsrcfile:   The arguments to this option are files which will be
echo                 compiled along with the test file.  These files must be
echo                 located in ..\SRC.  More than one file may be specified.

goto end_of_file

:no_instructions

:#
:#########################################################################
:#	This script builds a command file for the compiler, then using
:#	it, compiles all appropriate source to object.  It also builds
:#	a linker command file which is then used to build the final
:#	".abs" file.
:#########################################################################
:#

:#
:#########################################################################
:#
:#	 Store arguments
:#       Set up default symbols
:#
:# conf_dir:	Directory which contain the configuration files
:# src_dir:	Directory which contains library source files
:# lib_dir:	Directory which will contain completed library file
:# test_dir:	Directory which contains test routines
:#
:#########################################################################

    set  test_dir=..\TEST
    set  conf_dir=..\CONFIG
    set  src_dir=..\SRC
    set  lib_dir=..\LIB
    set  cmd_dir=..\CMD
    set  cmd_file=MCC68K.CMD
    set  out_file=
    set  compile_error=
    set	 xray_cmd=XRAY.INC
 
    set  test_flag=
    set  out_flag=
    set  cmd_flag=


    if exist %lib_dir%\temp_s.cmd	del %lib_dir%\temp_s.cmd
    if exist %lib_dir%\temp.cmd		del %lib_dir%\temp.cmd

:#
:#########################################################################
:#	Strip file extensions off configuration file and test file
:#########################################################################
:#

del_ext	config_file	%1					> setval.bat
call	setval.bat

:#
:#########################################################################
:#	Test to make sure configuration file exists
:#########################################################################
:#

if exist %conf_dir%\%config_file%.h goto config_exists
    echo Configuration file does not exist: %conf_dir%\%config_file%.H
    goto end_of_file
:config_exists

:#
:#########################################################################
:#	Begin building compiler command file
:#########################################################################
:#

:test_exists
    echo -DCONFIGURATION_FILE=%conf_dir%\%config_file%.H	>  commands.lis
    echo -DDEBUG_ON						>> commands.lis

:# 
:#########################################################################
:#	process any options
:#########################################################################
:#

:test_next_option

    if "%2" == "" goto no_more_options

    :check_file
	if not "%2" == "-testsrcfile" goto check_output
            set test_flag=1
	    set out_flag=
	    set cmd_flag=
	    shift
	    goto test_next_option
	
    :check_output
	if not "%2" == "-output" goto check_command
	    set out_flag=1
	    set test_flag=
	    set cmd_flag=
	    shift
	    goto test_next_option

    :check_command
	if not "%2" == "-cmdfile" goto save_files
	    set out_flag=
	    set test_flag=
	    set cmd_flag=1
	    shift
	    goto test_next_option

    :save_files
	if not "%test_flag%" == "1" goto save_output
	    if exist %src_dir%\%2 goto tst_s_exists
		echo Library source file does not exist:  %src_dir%\%2
		goto end_of_file
	    :tst_s_exists
	        echo %src_dir%\%2				>> commands.lis
		del_ext	lib_src	%2				> setval.bat
		call	setval.bat
		echo load %lib_src%.OBJ 		>> %lib_dir%\temp_s.cmd
	        shift
	        goto test_next_option

    :save_output
        if not "%out_flag%" == "1" goto save_cmd
	    set out_flag=
	    set out_file=%2
	    shift
	    goto test_next_option

    :save_cmd
	if not "%cmd_flag%" == "1" goto bad_option
	    set cmd_flag=
	    set cmd_file=%2
	    shift
	    goto test_next_option

    :bad_option
	echo Unrecognized option: %2
	goto end_of_file
 
:no_more_options

:#
:#########################################################################
:#	Check to insure linker command file exists
:#	Save linker load command for test routine
:#########################################################################
:#

if "%cmd_file%" == "" goto no_linker_cmd
    if exist %lib_dir%\%cmd_file% goto cmd_file_exists
	echo Linker command file does not exist:  %lib_dir%\%cmd_file%
	goto end_of_file

    :cmd_file_exists
	echo -e%lib_dir%\%cmd_file%				>> commands.lis
	goto command_ok

    :no_linker_cmd
	echo Linker command file has not been specified
	goto end_of_file

:command_ok

:#
:#########################################################################
:#	Check to insure library exists
:#########################################################################
:#

if exist %lib_dir%\%config_file%.lib goto library_exists
    echo Library file does not exist:  %lib_dir%\%config_file%.LIB
    goto end_of_file

:library_exists

    echo load %lib_dir%\%config_file%.LIB		>> %lib_dir%\temp_s.cmd

:#
:#########################################################################
:#      Test to insure there are test routines where we expect them
:#########################################################################
:#

if exist %test_dir%\*.c goto tests_exist
    echo Cannot find test routines in: %test_dir%\*.C
    goto end_of_file

:tests_exist

echo    %lib_dir%\%config_file%.LIB			>> commands.lis

if "%out_file%" == "" goto execute_tests
    echo.                                       	> %out_file%

:#
:#########################################################################
:# Now build the xray include file which will actually run the tests.
:#########################################################################
:#

:execute_tests
echo	error=continue					> %xray_cmd%
echo	ce @exc=1					>> %xray_cmd%
for %%f in (%test_dir%\*.C) do call test__1.bat %%f
echo	quit y						>> %xray_cmd%

:#
:#########################################################################
:#	And clean up
:#########################################################################
:#

:end_of_file

    if exist commands.lis		del	commands.lis
    if exist setval.bat			del	setval.bat
    if exist setval.bak			del	setval.bak
    if exist %lib_dir%\temp_s.cmd	del 	%lib_dir%\temp_s.cmd
    if exist %lib_dir%\temp.cmd		del 	%lib_dir%\temp.cmd

    set config_file=
    set xray_cmd=
 
    set test_dir=
    set conf_dir=
    set src_dir=
    set lib_dir=
    set cmd_dir=
    set out_file=
 
    set test_flag=
    set out_flag=
    set cmd_file=
