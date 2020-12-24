#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void WINAPI readCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {

}


constexpr auto buffSize = 512;

int main()
{
    HANDLE hPipe = { };
    LPTSTR PipeName = TEXT("\\\\.\\pipe\\MyPipe");
    BOOL pipeConnected = FALSE;
    int menu = -1;


    system("cls");



    //Открытие именованного канала
    hPipe = CreateFileA(PipeName,
                        GENERIC_READ,
                        0,
                        nullptr,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        nullptr);


    if (hPipe != nullptr && hPipe != INVALID_HANDLE_VALUE)
    {
        cout << "Connected to named pipe" << endl;
        pipeConnected = TRUE;
    }
    else
    {
        cout << "Named pipe connection error: " << GetLastError() << endl;
        system("pause");
        return 0;
    }

    //Цикл обмена сообщениями (получение)
    char msg[buffSize] = { };
    OVERLAPPED overlapped = { };
    while (1)
    {
        if (ReadFileEx(hPipe, &msg, buffSize, &overlapped, readCompletionRoutine))
        {
            cout << "Waiting for message... ";
            SleepEx(INFINITE, true); //Выход по тревожному вводу/выводу
            cout << "Got message!" << endl << msg << endl;

        }
        else
        {
            cout << "Read failure: " << GetLastError() << endl;
            system("pause");
            return 0;
        }
        if (msg[0] == '^') break;//Выход по условному символу
        for (int k =0;k<strlen(msg);k++) msg[k] = ' ';// Очистка строки
    }
    //Закрытие хендла
    if (CloseHandle(hPipe))
    {
        cout << "Named pipe disconnected" << endl;
        hPipe = nullptr;
    }
    else
    {
        cout << "Disconnection failure: " << GetLastError() << endl;
        system("pause");
        return 0;
    }



    return 0;
}
