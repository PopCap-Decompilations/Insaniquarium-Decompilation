#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>

#include "HatchScreen.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "ProfileMgr.h"
#include "BubbleMgr.h"
#include "Res.h"

Sexy::HatchScreen::HatchScreen(WinFishApp* theApp, int thePetId)
{
	mApp = theApp;
	mBubbleMgr = new BubbleMgr();

	mBubbleMgr->SetBubbleBounds(Rect(190, 220, 258, 200));
	mBubbleMgr->SetBubbleConfig(10, 5);
	mBubbleMgr->UpdateALot();
	m0x94 = 0;
	mPetId = thePetId;
	theApp->m0x884 = true;
	mX = 0;
	mY = 0;
	mWidth = mApp->mWidth;
	mHeight = mApp->mHeight;

	mContinueButton = MakeDialogButton2(99, this, "Please Wait...", IMAGE_MAINBUTTON);
	mContinueButton->SetFont(FONT_JUNGLEFEVER12OUTLINE);
	mContinueButton->mMouseVisible = false;
	mContinueButton->mColors[0] = Color(255, 240, 0, 255);
	mContinueButton->Resize(186, 445, 264, mContinueButton->mHeight);
	mApp->mWidgetManager->BringToFront(mContinueButton);

	mMenuButton = MakeDialogButton2(100, this, "Menu", IMAGE_MAINBUTTON);
	mMenuButton->Resize(525, 4, 80, mMenuButton->mHeight);
	if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 2)
		mMenuButton->mVisible = false;

	mOverlay = new HatchScreenOverlay(this);

	m0x9c = 0;
	m0xa0 = 0;
	m0xa4 = 480;
	mApp->StopMusic();
	mApp->PlayMusic(2, 54);
}

Sexy::HatchScreen::~HatchScreen()
{
	if (mBubbleMgr)
		delete mBubbleMgr;
	if (mContinueButton)
		delete mContinueButton;
	if (mMenuButton)
		delete mMenuButton;
	if (mOverlay)
		delete mOverlay;
}


void Sexy::HatchScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mContinueButton);
	theWidgetManager->AddWidget(mMenuButton);
	theWidgetManager->AddWidget(mOverlay);
}

void Sexy::HatchScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMenuButton);
	theWidgetManager->RemoveWidget(mContinueButton);
	theWidgetManager->RemoveWidget(mOverlay);
}

void Sexy::HatchScreen::Update()
{
	mBubbleMgr->Update();
	MarkDirty();
	m0x9c = mApp->mSeed->Next() % 4;
	m0xa0 = mApp->mSeed->Next() % 2;

	if (mIsDown && m0x94 < 140)
		m0x94 = 140;
	else if (m0x94 == 141)
	{
		if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
			mApp->PlaySample(SOUND_EVILLAFF);
		else
			mApp->PlaySample(SOUND_HATCH);
	}

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		if (m0x94 == 28 || m0x94 == 48 || m0x94 == 68)
			mApp->PlaySample(SOUND_BUY);
		if(m0x94 == 88)
			mApp->PlaySample(SOUND_GROW);
	}

	if (m0x94 >= 220 && mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		int aVal = m0x94 % 16;
		if (aVal <= 3)
			m0xa4--;
		else if (aVal >= 8 && aVal <= 11)
			m0xa4++;
	}
	else if(m0x94 >= 140 && mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		if (m0x94 <= 160)
			m0xa4 -= 12;
		else if (m0x94 <= 170)
			m0xa4 -= 8;
		else if (m0x94 <= 180)
			m0xa4 -= 5;
		else if (m0x94 <= 190)
			m0xa4 -= 2;
		else
			m0xa4--;
	}

	if (m0x94 == 170)
	{
		mContinueButton->mLabel = "Click Here to Continue";
		mContinueButton->mMouseVisible = true;
	}

	m0x94++;
}

void Sexy::HatchScreen::Draw(Graphics* g)
{
	// As the original: three strings shared by the pet name and the description lines
	SexyString aStr1 = "";
	SexyString aStr2 = "";
	SexyString aStr3 = "";

	g->DrawImage(IMAGE_HATCHSCREEN, 0, 0);
	g->DrawImageBox(Rect(20, 0, 600, IMAGE_SCREENTITLE->mHeight), IMAGE_SCREENTITLE);

	if (mMenuButton->mVisible)
		g->DrawImageBox(Rect(mMenuButton->mX - 1, mMenuButton->mY - 1, mMenuButton->mWidth + 2, IMAGE_SCREENTITLEHOLE->mHeight), IMAGE_SCREENTITLEHOLE);
	
	g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
	g->SetColor(Color(0xff, 200, 0, 0xff));

	g->DrawString("You have found:", 215, 25);

	int aShakeTimer = m0x94 % 32;
	int anYOffset = 0;
	if (aShakeTimer < 4)
		anYOffset = 0;
	else if (aShakeTimer < 8)
		anYOffset = -1;
	else if (aShakeTimer < 12)
		anYOffset = -2;
	else if (aShakeTimer < 16)
		anYOffset = -1;
	else if (aShakeTimer < 20)
		anYOffset = 0;
	else if (aShakeTimer < 24)
		anYOffset = 1;
	else if (aShakeTimer < 28)
		anYOffset = 2;
	else if (aShakeTimer < 32)
		anYOffset = 1;

	Graphics g2(*g);
	if (m0x94 <= 80)
	{
		int aCel = 0;
		if (m0x94 >= 56)
			aCel = (m0x94 - 56) / 2;
		g->DrawImageCel(IMAGE_EGGCRACK1, 258, anYOffset + 60, aCel);
	}
	else if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		int aCel = 0;
		if (m0x94 >= 140)
			aCel = 9;
		else if (m0x94 >= 120)
			aCel = (m0x94 - 120) / 2;

		if (m0x94 > 160)
		{
			m0x9c = 0;
			m0xa0 = 0;
		}
		g->DrawImageCel(IMAGE_EGGCRACK2, m0x9c + 258, m0xa0 + anYOffset + 60, aCel);
	}
	else if (m0x94 <= 140)
	{
		int aCel = 0;
		if (m0x94 >= 120)
			aCel = (m0x94 - 120) / 2;
		g->DrawImageCel(IMAGE_EGGCRACK2, m0x9c + 258, m0xa0 + anYOffset + 60, aCel);
	}
	else
	{
		// The original tests the pets in this order, as if-chains (compares, no jump tables)
		if (mPetId == PET_AMP)
			g2.ClipRect(236, 100, 160, 60);
		else if (mPetId == PET_CLYDE)
			g2.ClipRect(278, 100, 80, 80);
		else if (mPetId == PET_NIKO)
			g2.ClipRect(278, 70, 80, 80);
		else if (mPetId == PET_STINKY)
			g2.ClipRect(278, 100, 80, 80);
		else
			g2.ClipRect(278, 90, 80, 80);

		int aXOffset;
		if (mPetId == PET_SEYMOUR || mPetId == PET_SHRAPNEL ||
			mPetId == PET_CLYDE || mPetId == PET_RHUBARB || mPetId == PET_BRINKLEY)
			aXOffset = (m0x94 % 40) / 4;
		else if (mPetId == PET_VERT || mPetId == PET_NIKO)
		{
			aXOffset = m0x94 % 20;
			if (aXOffset >= 10)
				aXOffset = 19 - aXOffset;
		}
		else
			aXOffset = (m0x94 % 20) / 2;

		// An id without a pet image (outside 0-23) draws nothing
		if (mPetId == PET_ITCHY)
			g2.DrawImage(IMAGE_ITCHY, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_STINKY)
			g2.DrawImage(IMAGE_STINKY, 278 - aXOffset * 80, 100);
		else if (mPetId == PET_ZORF)
			g2.DrawImage(IMAGE_ZORF, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_PREGO)
			g2.DrawImage(IMAGE_PREGO, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_VERT)
			g2.DrawImage(IMAGE_VERT, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_WADSWORTH)
			g2.DrawImage(IMAGE_WADSWORTH, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_MERYL)
			g2.DrawImage(IMAGE_MERYL, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_RUFUS)
			g2.DrawImage(IMAGE_RUFUS, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_NIKO)
			g2.DrawImage(IMAGE_NIKO, 278 - aXOffset * 80, 70);
		else if (mPetId == PET_CLYDE)
			g2.DrawImage(IMAGE_CLYDE, 278 - aXOffset * 80, 100);
		else if (mPetId == PET_PRESTO)
			g2.DrawImage(IMAGE_PRESTO, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_SHRAPNEL)
			g2.DrawImage(IMAGE_SHRAPNEL, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_RHUBARB)
			g2.DrawImage(IMAGE_RHUBARB, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_SEYMOUR)
			g2.DrawImage(IMAGE_SEYMOUR, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_NIMBUS)
			g2.DrawImage(IMAGE_NIMBUS, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_AMP)
			g2.DrawImage(IMAGE_AMP, 236 - aXOffset * 160, 100);
		else if (mPetId == PET_ANGIE)
			g2.DrawImage(IMAGE_ANGIE, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_GUMBO)
			g2.DrawImage(IMAGE_GUMBO, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_GASH)
			g2.DrawImage(IMAGE_GASH, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_BLIP)
			g2.DrawImage(IMAGE_BLIP, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_BRINKLEY)
			g2.DrawImage(IMAGE_BRINKLEY, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_NOSTRADAMUS)
			g2.DrawImage(IMAGE_NOSTRADAMUS, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_WALTER)
			g2.DrawImage(IMAGE_WALTER, 278 - aXOffset * 80, 90);
		else if (mPetId == PET_STANLEY)
			g2.DrawImage(IMAGE_STANLEY, 278 - aXOffset * 80, 90);
		g2.SetColorizeImages(false);

		int aYDesc = 260;
		if (mPetId == PET_ITCHY)
			aStr1 = "ITCHY the Swordfish";
		else if (mPetId == PET_STINKY)
			aStr1 = "STINKY the Snail";
		else if (mPetId == PET_ZORF)
			aStr1 = "ZORF the Sea Horse";
		else if (mPetId == PET_PREGO)
			aStr1 = "PREGO the Momma Fish";
		else if (mPetId == PET_VERT)
			aStr1 = "VERT the Skeleton";
		else if (mPetId == PET_WADSWORTH)
			aStr1 = "WADSWORTH the Whale";
		else if (mPetId == PET_MERYL)
			aStr1 = "MERYL the Mermaid";
		else if (mPetId == PET_RUFUS)
			aStr1 = "RUFUS the Fiddler Crab";
		else if (mPetId == PET_NIKO)
			aStr1 = "NIKO the Oyster";
		else if (mPetId == PET_CLYDE)
			aStr1 = "CLYDE the Jellyfish";
		else if (mPetId == PET_PRESTO)
			aStr1 = "PRESTO the Tadpole";
		else if (mPetId == PET_SHRAPNEL)
			aStr1 = "SHRAPNEL the Robot Fish";
		else if (mPetId == PET_RHUBARB)
			aStr1 = "RHUBARB the Hermit Crab";
		else if (mPetId == PET_SEYMOUR)
			aStr1 = "SEYMOUR the Turtle";
		else if (mPetId == PET_NIMBUS)
			aStr1 = "NIMBUS the Manta Ray";
		else if (mPetId == PET_AMP)
			aStr1 = "AMP the Electric Eel";
		else if (mPetId == PET_ANGIE)
			aStr1 = "ANGIE the Angelfish";
		else if (mPetId == PET_GUMBO)
			aStr1 = "GUMBO the Angler";
		else if (mPetId == PET_GASH)
			aStr1 = "GASH the Shark";
		else if (mPetId == PET_BLIP)
			aStr1 = "BLIP the Porpoise";
		else if (mPetId == PET_BRINKLEY)
			aStr1 = "BRINKLEY";
		else if (mPetId == PET_NOSTRADAMUS)
			aStr1 = "NOSTRADAMUS the Nose";
		else if (mPetId == PET_WALTER)
			aStr1 = "WALTER the Penguin";
		else if (mPetId == PET_STANLEY)
			aStr1 = "STANLEY the Startlingly";
		else if (mPetId == 999)
			aStr1 = "Evil Alien Mastermind";

		g->SetColor(Color(255, 200, 0, 255));
		g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
		if (mPetId == PET_RHUBARB || mPetId == PET_WADSWORTH ||
			mPetId == PET_SHRAPNEL || mPetId == PET_NOSTRADAMUS)
			g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
		else if (mPetId == PET_BRINKLEY || mPetId == PET_STANLEY)
		{
			g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
			if (mPetId == PET_BRINKLEY)
				aStr2 = "the Scuba Diving Elephant";
			else if (mPetId == PET_STANLEY)
				aStr2 = "Small Sea Serpent";

			g->DrawString(aStr2, ((650 - g->GetFont()->StringWidth(aStr2)) >> 1) - 3, 265);
			aYDesc = 245;
		}

		g->DrawString(aStr1, ((650 - g->GetFont()->StringWidth(aStr1)) >> 1) - 3, aYDesc);
	}

	if (m0x94 > 170)
	{
		g->SetColor(Color(255, 255, 255, 255));
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);

		if (mPetId == PET_ITCHY)
		{
			aStr1 = "ITCHY helps you by attacking";
			aStr2 = "aliens when they appear.";
		}
		else if (mPetId == PET_STINKY)
		{
			aStr1 = "STINKY roams around the";
			aStr2 = "bottom of your tank, catching";
			aStr3 = "any coins you may have missed.";
		}
		else if (mPetId == PET_ZORF)
		{
			aStr1 = "ZORF gives you a hand in";
			aStr2 = "keeping your fish fed.";
		}
		else if (mPetId == PET_PREGO)
		{
			aStr1 = "PREGO helps populate your";
			aStr2 = "tank by giving birth to a new";
			aStr3 = "baby guppy every so often.";
		}
		else if (mPetId == PET_VERT)
		{
			aStr1 = "VERT drops gold coins just like";
			aStr2 = "a large guppy, but doesn't need";
			aStr3 = "fish food to survive.";
		}
		else if (mPetId == PET_WADSWORTH)
		{
			aStr1 = "WADSWORTH helps by sheltering";
			aStr2 = "your baby and medium guppies";
			aStr3 = "from hungry aliens.";
		}
		else if (mPetId == PET_MERYL)
		{
			aStr1 = "MERYL's song cheers up all the";
			aStr2 = "guppies in the tank, making";
			aStr3 = "them drop coins faster.";
		}
		else if (mPetId == PET_RUFUS)
		{
			aStr1 = "RUFUS does heavy damage to";
			aStr2 = "enemies you've lured to the";
			aStr3 = "bottom of the tank.";
		}
		else if (mPetId == PET_NIKO)
		{
			aStr1 = "NIKO produces pearls that";
			aStr2 = "you can click on for a";
			aStr3 = "hefty sum of money.";
		}
		else if (mPetId == PET_CLYDE)
		{
			aStr1 = "CLYDE drifts slowly through";
			aStr2 = "your tank, collecting any";
			aStr3 = "coins it passes by.";
		}
		else if (mPetId == PET_PRESTO)
		{
			aStr1 = "PRESTO has the ability to";
			aStr2 = "metamorph into any of";
			aStr3 = "your other pets.";
		}
		else if (mPetId == PET_SHRAPNEL)
		{
			aStr1 = "SHRAPNEL drops bombs that";
			aStr2 = "blow up fish on contact but";
			aStr3 = "give lots of cash when clicked.";
		}
		else if (mPetId == PET_RHUBARB)
		{
			aStr1 = "RHUBARB snaps his claws at";
			aStr2 = "fish, keeping them off the";
			aStr3 = "bottom of your tank.";
		}
		else if (mPetId == PET_SEYMOUR)
		{
			aStr1 = "SEYMOUR's presence makes all";
			aStr2 = "coins and diamonds drift";
			aStr3 = "at a slower rate.";
		}
		else if (mPetId == PET_NIMBUS)
		{
			aStr1 = "NIMBUS tosses any coins or";
			aStr2 = "food he catches back up";
			aStr3 = "toward the top of the tank.";
		}
		else if (mPetId == PET_AMP)
		{
			aStr1 = "AMP can electrocute your";
			aStr2 = "entire tank, killing your fish";
			aStr3 = "and turning them into diamonds.";
		}
		else if (mPetId == PET_ANGIE)
		{
			aStr1 = "ANGIE has the ability to";
			aStr2 = "resurrect dead fish.";
		}
		else if (mPetId == PET_GUMBO)
		{
			aStr1 = "GUMBO attracts guppies using";
			aStr2 = "the lantern on his head,";
			aStr3 = "luring them away from aliens.";
		}
		else if (mPetId == PET_GASH)
		{
			aStr1 = "GASH viciously attacks aliens,";
			aStr2 = "but will snack on one of";
			aStr3 = "your guppies from time to time.";
		}
		else if (mPetId == PET_BLIP)
		{
			aStr1 = "BLIP provides you with info";
			aStr2 = "that helps you better combat";
			aStr3 = "aliens and keep your fish fed.";
		}
		else if (mPetId == PET_BRINKLEY)
		{
			aStr1 = "Likes: peach muffins, all";
			aStr2 = "things brown and sticky";
			aStr3 = "Dislikes: arugula";
		}
		else if (mPetId == PET_NOSTRADAMUS)
		{
			aStr1 = "Little known fact:  NOSTRADAMUS";
			aStr2 = "is the long lost nose of ex-president";
			aStr3 = "Rutherford B. Hayes";
		}
		else if (mPetId == PET_WALTER)
		{
			aStr1 = "Choosing this pet donates all";
			aStr2 = "proceeds to the Falafel";
			aStr3 = "Foundation. Free the falafels!";
		}
		else if (mPetId == PET_STANLEY)
		{
			aStr1 = "STANLEY knows no fear.. except";
			aStr2 = "that of badgers, aprons,";
			aStr3 = "and badgers wearing aprons.";
		}
		else if (mPetId == 999)
		{
			aStr1 = "EVIL ALIEN MASTERMIND actually";
			aStr2 = "isn't very helpful at all.  Unless";
			aStr3 = "you consider devouring the entire";
			SexyString aStr4 = "";
			aStr4 = "contents of your fishtank helpful.";
			g->SetColor(Color(255, 255, 255, 255));
			g->DrawString(aStr4, ((650 - g->GetFont()->StringWidth(aStr4)) >> 1) - 3, 360);
		}

		g->DrawString(aStr1, ((650 - g->GetFont()->StringWidth(aStr1)) >> 1) - 3, 300);
		g->DrawString(aStr2, ((650 - g->GetFont()->StringWidth(aStr2)) >> 1) - 3, 320);
		g->DrawString(aStr3, ((650 - g->GetFont()->StringWidth(aStr3)) >> 1) - 3, 340);

		g->SetColor(Color(255, 255, 100, 255));

		if (mPetId == PET_PRESTO)
		{
			WriteWordWrapped(g, Rect(180, 373, 284, 0), "You have been awarded 5000 shells\nfor defeating the Final Boss!", -1, 0);
		}
		else
		{
			g->DrawString("Your game has been saved.", 221, 390);
		}

		if (mPetId == 999)
		{
			aStr1 = "Evil Alien Mastermind";
			g->SetColor(Color(255, 200, 0, 255));
			g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
			g->DrawString(aStr1, ((650 - g->GetFont()->StringWidth(aStr1)) >> 1) - 3, 260);
		}
	}
}

void Sexy::HatchScreen::DrawOverlay(Graphics* g)
{
	g->DrawImage(IMAGE_HATCHREFLECTION, 240, 60);
	if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 1)
	{
		if (m0x94 >= 140)
		{
			Graphics g2(*g);
			g2.ClipRect(238, m0xa4, 160, 160);
			int aFrame = (m0x94 % 20) / 2;
			g2.DrawImage(IMAGE_BOSS, 238 - (aFrame * 160), m0xa4);
		}
	}
	else if(m0x94 >= 140 && m0x94 < 200)
	{
		int aShardValues[16] = { -10, -15, 3, -13, 10, 5, 14, 12, 1, -11, -14, 12, -15, 11, 13, -1 };

		for (int i = 0; i < 8; i++)
		{
			int anOffset = m0x94 - 140;
			g->DrawImageCel(IMAGE_EGGSHARDS, aShardValues[i + 8] * anOffset + 268, aShardValues[i] * anOffset + 70, i);
		}
	}
}

void Sexy::HatchScreen::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::HatchScreen::ButtonDepress(int theId)
{
	if (theId == 99)
	{
		mApp->RemoveHatchScreen();
		if (mApp->mGameMode != GAMEMODE_TIME_TRIAL && mApp->mGameMode != GAMEMODE_CHALLENGE)
		{
			if (mApp->mCurrentProfile->mTank != 5 || mApp->mCurrentProfile->mLevel != 2)
				mApp->SwitchToBoard(false, false);
			else
				mApp->SwitchToInterludeScreen();
		}
		else
			mApp->LeaveGameBoard();
	}
	else if (theId == 100)
	{
		mApp->RemoveHatchScreen();
		if (mApp->mCurrentProfile->mTank == 5 && mApp->mCurrentProfile->mLevel == 2)
			mApp->SwitchToInterludeScreen();
		else
			mApp->SwitchToGameSelector();
	}
}

void HatchScreenOverlay::Draw(Sexy::Graphics* g)
{
	mScreen->DrawOverlay(g);
}
