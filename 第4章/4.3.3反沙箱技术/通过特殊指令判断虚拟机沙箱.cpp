#include <Windows.h>
#include <Wincrypt.h>
#include<stdio.h>
#include<stdlib.h>
#include <conio.h>
#pragma warning (disable: 4996)


int gensandbox_mouse_act() {
	POINT position1, position2;

	GetCursorPos(&position1);
	Sleep(20000);
	GetCursorPos(&position2);

	if ((position1.x == position2.x) && (position1.y == position2.y))
		return TRUE;
	else
		return FALSE;
}




int DetectMuiCacheRegKey()
{
	HKEY hKey;
	LONG lRet;
	DWORD dwIndex = 0, dwSize = MAX_PATH;
	CHAR szName[MAX_PATH];
	BYTE* pData = NULL;
	DWORD dwDataSize = 0;
	HMODULE Advapi = NULL;
	Advapi = LoadLibraryA("Advapi32.dll");
	lRet = RegOpenKeyExA(HKEY_CURRENT_USER, ("Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"), 0, KEY_READ, &hKey);
	if (lRet != ERROR_SUCCESS) {
		return 0;
	}
	while (TRUE) {
		dwSize = MAX_PATH;
		szName[0] = '\0';
		lRet = RegEnumValueA(hKey, dwIndex, szName, &dwSize, NULL, NULL, NULL, NULL);
		if (lRet == ERROR_SUCCESS) {
			++dwIndex;
		}
		else {
			if (lRet == ERROR_NO_MORE_ITEMS) {
				break;
			}
			else {
				printf("Failed to enumerate registry values, error code: %d\n", lRet);
				break;
			}
		}
	}
	RegCloseKey(hKey);
	return dwIndex;
}

int main() {
	if (!gensandbox_mouse_act()) {
		MessageBoxA(0, "successfully", 0, 0);
	}
	else
	{
		MessageBoxA(0, "error", 0, 0);
	}


	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	if (systemInfo.dwNumberOfProcessors < 4)
		exit(0);


	MEMORYSTATUSEX memoryStatus;
	DWORD RAMMB;
	GlobalMemoryStatusEx(&memoryStatus);
	RAMMB = memoryStatus.ullTotalPhys / 1024 / 1024;
	if (RAMMB < 4000)
		exit(0);


	HANDLE hDevice;
	DISK_GEOMETRY pDiskGeometry;
	DWORD bytesReturned;
	DWORD diskSizeGB;
	hDevice = CreateFileA("\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); 
	DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &pDiskGeometry, sizeof(pDiskGeometry), &bytesReturned, (LPOVERLAPPED)NULL);
	diskSizeGB = pDiskGeometry.Cylinders.QuadPart * (ULONG)pDiskGeometry.TracksPerCylinder * 
		(ULONG)pDiskGeometry.SectorsPerTrack * (ULONG)pDiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
	if (diskSizeGB < 50)
		exit(0);

}