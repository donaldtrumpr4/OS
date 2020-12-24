#include <windows.h>
#include <iostream>
#include <string>

using namespace std;



int main()
{
    HANDLE hPipe = { };
    HANDLE hEvent = { };
    LPTSTR PipeName = TEXT("\\\\.\\pipe\\MyPipe");
    BOOL pipeConnected = FALSE;


    system("cls");



    //Создание события
    hEvent = CreateEventA(
            nullptr,
            FALSE,
            FALSE,
            LPCSTR(R"(CHECK)"));
    if (hEvent)
    {
        //Создание именованного канала
        hPipe = CreateNamedPipeA(
                PipeName,
                (PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED),
                PIPE_TYPE_MESSAGE |
                PIPE_READMODE_MESSAGE |
                PIPE_WAIT,
                PIPE_UNLIMITED_INSTANCES,
                4096,
                4096,
                NMPWAIT_USE_DEFAULT_WAIT,
                nullptr);


        if (hPipe)
        {
            cout << "Named pipe has been created, connect named pipe now" << endl;
        }
        else
        {
            cout << "Named pipe creation error: " << GetLastError() << endl;
            system("pause");
            return 0;
        }
    }
    else
    {
        cout << "Event creation error: " << GetLastError() << endl;
        system("pause");
        return 0;
    }


    //Ожидание подключения
    if (hPipe)
    {
        cout << "Waiting for client connection... ";
        pipeConnected = ConnectNamedPipe(hPipe, nullptr);
        if (pipeConnected)
            cout << "Client connected!" << endl;
        else
        {
            cout << "Connection failure: " << GetLastError() << endl;
            system("pause");
            return 0;
        }

    }

    //Цикл обмена сообщениями (отправка)
    string msg;
    while (1)
    {

        cout << "Enter message: ";
        cin >> msg;
        cout << endl;

        OVERLAPPED overlapped = { };


        overlapped.hEvent = hEvent;
        WriteFile(hPipe, msg.c_str(), strlen(msg.data()), nullptr, &overlapped);
        cout << "Sending... ";
        if (msg[0] == '^') break;//Выход по условному символу
        msg.erase();//Очистка строки
        WaitForSingleObject(overlapped.hEvent, 5000);//Выход по событию
        cout << "Sent!" << endl;


    }
    //Отключение от именованного канала
    if (DisconnectNamedPipe(hPipe))
    {
        cout << "Named pipe disconnected" << endl;
        hPipe = nullptr;
    }




    //Закрытие хендлов

    if (hPipe != INVALID_HANDLE_VALUE)
        CloseHandle(hPipe);
    if (hEvent != INVALID_HANDLE_VALUE)
        CloseHandle(hEvent);

    return 0;
}