// DPInternetTests
// Multi-threaded Internet Tests
// By ThatOneScreensaver

// License Can Be Found in "LICENSE"

#include "stdafx.h"
#include "DPInternetTests.h"
#include <process.h>
#include <stdio.h>
#include <Windows.h>


#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")


// Disable Unreferenced Variable Warnings
#pragma warning(disable:4101)

// Disable Deprecated Function Warnings
#pragma warning(disable:4995)

// Disable Unsecure Function Warnings
#pragma warning(disable:4996)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

BOOL UsingCustomPort;
BOOL UsingProxy;
BOOL StressTestOn;

const char *Version = "DPInternetTests v1.7.1";
char CustomPortNum;
char CustomProxy[256];
char inputurl[512];
char Out[16384];
char Request[256];
char UserAgent[256];
	
char *Buffer;

DWORD LastError;

int Idk;
int TotalRequestsNum = 0;

// TODO: Create An Array For These...
HWND UserAgentCheckBox;
HWND ProxyCheckBox;
HWND InternetRequestButton;
HWND HTTPSRequestButton;
HWND StressTestButton;

WSADATA WinSockData;

// Function Declarations
char *RandomString(char *RandomString);
INT_PTR CALLBACK	InternetTest(HWND, UINT, WPARAM, LPARAM);
UINT __stdcall 		InternetTest(void *);
UINT __stdcall		HTTPSTest(void *);
UINT __stdcall		StressTest(void *);
UINT __stdcall 		WinSockStressTest(void *);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	/* 
	 * Create this as a way to make sure no other instances are running
	 */
	HANDLE hObject = CreateMutexA((LPSECURITY_ATTRIBUTES)0x0, 0, "DPInternetTests_Mutex");

	/* 
	 * Either it failed to create it (hObject returning 0), or it already exists
	 */
	if ((hObject == 0) || (LastError = GetLastError(), LastError == ERROR_ALREADY_EXISTS))
	{
		MessageBoxExA(NULL,
					  "Another instance of InternetTests is running\r\nExit it before starting a new instance",
					  NULL, MB_ICONERROR, 0);
		exit(0);
	}

	if (MessageBoxExA(hWnd, "The developer is not responsible for any damages caused by this program, by clicking \"Yes\" you agree to hold complete responsiblity for your actions", "Agreement", MB_ICONWARNING | MB_YESNO, 0) == 6)
		DialogBoxParamA(hInst, MAKEINTRESOURCEA(IDD_MAIN), NULL, InternetTest, 0);
	else
		// Exit
		exit(0);
}

INT_PTR CALLBACK InternetTest(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	tagRECT *lpRect;
	tagRECT Rect1;
	tagRECT Rect2;

	hWnd = hDlg;
	lpRect = &Rect2;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:

		// Set Window Title
		SetWindowTextA(hDlg, Version);
		
		LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_DPINTERNETTESTS));

		// Output Console Stuff
		Idk = sprintf(Out, "======================\r\n");
		Buffer = Out + Idk;
		Idk = sprintf(Buffer, "Reporting multi-threaded %s\r\n", Version);
		Buffer = Buffer + Idk;
		
		// Startup WinSock2
		if (WSAStartup(0x101, &WinSockData) != 0)
		{
			Idk = sprintf(Buffer, "\r\nFailed to start WinSock2, error = %d\r\n", WSAGetLastError());
			Buffer = Buffer + Idk;
		}


		// Set Text
		SetDlgItemTextA(hDlg, OverallResults, Out);


		hWnd = GetDesktopWindow();
		GetWindowRect(hWnd, lpRect);
		GetWindowRect(hDlg, &Rect1);
		

		// Set Window Position To Center Of Screen
		SetWindowPos(hDlg,NULL,
                 (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,0,0,1);


		// I know this is probably the worst place to put this but meh
		InternetRequestButton = GetDlgItem(hDlg, IDD_INTERNET_TEST);
		HTTPSRequestButton = GetDlgItem(hDlg, IDD_HTTPS_TEST);
		UserAgentCheckBox = GetDlgItem(hDlg, CustomUserAgentCheckBox);
		ProxyCheckBox = GetDlgItem(hDlg, CustomProxyCheckBox);

		// Begin thread on startup (uncomment this if you want to)
		//_beginthreadex(NULL, 0, InternetTest, NULL, 0, 0);
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		// Internet Button
		if (LOWORD(wParam) == IDD_INTERNET_TEST)
		{
			EnableWindow(InternetRequestButton, 0); /* Disable Button To Prevent Potential DDOSing */

			EnableWindow(HTTPSRequestButton, 0); /* Disable Button To Prevent Potential DDOSing */

			UserAgent[0] = '\0';
			sprintf(UserAgent, Version);

			_beginthreadex(NULL, 0, &InternetTest, NULL, 0, 0);

			return 1;
		}

		//******************************************************************
		// HTTPS Button Pressed
		if (LOWORD(wParam) == IDD_HTTPS_TEST)
		{
			UsingProxy = FALSE; /* Reset */

			// Zero-Out Any Left Over Residue 
			ZeroMemory(CustomProxy, sizeof(CustomProxy));
			ZeroMemory(UserAgent, sizeof(UserAgent));
			ZeroMemory(inputurl, sizeof(inputurl));

			// Get URL and store it
			GetDlgItemTextA(hDlg, IDD_HTTPS_URL, inputurl, 512);

			if (IsDlgButtonChecked(hDlg, CustomUserAgentCheckBox) == BST_CHECKED) /* Checked Box */
			{
				// Get UserAgent and store it
				GetDlgItemTextA(hDlg, CustomUserAgentString, UserAgent, sizeof(UserAgent));

				if (_stricmp(UserAgent, "") == 0) /* Blank/Empty User-Agent */
				{
					sprintf(UserAgent, Version); // Default to regular If no UA is specified
				}
			}

			else/* Default to regular UA */
			{
				sprintf(UserAgent, Version);
			}

			if (IsDlgButtonChecked(hDlg, CustomProxyCheckBox) == BST_CHECKED) /* Checked Box */
			{
				// Initial Value, this will remain
				// true if everything succeeds
				UsingProxy = TRUE;

				GetDlgItemTextA(hDlg, CustomProxyAddress, CustomProxy, sizeof(CustomProxy));

				if (_stricmp(CustomProxy, "") == 0) /* No Proxy Address was specificed */
				{
					UsingProxy = FALSE; /* Empty/No Proxy */
				}
			}

			else
			{
				UsingProxy = FALSE;
			}
			
			/* Disable (Almost) Everything */
			EnableWindow(InternetRequestButton, 0);
			EnableWindow(HTTPSRequestButton, 0);
			EnableWindow(UserAgentCheckBox, 0);
			EnableWindow(ProxyCheckBox, 0);

			// Begin Thread
			_beginthreadex(NULL, 0, HTTPSTest, NULL, 0, 0);

			return 1;
		}
		//******************************************************************


		//******************************************************************
		// Exiting
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDM_EXIT)/* All possible exit routes */
		{
			/*
			 * Yes was clicked
			 */
			if (MessageBoxExA(hDlg, "Exit DPInternetTests?", "DPInternetTests", MB_ICONQUESTION | MB_YESNO, 0) == 6)
			{
				DestroyWindow(hDlg);
			}
			return (INT_PTR)TRUE;
		}
		//******************************************************************

		//******************************************************************
		// Stress Test Button
		// if (LOWORD(wParam) == StressTestBtn)
		// {
		// 	if (StressTestOn == 0)
		// 	{
		// 		StressTestOn = TRUE;
		// 	}

		// 	else
		// 	{
		// 		StressTestOn = FALSE;
		// 	}

		// 	if (StressTestOn == TRUE)
		// 	{
		// 		UsingProxy = FALSE; /* Reset */

		// 		// Zero-Out Anything That Could Still Be Resident Here
		// 		ZeroMemory(CustomProxy, sizeof(CustomProxy));
		// 		ZeroMemory(UserAgent, sizeof(UserAgent));
		// 		ZeroMemory(inputurl, sizeof(inputurl));

		// 		// Get URL and store it
		// 		GetDlgItemTextA(hDlg, IDD_HTTPS_URL, inputurl, 512);

		// 		if (IsDlgButtonChecked(hDlg, CustomUserAgentCheckBox) == BST_CHECKED) /* Checked Box */
		// 		{
		// 			// Get UserAgent and store it
		// 			GetDlgItemTextA(hDlg, CustomUserAgentString, UserAgent, sizeof(UserAgent));

		// 			if (_stricmp(UserAgent, "") == 0) /* Blank/Empty User-Agent */
		// 			{
		// 				sprintf(UserAgent, Version); // Default to regular If no UA is specified
		// 			}
		// 		}

		// 		else/* Default to regular UA */
		// 		{
		// 			sprintf(UserAgent, Version);
		// 		}

		// 		if (IsDlgButtonChecked(hDlg, CustomProxyCheckBox) == BST_CHECKED) /* Checked Box */
		// 		{
		// 			// Initial Value, this will remain
		// 			// true if everything succeeds
		// 			UsingProxy = TRUE;

		// 			GetDlgItemTextA(hDlg, CustomProxyAddress, CustomProxy, sizeof(CustomProxy));

		// 			if (_stricmp(CustomProxy, "") == 0) /* No Proxy Address was specificed */
		// 			{
		// 				UsingProxy = FALSE; /* Empty/No Proxy */
		// 			}
		// 		}

		// 		else
		// 		{
		// 			UsingProxy = FALSE;
		// 		}

		// 		GetDlgItemTextA(hDlg, IDD_HTTPS_URL, inputurl, sizeof(inputurl));

		// 		SetDlgItemTextA(hDlg, StressTestBtn, "Stop Test");
				
		// 		if (MessageBoxExA(hWnd, "By pressing \"Yes\", you agree to hold all responsibility for your actions", "Agreement", MB_ICONWARNING | MB_YESNO, 0) == 6)
		// 		{
		// 			EnableWindow(InternetRequestButton, 0);

		// 			EnableWindow(HTTPSRequestButton, 0);
					
		// 			// _beginthreadex(NULL, 0, WinSockStressTest, NULL, 0, 0);
		// 			// _beginthreadex(NULL, 0, StressTest, NULL, 0, 0);
		// 			// _beginthreadex(NULL, 0, StressTest, NULL, 0, 0);
		// 			// _beginthreadex(NULL, 0, StressTest, NULL, 0, 0);
		// 			// _beginthreadex(NULL, 0, StressTest, NULL, 0, 0);
		// 		}

		// 		else
		// 		{
		// 			StressTestOn = FALSE;
		// 			SetDlgItemTextA(hDlg, StressTestBtn, "Start Test");
		// 		}

		// 	}
		// }
		break;
	case WM_DESTROY:
		WSACleanup();
		MessageBoxExA(hDlg, "Thank you so much for using my program! <3", "Thank You!", MB_ICONINFORMATION, 0);
		EndDialog(hDlg, message);
		return 1;
	}
	return 0;
}

//______________________________________________________
//__________T H R E A D E D  F U N C T I O N S__________
//______________________________________________________


UINT __stdcall InternetTest(void *)
{
	int Idk;
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	LPCSTR homeurl;
	
	Out[0] = '\0';

	// Feel Free To Replace This With Any URL
	// I'm just using this one bc it's the only one that could come to mind!
	homeurl = "http://www.msftconnecttest.com/redirect";

	// Begin Thread
	Idk = sprintf(Out, "Beginning InternetTest() Thread\r\n");
	Buffer = Out + Idk;

	// Output
	Idk = sprintf(Buffer, "\r\nDoing full GET request\r\n");
	Buffer = Buffer + Idk;

	// Show thread progress
	SetDlgItemTextA(hWnd, OverallResults, Out);

	IntOpenHandle = InternetOpenA("DPInternetTests",0,0,0,0);

	if (IntOpenHandle == 0)/* Failed */
	{
		Idk = sprintf(Buffer, "*** InternetOpen() failed!\r\n");
		Buffer = Buffer + Idk;

		goto End;
	}

	else/* Returned Non-Zero */
	{
		IntOpenUrl = InternetOpenUrlA(IntOpenHandle, homeurl, 0, 0,INTERNET_FLAG_RAW_DATA,0);

		if (IntOpenUrl == 0)/* Failed */
		{
			LastError = GetLastError();

			// Failed To Open The URL
			Idk = sprintf(Buffer, "*** InternetOpenURL () failed! Error = %.8X ( %u )\r\n", LastError, LastError);
			Buffer = Buffer + Idk;

			InternetCloseHandle(IntOpenHandle);
		}

		else/* Returned Non-Zero */
		{
			BufLen = 0x4000 - (int)(Buffer + -0x439680);
			HttpQueryInfoA(IntOpenUrl,
						  HTTP_QUERY_RAW_HEADERS_CRLF,
						  (LPVOID)Buffer,
						  &BufLen, /* don't even ask what the fustercluck this is*/
						  0);
			
			
			InternetCloseHandle(IntOpenUrl);
			InternetCloseHandle(IntOpenHandle);
		
		}
	}

	/* This part resets everything and shows results */
	End:

	SetDlgItemTextA(hWnd, OverallResults, Out);

	EnableWindow(InternetRequestButton, 1); /* Re-enable button */

	EnableWindow(HTTPSRequestButton, 1); /* Re-enable button */

	return 0xdeadbeef;

}

UINT __stdcall HTTPSTest(void *)
{
	int Idk;
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	

	// Zero It Out
	Out[0] = '\0';

	//------------------BEGINNING OF FULL GET REQUEST----------------

	Idk = sprintf(Out, "Beginning HTTPSTest() Thread\r\n\n");
	Buffer = Out + Idk;

	// Output
	Idk = sprintf(Buffer, "\r\nDoing HTTPS request with User-Agent: %s\r\n", UserAgent);
	Buffer = Buffer + Idk;

	if (UsingProxy == TRUE)
	{
		Idk = sprintf(Buffer, "Using Proxy Address %s\r\n", CustomProxy);
		Buffer = Buffer + Idk;
	
		// Open With Proxy
		IntOpenHandle = InternetOpenA(UserAgent,INTERNET_OPEN_TYPE_PROXY,CustomProxy,0,0);
	}

	else /* Not Using A Proxy */
	{
		// Normal
		IntOpenHandle = InternetOpenA(UserAgent,0,0,0,0);
	}

	SetDlgItemTextA(hWnd, OverallResults, Out);

	if (IntOpenHandle == 0)/* Failed (Returned Zero)*/
	{
		LastError = GetLastError();

		Idk = sprintf(Buffer, "*** InternetOpen() failed!, error = 0x%.8X ( %u )\r\n", LastError, LastError);
		Buffer = Buffer + Idk;

		goto End;
	}

	else/* Successful (Returned Non-Zero) */
	{

		IntOpenUrl = InternetOpenUrlA(IntOpenHandle, inputurl, 0, 0,INTERNET_FLAG_RAW_DATA,0);

		if (IntOpenUrl == 0)/* Failed */
		{
			LastError = GetLastError();

			// Failed To Open The URL
			Idk = sprintf(Buffer, "*** InternetOpenURL ( %s ) failed! Error = 0x%.8X ( %u )\r\n", inputurl, LastError, LastError);
			Buffer = Buffer + Idk;

			InternetCloseHandle(IntOpenHandle);

			goto End;
		}

		else/* Returned Non-Zero (Passed) */
		{
			BufLen = 0x4000 - (int)(Buffer + -0x439680);
			HttpQueryInfoA(IntOpenUrl,
						   HTTP_QUERY_RAW_HEADERS_CRLF,
						   (LPVOID)Buffer,
						   &BufLen, /* don't even ask what the fustercluck this is*/
						   0);
			Buffer = Buffer + BufLen;

			InternetCloseHandle(IntOpenUrl);
			InternetCloseHandle(IntOpenHandle);
		}
	}

	/* 
	 * Show end of Full HTTPS Request
	 */
	Idk = sprintf(Buffer, "End of full HTTPS request\r\n");
	Buffer = Buffer + Idk;

	SetDlgItemTextA(hWnd, OverallResults, Out);

	//--------------------END OF FULL GET REQUEST--------------------


	//----------------BEGINNING OF CUSTOM GET REQUEST---------------


	/* 
	 * Seperator
	 */
	Idk = sprintf(Buffer, "\r\n=================================================================\r\n");
	Buffer = Buffer + Idk;

	// Custom Content Request
	GetDlgItemTextA(hWnd, RequestInput, Request, sizeof(Request));

	// Output
	Idk = sprintf(Buffer, "\r\nDoing custom HTTPS request with User-Agent: %s\r\n", UserAgent);
	Buffer = Buffer + Idk;

	/* 
	 * Proxy is set
	 */
	if (UsingProxy == TRUE)
	{
		Idk = sprintf(Buffer, "Using Proxy Address %s\r\n", CustomProxy);
		Buffer = Buffer + Idk;
	
		// Open With Proxy
		IntOpenHandle = InternetOpenA(UserAgent,INTERNET_OPEN_TYPE_PROXY,CustomProxy,0,0);
	}


	else /* Not Using A Proxy */
	{
		// Normal
		IntOpenHandle = InternetOpenA(UserAgent,0,0,0,0);
	}

	// Doing this show thread progress so far in Results
	SetDlgItemTextA(hWnd, OverallResults, Out);

	if (IntOpenHandle == 0)/* Failed */
	{
		/* 
		 * Get error code and store it
		 */
		LastError = GetLastError();

		Idk = sprintf(Buffer, "*** InternetOpen() failed!, error = 0x%.8X ( %u )\r\n", LastError, LastError);
		Buffer = Buffer + Idk;
		
		goto End;
	}

	else/* Returned Non-Zero */
	{

		IntOpenUrl = InternetOpenUrlA(IntOpenHandle, inputurl, Request, INFINITE, INTERNET_FLAG_RAW_DATA | INTERNET_FLAG_PRAGMA_NOCACHE, 0);

		if (IntOpenUrl == 0)/* Failed */
		{
		   	/* 
		 	 * Get error code and store it
		 	 */	
			LastError = GetLastError();

			// Failed To Open The URL
			Idk = sprintf(Buffer, "*** InternetOpenURL ( %s ) failed! Error = 0x%.8X ( %u )\r\n", inputurl, LastError, LastError, LastError);
			Buffer = Buffer + Idk;

			/* 
			 * Close Internet Handle
			 */
			InternetCloseHandle(IntOpenHandle);

			goto End;
		}

		else/* Returned Non-Zero (Passed) */
		{
			BufLen = 0x4000 - (int)(Buffer + -0x439680);
			HttpQueryInfoA(IntOpenUrl,
						   HTTP_QUERY_RAW_HEADERS_CRLF,
						   (LPVOID)Buffer,
						   &BufLen, /* don't even ask what this fustercluck is*/
						   0);
			Buffer = Buffer + BufLen;

			InternetCloseHandle(IntOpenUrl);
			InternetCloseHandle(IntOpenHandle);
		}
	}

	/* 
	 * Show end of Custom HTTPS Request
	 */
	Idk = sprintf(Buffer, "End of custom HTTPS request\r\n");
	Buffer = Buffer + Idk;

	End:
	
	SetDlgItemTextA(hWnd, OverallResults, Out);

	EnableWindow(InternetRequestButton, 1); /* Re-enable button */

	EnableWindow(HTTPSRequestButton, 1); /* Re-enable button */
	
	EnableWindow(UserAgentCheckBox, 1);

	EnableWindow(ProxyCheckBox, 1);

	return 0xdeadbeef;
}

UINT __stdcall StressTest(void *)
{
	int Counter;
	int Idk;
	int InternalAddressInt[4];
	char *InternalAddressChar;
	char *NotUsed;
	char *sp;
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntConnectA;
	HINTERNET httpSession;
	HINTERNET OpenHead;
	HINTERNET OpenPost;
	hostent *hostEntry;
	

	// Zero It Out
	Out[0] = '\0';


	// Just because
	InternalAddressChar = inputurl;

	// Begin Thread
	Idk = sprintf(Out, "Beginning StressTest() Thread\r\n");
	Buffer = Out + Idk;

	GetDlgItemTextA(hWnd, IDD_HTTPS_URL, inputurl, sizeof(inputurl));

	/*
	 * If inputurl is empty, no hostname is provided
	 */
	if (_stricmp(inputurl, "") == 0)
	{
		
		Idk = sprintf(Buffer, "\r\n*** No Hostname Provided!\r\n");
		Buffer = Buffer + Idk;

		goto End;

	}



	// Before we start anything, must make sure that the "subject" server
	// is in the right format, in this case it has to be in e.g. "127.0.0.1"
	
	if (!isdigit(*inputurl)) /* first character is not in numerical format, must convert */
	{
		hostEntry = gethostbyname(inputurl);


		/* 
		 * Something happened and the server name could not be resolved
		 */
		if (hostEntry == NULL)
		{
			
			Idk = sprintf(Buffer, "\r\n*** Cannot resolve server name ( %s )\r\n", inputurl);
			Buffer = Buffer + Idk;

			// Don't Go Any Further
			goto End; 

		}

		/* 
		 * Incorrect Address Length
		 */
		if (hostEntry->h_length != 4)
		{
			Idk = sprintf(Buffer, "\r\n*** Length of internal address ( %s ) is incorrect, expected 4\r\n", hostEntry->h_length);
			Buffer = Buffer + Idk;

			// Don't Go Any Further
			goto End;
		}

		sp = hostEntry->h_addr_list[0];
		for (Counter = 0; Counter < 4; Counter++)
		{
			InternalAddressInt[Counter] = *(sp++);
			InternalAddressInt[Counter] &= 0xff;
		}

		/* 
		 * Convert individual digits into a single line
		 */
		sprintf(InternalAddressChar, "%d.%d.%d.%d", InternalAddressInt[0], InternalAddressInt[1], InternalAddressInt[2], InternalAddressInt[3]);

	}

	// Output
	Idk = sprintf(Buffer, "Resolved Address: %s\r\n", InternalAddressChar);
	Buffer = Buffer + Idk;

	Idk = sprintf(Buffer, "\r\nDoing Stress Test with User-Agent: %s\r\n", UserAgent);
	Buffer = Buffer + Idk;

	/*
	 * Proxy is set
	 */
	if (UsingProxy == TRUE)
	{
		Idk = sprintf(Buffer, "Using Proxy Address %s\r\n", CustomProxy);
		Buffer = Buffer + Idk;
	
		/* 
		 * Open internet handle with set proxy
		 */
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
		 * Get error, store it.
		 */
		LastError = GetLastError();

		Idk = sprintf(Buffer, "*** InternetOpen() failed!, error = 0x%.8X ( %u )\r\n", LastError, LastError);
		Buffer = Buffer + Idk;
		
		goto End;
	}

	else/* Returned Non-Zero */
	{

		IntConnectA = InternetConnectA(IntOpenHandle,
									  inputurl, // Input URL
									  INTERNET_DEFAULT_HTTP_PORT, // Port 80
									  NULL,
									  NULL,
									  INTERNET_SERVICE_HTTP, // Service Type
									  NULL,
									  NULL
									  );

		if (IntConnectA == 0)/* Failed */
		{
			
			/* 
			 * Get error, store it.
			 */
			LastError = GetLastError();

			/* Display error */
			Idk = sprintf(Buffer, "*** InternetConnectA ( %s ) failed! Error = 0x%.8X ( %u )\r\n", inputurl, LastError, LastError, LastError);
			Buffer = Buffer + Idk;

			InternetCloseHandle(IntOpenHandle);

			goto End;
		}

		else/* Returned Non-Zero */
		{
			/* 
			 * Create HTTP Request
			 */
			OpenHead = HttpOpenRequestA(IntConnectA,
							 "HEAD",
							 NULL, // Set To Nothing, NULL won't work
							 "HTTP/1.1",
							 NULL,
							 NULL,
							 INTERNET_FLAG_NO_AUTH,
							 0);
			/*
			 * Create HTTP Request
			 */
			OpenPost = HttpOpenRequestA(IntConnectA,
							 "POST",
						 	 NULL, // Set To Nothing, NULL won't work
						 	 "HTTP/1.1",
							 NULL,
						 	 NULL,
							 INTERNET_FLAG_NO_AUTH,
							 0);


			/* 
			 * Did we successfully create the requests?
			 */
			if (OpenHead && OpenPost != 0)
			{
				Idk = sprintf(Buffer, "\r\n*** Stress test is currently running...\r\n");
				Buffer = Buffer + Idk;

				SetDlgItemTextA(hWnd, OverallResults, Out);
			
				while(StressTestOn == TRUE)
				{
					if (HttpSendRequestA(OpenHead, NULL, NULL, NULL, NULL) && HttpSendRequestA(OpenPost, NULL, NULL, NULL, NULL) == TRUE)
					{
						TotalRequestsNum = TotalRequestsNum + 1;
						SetDlgItemInt(hWnd, TotalRequests, TotalRequestsNum, 0);
						OutputDebugStringA("Successfully sent request");
					}
				}

				SetDlgItemTextA(hWnd, StressTestBtn, "Start Test");

				InternetCloseHandle(IntConnectA);
				InternetCloseHandle(IntOpenHandle);
				
			}

			/*
			 * Failed to create the requests
			 */
			else
			{
				LastError = GetLastError();

				Idk = sprintf(Buffer, "*** HttpOpenRequest Failed!, error 0x%.8X ( %u )", LastError, LastError);
				Buffer = Buffer + Idk;
			}
		}
	}

	/* This Point Resets Everything */
	End:
	
	EnableWindow(InternetRequestButton, 1);

	EnableWindow(HTTPSRequestButton, 1);

	StressTestOn = FALSE; /* Just to ensure that it stays on FALSE */

	Idk = sprintf(Buffer, "\r\n*** StressTest() Thread ending...");
	Buffer = Buffer + Idk;

	SetDlgItemTextA(hWnd, StressTestBtn, "Start Test");

	SetDlgItemTextA(hWnd, OverallResults, Out);

	return 0xdeadbeef;
}

UINT __stdcall WinSockStressTest(void *)
{
	int Counter;
	int Idk;
	int InternalAddressInt[4];
	char *InternalAddressChar;
	char *NotUsed;
	char *sp;
	DWORD BufLen;
	HINTERNET IntOpenHandle;
	HINTERNET IntConnectA;
	HINTERNET httpSession;
	HINTERNET OpenHead;
	HINTERNET OpenPost;
	hostent *hostEntry;
	SOCKET s;
	sockaddr_in clientService;
	WSADATA *WinSockData;
	

	// Zero It Out
	Out[0] = '\0';


	// Just because
	InternalAddressChar = inputurl;


	/* 
	 * Beginning Thread
	 */
	Idk = sprintf(Out, "Beginning WinSockStressTest() Thread\r\n");
	Buffer = Out + Idk;


	GetDlgItemTextA(hWnd, IDD_HTTPS_URL, inputurl, sizeof(inputurl));

	if (_stricmp(inputurl, "") == 0)
	{
		
		Idk = sprintf(Buffer, "\r\n*** No Hostname Provided!\r\n");
		Buffer = Buffer + Idk;

		goto End;

	}



	// Before we start anything, must make sure that the "subject" server
	// is in the right format, in this case it has to be in e.g. "127.0.0.1"
	
	if (!isdigit(*inputurl)) /* first character is not in numerical format, must convert */
	{

		/* 
		 * Something happened, and the hostname could not be resolved
		 */
		if ( (hostEntry=gethostbyname(inputurl)) == NULL )
		{
			
			Idk = sprintf(Buffer, "\r\n*** Cannot resolve server name ( %s )\r\n", inputurl);
			Buffer = Buffer + Idk;

			// Don't Go Any Further
			goto End; 

		}

		/* 
		 * Incorrect Address Length
		 */
		if (hostEntry->h_length != 4)
		{
			Idk = sprintf(Buffer, "\r\n*** Length of internal address ( %s ) is incorrect, expected 4\r\n", hostEntry->h_length);
			Buffer = Buffer + Idk;

			// Don't Go Any Further
			goto End;
		}

		sp = hostEntry->h_addr_list[0];
		for (Counter = 0; Counter < 4; Counter++)
		{
			InternalAddressInt[Counter] = *(sp++);
			InternalAddressInt[Counter] &= 0xff;
		}
		sprintf(InternalAddressChar, "%d.%d.%d.%d", InternalAddressInt[0], InternalAddressInt[1], InternalAddressInt[2], InternalAddressInt[3]);

	}

	// Address Family/Type
	clientService.sin_family = AF_INET;

	// Address to connect to
	clientService.sin_addr.s_addr = inet_addr(InternalAddressChar);

	// Connect Using Port 80
	clientService.sin_port = htons(80);


	// Output
	Idk = sprintf(Buffer, "Resolved Address: %s\r\n", InternalAddressChar);
	Buffer = Buffer + Idk;

	// Use User Agent As Name
	hostEntry->h_name = UserAgent;

	Idk = sprintf(Buffer, "\r\nDoing Stress Test with User-Agent: %s\r\n", UserAgent);
	Buffer = Buffer + Idk;

	s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    
	if (s != 0xffffffff)
	{
		int j = 0; /* Counter */
		char GarbageData[512]; /* Garbage Data */
		char *Notused; /* (NOT USED) Placeholder for Received HTTPS Data */

		if (connect(s, (SOCKADDR*)&clientService, sizeof(clientService)) == 0)
		{
			Idk = sprintf(Buffer, "Stress Test via WinSock2 running...");
			Buffer = Buffer + Idk;

			SetDlgItemTextA(hWnd, OverallResults, Out);

			RandomString(GarbageData);

			/*
			 * Stress Test (send() garbage data) while StressTestOn is set to True
			 */
			do
			{
				int sent = sendto(s, GarbageData, sizeof(GarbageData), 0, (SOCKADDR *)&clientService, sizeof(clientService));
				if (sent != SOCKET_ERROR)
				{
					TotalRequestsNum = TotalRequestsNum + 1;
					SetDlgItemInt(hWnd, TotalRequests, TotalRequestsNum, 0);
					OutputDebugStringA("send() request successful!");
				}

				else
				{
					j = j + 1;

					/* 
					 * Failed Attempts Threshold
					 */
					if (j >= 30)
					{
						StressTestOn = FALSE;
						Idk = sprintf(Buffer, "\r\n*** Failed to send requests after 30 attempts, error = %d\r\n", WSAGetLastError);
						Buffer = Buffer + Idk;
						goto End;
					}
				}
			} while (StressTestOn == TRUE);
		}
	}

	/* This Point Resets Everything */
	End:
	
	EnableWindow(InternetRequestButton, 1);

	EnableWindow(HTTPSRequestButton, 1);

	StressTestOn = FALSE; /* Just to ensure that it stays on FALSE */

	Idk = sprintf(Buffer, "\r\n*** WinSockStressTest() Thread ending...");
	Buffer = Buffer + Idk;

	SetDlgItemTextA(hWnd, StressTestBtn, "Start Test");

	SetDlgItemTextA(hWnd, OverallResults, Out);


	return 0xdeadbeef;
}

char *RandomString(char *RandomString)
{
	int index;

	/* 
	 * Random Garbage Data Array
	 */
	char char1[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/,.-+=~`<>:";
	
	/* Do It Only Once */
	for(index = 0; index < 2; index++)
	{
		sprintf(RandomString, "%c", char1[rand() % (sizeof char1 - 1)]);
	}
	
	/* 
	 * Return Generated Garbage Data
	 */
	return RandomString;
}