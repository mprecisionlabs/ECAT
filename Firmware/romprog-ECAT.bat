    @echo off

    rem This utility programs the ECAT ROMs using the Dataman programmer and Windows application.
    rem The Dataman Windows application is automatically started if it's not already running.

    set DATAMAN_CMD="C:\Program Files (x86)\Dataman\Dataman-Pro\pg4uwcmd.exe"
    set SOURCE_DIR=C:\ROMProgrammer\ECAT
    set SOURCE_BINARY=update.bin

    rem Dataman project files for legacy EPROM devices (28F010)
    set ROM_HIGH_PROJ=ecatdevhi.eprj
    set ROM_LOW_PROJ=ecatdevlo.eprj

    rem Dataman project files for SST 39SF040 Flash devices
    rem - note that insertion check is turned off because pins are lifted
    rem set ROM_HIGH_PROJ=ecat-sst39sf040-hi.eprj
    rem set ROM_LOW_PROJ=ecat-sst39sf040-lo.eprj

    echo.
    echo Programming ECAT ROMs using Dataman-48Pro2 programmer
    echo.
    echo.
    echo Insert HIGH ROM into programmer...
    pause >nul

    %DATAMAN_CMD% /prj:%SOURCE_DIR%\%ROM_HIGH_PROJ% /loadfile:%SOURCE_DIR%\%SOURCE_BINARY% /program:noanyquest

    rem Detect result of command line execution
    rem - errorlevel >= 1 indicates error

    if errorlevel 1 goto FAILURE_HIGH

    echo.
    echo ____________________
    echo _______      _______
    echo _______ GOOD _______
    echo _______      _______
    echo ____________________
    echo.
    echo HIGH ROM programming complete.
    echo.
    echo Remove and insert LOW ROM into programmer...
    pause >nul

    %DATAMAN_CMD% /prj:%SOURCE_DIR%\%ROM_LOW_PROJ% /loadfile:%SOURCE_DIR%\%SOURCE_BINARY% /program:noanyquest

    rem Detect result of command line execution
    rem - errorlevel >= 1 indicates error

    if errorlevel 1 goto FAILURE_LOW

    echo.
    echo ____________________
    echo _______      _______
    echo _______ GOOD _______
    echo _______      _______
    echo ____________________
    echo.
    echo LOW ROM programming complete.

    goto END

:FAILURE_HIGH
    set FAIL_MODE=HIGH
    goto FAILURE

:FAILURE_LOW
    set FAIL_MODE=LOW
    goto FAILURE

:FAILURE
    echo ****************************************
    echo ****************************************
    echo        Error programming %FAIL_MODE% ROM
    echo ****************************************
    echo ****************************************
    echo.
    echo See Windows application activity log window for details
    goto END

:END
    echo.
    echo End of run, press any key to exit...
    pause >nul


