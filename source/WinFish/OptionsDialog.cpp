#include <SexyAppFramework/Slider.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/DialogButton.h>

#include "OptionsDialog.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Res.h"

using namespace Sexy;

Sexy::OptionsDialog::OptionsDialog(WinFishApp* theApp, bool theFlag)
	: MoneyDialog(theApp, IMAGE_DIALOG, IMAGE_DIALOGBUTTON, DIALOG_OPTIONS, true, "Options",
		"", "OK", BUTTONS_FOOTER)
{
	mApp2 = theApp;
	mFlag = theFlag;
	SetColor(3, Color(0xff, 0xff, 100));
	mRegisterButton = MakeDialogButton(0, this, "Register", NULL);
	mHelpButton = MakeDialogButton(1, this, "Help", NULL);
	mWebLinkButton = MakeDialogButton(2, this, mApp2->GetString("weblinktext", ""), NULL);

	mCheckUpdatesButton = MakeDialogButton(4, this, "Check Updates", NULL);
	mBackButton = MakeDialogButton(3, this, "Back to Main Menu", NULL);

	mMusicSlider = new Slider(IMAGE_SLIDERTRACK, IMAGE_SLIDERTHUMB, 5, this);
	float aMusicValue = (float)theApp->GetMusicVolume();
	if (aMusicValue > 1.0f) aMusicValue = 1.0f; if (aMusicValue < 0.0) aMusicValue = 0;
	mMusicSlider->SetValue(aMusicValue);

	mSFXSlider = new Slider(IMAGE_SLIDERTRACK, IMAGE_SLIDERTHUMB, 6, this);
	mSFXSlider->SetValue(theApp->GetSfxVolume());

	mFullscreenCB = MakeCheckbox(7, this, !theApp->mIsWindowed);
	mCustomCursorsCB = MakeCheckbox(8, this, theApp->mCustomCursorsEnabled);
	m3DCB = MakeCheckbox(9, this, theApp->Is3DAccelerated());

	if (mFlag)
		mBackButton->SetVisible(false);
	if (mApp2->mDontUpdate)
		mCheckUpdatesButton->SetVisible(false);
	if(mApp2->mBuildUnlocked)
		mRegisterButton->SetVisible(false);
	mWebLinkButton->SetVisible(mWebLinkButton->mLabel.size() != 0);
}

Sexy::OptionsDialog::~OptionsDialog()
{
	if (mRegisterButton)
		delete mRegisterButton;
	if (mCheckUpdatesButton)
		delete mCheckUpdatesButton;
	if (mWebLinkButton)
		delete mWebLinkButton;
	if (mMusicSlider)
		delete mMusicSlider;
	if (mSFXSlider)
		delete mSFXSlider;
	if (mFullscreenCB)
		delete mFullscreenCB;
	if (mCustomCursorsCB)
		delete mCustomCursorsCB;
	if (m3DCB)
		delete m3DCB;
	if (mHelpButton)
		delete mHelpButton;
	if (mBackButton)
		delete mBackButton;
}

void Sexy::OptionsDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mRegisterButton);
	theWidgetManager->AddWidget(mCheckUpdatesButton);
	theWidgetManager->AddWidget(mHelpButton);
	theWidgetManager->AddWidget(mMusicSlider);
	theWidgetManager->AddWidget(mSFXSlider);
	theWidgetManager->AddWidget(mCustomCursorsCB);
	theWidgetManager->AddWidget(m3DCB);
	theWidgetManager->AddWidget(mFullscreenCB);
	theWidgetManager->AddWidget(mWebLinkButton);
	theWidgetManager->AddWidget(mBackButton);
}

void Sexy::OptionsDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mRegisterButton);
	theWidgetManager->RemoveWidget(mCheckUpdatesButton);
	theWidgetManager->RemoveWidget(mHelpButton);
	theWidgetManager->RemoveWidget(mMusicSlider);
	theWidgetManager->RemoveWidget(mSFXSlider);
	theWidgetManager->RemoveWidget(mFullscreenCB);
	theWidgetManager->RemoveWidget(mCustomCursorsCB);
	theWidgetManager->RemoveWidget(m3DCB);
	theWidgetManager->RemoveWidget(mWebLinkButton);
	theWidgetManager->RemoveWidget(mBackButton);
}

void Sexy::OptionsDialog::Draw(Graphics* g)
{
	MoneyDialog::Draw(g);
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColorizeImages(true);
	g->SetColor(mColors[COLOR_LINES]);
	g->DrawString("Music", 48, 78);
	g->DrawString("Sound Fx", 48, 110);
	g->DrawString("Fullscreen", mFullscreenCB->mX - mX + 43, mFullscreenCB->mY - mY + 24);
	g->DrawString("Custom Cursors", mCustomCursorsCB->mX - mX + 43, mCustomCursorsCB->mY - mY + 24);
	g->DrawString("Hardware Acceleration", m3DCB->mX - mX + 43, m3DCB->mY - mY + 24);
	g->SetColorizeImages(false);
}

// The buttons are stacked upwards from the footer: Back and the web link get full rows; of the visible Help, Register and
// Check Updates buttons, an odd count puts the first on a full row and the last two share the next row
void Sexy::OptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	MoneyDialog::Resize(theX, theY, theWidth, theHeight);
	int aWidgetY = mYesButton->mY - 34;

	if (mBackButton->mVisible)
	{
		mBackButton->Resize(mX + mContentInsets.mLeft, aWidgetY, mWidth - mContentInsets.mRight - mContentInsets.mLeft, 33);
		aWidgetY -= 34;
	}

	int aHalfWidth = (mWidth - mContentInsets.mRight - mContentInsets.mLeft - 6) / 2;
	if (mWebLinkButton->mVisible)
	{
		mWebLinkButton->Resize(mX + mContentInsets.mLeft, aWidgetY, mWidth - mContentInsets.mRight - mContentInsets.mLeft, 33);
		aWidgetY -= 34;
	}

	ButtonWidget* aButtons[3];
	int aButtonCount = 0;
	if (mHelpButton->mVisible)
		aButtons[aButtonCount++] = mHelpButton;
	if (mRegisterButton->mVisible)
		aButtons[aButtonCount++] = mRegisterButton;
	if (mCheckUpdatesButton->mVisible)
		aButtons[aButtonCount++] = mCheckUpdatesButton;

	if (aButtonCount == 1 || aButtonCount == 3)
	{
		aButtons[0]->Resize(mX + mContentInsets.mLeft, aWidgetY, mWidth - mContentInsets.mRight - mContentInsets.mLeft, 33);
		aWidgetY -= 34;
	}
	if (aButtonCount > 1)
	{
		aButtons[aButtonCount - 2]->Resize(mX + mContentInsets.mLeft, aWidgetY, aHalfWidth, 33);
		aButtons[aButtonCount - 1]->Resize(mX + mContentInsets.mLeft + aHalfWidth + 3, aWidgetY, aHalfWidth, 33);
	}

	mMusicSlider->Resize(mX + 152, mY + 58, mWidth - 192, 33);
	mSFXSlider->Resize(mX + 152, mY + 90, mWidth - 192, 33);

	mFullscreenCB->Resize(mX + 33, mY + 128, 46, 45);
	mCustomCursorsCB->Resize(mX + 161, mY + 128, 46, 45);
	m3DCB->Resize(mX + 33, mY + 172, 46, 45);
}

int Sexy::OptionsDialog::GetPreferredHeight(int theWidth)
{
	int aPrefHght = 362;
	if (!mBackButton->mVisible)
		aPrefHght = 328;
	if (mRegisterButton->mVisible && mCheckUpdatesButton->mVisible)
		aPrefHght += 34;
	if (mWebLinkButton->mVisible)
		aPrefHght += 34;
	return aPrefHght;
}

// Overrides both MoneyDialog's and CheckboxListener's CheckboxChecked
void Sexy::OptionsDialog::CheckboxChecked(int theId, bool checked)
{
	MoneyDialog::CheckboxChecked(theId, checked);
	switch (theId)
	{
	case 7:
		if (!checked && mApp2->mForceFullscreen)
		{
			mApp2->DoDialog(8, true, "No Windowed Mode", "Windowed mode is only available if your desktop was running in either\n16 bit or 32 bit color mode when you started the game.\n\nIf you\'d like to run in Windowed mode then you need to quit the game and switch your desktop to 16 or 32 bit color mode.",
				"OK", BUTTONS_FOOTER);
			mFullscreenCB->SetChecked(true, false);
		}
		break;
	case 9:
		if (checked)
		{
			if (!mApp2->Is3DAccelerationSupported())
			{
				m3DCB->SetChecked(false, false);
				mApp2->DoDialog(14, true, "Not Supported",
					"Hardware Acceleration cannot be enabled on this computer.\n\nYour video card does not\nmeet the minimum requirements\nfor this game.",
					"OK", BUTTONS_FOOTER);
			}
			else if (!mApp2->Is3DAccelerationRecommended())
			{
				mApp2->DoDialog(14, true, "Warning",
					"Your video card may not fully support this feature.\n\nIf you experience slower performance, please disable Hardware Acceleration.",
					"OK", BUTTONS_FOOTER);
			}
		}
		break;
	}
}

void Sexy::OptionsDialog::SliderVal(int theId, double theVal)
{
	switch (theId)
	{
	case 5:
		mApp2->SetMusicVolume(theVal);
		break;
	case 6:
		mApp2->SetSfxVolume(theVal);
		if (!mSFXSlider->mDragging)
			mApp2->PlaySample(SOUND_BUTTONCLICK);
		break;
	}
}

void Sexy::OptionsDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::OptionsDialog::ButtonDepress(int theId)
{
	MoneyDialog::ButtonDepress(theId);
	switch (theId)
	{
	case 0:
		mApp2->DoRegisterDialog();
		break;
	case 1:
		mApp2->SwitchToHelpScreen(false);
		break;
	case 2:
		mApp2->OpenURL(mApp2->GetString("weblink", "http://www.popcap.com"));
		break;
	case 3:
		if (mApp2->mGameMode != GAMEMODE_VIRTUAL_TANK && mApp2->mBoard && mApp2->mBoard->NeedSaveGame())
			mApp2->DoLeaveGameDialog();
		else
			mApp2->LeaveGameBoard();
		break;
	case 4:
		mApp2->DoUpdateCheckDialog();
		break;
	}
}
