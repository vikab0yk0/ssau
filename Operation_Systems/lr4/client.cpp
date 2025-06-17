#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>

using namespace std;

int main() {
    string pipeName = R"(\\.\pipe\MyPipe)";
    HANDLE hPipe;

    while (true) {
        hPipe = CreateFileA(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, NULL);

        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY) {
            cerr << "Could not open pipe. Error: " << GetLastError() << endl;
            return 1;
        }

        if (!WaitNamedPipeA(pipeName.c_str(), 5000)) {
            cerr << "Pipe is busy. Timeout.\n";
            return 1;
        }
    }

    cout << "Connected to server.\n";

    char buffer[256];
    DWORD bytesRead;

    // Чтение ID от сервера
    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        cout << "Server: " << buffer << endl;
    }

    // Отправка подтверждения подключения
    string helloMsg = "Client connected successfully";
    DWORD bytesWritten;
    WriteFile(hPipe, helloMsg.c_str(), helloMsg.length() + 1, &bytesWritten, NULL);

    while (true) {
        cout << "Enter message (or 'exit' to quit): ";
        string msg;
        getline(cin, msg);

        if (msg == "exit") break;

        WriteFile(hPipe, msg.c_str(), msg.length() + 1, &bytesWritten, NULL);
    }

    CloseHandle(hPipe);
    cout << "Disconnected from server.\n";
    return 0;
}
