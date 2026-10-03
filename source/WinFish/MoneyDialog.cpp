#include <SexyAppFramework/DialogButton.h>

#include "MoneyDialog.h"
#include "WinFishCommon.h"
#include "WinFishApp.h"
#include "Res.h"

using namespace Sexy;

Sexy::MoneyDialog::MoneyDialog(WinFishApp* theApp, Image* theComponentImage, Image* theButtonComponentImage, 
	int theId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, 
	const SexyString& theDialogFooter, int theButtonMode) :
	Dialog(theComponentImage, theButtonComponentImage, theId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode)
{
	mApp = theApp;
	mEnableButtonsTimer = -1;
	DefaultDialogSettings(this);
}

Sexy::MoneyDialog::~MoneyDialog()
{
}

int Sexy::MoneyDialog::GetContentX()
{
	return mContentInsets.mLeft + mBackgroundInsets.mLeft + mX;
}

int Sexy::MoneyDialog::GetContentWidth()
{
	return mWidth - mContentInsets.mRight - mContentInsets.mLeft - mBackgroundInsets.mRight - mBackgroundInsets.mLeft;
}

// The y below the header
int Sexy::MoneyDialog::GetBodyY()
{
	return mContentInsets.mTop + mBackgroundInsets.mTop + mY + 54;
}

void Sexy::MoneyDialog::Update()
{
	Dialog::Update();
	if (mUpdateCnt == mEnableButtonsTimer)
	{
		if (mYesButton)
			mYesButton->SetDisabled(false);
		if (mNoButton)
			mNoButton->SetDisabled(false);
	}
}

void Sexy::MoneyDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::MoneyDialog::ButtonDepress(int theId)
{
	if (mUpdateCnt > mEnableButtonsTimer)
		Dialog::ButtonDepress(theId);
}

void Sexy::MoneyDialog::DisableButtons(int theEnableButtonsTimer)
{
	mEnableButtonsTimer = theEnableButtonsTimer;
	if (mYesButton)
		mYesButton->SetDisabled(true);
	if (mNoButton)
		mNoButton->SetDisabled(true);
}

void Sexy::MoneyDialog::CheckboxChecked(int theId, bool checked)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}
