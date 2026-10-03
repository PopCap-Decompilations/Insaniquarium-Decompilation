#ifndef __BETASUPPORT_H__
#define __BETASUPPORT_H__

#include <SexyAppFramework/HTTPTransfer.h>
#include <SexyAppFramework/Buffer.h>
#include <windows.h>
#include <string>
#include <map>

namespace Sexy
{
	class SexyApp;

	// PopCap beta-program login and online validation. Only reached from SexyApp::PreDisplayHook when
	// mBetaValidate is set, which never happens in the shipped game (WinFishApp also overrides the hook).
	// Original sizeof 0x1AC (operator new in SexyApp::InitPropertiesHook); one vtable slot (the destructor).
	class BetaSupport
	{
	public:
		SexyApp*				mApp;					// 0x004
		HFONT					mTahomaFont;			// 0x008 Tahoma 8pt
		HFONT					mArialFont;				// 0x00C Arial 8pt
		HFONT					mTahomaBoldFont;		// 0x010 Tahoma 10pt bold
		int						mTimerCount;			// 0x014 timer-1 ticks, drive the "Please Wait . . ." dots
		HWND					mHWnd;					// 0x018 current popup ("LoginWindow" or "ValidateWindow")
		HWND					mEditWindow;			// 0x01C read-only "Please Wait" edit of the validate window
		HWND					mUserNameEditWindow;	// 0x020 "Email Address" edit of the login window
		HWND					mPasswordEditWindow;	// 0x024 "Password" edit of the login window
		HTTPTransfer			mTransfer;				// 0x028
		bool					mCancelled;				// 0x128
		bool					mDone;					// 0x129 ends DoMessageLoop
		std::string				mCommentURL;			// 0x12C value of the ":COMMENTURL" reply line
		std::string				m0x148;					// 0x148 only constructed and destroyed in this build
		std::string				mUserName;				// 0x164 registry "Name" / the login's "Email Address"
		std::string				mPassword;				// 0x180 registry "Password"
		bool					mValidated;				// 0x19C
		std::map<std::string, Buffer> m0x1a0;			// 0x1A0 only constructed and destroyed in this build

	public:
		BetaSupport(SexyApp* theApp);
		virtual ~BetaSupport();

		void					DoMessageLoop();
		void					ReadFromRegistry();
		void					WriteToRegistry();
		bool					Login();
		bool					Validation();
		bool					Validate();

		static LRESULT CALLBACK	ValidateWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK	LoginWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}

#endif
