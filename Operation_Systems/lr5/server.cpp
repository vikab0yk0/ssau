#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <ctime>
#include <windows.h>
#include <string>

#define MAX_CLIENTS 10
#define MAX_ATTEMPTS 3
#define PIPE_PREFIX L"\\\\.\\pipe\\bank_client"

struct TransactionMessage {
    DWORD clientId;
    bool authorized;
    double amount;
};

std::wstring GeneratePassword() {
    std::wstring pwd;
    for (int i = 0; i < 3; ++i)
        pwd += L'0' + rand() % 10;
    return pwd;
}

std::wstring GeneratePipeName(int clientId) {
    return PIPE_PREFIX + std::to_wstring(clientId);
}

bool IsValidPassword(const std::wstring& pwd) {
    if (pwd.size() != 3) return false;
    for (wchar_t c : pwd)
        if (c < L'0' || c > L'9')
            return false;
    return true;
}

HANDLE hSemaphore;
std::mutex consoleMutex;

// Хранит пароль для каждого клиента
std::map<int, std::wstring> clientPasswords;

// Заблокированные клиенты (по ID)
std::set<int> blockedClients;

// Активные клиенты (которые уже подключены) для проверки повторных подключений
std::set<int> activeClients;

// Мьютекс для синхронизации доступа к activeClients и blockedClients
std::mutex clientsMutex;

void ClientHandler(int clientId) {
    std::wstring pipeName = GeneratePipeName(clientId);

    while (true) {
        HANDLE hPipe = CreateNamedPipeW(
            pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1, 0, 0, 0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::wcout << L"Error creating pipe for client " << clientId << L". Error code: " << GetLastError() << std::endl;
            Sleep(1000); // подождать и попробовать снова
            continue;
        }

        BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!connected) {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::wcout << L"Failed to connect client " << clientId << L". Error code: " << GetLastError() << std::endl;
            CloseHandle(hPipe);
            Sleep(1000);
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clientsMutex);

            if (blockedClients.find(clientId) != blockedClients.end()) {
                wchar_t msg = L'B'; // B = blocked
                DWORD written;
                WriteFile(hPipe, &msg, sizeof(wchar_t), &written, NULL);
                CloseHandle(hPipe);
                std::wcout << L"Blocked client " << clientId << L" tried to connect. Connection refused." << std::endl;
                continue; // Ждём новое подключение
            }

            activeClients.insert(clientId);
        }

        // Авторизация
        wchar_t startMsg = L'S'; // S = Start auth
        DWORD bytesWritten = 0;
        WriteFile(hPipe, &startMsg, sizeof(wchar_t), &bytesWritten, NULL);

        int attempts = 0;
        bool authorized = false;
        wchar_t pwdBuffer[4] = {};
        DWORD bytesRead = 0;

        while (attempts < MAX_ATTEMPTS) {
            BOOL readRes = ReadFile(hPipe, pwdBuffer, sizeof(pwdBuffer), &bytesRead, NULL);
            if (!readRes || bytesRead == 0) {
                // Клиент отключился
                break;
            }
            std::wstring enteredPwd(pwdBuffer, 3);

            if (enteredPwd == clientPasswords[clientId]) {
                authorized = true;
                break;
            }
            else {
                attempts++;
                wchar_t failMsg = L'N'; // N = wrong password
                WriteFile(hPipe, &failMsg, sizeof(wchar_t), &bytesWritten, NULL);
            }
        }

        if (!authorized) {
            // Заблокировать клиента
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                blockedClients.insert(clientId);
                activeClients.erase(clientId);
            }
            wchar_t blockMsg = L'B'; // B = blocked
            WriteFile(hPipe, &blockMsg, sizeof(wchar_t), &bytesWritten, NULL);
            std::wcout << L"Client " << clientId << L" blocked due to 3 incorrect password attempts." << std::endl;
            CloseHandle(hPipe);
            continue; // Ждём новое подключение
        }

        // Авторизация успешна
        wchar_t successMsg = L'Y'; // Y = success
        WriteFile(hPipe, &successMsg, sizeof(wchar_t), &bytesWritten, NULL);
        std::wcout << L"Client " << clientId << L" authorized." << std::endl;

        // Работа с транзакциями
        WaitForSingleObject(hSemaphore, INFINITE);

        TransactionMessage msg;
        DWORD bytes = 0;
        bool clientDisconnected = false;

        while (true) {
            BOOL readResult = ReadFile(hPipe, &msg, sizeof(msg), &bytes, NULL);
            if (!readResult || bytes == 0) {
                std::wcout << L"Client " << clientId << L" disconnected." << std::endl;
                clientDisconnected = true;
                break;
            }
            if (msg.amount < 0) {
                std::wcout << L"Client " << clientId << L" finished session." << std::endl;
                break;
            }

            std::wcout << L"Client " << msg.clientId << L" transaction: " << msg.amount << std::endl;
        }

        ReleaseSemaphore(hSemaphore, 1, NULL);
        CloseHandle(hPipe);

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            activeClients.erase(clientId);
        }

    }
}



int main() {
    srand((unsigned)time(NULL));

    int clientCount = 0;
    std::wcout << L"Enter number of clients (max " << MAX_CLIENTS << L"): ";
    std::wcin >> clientCount;
    if (clientCount > MAX_CLIENTS) clientCount = MAX_CLIENTS;

    std::wcin.ignore(); // Очистить буфер перед getline

    for (int i = 0; i < clientCount; ++i) {
        std::wstring pwd;
        std::wcout << L"Enter password for client " << i << L" (3 digits) or leave blank for auto-gen: ";
        std::getline(std::wcin, pwd);
        if (!IsValidPassword(pwd)) {
            pwd = GeneratePassword();
        }
        clientPasswords[i] = pwd;
        std::wcout << L"Password for client " << i << L": " << pwd << std::endl;
    }

    hSemaphore = CreateSemaphore(NULL, 2, 2, NULL);
    if (hSemaphore == NULL) {
        std::wcerr << L"Failed to create semaphore. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < clientCount; ++i) {
        threads.emplace_back(ClientHandler, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    CloseHandle(hSemaphore);

    std::wcout << L"Server shutting down." << std::endl;
    return 0;
}
