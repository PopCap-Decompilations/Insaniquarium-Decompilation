#include <SexyAppFramework/ScrollbarWidget.h>
#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/ListWidget.h>

#include "UserDialog.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "ProfileMgr.h"
#include "Res.h"

using namespace Sexy;

Sexy::UserDialog::UserDialog(WinFishApp* theApp, bool transferShells)
	: MoneyDialog(theApp, IMAGE_DIALOG, IMAGE_DIALOGBUTTON, transferShells ? DIALOG_GIVE_SHELLS : DIALOG_USER_DIALOG, true, transferShells ? "TRANSFER SHELLS" : "WHO ARE YOU?",
		"", "", BUTTONS_OK_CANCEL)
{
	mTransferShells = transferShells;
	mListWidget = new ListWidget(0, FONT_LIDDIE12, this);
	static int anListWidgetColors[][3] =
	{ {0x35, 0x2b, 0x1d},
	{0, 0, 0},
	{255, 0xe1, 0x49},
	{255, 255, 255},
	{0xbc, 0x16, 0x16} };
	mListWidget->SetColors(anListWidgetColors,5);
	mListWidget->mDrawOutline = true;
	mListWidget->mJustify = 1;
	mListWidget->mItemHeight = 15;

	mScrollbarWidget = new ScrollbarWidget(0, mListWidget);

	mRenameButton = MakeDialogButton(0, this, "Rename", FONT_JUNGLEFEVER12OUTLINE);
	mDeleteButton = MakeDialogButton(1, this, "Delete", FONT_JUNGLEFEVER12OUTLINE);
	mEditWidget = MakeEditWidget(0, this);
	mEditWidget->SetText("0", true);
	mEditWidget->mCursorPos = mEditWidget->mString.size();
	mListWidget->mScrollbar = mScrollbarWidget;

	if (!mTransferShells)
		mListWidget->AddLine("(Create a New User)", false);

	UserProfile* aCurrentProfile = theApp->mCurrentProfile;
	if (aCurrentProfile && !mTransferShells)
		mListWidget->SetSelect(mListWidget->AddLine(aCurrentProfile->mUserName, false));


	UserProfilesMap& aProfilesMap = theApp->mProfileMgr->mProfilesMap;
	for (UserProfilesMap::iterator it = aProfilesMap.begin(); it != aProfilesMap.end(); ++it)
	{
		if (!aCurrentProfile || it->second.mUserName != aCurrentProfile->mUserName)
			mListWidget->AddLine(it->second.mUserName, false);
	}

	if (aProfilesMap.size() < 8)
		mScrollbarWidget->SetVisible(false);
	if (mTransferShells)
	{
		mRenameButton->SetVisible(false);
		mDeleteButton->SetVisible(false);
	}
	else
		mEditWidget->SetVisible(false);
}

Sexy::UserDialog::~UserDialog()
{
	delete mListWidget;
	delete mScrollbarWidget;
	delete mRenameButton;
	delete mDeleteButton;
	delete mEditWidget;
}

void Sexy::UserDialog::Draw(Graphics* g)
{
	MoneyDialog::Draw(g);
	if (!mTransferShells)
		return;

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(0xffffff));

	g->DrawString("Transfer Shells to User", GetContentX() - mX + 70, GetBodyY() - mY - 7);
	g->DrawString("Transfer Amount", GetContentX() - mX, mEditWidget->mY - mY + 17);

	g->DrawImage(IMAGE_HELPSHELL, mEditWidget->mX - mX - 35, mEditWidget->mY - mY + 3);
	DrawEditWidgetBox(g, mEditWidget);
}

void Sexy::UserDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mListWidget);
	theWidgetManager->AddWidget(mScrollbarWidget);
	theWidgetManager->AddWidget(mDeleteButton);
	theWidgetManager->AddWidget(mRenameButton);
	theWidgetManager->AddWidget(mEditWidget);
	if (mTransferShells)
		theWidgetManager->SetFocus(mEditWidget);
}

void Sexy::UserDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	MoneyDialog::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mListWidget);
	theWidgetManager->RemoveWidget(mScrollbarWidget);
	theWidgetManager->RemoveWidget(mDeleteButton);
	theWidgetManager->RemoveWidget(mRenameButton);
	theWidgetManager->RemoveWidget(mEditWidget);
}

void Sexy::UserDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	MoneyDialog::Resize(theX, theY, theWidth, theHeight);

	int aListX = GetContentX() + 10;
	int aListY = GetBodyY();
	int aWidth = GetContentWidth() - 20;
	int aScrollbarWidth = 16;
	if (!mScrollbarWidget->mVisible)
		aScrollbarWidth = 0;

	int aListWidth = aWidth - aScrollbarWidth;
	mListWidget->Resize(aListX, aListY, aListWidth, 130);
	mScrollbarWidget->ResizeScrollbar(aListWidth + aListX, aListY, aScrollbarWidth, 130);
	mRenameButton->Layout(4355, mYesButton);
	mDeleteButton->Layout(4355, mNoButton);
	mEditWidget->Layout(464, mYesButton, aListX + 170, -10, aWidth - 170, 24);
}
int Sexy::UserDialog::GetPreferredHeight(int theWidth)
{
	return MoneyDialog::GetPreferredHeight(theWidth) + 190;
}

void Sexy::UserDialog::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::UserDialog::ButtonDepress(int theId)
{
	MoneyDialog::ButtonDepress(theId);

	SexyString aStr = GetSelectedUserName();

	if (aStr.size() > 0)
	{
		switch (theId)
		{
		case 0:
			mApp->DoRenameDialog(aStr);
			break;
		case 1:
			mApp->DoDeleteWarningDialog(aStr);
			break;
		}
	}
}

void Sexy::UserDialog::ListClicked(int theId, int theIdx, int theClickCount)
{
	if (mTransferShells)
	{
		mListWidget->SetSelect(theIdx);
		return;
	}
	if (theIdx == 0)
	{
		mApp->DoNewUserDialog();
		return;
	}
	mListWidget->SetSelect(theIdx);
	if (theClickCount == 2)
		mApp->UserDialogOkPressed(true);
}

void Sexy::UserDialog::EditWidgetText(int theId, const SexyString& theString)
{
	mApp->ButtonDepress(mId + 2000);
}

bool Sexy::UserDialog::AllowChar(int theId, SexyChar theChar)
{
	return isdigit(theChar) != 0;
}

SexyString Sexy::UserDialog::GetSelectedUserName()
{
	if (!mTransferShells && mListWidget->mSelectIdx < 1)
		return "";
	if (mListWidget->mSelectIdx >= 0 && mListWidget->mSelectIdx < mListWidget->GetLineCount())
		return mListWidget->GetStringAt(mListWidget->mSelectIdx);
	return "";
}

int Sexy::UserDialog::GetShellsValue()
{
	int aVal = atoi(mEditWidget->mString.c_str());
	return aVal < 0 ? 0 : aVal;
}

void Sexy::UserDialog::RemoveSelectedUser()
{
	int aSelIdx = mListWidget->mSelectIdx;
	mListWidget->RemoveLine(aSelIdx);
	aSelIdx--;
	if (aSelIdx < 1)
		aSelIdx = 1;
	if (mListWidget->GetLineCount() > 1)
		mListWidget->SetSelect(aSelIdx);
}

void Sexy::UserDialog::RenameSelectedUser(const SexyString& theNewName)
{
	int aSelIdx = mListWidget->mSelectIdx;
	if (aSelIdx >= 1)
		mListWidget->SetLine(aSelIdx, theNewName);
}

