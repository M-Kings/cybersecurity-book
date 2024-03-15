
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <intrin.h>


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


void EvadeVM() {
	
	DWORD RAMMB;
	
	HANDLE hDevice;
	DISK_GEOMETRY pDiskGeometry;
	DWORD bytesReturned;
	DWORD diskSizeGB;
	hDevice = CreateFileA("\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); 
	DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &pDiskGeometry, sizeof(pDiskGeometry), &bytesReturned, (LPOVERLAPPED)NULL);
	diskSizeGB = pDiskGeometry.Cylinders.QuadPart 
        * (ULONG)pDiskGeometry.TracksPerCylinder 
        * (ULONG)pDiskGeometry.SectorsPerTrack 
        * (ULONG)pDiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
	if (diskSizeGB < 50)
        exit(-1);

//check RAM size
    MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(memoryStatus);
	GlobalMemoryStatusEx(&memoryStatus);
	RAMMB = memoryStatus.ullTotalPhys / 1024 / 1024;
	if (RAMMB < 4000)
        exit(-1);

//check CPU	number
    SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	if (systemInfo.dwNumberOfProcessors < 4)
        exit(-1);

	return;
}

bool GetHDDVendorId(std::string& outVendorId) {
    HANDLE hDevice = CreateFileA(("\\\\.\\PhysicalDrive0"),0,FILE_SHARE_READ | FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
    if (hDevice == INVALID_HANDLE_VALUE) return false;
    STORAGE_PROPERTY_QUERY storage_property_query = {};
    storage_property_query.PropertyId = StorageDeviceProperty;
    storage_property_query.QueryType = PropertyStandardQuery;
    STORAGE_DESCRIPTOR_HEADER storage_descriptor_header = {};
    DWORD BytesReturned = 0;
    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
        &storage_property_query, sizeof(storage_property_query),
        &storage_descriptor_header, sizeof(storage_descriptor_header),
        &BytesReturned,0 )) {
        printf("DeviceIoControl() for size query failed\n");
        CloseHandle(hDevice);
        return false;
    }
    if (!BytesReturned) {
        CloseHandle(hDevice);
        return false;
    }
    std::vector<char> buff(storage_descriptor_header.Size); //_STORAGE_DEVICE_DESCRIPTOR
    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
        &storage_property_query, sizeof(storage_property_query),
        buff.data(), buff.size(), &BytesReturned, 0)) {
        CloseHandle(hDevice);
        return false;
    }
    CloseHandle(hDevice);
    if (BytesReturned) {
        STORAGE_DEVICE_DESCRIPTOR* device_descriptor = (STORAGE_DEVICE_DESCRIPTOR*)buff.data();
        if (device_descriptor->VendorIdOffset)
            outVendorId = &buff[device_descriptor->VendorIdOffset];

        return true;
    }
    return false;
}


static inline UINT64 rdtsc_diff_vmexit() {

    UINT64 ret, ret2;
    int CPUInfo[4] = { 0 };
    ret = __rdtsc();
    __cpuid(CPUInfo, 0);
    ret2 = __rdtsc();
    return ret2 - ret;
}
int cpu_rdtsc_force_vmexit() {
    int i;
    UINT64 avg = 0;
    for (i = 0; i < 10; i++) {
        avg = avg + rdtsc_diff_vmexit();
        Sleep(500);
    }
    avg = avg / 10;
    printf("avg = %d", avg);
    return (avg < 1000 && avg > 0) ? FALSE : TRUE;
}
int __cdecl main()
{
    if (cpu_rdtsc_force_vmexit())
        exit(0);
}




//std::string vendorId;
//if (GetHDDVendorId(vendorId)) {
//    std::cout << "Hard Drive Vendor ID: " << vendorId << std::endl;
//}
//else {
//    std::cerr << "Failed to get Hard Drive Vendor ID" << std::endl;
//}
//return 0;