#include <SexyAppFramework/HTTPTransfer.h>

#include "UpdateCheckDialog.h"
#include "WinFishApp.h"
#include "InternetManager.h"

using namespace Sexy;

Sexy::UpdateCheckDialog::UpdateCheckDialog(Image* theComponentImage, Image* theButtonComponentImage, Image* theWaitImage, int theId) :
	Dialog(theComponentImage, theButtonComponentImage, theId, true, gSexyApp->GetString("UPDATE_CHECK_TITLE"),
		gSexyApp->GetString("UPDATE_CHECK_BODY"), gSexyApp->GetString("DIALOG_BUTTON_CANCEL", DIALOG_CANCEL_STRING), BUTTONS_FOOTER)
{
	mWaitBarImage = theWaitImage;
	mCheckFinished = 0;
	mWaitBarXOffset = 0;
	mYOffset = 66;
}

Sexy::UpdateCheckDialog::~UpdateCheckDialog()
{
}

void Sexy::UpdateCheckDialog::Update()
{
	Widget::Update();

	if (!mCheckFinished && mUpdateCnt % 3 == 0)
	{
		mWaitBarXOffset = (mWaitBarXOffset + 1) % 32;
		MarkDirty();
	}

	bool aFailed = gSexyApp->mInternetManager->GetUpdateResultCode() == HTTPTransfer::RESULT_NOT_FOUND;
	bool aDone = gSexyApp->mInternetManager->GetUpdateResultCode() == HTTPTransfer::RESULT_NOT_COMPLETED;
	if (mCheckFinished)
		return;

	if (!aFailed && !aDone)
		return;

	mCheckFinished = true;
	if (aDone)
		gSexyApp->mInternetManager->StartAdLoading();

	Dialog* aDia = NULL;
	if (aFailed || gSexyApp->mInternetManager->IsUpToDate())
		aDia = ((WinFishApp*)gSexyApp)->DoDialog(mId + 20000, true,gSexyApp->GetString("UP_TO_DATE_TITLE"),
			gSexyApp->GetString("UP_TO_DATE_BODY"), gSexyApp->GetString("DIALOG_BUTTON_OK", DIALOG_OK_STRING), BUTTONS_FOOTER);
	else
		aDia = ((WinFishApp*)gSexyApp)->DoDialog(mId + 10000, true, gSexyApp->GetString("NEW_VERSION_TITLE"),
			gSexyApp->GetString("NEW_VERSION_BODY"), "", BUTTONS_YES_NO);

	aDia->Move(mX + 32, mY - 32);
}

void Sexy::UpdateCheckDialog::Draw(Graphics* g)
{
	Dialog::Draw(g);

	// The bar's y and width are locals: after ClipRect the original reads only mX back from aClipRect
	int aY = mHeight - mYOffset - mContentInsets.mBottom;
	int aWidth = mWidth - mContentInsets.mRight - mContentInsets.mLeft - 48;
	Rect aClipRect = Rect(mContentInsets.mLeft + 24, aY, aWidth, 16);

	Graphics g2(*g);

	g2.ClipRect(aClipRect);

	int aNumTiles = (aWidth / 32) + 2;
	int aX = 0;
	for (int i = 0; i < aNumTiles; i++)
	{
		g2.DrawImage(mWaitBarImage, aX - mWaitBarXOffset + aClipRect.mX, aY);
		aX += 32;
	}

	g->SetColor(Color(0, 0, 0));
	g->DrawRect(aClipRect.mX - 1, aY - 1, aWidth + 1, 16 + 1);
}

int Sexy::UpdateCheckDialog::GetPreferredHeight(int theWidth)
{
	return Dialog::GetPreferredHeight(theWidth) + 32;
}

