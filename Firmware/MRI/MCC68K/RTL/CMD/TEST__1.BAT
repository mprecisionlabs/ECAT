
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

:########################################################################
:#
:#	This batch file is intended to be called by test_lib only.
:#	It is not intended to be user callable.  All input parameters
:#	are set as environment variables, except the pathname of the
:#	test routine.  All possible input argument testing has already
:#	been done.
:#
:# Synopsis:
:#	test__1 {test_path}
:#	    {test_path}:  Pathname of the test routine.
:#
:########################################################################

:#
:#########################################################################
:#	Strip file extension off test routine pathname
:#	and set to "test_file".
:#########################################################################
:#

del_ext	test_file	%1				> setval.bat
call	setval.bat
del_dir	test_file	%test_file%			> setval.bat
call	setval.bat
del	setval.bat

:#
:#########################################################################
:#	Double check to insure that test routine still exists
:#########################################################################
:#

if exist %test_dir%\%test_file%.C goto run_compile
    echo Test routine does not exist:  %test_dir%\%test_file%.C
    goto end_of_file

:run_compile

:#
:#########################################################################
:#	Compile all ".c" files
:#########################################################################
:#

if "%out_file%" == "" goto no_log_1
    echo.						>> %out_file%
    echo COMPILING: %test_file% LIBRARY: %config_file%	>> %out_file%

:no_log_1
    echo.
    echo COMPILING: %test_file% LIBRARY: %config_file%

    if exist %test_file%.abs del %test_file%.abs

    cd %lib_dir%
    echo load %test_file%.OBJ					> temp.cmd
    copy %cmd_file% + temp.cmd + temp_s.cmd	%test_file%.cmd	> nul
    cd %cmd_dir%

    if "%out_file%" == "" goto no_log_2
        MCC68K %test_dir%\%test_file%.c -dcommands.lis -c	>> %out_file%
	if errorlevel 1 goto comp_failure
	LNK68K -c %lib_dir%\%test_file%.cmd			>> %out_file%
	if errorlevel 1 goto comp_failure
	goto compilation_done

    :no_log_2
        MCC68K %test_dir%\%test_file%.c -dcommands.lis -c
	if errorlevel 1 goto comp_failure
	LNK68K -c %lib_dir%\%test_file%.cmd
	if errorlevel 1 goto comp_failure

    :compilation_done

:#
:#########################################################################
:#	Add information to xray.cmd file to get debugger to run
:#	test.
:#########################################################################
:#

:build_include


    if exist %test_dir%\%test_file%.in goto dot_in_exists
	echo.					>%test_dir%\%test_file%.in

    :dot_in_exists

    echo clear							   >> %xray_cmd%
    echo load %test_file%.ABS					   >> %xray_cmd%

    if "%OS%" == "Windows_NT" goto windows_nt

        echo inport &_simulated_input,f=%test_dir%\%test_file%.IN  >> %xray_cmd%
        echo outport &_simulated_output,f=%test_file%.OUT	   >> %xray_cmd%
	goto no_windows_nt

    :windows_nt

        echo inport ^&_simulated_input,f=%test_dir%\%test_file%.IN >> %xray_cmd%
        echo outport ^&_simulated_output,f=%test_file%.OUT	   >> %xray_cmd%

    :no_windows_nt

    echo bi _simulated_halt					   >> %xray_cmd%
    echo go							   >> %xray_cmd%

:include_complete

goto end_of_file

:#
:#########################################################################
:#	Print out results if compilation failed
:#########################################################################
:#

:comp_failure

    echo COMPILATION FAILED

if "%out_file%" == "" goto end_of_file

    echo COMPILATION FAILED				>> %out_file%

:end_of_file

