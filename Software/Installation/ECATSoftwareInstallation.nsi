; ECATSoftwareInstallation.nsi
;
; This script was created using Nullsoft Scriptable Install System (NSIS) v3.0a1
;
; It remembers the directory, has uninstall support,
;   and optionally installs start menu shortcuts.

; Files to be installed are assumed to be in the following directories (relative
;   to this installation script's directory):
;
;   EXE files (the applications themselves):  "..\Software Release"
;   SIM.CFG:                ".\install_files"
;   Help files:             ".\install_files\Help"
;   Surge example files:    ".\install_files\Surge"
;   EFT example files:      ".\install_files\EFT"
;   PQF example files:      ".\install_files\PQF"

;--------------------------------

!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "StrFunc.nsh"

!insertmacro MUI_PAGE_INIT
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_LANGUAGE "English"

; must call (declare?) these now in order to use later
${StrLoc}
${StrRep}

Var common_components_installed_pre
Var common_components_installed_post
Var shortcut_type
Var app_data_path

; Request application privileges for Windows Vista and newer
RequestExecutionLevel admin

; The name of the installer
Name "ECAT Software"

; The file to write
OutFile "setup.exe"

; Source file directories (relative to this script's directory)
!define EXE_SOURCE_LOCATION "..\Software Release"
!define OTHER_FILES_SOURCE_LOCATION ".\install_files"

; The default installation directory
!define APP_NAME "ECAT"
; Old app name was used with Wise Installer, before Release 2.13 (FW5.30/Surge5.35/Burst5.32/PQF3.23)
!define OLD_APP_NAME "KeyTek ECAT Software"
!define GROUP_NAME "Thermo"
!define COMPANY_NAME "Thermo Fisher Scientific"
!define GROUP_FOLDER "${GROUP_NAME} - ${APP_NAME}"
!define INSTALL_PATH "${GROUP_NAME}\${APP_NAME}"
!define OLD_INSTALLER_EXECUTABLE "UNWISE.EXE"  ; name of obsolete uninstaller executable
InstallDir $PROGRAMFILES\${INSTALL_PATH}

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
!define REGISTRY_APP_LOCATION "Software\${INSTALL_PATH}"
!define REGISTRY_UNINSTALL_LOCATION "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define REGISTRY_UNINSTALL_LOCATION_OLD "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${OLD_APP_NAME}"
InstallDirRegKey HKLM ${REGISTRY_APP_LOCATION} Install_Dir

; Set Overwrite mode to ON, so that all new file copies overwrite existing
;   files.  This is explicitly, selectively overridden where required.
SetOverwrite on

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; Sections (the components to install)

Section "!SurgeWare" sec_surgeware

    Call installCommonComponents

    ; Set output path to the installation directory.
    SetOutPath $INSTDIR
    ; Put file there
    File "${EXE_SOURCE_LOCATION}\E500.exe"
    CreateDirectory "$INSTDIR\Surge"
    SetOutPath "$INSTDIR\Surge"
    File /r "${OTHER_FILES_SOURCE_LOCATION}\Surge\*"
    CreateDirectory "$INSTDIR\Help"
    SetOutPath "$INSTDIR\Help"
    File "${OTHER_FILES_SOURCE_LOCATION}\Help\Surge.hlp"
    
    Call installSizeInRegistry

SectionEnd

Section "!BurstWare" sec_burstware

    Call installCommonComponents

    ; Set output path to the installation directory.
    SetOutPath $INSTDIR
    ; Put file there
    File "${EXE_SOURCE_LOCATION}\E400.exe"
    CreateDirectory "$INSTDIR\EFT"
    SetOutPath "$INSTDIR\EFT"
    File /r "${OTHER_FILES_SOURCE_LOCATION}\EFT\*"
    CreateDirectory "$INSTDIR\Help"
    SetOutPath "$INSTDIR\Help"
    File "${OTHER_FILES_SOURCE_LOCATION}\Help\Burst.hlp"

    Call installSizeInRegistry

SectionEnd

Section "!PQFWare" sec_pqfware

    Call installCommonComponents

    ; Set output path to the installation directory.
    SetOutPath $INSTDIR
    ; Put file there
    File "${EXE_SOURCE_LOCATION}\PQFWare.exe"
    CreateDirectory "$INSTDIR\PQF"
    SetOutPath "$INSTDIR\PQF"
    File /r "${OTHER_FILES_SOURCE_LOCATION}\PQF\*"
    CreateDirectory "$INSTDIR\Help"
    SetOutPath "$INSTDIR\Help"
    File "${OTHER_FILES_SOURCE_LOCATION}\Help\PQF.hlp"

    Call installSizeInRegistry

SectionEnd

;--------------------------------

SubSection /e "Installation options" sec_install_opt

    ; Create start menu groups and shortcuts
    Section "Start Menu Shortcuts" sec_start_shortcuts
        StrCpy $shortcut_type "start_menu"
        Call installShortcuts
        StrCpy $shortcut_type ""
        SectionEnd
    
    ; Create Desktop shortcuts
    Section /o "Desktop Shortcuts" sec_desk_shortcuts
        StrCpy $shortcut_type "desktop"
        Call installShortcuts
        StrCpy $shortcut_type ""
    SectionEnd
    
    ; COM2 and Simulation are mutually exclusive options
    Section /o "Use COM2 instead of COM1" sec_com2
        ; installation of this option is performed conditionally in installShortcuts
    SectionEnd

    Section /o "Run in Simulation Mode only" sec_sim
        ; installation of this option is performed conditionally in installShortcuts
    SectionEnd

    ; 220V/50Hz is for PQF only, display conditionally (handled in .onInit and .onSelChange)
    Section /o "220V/50Hz line (PQFWare only)" sec_220_50

        ; Write info to INI file.  The INI file is stored in the app data directory
        ;   that is writable by all users, not into the program directory
        WriteINIStr $app_data_path\PQF.ini "Line Parameters" Voltage 220
        WriteINIStr $app_data_path\PQF.ini "Line Parameters" Frequency 50
        FlushINI $app_data_path\PQF.ini

    SectionEnd

SubSectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

    SetShellVarContext all
    StrCpy $app_data_path "$APPDATA\${INSTALL_PATH}"

    ; Remove program installation directory and all contents
    RMDir /r /REBOOTOK "$INSTDIR"

    ; Remove program data that was explicitly installed or automatically
    ;   generated by the application (INI files), leaving other files such as
    ;   SIM.CFG and LOG files.  Remove directory only if it's empty.
    Delete $app_data_path\*.ini
    RMDir "$app_data_path"

    ; Remove start menu shortcuts, except possibly app_data_path shortcut
    Delete "$SMPROGRAMS\${GROUP_FOLDER}\SurgeWare.lnk"
    Delete "$SMPROGRAMS\${GROUP_FOLDER}\BurstWare.lnk"
    Delete "$SMPROGRAMS\${GROUP_FOLDER}\PQFWare.lnk"
    Delete "$SMPROGRAMS\${GROUP_FOLDER}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${GROUP_FOLDER}"

    ; Remove desktop shortcuts
    Delete $DESKTOP\SurgeWare.lnk
    Delete $DESKTOP\BurstWare.lnk
    Delete $DESKTOP\PQFWare.lnk

    ; Remove registry keys
    DeleteRegKey HKLM ${REGISTRY_UNINSTALL_LOCATION}
    DeleteRegKey HKLM ${REGISTRY_APP_LOCATION}

SectionEnd

;--------------------------------

; Functions

Function .onInit

    Push $0
    Push $R0

    ; Set the shell context to "all" to install for all users on system
    SetShellVarContext all
    
    ; Determine whether present installation is "old" (Wise Installer) or "new" (NSIS) or non-existent ("")
    Var /GLOBAL present_installation
    StrCpy $present_installation ""

    ; Uninstall present version before installing "new" version
    ; If this is a Wise Installer installation (pre-NSIS), then save
    ;   application log files, and move them to new installation location
    ReadRegStr $R0 HKLM "${REGISTRY_UNINSTALL_LOCATION}" "UninstallString"
    StrCmp $R0 "" check_for_old new
    
    new:
        StrCpy $present_installation "new"
        Goto prompt
        
    check_for_old:
        ReadRegStr $R0 HKLM "${REGISTRY_UNINSTALL_LOCATION_OLD}" "UninstallString"
        StrCmp $R0 "" after_uninstall old
        
    old:
        StrCpy $present_installation "old"
        Goto prompt
        
    Goto uninst
    
    prompt:
        MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
            "${APP_NAME} is already installed.$\n$\nSelect `OK` to remove the \
            previous version$\nor `Cancel` to cancel this upgrade." \
        IDOK uninst
        Abort

    uninst:
        ClearErrors
        ${If} $present_installation == "new"
            ; Run the uninstaller for an existing NSIS installation
            ExecWait '$R0 /S'
            Sleep 1000  ; wait  second for unintaller to complete
            Goto after_uninstall
        ${ElseIf} $present_installation == "old"
            ; Before executing uninstaller, copy LOG and CFG files (if any) to
            ;   temporary directory, to restore to new installation
            CreateDirectory $TEMP\${APP_NAME}
            ; Get installation directory to copy files from
            VAR /GLOBAL wise_install_directory
            StrCpy $wise_install_directory $R0
            ${StrLoc} $0 $wise_install_directory '\${OLD_INSTALLER_EXECUTABLE}' ">"
            StrCpy $wise_install_directory $wise_install_directory $0
            ; Copy SIM.CFG file before uninstall (because it will be deleted
            ;   during the uninstall), then copy remaining files after the
            ;   uninstall (except INI files, which will be created as necessary
            ;   during the new install or normal operation)
            CopyFiles /SILENT $wise_install_directory\sim.cfg $TEMP\${APP_NAME}
            
            ; Run the uninstaller for an existing Wise installation (previous installer utility)
            ; Insert "/S" into UninstallString for silent uninstall; must be after
            ;   uninstall executable path and name (...\UNWISE.EXE) and before install
            ;   log and name (...\INSTALL.LOG)
            Var /GLOBAL wise_uninstall_string
            Var /GLOBAL exec_with_silent_switch
            StrCpy $exec_with_silent_switch "${OLD_INSTALLER_EXECUTABLE} /S"
            StrCpy $wise_uninstall_string $R0
            ${StrRep} $wise_uninstall_string $R0 ${OLD_INSTALLER_EXECUTABLE} $exec_with_silent_switch
            ExecWait $wise_uninstall_string
            Sleep 3000  ; wait 3 seconds for unintaller to complete

            ; Copy remaining files (except INI files, which will be created as
            ;   necessary during the new install or normal operation) - see
            ;   comment before uninstall.  Also delete EXE files in case the
            ;   uninstaller is still running and got copied here (unwise.exe).
            ;   Delete the TEMP directory before ending the installation (in
            ;   function .onGUIEnd).
            CopyFiles /SILENT $wise_install_directory\*.* $TEMP\${APP_NAME}
            Delete $TEMP\${APP_NAME}\*.ini
            Delete $TEMP\${APP_NAME}\*.exe

            Goto after_uninstall

        ${EndIf}
        
    after_uninstall:
    ; End of removal of previous installation

    StrCpy $common_components_installed_pre "False"
    StrCpy $common_components_installed_post "False"
    StrCpy $shortcut_type ""
    
    ; If sec_pqfware is NOT selected, deselect and disable sec_220_50
    ; Else enable sec_220_50
    SectionGetFlags ${sec_pqfware} $0
    IntOp $0 $0 & ${SF_SELECTED}
    ${If} $0 != ${SF_SELECTED}
        !insertmacro UnSelectSection ${sec_220_50}
        !insertmacro SetSectionFlag ${sec_220_50} ${SF_RO}
    ${Else}
        !insertmacro ClearSectionFlag ${sec_220_50} ${SF_RO}
    ${EndIf}
        
    Pop $0
    Pop $R0

FunctionEnd

Function .onSelChange

    Push $0
    Push $1
    Push $2

    ; If sec_pqfware is NOT selected, deselect and disable sec_220_50
    ; Else enable sec_220_50
    SectionGetFlags ${sec_pqfware} $0
    IntOp $0 $0 & ${SF_SELECTED}
    ${If} $0 != ${SF_SELECTED}
        !insertmacro UnSelectSection ${sec_220_50}
        !insertmacro SetSectionFlag ${sec_220_50} ${SF_RO}
    ${Else}
        !insertmacro ClearSectionFlag ${sec_220_50} ${SF_RO}
    ${EndIf}

    ; sec_com2 and sec_sim are mutually exclusive.
    ;   If one is selected, deselect the other.
    StrCmp $R9 ${sec_com2} check_sec_com2
        SectionGetFlags ${sec_com2} $0
        IntOp $0 $0 & ${SF_SELECTED}
        IntCmp $0 ${SF_SELECTED} 0 done done
            StrCpy $R9 ${sec_com2}
            SectionGetFlags ${sec_sim} $0
            IntOp $0 $0 & ${SECTION_OFF}
            SectionSetFlags ${sec_sim} $0
        Goto done
    check_sec_com2:
        SectionGetFlags ${sec_sim} $0
        IntOp $0 $0 & ${SF_SELECTED}
        IntCmp $0 ${SF_SELECTED} 0 done done
            StrCpy $R9 ${sec_sim}
            SectionGetFlags ${sec_com2} $0
            IntOp $0 $0 & ${SECTION_OFF}
            SectionSetFlags ${sec_com2} $0
    done:

    ; If either sec_com2 or sec_sim is selected, force creation of start menu shortcuts,
    ;   because both of these options modify the shortcut target line.
    SectionGetFlags ${sec_start_shortcuts} $0
    SectionGetFlags ${sec_com2} $1
    SectionGetFlags ${sec_sim} $2
    IntOp $1 $1 & ${SF_SELECTED}}
    IntOp $2 $2 & ${SF_SELECTED}}
    ${If} $1 != 0
    ${OrIf} $2 != 0
        !insertmacro SetSectionFlag ${sec_start_shortcuts} ${SF_SELECTED}}
    ${EndIf}
    
    Pop $0
    Pop $1
    Pop $2
    
FunctionEnd

Function installShortcuts

    Push $0
    Push $1

    Var /GLOBAL option_string
    StrCpy $option_string ""
    Var /GLOBAL at_least_one_component_selected 
    StrCpy $at_least_one_component_selected "False"

    Var /GLOBAL shortcut_location
    ${If} $shortcut_type == "start_menu"
        StrCpy $shortcut_location "$SMPROGRAMS\${GROUP_FOLDER}"
    ${ElseIf} $shortcut_type == "desktop"
        StrCpy $shortcut_location "$DESKTOP"
    ${Else}
        Return
    ${EndIf}
    
    ; Add COM2 or simulation option to shortcut target if required
    SectionGetFlags ${sec_com2} $0
    SectionGetFlags ${sec_sim} $1
    IntOp $0 $0 & ${SF_SELECTED}}
    IntOp $1 $1 & ${SF_SELECTED}}
    ${If} $0 != 0
        StrCpy $option_string "COM2"
    ${ElseIf} $1 != 0
        StrCpy $option_string "simulation"
    ${EndIf}
    
    ; Create shortcuts for installed components
    !insertmacro SectionFlagIsSet ${sec_surgeware} ${SF_SELECTED} make_surge_link after_make_surge_link
    make_surge_link:
        StrCpy $at_least_one_component_selected "True"
        CreateDirectory $shortcut_location
        CreateShortCut "$shortcut_location\SurgeWare.lnk" "$INSTDIR\E500.exe" $option_string "$INSTDIR\E500.exe" 0
        
    after_make_surge_link:
    
    !insertmacro SectionFlagIsSet ${sec_burstware} ${SF_SELECTED} make_burst_link after_make_burst_link
    make_burst_link:
        ${If} $at_least_one_component_selected != "True"
            StrCpy $at_least_one_component_selected "True"
            CreateDirectory $shortcut_location
        ${EndIf}
        CreateShortCut "$shortcut_location\BurstWare.lnk" "$INSTDIR\E400.exe" $option_string "$INSTDIR\E400.exe" 0
        
    after_make_burst_link:
    
    !insertmacro SectionFlagIsSet ${sec_pqfware} ${SF_SELECTED} make_pqf_link after_make_pqf_link
    make_pqf_link:
        ${If} $at_least_one_component_selected != "True"
            StrCpy $at_least_one_component_selected "True"
            CreateDirectory $shortcut_location
        ${EndIf}
        CreateShortCut "$shortcut_location\PQFWare.lnk" "$INSTDIR\PQFWare.exe" $option_string "$INSTDIR\PQFWare.exe" 0
        
    after_make_pqf_link:

    ${If} $at_least_one_component_selected == "True"
    ${AndIf} $shortcut_type == "start_menu"
        ; Create a Start Menu 'Uninstall' shortcut; don't create a Desktop shortcut for this
        CreateShortCut "$shortcut_location\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0

        ; Create a Start Menu shortcut for the Application Data folder; don't create a Desktop shortcut for this
        CreateShortCut "$shortcut_location\Application Data Folder.lnk" "$app_data_path" "" "$app_data_path" 0

    ${EndIf}
    
    Pop $0
    Pop $1
	
FunctionEnd

Function installCommonComponents

    ; Run this only if one of the primary components (Surge, Burst, PQF) is selected.
    ; This function should only be called from these primary components,
    ;   and only once per installation.
    
    ; First, find old-style existing installation, copy .INI, .LOG, and .CFG
    ;   files (except for INSTALL.LOG) to %TEMP% directory.  Then uninstall
    ;   that installation.  Move (don't copy) those files from %TEMP% _after_
    ;   this installation is mostly complete (to overwrite existing files).
    
    ${If} $common_components_installed_pre == "True"
        Return
    ${EndIf}

    ; Copy files from older installation (if any, presently stored in $TEMP
    ;   directory) to this installation's Application Data/Program Data
    ;   directory, and remove $TEMP directory.  See comment in .onInit following
    ;   uninstall.
    ; None of these files, except SIM.CFG, are part of the new installation;
    ;   they're files that were generated by/for the user at some time after
    ;   the previous installation.
    ; Don't overwrite existing files.
    SetOverwrite off
    CopyFiles /SILENT $TEMP\${APP_NAME}\*.* $app_data_path
    SetOverwrite on
    RMDir /r "$TEMP\${APP_NAME}"

    ; Install simulation file into directory writable by all users.
    ;   Don't overwrite if still present from previous installation.
    StrCpy $app_data_path "$APPDATA\${INSTALL_PATH}"
    SetOutPath $app_data_path
    SetOverwrite off
    File "${OTHER_FILES_SOURCE_LOCATION}\sim.cfg"
    SetOverwrite on

    ; Write the installation path into the registry
    WriteRegStr HKLM ${REGISTRY_APP_LOCATION} Install_Dir "$INSTDIR"

    ; Write the uninstall keys for Windows
    WriteRegStr HKLM ${REGISTRY_UNINSTALL_LOCATION} "DisplayName" ${APP_NAME}
    WriteRegStr HKLM ${REGISTRY_UNINSTALL_LOCATION} "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteRegStr HKLM ${REGISTRY_UNINSTALL_LOCATION} "Publisher" "${COMPANY_NAME}"
    WriteRegDWORD HKLM ${REGISTRY_UNINSTALL_LOCATION} "NoModify" 1
    WriteRegDWORD HKLM ${REGISTRY_UNINSTALL_LOCATION} "NoRepair" 1
    
    SetOutPath $INSTDIR
    WriteUninstaller "uninstall.exe"
    
    StrCpy $common_components_installed_pre "True"
    
FunctionEnd

Function installSizeInRegistry

    ; To be called each time a component is installed (at the end of each section),
    ;   to update total installed size.

    Push $0
    Push $1
    Push $2
    
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD HKLM "${REGISTRY_UNINSTALL_LOCATION}" "EstimatedSize" "$0"
    
    Pop $0
    Pop $1
    Pop $2

FunctionEnd
