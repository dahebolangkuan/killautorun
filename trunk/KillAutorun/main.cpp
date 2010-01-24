#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <Dbt.h>
#include "resource.h"
#include "banned.h"
#include "main.h"

HICON hIcon;
HINSTANCE hinstance;
NOTIFYICONDATA	nib;
HWND hDlg = NULL;
bool isUsb = true;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	MSG        Msg;
	HWND       hWnd;
	WNDCLASS	wndclass;

	int tmp = 0;

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter; 
	HDEVNOTIFY hDeviceNotify = NULL; 
	     
	static const GUID GuidDevInterfaceList[] = 
	{ 
	  { 0xa5dcbf10, 0x6530, 0x11d2, { 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed } }, 
	  { 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } }, 
	  { 0x4d1e55b2, 0xf16f, 0x11Cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } }, 
	  { 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } } 
	}; 
	
	DisableAutorun();

	//CreateMutexEx(NULL, L"KillAutorunMutex", CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);
	CreateMutex(NULL, TRUE, TEXT("KillAutorunMutex"));
	tmp = GetLastError();
	if(tmp == ERROR_ALREADY_EXISTS)
		return 0;
	
	hIcon = static_cast<HICON>(LoadImage(hInstance,
                                       MAKEINTRESOURCE(MAINICON),
                                       IMAGE_ICON,
                                       16,
                                       16,
                                       LR_DEFAULTSIZE));

	
	hinstance = hInstance;
	static TCHAR szAppName[] = TEXT ("KillAutorun") ; 
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;
    if (!RegisterClass (&wndclass))
    {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		return 0 ;
    }

	hWnd = CreateWindow(szAppName,
	     TEXT(""),
	     WS_BORDER,
	     CW_USEDEFAULT,
             CW_USEDEFAULT,
	     CW_USEDEFAULT,
	     CW_USEDEFAULT,
              HWND_MESSAGE,
             NULL,
	     hInstance,
	     NULL);

	nib.cbSize = sizeof(NOTIFYICONDATA);
	nib.hIcon = hIcon;
	nib.hWnd = hWnd;
	nib.uID = 1;
	nib.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	tmp = _tcslen(szAppName)+1;
	_tcscpy_s(nib.szTip, tmp, szAppName);
	nib.uCallbackMessage = WM_USER_NIC;
	nib.uVersion = NOTIFYICON_VERSION;

	Shell_NotifyIcon(NIM_ADD, &nib);
	
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter)); 
	 
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE); 
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE; 
	 
	for (int i = 0; i < sizeof(GuidDevInterfaceList); i++) 
	{ 
	  NotificationFilter.dbcc_classguid = GuidDevInterfaceList[i]; 
	 
	  hDeviceNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE); 
	  if (hDeviceNotify == NULL) 
	  { 
		// Handle the error... 
	  } 
	}

	while( GetMessage(&Msg, NULL, 0, 0) )
	{
             TranslateMessage(&Msg);
             DispatchMessage(&Msg);
	}
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	POINT	point ;
	static HMENU hMenu;
    switch(Msg)
    {
	case WM_CREATE:
          hMenu = LoadMenu(hinstance, MAKEINTRESOURCE(IDR_MENU1));
		  hMenu = GetSubMenu(hMenu, 0);
          break;
    case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nib);
        PostQuitMessage(WM_QUIT);
        break;

	case WM_DEVICECHANGE: 
    { 
      PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR) lParam; 
 
      switch (wParam) 
      { 
        case DBT_DEVICEARRIVAL: 
			if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME ) 
			{ 
				PDEV_BROADCAST_VOLUME pVol = (PDEV_BROADCAST_VOLUME) pHdr; 
				char cDriveLetter = GetDriveLetter(pVol->dbcv_unitmask); 
				CleanAutorun(cDriveLetter);
			} 
			else if(pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE){
				PDEV_BROADCAST_DEVICEINTERFACE pVol = (PDEV_BROADCAST_DEVICEINTERFACE) pHdr; 
				//char cDriveLetter = GetDriveLetter(pVol->dbcv_unitmask); 
				if(IsDiskDrive(pVol->dbcc_name)&&(!isUsb)){
					WorstCaseCleanAutorun();
				}
			}
        break; 
      } 
    } 
    break;
	case WM_USER_NIC:
		if(wParam == 1){
			if(lParam == WM_RBUTTONUP){
				GetCursorPos(&point);
				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL) ;
			}
		}
		
		break;
	case WM_COMMAND:
          switch (LOWORD (wParam))
          {
			case ID_POPUP_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
			  break;
			case ID_POPUP_OPTIONS:
				if(hDlg == NULL)
					hDlg = CreateDialog(hinstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, reinterpret_cast<DLGPROC>(DlgProc));
				ShowWindow(hDlg, SW_SHOW);
			  break;
		  }
		  break;
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDCLOSE:
		case IDCANCEL:
			EndDialog(hWndDlg, 0);
			hDlg = NULL;
			return TRUE;
			break;
		case IDOK:
			EndDialog(hWndDlg, 0);
			hDlg = NULL;
			return TRUE;
		}
		break;
	}

	return FALSE;
}

char GetDriveLetter(unsigned long ulUnitMask) 
{ 
  char c = 0;
  for (c = 0; c < 26; c++) 
  { 
    if (ulUnitMask & 0x01) 
    { 
      break; 
    } 
 
    ulUnitMask = ulUnitMask >> 1; 
  } 
 
  return (c + 'A'); 
}

/// <summary>
/// function to delete autorun.inf file from the specified drive
/// </summary>
void CleanAutorun(char DriveLetter){
	char szMessage[80]; 
	char file[256];
	wsprintfA(file, "%c:\\autorun.inf", DriveLetter);
	
	SetFileAttributesA(file, FILE_ATTRIBUTE_NORMAL);
	DeleteFileA(file);	//delete autorun script if exist

	//wsprintfA(szMessage, "Device '%c:' has been inserted.", DriveLetter); 
	//MessageBoxA(NULL, szMessage, "USB Notice", MB_OK); 
}

/// <summary>
/// Set registry value to disable autorun
/// </summary>
void DisableAutorun(){
	HKEY   hkey;
	DWORD  dwDisposition;
	LONG res;
	DWORD dwType, dwSize, dwData;
 
	res = RegCreateKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), &hkey);
	if(res == ERROR_SUCCESS)
	{
		dwType = REG_DWORD;
		dwSize = sizeof(DWORD);
		dwData = 0x7c;
		RegSetValue(hkey, TEXT("NoDriveTypeAutoRun"), dwType, (LPCTSTR)&dwData, dwSize);
	 
		RegCloseKey(hkey);
	}
}

/// <summary>
/// I cant figure out how to find drive letter for certain removable media,
/// so I have no choice but to use brute force :(
/// </summary>
void WorstCaseCleanAutorun(){
	for(int i = 0; i < 60; i++){
		for(char c = 'a'; c<='z'; c++){
			CleanAutorun(c);
		}
		Sleep(1000);
	}
}

/// <summary>
/// detect if device inserted is a disk drive
/// </summary>
bool IsDiskDrive(const TCHAR *name){
	TCHAR buffer1[256], buffer2[256], buffer3[256];
	TCHAR tmp;
	int namelen;
	int lastIndex = 4, i = 0;
	namelen = _tcsclen(name);
	ZeroMemory(buffer1, sizeof(buffer1));
	ZeroMemory(buffer2, sizeof(buffer2));
	ZeroMemory(buffer3, sizeof(buffer3));
	bool ret = false;
	
	do{
		tmp = name[lastIndex];
		if(tmp != L'#')
			buffer1[i] = name[lastIndex];
		else break;
		i++;
		lastIndex++;
	}while(true);
	i = 0;
	lastIndex++;
	do{
		tmp = name[lastIndex];
		if(tmp != L'#')
			buffer2[i] = name[lastIndex];
		else break;
		i++;
		lastIndex++;
	}while(true);
	i = 0;
	lastIndex++;
	do{
		tmp = name[lastIndex];
		if(tmp != L'#')
			buffer3[i] = name[lastIndex];
		else break;
		i++;
		lastIndex++;
	}while(true);

	HKEY   hkey;
	DWORD  dwDisposition;
	LONG res;
	DWORD dwType, dwSize, dwData;
	TCHAR keypath[256], value[32];
	wsprintf(keypath, L"SYSTEM\\CurrentControlSet\\Enum\\%s\\%s\\%s", buffer1, buffer2, buffer3);
	dwSize = sizeof(value);
	dwType = REG_SZ;
	//RegGetValue(HKEY_LOCAL_MACHINE, keypath, TEXT("Class"), RRF_RT_REG_SZ, NULL, value, &dwSize);
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, keypath, 0, KEY_READ, &hkey);
	
	RegQueryValueEx(hkey, TEXT("Class"), NULL, &dwType, (LPBYTE) value, &dwSize); 

	if(_tcscmp(value, TEXT("DiskDrive"))==0){
		ret = true;
	}
	if(_tcscmp(buffer1, TEXT("USB"))==0){
		isUsb = true;
	}
	else{
		isUsb = false;
	}
	return ret;
}