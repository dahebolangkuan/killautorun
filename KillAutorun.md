A utility used to prevent damage from malicious autorun.inf file

# Introduction #

Autorun is widely used by computer virus to infect other computer via removable drives. Windows XP is particularly vulnerable to this kind of transmission due to its behavior of executing autorun.inf file automatically (Vista and 7 do not execute autorun script without user approval).

KillAutorun will help protect your Windows XP system using these steps:
  1. Disabling automatic execution of autorun script upon insertion of removable device.
  1. Automatic deletion of autorun.inf file from your removable drive upon insertion. This is done to prevent accidental invocation of autorun script by double-clicking drive's icon from Windows Explorer.
  1. Automatic creation of a special folder named 'AUTORUN.INF' that cannot be deleted to prevent creation of malicious autorun script by malware.

KillAutorun is recommended for Windows XP user who often logged in to their computer as administrative user. You can also use KillAutorun on Vista and 7 but some features might not work due to UAC.