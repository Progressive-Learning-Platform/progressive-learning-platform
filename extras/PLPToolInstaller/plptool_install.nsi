; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

!include x64.nsh

; The name of the installer
Name "PLPTool"

; The file to write
OutFile "plptoolsetup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\PLPTool4

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\PLPTool4" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "PLPTool Install (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "PLPToolStatic.jar"
  File "PLPToolWin.bat"
  ;File "rxtxSerial32.dll"
  ;File "rxtxSerial64.dll"
  
  ${If} ${RunningX64}
	File /oname=rxtxSerial.dll rxtxSerial64.dll
  ${Else}
	File /oname=rxtxSerial.dll rxtxSerial32.dll
  ${EndIf}
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\PLPTool4 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4" "DisplayName" "PLPTool Uninstallation"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\PLPTool 4"
  CreateShortCut "$SMPROGRAMS\PLPTool 4\PLPTool.lnk" "$INSTDIR\PLPToolWin.bat" "" "$INSTDIR\PLPToolWin.bat" 0
  CreateShortCut "$SMPROGRAMS\PLPTool 4\PLP Serial Terminal.lnk" "$INSTDIR\PLPToolWin.bat" "--serialterminal" 0  
  CreateShortCut "$SMPROGRAMS\PLPTool 4\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0

    
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4"
  DeleteRegKey HKLM SOFTWARE\PLPTool4

  ; Remove files and uninstaller
  Delete $INSTDIR\PLPToolStatic.jar
  Delete $INSTDIR\rxtxSerial.dll
  Delete $INSTDIR\rxtxSerial32.dll
  Delete $INSTDIR\rxtxSerial64.dll
  Delete $INSTDIR\PLPToolWin.bat
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\PLPTool 4\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\PLPTool 4"
  RMDir "$INSTDIR"

SectionEnd
