#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/D3DInterface.h>
#include <SexyAppFramework/DDInterface.h>

#include "HelpScreen.h"
#include "WinFishCommon.h"
#include "WinFishApp.h"
#include "BubbleMgr.h"
#include "Res.h"

using namespace Sexy;

int Sexy::gHelpScreenTab = 1;
const double PI = 3.141590118408203;

// The virtual tank fish colour tables; the original HelpScreen has its own copy (0x5E1178), as Fish and
// BubbleMgr do, and the virtual tank pages colour their sample fish from it
const int HS_RED_TABLE1[30] = {
	125, 255, 135, 175, 195, 255, 255,  60, 255, 190,
	190, 145, 240, 255, 210, 175, 255,  65, 240, 240,
	255, 255, 175, 225, 130, 160, 205, 190, 240, 210
};

const int HS_GREEN_TABLE1[30] = {
	210, 240, 250, 235, 140, 255, 255,  60,  65,  60,
	190, 145, 145, 255, 215, 240, 180,  70, 240, 225,
	230,  40, 215, 175, 165, 230,  50, 140, 250, 250
};

const int HS_BLUE_TABLE1[30] = {
	255,  60, 245, 135,  90,  85, 255,  60,  65, 255,
	190, 175, 235,  40, 175, 175, 110, 160, 240, 125,
	  0,   0, 110,  95, 145,  50, 180, 250, 240,  50
};

const int HS_RED_TABLE2[30] = {
	 50,  45, 255,  50,  65, 250, 230,   0,  75, 255,
	250,  30, 110, 210, 110,  30, 210, 240,   0, 240,
	  0, 255,  15,  65, 125, 145, 255, 250,  80, 110
};

const int HS_GREEN_TABLE2[30] = {
	110,  95,   0, 115, 120, 155, 130,   0,  75,   0,
	250,  25,  10,  10,  95,  80,  10, 240,   5,   5,
	125, 225,  30,  55,  15,   0, 145, 245, 250, 210
};

const int HS_BLUE_TABLE2[30] = {
	210, 195, 125, 210,  65,   0, 250,   0,  75, 125,
	250, 225, 225,   0, 210, 125,   0, 240, 130, 240,
	  0,   0,  15, 190,  15, 210, 220, 175, 145,   0
};

Sexy::HelpScreen::HelpScreen(WinFishApp* theApp, bool instructions)
{
	mApp = theApp;
	mBubbleMgr = new BubbleMgr();
	mContinueButton = MakeDialogButton(0, this, "Click Here To Continue", nullptr);
	mContinueButton->Resize(210, 415, 220, mContinueButton->mHeight);
	mContinueButton->SetColor(DialogButton::COLOR_LABEL, Color(255, 240, 0));

	mMenuButton = MakeDialogButton2(1, this, "Menu", IMAGE_MAINBUTTON);
	mMenuButton->Resize(525, 4, 80, mMenuButton->mHeight);

	mPreviousButton = MakeDialogButton2(3, this, "Previous", IMAGE_MAINBUTTON);
	mPreviousButton->Resize(190, 430, 120, mMenuButton->mHeight);

	mNextButton = MakeDialogButton2(2, this, "Next", IMAGE_MAINBUTTON);
	mNextButton->Layout(LAY_SameBottom | LAY_Right | LAY_SameHeight | LAY_SameWidth, mPreviousButton, 20);

	if (instructions)
	{
		mPageIdx = 0;
		mNextButton->mVisible = false;
		mPreviousButton->mVisible = false;
		mBubbleMgr->SetBubbleBounds(Rect(0, 60, 640, 420));
	}
	else
	{
		mPageIdx = gHelpScreenTab;
		mContinueButton->mVisible = false;
		if (mApp->mBoard)
		{
			mMenuButton->Resize(480, 4, 125, mMenuButton->mHeight);
			mMenuButton->mLabel = "Back to Game";
		}
		mBubbleMgr->SetBubbleBounds(Rect(0, 80, 640, 400));
		mBubbleMgr->SetBubbleFishBounds(Rect(0, 80, 640, 350));
	}
	m0xa8 = -1;
	mBubbleMgr->SetBubbleConfig(20, 3);
	mBubbleMgr->UpdateALot();
}

Sexy::HelpScreen::~HelpScreen()
{
	if(mPageIdx > 0)
		gHelpScreenTab = mPageIdx;
	if (mBubbleMgr)
		delete mBubbleMgr;
	if (mContinueButton)
		delete mContinueButton;
	if (mMenuButton)
		delete mMenuButton;
	if (mNextButton)
		delete mNextButton;
	if (mPreviousButton)
		delete mPreviousButton;
}

void Sexy::HelpScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mMenuButton);
	theWidgetManager->AddWidget(mContinueButton);
	theWidgetManager->AddWidget(mNextButton);
	theWidgetManager->AddWidget(mPreviousButton);
}

void Sexy::HelpScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMenuButton);
	theWidgetManager->RemoveWidget(mContinueButton);
	theWidgetManager->RemoveWidget(mNextButton);
	theWidgetManager->RemoveWidget(mPreviousButton);
}

void Sexy::HelpScreen::Update()
{
	Widget::Update();

	if (mPageIdx != m0xa8)
	{
		m0xa8 = mPageIdx;
		if (mPageIdx == 8)
		{
			mBubbleMgr->SetBubbleFishConfig(3, 3);
		}
		else
		{
			mBubbleMgr->SetBubbleFishConfig(0, 0);
			mBubbleMgr->ScatterFish();
		}
	}
	mBubbleMgr->Update();
	MarkDirty();
}

void Sexy::HelpScreen::Draw(Graphics* g)
{
	if (mPageIdx == 0)
		g->DrawImageBox(Rect(-5, -5, 650, 490), IMAGE_SCREENBACK);
	else
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
		DrawWaves(g, mPageIdx <= 0 ? 60 : 80, mUpdateCnt);
		if (mPageIdx != 8)
		{
			Rect aTabRect1 = GetVertTabRect(0);
			Rect aTabRect2 = GetVertTabRect(1);
			Rect aTabRect4 = GetVertTabRect(3);
			int aYStartOffset = (aTabRect2.mY - aTabRect1.mHeight - aTabRect1.mY) / 2;

			// The size is right minus left and bottom minus top, as the original computes it
			int aTop = aTabRect1.mY - aYStartOffset;
			Rect aDrawRect = Rect(aTabRect1.mX, aTop, aTabRect1.mX + aTabRect1.mWidth - aTabRect1.mX,
				aTabRect4.mY + aTabRect4.mHeight + aYStartOffset - aTop);

			g->SetColor(Color(0, 0, 0, 30));
			g->FillRect(aDrawRect);
			g->SetColor(Color(0x88aaaa));
			g->DrawRect(aDrawRect);
			for (int i = 0; i < 3; i++)
			{
				Rect aTabRect = GetVertTabRect(i);
				g->FillRect(aTabRect.mX,aTabRect.mHeight + aTabRect.mY + aYStartOffset,aTabRect.mWidth,1);
			}
		}
	}

	switch (mPageIdx)
	{
	case 0:
		DrawInstructionsPage(g);
		break;
	case 1:
		DrawTheBasicsPage(g);
		break;
	case 2:
		DrawUpgradesPage(g);
		break;
	case 3:
		DrawPetsPage(g);
		break;
	case 4:
		DrawGameModesPage(g);
		break;
	case 5:
		DrawVirtualTankPage(g);
		break;
	case 6:
		DrawVirtualTankFishPage(g);
		break;
	case 7:
		DrawConfigVirtualTankPage(g);
		break;
	case 8:
		DrawCreditsPage(g);
		break;
	}
	
	SexyString aTabName = "";
	if (mPageIdx > 0)
	{
		switch (mPageIdx)
		{
		case 1:
			aTabName = "THE BASICS";
			break;
		case 2:
			aTabName = "UPGRADES";
			break;
		case 3:
			aTabName = "PETS";
			break;
		case 4:
			aTabName = "GAME MODES";
			break;
		case 5:
			aTabName = "VIRTUAL TANK";
			break;
		case 6:
			aTabName = "VIRTUAL TANK FISH";
			break;
		case 7:
			aTabName = "CONFIGURING VIRTUAL TANK";
			break;
		case 8:
			aTabName = "CREDITS";
			break;
		}

		g->SetFont(FONT_CONTINUUMBOLD14);
		g->SetColor(Color(0xffffff));
		DrawStringWithOutline(g, aTabName, 320 - g->GetFont()->StringWidth(aTabName) / 2, 60, FONT_CONTINUUMBOLD14OUTLINE, 0);

		g->SetColor(Color(0xffffff));
		SexyString aTabPage = StrFormat("Page %d of 8", mPageIdx);
		g->SetColor(Color(0xffffff));
		DrawStringWithOutline(g, aTabPage, 320 - g->GetFont()->StringWidth(aTabPage) / 2, 80,
			FONT_CONTINUUMBOLD14OUTLINE, 0);
	}

	g->DrawImageBox(Rect(20, 0, 600, IMAGE_SCREENTITLE->mHeight), IMAGE_SCREENTITLE);
	g->DrawImageBox(Rect(mMenuButton->mX - 1, mMenuButton->mY - 1, mMenuButton->mWidth + 2, IMAGE_SCREENTITLEHOLE->mHeight), IMAGE_SCREENTITLEHOLE);

	g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
	g->SetColor(Color(0xff, 200, 0, 0xff));
	const char* aTitleString = "Instructions";
	if (mPageIdx != 0)
		aTitleString = "Insaniquarium Help";

	WriteCenteredLine(g, 25, aTitleString);
}

Rect Sexy::HelpScreen::GetVertTabRect(int theTabNum)
{
	return Rect(10, theTabNum*65 + 160, 620, 50);
}

Rect Sexy::HelpScreen::GetLeftRectVertTab(int theTabNum, int theYWidth)
{
	Rect aRec = GetVertTabRect(theTabNum);
	return Rect(22, aRec.mY + theYWidth, 75, aRec.mHeight - 2 * theYWidth);
}

// Moves the point around a 10 px wide loop: x follows the sine at 2 degrees per update, y the cosine at 5
void Sexy::HelpScreen::CircleAroundCursor(int theUpdateCnt, int& theX, int& theY)
{
	float anAngle = (float)(theUpdateCnt * 2 * PI / 180.0);
	float aSin = (float)sin(anAngle);
	theX = (int)(aSin * 10.0 + theX);

	anAngle = (float)(theUpdateCnt * 5 * PI / 180.0);
	float aCos = (float)cos(anAngle);
	theY = (int)(aCos * 10.0 + theY);
}

void Sexy::HelpScreen::DrawTabTitle(Graphics* g, SexyString theLine, const SexyString& theLine2)
{
	Rect aRect(10, 110, 620, 0);
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(0xffffaa));
	WriteWordWrapped(g, aRect, theLine, -1, 0);
	aRect.mY += 15;
	WriteWordWrapped(g, aRect, theLine2, -1, 0);
}

void Sexy::HelpScreen::DrawVertTabText(Graphics* g, int theTabIdx,SexyString theLine, const SexyString& theLine2, int theHeightDrawOffset)
{
	Rect aTabRect = GetVertTabRect(theTabIdx);
	Rect aRect = GetLeftRectVertTab(theTabIdx, theHeightDrawOffset);
	g->SetColor(Color(0x60000000));
	g->FillRect(aRect);
	g->SetColor(Color(0x316584));
	g->DrawRect(aRect);
	g->SetColor(Color(0xffff88));
	g->SetFont(FONT_CONTINUUMBOLD14);
	
	// Drawn off screen (x = -100), so the tab number never shows
	DrawStringWithOutline(g, StrFormat("%d.", theTabIdx+1), -100, aTabRect.mY + 35, FONT_CONTINUUMBOLD14OUTLINE, 0);

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	int aY = aTabRect.mY + 20;
	SetVaryingBlueColor(g, aY);
	g->DrawString(theLine, 120, aY);
	aY += 15;
	SetVaryingBlueColor(g, aY);
	g->DrawString(theLine2, 120, aY);
}

void Sexy::HelpScreen::DrawInstructionsPage(Graphics* g)
{
	Rect aLeftRect(30, 70, 185, 320);
	Rect aMiddleRect(227, 70, 185, 320);
	Rect aRightRect(424, 70, 185, 320);
	DrawInstrLeftPart(g, aLeftRect);
	DrawInstrMiddlePart(g, aMiddleRect);
	DrawInstrRightPart(g, aRightRect);
	DrawHorzCrease(g, IMAGE_HORZCREASE, 13, 230, 23);
	DrawHorzCrease(g, IMAGE_HORZCREASE, 209, 230, 24);
	DrawHorzCrease(g, IMAGE_HORZCREASE, 406, 230, 24);
	DrawHorzCrease(g, IMAGE_HORZCREASE, 603, 230, 24);

	DrawVertCrease(g, IMAGE_VERTCREASE, 120, 33, 40);
	DrawVertCrease(g, IMAGE_VERTCREASE, 515, 33, 40);
	DrawVertCrease(g, IMAGE_VERTCREASE, 120, 386, 83);
	DrawVertCrease(g, IMAGE_VERTCREASE, 515, 386, 83);

	g->DrawImage(IMAGE_BOLT, 105, 53);
	g->DrawImage(IMAGE_BOLT, 135, 53);
	g->DrawImage(IMAGE_BOLT, 500, 53);
	g->DrawImage(IMAGE_BOLT, 530, 53);

	g->DrawImage(IMAGE_BOLT, 105, 406);
	g->DrawImage(IMAGE_BOLT, 135, 406);
	g->DrawImage(IMAGE_BOLT, 500, 406);
	g->DrawImage(IMAGE_BOLT, 530, 406);
}

void Sexy::HelpScreen::DrawFishBox(Graphics* g, Rect& theRect)
{
	g->DrawImageBox(theRect, IMAGE_FISHBOX);
}

void Sexy::HelpScreen::DrawInstrLeftPart(Graphics* g, Rect& theRect)
{
	DrawFishBox(g, theRect);

	int aTopY = theRect.mY + 30;
	int aLeftX = theRect.mX + 50;
	int aRightX = theRect.mWidth + theRect.mX - 50;
	float aRadianVal = (float)(mUpdateCnt * 5 * PI / 180.0);
	float aSinVal = (float)sin(aRadianVal);
	int anObjY = (int)((theRect.mY + 120) + aSinVal * 3.0);

	int anAnimTimer = mUpdateCnt % 115;

	bool doSwap = ((mUpdateCnt / 115) % 2);

	int aFoodX = InterpolateInt(aLeftX, aRightX, anAnimTimer, 30, doSwap);
	if (anAnimTimer >= 40 && anAnimTimer < 85)
	{
		int aFoodY = InterpolateInt(aTopY, anObjY, anAnimTimer - 40, 45, false);
		g->DrawImageCel(IMAGE_FOOD, aFoodX - 10, aFoodY - 20, (mUpdateCnt / 4) % 10, 0);
	}

	int aFishX = InterpolateInt(aLeftX + 15, aRightX - 10, anAnimTimer - 45, 40, doSwap);
	Rect anAnimSrcRect = Rect(((mUpdateCnt/4)%10) * 80, 0, 80, 80);
	Image* anImg = IMAGE_SMALLSWIM;
	bool doMirror = doSwap;
	if (anAnimTimer >= 45)
	{
		if (anAnimTimer < 65)
		{
			anAnimSrcRect.mX = ((anAnimTimer - 45) / 2) * 80;
			anImg = IMAGE_SMALLTURN;
		}
		else
		{
			doMirror = !doSwap;
			if (anAnimTimer >= 75 && anAnimTimer < 95)
			{
				anAnimSrcRect.mX = ((anAnimTimer - 75) / 2) * 80;
				anImg = IMAGE_SMALLEAT;
			}
		}
	}
	g->DrawImageMirror(anImg, aFishX -37, anObjY-40, anAnimSrcRect, doMirror);

	bool doClick = anAnimTimer >= 40 && anAnimTimer < 60;

	DrawCursor(g, aFoodX, aTopY, doClick);

	const char* aStrLines[3] = { "Click on the tank" , "to drop food" , "for your fish." };
	SexyString aStrTitle = "FEED ME";
	DrawInstrText(g, theRect, aStrTitle, aStrLines, 3);
}

void Sexy::HelpScreen::DrawInstrMiddlePart(Graphics* g, Rect& theRect)
{
	DrawFishBox(g, theRect);
	int aCounter = mUpdateCnt % 40;
	float aRadianVal = (float)(mUpdateCnt * 3.0 * PI / 180.0);
	int anX = theRect.mX + theRect.mWidth / 2;
	int anY = theRect.mY + 80;
	aRadianVal = (float)sin(aRadianVal);
	int aSylvY = (int)(aRadianVal * 5.0 + (theRect.mY + 20));

	// The cursor circles around the middle; the click burst stays where it was 0-19 updates ago
	int aCursorX = anX;
	int aCursorY = anY;
	int aClickX = anX;
	int aClickY = anY;
	aCounter -= 20;
	int anUpdateCnt = mUpdateCnt;
	CircleAroundCursor(anUpdateCnt, aCursorX, aCursorY);
	CircleAroundCursor(anUpdateCnt - aCounter, aClickX, aClickY);

	int anXWOffset = anX - 80;

	Rect aSrcRect = Rect(((anUpdateCnt / 3) % 10) * 160, 0, 160, 160);
	g->DrawImage(IMAGE_SYLV, anXWOffset, aSylvY, aSrcRect);

	if (aCounter >= 0)
	{
		if (aCounter < 10)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->SetColorizeImages(true);
			g->SetColor(Color(255, 255, 255, (10 - aCounter) * 10));
			g->DrawImage(IMAGE_SYLV, anXWOffset, aSylvY, aSrcRect);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
			g->SetColorizeImages(false);
		}
		if (aCounter < 20)
			DrawClickLines(g, aClickX, aClickY);
	}

	DrawCursor(g, aCursorX, aCursorY, false);

	const char* aLinesStr[2] = { "Use the mouse to" , "zap Aliens!" };
	SexyString aTitleStr = "FEAR ME";
	DrawInstrText(g, theRect, aTitleStr, aLinesStr, 2);
}

void Sexy::HelpScreen::DrawInstrRightPart(Graphics* g, Rect& theRect)
{
	DrawFishBox(g, theRect);
	g->DrawImage(IMAGE_EGGPIECES, (theRect.mWidth-IMAGE_EGGPIECES->mWidth) / 2 + theRect.mX, theRect.mY + 80);
	const char* aLines[4] = { "Collect all 3 pieces" ,"of the egg to" ,"advance a level and" ,"gain a new pet!" };
	SexyString aTitleStr = "FIND ME";
	DrawInstrText(g, theRect, aTitleStr, aLines, 4);
}

// The burst of eight short lines around a click
void Sexy::HelpScreen::DrawClickLines(Graphics* g, int theX, int theY)
{
	g->SetColor(Color(0xffffff));
	g->DrawLine(theX - 3, theY - 3, theX - 8, theY - 8);
	g->DrawLine(theX + 3, theY + 3, theX + 8, theY + 8);
	g->DrawLine(theX - 3, theY + 3, theX - 8, theY + 8);
	g->DrawLine(theX + 3, theY - 3, theX + 8, theY - 8);
	g->DrawLine(theX - 3, theY, theX - 8, theY);
	g->DrawLine(theX + 3, theY, theX + 8, theY);
	g->DrawLine(theX, theY + 3, theX, theY + 8);
	g->DrawLine(theX, theY - 3, theX, theY - 8);
}

void Sexy::HelpScreen::DrawCursor(Graphics* g, int theX, int theY, bool doClick)
{
	if (doClick)
	{
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
		g->SetColor(Color(0xffffff));
		g->DrawLine(theX - 3, theY - 3, theX - 8, theY - 8);
		g->DrawLine(theX + 3, theY + 3, theX + 8, theY + 8);
		g->DrawLine(theX - 3, theY + 3, theX - 8, theY + 8);
		g->DrawLine(theX + 3, theY - 3, theX + 8, theY - 8);
		g->DrawLine(theX - 3, theY, theX - 8, theY);
		g->DrawLine(theX + 3, theY, theX + 8, theY);
		g->DrawLine(theX, theY + 3, theX, theY + 8);
		g->DrawLine(theX, theY - 3, theX, theY - 8);
	}
	g->DrawImage(IMAGE_CURSOR_POINTER, theX - IMAGE_CURSOR_POINTER->mWidth / 2, theY - IMAGE_CURSOR_POINTER->mHeight / 2);
}

void Sexy::HelpScreen::DrawInstrText(Graphics* g, Rect& theRect, const SexyString& theTitle, const char** theLines, int theNumOfLines)
{
	int aY = theRect.mHeight + theRect.mY - 100;
	int aCenterX = theRect.mWidth / 2 + theRect.mX;
	g->SetFont(FONT_CONTINUUMBOLD14);
	g->SetColor(Color(0xffff00));

	int aStrWdth = g->GetFont()->StringWidth(theTitle);
	DrawStringWithOutline(g, theTitle, aCenterX - aStrWdth / 2 - 5, aY, FONT_CONTINUUMBOLD14OUTLINE, 0);

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	aY += 25;

	for (int i = 0; i < theNumOfLines; i++)
	{
		g->SetColor(Color(0xffffff));
		g->DrawString(theLines[i], aCenterX - g->GetFont()->StringWidth(theLines[i]) / 2, aY);
		aY += 15;
	}
}

void Sexy::HelpScreen::DrawTheBasicsPage(Graphics* g)
{
	DrawTabTitle(g, "Drop food for your fish to make them grow, then defend them from evil aliens!", 
		"Collect coins to buy upgrades and Egg pieces.  Complete an Egg to level up!");
	DrawVertTabText(g, 0, "Fish turn green when hungry!  Click on the tank to drop food.",
		"Well fed fish grow larger, but unfed fish may starve!");
	DrawVertTabText(g, 1, "Fish will drop coins for you.  Click on the coins to earn",
		"money!  Bigger fish will drop more valuable coins.");
	DrawVertTabText(g, 2, "Aliens will attack your fish!  Click on them to shoot.  Clicking",
		"on the right side of the alien will make it move left, and so on.");
	DrawVertTabText(g, 3, "Click buttons at the top to buy new upgrades.  Buy 3 Egg"
	, "Pieces to finish the level and earn a new pet!");

	Rect aRect = GetLeftRectVertTab(0, 4);
	g->DrawImageCel(IMAGE_HUNGRYSWIM, aRect.mWidth / 2 - 40 + aRect.mX, aRect.mHeight / 2 - 40 + aRect.mY, mUpdateCnt/3%10, 0);

	aRect = GetLeftRectVertTab(1, 4);
	g->DrawImageCel(IMAGE_MONEY, aRect.mWidth / 2 - 36 + aRect.mX, aRect.mHeight / 2 - 36 + aRect.mY, mUpdateCnt / 2 % 10, 1);

	// The alien bobs: y by 3 * sin at 4 degrees per update, x by 2 * cos at 2 degrees (subtracted)
	aRect = GetLeftRectVertTab(2, 4);
	float aRad = (float)(mUpdateCnt * 4 * PI / 180.0);
	aRad = (float)sin(aRad);
	int aFinalY = (int)(aRad * 3.0 + (aRect.mY + (aRect.mHeight - IMAGE_HELPALIEN->mHeight) / 2) + 3.0);
	float aBaseX = (float)(aRect.mX + (aRect.mWidth - IMAGE_HELPALIEN->mWidth) / 2);
	aRad = (float)(mUpdateCnt * 2 * PI / 180.0);
	aRad = (float)cos(aRad);

	g->DrawImage(IMAGE_HELPALIEN, (int)(aBaseX - aRad * 2.0 + 2.0), aFinalY);

	aRect = GetLeftRectVertTab(3, 4);
	g->DrawImageCel(IMAGE_EGGPIECES, aRect.mWidth / 2 - 23 + aRect.mX, aRect.mHeight / 2 - 18 + aRect.mY, mUpdateCnt / 36 % 3);
}

void Sexy::HelpScreen::DrawUpgradesPage(Graphics* g)
{
	DrawTabTitle(g, "There are several upgrades you can purchase if you have enough money.",
		"Click on the buttons at the top of the screen to buy the ones you want.");
	DrawVertTabText(g, 0, "You can buy new fish for your tank!  Click on the appropriate",
		"button to buy a new Guppy or Carnivore, if you can afford it.");
	DrawVertTabText(g, 1, "You can upgrade the quality of your food pellets.  Better food",
		"will feed fish for a longer time and make them grow faster.");
	DrawVertTabText(g, 2, "You can also increase the number of food pellets you can",
		"drop at once by upgrading your food quantity.");
	DrawVertTabText(g, 3, "You can also increase the power of your anti-alien weapon."
		, "More powerful weapons kill aliens in fewer clicks!");

	Rect aRect = GetLeftRectVertTab(0, 4);
	g->DrawImageCel(IMAGE_SMALLSWIM, aRect.mWidth / 2 - 40 + aRect.mX, aRect.mHeight / 2 - 40 + aRect.mY, mUpdateCnt / 3 % 10, 0);

	// The pellets bob by 3 * sin; the angles are floats
	aRect = GetLeftRectVertTab(1, 4);
	int aBaseX = aRect.mWidth / 2 + aRect.mX;
	float aBaseY = (float)(aRect.mHeight / 2 + aRect.mY - 20);
	float aRadY = (float)(mUpdateCnt * 5 * PI / 180.0);
	float aSin = (float)sin(aRadY);
	g->DrawImageCel(IMAGE_FOOD, aBaseX - 40, (int)(aSin * 3.0 + aBaseY), mUpdateCnt / 3 % 10, 0);
	aSin = (float)sin((float)((double)aRadY + 30.017454f));
	g->DrawImageCel(IMAGE_FOOD, aBaseX - 20, (int)(aSin * 3.0 + aBaseY), mUpdateCnt / 4 % 10, 1);
	aSin = (float)sin((float)((double)aRadY + 60.034908f));
	g->DrawImageCel(IMAGE_FOOD, aBaseX, (int)(aSin * 3.0 + aBaseY), mUpdateCnt / 3 % 10, 2);

	// The first pellet here takes the sine of the constant alone, not of the angle, so it does not bob
	aRadY = (float)(mUpdateCnt * 5 * PI / 180.0 + 60.0);
	aRect = GetLeftRectVertTab(2, 4);
	aBaseY = (float)(aRect.mHeight / 2 + aRect.mY - 20);
	aBaseX = aRect.mWidth / 2 + aRect.mX;
	aSin = (float)sin(0.08726639f);
	g->DrawImageCel(IMAGE_FOOD, aBaseX - 40, (int)(aSin * 3.0 + aBaseY), mUpdateCnt / 3 % 10, 0);
	aSin = (float)sin((float)((double)aRadY + 0.08726639f));
	g->DrawImageCel(IMAGE_FOOD, aBaseX - 20, (int)(aSin * 3.0 + aBaseY), (mUpdateCnt / 4 + 3) % 10, 0);
	aSin = (float)sin((float)((double)aRadY + aRadY + 0.08726639f));
	g->DrawImageCel(IMAGE_FOOD, aBaseX, (int)(aSin * 3.0 + aBaseY), (mUpdateCnt / 3 + 6) % 10, 0);

	aRect = GetLeftRectVertTab(3, 4);
	g->DrawImageCel(IMAGE_LASERUPGRADES, aRect.mWidth / 2 - 23 + aRect.mX, aRect.mHeight / 2 - 18 + aRect.mY, mUpdateCnt / 36 % 3);
}

void Sexy::HelpScreen::DrawPetsPage(Graphics* g)
{
	DrawTabTitle(g, "You can finish a level by collecting all three pieces of an Egg.  Click on the",
		"Egg Piece button in the control bar to buy an Egg Piece.");
	DrawVertTabText(g, 0, "Advancing to the next level makes your egg hatch.  This gives you",
		"a new pet!  Each pet has different powers to help you out.");
	DrawVertTabText(g, 1, "ITCHY the Swordfish is one of the pets that you can get.",
		"He will attack aliens that infiltrate your tank!");
	DrawVertTabText(g, 2, "STINKY the Snail is another pet that you can get.",
		"He roams the ground picking up stray coins for you!");
	DrawVertTabText(g, 3, "You can only bring 3 pets into a level. If you have more than 3"
		, "pets, you\'ll have to choose which 3 you want to have in the tank.");

	Rect aRect = GetLeftRectVertTab(0, 4);
	g->DrawImageCel(IMAGE_EGGPIECES, aRect.mWidth / 2 - 23 + aRect.mX, aRect.mHeight / 2 -18 + aRect.mY, 2);
	
	aRect = GetLeftRectVertTab(1, 4);
	g->DrawImageCel(IMAGE_SCL_ITCHY, aRect.mWidth / 2 - 30 + aRect.mX, aRect.mHeight / 2 -30 + aRect.mY, mUpdateCnt/4 % 10);
	
	aRect = GetLeftRectVertTab(2, 4);
	g->DrawImageCel(IMAGE_SCL_STINKY, aRect.mWidth / 2 - 30 + aRect.mX, aRect.mHeight / 2 -30 + aRect.mY, mUpdateCnt/4 % 10);
	
	aRect = GetLeftRectVertTab(3, 4);
	g->DrawImage(IMAGE_HELPPETCHOOSE, (aRect.mWidth- IMAGE_HELPPETCHOOSE->mWidth) / 2 + aRect.mX, (aRect.mHeight- IMAGE_HELPPETCHOOSE->mHeight) / 2 + 2 + aRect.mY);
}

void Sexy::HelpScreen::DrawGameModesPage(Graphics* g)
{
	DrawTabTitle(g, "Insaniquarium has four different game modes that you can play!",
		"They are Adventure, Time Trial, Challenge, and Virtual Tank.");
	DrawVertTabText(g, 0, "Adventure is the main game mode.  In this mode, you",
		"progress through multiple tanks and accumulate pets.");
	DrawVertTabText(g, 1, "The goal of Time Trial mode is to collect as much",
		"money as you can before the time runs out.");
	DrawVertTabText(g, 2, "Challenge mode is for experts.  In this mode, you must",
		"deal with price inflation and increasingly difficult aliens.");
	DrawVertTabText(g, 3, "Virtual Tank is a Virtual Aquarium.  Use Shells earned in"
		, "other modes to purchase items and fish for your tank!");

	SexyString aStr;
	g->SetColor(Color(0xbbffbb));
	g->SetFont(FONT_CONTINUUMBOLD12);

	Rect aRect = GetLeftRectVertTab(0, 4);
	aStr = "Adventure";
	DrawGameModesPageText(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 30, 100);

	aRect = GetLeftRectVertTab(1, 4);
	aStr = "Time Trial";
	DrawGameModesPageText(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 30, 100);

	aRect = GetLeftRectVertTab(2, 4);
	aStr = "Challenge";
	DrawGameModesPageText(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 30, 100);

	aRect = GetLeftRectVertTab(3, 4);
	aStr = "Virtual";
	DrawGameModesPageText(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 18, 100);
	aStr = "Tank";
	DrawGameModesPageText(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 37, 100);
}

// The last parameter is unused (every caller passes 100)
void Sexy::HelpScreen::DrawGameModesPageText(Graphics* g, const SexyString& theString, int theX, int theY, int theUnused)
{
	int aVal = (mUpdateCnt + theY) % 200;
	if (aVal > 100)
		aVal = 200 - aVal;
	g->SetColor(Color(mApp->HSLToRGB(40, 200, (aVal * 70) / 100 + 180)));
	g->DrawString(theString, theX, theY);
}

void Sexy::HelpScreen::DrawVirtualTankPage(Graphics* g)
{
	DrawTabTitle(g, "Virtual Tank is a Virtual Aquarium.  In it, you can buy, name,",
		"take care of, and play with your own unique fish.");
	DrawVertTabText(g, 0, "Access the Virtual Tank store from within Virtual Tank.",
		"You must use Shells to buy items in the store.");
	DrawVertTabText(g, 1, "You can earn Shells at certain times in all four",
		"game modes.  Shells are the currency of Virtual Tank.");
	DrawVertTabText(g, 2, "Items in the Virtual Tank store change daily with the real",
		"world date so check back every day to see what\'s new!");
	DrawVertTabText(g, 3, "Some items in the store are quite rare so make sure to"
		, "check the store every day so you don\'t miss a rare item!");

	Rect aSrcRect;
	SexyString aStr; // unused on this page, as in the original
	g->SetColor(Color(0xbbffbb));
	g->SetFont(FONT_CONTINUUMBOLD12);

	Rect aRect = GetLeftRectVertTab(0, 4);
	int aCelWidth = IMAGE_TROPHYBUTTONS->GetCelWidth();
	int aCelHeight = IMAGE_TROPHYBUTTONS->GetHeight();
	g->DrawImageCel(IMAGE_TROPHYBUTTONS, (aRect.mWidth - aCelWidth) / 2 + aRect.mX,
		(aRect.mHeight - aCelHeight) / 2 + aRect.mY, 2);

	aRect = GetLeftRectVertTab(1, 4);
	g->DrawImage(IMAGE_HELPSHELL, (aRect.mWidth - IMAGE_HELPSHELL->mWidth) / 2 + aRect.mX,
		(aRect.mHeight - IMAGE_HELPSHELL->mHeight) / 2 + aRect.mY);

	// The sample fish's two colours: entry 1 of the colour tables, plus 5 and capped at 255
	Color aColor1(HS_RED_TABLE1[1] + 5 > 255 ? 255 : HS_RED_TABLE1[1] + 5,
		HS_GREEN_TABLE1[1] + 5 > 255 ? 255 : HS_GREEN_TABLE1[1] + 5,
		HS_BLUE_TABLE1[1] + 5 > 255 ? 255 : HS_BLUE_TABLE1[1] + 5, 255);
	Color aColor2(HS_RED_TABLE2[1] + 5 > 255 ? 255 : HS_RED_TABLE2[1] + 5,
		HS_GREEN_TABLE2[1] + 5 > 255 ? 255 : HS_GREEN_TABLE2[1] + 5,
		HS_BLUE_TABLE2[1] + 5 > 255 ? 255 : HS_BLUE_TABLE2[1] + 5, 255);

	aRect = GetLeftRectVertTab(2, 4);
	aSrcRect = Rect(mUpdateCnt / 2 % 10 * 80, 0, 80, 80);
	int anX = (aRect.mWidth - 80) / 2 + aRect.mX;
	int anY = (aRect.mHeight - 80) / 2 + aRect.mY;
	g->DrawImage(IMAGE_SIMSWIM1, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	g->SetColorizeImages(true);
	g->SetColor(aColor2);
	g->DrawImage(IMAGE_SIMSWIM2, anX, anY, aSrcRect);
	g->SetColor(aColor1);
	g->DrawImage(IMAGE_SIMSWIM3, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	g->SetColorizeImages(false);

	aRect = GetLeftRectVertTab(3, 4);
	aSrcRect = Rect(mUpdateCnt/4%10*50,0,50,50);
	g->DrawImage(IMAGE_BALLS, (aRect.mWidth - 50) / 2 + aRect.mX, (aRect.mHeight - 50) / 2 + aRect.mY, aSrcRect);
}

void Sexy::HelpScreen::DrawVirtualTankFishPage(Graphics* g)
{
	DrawTabTitle(g, "Virtual Tank fish behave differently than fish in other game modes.",
		"Here is a list of differences.");
	DrawVertTabText(g, 0, "Virtual Tank fish want to eat three times a day.  They will",
		"become unhappy if you don\'t feed them three times every day.");
	DrawVertTabText(g, 1, "Fish in Virtual Tank will grow eventually.  The time",
		"to grow is about one real world week if fed every day.", 0);
	DrawVertTabText(g, 2, "To feed fish which don\'t eat the normal food that you drop by",
		"clicking, press the \"Feed\" button on the Virtual Tank menu.");
	DrawVertTabText(g, 3, "Virtual Tank fish can not die or be killed.  That\'s not"
		, "to say that you shouldn\'t take care of them, though!");

	Rect aSrcRect;
	SexyString aStr;
	g->SetColor(Color(0xbbffbb));
	g->SetFont(FONT_CONTINUUMBOLD12);

	// The sample fish's two colours: entry 15 of the colour tables, plus 5 and capped at 255
	Color aColor1(HS_RED_TABLE1[15] + 5 > 255 ? 255 : HS_RED_TABLE1[15] + 5,
		HS_GREEN_TABLE1[15] + 5 > 255 ? 255 : HS_GREEN_TABLE1[15] + 5,
		HS_BLUE_TABLE1[15] + 5 > 255 ? 255 : HS_BLUE_TABLE1[15] + 5, 255); // loc84
	Color aColor2(HS_RED_TABLE2[15] + 5 > 255 ? 255 : HS_RED_TABLE2[15] + 5,
		HS_GREEN_TABLE2[15] + 5 > 255 ? 255 : HS_GREEN_TABLE2[15] + 5,
		HS_BLUE_TABLE2[15] + 5 > 255 ? 255 : HS_BLUE_TABLE2[15] + 5, 255); // loc94

	Rect aRect = GetLeftRectVertTab(0, 4);

	int aFrameIdx = mUpdateCnt / 2 % 27;
	int aResId = IMAGE_SIMSWIM1_ID;
	if (aFrameIdx < 10)
		aResId = IMAGE_SIMEAT1_ID;
	else
		aFrameIdx += 3;

	aSrcRect = Rect((aFrameIdx % 10) * 80, 0, 80, 80);

	int anX = (aRect.mWidth - 80) / 2 + aRect.mX;
	int anY = (aRect.mHeight - 80) / 2 + aRect.mY;

	Image* aToDrawImg = GetImageById(aResId);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	g->SetColorizeImages(true);
	g->SetColor(aColor2);

	aToDrawImg = GetImageById(aResId+1);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetColor(aColor1);

	aToDrawImg = GetImageById(aResId + 2);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	g->SetColorizeImages(false);

	aRect = GetLeftRectVertTab(1, 4);
	aColor1 = Color(HS_RED_TABLE1[15] + 5 > 255 ? 255 : HS_RED_TABLE1[15] + 5,
		HS_GREEN_TABLE1[15] + 5 > 255 ? 255 : HS_GREEN_TABLE1[15] + 5,
		HS_BLUE_TABLE1[15] + 5 > 255 ? 255 : HS_BLUE_TABLE1[15] + 5, 255);
	aColor2 = Color(HS_RED_TABLE2[15] + 5 > 255 ? 255 : HS_RED_TABLE2[15] + 5,
		HS_GREEN_TABLE2[15] + 5 > 255 ? 255 : HS_GREEN_TABLE2[15] + 5,
		HS_BLUE_TABLE2[15] + 5 > 255 ? 255 : HS_BLUE_TABLE2[15] + 5, 255);

	aSrcRect = Rect(((mUpdateCnt / 2) % 10) * 80, 80, 80, 80);

	anX = (aRect.mWidth - 80) / 2 + aRect.mX;
	anY = (aRect.mHeight - 80) / 2 + aRect.mY;

	aToDrawImg = GetImageById(IMAGE_SIMSWIM1_ID);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	g->SetColorizeImages(true);
	g->SetColor(aColor2);

	aToDrawImg = GetImageById(IMAGE_SIMSWIM2_ID);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetColor(aColor1);

	aToDrawImg = GetImageById(IMAGE_SIMSWIM3_ID);
	g->DrawImage(aToDrawImg, anX, anY, aSrcRect);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	g->SetColorizeImages(false);

	aRect = GetLeftRectVertTab(2, 4);
	int aCelWidth = IMAGE_TROPHYBUTTONS->GetCelWidth();
	int aCelHeight = IMAGE_TROPHYBUTTONS->GetHeight();
	g->DrawImageCel(IMAGE_TROPHYBUTTONS, (aRect.mWidth - aCelWidth) / 2 + aRect.mX,
		(aRect.mHeight - aCelHeight) / 2 + 2 + aRect.mY, 1);

	aRect = GetLeftRectVertTab(3, 4);
	aSrcRect = Rect(720, 0, 80, 80);
	g->DrawImage(IMAGE_SMALLDIE, (aRect.mWidth - 80) / 2 + aRect.mX, (aRect.mHeight - 80) / 2 + aRect.mY, aSrcRect);
	aStr = "?";
	g->SetColor(Color(0xffff00));
	g->SetFont(FONT_CONTINUUMBOLD14);
	DrawStringWithOutline(g, aStr, (aRect.mWidth - g->GetFont()->StringWidth(aStr)) / 2 + aRect.mX, aRect.mY + 25, FONT_CONTINUUMBOLD14OUTLINE, 0);
}

void Sexy::HelpScreen::DrawConfigVirtualTankPage(Graphics* g)
{
	DrawTabTitle(g, "You can configure the Virtual Tank in several ways.",
		"Here is a list of the options.");
	DrawVertTabText(g, 0, "Press the \"Fish\" button to go to a screen where you can see",
		"information about your fish as well as hide, show, and sell them.");
	DrawVertTabText(g, 1, "Press the \"Pets\" button to select which pets you would like",
		"to display in your Virtual Tank.");
	DrawVertTabText(g, 2, "Press the \"Tank\" button to select which tank backdrop to display",
		"and whether or not to show various items in your tank.");
	DrawVertTabText(g, 3, "You can set Virtual Tank as your computer\'s Screensaver by"
		, "pressing the \"Screensaver\" button on the \"Tank\" screen.", 0);

	SexyString aStr; // unused on this page, as in the original
	g->SetColor(Color(0xbbffbb));
	g->SetFont(FONT_CONTINUUMBOLD12);

	int aCelWidth = IMAGE_TROPHYBUTTONS->GetCelWidth();
	int aCelHeight = IMAGE_TROPHYBUTTONS->GetHeight();
	Rect aRect = GetLeftRectVertTab(0, 4);
	g->DrawImageCel(IMAGE_TROPHYBUTTONS, (aRect.mWidth - aCelWidth) / 2 + aRect.mX,
		(aRect.mHeight - aCelHeight) / 2 + 3 + aRect.mY, 3);
	aRect = GetLeftRectVertTab(1, 4);
	g->DrawImageCel(IMAGE_TROPHYBUTTONS, (aRect.mWidth - aCelWidth) / 2 + aRect.mX,
		(aRect.mHeight - aCelHeight) / 2 + aRect.mY, 4);
	aRect = GetLeftRectVertTab(2, 4);
	g->DrawImageCel(IMAGE_TROPHYBUTTONS, (aRect.mWidth - aCelWidth) / 2 + aRect.mX,
		(aRect.mHeight - aCelHeight) / 2 + aRect.mY, 5);
	aRect = GetLeftRectVertTab(3, 4);
	g->DrawImage(IMAGE_HELPSCREENSAVER, (aRect.mWidth - IMAGE_HELPSCREENSAVER->mWidth) / 2 + aRect.mX,
		(aRect.mHeight - IMAGE_HELPSCREENSAVER->mHeight) / 2 + 2 + aRect.mY);
}

void Sexy::HelpScreen::DrawCreditsPage(Graphics* g)
{
	bool is3DAccel = mApp->Is3DAccelerated();
	D3DInterface* anInterface = mApp->mDDInterface->mD3DInterface;
	SexyTransform2D aTransform;
	int anY = 150;
	int aBobY = 150;

	#define CREDITS_ROWS 11
	const char* aCreditsMatrix[CREDITS_ROWS][2] = {
		{"Game Design", "George Fan"},
		{"Producer", "Jason Kapalka, Sukhbir Sidhu"},
		{"Programming", "George Fan, Thien Tran, Brian Rothstein"},
		{"Technical Assistance", "David Parton"},
		{"Art", "Josh Langley, Walter Wilson"},
		{"Character Design", "George Fan"},
		{"Music", "Jonne Valtonen, George Fan"},
		{"PopCap Framework", "Brian Fiete, David Parton"},
		{"Biz Dev","Don Walters"},
		{"QA", "Eric Harman, Shawn Conard,"},
		{"", "Brenna Flood, Chad Zoellner"}, };

	for (int i = 0; i < CREDITS_ROWS; i++)
	{
		if (is3DAccel)
		{
			float aRad = (float)(mUpdateCnt * 3.0 * PI / 180.0 + PI * ((double)aBobY + aBobY) / 180.0);
			aRad = (float)sin(aRad);
			aTransform.m12 = (float)(aRad * 3.0);
			anInterface->PushTransform(aTransform, true);
		}
		SexyString aStr;
		g->SetColor(Color(0xbbffbb));
		g->SetFont(FONT_CONTINUUMBOLD14);

		aStr = aCreditsMatrix[i][0];
		if (aStr.size() == 0)
			anY -= 7;

		DrawCreditsPageLeftText(g, aStr, 20, anY, 100);

		aStr = aCreditsMatrix[i][1];
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
		SetVaryingBlueColor(g, anY);
		g->DrawString(aStr, 300, anY);

		anY += 25;

		aBobY = anY;
		if (is3DAccel)
			anInterface->PopTransform();
	}
}

// The last parameter is unused (the caller passes 100)
void Sexy::HelpScreen::DrawCreditsPageLeftText(Graphics* g, const SexyString& theString, int theX, int theY, int theUnused)
{
	int aVal = (mUpdateCnt + theY) % 200;
	if (aVal > 100)
		aVal = 200 - aVal;
	g->SetColor(Color(mApp->HSLToRGB(40, 200, (aVal * 70) / 100 + 180)));
	g->SetFont(FONT_CONTINUUMBOLD14);
	DrawStringWithOutline(g, theString, theX, theY, FONT_CONTINUUMBOLD14OUTLINE, 0);
}

// The text shimmers in blue: the lightness follows the update count plus the line's y, back and forth
// (one body with HighScoreScreen::SetVaryingBlueColor in the original)
void Sexy::HelpScreen::SetVaryingBlueColor(Graphics* g, int theY)
{
	int aVal = (mUpdateCnt + theY) % 200;
	if (aVal > 100)
		aVal = 200 - aVal;
	g->SetColor(Color(mApp->HSLToRGB(159, 119, (aVal * 70) / 100 + 180)));
}

void Sexy::HelpScreen::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::HelpScreen::ButtonDepress(int theId)
{
	if (theId == 1)
	{
		mApp->RemoveHelpScreen();
		if (!mApp->mBoard)
		{
			mApp->SwitchToGameSelector();
			return;
		}
	}
	else if(theId == 0)
	{
		mApp->StartGame();
		mApp->RemoveHelpScreen();
		return;
	}
	else if (theId == 2)
	{
		mPageIdx++;
		if (mPageIdx > 8)
			mPageIdx = 1;
		return;
	}
	else if (theId == 3)
	{
		mPageIdx--;
		if (mPageIdx < 1)
			mPageIdx = 8;
		return;
	}
}

