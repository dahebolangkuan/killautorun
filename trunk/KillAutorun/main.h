#define WM_USER_NIC WM_USER+1

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
char GetDriveLetter(unsigned long ulUnitMask);
void CleanAutorun(char DriveLetter);
void WorstCaseCleanAutorun();
bool IsDiskDrive(const TCHAR *name);
void DisableAutorun();