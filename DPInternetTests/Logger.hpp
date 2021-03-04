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

#include <stdio.h>
#include <windows.h>

class Logger{

public:

/* 
 * NAME: Setup
 * 
 * PURPOSE: Setup log writing
 *
 * ARGUMENTS:
 * 
 *      None
 * 
 */
static void Setup();

/* 
 * NAME: CopyResults
 * 
 * PURPOSE: Copy results from log to clipboard
 *
 * ARGUMENTS:
 * 
 *      hDlg - Dialog handle in which clipboard
 *             data is stored under.
 * 
 */
static void CopyResults(HWND hDlg);

/* 
 * NAME: LogToFile
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 * 
 *     ToFile - Input string to write
 * 
 */
static void LogToFile(const char *ToFile);

/* 
 * NAME: LogToBox
 * 
 * PURPOSE: Log input string to file
 *
 * ARGUMENTS:
 *      
 *      hDlg - Dialog handle to display message in
 *      ToLog - String to write
 *      Type - Type of message to display
 *      
 *      3 = Write log to blank page (spaced out from previous string)
 *      2 = Write log to existing page (spaced out from previous string)
 *      1 = Write log to blank page (not spaced out)
 *      0 = Write log to existing page (not spaced out)
 * 
 * NOTE: Do NOT input type 0 or 2 prior to inputting type 1 or 3
 *       Doing so will cause a crash.
 */
static void LogToBox(HWND hDlg, const char *ToLog, int Type);

};