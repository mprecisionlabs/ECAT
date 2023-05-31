
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


:########################################################################
:#
:#	This batch file is intended to be called by bld_lib only.
:#	It is not intended to be user callable.  All input parameters
:#	are set as environment variables, except the pathname of the
:#	build routine.  All possible input argument testing has already
:#	been done.
:#
:# Synopsis:
:#	bld__1 {src_path}
:#	    {src_path}:  Pathname of the build routine.
:#
:########################################################################

echo	%1

if "%out_file%" == "" goto no_output

    echo %1							>> %out_file%
    MCC68K %1 -c -DCONFIGURATION_FILE=%conf_dir%%config_file%.h >> %out_file%
    goto end_file

:no_output

    MCC68K %1 -c -DCONFIGURATION_FILE=%conf_dir%%config_file%.h

:end_file
