; PLPTool NSIS installer script
;--------------------------------

!include x64.nsh
!include FileAssociation.nsh

!define PRODUCT_NAME "PLPTool 5"
!define JRE_VERSION "1.6"
!define JRE_URL "http://javadl.sun.com/webapps/download/AutoDL?BundleId=18714&/jre-6u5-windows-i586-p.exe"

; The name of the installer
Name "PLPTool"

; The file to write
OutFile "plptool5.exe"

; The default installation directory
InstallDir $PROGRAMFILES\PLPTool5

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\PLPTool5" "Install_Dir"

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
;Section "Download Java Runtime (if not already installed)"
;  Call DetectJRE
;SectionEnd

Section "PLPTool Install (required)"
  Call DetectJREandFail
  SectionIn RO

  SetShellVarContext all
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "..\..\PLPTool\store\PLPToolStatic.jar"

  IfFileExists $INSTDIR\PLPToolWin.bat 0 +2
    Delete $INSTDIR\PLPToolWin.bat    
  Push `cd /D $INSTDIR$\r$\n"$R0" -Djava.library.path=. -jar PLPToolStatic.jar -W -D .\resources %1 %2 %3 %4 %5 %6 %7 %8 %9$\r$\n`
  Push `$INSTDIR\PLPToolWin.bat`
  Call WriteToFile
  
  IfFileExists $INSTDIR\PLPToolWinSafe.bat 0 +2
    Delete $INSTDIR\PLPToolWinSafe.bat    
  Push `cd /D $INSTDIR$\r$\n"$R0" -Djava.library.path=. -jar PLPToolStatic.jar -N %1 %2 %3 %4 %5 %6 %7 %8 %9$\r$\n`
  Push `$INSTDIR\PLPToolWinSafe.bat`
  Call WriteToFile
  
  ${If} ${RunningX64}
	File /oname=rxtxSerial.dll ..\..\PLPTool\store\rxtxSerial64.dll
  ${Else}
	File /oname=rxtxSerial.dll ..\..\PLPTool\store\rxtxSerial32.dll
  ${EndIf}
  
  CreateDirectory "$INSTDIR\resources"
  CreateDirectory "$INSTDIR\resources\web"
  SetOutPath $INSTDIR\resources
  File "..\..\..\..\misc\art\plptool\appicon.ico"
  File "..\..\..\..\misc\art\plptool\terminal.ico"
  File "..\..\..\..\misc\art\plptool\folder.ico"
  File "..\..\..\..\misc\art\plptool\project.ico"
  SetOutPath $INSTDIR
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\PLPTool5 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool5" "DisplayName" "PLPTool Uninstallation"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool5" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool5" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool5" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Associate PLP projects with PLPTool"
  ${unregisterExtension} ".plp" "PLPTool"
  ${unregisterExtension} ".plp" "PLP Project"
  ${unregisterExtension} ".plp" "with PLPTool"
  ${unregisterExtension} ".plp" "Edit with PLPTool"
  ${registerExtension} "$INSTDIR\PLPToolWin.bat" ".plp" "PLPTool" "$INSTDIR\resources\project.ico"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\PLPTool 5"
  CreateShortCut "$SMPROGRAMS\PLPTool 5\PLPTool.lnk" "$INSTDIR\PLPToolWin.bat" "" "$INSTDIR\resources\appicon.ico" 0
  CreateShortCut "$SMPROGRAMS\PLPTool 5\PLPTool (Safe Mode).lnk" "$INSTDIR\PLPToolWinSafe.bat" "" "$INSTDIR\resources\appicon.ico" 0
  CreateShortCut "$SMPROGRAMS\PLPTool 5\PLP Serial Terminal.lnk" "$INSTDIR\PLPToolWin.bat" "--serialterminal" "$INSTDIR\resources\terminal.ico" 0  
  CreateShortCut "$SMPROGRAMS\PLPTool 5\Browse Installation Directory.lnk" "$INSTDIR" "" "$INSTDIR\resources\folder.ico" 0    
  CreateShortCut "$SMPROGRAMS\PLPTool 5\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0   
  
SectionEnd

Section "Example Programs and Software Library"
  SetOutPath $INSTDIR\examples
  File /r "..\..\examples\*.plp"
  SetOutPath $INSTDIR\libplp
  File /r "..\..\libplp\*.asm"
SectionEnd

Section "PLPTool Extensions"
  SetOutPath $INSTDIR\resources
  File "..\..\PLPTool-Extensions\AutoSaver5\dist\AutoSaver5.jar"
  File "..\..\PLPTool-Extensions\Flowchart\dist\Flowchart.jar"
  File "..\..\PLPTool-Extensions\CacheSim\dist\CacheSim.jar"
  SetOutPath $INSTDIR\resources\web
  File "..\..\PLPTool-Extensions\SocketInterface\dist\SocketInterface.jar"
  SetOutPath $INSTDIR
SectionEnd

;Section /o "PLP CPU Hardware for Nexys2/3"
;  File /r "hardware"
;SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  SetShellVarContext all
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PLPTool5"
  DeleteRegKey HKLM SOFTWARE\PLPTool5

  ; Remove file association
  ${unregisterExtension} ".plp" "PLPTool"
  ${unregisterExtension} ".plp" "PLP Project"
  ${unregisterExtension} ".plp" "with PLPTool"
  ${unregisterExtension} ".plp" "Edit with PLPTool"
  
  ; Remove files and uninstaller
  Delete $INSTDIR\PLPToolStatic.jar
  Delete $INSTDIR\rxtxSerial.dll
  Delete $INSTDIR\rxtxSerial32.dll
  Delete $INSTDIR\rxtxSerial64.dll
  Delete $INSTDIR\PLPToolWin.bat
  Delete $INSTDIR\PLPToolWinSafe.bat
  Delete $INSTDIR\uninstall.exe
  Delete "$INSTDIR\libplp\*.*"
  Delete "$INSTDIR\examples\*.*"
  Delete "$INSTDIR\hardware\*.*"
  Delete "$INSTDIR\resources\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\PLPTool 5\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\PLPTool 5"
  RMDir "$INSTDIR\libplp"
  RMDir "$INSTDIR\examples"
  RMDir "$INSTDIR\hardware"
  RMDir "$INSTDIR\resources"
  RMDir "$INSTDIR"

SectionEnd

Function DetectJREandFail

  ${If} ${RunningX64}
	SetRegView 64
  ${Else}
	SetRegView 32
  ${EndIf}

  ReadRegStr $R1 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment" "CurrentVersion"
    ReadRegStr $R0 HKLM "SOFTWARE\JavaSoft\Java Runtime Environment\$R1" "JavaHome"
    StrCpy $R0 "$R0\bin\java.exe"
    IfErrors error
    IfFileExists $R0 0 error
   
   Goto done
   
   error:
    MessageBox MB_ICONSTOP "Failed to detect a Java Runtime Environment! You can download Java Runtime from www.oracle.com"
    Abort   
   
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
