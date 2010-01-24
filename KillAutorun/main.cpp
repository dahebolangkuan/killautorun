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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	MSG        Msg;
	HWND       hWnd;
	WNDCLASS	wndclass;

	int tmp = 0;
	
	DisableAutorun();

	CreateMutexEx(NULL, L"KillAutorunMutex", CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);

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
			if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) 
          { 
            PDEV_BROADCAST_VOLUME pVol = (PDEV_BROADCAST_VOLUME) pHdr; 
            char cDriveLetter = GetDriveLetter(pVol->dbcv_unitmask); 
			CleanAutorun(cDriveLetter);
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

	wsprintfA(szMessage, "Device '%c:' has been inserted.", DriveLetter); 
	MessageBoxA(NULL, szMessage, "USB Notice", MB_OK); 
}

/// <summary>
/// Set registry value to disable autorun
/// </summary>
void DisableAutorun(){
	HKEY   hkey;
	DWORD  dwDisposition;
	LONG res;
	DWORD dwType, dwSize, dwData;
 
	res = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, &dwDisposition);
	if(res == ERROR_SUCCESS)
	{
		dwType = REG_DWORD;
		dwSize = sizeof(DWORD);
		dwData = 0x7c;
		RegSetValueEx(hkey, TEXT("NoDriveTypeAutoRun"), 0, dwType, (PBYTE)&dwData, dwSize);
	 
		RegCloseKey(hkey);
	}
}