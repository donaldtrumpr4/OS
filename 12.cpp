#include <windows.h>
#include <iostream>
#include "ctime"
using namespace std;

#define MAX_OVRLP 16
#define REC_SIZE 4096

static VOID WINAPI ReadDone(DWORD, DWORD, LPOVERLAPPED);
static VOID WINAPI WriteDone(DWORD, DWORD, LPOVERLAPPED);

OVERLAPPED OverLapOut[MAX_OVRLP], OverLapIn[MAX_OVRLP];
CHAR Rec[MAX_OVRLP][REC_SIZE];
HANDLE hInputFile, hOutputFile;
LONGLONG nRecord, nDone;
LARGE_INTEGER FileSize;

static VOID WINAPI ReadDone(DWORD Code, DWORD nBytes, LPOVERLAPPED pOv)
{
    LARGE_INTEGER CurPosIn, CurPosOut;
    DWORD ic, i;
    nDone++;

    ic = (DWORD)(pOv->hEvent);

    CurPosIn.LowPart = OverLapIn[ic].Offset;
    CurPosIn.HighPart = OverLapIn[ic].OffsetHigh;

    CurPosOut.QuadPart = CurPosIn.QuadPart;

    OverLapOut[ic].Offset = CurPosOut.LowPart;
    OverLapOut[ic].OffsetHigh = CurPosOut.HighPart;

    WriteFileEx(hOutputFile, Rec[ic], REC_SIZE, &OverLapOut[ic], WriteDone); //вызов функции записи

    CurPosIn.QuadPart += REC_SIZE * (LONGLONG)(MAX_OVRLP);

    cout<<CurPosIn.QuadPart<<" /"<<FileSize.QuadPart<<" - "<<nDone<<" Read (ic="<<ic<<")"<<endl;

    OverLapIn[ic].Offset = CurPosIn.LowPart;
    OverLapIn[ic].OffsetHigh = CurPosIn.HighPart; //смещение для следующего чтения
}
static VOID WINAPI WriteDone(DWORD Code, DWORD nBytes, LPOVERLAPPED pOv)
{
    LARGE_INTEGER CurPosIn;
    DWORD ic;
    nDone++;

    ic = (DWORD) (pOv->hEvent);

    CurPosIn.LowPart = OverLapIn[ic].Offset;
    CurPosIn.HighPart = OverLapIn[ic].OffsetHigh;

    cout<<CurPosIn.QuadPart<<" /"<<FileSize.QuadPart<<" - "<<nDone<<" Write (ic="<<ic<<")"<<endl;


    if (CurPosIn.QuadPart < FileSize.QuadPart)
        ReadFileEx(hInputFile, Rec[ic], REC_SIZE, &OverLapIn[ic], ReadDone); //вызов следующей функции чтения
}

int main()
{
    //char* key = new char();
    //char* key2 = new char();
    DWORD ic;
    LARGE_INTEGER CurPosIn;
    long double time_start,time;
    time_start = clock();


    hInputFile = CreateFile(
            LPCSTR("C:\\lab_test\\2.txt"),
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            (FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED),
            NULL
            );
    hOutputFile = CreateFile(
            LPCSTR("C:\\lab_test\\888.txt"),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            (FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED),
            NULL
            );
    //открытие файлов


    FileSize.LowPart = GetFileSize(hInputFile, reinterpret_cast<LPDWORD>(&FileSize.HighPart));
    nRecord = FileSize.QuadPart / REC_SIZE;
    if ((FileSize.QuadPart % REC_SIZE) != 0) nRecord++; //Подсчет количества операций ввода-вывода


    CurPosIn.QuadPart = 0;
    for (ic = 0; ic < MAX_OVRLP; ic++)
    {
        OverLapIn[ic].hEvent = (HANDLE) (ic);
        OverLapOut[ic].hEvent = (HANDLE) (ic); //передача индекса overlapped ввода-вывода через поле события hEvent


        OverLapIn[ic].Offset = CurPosIn.LowPart;
        OverLapIn[ic].OffsetHigh = CurPosIn.HighPart; //


        if (CurPosIn.QuadPart < FileSize.QuadPart)
        {
            ReadFileEx(hInputFile, Rec[ic], REC_SIZE, &OverLapIn[ic], ReadDone);
            cout << "FIRST READ (ic=" << ic << ")" << OverLapIn[ic].Offset << endl;
        } //первая операция чтения для всех потоков

        CurPosIn.QuadPart += (LONGLONG) REC_SIZE; //установка начального смещения
    }

    nDone = 0;
    while (nDone < 2 * nRecord) SleepEx(INFINITE, TRUE); //ожидание завершения всех операций


    SetFilePointerEx(hOutputFile, FileSize, nullptr, FILE_BEGIN);
    SetEndOfFile(hOutputFile);//установка фактического конца файла


    CloseHandle(hInputFile);
    CloseHandle(hOutputFile);
    time = (clock() - time_start)/1000;
    long double speed = FileSize.QuadPart/time/1024;
    cout<<"Time taken: "<<time<<" s"<<endl;
    cout<<"Speed: "<<speed<<" kB/s"<<endl;

    return 0;
}
