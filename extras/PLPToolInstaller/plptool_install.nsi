; PLPTool NSIS installer script
;--------------------------------

!include x64.nsh

!define PRODUCT_NAME "PLPTool 4"
!define JRE_VERSION "1.6"
!define JRE_URL "http://javadl.sun.com/webapps/download/AutoDL?BundleId=18714&/jre-6u5-windows-i586-p.exe"

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
;RequestExecutionLevel admin


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
  File "..\..\reference\sw\PLPTool\store\PLPToolStatic.jar"
  File "..\..\reference\sw\PLPTool\store\PLPToolWin.bat"
  ;File "rxtxSerial32.dll"
  ;File "rxtxSerial64.dll"
  
  ${If} ${RunningX64}
	File /oname=rxtxSerial.dll ..\..\reference\sw\PLPTool\store\rxtxSerial64.dll
  ${Else}
	File /oname=rxtxSerial.dll ..\..\reference\sw\PLPTool\store\rxtxSerial32.dll
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

Section "Check and install Java Runtime"
  Call DetectJRE
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

Function GetJRE
        MessageBox MB_OK "${PRODUCT_NAME} uses Java ${JRE_VERSION}, it will now \
                         be downloaded and installed"
 
        StrCpy $2 "$TEMP\Java Runtime Environment.exe"
        nsisdl::download /TIMEOUT=30000 ${JRE_URL} $2
        Pop $R0 ;Get the return value
                StrCmp $R0 "success" +3
                MessageBox MB_OK "Download failed: $R0"
                Quit
        ExecWait $2
        Delete $2
FunctionEnd
 
 
Function DetectJRE
  ReadRegStr $2 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment" \
             "CurrentVersion"
  StrCmp $2 ${JRE_VERSION} done
 
  Call GetJRE
 
  done:
FunctionEnd
