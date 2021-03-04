#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <WinInet.h>

#pragma comment(lib, "Wininet")

class Tests{
    
public:
    static UINT __stdcall InternetTest(void *);
    static UINT __stdcall HTTPSTest(void *);
    static UINT __stdcall WinsockStressTest(void *);
};
