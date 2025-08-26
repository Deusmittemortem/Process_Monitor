#include "Process+Monitoring.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const std::wstring targetProcess = L"notepad++.exe";   
    const auto timeout = std::chrono::seconds(23);    
    const auto pollInterval = std::chrono::seconds(10);  

    MonitorAndLogoffOnAbsence(targetProcess, timeout, pollInterval);
    return 0;
}
