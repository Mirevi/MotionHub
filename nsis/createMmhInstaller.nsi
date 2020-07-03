Name "MotionHub"
!define INSTALLATIONNAME "MotionHub"
OutFile "Install MotionHub.exe"
InstallDir $PROGRAMFILES64\${INSTALLATIONNAME}

Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

LicenseData "..\LICENSE.txt"

Section ""
  SetOutPath $INSTDIR
  File /r "..\build\bin\"
  WriteUninstaller $INSTDIR\uninstall.exe
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "DisplayName" "MotionHub"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}" "NoRepair" 1
SectionEnd

Section "License"
  File "..\LICENSE.txt"
SectionEnd

Section "Shortcuts"
  CreateDirectory "$SMPROGRAMS\${INSTALLATIONNAME}"
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${INSTALLATIONNAME}\MotionHub.lnk" "$INSTDIR\MireviMotionHub.exe" "" "$INSTDIR\MireviMotionHub.exe" 0
  CreateShortcut "$DESKTOP\MotionHub.lnk" "$INSTDIR\MireviMotionHub.exe"
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALLATIONNAME}"
  Delete $INSTDIR\MireviMotionHub.exe
  Delete  $INSTDIR\license.txt
  Delete $DESKTOP\MotionHub.lnk
  Delete $INSTDIR\uninstall.exe
  RMDir /r $INSTDIR
  Delete "$SMPROGRAMS\${INSTALLATIONNAME}\*.*"
  RMDir "$SMPROGRAMS\${INSTALLATIONNAME}"
SectionEnd
