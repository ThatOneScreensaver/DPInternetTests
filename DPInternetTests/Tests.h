#include <stdio.h>
#include <windows.h>
#include <WinInet.h>

#pragma comment(lib, "Wininet")

class Tests{
    
public:
static UINT __stdcall InternetTest(void *);
static UINT __stdcall Tests::HTTPSTest(void *);

};