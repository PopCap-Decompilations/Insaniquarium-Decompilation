#include "BetaSupport.h"
#include "SexyApp.h"
#include <SexyAppFramework/Common.h>
#include <shellapi.h>

using namespace Sexy;

// 0x413AD0 (hWnd in EBX). The original computes the y position as top - (int)(dh * -0.382) (fmul by the
// double -0.382 at 0x5C3C30, then sub), which equals the form below; SexyAppBase::MakeWindow uses the same.
static void CenterWindow(HWND theWindow)
{
	RECT aDesktopRect;
	::SystemParametersInfoA(SPI_GETWORKAREA, 0, &aDesktopRect, 0);

	RECT aRect;
	::GetWindowRect(theWindow, &aRect);

	int aWidth = aRect.right - aRect.left;
	int aHeight = aRect.bottom - aRect.top;

	::MoveWindow(theWindow,
		aDesktopRect.left + ((aDesktopRect.right - aDesktopRect.left) - aWidth)/2,
		aDesktopRect.top + (int) (((aDesktopRect.bottom - aDesktopRect.top) - aHeight)*0.382),
		aWidth, aHeight, FALSE);
}

BetaSupport::BetaSupport(SexyApp* theApp)
{
	mApp = theApp;

	mTimerCount = 0;
	mEditWindow = NULL;
	mCancelled = false;
	mDone = true;
	mHWnd = NULL;

	HWND aDesktopWnd = ::GetDesktopWindow();
	HDC aDC = ::GetDC(aDesktopWnd);

	mTahomaFont = ::CreateFontA(-MulDiv(8, 96, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Tahoma");

	mArialFont = ::CreateFontA(-MulDiv(8, 96, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Arial");

	mTahomaBoldFont = ::CreateFontA(-MulDiv(10, 96, 72), 0, 0, 0, FW_BOLD, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Tahoma");

	::ReleaseDC(aDesktopWnd, aDC);

	mValidated = false;
}

BetaSupport::~BetaSupport()
{
	if (mHWnd != NULL)
	{
		DoMessageLoop();
		::DestroyWindow(mHWnd);
		mHWnd = NULL;
	}

	::DeleteObject(mTahomaFont);
	::DeleteObject(mArialFont);
	::DeleteObject(mTahomaBoldFont);

	DoMessageLoop();
}

void BetaSupport::DoMessageLoop()
{
	MSG msg;
	while ((!mDone) && (::GetMessageA(&msg, NULL, 0, 0) > 0))
	{
		if (!::IsDialogMessageA(mHWnd, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessageA(&msg);
		}
	}
}

LRESULT CALLBACK BetaSupport::ValidateWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BetaSupport* aBetaSupport = (BetaSupport*) ::GetWindowLongA(hWnd, GWL_USERDATA);

	switch (uMsg)
	{
	case WM_CLOSE:
		aBetaSupport->mTransfer.Abort();
		return 0;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			aBetaSupport->mTransfer.Abort();
			aBetaSupport->mCancelled = true;
		}
		break;
	case WM_TIMER:
		if (aBetaSupport != NULL)
		{
			if (wParam == 0)
			{
				// GetResultCode is called twice, as in the original
				if ((aBetaSupport->mTransfer.GetResultCode() != HTTPTransfer::RESULT_NOT_COMPLETED) &&
					(aBetaSupport->mTransfer.GetResultCode() != HTTPTransfer::RESULT_NOT_STARTED))
					aBetaSupport->mDone = true;
			}
			else if (wParam == 1)
			{
				aBetaSupport->mTimerCount++;

				std::string aNewString = "Please Wait";

				for (int i = 0; i < (aBetaSupport->mTimerCount % 10); i++)
					aNewString += " .";

				::SetWindowTextA(aBetaSupport->mEditWindow, aNewString.c_str());
			}
		}
		break;
	}

	return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK BetaSupport::LoginWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BetaSupport* aBetaSupport = (BetaSupport*) ::GetWindowLongA(hWnd, GWL_USERDATA);

	switch (uMsg)
	{
	case WM_PAINT:
		{
			RECT aClientRect;
			::GetClientRect(hWnd, &aClientRect);

			PAINTSTRUCT ps;
			::BeginPaint(hWnd, &ps);

			HGDIOBJ anOldFont = ::SelectObject(ps.hdc, aBetaSupport->mTahomaFont);
			::SetBkMode(ps.hdc, TRANSPARENT);
			::SetTextColor(ps.hdc, RGB(0, 0, 0));

			const char* aString = "Email Address";
			::TextOutA(ps.hdc, 6, 8, aString, strlen(aString));
			aString = "Password";
			::TextOutA(ps.hdc, 6, 53, aString, strlen(aString));

			::SelectObject(ps.hdc, aBetaSupport->mArialFont);

			HBRUSH aBrush = ::CreateSolidBrush(RGB(0x71, 0x73, 0x97));
			RECT aLineRect;
			aLineRect.left = 4;
			aLineRect.top = 98;
			aLineRect.right = aClientRect.right - 4;
			aLineRect.bottom = 99;
			::FillRect(ps.hdc, &aLineRect, aBrush);
			::DeleteObject(aBrush);

			::SetTextColor(ps.hdc, RGB(0x68, 0x1E, 0x1E));
			RECT aTextRect;
			aTextRect.left = 8;
			aTextRect.top = 108;
			aTextRect.right = 294;
			aTextRect.bottom = 300;
			::DrawTextA(ps.hdc,
				"This product is intended only for authorized members of\n"
				"the PopCap Beta program.  Any unauthorized distribution\n"
				"should be reported to beta@popcap.com.",
				-1, &aTextRect, 0);

			::SelectObject(ps.hdc, anOldFont);
			::EndPaint(hWnd, &ps);
		}
		break;
	case WM_CLOSE:
		// Falls through to DefWindowProc, as in the original
		aBetaSupport->mCancelled = true;
		aBetaSupport->mDone = true;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			if (::GetFocus() == aBetaSupport->mUserNameEditWindow)
				::SetFocus(aBetaSupport->mPasswordEditWindow);
			else
				aBetaSupport->mDone = true;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			aBetaSupport->mCancelled = true;
			aBetaSupport->mDone = true;
		}
		break;
	}

	return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void BetaSupport::ReadFromRegistry()
{
	if (!mApp->mPlayingDemoBuffer)
	{
		HKEY aKey;
		if (::RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\PopCap\\Beta", 0, KEY_READ, &aKey) == ERROR_SUCCESS)
		{
			BYTE aData[256];
			DWORD aLength;
			DWORD aType;

			aLength = 256;
			if (::RegQueryValueExA(aKey, "Name", 0, &aType, aData, &aLength) == ERROR_SUCCESS)
				mUserName = (const char*) aData;

			aLength = 256;
			if (::RegQueryValueExA(aKey, "Password", 0, &aType, aData, &aLength) == ERROR_SUCCESS)
				mPassword = (const char*) aData;

			::RegCloseKey(aKey);
		}
	}
}

void BetaSupport::WriteToRegistry()
{
	if (!mApp->mPlayingDemoBuffer)
	{
		HKEY aKey;
		DWORD aDisposition;
		if (::RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\PopCap\\Beta", 0, (LPSTR) "Key", REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &aKey, &aDisposition) == ERROR_SUCCESS)
		{
			// Written without the terminating NUL, as in the original
			::RegSetValueExA(aKey, "Name", 0, REG_SZ, (const BYTE*) mUserName.c_str(), mUserName.length());
			::RegSetValueExA(aKey, "Password", 0, REG_SZ, (const BYTE*) mPassword.c_str(), mPassword.length());
			::RegCloseKey(aKey);
		}
	}
}

bool BetaSupport::Login()
{
	mCancelled = false;
	mDone = false;

	if (mApp->mPlayingDemoBuffer)
	{
		// sic: syncs a local, not mCancelled
		bool aCancelled = false;
		mApp->DemoSyncBool(&aCancelled);
		return !aCancelled;
	}

	ReadFromRegistry();

	WNDCLASSA wc;
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = ::LoadIcon(NULL, IDI_QUESTION);
	wc.hInstance = gHInstance;
	wc.lpfnWndProc = LoginWindowProc;
	wc.lpszClassName = "LoginWindow";
	wc.lpszMenuName = NULL;
	::RegisterClassA(&wc);

	RECT aRect;
	aRect.left = 0;
	aRect.top = 0;
	aRect.right = 306;
	aRect.bottom = 162;

	DWORD aWindowStyle = WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	::AdjustWindowRect(&aRect, aWindowStyle, FALSE);

	mHWnd = ::CreateWindowA("LoginWindow", "PopCap Beta Login...",
		aWindowStyle,
		64, 64,
		aRect.right - aRect.left,
		aRect.bottom - aRect.top,
		NULL,
		NULL,
		gHInstance,
		0);
	::SetWindowLongA(mHWnd, GWL_USERDATA, (LONG) this);

	// The login window gets both timers too, but ignores WM_TIMER
	::SetTimer(mHWnd, 0, 20, NULL);
	::SetTimer(mHWnd, 1, 1000, NULL);

	mUserNameEditWindow = ::CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", mUserName.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
		6, 20, 204, 22,
		mHWnd,
		NULL,
		gHInstance,
		0);
	::SendMessageA(mUserNameEditWindow, WM_SETFONT, (WPARAM) mTahomaFont, 0);

	mPasswordEditWindow = ::CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", mPassword.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_PASSWORD,
		6, 65, 204, 22,
		mHWnd,
		NULL,
		gHInstance,
		0);
	::SendMessageA(mPasswordEditWindow, WM_SETFONT, (WPARAM) mTahomaFont, 0);

	HWND aLoginButton = ::CreateWindowA("BUTTON", "Login",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
		222, 6, 72, 20,
		mHWnd,
		(HMENU) IDOK,
		gHInstance,
		0);
	::SendMessageA(aLoginButton, WM_SETFONT, (WPARAM) mTahomaFont, 0);

	HWND aCancelButton = ::CreateWindowA("BUTTON", "Cancel",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
		222, 32, 72, 20,
		mHWnd,
		(HMENU) IDCANCEL,
		gHInstance,
		0);
	::SendMessageA(aCancelButton, WM_SETFONT, (WPARAM) mTahomaFont, 0);

	CenterWindow(mHWnd);

	if (mPassword.length() > 0)
		::SetFocus(aLoginButton);
	else
		::SetFocus(mUserNameEditWindow);

	::ShowWindow(mHWnd, SW_NORMAL);

	DoMessageLoop();

	if (!mCancelled)
	{
		char aString[260];

		::GetWindowTextA(mUserNameEditWindow, aString, 256);
		mUserName = aString;

		::GetWindowTextA(mPasswordEditWindow, aString, 256);
		mPassword = aString;
	}

	::DestroyWindow(mHWnd);
	mHWnd = NULL;

	mApp->DemoSyncBool(&mCancelled);
	return !mCancelled;
}

bool BetaSupport::Validation()
{
	mDone = false;
	mCancelled = false;

	std::string aURL = "http://www.popcap.com/beta_validate.php?prod=" + mApp->mProdName +
		"&version=" + mApp->mProductVersion +
		"&username=" + URLEncode(mUserName) +
		"&pass=" + URLEncode(mPassword);

	mTransfer.Get(aURL);

	WNDCLASSA wc;
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = ::LoadIcon(NULL, IDI_INFORMATION);
	wc.hInstance = gHInstance;
	wc.lpfnWndProc = ValidateWindowProc;
	wc.lpszClassName = "ValidateWindow";
	wc.lpszMenuName = NULL;
	::RegisterClassA(&wc);

	RECT aRect;
	aRect.left = 0;
	aRect.top = 0;
	aRect.right = 240;
	aRect.bottom = 64;

	DWORD aWindowStyle = WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	::AdjustWindowRect(&aRect, aWindowStyle, FALSE);

	mHWnd = ::CreateWindowA("ValidateWindow", "Validating Beta",
		aWindowStyle,
		64, 64,
		aRect.right - aRect.left,
		aRect.bottom - aRect.top,
		NULL,
		NULL,
		gHInstance,
		0);
	::SetWindowLongA(mHWnd, GWL_USERDATA, (LONG) this);

	// Check every 20ms to see if the transfer has completed
	::SetTimer(mHWnd, 0, 20, NULL);

	// Every second we should change the edit text
	::SetTimer(mHWnd, 1, 1000, NULL);

	mEditWindow = ::CreateWindowA("EDIT", "Please Wait",
		WS_VISIBLE | WS_CHILD | ES_READONLY,
		20, 10, 224, 24,
		mHWnd,
		NULL,
		gHInstance,
		0);
	::SendMessageA(mEditWindow, WM_SETFONT, (WPARAM) mTahomaBoldFont, 0);

	HWND anAbortButton = ::CreateWindowA("BUTTON", "Abort",
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		72, 36, 96, 22,
		mHWnd,
		(HMENU) IDCANCEL,
		gHInstance,
		0);
	::SendMessageA(anAbortButton, WM_SETFONT, (WPARAM) mTahomaFont, 0);

	CenterWindow(mHWnd);
	::ShowWindow(mHWnd, SW_NORMAL);

	DoMessageLoop();

	::DestroyWindow(mHWnd);
	mHWnd = NULL;

	if (mTransfer.GetResultCode() == HTTPTransfer::RESULT_DONE)
	{
		std::string aContent = mTransfer.GetContent();

		Buffer aBuffer;
		aBuffer.SetData((uchar*) aContent.c_str(), aContent.length());

		std::string aResult = aBuffer.ReadLine();

		bool aParseError = false;
		std::string anError;
		std::string anUpdateURL;

		while (!aBuffer.AtEnd())
		{
			std::string aLine = aBuffer.ReadLine();
			std::string aValue;

			int aSpacePos = aLine.find(' ');
			if (aSpacePos != -1)
			{
				aValue = aLine.substr(aSpacePos + 1);
				aLine = aLine.substr(0, aSpacePos);
			}

			if (aLine == ":ERROR")
				anError = aValue;
			else if (aLine == ":UPDATEURL")
				anUpdateURL = aValue;
			else if (aLine == ":COMMENTURL")
				mCommentURL = aValue;
			else if (aLine.length() > 0)
				aParseError = true;
		}

		if (((aResult != "SUCCEEDED") && (aResult != "FAILED")) || (aParseError))
		{
			::MessageBoxA(NULL, "Unable to parse Beta Validation results! Check your internet connection and try again",
				"Validation Failed!", MB_ICONERROR);
			return false;
		}

		if (aResult == "SUCCEEDED")
			return true;

		if (aResult == "FAILED")
		{
			if ((anError == "Update Needed") && (anUpdateURL.substr(0, 7) == "http://"))
			{
				if (::MessageBoxA(NULL, "A new version of this product is now available. Click OK to open a page to download it from.",
					"Update Required!", MB_ICONWARNING) == IDOK)
				{
					::ShellExecuteA(NULL, "open", anUpdateURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
					return false;
				}
				// sic: any other answer falls through to return true
			}
			else
			{
				if (anError == "Server Error")
					::MessageBoxA(NULL, "Internal Server Error! Try again later.", "Server Error", MB_ICONINFORMATION);
				else if (anError == "Beta Discontinued")
					::MessageBoxA(NULL, "This product has been removed from the PopCap Beta program.\n"
						"Thanks for helping us test during the beta period!", "Beta Discontinued", MB_ICONINFORMATION);
				else if (anError == "Invalid Username")
					::MessageBoxA(NULL, "Unable to locate a PopCap Beta account for the supplied user name.",
						"Invalid User Name!", MB_ICONERROR);
				else if (anError == "Account Disabled")
					::MessageBoxA(NULL, "This PopCap Beta account has been disabled!", "Account Disabled!", MB_ICONERROR);
				else if (anError == "Invalid Password")
					::MessageBoxA(NULL, "The password you supplied did not match this account.", "Invalid Password!", MB_ICONERROR);
				else
					::MessageBoxA(NULL, anError.c_str(), "Error!", MB_ICONERROR);

				return false;
			}
		}

		return true;
	}
	else if (mTransfer.GetResultCode() != HTTPTransfer::RESULT_ABORTED)
	{
		::MessageBoxA(NULL, "Beta Validation failed! Check your internet connection and try again.",
			"Validation Failed!", MB_ICONERROR);
	}

	return false;
}

bool BetaSupport::Validate()
{
	if (!Login())
		return false;

	mApp->DemoSyncString(&mUserName);
	mApp->DemoSyncString(&mPassword);

	mApp->mUserName = mUserName;

	if (!Validation())
		return false;

	WriteToRegistry();

	mValidated = true;
	return true;
}
