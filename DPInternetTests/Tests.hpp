#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <WinInet.h>

#pragma comment(lib, "Wininet")

class Tests{
    
public:
    static const char *TestErrors(DWORD ErrorCode) {
        switch (ErrorCode)
        {
            case 87:
                return "No Input URL was provided";
                break;
            case 12002:
                return "Internet connection timed out";
                break;
            case 12004:
                return "Internal WinHTTP error, this is NOT an application error!";
                break;
            case 12005:
                return "Invalid URL was provided";
                break;
            case 12006:
                return "Unrecognized URL scheme (\"http://\", \"https://\" and \"ftp://\" are the only ones supported)";
                break;
            case 12007:
                return "Input URL was not resolved properly";
                break;
            case 12009:
                return "Invalid WinHTTP option";
                break;
            case 12029:
                return "Internet connection was forcibly closed";
                break;
            default:
                return "Unlisted Error, please contact the developer as soon as possible!";
                break;
        }
    }
    static UINT __stdcall InternetTest(void *);
    static UINT __stdcall HTTPSTest(void *);
    static UINT __stdcall WinsockStressTest(void *);
};
