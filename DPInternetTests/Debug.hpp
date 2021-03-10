#include "Logger.hpp"

//
// Putting this here because it already
// exists in DPInternetTests.cpp
//
extern char ToOutputLog[16384];
extern char Version[];

class Debug {
public:
    static void SysInfo(HWND hDlg) {
        SYSTEM_INFO SysInfo;
        OSVERSIONINFOA OSVer;

        memset(&SysInfo, 0, sizeof(SYSTEM_INFO));
        OSVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetSystemInfo(&SysInfo);
        GetVersionExA(&OSVer);
        
        Logger::LogToBox(hDlg, "======== System Information ========", 1);
        
        sprintf(ToOutputLog, "Reporting %s", Version);
        Logger::LogToBox(hDlg, ToOutputLog, 0);

        sprintf(ToOutputLog, "Windows Version: %u.%u %d", OSVer.dwMajorVersion, OSVer.dwMinorVersion, OSVer.dwBuildNumber);
        Logger::LogToBox(hDlg, ToOutputLog, 2);

        sprintf(ToOutputLog, "Number of Processors Installed: %d", SysInfo.dwNumberOfProcessors);
        Logger::LogToBox(hDlg, ToOutputLog, 2);
        // time
        switch(SysInfo.wProcessorArchitecture)
        {
            case PROCESSOR_ARCHITECTURE_AMD64:
                Logger::LogToBox(hDlg, "Processor Architecture: x64 (AMD or Intel)", 0);
                break;
            case PROCESSOR_ARCHITECTURE_ARM:
                Logger::LogToBox(hDlg, "Processor Architecture: ARM-based", 0);
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                Logger::LogToBox(hDlg, "Processor Architecture: ARM64-based", 0);
                break;
            case PROCESSOR_ARCHITECTURE_IA64:
                Logger::LogToBox(hDlg, "Processor Architecture: Intel Itanium-based", 0);
                break;
            case PROCESSOR_ARCHITECTURE_INTEL:
                Logger::LogToBox(hDlg, "Processor Architecture: x86 (AMD or Intel)", 0);
                break;
            default:
                Logger::LogToBox(hDlg, "Unknown processor architecture type", 0);
                break;
        }
    }
};