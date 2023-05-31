    @ECHO off
    rem Usage:  BUILD
    rem      no options

    echo.

    rem Set the root working directory (this directory) to the root of drive M:
    rem This works around file path length issues (mcc68k.exe doesn't work
    rem   if its full path length is greater than 59 characters)
    rem Don't do this if this batch file is being called from the makefile executed in build-main.bat,
    rem   because drive M: has already been assigned.
    set MRI_DRIVE=M:
    set DO_NOT_UNSUBST=0
    if not exist %MRI_DRIVE% (
        echo Working directory %cd% is being aliased to drive %MRI_DRIVE%
        subst %MRI_DRIVE% %cd%
        pushd %MRI_DRIVE%
    ) else (
        set DO_NOT_UNSUBST=1
    )
        

    rem Set up the environment for building ECAT/LTS code using the older MRI run
    rem   time library with the newer MRI compiler/assembler/linker tools.
    call .\compiler.bat

    rem Build the Library only.
    pushd %ECATLIBSOURCE%
    rem Delete all object files (always a full build)
    if exist *.obj del *.obj
    set BUILDERR=
    call %MAKEUTIL% -f%LIBMAKEFILE%
    if %ERRORLEVEL% NEQ 0 set BUILDERR=YES
    popd

    rem Don't do anything further if Make returned an error
    if %BUILDERR%b==YESb goto ERROR
    goto END

:ERROR
    echo +++++++++++++++++++++++++++++++++++++++++++++
    echo ++++++++++++ !!! BUILD ERROR !!! ++++++++++++
    echo +++++++++++++++++++++++++++++++++++++++++++++
    goto END

:END
    rem Return to original working directory and remove substituted drive M:
    rem Don't do this if this batch file is being called from the makefile executed in build-main.bat,
    rem   because drive M: is still in use there.
    if %DO_NOT_UNSUBST%==0 (
        popd
        subst %MRI_DRIVE% /d
        echo.
    )

