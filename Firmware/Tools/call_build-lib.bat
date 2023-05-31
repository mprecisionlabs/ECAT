    @echo off

    rem Helper script required by main ECAT makefile
    rem     in order to recursively build library makefile
    pushd %ECATPROJ%
    call build-lib.bat
    popd

