/*++
						The MIT License

Copyright (c) 2021 A Screensaver

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--*/

#include "Logger.h"
#include "Resource.h"

const char *LogFilename = "DPInternetTests_log.txt";
char *InterOut;
char LogBox[2048];
char Out[2048];
FILE *LogFile;
int CxsWritten; /* Characters written by sprintf */
SYSTEMTIME LocalTime;

//
// Putting this here because it already
// exists in DPInternetTests.cpp
//
extern char ToOutputLog[16384];


void
Logger::Setup()
/*++

Routine Description:
    
    Erase log.txt from executable directory and reset log

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // Delete File
    //

    DeleteFileA(LogFilename);

    //
    // Allocate Memory and Open File
    //

    malloc(sizeof(LogFile));
    LogFile = fopen(LogFilename, "a");
    
    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to create log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
            
        free(LogFile);
        return;
    }
    
    fprintf(LogFile, "Starting Log\r\n");

    // Close file and free memory
    fclose(LogFile);
    free(LogFile);
}

void
Logger::CopyResults(HWND hDlg)
/*++

Routine Description:
    
    Open clipboard and erase previous data,
    Copy log to memory and then into clipboard.

Arguments:

    hDlg - Dialog handle in which the clipboard is stored.

Return Value:

    None.

--*/

{
    HGLOBAL hGlobal;

    //
    // Open clipboard and empty/wipe it out.
    //

    if (!OpenClipboard(hDlg))
    {
        Logger::LogToFile("Logger::CopyResults() : Failed to Open Clipboard");
        return;
    }

    EmptyClipboard();

    //
    // Get log from box and allocate memory for clipboard data.
    //
    
    GetDlgItemTextA(hDlg, OverallResults, LogBox, sizeof(LogBox));
    hGlobal = GlobalAlloc(GMEM_MOVEABLE, strlen(LogBox) + 1);

    if (!hGlobal)
    {
        LogToFile("Logger::CopyResults() : Failed to allocate memory for clipboard data");
        return;
    }

    //
    // Copy clpboard data to memory.
    //

    memcpy(GlobalLock(hGlobal), LogBox, strlen(LogBox) + 1);

    //
    // Release memory, set clipboard data
    // and close clipboard.
    //

    GlobalUnlock(hGlobal);
    SetClipboardData(CF_TEXT, hGlobal);
    CloseClipboard();
    
    if (IsDebuggerPresent() != 0)
        OutputDebugStringA(LogBox);
}

void
Logger::LogToFile(const char *ToFile)
/*++

Routine Description:
    
    Write input string to external log file

Arguments:

    ToFile - String to write

Return Value:

    None.

--*/

{
    //
    // Allocate Memory and Open File
    //

    malloc(sizeof(LogFile));
    LogFile = fopen(LogFilename, "a");

    if (LogFile == 0)
    {
        MessageBoxA(NULL, "Failed to write to log file", "Error", MB_ICONERROR);

        if (LogFile)
            fclose(LogFile);
            
        free(LogFile);
        return;
    }

    //
    // Get local time, store it.
    // Then write input argument to file.
    //

    GetLocalTime(&LocalTime);
    fprintf(LogFile,
              "%02d/%02d/%04d @ %02d:%02d:%02d (Local Time) : %s\r\n",
              LocalTime.wMonth,
              LocalTime.wDay,
              LocalTime.wYear,
              LocalTime.wHour,
              LocalTime.wMinute,
              LocalTime.wSecond,
              ToFile);

    //
    // Close file and free memory
    //
    
    fclose(LogFile);
    free(LogFile);
}

void
Logger::LogToBox(HWND hDlg, const char *ToLog, int Type)
/*++

Routine Description:
    
    Write input string to dialog output log

Arguments:

    hDlg - Dialog handle to display message in

    ToLog - String to write

    Type - Type of message to display
        3 = Write log to blank page (with spacing)
        2 = Write log to existing page (with spacing)
        1 = Write log to blank page (without spacing)
        0 = Write log to existing page (without spacing)

Return Value:

    None.

--*/

{
    if (Type == 3)
    {
        CxsWritten = sprintf(Out, "\r\n%s\r\n", ToLog);
        InterOut = Out + CxsWritten;
    }
    if (Type == 2)
    {
        CxsWritten = sprintf(InterOut, "\r\n%s\r\n", ToLog);
        InterOut = InterOut + CxsWritten;
    }

    if (Type == 1)
    {
        CxsWritten = sprintf(Out, "%s\r\n", ToLog);
        InterOut = Out + CxsWritten;
    }

    if (Type == 0)
    {
        CxsWritten = sprintf(InterOut, "%s\r\n", ToLog);
        InterOut = InterOut + CxsWritten;
    }
    
    SetDlgItemTextA(hDlg, OverallResults, Out);
}