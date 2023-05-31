These diskette images should no longer be required.
Read the document "EMCPro & CEMaster Firmware.txt"
in the project root directory.

-------------------------------------------------------------------------------

All tools required to build ECAT, EMCPro/EMCPro1089, and CEMaster
are archived in SourceSafe.

Two sets of tools are included here:  one for Windows 3.1 computers, and the
other for Window 95/98/NT/XP etc.  As of this time (February 2004), we use
Windows XP for building all applications.  The Windows 3.1 version has been
used for EMCPro/CEMaster debugging, though this activity hasn't been performed
for several years, and could probably use the newer version with a little
effort.

For building applications, install only the "MCC68K C Compiler" and
the "ASM68K Assembler".  For debugging on a Windows 3.1 computer, only
the "XHM68K XRAY Debugger Monitor (C)" is required (and possibly
"Win32s").  For Windows 95/98/NT debugging, use the "XOM68K XRAY
Debugger Monitor (C++)" (as of this version of XRAY, XOM68K replaces
XHM68K for C language debugging).

For either tool set, copy each of its subfolders to a separate floppy
disk in order to install (required for the installation program to
function correctly).  Run Setup.exe on the Installation disk and
select only the components to be installed.

Use serial number "M-026442".

For more complete information on setting up the development environments for
either ECAT or EMCPro/EMCPro1089/CEMaster, see the following documents
(referenced here by their SourceSafe project locations):

	ECAT ..................	$/ECAT Firmware/Current/readme.txt
	EMCPro/CEMaster .......	$/CmplMstr/EMCPro & CEMaster Firmware.txt
