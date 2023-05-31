@echo off

:#
:########################################################################
:# THIS INFORMATION IS PROPRIETARY TO					#
:# MICROTEC RESEARCH, INC.						#
:#----------------------------------------------------------------------#
:# Copyright (c) 1993 Microtec Research, Inc.				#
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

echo bld_lib {library_name} [ -output {file name} ]
echo. 
echo {library_name}: Name of the library which is to be built.  This is also
echo                 the name of the configuration file used for the library
echo                 build process.
echo. 
echo -output:        The argument to this option is the name of a file
echo                 which will be used to record the results of the library
echo                 build.
echo.

goto end_of_file

:no_instructions

:#
:#########################################################################
:#
:#       Set up default symbols
:#       Also do some error checking
:#
:# conf_dir:	Directory which contain the configuration files
:# src_dir:	Directory which contains library source files
:# lib_dir:	Directory which will contain completed library file
:#
:#########################################################################
:#

set out_file=
 
:	establish defaults

set conf_dir=..\CONFIG\
set src_dir=..\SRC\
set lib_dir=..\LIB\

:#
:#########################################################################
:#	Strip .h and .lib extensions of argument
:#########################################################################
:#

del_ext	config_file	%1				> setcmd.bat
call	setcmd.bat

del	setcmd.bat

:#
:#########################################################################
:#	Test to make sure configuration file exists
:#########################################################################
:#

if exist %conf_dir%%config_file%.h goto config_exists
    echo Configuration file not accessible: %conf_dir%%config_file%.H
    goto end_of_file
:config_exists
 
:#
:#########################################################################
:#	Check for -output option and set variable if used
:#########################################################################
:#

if not "%2" == "-output" goto no_output_option
    set out_file=%3
    goto compile_files

:no_output_option
if "%2" == "" goto compile_files
    echo Unrecognized option: %2
    goto error_condition

:#
:#########################################################################
:#	First delete all *.obj and *.src files, commands assume that
:#		no extra files of these types are in the local directory
:#	Then compile all ".c" files
:#########################################################################
:#
:compile_files

if exist *.obj	del	*.obj
if exist *.src	del	*.src

echo.
echo Compiling library source files
echo.

if "%out_file%" == "" goto no_compiler_out_file

    echo.						>  %out_file%
    echo Compiling library source files			>> %out_file%
    echo.						>> %out_file%

:no_compiler_out_file

    for %%f in (..\SRC\*.C) do call bld__1.bat	%%f

:compiler_complete

:#
:#########################################################################
:#	Build library command file
:#########################################################################
:#

echo create %lib_dir%%config_file%.LIB		> library.cmd
for %%f in (*.OBJ) do echo addmod %%f		>> library.cmd
echo save					>> library.cmd
echo end					>> library.cmd

:#
:#########################################################################
:#	Now actually build the library file
:#########################################################################
:#

echo.
echo Building library file
echo.

if exist %lib_dir%%config_file%.lib	del	%lib_dir%%config_file%.lib

if "%out_file%" == "" goto no_librarian_out_file
    echo.					>> %out_file%
    echo Building library file			>> %out_file%
    echo.					>> %out_file%

    LIB68K	< library.cmd			>> %out_file%
    if errorlevel 1 goto error_condition
    goto librarian_complete

:no_librarian_out_file

    LIB68K					< library.cmd
    if errorlevel 1 goto error_condition

:#
:#########################################################################
:#	And clean up
:#########################################################################
:#

:librarian_complete

if exist *.src		del	*.src
if exist library.cmd	del	library.cmd
goto	end_of_file

:error_condition

    echo.
    echo Library build terminated.
    echo.

    if "%out_file%" == "" goto end_of_file

	echo.						>> %out_file%
	echo Library build terminated.			>> %out_file%
	echo.						>> %out_file%


:end_of_file
