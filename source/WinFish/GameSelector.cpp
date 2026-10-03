#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/ButtonWidget.h>
#include <SexyAppFramework/DialogButton.h>

#include "GameSelector.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "ProfileMgr.h"
#include "Res.h"

// 0x5E10A4: a writable .data global (5) in the original, read from memory at each use
int gMerylSpeechFadeTime = 5;

Sexy::GameSelector::GameSelector(WinFishApp* theApp)
{
	mApp = theApp;
	mSparkleX = 0;
	mSparkleY = 0;
	// The original constructs the two label colours once and copies them into every button
	Color aLabelColor(0xff, 0xf0, 0);
	Color aLabelHiliteColor(200, 200, 0xff);
	mAdventureButton = new ButtonWidget(1, this);
	mAdventureButton->mDoFinger = true;
	mAdventureButton->mButtonImage = IMAGE_BLANK;
	mAdventureButton->mOverImage = IMAGE_BATTLETANKBUTTON;
	mAdventureButton->mDownImage = IMAGE_BATTLETANKBUTTOND;
	mAdventureButton->SetFont(FONT_JUNGLEFEVER15OUTLINE);
	mAdventureButton->mColors[ButtonWidget::COLOR_LABEL] = aLabelColor;
	mAdventureButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aLabelHiliteColor;
	mAdventureButton->Resize(357, 48, 217, 66);

	mVirtualTankButton = new ButtonWidget(2, this);
	mVirtualTankButton->mDoFinger = true;
	mVirtualTankButton->mButtonImage = IMAGE_BLANK;
	mVirtualTankButton->mOverImage = IMAGE_BATTLETANKBUTTON;
	mVirtualTankButton->mDownImage = IMAGE_BATTLETANKBUTTOND;
	mVirtualTankButton->SetFont(FONT_JUNGLEFEVER15OUTLINE);
	mVirtualTankButton->mLabel = "Virtual Tank";
	mVirtualTankButton->mColors[ButtonWidget::COLOR_LABEL] = aLabelColor;
	mVirtualTankButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aLabelHiliteColor;
	mVirtualTankButton->Resize(357, 287, 217, 66);

	mChallengeButton = new ButtonWidget(9, this);
	mChallengeButton->mDoFinger = true;
	mChallengeButton->mButtonImage = IMAGE_BLANK;
	mChallengeButton->mOverImage = IMAGE_MIDDLEBUTTON;
	mChallengeButton->mDownImage = IMAGE_MIDDLEBUTTOND;
	mChallengeButton->SetFont(FONT_JUNGLEFEVER15OUTLINE);
	mChallengeButton->mLabel = "Challenge";
	mChallengeButton->mColors[ButtonWidget::COLOR_LABEL] = aLabelColor;
	mChallengeButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aLabelHiliteColor;
	mChallengeButton->Resize(359, 212, 213, 48);

	mTimeTrialButton = new ButtonWidget(8, this);
	mTimeTrialButton->mDoFinger = true;
	mTimeTrialButton->mButtonImage = IMAGE_BLANK;
	mTimeTrialButton->mOverImage = IMAGE_MIDDLEBUTTON;
	mTimeTrialButton->mDownImage = IMAGE_MIDDLEBUTTOND;
	mTimeTrialButton->SetFont(FONT_JUNGLEFEVER15OUTLINE);
	mTimeTrialButton->mLabel = "Time Trial";
	mTimeTrialButton->mColors[ButtonWidget::COLOR_LABEL] = aLabelColor;
	mTimeTrialButton->mColors[ButtonWidget::COLOR_LABEL_HILITE] = aLabelHiliteColor;
	mTimeTrialButton->Resize(359, 142, 213, 48);

	mOptionsButton = MakeDialogButton2(3, this, "Options", IMAGE_LEFTBUTTON);
	mOptionsButton->Resize(325, 412, 92, mOptionsButton->mHeight);

	mQuitButton = MakeDialogButton2(4, this, "Quit", IMAGE_RIGHTBUTTON);
	mQuitButton->Resize(514, 412, 89, mQuitButton->mHeight);

	mHallOfFameButton = MakeDialogButton2(6, this, "Hall of Fame", IMAGE_MAINBUTTON);
	mHallOfFameButton->Resize(401, 380, 127, mHallOfFameButton->mHeight);

	mHelpButton = MakeDialogButton2(7, this, "Help", IMAGE_CENTERBUTTON);
	mHelpButton->Resize(419, 412, 93, mHelpButton->mHeight);

	mGameSelectorOverlay = new GameSelectorOverlay(this);
	mGameSelectorOverlay->Resize(0, 0, 640, 480);

	mMerylBlinkTimer = mApp->mSeed->Next() % 150 + 100;
	mMerylFlopTimer = mApp->mSeed->Next() % 500 + 200;

	mNotYouButton = NULL;

	if (!mApp->mGameNotPlayed && !mApp->IsSongPlaying(2, 0))
	{
		mApp->StopMusic();
		mApp->PlayMusic(2, 0);
	}
	if (mApp->mCurrentProfile)
		mApp->mCurrentProfile->ResetAfterFinalTank();

	UpdateAdventureButton();
	mHoverId = 0;
	mSpeechFadeTimer = 0;
	mPreviousHoverId = 0;
	mHoverChangeTimer = 0;

	mSandboxCheatCode = new CheatCode();
	mSandboxCheatCode->AddKey(KEYCODE_UP);
	mSandboxCheatCode->AddKey(KEYCODE_UP);
	mSandboxCheatCode->AddKey(KEYCODE_DOWN);
	mSandboxCheatCode->AddKey(KEYCODE_DOWN);
	mSandboxCheatCode->AddKey(KEYCODE_LEFT);
	mSandboxCheatCode->AddKey(KEYCODE_RIGHT);
	mSandboxCheatCode->AddKey(KEYCODE_LEFT);
	mSandboxCheatCode->AddKey(KEYCODE_RIGHT);
	mSandboxCheatCode->AddChar('b');
	mSandboxCheatCode->AddChar('a');
	mGiveCheatCode = new CheatCode("give");

	mSparkleAnimTimer = 0;
}

Sexy::GameSelector::~GameSelector()
{
	if (mAdventureButton)
		delete mAdventureButton;
	if (mVirtualTankButton)
		delete mVirtualTankButton;
	if (mOptionsButton)
		delete mOptionsButton;
	if (mQuitButton)
		delete mQuitButton;
	if (mChallengeButton)
		delete mChallengeButton;
	if (mTimeTrialButton)
		delete mTimeTrialButton;
	if (mHallOfFameButton)
		delete mHallOfFameButton;
	if (mHelpButton)
		delete mHelpButton;
	if (mGameSelectorOverlay)
		delete mGameSelectorOverlay;
	if (mSandboxCheatCode)
		delete mSandboxCheatCode;
	if (mGiveCheatCode)
		delete mGiveCheatCode;
	if (mNotYouButton)
	{
		delete mNotYouButton;
		mNotYouButton = NULL;
	}
}

void Sexy::GameSelector::Update()
{
	Widget::Update();
	mMerylBlinkTimer--;
	if (mMerylBlinkTimer <= 0)
	{
		int aVal;
		if (mApp->mSeed->Next() % 7 == 0)
			aVal = mApp->mSeed->Next() % 15 + 15;
		else
			aVal = mApp->mSeed->Next() % 150 + 100;
		mMerylBlinkTimer = aVal;
	}

	mMerylFlopTimer--;
	if (mMerylFlopTimer <= 0)
		mMerylFlopTimer = mApp->mSeed->Next() % 500 + 200;

	if (mSparkleAnimTimer > 0)
	{
		mSparkleAnimTimer++;
		if (mSparkleAnimTimer >= 60)
			mSparkleAnimTimer = 0;
	}
	else if (mSparkleAnimTimer == 0 && mApp->mCurrentProfile && mApp->mCurrentProfile->m0x80 && mUpdateCnt > 40)
	{
		mSparkleAnimTimer = 1;
		mSparkleX = 170;
		mSparkleY = 310;
		// As the original: x is drawn before y, and each try is tested with GetImagePixel
		int aSparkleX, aSparkleY;
		for (int aCnt = 0; aCnt < 100; aCnt++)
		{
			aSparkleX = Rand() % IMAGE_BONUSAWARD->mWidth;
			aSparkleY = Rand() % IMAGE_BONUSAWARD->mHeight;
			if ((GetImagePixel((MemoryImage*)IMAGE_BONUSAWARD, aSparkleX, aSparkleY) & 0xff000000) == 0xff000000)
				break;
		}

		mSparkleX += aSparkleX;
		mSparkleY += aSparkleY;
		mSparkleX += IMAGE_SPARKLE->GetCelWidth() / -2;
		mSparkleY += IMAGE_SPARKLE->GetCelHeight() / -2;
	}

	MarkDirty();
	if (mAdventureButton->mIsOver || mAdventureButton->mIsDown)
		UpdateHover(1);
	else if (mTimeTrialButton->mIsOver || mTimeTrialButton->mIsDown)
		UpdateHover(2);
	else if (mChallengeButton->mIsOver || mChallengeButton->mIsDown)
		UpdateHover(3);
	else if (mVirtualTankButton->mIsOver || mVirtualTankButton->mIsDown)
		UpdateHover(4);
	else if (mApp->mCurrentProfile && mApp->mCurrentProfile->mBonusItemId >= 6 &&
		Rect(220, 340, 50, 50).Contains(mWidgetManager->mLastMouseX, mWidgetManager->mLastMouseY) &&
		mApp->mDialogMap.size() == 0)
		// The original tests m0x80 unsigned here (0x51EE4A: cmp 0; jbe)
		UpdateHover((uint)mApp->mCurrentProfile->m0x80 > 0 ? 6 : 5);
	else
		UpdateHover(0);

	if (mSpeechFadeTimer > 0)
		mSpeechFadeTimer--;

	if (mNotYouButton->mVisible)
	{
		if (!mApp->mCurrentProfile || mHoverId != 0)
		{
			mNotYouButton->SetVisible(false);
			MarkDirty();	
			UpdateAdventureButton();
			return;
		}
	}

	if(!mNotYouButton->mVisible)
	if (mApp->mCurrentProfile && mHoverId == 0)
	{
		mNotYouButton->SetVisible(true);
		MarkDirty();
	}

	UpdateAdventureButton();
}

void Sexy::GameSelector::Draw(Graphics* g)
{
	g->DrawImage(IMAGE_SELECTORSCREEN, 0, 0);
	UserProfile* aProf = mApp->mCurrentProfile;
	if (aProf && aProf->mBonusItemId > 5)
	{
		g->DrawImage(aProf->m0x80 != 0 ? IMAGE_BONUSAWARD2 : IMAGE_BONUSAWARD, 170, 310);
		if (mSparkleAnimTimer > 0 && mSparkleAnimTimer <= IMAGE_SPARKLE->mNumCols)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->SetColorizeImages(true);
			g->SetColor(Color(0xffffff));
			g->DrawImageCel(IMAGE_SPARKLE, mSparkleX, mSparkleY, mSparkleAnimTimer - 1);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}
	}

	g->DrawImageAnim(IMAGE_MERYLBLINK, 139, 196, mMerylBlinkTimer);
	g->DrawImageAnim(IMAGE_MERYLTAIL, 39, 301, mMerylFlopTimer);

	if (mSpeechFadeTimer > 0)
	{
		int aVal2 = mSpeechFadeTimer * 255 / gMerylSpeechFadeTime;
		DrawMerylSpeak(g, mHoverId, 255 - aVal2, true);
		DrawMerylSpeak(g, mPreviousHoverId, aVal2, false);
	}
	else
		DrawMerylSpeak(g, mHoverId, 255, true);
}

// A static helper in the original (0x5175B0; LTCG gave it a register calling convention): sets the
// label colour and moves a button's label one pixel down and right while the button is held down
static void SetupButtonLabel(int* theOffsetX, int* theOffsetY, Sexy::ButtonWidget* theButton, Sexy::Graphics* g)
{
	g->SetColor(Sexy::Color(225, 250, 250, 255));
	if (theButton->mIsDown && theButton->mIsOver)
	{
		*theOffsetX = 1;
		*theOffsetY = 1;
	}
	else
	{
		*theOffsetX = 0;
		*theOffsetY = 0;
	}
}

void Sexy::GameSelector::DrawOverlay(Graphics* g)
{
	// The original reads mApp->mCurrentProfile again at each use
	if (!mApp->mCurrentProfile)
		return;

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	SexyString anAdvString;
	int anOffsetX, anOffsetY;
	SetupButtonLabel(&anOffsetX, &anOffsetY, mAdventureButton, g);
	if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
		anAdvString = GetCyraxEndGameString(mApp->mCurrentProfile->mCyraxNum + 1);
	else if (mApp->mCurrentProfile->mLevel == 6)
		anAdvString = StrFormat("Bonus Level %d", mApp->mCurrentProfile->mTank);
	else if (mApp->mCurrentProfile->mTank != 1 || mApp->mCurrentProfile->mLevel != 1)
		anAdvString = StrFormat("Tank %d-%d", mApp->mCurrentProfile->mTank, mApp->mCurrentProfile->mLevel);

	if (anAdvString.size() > 0)
		g->DrawString(anAdvString, anOffsetX - g->GetFont()->StringWidth(anAdvString) / 2 + 465, anOffsetY + 107);

	SetupButtonLabel(&anOffsetX, &anOffsetY, mVirtualTankButton, g);
	if (mApp->mCurrentProfile->mShells > 0)
	{
		// The original formats into a char buffer; each call below makes its own temporary string
		char aShellsStr[256];
		sprintf(aShellsStr, "%d Shells", mApp->mCurrentProfile->mShells);
		int aStrWdth = FONT_JUNGLEFEVER10OUTLINE->StringWidth(aShellsStr);
		g->DrawString(aShellsStr, anOffsetX - aStrWdth / 2 + 465, anOffsetY + 346);
	}
}

void Sexy::GameSelector::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mAdventureButton);
	theWidgetManager->AddWidget(mVirtualTankButton);
	theWidgetManager->AddWidget(mOptionsButton);
	theWidgetManager->AddWidget(mQuitButton);
	theWidgetManager->AddWidget(mChallengeButton);
	theWidgetManager->AddWidget(mTimeTrialButton);
	theWidgetManager->AddWidget(mHallOfFameButton);
	theWidgetManager->AddWidget(mHelpButton);

	if (!mNotYouButton)
	{
		// The original reads FONT_TINY once, before the new, and measures with it
		Font* aFont = FONT_TINY;
		mNotYouButton = new HyperlinkWidget(9999, this);
		mNotYouButton->SetFont(aFont);
		mNotYouButton->mColor = Color(0, 0, 120);
		mNotYouButton->mOverColor = Color(100, 100, 220);
		mNotYouButton->mDoFinger = true;
		mNotYouButton->mLabel = "If this is not you, click here.";
		mNotYouButton->mUnderlineSize = 1;
		mNotYouButton->SetVisible(true);
		int aStrWdth = aFont->StringWidth(mNotYouButton->mLabel);
		mNotYouButton->Resize(97, 90, aStrWdth, aFont->GetHeight());
		// The label only sizes the link: DrawMerylSpeak draws the text itself, fading with the speech bubble
		mNotYouButton->mLabel = "";
	}

	theWidgetManager->AddWidget(mNotYouButton);
	theWidgetManager->AddWidget(mGameSelectorOverlay);
}

void Sexy::GameSelector::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mAdventureButton);
	theWidgetManager->RemoveWidget(mVirtualTankButton);
	theWidgetManager->RemoveWidget(mOptionsButton);
	theWidgetManager->RemoveWidget(mQuitButton);
	theWidgetManager->RemoveWidget(mChallengeButton);
	theWidgetManager->RemoveWidget(mTimeTrialButton);
	theWidgetManager->RemoveWidget(mHallOfFameButton);
	theWidgetManager->RemoveWidget(mHelpButton);
	if(mNotYouButton)
		theWidgetManager->RemoveWidget(mNotYouButton);
	theWidgetManager->RemoveWidget(mGameSelectorOverlay);
}

void Sexy::GameSelector::OrderInManagerChanged()
{
	mWidgetManager->PutInfront(mGameSelectorOverlay, this);
	if(mNotYouButton)
		mWidgetManager->PutInfront(mNotYouButton, this);
	mWidgetManager->PutInfront(mHelpButton, this);
	mWidgetManager->PutInfront(mHallOfFameButton, this);
	mWidgetManager->PutInfront(mTimeTrialButton, this);
	mWidgetManager->PutInfront(mChallengeButton, this);
	mWidgetManager->PutInfront(mQuitButton, this);
	mWidgetManager->PutInfront(mOptionsButton, this);
	mWidgetManager->PutInfront(mVirtualTankButton, this);
	mWidgetManager->PutInfront(mAdventureButton, this);
}

void Sexy::GameSelector::KeyChar(SexyChar theChar)
{
	if (mApp->mDebugKeysEnabled)
		return;
	if (mSandboxCheatCode->CheckCodeActivated(theChar))
		SandboxActivated();
	else if (mGiveCheatCode->CheckCodeActivated(theChar))
		mApp->DoGiveDialog();
}

void Sexy::GameSelector::KeyDown(KeyCode theKey)
{
	if (mApp->mDebugKeysEnabled)
		return;
	if (mSandboxCheatCode->CheckCodeActivated(theKey))
		SandboxActivated();
	else if (mGiveCheatCode->CheckCodeActivated(theKey))
		mApp->DoGiveDialog();
}

void Sexy::GameSelector::ButtonPress(int theId, int theClickCount)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::GameSelector::ButtonDepress(int theId)
{
	mApp->mRelaxMode = false;
	if (theId == 1)
	{
		mApp->mGameMode = GAMEMODE_ADVENTURE;
		mApp->RemoveGameSelector();
		mApp->SwitchToBoard(true, true);
	}
	else if (theId == 2)
	{
		mApp->mGameMode = GAMEMODE_VIRTUAL_TANK;
		mApp->RemoveGameSelector();
		mApp->SwitchToBoard(true, true);
	}
	else if (theId == 3)
	{
		mApp->DoOptionsDialog(true);
	}
	else if (theId == 4)
	{
		mApp->DoQuitDialog();
	}
	else if (theId == 6)
	{
		mApp->SwitchToHighScoreScreen();
	}
	else if (theId == 7)
	{
		mApp->SwitchToHelpScreen(false);
	}
	else if (theId == 8)
	{
		UserProfile* aProf = mApp->mCurrentProfile;
		if (aProf->mTank <= 1 && !aProf->mFinishedGame)
		{
			mApp->DoDialog(14, true, "Not Yet!", "You\'ll need to complete a tank in Adventure Mode before this option becomes available.", "OK", Dialog::BUTTONS_FOOTER);
			return;
		}
		mApp->mGameMode = GAMEMODE_TIME_TRIAL;
		mApp->RemoveGameSelector();
		if (mApp->LoadBoardGame())
			return;
		mApp->SwitchToTankScreen();
	}
	else if (theId == 9)
	{
		if (mApp->mCurrentProfile->mFinishedGame)
		{
			mApp->mGameMode = GAMEMODE_CHALLENGE;
			mApp->RemoveGameSelector();
			if (mApp->LoadBoardGame())
				return;
			mApp->SwitchToTankScreen();
		}
		else
			mApp->DoDialog(14, true, "Not Yet!", "You\'ll need to beat Adventure Mode before this option becomes available.", "OK", Dialog::BUTTONS_FOOTER);
	}
	else if (theId == 9999)
	{
		mApp->DoWhoAreYouDialog();
	}
}

void Sexy::GameSelector::MouseDown(int x, int y, int theClickCount)
{
	if (x >= 20 && x < 310 && y >= 150 && y < 510 && mMerylFlopTimer > 15)
		mMerylFlopTimer = 15;
}

void Sexy::GameSelector::DrawMerylSpeak(Graphics* g, int theHoverId, int theAlphaValue, bool theDrawSpeechBubble)
{
	// The original reads mApp->mCurrentProfile again at each use
	SexyString anUserName;
	int aTank = 1;
	int aLevel = 1;
	bool aFinishedGame = false;
	if (mApp->mCurrentProfile)
	{
		anUserName = mApp->mCurrentProfile->mUserName;
		aTank = mApp->mCurrentProfile->mTank;
		aLevel = mApp->mCurrentProfile->mLevel;
		aFinishedGame = mApp->mCurrentProfile->mFinishedGame;
	}
	if (theDrawSpeechBubble)
	{
		if(anUserName.empty() && theHoverId <= 0)
			return;
		g->DrawImage(IMAGE_SELECTORSPEECHBUBBLE, 45, 20);
	}

	if (theHoverId > 0)
	{
		Rect aRect(64, 40, 214, 60);
		g->SetFont(FONT_BLAMBOTPRO8);
		g->SetColor(Color(0,0,100, theAlphaValue));
		SexyString aStrToDraw;
		switch (theHoverId)
		{
		case 1:
			if (aTank == 1 && aLevel == 1 && !aFinishedGame)
				aStrToDraw = "New to Insaniquarium?\nClick here to start your aquatic adventure!";
			else
				aStrToDraw = "Feed fish and fight aliens!\nClick here to continue\nyour adventure...";
			break;
		case 2:
			aStrToDraw = "How much money can you earn before time runs out?";
			break;
		case 3:
			aStrToDraw = "Can you fend off the increasingly difficult aliens?";
			break;
		case 4:
			aStrToDraw = "Buy and raise your own\ncustom fish, then use them\nas a screensaver!";
			break;
		case 5:
			aStrToDraw = "Wow!  Nice trophy!";
			break;
		case 6:
			aStrToDraw = "The pets wanted you to have this solid gold trophy since you are now the undisputed champion of all Insaniquarium!";
			break;
		default:
			break;
		}

		int aHght = GetWordWrappedHeight(g, aRect.mWidth, aStrToDraw, 20);
		aRect.mY += (aRect.mHeight - aHght) / 2;
		WriteWordWrapped(g, aRect, aStrToDraw, 20, 0);
	}
	else if (!anUserName.empty())
	{
		// The original builds the name with its "!" in a second string
		SexyString aNameStr;
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, theAlphaValue));

		if (mApp->mCurrentProfile && !mApp->mCurrentProfile->m0x4c)
			g->DrawImage(IMAGE_WELCOMETO, (250 - IMAGE_WELCOMETO->mWidth) / 2 + 45, 33);
		else
			g->DrawImage(IMAGE_WELCOMEBACK, (250 - IMAGE_WELCOMETO->mWidth) / 2 + 45, 33);
		g->SetColorizeImages(false);

		aNameStr = anUserName + "!";
		// FONT_BLAMBOTPRO15 is read once: it measures the name, then becomes the font
		Font* aFont = FONT_BLAMBOTPRO15;
		int aNameX = (250 - aFont->StringWidth(aNameStr)) / 2 + 45;
		g->SetFont(aFont);
		g->SetColor(Color(0, 0, 100, theAlphaValue));
		g->DrawString(aNameStr, aNameX, 77);
		if (mNotYouButton->mVisible)
		{
			g->SetFont(mNotYouButton->mFont);
			Color theColor = mNotYouButton->mIsOver ? mNotYouButton->mOverColor : mNotYouButton->mColor;
			theColor.mAlpha = theAlphaValue;
			g->SetColor(theColor);
			int aY = (g->GetFont()->GetAscent() + mNotYouButton->mHeight) / 2 + mNotYouButton->mY - 1;
			g->DrawString("If this is not you, click here.", mNotYouButton->mX, aY);
		}
	}
}

void Sexy::GameSelector::UpdateHover(int theHover)
{
	if (mHoverId != theHover)
	{
		if (mHoverChangeTimer++ >= 5)
		{
			mSpeechFadeTimer = gMerylSpeechFadeTime;
			mPreviousHoverId = mHoverId;
			mHoverId = theHover;
			mHoverChangeTimer = 0;
		}
	}
}

void Sexy::GameSelector::UpdateAdventureButton()
{
	UserProfile* aProf = mApp->mCurrentProfile;
	if (aProf)
	{
		if (aProf->mFinishedGame)
		{
			mAdventureButton->mLabel = "Bonus Adventure";
			return;
		}
		if(aProf->mTank == 1 && aProf->mLevel == 1)
		{
			mAdventureButton->mLabel = "Start Adventure";
			return;
		}
	}
	mAdventureButton->mLabel = "Adventure";
}

void Sexy::GameSelector::SandboxActivated()
{
	if (mApp->mCurrentProfile && mApp->mCurrentProfile->mBonusItemId >= 6)
	{
		mApp->mGameMode = GAMEMODE_SANDBOX;
		if (mApp->mCurrentProfile->m0xb4 < 11)
			mApp->mCurrentProfile->m0xb4 = 11;
		mApp->RemoveGameSelector();
		mApp->StartGame();
	}
}

// Called by WinFishApp after the current user profile changes; empty in the original (0x4E9E00, a shared bare ret)
void Sexy::GameSelector::ProfileChanged()
{
}

void GameSelectorOverlay::Draw(Sexy::Graphics* g)
{
	mScreen->DrawOverlay(g);
}
