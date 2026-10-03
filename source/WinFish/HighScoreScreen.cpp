#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/D3DInterface.h>
#include <SexyAppFramework/DDInterface.h>

#include "HighScoreScreen.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "BubbleMgr.h"
#include "HighScoreMgr.h"
#include "ProfileMgr.h"
#include "Res.h"

using namespace Sexy;

int Sexy::gHighScoreScreenPage = 1;

const double PI = 3.141590118408203;

Sexy::HighScoreScreen::HighScoreScreen(WinFishApp* theApp)
{
	mApp = theApp;

	mBubbleMgr = new BubbleMgr();
	mBubbleMgr->SetBubbleBounds(Rect(0, 60, 640, 420));
	mBubbleMgr->SetBubbleFishBounds(Rect(0, 80, 640, 350));
	mBubbleMgr->SetBubbleConfig(20, 3);
	mBubbleMgr->SetBubbleFishConfig(3, 3);
	mBubbleMgr->UpdateALot();

	mPage = gHighScoreScreenPage;
	mMenuButton = MakeDialogButton2(4, this, "Menu", IMAGE_MAINBUTTON);
	mMenuButton->Resize(525, 4, 80, mMenuButton->mHeight);

	mAdvButton = MakeDialogButton2(PAGE_ADV, this, "Adventure", IMAGE_MAINBUTTON);

	mTimeTrialButton = MakeDialogButton2(PAGE_TIME, this, "Time Trial", IMAGE_MAINBUTTON);

	mChallengeButton = MakeDialogButton2(PAGE_CHAL, this, "Challenge", IMAGE_MAINBUTTON);

	mPersonalButton = MakeDialogButton2(PAGE_PERSONAL, this, "Personal", IMAGE_MAINBUTTON);

	mAdvButton->Resize(20, 470 - mAdvButton->mHeight, 120, mAdvButton->mHeight);
	mTimeTrialButton->Layout(LAY_SameBottom | LAY_Right | LAY_SameHeight | LAY_SameWidth,mAdvButton, 40);
	mChallengeButton->Layout(LAY_SameBottom | LAY_Right | LAY_SameHeight | LAY_SameWidth, mTimeTrialButton, 40);
	mPersonalButton->Layout(LAY_SameBottom | LAY_Right | LAY_SameHeight | LAY_SameWidth, mChallengeButton, 40);
}

Sexy::HighScoreScreen::~HighScoreScreen()
{
	gHighScoreScreenPage = mPage;
	delete mBubbleMgr;
	delete mMenuButton;
	delete mAdvButton;
	delete mTimeTrialButton;
	delete mChallengeButton;
	delete mPersonalButton;
}

void Sexy::HighScoreScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mMenuButton);
	theWidgetManager->AddWidget(mAdvButton);
	theWidgetManager->AddWidget(mTimeTrialButton);
	theWidgetManager->AddWidget(mChallengeButton);
	theWidgetManager->AddWidget(mPersonalButton);
}

void Sexy::HighScoreScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMenuButton);
	theWidgetManager->RemoveWidget(mAdvButton);
	theWidgetManager->RemoveWidget(mTimeTrialButton);
	theWidgetManager->RemoveWidget(mChallengeButton);
	theWidgetManager->RemoveWidget(mPersonalButton);
}

void Sexy::HighScoreScreen::Update()
{
	Widget::Update();
	mBubbleMgr->Update();
	MarkDirty();
}

void Sexy::HighScoreScreen::Draw(Graphics* g)
{
	int aLumin = 107;
	for (int i = 0; i < 480; i += 4)
	{
		g->SetColor(Color(mApp->HSLToRGB(151, 138, aLumin)));
		g->FillRect(0, i, mWidth, 4);
		if (--aLumin < 50)
			aLumin = 50;
	}
	mBubbleMgr->Draw(g);
	DrawWaves(g, 60, mUpdateCnt);

	SexyString aGameModeStr;
	switch (mPage)
	{
	case PAGE_ADV:
		aGameModeStr = "Adventure";
		DrawGameModePage(g);
		break;
	case PAGE_TIME:
		aGameModeStr = "Time Trial";
		DrawGameModePage(g);
		break;
	case PAGE_CHAL:
		aGameModeStr = "Challenge";
		DrawGameModePage(g);
		break;
	case PAGE_PERSONAL:
		aGameModeStr = "Personal Records";
		DrawPersonalRecordsPage(g);
		break;
	default:
		break;
	}

	g->SetFont(FONT_CONTINUUMBOLD14);
	g->SetColor(Color(0xFFFFFF));
	DrawStringWithOutline(g, aGameModeStr, 320 - g->GetFont()->StringWidth(aGameModeStr) / 2, 60, FONT_CONTINUUMBOLD14OUTLINE, 0);
	g->DrawImageBox(Rect(0, 0, 640, IMAGE_SCREENTITLE->mHeight), IMAGE_SCREENTITLE);

	g->DrawImageBox(Rect(mMenuButton->mX - 1, mMenuButton->mY - 1, mMenuButton->mWidth + 2, IMAGE_SCREENTITLEHOLE->mHeight), IMAGE_SCREENTITLEHOLE);

	g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
	g->SetColor(Color(255, 200, 0, 255));
	WriteCenteredLine(g, 25, "Hall of Fame");
}

// With the 3D renderer each line bobs by 3 * sin(angle): the update count at 3 degrees per update plus
// the line's y at 2 degrees per pixel
void Sexy::HighScoreScreen::DrawGameModePage(Graphics* g)
{
	bool isTimeTrial = mPage == PAGE_TIME;
	bool isAdventure = mPage == PAGE_ADV;
	int aLowerRowY = 280;
	bool showTank5 = false;

	if (mPage == PAGE_ADV && mApp->mCurrentProfile->mFinishedGame)
	{
		showTank5 = true;
		aLowerRowY = 240;
	}

	bool isAccel = mApp->Is3DAccelerated();
	D3DInterface* anInterface = mApp->mDDInterface->mD3DInterface;
	SexyTransform2D aTransform;
	HighScoreMgr* aMgr = mApp->mHighScoreMgr;

	for (int aTank = 1; aTank <= 4; aTank++)
	{
		HighScoreList* aList = &aMgr->mChallengeScores[aTank - 1];
		if (isTimeTrial)
			aList = &aMgr->mTimeTrialScores[aTank - 1];

		int aY = 100;
		int aX = (aTank & 1) != 0 ? 60 : 380;
		if (aTank >= 3)
			aY = aLowerRowY;

		g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
		g->SetColor(Color(255, 255, 100));

		SexyString aStr = StrFormat("Tank %d", aTank);
		int aStrWidth = g->GetFont()->StringWidth(aStr);

		if (isAccel)
		{
			float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aY + aY) / 180.0);
			aRad = (float)sin(aRad);
			aTransform.m12 = (float)(aRad * 3.0);
			anInterface->PushTransform(aTransform, true);
		}
		g->DrawString(aStr, aX + (200 - aStrWidth) / 2, aY);

		if (isAccel)
			anInterface->PopTransform();

		aY += g->GetFont()->GetHeight();

		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
		g->SetColor(Color(255, 255, 255));

		// One row per entry of the tank's list, at most 5. The Adventure page walks the Challenge list for the
		// row count but shows each level's best time.
		int aLevel = 1;
		for (HighScoreList::iterator it = aList->begin(); it != aList->end() && aLevel - 1 < 5; ++it, ++aLevel)
		{
			if (isAccel)
			{
				float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aY + aY) / 180.0);
				aRad = (float)sin(aRad);
				aTransform.m12 = (float)(aRad * 3.0);
				anInterface->PushTransform(aTransform, true);
			}

			SetVaryingBlueColor(g, aY);

			HighScoreEntry* anEntry;
			if (isAdventure)
			{
				anEntry = &aMgr->GetPerLevelEntry(aTank, aLevel);
				g->DrawString(StrFormat("%d-%d", aTank, aLevel), aX - 30, aY);
			}
			else
				anEntry = &*it;

			g->DrawString(anEntry->mUserName, aX, aY);

			if (isTimeTrial)
				aStr = StrFormat("%d", anEntry->mScore);
			else
				aStr = GetPlayTimeString(anEntry->mScore);

			g->DrawString(aStr, aX - g->GetFont()->StringWidth(aStr) + 200, aY);
			aY += 15;

			if (isAccel)
				anInterface->PopTransform();
		}
	}

	if (!showTank5)
		return;

	if (isAccel)
	{
		float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * (365 + 365) / 180.0);
		aRad = (float)sin(aRad);
		aTransform.m12 = (float)(aRad * 3.0);
		anInterface->PushTransform(aTransform, true);
	}

	g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
	g->SetColor(Color(255, 255, 100));
	SexyString aStr = "Tank 5 (Surviving Pets)";
	g->DrawString(aStr, 320 - g->GetFont()->StringWidth(aStr) / 2, 365);

	if (isAccel)
		anInterface->PopTransform();

	int aY = g->GetFont()->GetHeight() + 365;
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(255, 255, 255));

	HighScoreList* aList = aMgr->GetPerLevelScoresList(5, 1);

	int i = 0;
	for (HighScoreList::iterator it = aList->begin(); it != aList->end() && i < 3; ++it, ++i)
	{
		if (isAccel)
		{
			float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aY + aY) / 180.0);
			aRad = (float)sin(aRad);
			aTransform.m12 = (float)(aRad * 3.0);
			anInterface->PushTransform(aTransform, true);
		}

		SetVaryingBlueColor(g, aY);
		HighScoreEntry* anEntry = &*it;
		g->DrawString(anEntry->mUserName, 220, aY);

		aStr = StrFormat("%d / 18", anEntry->mScore);
		g->DrawString(aStr, 420 - g->GetFont()->StringWidth(aStr), aY);
		aY += 15;

		if (isAccel)
			anInterface->PopTransform();
	}
}

void Sexy::HighScoreScreen::DrawPersonalRecordsPage(Graphics* g)
{
	bool isAccel = mApp->Is3DAccelerated();
	D3DInterface* anInterface = mApp->mDDInterface->mD3DInterface;
	SexyTransform2D aTransform;
	SexyString aStr;
	g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
	g->SetColor(Color(255, 255, 100));
	if (isAccel)
	{
		float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * (110 + 110) / 180.0);
		aRad = (float)sin(aRad);
		aTransform.m12 = (float)(aRad * 3.0);
		anInterface->PushTransform(aTransform, true);
	}

	aStr = "Adventure";
	int aStrWdth = g->GetFont()->StringWidth(aStr);
	g->DrawString(aStr, 320 - aStrWdth / 2, 110);

	if (isAccel)
	{
		anInterface->PopTransform();
		float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * (290 + 290) / 180.0);
		aRad = (float)sin(aRad);
		aTransform.m12 = (float)(aRad * 3.0);
		anInterface->PushTransform(aTransform, true);
	}

	aStr = "Time Trial";
	aStrWdth = g->GetFont()->StringWidth(aStr);
	g->DrawString(aStr, 173 - aStrWdth / 2, 290);

	aStr = "Challenge";
	aStrWdth = g->GetFont()->StringWidth(aStr);
	g->DrawString(aStr, 466 - aStrWdth / 2, 290);

	if (isAccel)
		anInterface->PopTransform();

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(0xffffff));

	int aXPosText = 20;
	int aTextYPos = 140;
	for (int aTankNum = 1; aTankNum <= 4; aTankNum++)
	{
		aTextYPos = 140;
		for (int aLevelNum = 1; aLevelNum <= 5; aLevelNum++)
		{
			if (isAccel)
			{
				float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aTextYPos + aTextYPos) / 180.0);
				aRad = (float)sin(aRad);
				aTransform.m12 = (float)(aRad * 3.0);
				anInterface->PushTransform(aTransform, true);
			}
			SetVaryingBlueColor(g, aTextYPos);
			g->SetColor(Color(0xffff88));

			g->DrawString(StrFormat("%d-%d", aTankNum, aLevelNum), aXPosText, aTextYPos);

			int aScore = mApp->mCurrentProfile->GetAdventureScore(aTankNum, aLevelNum);

			SexyString aScoreStr;
			if (aScore >= 0)
				aScoreStr = GetPlayTimeString(aScore);
			else
				aScoreStr = "-----";

			SetVaryingBlueColor(g, aTextYPos);
			int aScoreStrWdth = g->GetFont()->StringWidth(aScoreStr);
			g->DrawString(aScoreStr, aXPosText + 100 - aScoreStrWdth, aTextYPos);

			aTextYPos += 15;

			if (isAccel)
				anInterface->PopTransform();
		}
		aXPosText += 166;
	}

	int aFinalBossScore = mApp->mCurrentProfile->GetAdventureScore(5, 1);

	if (aFinalBossScore >= 0)
	{
		aTextYPos += 5;
		if (isAccel)
		{
			float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aTextYPos + aTextYPos) / 180.0);
			aRad = (float)sin(aRad);
			aTransform.m12 = (float)(aRad * 3.0);
			anInterface->PushTransform(aTransform, true);
		}
		SetVaryingBlueColor(g, aTextYPos);
		g->SetColor(Color(0xffff88));

		g->DrawString("Final Boss", 220, aTextYPos);

		SetVaryingBlueColor(g, aTextYPos);
		g->DrawString(StrFormat("%d/18 Pets Saved", aFinalBossScore), 320, aTextYPos);

		if (isAccel)
			anInterface->PopTransform();
	}

	for (int i = 0; i < 2; i++)
	{
		aTextYPos = 315;
		// The column's heading center (173 or 466) less 90
		int aTextXPos = (i != 0 ? 466 : 173) - 90;
		for (int aTankNum = 1; aTankNum <= 4; aTankNum++)
		{
			if (isAccel)
			{
				float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aTextYPos + aTextYPos) / 180.0);
				aRad = (float)sin(aRad);
				aTransform.m12 = (float)(aRad * 3.0);
				anInterface->PushTransform(aTransform, true);
			}

			SetVaryingBlueColor(g, aTextYPos);
			g->SetColor(Color(0xffff88));

			g->DrawString(StrFormat("Tank %d", aTankNum), aTextXPos, aTextYPos);
			int aScore;
			if (i == 0)
				aScore = mApp->mCurrentProfile->GetTimeTrialScore(aTankNum);
			else
				aScore = mApp->mCurrentProfile->GetChallengeScore(aTankNum);

			SexyString aScoreStr;
			if (aScore >= 0)
				aScoreStr = i == 0 ? CommaSeperate(aScore) : GetPlayTimeString(aScore);
			else
				aScoreStr = "-----";

			SetVaryingBlueColor(g, aTextYPos);
			int aStrWdth = g->GetFont()->StringWidth(aScoreStr);
			g->DrawString(aScoreStr, aTextXPos + 170 - aStrWdth, aTextYPos);

			aTextYPos += 15;

			if (isAccel)
				anInterface->PopTransform();
		}
	}
}

// The rows shimmer in blue: the lightness follows the update count plus the row's y, back and forth
void Sexy::HighScoreScreen::SetVaryingBlueColor(Graphics* g, int theY)
{
	int aVal = (mUpdateCnt + theY) % 200;
	if (aVal > 100)
		aVal = 200 - aVal;
	g->SetColor(Color(mApp->HSLToRGB(159, 119, (aVal * 70) / 100 + 180)));
}

void Sexy::HighScoreScreen::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::HighScoreScreen::ButtonDepress(int theId)
{
	if (theId == 4)
		mApp->RemoveHighScoreScreen();
	else if ((uint)theId <= 3)
		mPage = theId;
}

