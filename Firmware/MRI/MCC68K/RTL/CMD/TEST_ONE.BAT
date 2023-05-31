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

if not "%2" == "" goto no_instructions
:show_instructions

echo test_one {library_name} {test_file} [ -cmdfile {file name} ]
echo          [ -output {file name} ] [ -testsrcfile {file name ...} ]
echo.
echo The batch file "test_one" compiles an individual library test in ..\TEST
echo and generates an .ABS file for that test. It also generates an XRAY
echo Debugger include file "XRAY.INC" which executes the .ABS file and places
echo the output on the stdio viewport. 
echo.
echo {library_name}: Name of the library which is to be tested.  This is also
echo                 the name of the configuration file used for the library
echo                 build process.
echo.
echo {test_file}:    The name of the C test file which is to be executed.  This
echo                 file is located in ..\TEST.
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
:#

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

del_ext	config_file	%1					>  setval.bat
del_ext	test_file	%2					>> setval.bat
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
:#	Test to make sure test file exists
:#	Then write its name out to "commands.lis"
:#	Then begin building linker command file
:#########################################################################
:#

if exist %test_dir%\%test_file%.c goto test_exists
    echo Test file does not exist: %test_dir%\%test_file%.C
    goto end_of_file

:test_exists
    echo %test_dir%\%test_file%.C				>  commands.lis
    echo -DCONFIGURATION_FILE=%conf_dir%\%config_file%.H	>> commands.lis
    echo -DDEBUG_ON						>> commands.lis

:# 
:#########################################################################
:#	process any options
:#########################################################################
:#

:test_next_option

    if "%3" == "" goto no_more_options

    :check_file
	if not "%3" == "-testsrcfile" goto check_output
            set test_flag=1
	    set out_flag=
	    set cmd_flag=
	    shift
	    goto test_next_option
	
    :check_output
	if not "%3" == "-output" goto check_command
	    set out_flag=1
	    set test_flag=
	    set cmd_flag=
	    shift
	    goto test_next_option

    :check_command
	if not "%3" == "-cmdfile" goto save_files
	    set out_flag=
	    set test_flag=
	    set cmd_flag=1
	    shift
	    goto test_next_option

    :save_files
	if not "%test_flag%" == "1" goto save_output
	    if exist %src_dir%\%3 goto tst_s_exists
		echo Library source file does not exist:  %src_dir%\%3
		goto end_of_file
	    :tst_s_exists
	        echo %src_dir%\%3				>> commands.lis
		del_ext	lib_src	%3				> setval.bat
		call	setval.bat
		echo load %lib_src%.OBJ 		>> %lib_dir%\temp_s.cmd
	        shift
	        goto test_next_option

    :save_output
        if not "%out_flag%" == "1" goto save_cmd
	    set out_flag=
	    set out_file=%3
	    shift
	    goto test_next_option

    :save_cmd
	if not "%cmd_flag%" == "1" goto bad_option
	    set cmd_flag=
	    set cmd_file=%3
	    shift
	    goto test_next_option

    :bad_option
	echo Unrecognized option: %3
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
	echo -e%lib_dir%\%cmd_file%			>> commands.lis
	echo load %test_file%.OBJ			>  %lib_dir%\temp.cmd
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

:#
:#########################################################################
:#	Compile all ".c" files
:#########################################################################
:#

if "%out_file%" == "" goto no_log_1
    echo.							>  %out_file%
    echo COMPILING: %test_file% LIBRARY: %config_file%		>> %out_file%

:no_log_1
    echo.
    echo COMPILING: %test_file% LIBRARY: %config_file%

    if exist %test_file%.abs del %test_file%.abs

    echo %lib_dir%\%config_file%.LIB				>> commands.lis
    
    cd	%lib_dir%
    echo load %lib_dir%\%config_file%.LIB			>> temp_s.cmd
    copy %cmd_file% + temp.cmd + temp_s.cmd	%test_file%.cmd > nul
    cd %cmd_dir%

    if "%out_file%" == "" goto no_log_2
        MCC68K -dcommands.lis -c 				>> %out_file%
	if errorlevel 1 goto comp_failure
	LNK68K -c %lib_dir%\%test_file%.cmd			>> %out_file%
	if errorlevel 1 goto comp_failure
	goto compilation_done

    :no_log_2
        MCC68K -dcommands.lis -c 
	if errorlevel 1 goto comp_failure
	LNK68K -c %lib_dir%\%test_file%.cmd
	if errorlevel 1 goto comp_failure

    :compilation_done

:#
:#########################################################################
:#	Build xray include file to run the test.
:#########################################################################
:#

:build_include

echo	error=continue						> %xray_cmd%
echo	ce @exc=1						>> %xray_cmd%
echo	clear							>> %xray_cmd%
echo	load %test_file%.ABS					>> %xray_cmd%

if "%OS%" == "Windows_NT" goto windows_nt

    echo inport &_simulated_input,f=%test_dir%\%test_file%.IN	>> %xray_cmd%
    goto no_windows_nt

:windows_nt

    echo inport ^&_simulated_input,f=%test_dir%\%test_file%.IN	>> %xray_cmd%

:no_windows_nt

echo	bi _simulated_halt					>> %xray_cmd%
echo	go							>> %xray_cmd%

goto	end_of_file

:#
:#########################################################################
:#	And clean up
:#########################################################################
:#


:comp_failure

    echo COMPILATION FAILED

if "%out_file%" == "" goto end_of_file

    echo COMPILATION FAILED					>> %out_file%


:end_of_file

    if exist commands.lis		del	commands.lis
    if exist setval.bat			del	setval.bat
    if exist setval.bak			del	setval.bak
    if exist %lib_dir%\temp_s.cmd	del 	%lib_dir%\temp_s.cmd
    if exist %lib_dir%\temp.cmd		del 	%lib_dir%\temp.cmd

    set config_file=
    set test_file=
 
    set test_dir=
    set conf_dir=
    set src_dir=
    set lib_dir=
    set cmd_dir=
    set out_file=
 
    set test_flag=
    set out_flag=
    set cmd_file=
