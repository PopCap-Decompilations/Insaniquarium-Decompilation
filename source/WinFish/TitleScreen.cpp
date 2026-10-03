#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/Graphics.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Rect.h>
#include <SexyAppFramework/HyperlinkWidget.h>

#include "TitleScreen.h"
#include "WinFishApp.h"
#include "Res.h"

using namespace Sexy;

// 0x5E160C: Stinky's starting x; while loading he stays this far ahead of the bar's progress
static int gStinkyStartX = 160;

TitleScreen::TitleScreen(WinFishApp* theApp)
{
	mApp = theApp;
	m0x94 = 0;
	m0x98 = IMAGE_LOADERBAR->mWidth;
	m0x9c = 0;
	m0xc0 = false;
	m0xa0 = false;
	m0xc1 = false;
	m0xa1 = false;

	Font* aFont = FONT_JUNGLEFEVER17OUTLINE;
	mHyperlink1 = new HyperlinkWidget(0, this);
	mHyperlink1->SetFont(aFont);
	mHyperlink1->mColor = Color(0xffec91);
	mHyperlink1->mOverColor = Color(0xFFFFFF);
	mHyperlink1->mUnderlineSize = 2;
	mHyperlink1->SetVisible(false);

	mHyperlink2 = new HyperlinkWidget(1, this);
	mHyperlink2->mLabel = "Click Here To Register!";
	mHyperlink2->SetFont(aFont);
	mHyperlink2->mColor = Color(0xffec91);
	mHyperlink2->mOverColor = Color(0xFFFFFF);
	mHyperlink2->mUnderlineSize = 2;
	mHyperlink2->SetVisible(true);

	mHyperlink1->Resize(180, 415, IMAGE_LOADERBAR->mWidth, IMAGE_LOADERBAR->mHeight);
	int aStrWdth = aFont->StringWidth(mHyperlink2->mLabel);
	mHyperlink2->Resize(0, 0, aStrWdth + 20, aFont->GetHeight() + 20);

	// The registration flags are read through gSexyApp, IsScreenSaver through mApp
	if (!gSexyApp->mIsRegistered)
	{
		if (!mApp->IsScreenSaver())
		{
			if (!gSexyApp->mBuildUnlocked)
				TrialFunction();
		}
		else
			mHyperlink2->SetVisible(false);
	}
	else
		mHyperlink2->SetVisible(false);

	mStinkyX = gStinkyStartX;
	m0xcc = 0;
	mBlendInCounter = 0;
}

TitleScreen::~TitleScreen()
{
	if (mHyperlink1)
		delete mHyperlink1;
	if (mHyperlink2)
		delete mHyperlink2;
}

void TitleScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mHyperlink2);
	theWidgetManager->AddWidget(mHyperlink1);
}

void TitleScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mHyperlink2);
	theWidgetManager->RemoveWidget(mHyperlink1);
}

void TitleScreen::Draw(Graphics* g)
{
	Widget::Draw(g);

	int anImgId = IMAGE_LOADERBAR_ID;
	int anXBtnDraw = 180;
	int anYBtnDraw = 415;
	if (mHyperlink1->mVisible)
	{
		anImgId = mHyperlink1->mIsOver ? IMAGE_LOADERBAROVER_ID : IMAGE_LOADERBAR_ID;
		if (mHyperlink1->mIsOver && mHyperlink1->mIsDown)
		{
			anXBtnDraw = 181;
			anYBtnDraw = 416;
		}
	}
	Image* aLoaderImg = GetImageById(anImgId);
	g->DrawImage(IMAGE_TITLEPAGE, 0, 0);

	Graphics g2(*g);

	g2.ClipRect(anXBtnDraw, anYBtnDraw, m0x94, aLoaderImg->mHeight);
	g2.DrawImage(aLoaderImg, anXBtnDraw, anYBtnDraw);

	if (mHyperlink1->mVisible)
	{
		int anAlpha = (mBlendInCounter * 255) / 8;

		if (anAlpha != 0)
		{
			g->SetColorizeImages(true);
			g->SetColor(Color(255, 255, 255, anAlpha));
			g->DrawImage(IMAGE_LOADERBARLOADING, 230, 415);
			g->SetColor(Color(255, 255, 255, 255- anAlpha));
			g->DrawImage(IMAGE_LOADERPLAY, anXBtnDraw, anYBtnDraw);
			g->SetColorizeImages(false);
		}
		else
		{
			g->DrawImage(IMAGE_LOADERPLAY, anXBtnDraw, anYBtnDraw);
		}
	}
	else
		g2.DrawImage(IMAGE_LOADERBARLOADING, 230, 415);

	int aStinkyCel = (mUpdateCnt / 4) % 10;
	if (mStinkyX < 510)
		g->DrawImageMirror(IMAGE_STINKY, mStinkyX, anYBtnDraw - 30, Rect(aStinkyCel * 80, 0, 80, 80), true);
	g->DrawImage(IMAGE_TITLEPAGEMASK, 384, 388);
	// The original centres these lines with DrawString at (mWidth - width) / 2 - 13
	if (mApp->IsScreenSaver())
	{
		SexyString aStr;
		aStr = "Starting Screensaver";
		g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
		g->SetColor(Color(0xffec91));
		int aStrWdth = g->GetFont()->StringWidth(aStr);
		g->DrawString(aStr, (mWidth - aStrWdth) / 2 - 13, 395);
	}
	else
	{
		// The registration flags are read through gSexyApp
		if (gSexyApp->mIsRegistered && !gSexyApp->mBuildUnlocked)
		{
			SexyString aStr = "THANKS FOR REGISTERING!";
			g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
			g->SetColor(Color(255, 255, 255));
			int aStrWdth = g->GetFont()->StringWidth(aStr);
			g->DrawString(aStr, (mWidth - aStrWdth) / 2 - 13, 470);
		}
		if (!gSexyApp->mIsRegistered)
		{
			g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
			g->SetColor(Color(255, 255, 255));
			int aStrWdth = g->GetFont()->StringWidth(m0xa4);
			g->DrawString(m0xa4, (mWidth - aStrWdth) / 2 - 13, 470);
		}
	}

	// The version is right-aligned 16 pixels from the edge. The original copies mProductVersion into a
	// temporary before the concatenation.
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(255, 255, 255));
	SexyString aVersionStr = "Version " + SexyString(mApp->mProductVersion);
	int aStrWdth = g->GetFont()->StringWidth(aVersionStr);
	g->DrawString(aVersionStr, mWidth - aStrWdth - 16, 211);
}

void TitleScreen::Update()
{
	Widget::Update();
	if (mBlendInCounter > 0)
		mBlendInCounter--;

	// The original reads the bar width and the app (through gSexyApp, not mApp) once, before the
	// progress call, and uses those copies for the rest of the function
	int aBarWidth = m0x98;
	WinFishApp* anApp = (WinFishApp*)gSexyApp;
	if (m0x94 < (int)(anApp->GetLoadingThreadProgress() * (double)aBarWidth))
	{
		if (m0x9c < 3)
			m0x9c++;
		m0x94 += m0x9c;
		m0xcc++;
		mStinkyX = gStinkyStartX + m0x94;
		if (m0x94 > aBarWidth)
		{
			m0x94 = aBarWidth;
			m0x9c--;
		}

		MarkDirty();
	}
	else if (m0xa1)
	{
		if (mStinkyX < mWidth)
		{
			if (m0x9c < 3)
				m0x9c++;
			m0xcc++;
			mStinkyX += m0x9c;
			MarkDirty();
		}
		if (anApp->IsScreenSaver() && mStinkyX > 510)
			anApp->TitleScreenIsFinished();
	}
	else if (anApp->mLoadingThreadCompleted)
	{
		m0xa1 = true;

		if (!anApp->IsScreenSaver())
		{
			mBlendInCounter = 8;
			mHyperlink1->SetVisible(true);
			MarkDirty();
		}
	}

	if (m0xa0 && anApp->CanShowRegisterDialog())
	{
		m0xa0 = false;
		anApp->DoRegisterDialog();
	}
	if (m0xc1 && anApp->CanShowRegisterDialog())
	{
		m0xc1 = false;
		anApp->DoTrialVersionExpiredDialog();
	}
	if (mUpdateCnt % 4 == 0)
		MarkDirty();
}

void Sexy::TitleScreen::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Widget::Resize(theX, theY, theWidth, theHeight);
	mHyperlink2->Resize(theX + (mWidth - mHyperlink2->mWidth) / 2, 350, mHyperlink2->mWidth, mHyperlink2->mHeight);
}

// Both button callbacks reach the app through gSexyApp, not mApp
void Sexy::TitleScreen::ButtonPress(int theId)
{
	gSexyApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::TitleScreen::ButtonDepress(int theId)
{
	switch (theId)
	{
	case 0:
		((WinFishApp*)gSexyApp)->TitleScreenIsFinished();
		break;
	case 1:
		m0xa0 = true; // Update opens the register dialog once loading is far enough
		break;
	}
}

void Sexy::TitleScreen::RegisterSuccessful()
{
	m0xc0 = false;
	mHyperlink2->SetVisible(false);
	if (!mApp->IsScreenSaver())
		mHyperlink1->SetVisible(m0x94 == m0x98);
	MarkDirty();
}

void Sexy::TitleScreen::TrialFunction()
{
	// The original reads the trial limits through gSexyApp, not mApp
	WinFishApp* anApp = (WinFishApp*)gSexyApp;
	if (anApp->mMaxTime > 0)
	{
		int aTimeRemain = anApp->mMaxTime - anApp->mTimesPlayed;
		if (aTimeRemain <= 0)
			m0xc0 = true;
		else if (aTimeRemain == 1)
			m0xa4 = "YOU HAVE 1 MINUTE LEFT IN THIS TRIAL!";
		else
			m0xa4 = StrFormat("YOU HAVE %d MINUTES LEFT IN THIS TRIAL!", aTimeRemain);
	}
	else if (anApp->mMaxPlays > 0)
	{
		int aPlays = anApp->mMaxPlays - anApp->mTimesPlayed;
		if (aPlays <= 0)
			m0xc0 = true;
		else if(aPlays == 1)
			m0xa4 = "YOU HAVE 1 FREE PLAY REMAINING!";
		else
			m0xa4 = StrFormat("YOU HAVE %d FREE PLAYS REMAINING!", aPlays);
	}
	else if (anApp->mMaxExecutions > 0)
	{
		// As the original (and WinFishApp::CheckTrialEnded): the current execution still counts as remaining
		int aExecs = anApp->mMaxExecutions - anApp->mTimesExecuted + 1;
		if (aExecs <= 0)
			m0xc0 = true;
		else if (aExecs == 1)
			m0xa4 = "YOU HAVE 1 FREE PLAY REMAINING!";
		else
			m0xa4 = StrFormat("YOU HAVE %d FREE PLAYS REMAINING!", aExecs);
	}
	if (m0xc0)
	{
		m0xc1 = true;
		mHyperlink2->SetVisible(false);
		mHyperlink1->SetVisible(false);
	}
}
