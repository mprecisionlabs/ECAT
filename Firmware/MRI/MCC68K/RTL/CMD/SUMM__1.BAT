
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

:########################################################################
:#
:#	This batch file is intended to be called by "summary" only.
:#	It is not intended to be user callable.  All input parameters
:#	are set as environment variables, except the pathname of the
:#	test routine.  All possible input argument testing has already
:#	been done.
:#
:# Synopsis:
:#	summ__1 {test_path}
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
:#	Print header for individual test
:#########################################################################
:#

if "%out_file%" == "" goto no_log_1
    echo.						>> %out_file%
    echo TESTING: %test_file% LIBRARY: %lib_name%	>> %out_file%

:no_log_1
    echo.
    echo TESTING: %test_file% LIBRARY: %lib_name%

:#
:#########################################################################
:#	Check to be sure that the debugger output file exists
:#########################################################################
:#

if not exist %test_file%.out goto comp_failure

:#
:#########################################################################
:#	Print out results
:#########################################################################
:#

if "%out_file%" == "" goto no_log_4
    echo ***************************				>> %out_file%
    type %test_file%.out					>> %out_file%
    echo ***************************				>> %out_file%

:no_log_4
    echo ***************************
    type %test_file%.out
    echo ***************************

goto end_of_file

:#
:#########################################################################
:#	Print out results if compilation failed
:#########################################################################
:#

:comp_failure

    echo ***************************
    echo COMPILATION FAILED
    echo ***************************

if "%out_file%" == "" goto end_of_file

    echo ***************************			>> %out_file%
    echo COMPILATION FAILED				>> %out_file%
    echo ***************************			>> %out_file%

:end_of_file

