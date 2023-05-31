rem This file assumes that the working directory is on a substituted drive (not C:)
set BINARY_FILE=update.bin
set TARGET_DIRECTORY=%USERPROFILE%\ROMProgrammer\ECAT
echo.
echo Copying %BINARY_FILE% to %TARGET_DIRECTORY%...
copy .\%BINARY_FILE% %TARGET_DIRECTORY% 
dir %TARGET_DIRECTORY%\%BINARY_FILE%
echo.

