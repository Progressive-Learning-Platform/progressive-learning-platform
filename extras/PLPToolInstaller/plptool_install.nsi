; PLPTool NSIS installer script
;--------------------------------

!include x64.nsh
!include FileAssociation.nsh

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
  File "..\..\reference\sw\PLPTool\store\PLPToolStatic.jar"

  IfFileExists $INSTDIR\PLPToolWin.bat 0 +2
    Delete $INSTDIR\PLPToolWin.bat    
  Push `cd /D $INSTDIR$\r$\njava -Djava.library.path=. -jar PLPToolStatic.jar %1 %2 %3 %4 %5 %6 %7 %8 %9$\r$\n`
  Push `$INSTDIR\PLPToolWin.bat`
  Call WriteToFile
  
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

Section "Download Java Runtime (if not already installed)"
  Call DetectJRE
SectionEnd

Section "Associate PLP projects with PLPTool"
  ${registerExtension} "$INSTDIR\PLPToolWin.bat" ".plp" "PLP project"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\PLPTool 4"
  CreateShortCut "$SMPROGRAMS\PLPTool 4\PLPTool.lnk" "$INSTDIR\PLPToolWin.bat" "" "$INSTDIR\PLPToolWin.bat" 0
  CreateShortCut "$SMPROGRAMS\PLPTool 4\PLP Serial Terminal.lnk" "$INSTDIR\PLPToolWin.bat" "--serialterminal" 0  
  CreateShortCut "$SMPROGRAMS\PLPTool 4\Install Directory.lnk" "$INSTDIR" "" 0    
  CreateShortCut "$SMPROGRAMS\PLPTool 4\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0   
  
SectionEnd

Section "Example Programs and Software Library"
  File /r "..\..\reference\sw\examples"
  File /r "..\..\reference\sw\libplp"
SectionEnd

Section /o "PLP CPU Hardware for Nexys2/3"
  File /r "hardware"
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool4"
  DeleteRegKey HKLM SOFTWARE\PLPTool4

  ; Remove file association
  ${unregisterExtension} ".plp" "PLP project"
  
  ; Remove files and uninstaller
  Delete $INSTDIR\PLPToolStatic.jar
  Delete $INSTDIR\rxtxSerial.dll
  Delete $INSTDIR\rxtxSerial32.dll
  Delete $INSTDIR\rxtxSerial64.dll
  Delete $INSTDIR\PLPToolWin.bat
  Delete $INSTDIR\uninstall.exe
  Delete "$INSTDIR\libplp\*.*"
  Delete "$INSTDIR\examples\*.*"
  Delete "$INSTDIR\hardware\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\PLPTool 4\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\PLPTool 4"
  RMDir "$INSTDIR\libplp"
  RMDir "$INSTDIR\examples"
  RMDir "$INSTDIR\hardware"
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

Function WriteToFile
Exch $0 ;file to write to
Exch
Exch $1 ;text to write
 
  FileOpen $0 $0 a #open file
  FileSeek $0 0 END #go to end
  FileWrite $0 $1 #write to file
  FileClose $0
 
Pop $1
Pop $0
FunctionEnd
 
!macro WriteToFile NewLine File String
  !if `${NewLine}` == true
  Push `${String}$\r$\n`
  !else
  Push `${String}`
  !endif
  Push `${File}`
  Call WriteToFile
!macroend
!define WriteToFile `!insertmacro WriteToFile false`
!define WriteLineToFile `!insertmacro WriteToFile true`
