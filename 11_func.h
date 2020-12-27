//
// Created by Viktor on 15.10.2020.
//
#ifndef LAB1_1_FUNC_H
#define LAB1_1_FUNC_H
#include <Windows.h>
#include <iostream>

using namespace std;



void GetDriveList()
{
    wchar_t disks[256];
    wchar_t *disk;
    DWORD sizebuf=256;
    GetLogicalDriveStringsW(sizebuf, disks);
    disk=disks;
    while (*disk)
    {
        wprintf(L"%s\n", disk);
        disk=disk+wcslen(disk)+1;
    }
}




void GetDiskInfo(char* path)
{
    int d;
    d = GetDriveType(LPCSTR(path));
    if( d == DRIVE_UNKNOWN ) cout << "UNKNOWN" << endl;
    if( d == DRIVE_NO_ROOT_DIR ) cout << "DRIVE NO ROOT DIR" << endl;
    if( d == DRIVE_REMOVABLE ) cout << "REMOVABLE" << endl;
    if( d == DRIVE_FIXED ) cout << "FIXED" << endl;
    if( d == DRIVE_REMOTE ) cout << "REMOTE" << endl;
    if( d == DRIVE_CDROM ) cout << "CDROM" << endl;
    if( d == DRIVE_RAMDISK ) cout << "RAMDISK" << endl;

    char Name[MAX_PATH];
    char NameFileSystem[MAX_PATH];

    DWORD SerialNumber;
    DWORD TypeFileSystem;
    DWORD SizeTom;

    DWORD SectorPerCluster;
    DWORD BytePerSector;
    DWORD FreeClusters;
    DWORD TotalClusters;


    GetVolumeInformationA(
            LPCSTR(path),
            reinterpret_cast<LPSTR>(LPWSTR(Name)),
            sizeof(Name),
            &SerialNumber,
            &SizeTom,
            &TypeFileSystem,
            reinterpret_cast<LPSTR>(LPWSTR(NameFileSystem)),
            sizeof(NameFileSystem)
            );

    cout<<SizeTom<<endl;
    //cout<<TypeFileSystem<<endl;

    if ((0x00000002 & TypeFileSystem) == 0x00000002)
    {
        cout<<"FILE_CASE_PRESERVED_NAMES"<<endl;
    }
    if ((0x00000001 & TypeFileSystem) == 0x00000001)
    {
        cout<<"FILE_CASE_SENSITIVE_SEARCH"<<endl;

    }
    if ((0x20000000 & TypeFileSystem) == 0x20000000)
    {
        cout<<"FILE_DAX_VOLUME"<<endl;

    }
    if ((0x00000010 & TypeFileSystem) == 0x00000010)
    {
        cout<<"FILE_FILE_COMPRESSION"<<endl;

    }
    if ((0x00040000 & TypeFileSystem) == 0x00040000)
    {
        cout<<"FILE_NAMED_STREAMS"<<endl;

    }
    if ((0x00000008 & TypeFileSystem) == 0x00000008)
    {
        cout<<"FILE_PERSISTENT_ACLS"<<endl;

    }
    if ((0x00080000 & TypeFileSystem) == 0x00080000)
    {
        cout<<"FILE_READ_ONLY_VOLUME"<<endl;

    }
    if ((0x00100000 & TypeFileSystem) == 0x00100000)
    {
        cout<<"FILE_SEQUENTIAL_WRITE_ONCE"<<endl;

    }
    if ((0x00020000 & TypeFileSystem) == 0x00020000)
    {
        cout<<"FILE_SUPPORTS_ENCRYPTION"<<endl;

    }
    if ((0x00800000 & TypeFileSystem) == 0x00800000)
    {
        cout<<"FILE_SUPPORTS_EXTENDED_ATTRIBUTES"<<endl;

    }
    if ((0x00400000 & TypeFileSystem) == 0x00400000)
    {
        cout<<"FILE_SUPPORTS_HARD_LINKS"<<endl;

    }
    if ((0x00010000 & TypeFileSystem) == 0x00010000)
    {
        cout<<"FILE_SUPPORTS_OBJECT_IDS"<<endl;

    }
    if ((0x01000000 & TypeFileSystem) == 0x01000000)
    {
        cout<<"FILE_SUPPORTS_OPEN_BY_FILE_ID"<<endl;

    }
    if ((0x00000080 & TypeFileSystem) == 0x00000080)
    {
        cout<<"FILE_SUPPORTS_REPARSE_POINTS"<<endl;

    }
    if ((0x00000040 & TypeFileSystem) == 0x00000040)
    {
        cout<<"FILE_SUPPORTS_SPARSE_FILES"<<endl;

    }
    if ((0x00200000 & TypeFileSystem) == 0x00200000)
    {
        cout<<"FILE_SUPPORTS_TRANSACTIONS"<<endl;

    }
    if ((0x02000000 & TypeFileSystem) == 0x02000000)
    {
        cout<<"FILE_SUPPORTS_USN_JOURNAL"<<endl;

    }
    if ((0x00000004 & TypeFileSystem) == 0x00000004)
    {
        cout<<"FILE_UNICODE_ON_DISK"<<endl;

    }
    if ((0x00008000 & TypeFileSystem) == 0x00008000)
    {
        cout<<"FILE_VOLUME_IS_COMPRESSED"<<endl;

    }
    if ((0x00000020 & TypeFileSystem) == 0x00000020)
    {
        cout<<"FILE_VOLUME_QUOTAS"<<endl;

    }
    if ((0x08000000 & TypeFileSystem) == 0x08000000)
    {
        cout<<"FILE_SUPPORTS_BLOCK_REFCOUNTING"<<endl;

    }





    std::cout << Name << std::endl;
    std::cout << NameFileSystem << std::endl;
    std::cout << SerialNumber << std::endl;

    GetDiskFreeSpaceA(
            LPCSTR(path),
            &SectorPerCluster,
            &BytePerSector,
            &FreeClusters,
            &TotalClusters
            );

    cout << "Sectors per cluster: " << SectorPerCluster <<endl;
    cout << "Bytes per sector: " << BytePerSector <<endl;
    cout << "Free clusters: " << FreeClusters <<endl;
    cout << "Total clusters: " << TotalClusters <<endl;
    DWORD kBpC = SectorPerCluster*BytePerSector/1024;
    double TotalSpace = TotalClusters*kBpC/1024/1024;
    double FreeSpace = FreeClusters*kBpC/1024/1024;


    cout<< "Space: " << FreeSpace <<"GB / " << TotalSpace << "GB" <<endl;

}

void CreateDir(char* path)
{
    CreateDirectoryA(
            LPCSTR(path),
           nullptr
            );

}

void RemoveDir(char* path)
{
    RemoveDirectoryA(
            LPCSTR(path)
            );
}

void Create(char* path)
{
    HANDLE A;
    A = CreateFileA(
            LPCSTR(path),
            (GENERIC_READ | GENERIC_WRITE),
            0,
            nullptr,
            1,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
            );
    CloseHandle(A);
}

void Copy(
        char* expath,
        char* newpath
        )
{
    if (!(CopyFileA(
            LPCSTR(expath),
            LPCSTR(newpath),
            true
    )))
        cout<<"File already exists" <<endl;

}


void Move(
        char* expath,
        char* newpath
        )
{
    MoveFileA(
            LPCSTR(expath),
            LPCSTR(newpath)
            );
}

void GetAtr(char* path)
{
    DWORD Att;
    Att = GetFileAttributesA(
            LPCSTR(path)
            );
    cout<<Att<<endl;
}
void SetAtr(char* path,DWORD new_att)
{
    DWORD Att;
    SetFileAttributesA(
            LPCSTR(path),
            new_att
            );
}

void GetByHandle(char* path)
{
    HANDLE A;

    A = CreateFileA(
            LPCSTR(path),
            (GENERIC_READ | GENERIC_WRITE),
            0,
            nullptr,
            3,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );
    BY_HANDLE_FILE_INFORMATION l;
    GetFileInformationByHandle(
            A,
            &l
            );

    LARGE_INTEGER FileSize, FileIndex;
    SYSTEMTIME ct_sys,la_sys,lw_sys;
    FileTimeToSystemTime(&l.ftCreationTime,&ct_sys);
    FileTimeToSystemTime(&l.ftLastAccessTime,&la_sys);
    FileTimeToSystemTime(&l.ftLastWriteTime,&lw_sys);



    FileSize.LowPart = l.nFileSizeLow;
    FileSize.HighPart = l.nFileSizeHigh;
    FileIndex.LowPart =l.nFileIndexLow;
    FileIndex.HighPart=l.nFileIndexHigh;

    cout<<"File attributes: "<<l.dwFileAttributes<<endl;
    cout<<"Volume serial: "<<l.dwVolumeSerialNumber<<endl;
    cout<<"FileSize: "<<FileSize.QuadPart<<endl;
    cout<<"Number of links: "<<l.nNumberOfLinks<<endl;
    cout<<"FileIndex: "<<FileIndex.QuadPart<<endl;
    cout<<"Creation time: "<<ct_sys.wYear<<"."<<ct_sys.wMonth<<"."<<ct_sys.wDay<<" "<<ct_sys.wHour<<":"<<ct_sys.wMinute<<endl;
    cout<<"Last Access time: "<<la_sys.wYear<<"."<<la_sys.wMonth<<"."<<la_sys.wDay<<" "<<la_sys.wHour<<":"<<la_sys.wMinute<<endl;
    cout<<"Last Write time: "<<lw_sys.wYear<<"."<<lw_sys.wMonth<<"."<<lw_sys.wDay<<" "<<lw_sys.wHour<<":"<<lw_sys.wMinute<<endl;


    CloseHandle(A);
}

void GetTime(char* path)
{
    HANDLE A;
    A = CreateFileA(
            LPCSTR(path),
            (GENERIC_READ | GENERIC_WRITE),
            0,
            nullptr,
            3,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );
    FILETIME CT,LA,LW;
    GetFileTime(A,&CT,&LA,&LW);
    SYSTEMTIME ct_sys,la_sys,lw_sys;
    FileTimeToSystemTime(&CT,&ct_sys);
    FileTimeToSystemTime(&LA,&la_sys);
    FileTimeToSystemTime(&LW,&lw_sys);

    cout<<"Creation time: "<<ct_sys.wYear<<"."<<ct_sys.wMonth<<"."<<ct_sys.wDay<<" "<<ct_sys.wHour<<":"<<ct_sys.wMinute<<endl;
    cout<<"Last Access time: "<<la_sys.wYear<<"."<<la_sys.wMonth<<"."<<la_sys.wDay<<" "<<la_sys.wHour<<":"<<la_sys.wMinute<<endl;
    cout<<"Last Write time: "<<lw_sys.wYear<<"."<<lw_sys.wMonth<<"."<<lw_sys.wDay<<" "<<lw_sys.wHour<<":"<<lw_sys.wMinute<<endl;





    CloseHandle(A);
}



#endif //LAB1_1_FUNC_H
