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

if not "%1" == "" goto no_instructions
:show_instructions

echo summary {library_name} [ -output {file name} ]
echo.
echo The batch file "summary" generates an easily readable output summary of
echo the tests run by the batch file "test_lib".  
echo.
echo {library_name}: Name of the library which is to be tested.  This is also
echo                 the name of the configuration file used for the library
echo                 build process.
echo.
echo -output:        The argument to this option is the name of a file
echo                 which will be used to record the results of the test.
echo.

goto end_of_file

:no_instructions

:#
:#########################################################################
:#	This batch file will generate a "summary" output of all the
:#	test files in the ..\test directory.
:#########################################################################
:#

:#
:#########################################################################
:#
:#	 Store arguments
:#       Set up default symbols
:#
:# test_dir:	Directory which contains test routines
:#
:#########################################################################

    set	 lib_name=%1
    set  test_dir=..\TEST
    set  out_file=
 
    set  out_flag=


:# 
:#########################################################################
:#	process any options
:#########################################################################
:#

:test_next_option

    if "%2" == "" goto no_more_options

    :check_output
	if not "%2" == "-output" goto save_output
	    set out_flag=1
	    shift
	    goto test_next_option

    :save_output
        if not "%out_flag%" == "1" goto bad_option
	    set out_flag=
	    set out_file=%2
	    shift
	    goto test_next_option

    :bad_option
	echo Unrecognized option: %2
	goto end_of_file
 
:no_more_options

:#
:#########################################################################
:#      Test to insure there are test routines where we expect them
:#########################################################################
:#

if exist %test_dir%\*.c goto tests_exist
    echo Cannot find test routines in: %test_dir%\*.C
    goto end_of_file

:tests_exist

if exist %out_file%		del %out_file%
for %%f in (%test_dir%\*.C) do	call summ__1.bat %%f

:#
:#########################################################################
:#	And clean up
:#########################################################################
:#

:end_of_file

    if exist setval.bat			del setval.bat

    set test_file=
    set lib_name=
    set test_dir=
    set out_file=
 
    set out_flag=
