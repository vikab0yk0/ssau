#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <string>

using namespace std;

struct ClientInfo {
    HANDLE pipe;
    int id;
    bool isConnected;
};

vector<ClientInfo> clients;
int nextClientId = 1;
CRITICAL_SECTION cs;

void HandleClient(ClientInfo client) {
    char buffer[256];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(client.pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!success || bytesRead == 0) {
            EnterCriticalSection(&cs);
            client.isConnected = false;
            cout << "Client ID " << client.id << " disconnected.\n";
            LeaveCriticalSection(&cs);
            break;
        }

        buffer[bytesRead] = '\0';

        EnterCriticalSection(&cs);
        cout << "Message from client ID " << client.id << ": " << buffer << endl;
        LeaveCriticalSection(&cs);
    }

    DisconnectNamedPipe(client.pipe);
    CloseHandle(client.pipe);
}

int main() {
    InitializeCriticalSection(&cs);

    cout << "Server started. Waiting for clients...\n";

    while (true) {
        string pipeName = R"(\\.\pipe\MyPipe)";
        HANDLE hPipe = CreateNamedPipeA(
            pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            512, 512,
            0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            cerr << "CreateNamedPipe failed. Error: " << GetLastError() << endl;
            continue;
        }

        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (connected) {
            ClientInfo client;
            client.pipe = hPipe;
            client.id = nextClientId++;
            client.isConnected = true;

            EnterCriticalSection(&cs);
            clients.push_back(client);
            cout << "New client connected. ID: " << client.id << endl;
            LeaveCriticalSection(&cs);

            // Отправка ID клиенту
            string idMsg = "Your ID is: " + to_string(client.id);
            DWORD bytesWritten;
            WriteFile(hPipe, idMsg.c_str(), idMsg.length() + 1, &bytesWritten, NULL);

            // Поток обработки клиента
            thread(HandleClient, client).detach();
        }
        else {
            CloseHandle(hPipe);
        }

        Sleep(100);
    }

    DeleteCriticalSection(&cs);
    return 0;
}
