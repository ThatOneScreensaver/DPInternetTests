// DPInternetTests.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "DPInternetTests.h"
#include <process.h>
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

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

const char *Version = "DPInternetTests v1.4";
char CustomPortNum;
char CustomProxy[256];
char inputurl[512];
char UserAgent[256];
char Out[16384];
	
char *Buffer;

int Idk;

HWND InternetRequestBtn;
HWND HTTPSRequestBtn;

// Function Declarations
INT_PTR CALLBACK	InternetTest(HWND, UINT, WPARAM, LPARAM);
UINT __stdcall 		InternetTest(void *);
UINT __stdcall		HTTPSTest(void *);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if (MessageBoxExA(hWnd, "The developer is not responsible for any damages caused by this program, by clicking \"Yes\" you agree to hold complete responsiblity for your actions", "Agreement", MB_ICONWARNING | MB_YESNO, 0) == 6)
		DialogBoxParamA(hInst, MAKEINTRESOURCEA(IDD_MAIN), NULL, InternetTest, 0);
	else
		MessageBeep(MB_ICONERROR);
		exit(0);
}

INT_PTR CALLBACK InternetTest(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	tagRECT *lpRect;
	tagRECT Rect1;
	tagRECT Rect2;

	hWnd = hDlg;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		lpRect = &Rect2;

		// Set Window Title
		SetWindowTextA(hDlg, Version);
		
		LoadAcceleratorsA(hInst, MAKEINTRESOURCEA(IDC_DPINTERNETTESTS));

		// Output Console Stuff
		Idk = sprintf(Out, "======================\r\n");
		Buffer = Out + Idk;
		Idk = sprintf(Buffer, "Reporting %s\r\n", Version);
		Buffer = Buffer + Idk;

		// We can Multi-thread
		#ifdef _MT
		Idk = sprintf(Buffer, "*** Multi-Threading Support Enabled\r\n");
		Buffer = Buffer + Idk;
		goto Set;
		#endif

		Idk = sprintf(Buffer, "*** Multi-Threading Support Disabled\r\n");
		Buffer = Buffer + Idk;

		// Set Text
		Set:
		SetDlgItemTextA(hDlg, OverallResults, Out);

		hWnd = GetDesktopWindow();
		GetWindowRect(hWnd, lpRect);
		GetWindowRect(hDlg, &Rect1);
		

		// Set Window Position To Center Of Screen
		SetWindowPos(hDlg,NULL,
                 (Rect2.right + Rect2.left) / 2 - (Rect1.right - Rect1.left) / 2,
                 (Rect2.top + Rect2.bottom) / 2 - (Rect1.bottom - Rect1.top) / 2,0,0,1);

		InternetRequestBtn = GetDlgItem(hDlg, IDD_INTERNET_TEST);
		HTTPSRequestBtn = GetDlgItem(hDlg, IDD_HTTPS_TEST);
		

		return (INT_PTR)TRUE;

	case WM_COMMAND:
	
		if (LOWORD(wParam) == IDD_INTERNET_TEST)/* Internet Button */
		{
			EnableWindow(InternetRequestBtn, 0); /* Disable Button To Prevent Potential DDOSing */

			EnableWindow(HTTPSRequestBtn, 0); /* Disable Button To Prevent Potential DDOSing */

			UserAgent[0] = '\0';
			sprintf(UserAgent, Version);

			_beginthreadex(NULL, 0, &InternetTest, NULL, 0, 0);

			return 1;
		}
		if (LOWORD(wParam) == IDD_HTTPS_TEST)/* HTTPS Button */
		{
			UsingProxy = FALSE; /* Reset */

			// Zero-Out Anything That Could Still Be Resident Here
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
			
			EnableWindow(InternetRequestBtn, 0); /* Disable Button To Prevent Potential DDOSing */

			EnableWindow(HTTPSRequestBtn, 0); /* Disable Button To Prevent Potential DDOSing */

			_beginthreadex(NULL, 0, HTTPSTest, NULL, 0, 0);

			return 1;
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDM_EXIT)/* All possible exit routes */
		{
			if (MessageBoxExA(hDlg, "Exit DPInternetTests?", "DPInternetTests", MB_ICONQUESTION | MB_YESNO, 0) == 6)
			{
				DestroyWindow(hDlg);
			}
			return (INT_PTR)TRUE;
		}
		break;
	case WM_DESTROY:
		MessageBoxExA(hDlg, "Thank you so much for using my program! <3", "Thank You!", MB_ICONINFORMATION, 0);
		EndDialog(hDlg, message);
		return 1;
	}
	return 0;
}

UINT __stdcall InternetTest(void *)
{
	int Idk;
	DWORD BufLen;
	DWORD LastError;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	LPCSTR homeurl;
	
	Out[0] = '\0';

	// Feel Free To Replace This With Any URL
	// I'm just using this one bc it's the only one that could come to mind!
	homeurl = "http://www.msftconnecttest.com/redirect";

	// Begin Thread
	Idk = sprintf(Out, "Calling InternetTest() Thread\r\n");
	Buffer = Out + Idk;

	// Output
	Idk = sprintf(Buffer, "\r\nDoing full GET request\r\n");
	Buffer = Buffer + Idk;

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
	End:

	EnableWindow(InternetRequestBtn, 1); /* Re-enable button */

	EnableWindow(HTTPSRequestBtn, 1); /* Re-enable button */

	SetDlgItemTextA(hWnd, OverallResults, (LPCSTR)&Out);

	// Reason why I'm not using _endthreadex is because
	// I don't want it to restart the thread every single
	// time this function is called, just leave it running
	return 0xdeadbeef;

}

UINT __stdcall HTTPSTest(void *)
{
	int Idk;
	DWORD BufLen;
	DWORD LastError;
	HINTERNET IntOpenHandle;
	HINTERNET IntOpenUrl;
	

	// Zero It Out
	Out[0] = '\0';

	// Begin Thread
	Idk = sprintf(Out, "Calling HTTPSTest() Thread\r\n\n");
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


	if (IntOpenHandle == 0)/* Failed */
	{
		Idk = sprintf(Buffer, "*** InternetOpen() failed!\r\n");
		Buffer = Buffer + Idk;
		
		goto End;
	}

	else/* Returned Non-Zero */
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

	End:

	EnableWindow(InternetRequestBtn, 1); /* Re-enable button */

	EnableWindow(HTTPSRequestBtn, 1); /* Re-enable button */

	SetDlgItemTextA(hWnd, OverallResults, (LPCSTR)&Out);
	
	// Reason why I'm not using _endthreadex is because
	// I don't want it to restart the thread every single
	// time this function is called, just leave it running
	return 0xdeadbeef;

}