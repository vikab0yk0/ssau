#include <iostream>
#include <windows.h>
#include <string>

#define MAX_ATTEMPTS 3
#define PIPE_PREFIX L"\\\\.\\pipe\\bank_client"

struct TransactionMessage {
    DWORD clientId;
    bool authorized;
    double amount;
};

int main() {
    int clientId;
    std::wcout << L"Enter your client ID: ";
    std::wcin >> clientId;
    std::wcin.ignore();

    std::wstring pipeName = PIPE_PREFIX + std::to_wstring(clientId);

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    while (true) {
        hPipe = CreateFileW(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING,
            0, NULL);

        if (hPipe != INVALID_HANDLE_VALUE) {
            break;
        }
        std::wcout << L"Waiting for server to create pipe..." << std::endl;
        Sleep(1000);
    }

    // Ждем код от сервера о состоянии соединения
    wchar_t serverMsg;
    DWORD bytesRead;
    if (!ReadFile(hPipe, &serverMsg, sizeof(wchar_t), &bytesRead, NULL) || bytesRead == 0) {
        std::wcerr << L"Failed to read initial server message." << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    if (serverMsg == L'B') {
        std::wcout << L"You are blocked by the server due to previous incorrect password attempts." << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    if (serverMsg != L'S') {
        std::wcerr << L"Unexpected server message." << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Авторизация
    int attempts = 0;
    bool authorized = false;

    while (attempts < MAX_ATTEMPTS) {
        std::wstring pwd;
        std::wcout << L"Enter your 3-digit password: ";
        std::getline(std::wcin, pwd);

        if (pwd.size() != 3) {
            std::wcout << L"Password must be exactly 3 digits." << std::endl;
            continue;
        }

        DWORD bytesWritten;
        BOOL writeRes = WriteFile(hPipe, pwd.c_str(), (DWORD)(3 * sizeof(wchar_t)), &bytesWritten, NULL);
        if (!writeRes || bytesWritten == 0) {
            std::wcerr << L"Failed to send password." << std::endl;
            CloseHandle(hPipe);
            return 1;
        }

        wchar_t resp;
        DWORD respRead;
        BOOL readRes = ReadFile(hPipe, &resp, sizeof(wchar_t), &respRead, NULL);
        if (!readRes || respRead == 0) {
            std::wcerr << L"Failed to read response from server." << std::endl;
            CloseHandle(hPipe);
            return 1;
        }

        if (resp == L'Y') {
            authorized = true;
            std::wcout << L"Authorization successful!" << std::endl;
            break;
        }
        else if (resp == L'N') {
            std::wcout << L"Incorrect password. Try again." << std::endl;
            attempts++;
        }
        else if (resp == L'B') {
            std::wcout << L"You are blocked due to 3 incorrect password attempts." << std::endl;
            CloseHandle(hPipe);
            return 1;
        }
        else {
            std::wcout << L"Unexpected server response." << std::endl;
            CloseHandle(hPipe);
            return 1;
        }
    }

    if (!authorized) {
        std::wcout << L"Authorization failed. Exiting." << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Работа с транзакциями
    while (true) {
        double amount;

        while (true) {
            std::wcout << L"Enter transaction amount (negative to exit): ";
            std::wcin >> amount;

            if (std::wcin.fail()) {               
                std::wcin.clear();                
                std::wcin.ignore(10000, L'\n'); 
                std::wcerr << L"Invalid input, please enter a number." << std::endl;
                continue;                       
            }

            std::wcin.ignore(10000, L'\n');      
            break;                             
        }

        TransactionMessage msg;
        msg.clientId = clientId;
        msg.authorized = true;
        msg.amount = amount;

        DWORD bytesWritten;
        BOOL writeRes = WriteFile(hPipe, &msg, sizeof(msg), &bytesWritten, NULL);
        if (!writeRes || bytesWritten == 0) {
            std::wcerr << L"Failed to send transaction." << std::endl;
            break;
        }

        if (amount < 0)
            break;
    }

    CloseHandle(hPipe);
    std::wcout << L"Client disconnected." << std::endl;
    return 0;
}
