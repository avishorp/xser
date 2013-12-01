;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  !include x64.nsh

  ;Name and file
  Name "XSer"
  OutFile "XSerSetup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\XSer"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\XSer" "Install_Dir"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

  ; Require Windows Vista minimum
  ;TargetMinimalOS 6.0

;Function .onInit
;  SetOutPath $INSTDIR
;  LogSet on
;FunctionEnd

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\..\..\gpl-3.0.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections


;;
;; XSer Service
;;
Section "XSer Service" XSerServiceSect

  SetOutPath "$INSTDIR"
  
  ; XSer Files
  ; ----------
  File "..\..\Release\xserserv.exe"
  File "..\..\Release\libxser.dll"
  File "$%BOOST%\lib\boost_chrono-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_date_time-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_filesystem-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_locale-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_log-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_system-vc110-mt-1_54.dll"
  File "$%BOOST%\lib\boost_thread-vc110-mt-1_54.dll"

  ; MSVC Runtime DLLs
  ; -----------------
  File "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\redist\x86\Microsoft.VC110.CRT\msvcr110.dll"
  File "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\redist\x86\Microsoft.VC110.CRT\msvcp110.dll"
  File "C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\redist\x86\Microsoft.VC110.CRT\vccorlib110.dll"


  ;Store installation folder
  WriteRegStr HKCU "Software\Modern UI Test" "Install_Dir" $INSTDIR

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\XSer" "DisplayName" "XSer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\XSer" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\XSer" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\XSer" "NoRepair" 1

  ;Create uninstaller
  WriteUninstaller "uninstall.exe"

  ; Install the service
  nsExec::ExecToLog '"$INSTDIR\xserserv.exe" install'  

SectionEnd


;;
;; XSer Driver
;;
Section "XSer Driver" XSerDriver
  SetOutPath "$INSTDIR\driver"
  File "inf\xser.inf"
  File "inf\xser.cat"
  MessageBox MB_OK '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\driver\xser.inf"'
${DisableX64FSRedirection}
  nsExec::ExecToLog '"$SYSDIR\PnPutil.exe" /a "$INSTDIR\driver\xser.inf"'
${EnableX64FSRedirection}
SectionEnd

;--------------------------------
;Descriptions



  ;Language strings
  LangString DESC_XserServiceSect ${LANG_ENGLISH} "XSer monitor service"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${XSerServiceSect} $(DESC_XSerServiceSect)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ; Uninstall the service
  nsExec::ExecToLog '"$INSTDIR\xserserv.exe" uninstall'
  
  Delete "$INSTDIR\xserserv.exe"
  Delete "$INSTDIR\libxser.dll"
  Delete "$INSTDIR\boost_chrono-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_date_time-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_filesystem-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_locale-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_log-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_system-vc110-mt-1_54.dll"
  Delete "$INSTDIR\boost_thread-vc110-mt-1_54.dll"
  Delete "$INSTDIR\driver\xser.inf"
  Delete "$INSTDIR\driver\xser.cat"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\XSer"

SectionEnd