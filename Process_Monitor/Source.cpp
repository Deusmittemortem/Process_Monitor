#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

bool IsProcessRunningByName(const std::wstring& name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);

    bool running = false;
    if (Process32FirstW(snap, &pe)) {
        do {
            
            if (_wcsicmp(pe.szExeFile, name.c_str()) == 0) {
                running = true;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return running;
}

int main() {
    setlocale(LC_ALL, "Ru");
    const std::wstring target = L"notepad++.exe"; // The process name is case sensitive
    const auto timeout = std::chrono::seconds(20);

    std::wcout << L"Мониторинг процесса: " << target << L"\n"; //Debug run 

    std::chrono::steady_clock::time_point missingSince{};
    bool wasMissing = false;

    while (true) {
        bool running = IsProcessRunningByName(target);

        if (running) {
            wasMissing = false;
            missingSince = {};
        }
        else {
            if (!wasMissing) {
                wasMissing = true;
                missingSince = std::chrono::steady_clock::now();
            }
            else {
                auto absentFor = std::chrono::steady_clock::now() - missingSince;
                if (absentFor >= timeout) {
                    //ExitWindowsEx(EWX_LOGOFF, 0);
                    std::cout << "Процесс выключен\n"; //Debug run
                    break;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(10)); 
    }
    return 0;
}
