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
#include "Tests.h"

//
// All of these already exist in the main cpp file
//
extern BOOL UsingProxy;
extern char *Buffer;
extern char inputurl[512]; /* Input URL */
extern char CstmHeader[256]; /* Custom HTTP Header */
extern char CustomProxy[256]; /* Custom Proxy */
extern char ToOutputLog[16384];
extern char UserAgent[256]; /* Custom User Agent */
extern const char *Version;
extern DWORD LastError; /* Error Code */
extern HWND hWnd;
extern HWND InternetRequestButton;
extern HWND HTTPSRequestButton;



UINT __stdcall
Tests::InternetTest(void *)
// Thread Steps:
// 
// 1) Open Internet Handle
// 2) Open URL using created internet handle
// 3) Get HTTP Query Info and Display it
// 
// 4) End Thread

{
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	LPCSTR homeurl = "http://www.msftconnecttest.com/redirect";

    //
    // Disable buttons
    //

	EnableWindow(InternetRequestButton, 0); 
	EnableWindow(HTTPSRequestButton, 0);

	//
	// Display Thread Progress
	//

	Logger::LogToBox(hWnd, "Beginning InternetTest() Thread", 1);
	Logger::LogToBox(hWnd, "Doing full GET request", 2);

	//
	// Open Internet Handle
	//

	IntOpenHandle = InternetOpenA(Version,0,0,0,0);
	if (IntOpenHandle == 0)
	{
		Logger::LogToBox(hWnd, "*** InternetOpen() failed!", 0);
		return 0;
	}

	//
	// Open Predefined URL
	//
    
	IntOpenUrl = InternetOpenUrlA(IntOpenHandle, homeurl, 0, 0,INTERNET_FLAG_RAW_DATA,0);
	if (IntOpenUrl == 0)
	{
		/* Get error and store it */
		LastError = GetLastError();

		// Failed To Open The URL
		sprintf(ToOutputLog, "*** InternetOpenURL () failed! Error = %.8X ( %u )\r\n", LastError, LastError);
		Logger::LogToBox(hWnd, ToOutputLog, 0);

		InternetCloseHandle(IntOpenHandle);
		return 0;
	}

	//
	// Get and store http headers
	//

	BufLen = 16384 - strlen(ToOutputLog);
	HttpQueryInfoA(IntOpenUrl,
		     	   HTTP_QUERY_RAW_HEADERS_CRLF,
				   (LPVOID)ToOutputLog,
				   &BufLen, 
				   0);
	
	//
	// Display headers and enable buttons
	//

	Logger::LogToBox(hWnd, ToOutputLog, 0);
	InternetCloseHandle(IntOpenUrl);
	InternetCloseHandle(IntOpenHandle);
	EnableWindow(InternetRequestButton, 1);
	EnableWindow(HTTPSRequestButton, 1);
	return 0xdeadbeef;
}

UINT __stdcall
Tests::HTTPSTest(void *)
// Thread Steps:
// 
// 1) Open Internet Handle with User-Agent (either with or without proxy)
// 2) Open URL using previously created internet handle
// 3) Get HTTP Query Info and Display it
// 
// 4) Open Internet Handle with User-Agent (either with or without proxy)
// 5) Open URL using previously created internet handle with (custom) http headers 
// 6) Get HTTP Query Info and Display it
// 
// 7) End Thread

{
	int Idk;
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	
	//***************************************************************************//

	// Zero-Out Any Left Over Residue 
	memset(CustomProxy, 0, sizeof(CustomProxy));
	memset(UserAgent, 0, sizeof(UserAgent));
	memset(inputurl, 0, sizeof(inputurl));

	// Get URL and store it
	GetDlgItemTextA(hWnd, IDD_HTTPS_URL, inputurl, 512);

	//
	// Get user agent setting
	// If it's blank, use default UA
	//

	GetDlgItemTextA(hWnd, CustomUserAgentString, UserAgent, sizeof(UserAgent));
	if (_stricmp(UserAgent, "") == 0)
	{
		sprintf(UserAgent, Version); // Default to regular If no UA is specified
	}
			
	//
	// Get proxy address
	//

	GetDlgItemTextA(hWnd, CustomProxyAddress, CustomProxy, sizeof(CustomProxy));
	if (_stricmp(CustomProxy, "") == 0)
	{
		UsingProxy = FALSE; /* Empty/No Proxy */
	}
	else
	{
		UsingProxy = TRUE;
	}
			
	//
	// Disable Buttons
	//
			
	EnableWindow(InternetRequestButton, 0);
	EnableWindow(HTTPSRequestButton, 0);

	//***************************************************************************//

	// Zero It Out
	ToOutputLog[0] = '\0';

	//------------------BEGINNING OF FULL GET REQUEST----------------

	Logger::LogToBox(hWnd, "Beginning HTTPSTest() Thread", 1);

	//
	// Output
	//
	
	sprintf(ToOutputLog, "Doing HTTPS request with User-Agent \"%s\"", UserAgent);
	Logger::LogToBox(hWnd, ToOutputLog, 2);

	//
	// Determine whether to open internet handle with proxy
	//

	if (UsingProxy == TRUE)
	{
		sprintf(ToOutputLog, "Using Proxy Address \"%s\"", CustomProxy);
		Logger::LogToBox(hWnd, ToOutputLog, 0);
	
		// Open With Proxy
		IntOpenHandle = InternetOpenA(UserAgent,INTERNET_OPEN_TYPE_PROXY,CustomProxy,0,0);
	}
	else
	{
		// Normal
		IntOpenHandle = InternetOpenA(UserAgent,0,0,0,0);
	}

	//
	// Check whether the handle got created successfully
	//
	
	if (IntOpenHandle == 0)
	{
		LastError = GetLastError();

		sprintf(Buffer, "*** InternetOpen() failed!, error = 0x%.8X ( %u )\r\n", LastError, LastError);


		InternetCloseHandle(IntOpenHandle);
		goto Close;
	}

	//
	// Open input URL and determine if opened properly
	//

	IntOpenUrl = InternetOpenUrlA(IntOpenHandle, inputurl, 0, 0,INTERNET_FLAG_RAW_DATA,0);
	if (IntOpenUrl == 0)
	{
		LastError = GetLastError();

		sprintf(ToOutputLog, "*** InternetOpenURL ( %s ) failed! Error = 0x%.8X ( %u )", inputurl, LastError, LastError);
		Logger::LogToBox(hWnd, ToOutputLog, 0);			

		InternetCloseHandle(IntOpenHandle);
		goto Close;
	}
		
	BufLen = 0x4000 - strlen(ToOutputLog);	
	HttpQueryInfoA(IntOpenUrl,
				   HTTP_QUERY_RAW_HEADERS_CRLF, /* All headers returned from remote server */
				   (LPVOID)ToOutputLog,
				   &BufLen,
				   0);
	Logger::LogToBox(hWnd, ToOutputLog, 0);

	InternetCloseHandle(IntOpenUrl);
	InternetCloseHandle(IntOpenHandle);
	
	//
	// End of HTTPS get request
	//

	Logger::LogToBox(hWnd, "End of full HTTP request", 0);

	//--------------------END OF FULL GET REQUEST--------------------


	//----------------BEGINNING OF CUSTOM GET REQUEST---------------


	Logger::LogToBox(hWnd, "=================================================================", 2);
	

	// Custom Content Request
	GetDlgItemTextA(hWnd, CustomHeader, CstmHeader, sizeof(CstmHeader));

	// Output
	sprintf(ToOutputLog, "Doing custom HTTPS request with User-Agent \"%s\"", UserAgent);
	Logger::LogToBox(hWnd, ToOutputLog, 2);

	/* 
	 * Proxy is set
	 */
	if (UsingProxy == TRUE)
	{
		sprintf(ToOutputLog, "Using Proxy Address \"%s\"", CustomProxy);
		Logger::LogToBox(hWnd, ToOutputLog, 0);
	
		// Open With Proxy
		IntOpenHandle = InternetOpenA(UserAgent,INTERNET_OPEN_TYPE_PROXY,CustomProxy,0,0);
	}

	else /* Not Using A Proxy */
	{
		// Normal
		IntOpenHandle = InternetOpenA(UserAgent,0,0,0,0);
	}


	if (IntOpenHandle == 0)/* Failed */
	{
		/* 
		 * Get error code and store it
		 */
		LastError = GetLastError();

		sprintf(ToOutputLog, "*** InternetOpen() failed!, error = 0x%.8X ( %u )\r\n", LastError, LastError);
		Logger::LogToBox(hWnd, ToOutputLog, 0);
		
		return 0;
	}

	
	IntOpenUrl = InternetOpenUrlA(IntOpenHandle, inputurl, CstmHeader, INFINITE, INTERNET_FLAG_RAW_DATA | INTERNET_FLAG_PRAGMA_NOCACHE, 0);

	if (IntOpenUrl == 0)/* Failed */
	{
	  	// Store error code
		LastError = GetLastError();

		//
		// Show error with code and exit thread
		//
		sprintf(ToOutputLog, "*** InternetOpenURL ( %s ) failed! Error = 0x%.8X ( %u )\r\n", inputurl, LastError, LastError);
		Logger::LogToBox(hWnd, ToOutputLog, 0);
		InternetCloseHandle(IntOpenHandle);
		return 0;
	}

		
	BufLen = 0x4000 - strlen(ToOutputLog);
	HttpQueryInfoA(IntOpenUrl, /* Handle */
				   HTTP_QUERY_RAW_HEADERS_CRLF, /* Request Info Level */
				   (LPVOID)ToOutputLog, /* Buffer to store query info to */
				   &BufLen, /* Length of said buffer */
				   0); /* Index */
	Logger::LogToBox(hWnd, ToOutputLog, 0);

	//
	// Close handles, and enable buttons
	//

	Logger::LogToBox(hWnd, "End of custom HTTPS request", 0);
	InternetCloseHandle(IntOpenUrl);
	InternetCloseHandle(IntOpenHandle);

	Close:

	EnableWindow(InternetRequestButton, 1);
	EnableWindow(HTTPSRequestButton, 1);
	return 0xdeadbeef;
}