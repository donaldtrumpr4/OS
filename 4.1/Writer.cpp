#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include "random"
#include "ctime"
//# pragma comment(lib, "winmm.lib")
using namespace std;



#define pageCount 17
#define pageSize 4096



int main(int argc, char* argv[])
{
    int id = 0;
    int logMode = 0;
    if (argc == 2)
        id = atoi(argv[1]);




    //Логгер
    ofstream log;
    log.open("C:\\Users\\Viktor\\CLionProjects\\os\\part4\\4.1\\MainApp\\cmake-build-debug-visual-studio-x64\\exe\\writerLog.txt", fstream::out | fstream::app);


    //Мьютекс для логгера записи
    HANDLE logWriteMutex = OpenMutexA(MUTEX_MODIFY_STATE | SYNCHRONIZE,
                                      false,
                                      "logWriteMutex");


    //Открытие семафоров
    HANDLE writeSemaphore[pageCount] = {};
    HANDLE readSemaphore[pageCount] = {};
    for (int i = 0; i < pageCount; i++) {
        writeSemaphore[i] = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE,
                                           false,
                                           ("writeSemaphore_" + to_string(i)).c_str());
        if (writeSemaphore[i] == nullptr) {
            WaitForSingleObject(logWriteMutex, INFINITE);
            cout << "Open write semaphore error, id " << id << ": " << GetLastError() << endl;
            ReleaseMutex(logWriteMutex);
            system("pause");
            return 0;
        }
        readSemaphore[i] = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE,
                                          false,
                                          ("readSemaphore_" + to_string(i)).c_str());
        if (readSemaphore[i] == nullptr) {
            WaitForSingleObject(logWriteMutex, INFINITE);
            cout << "Open read semaphore error, id " << id << ": " << GetLastError() << endl;
            ReleaseMutex(logWriteMutex);
            system("pause");
            return 0;
        }
    }
    //открытие проецируемого файла на запись
    HANDLE fileMapping = OpenFileMappingA(GENERIC_WRITE,
                                          false,
                                          "fileMapping");
    if (fileMapping == nullptr) {
        WaitForSingleObject(logWriteMutex, INFINITE);
        cout << "Open file mapping error, id " << id << ": " << GetLastError() << endl;
        ReleaseMutex(logWriteMutex);
        system("pause");
        return 0;
    }
    void* p_mapViewOfFile = MapViewOfFile(fileMapping,
                                          FILE_MAP_WRITE,
                                          0,
                                          0,
                                          pageSize * pageCount);
    if (p_mapViewOfFile == nullptr) {
        WaitForSingleObject(logWriteMutex, INFINITE);
        cout << "Open map view of file error, id " << id << ": " << GetLastError() << endl;
        ReleaseMutex(logWriteMutex);
        system("pause");
        return 0;
    }
    time_t rawtime;


    char data[pageSize] = "HelloWorld\0";
    //while (1)
    for (int k = 0;k<40;k++)
    {
        //ожидание свободного семафора (страницы) на запись
        DWORD currentPage = WaitForMultipleObjects(pageCount, writeSemaphore, false, INFINITE) - WAIT_OBJECT_0;

        WaitForSingleObject(logWriteMutex, INFINITE);

        time ( &rawtime );
        log << "Time " << ctime (&rawtime) << "| Writer  [" << id << "] Got access to page = " << currentPage << "| Data " << data << endl;
        //log << rawtime << "\tW\t" << id << "\tWRITE\t" << currentPage << endl;

        ReleaseMutex(logWriteMutex);
        //запись в проецируемый файл
        memcpy((void*)((char*)p_mapViewOfFile + (currentPage * pageSize)), data, pageSize);
        Sleep(500 + (rand() % 1000));

        WaitForSingleObject(logWriteMutex, INFINITE);
        time ( &rawtime );
        log << "Time " << ctime (&rawtime) << "| Writer  [" << id << "] Lost access to page = " << currentPage<<endl;
        //log << rawtime << "\tW\t" << id << "\tWAIT\t" << currentPage << endl;


        ReleaseMutex(logWriteMutex);
        //освобождение семаформа на чтение для текущей страницы
        ReleaseSemaphore(readSemaphore[currentPage], 1, nullptr);
    }
    return 0;
}
