#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <process.h> 
#include "resource.h"
#include "banned.h"
#include "main.h"
#include "AsyncDelete.h"

bool DeleteAutorunAsync(char DriveLetter){
	char *pDrive = new char;
	*pDrive = DriveLetter;
	_beginthread( DeleteProc, 0, pDrive );

	return true;
}

void DeleteProc(void *pArg){
	char *pDrive = 0;
	pDrive = (char *) pArg;
	char file[256];
	BOOL status = TRUE;
	DWORD lastError = 0;
	wsprintfA(file, "%c:\\autorun.inf", *pDrive);
	
	SetFileAttributesA(file, FILE_ATTRIBUTE_NORMAL);
	status = DeleteFileA(file);	//delete autorun script if exist
	if(status == 0) {
		int maxi = 100;
		if(*pDrive == 'A' || *pDrive == 'B') maxi = 5;
		for(int i = 0; i<maxi; i++){
			Sleep(1000);
			SetFileAttributesA(file, FILE_ATTRIBUTE_NORMAL);
			status = DeleteFileA(file);	//delete autorun script if exist
			if(status) {
				break;
			}
		}
	}

	delete pDrive;
}
