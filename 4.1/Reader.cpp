#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include "ctime"


using namespace std;

#define pageCount 17
#define pageSize 4096




int main(int argc, char* argv[])
{
    int id = 0;
    if (argc == 2)
        id = atoi(argv[1]);

    //Логгер
    ofstream log;
    log.open("C:\\Users\\Viktor\\CLionProjects\\os\\part4\\4.1\\MainApp\\cmake-build-debug-visual-studio-x64\\exe\\readerLog.txt", fstream::out | fstream::app);


    //Мьютекс для логгера чтения
    HANDLE logReadMutex = OpenMutexA(MUTEX_MODIFY_STATE | SYNCHRONIZE,
                                     false,
                                     "logReadMutex");

    //Открытие семафоров
    HANDLE writeSemaphore[pageCount];
    HANDLE readSemaphore[pageCount];
    for (int i = 0; i < pageCount; i++) {
        writeSemaphore[i] = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE,
                                           false,
                                           ("writeSemaphore_" + to_string(i)).c_str());
        if (writeSemaphore[i] == nullptr) {
            WaitForSingleObject(logReadMutex, INFINITE);
            cout << "Open write semaphore error, id " << id << ": " << GetLastError() << endl;
            ReleaseMutex(logReadMutex);
            system("pause");
            return 0;
        }



        readSemaphore[i] = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE,
                                          false,
                                          ("readSemaphore_" + to_string(i)).c_str());
        if (readSemaphore[i] == nullptr) {
            WaitForSingleObject(logReadMutex, INFINITE);
            cout << "Open read semaphore error, id " << id << ": " << GetLastError() << endl;
            ReleaseMutex(logReadMutex);
            system("pause");
            return 0;
        }
    }
    //открытие проецируемого файла на чтение
    HANDLE fileMapping = OpenFileMappingA(GENERIC_READ,
                                          false,
                                          "fileMapping");
    if (fileMapping == nullptr) {
        WaitForSingleObject(logReadMutex, INFINITE);
        cout << "Open file mapping error, id " << id << ": " << GetLastError() << endl;
        ReleaseMutex(logReadMutex);
        system("pause");
        return 0;
    }
    void* p_mapViewOfFile = MapViewOfFile(fileMapping,
                                          FILE_MAP_READ,
                                          0,
                                          0,
                                          pageSize * pageCount);
    if (p_mapViewOfFile == nullptr) {
        WaitForSingleObject(logReadMutex, INFINITE);
        cout << "Open map view of file error, id " << id << ": " << GetLastError() << endl;
        ReleaseMutex(logReadMutex);
        system("pause");
        return 0;
    }

    time_t rawtime;

    char data[pageSize];
    //while (1)
    for (int k = 0;k<40;k++)
    {
        //ожидание свободного семафора (страницы) на чтение

        DWORD currentPage = WaitForMultipleObjects(pageCount, readSemaphore, false, INFINITE) - WAIT_OBJECT_0;

        WaitForSingleObject(logReadMutex, INFINITE);
        time ( &rawtime );
        log << "Time " << ctime (&rawtime) << "| Reader  [" << id << "] Got access to page = " << currentPage << endl;
        //log << rawtime << "\tR\t" << id << "\tREAD\t" << currentPage << endl;

        ReleaseMutex(logReadMutex);
        //чтение данных из проецируемого файла
        memcpy(data, (void*)((char*)p_mapViewOfFile + (currentPage * pageSize)), pageSize);

        Sleep(500 + (rand() % 1000));

        WaitForSingleObject(logReadMutex, INFINITE);
        time ( &rawtime );

        log << "Time " << ctime (&rawtime) << "| Reader  [" << id << "] Lost access to page = " << currentPage <<"| Data = " << data << endl;
        //log << rawtime << "\tR\t" << id << "\tWAIT\t" << currentPage << endl;

        ReleaseMutex(logReadMutex);

        //освобождение семаформа на запись для текущей страницы
        ReleaseSemaphore(writeSemaphore[currentPage], 1, nullptr);
    }




    return 1;
}