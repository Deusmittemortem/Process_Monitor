#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <chrono>
#include <thread>

// Proverka sushestvovania processa
static bool IsProcessRunningByName(const std::wstring& name) {
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

static void MonitorAndLogoffOnAbsence(const std::wstring& targetName,
    std::chrono::seconds timeout,
    std::chrono::seconds pollInterval)
{
    std::chrono::steady_clock::time_point missingSince{};
    bool wasMissing = false;

    for (;;) {
        const bool running = IsProcessRunningByName(targetName);

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
                const auto absentFor = std::chrono::steady_clock::now() - missingSince;
                if (absentFor >= timeout) {
                    MessageBoxW(nullptr,
                        L"Процесс остановлен",
                        L"Мониторинг процесса",
                        MB_OK | MB_ICONINFORMATION);
                    break;
                }
            }
        }
        std::this_thread::sleep_for(pollInterval);
    }
}

