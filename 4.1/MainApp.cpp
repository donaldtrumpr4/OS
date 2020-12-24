#include "windows.h"
#include "iostream"
#include "string"
#include "fstream"

using namespace std;


#define pageCount 17
#define pageSize 4096
#define writeProcessCount 5
#define readProcessCount 5

int main()
{
    LPCSTR fileName = ".\\exe\\file.txt";
    LPCSTR exeWrite = "C:\\Users\\Viktor\\CLionProjects\\os\\part4\\4.1\\Writer\\cmake-build-release-visual-studio\\Writer.exe";
    LPCSTR exeRead = "C:\\Users\\Viktor\\CLionProjects\\os\\part4\\4.1\\Reader\\cmake-build-release-visual-studio\\Reader.exe";
    DWORD total = pageCount * pageSize;

    //Создание хендла файла для записи
    cout << "Creating file.........";
    HANDLE file = CreateFileA(nullptr,
                              GENERIC_ALL,
                              0,
                              nullptr,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (!(file))
    {
        cout << "Error: " << GetLastError() << endl;
        return 0;
    }
    cout<<"OK"<<endl;


    //Создание и очистка файлов логгеров
    ofstream log;
    log.open(".\\exe\\writerLog.txt", fstream::out | fstream::trunc);
    log.close();
    log.open(".\\exe\\readerLog.txt", fstream::out | fstream::trunc);
    log.close();






    //Проецирование файла для записи в память
    cout << "Creating file mapping.........";

    HANDLE fileMapping = CreateFileMappingA(file,
                                            nullptr,
                                            PAGE_READWRITE,
                                            0,
                                            pageSize * pageCount,
                                            "fileMapping");
    if (!(fileMapping))
    {
        cout << "Error: " << GetLastError() << endl;
        return 0;
    }
    cout<<"OK"<<endl;

    cout << "Creating map view of file.........";
    void* p_mapViewOfFile = MapViewOfFile(fileMapping,
                                          FILE_MAP_READ,
                                          0,
                                          0,
                                          pageSize * pageCount);
    if (!(p_mapViewOfFile))
    {
        cout << "Error: " << GetLastError() << endl;
        return 0;
    }
    cout<<"OK"<<endl;



    //Блокирование памяти
    cout << "Locking pages.........";

    if (!(VirtualLock(p_mapViewOfFile,pageSize * pageCount)))
    {
        cout << "Error" << GetLastError() << endl;
        return 0;
    }
    cout << "OK" << endl;


    //Создание семафоров
    cout << "Creating read and write semaphores.........";

    HANDLE writeSemaphore[pageCount];
    HANDLE readSemaphore[pageCount];
    for (int i = 0; i < pageCount; i++)
    {
        writeSemaphore[i] = CreateSemaphoreA(nullptr,
                                             1,
                                             1,
                                             ("writeSemaphore_" + to_string(i)).c_str());
        if (writeSemaphore[i] == nullptr) {
            cout << "Create write semaphore error: " << GetLastError() << endl;
            return 0;
        }
        readSemaphore[i] = CreateSemaphoreA(nullptr,
                                            0,
                                            1,
                                            ("readSemaphore_" + to_string(i)).c_str());
        if (readSemaphore[i] == nullptr) {
            cout << "Create read semaphore error: " << GetLastError() << endl;
            return 0;
        }
    }
    cout << "OK" << endl;



    //Создание мьютексов для логгеров
    cout << "Creating log mutexes.........";

    HANDLE logWriteMutex = CreateMutexA(nullptr,
                                        false,
                                        "logWriteMutex");
    if (!(logWriteMutex))
    {
        cout << "Error: " << GetLastError() << endl;
        return 0;
    }
    HANDLE logReadMutex = CreateMutexA(nullptr,
                                       false,
                                       "logReadMutex");
    if (!(logReadMutex))
    {
        cout << "Error: " << GetLastError() << endl;
        return 0;
    }
    cout << "OK" << endl;





    //Запуск процессов писателей

    cout<<"Starting up writers.........";

    string commandLine;
    PROCESS_INFORMATION writeProcessInformation[writeProcessCount] = { };
    for (int i = 0; i < writeProcessCount; i++)
    {
        STARTUPINFOA writeStartupInfo = { };
        writeStartupInfo.cb = sizeof(writeStartupInfo);
        //Передача в консоль параметра, хранящего id процесса
        commandLine = exeWrite + (string)" " + to_string(i);
        bool success = CreateProcessA(exeWrite,
                                      LPTSTR(commandLine.c_str()),
                                      nullptr,
                                      nullptr,
                                      false,
                                      0,
                                      nullptr,
                                      nullptr,
                                      &writeStartupInfo,
                                      &(writeProcessInformation[i]));
        if (!success) {
            cout << "Error: " << GetLastError() << endl;
            return 0;
        }
    }
    cout<<"OK"<<endl;



    //Запуск процессов читателей
    cout<<"Starting up readers.........";

    PROCESS_INFORMATION readProcessInformation[readProcessCount] = { };
    for (int i = 0; i < readProcessCount; i++)
    {
        STARTUPINFOA readStartupInfo = { };
        readStartupInfo.cb = sizeof(readStartupInfo);
        //Передача в консоль параметра, хранящего id процесса
        commandLine = exeRead + (string)" " + to_string(i);
        bool success = CreateProcessA(exeRead,
                                      LPTSTR(commandLine.c_str()),
                                      nullptr,
                                      nullptr,
                                      false,
                                      0,
                                      nullptr,
                                      nullptr,
                                      &readStartupInfo,
                                      &(readProcessInformation[i]));
        if (!success) {
            cout << "Error: " << GetLastError() << endl;
            return 0;
        }
    }

    cout<<"OK"<<endl;
    cout << "Press any key to terminate........." << endl;
    system("pause");

    //Закрытие хендлов
    CloseHandle(fileMapping);
    CloseHandle(file);
    CloseHandle(logReadMutex);
    CloseHandle(logWriteMutex);
    for (int i = 0; i < pageCount; i++)
    {
        CloseHandle(writeSemaphore[i]);
        CloseHandle(readSemaphore[i]);
        TerminateProcess(writeProcessInformation[i].hProcess, 0);
        TerminateProcess(readProcessInformation[i].hProcess, 0);
    }

    cout << "Program terminated" << endl;




    return 1;
}