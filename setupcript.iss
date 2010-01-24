[Setup]
AppName=KillAutorun
AppVerName=KillAutorun version 1.0 alpha
DefaultDirName={pf}\KillAutorun
DefaultGroupName=KillAutorun
UninstallDisplayIcon={app}\KillAutorun.exe
OutputDir=setup
AlwaysRestart=yes

[Files]
Source: "Release\KillAutorun.exe"; DestDir: "{app}"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\KillAutorun"; Filename: "{app}\KillAutorun.exe"

; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "KillAutorun"; ValueData: "{app}\KillAutorun.exe"
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Policies\Explorer"; ValueType: dword; ValueName: "NoDriveTypeAutoRun"; ValueData: "$7c"
