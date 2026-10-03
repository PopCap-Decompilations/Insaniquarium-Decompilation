#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/MTRand.h>

#include "StoreScreen.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "MyLabelWidget.h"
#include "ProfileMgr.h"
#include "Fish.h"
#include "Oscar.h"
#include "Ultra.h"
#include "Gekko.h"
#include "Penta.h"
#include "Grubber.h"
#include "Breeder.h"
#include "SylvesterFish.h"
#include "BallFish.h"
#include "BiFish.h"
#include "Food.h"
#include "StoreButtonWidget.h"
#include "Res.h"

#include <ctime>

Sexy::StoreScreen::StoreScreen(WinFishApp* theApp)
{
	mApp = theApp;
	mWidgetManager = theApp->mWidgetManager;
	mX = 0;
	mY = 0;
	mStoreButtonsX = 192;
	mStoreButtonsXGap = 111;
	mWidth = theApp->mWidth;
	mHeight = theApp->mHeight;
	m0x90 = 0;
	mOverButtonId = -1;
	mBoughtItemTimer = 0;
	mSaveData = false;

	mBackButton = MakeDialogButton(99, this, "Back", FONT_JUNGLEFEVER10OUTLINE);
	mBackButton->mColors[0] = Color(255, 240, 0, 255);
	mBackButton->Resize(6, 440, 113, mBackButton->mHeight);

	Image* aBtnImages[8] = {IMAGE_STORELEFTBUTTON , IMAGE_STORELEFTBUTTON ,IMAGE_STORELEFTBUTTON ,
		IMAGE_STORETOPBUTTON,IMAGE_STORETOPBUTTON ,IMAGE_STORETOPBUTTON ,IMAGE_STORETOPBUTTON ,
		IMAGE_STORERIGHTBUTTON };

	Rect aBtnRects[8] = {
		Rect(1, 5, 109, 140),
		Rect(1, 143, 109, 140),
		Rect(1, 282, 109, 140),
		Rect(mStoreButtonsX + 1, 38, 98, 140),
		Rect(mStoreButtonsXGap + mStoreButtonsX, 38, 98, 140),
		Rect(mStoreButtonsXGap * 2 + mStoreButtonsX, 38, 98, 140),
		Rect(mStoreButtonsXGap * 3 + mStoreButtonsX, 38, 98, 140),
		Rect(489, 339, 121, 100)
	};

	for (int i = 0; i < 8; i++)
	{
		mStoreButtons[i] = new StoreButtonWidget(i, this, aBtnImages[i]);
		mStoreButtons[i]->Resize(aBtnRects[i]);
	}
	mStoreButtons[7]->m0x154 = 84;
	mStoreButtons[3]->m0x150 = 0;
	mStoreButtons[4]->m0x150 = 0;
	mStoreButtons[5]->m0x150 = 0;
	mStoreButtons[6]->m0x150 = 0;

	mShellsLabel = new MyLabelWidget();
	mShellsLabel->mAlignment = 2;
	mShellsLabel->mMouseVisible = false;
	mShellsLabel->mX = 238;
	mShellsLabel->mY = 439;
	mShellsLabel->mLabelFont = FONT_CONTINUUMBOLD12;
	mShellsLabel->mHeight = mShellsLabel->mLabelFont->GetHeight();
	mShellsLabel->mWidth = 80;
	mShellsLabel->mLabelColor = Color(250, 155, 150, 255);
	sprintf(mShellsString, "%d", mApp->mCurrentProfile->mShells);
	mShellsLabel->SetLabel(mShellsString);
	mMerylBlinkTimer = mApp->mSeed->Next() % 150 + 100;
	mStoreScreenUpdateCnt = 0;
	mApp->StopMusic();
	mApp->PlayMusic(2, 0, false);
	mStoreButtonLast = nullptr;

	mOverlay = new StoreScreenOverlay(this);
}

Sexy::StoreScreen::~StoreScreen()
{
	for (int i = 0; i < 8; i++)
		if (mStoreButtons[i])
			delete mStoreButtons[i];
	if (mBackButton)
		delete mBackButton;
	if (mShellsLabel)
		delete mShellsLabel;
	if (mOverlay)
		delete mOverlay;
}

void Sexy::StoreScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	WidgetContainer::AddedToManager(theWidgetManager);
	InitializeStoreButtons(0);
	for (int i = 0; i < 8; i++)
		theWidgetManager->AddWidget(mStoreButtons[i]);
	theWidgetManager->AddWidget(mOverlay);
	theWidgetManager->AddWidget(mBackButton);
	theWidgetManager->AddWidget(mShellsLabel);
}

void Sexy::StoreScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	WidgetContainer::RemovedFromManager(theWidgetManager);
	for (int i = 0; i < 8; i++)
		theWidgetManager->RemoveWidget(mStoreButtons[i]);
	theWidgetManager->RemoveWidget(mOverlay);
	theWidgetManager->RemoveWidget(mBackButton);
	theWidgetManager->RemoveWidget(mShellsLabel);
}

void Sexy::StoreScreen::Update()
{
	mMerylBlinkTimer--;
	if (mMerylBlinkTimer <= 0)
	{
		if (mApp->mSeed->Next() % 7 == 0)
			mMerylBlinkTimer = mApp->mSeed->Next() % 15 + 15;
		else
			mMerylBlinkTimer = mApp->mSeed->Next() % 150 + 100;
	}

	MarkDirty();
	mStoreScreenUpdateCnt++;
	if (mBoughtItemTimer != 0)
	{
		if (mOverButtonId >= 0)
			mBoughtItemTimer = 0;
		else
			mBoughtItemTimer--;
	}
}

void Sexy::StoreScreen::OrderInManagerChanged()
{
	for (int i = 0;i < 8;i++)
		mWidgetManager->BringToFront(mStoreButtons[i]);
	mWidgetManager->BringToFront(mOverlay);
	mWidgetManager->BringToFront(mBackButton);
	mWidgetManager->BringToFront(mShellsLabel);
}

void Sexy::StoreScreen::DrawOverlay(Graphics* g)
{
	g->DrawImage(IMAGE_STORESCREEN, 0, 0);
	for (int i = 0;i < 8;i++)
	{
		StoreButtonWidget* aButton = mStoreButtons[i];
		g->Translate(aButton->mX, aButton->mY);
		aButton->DrawOverlay(g);
		g->Translate(-aButton->mX, -aButton->mY);
	}
	g->DrawImageAnim(IMAGE_MERYLBLINK, 255, 289, mMerylBlinkTimer);
	// Both bounds of mOverButtonId are tested (0x52BC01: test; jl, then cmp 8; jge)
	if ((mOverButtonId >= 0 && mOverButtonId < 8 && mStoreScreenUpdateCnt > 15) || mBoughtItemTimer != 0)
	{
		const char* aStr;
		if (mBoughtItemTimer != 0)
			aStr = "Thanks for shopping at\nthe Fish Emporium!";
		else
		{
			aStr = GetProductDescription(mStoreButtons[mOverButtonId]);
			if (aStr == NULL)
				return;
		}

		const char* newLineChar = strchr(aStr, '\n');
		int aNewLineCnt = 0;
		while (newLineChar)
		{
			aNewLineCnt++;
			newLineChar = strchr(newLineChar + 1, '\n');
		}
		g->DrawImage(IMAGE_SPEECHBUBBLE, 390, 220);
		g->SetColor(Color(0, 0, 0, 255));
		g->SetFont(FONT_BLAMBOTPRO8);

		Rect aStrRect(393, 235, 200, 300);
		if (aNewLineCnt < 1)
			aStrRect.mY = 250;
		if (aNewLineCnt < 2)
			aStrRect.mY += 10;
		else if (aNewLineCnt >= 3)
			aStrRect.mY -= 5;

		WriteWordWrapped(g, aStrRect, aStr, 20, 0);
	}
}

void Sexy::StoreScreen::MouseEnter()
{
	Widget::MouseEnter();
	if (mStoreButtonLast != nullptr)
	{
		mStoreButtonLast->HandleMouseEvent(false);
		mStoreButtonLast = nullptr;
	}
}

void Sexy::StoreScreen::MouseMove(int theX, int theY)
{
	if (mBoughtItemTimer <= 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (mStoreButtons[i]->Contains(theX, theY))
			{
				mOverButtonId = i;
				return;
			}
		}
		mOverButtonId = -1;
	}
	else
	{
		mBoughtItemTimer -= 3;
		if (mBoughtItemTimer < 0)
			mBoughtItemTimer = 0;
		mOverButtonId = -1;
	}
}

void Sexy::StoreScreen::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
}

void Sexy::StoreScreen::ButtonDepress(int theId)
{
	if (theId == 99)
	{
		if (mSaveData)
			mApp->SaveVirtualTankAndUserData();

		mApp->mBoard->StartMusic();
		mApp->RemoveStoreScreen();
		mApp->mBoard->PauseGame(false);
	}
	else if (theId < 8 && mStoreButtons[theId])
	{
		StoreButtonWidget* aBtn = mStoreButtons[theId];
		if (aBtn->m0x128 > mApp->mCurrentProfile->mShells)
		{
			const char* aStrExtension;
			switch (aBtn->m0x14c)
			{
			case PRODUCT_FISH:
				aStrExtension = "this fish";
				break;
			case PRODUCT_BUBBULATOR:
				aStrExtension = "the Bubbulator";
				break;
			case PRODUCT_BACKDROP:
				aStrExtension = "this backdrop";
				break;
			case PRODUCT_ALIEN_ATTRACTOR:
				aStrExtension = "the Alien Attractor";
				break;
			case PRODUCT_UPGRADE:
				aStrExtension = "this upgrade";
				break;
			default:
				aStrExtension = "item";
				break;
			}

			mApp->DoDialog(DIALOG_INFO, true, "Not Enough Shells", StrFormat("Sorry, but you need more Shells to purchase %s.\n\nKeep playing to earn more!", aStrExtension), "OK", Dialog::BUTTONS_FOOTER);
		}
		else
		{
			mStoreButtonLast = aBtn;
			aBtn->HandleMouseEvent(true);

			GameObject* aProduct = aBtn->GetProduct(false);
			if (aProduct != nullptr && aBtn->m0x14c == PRODUCT_FISH)
			{
				int aNextId = mApp->mBoard->GetNextVirtualTankId();
				if (aNextId < 0)
				{
					if (mApp->mCurrentProfile->mBubbulatorBought == 0)
						mApp->DoDialog(DIALOG_INFO, true, "Fishtank is Full",
							"You\'ll need to free up some room in your tank before you can buy another fish.  You can either buy a bubbulator or sell a fish back to us.",
							"OK", Dialog::BUTTONS_FOOTER);
					else
						mApp->DoDialog(DIALOG_INFO, true, "Fishtank is Full",
							"You\'ll need to free up some room in your tank before you can buy another fish.",
							"OK", Dialog::BUTTONS_FOOTER);
				}
				else
				{
					aProduct->mVirtualTankId = aNextId;
					mApp->DoFishNamingDialog("Please choose a name for your fish.", aProduct->mType == TYPE_BREEDER, aProduct->mName, true);
				}
			}
			else
			{
				// The original's case order (its exception states and string literals follow it)
				switch (aBtn->m0x14c)
				{
				case PRODUCT_BUBBULATOR:
					mApp->DoConfirmPurchaseDialog("Would you like to\nbuy The Bubbulator?");
					break;
				case PRODUCT_ALIEN_ATTRACTOR:
					mApp->DoConfirmPurchaseDialog("Would you like to\nbuy the Alien Attractor?");
					break;
				case PRODUCT_UPGRADE:
					mApp->DoConfirmPurchaseDialog("Would you like to\nupgrade your fish food?");
					break;
				case PRODUCT_BACKDROP:
					mApp->DoConfirmPurchaseDialog("Would you like to\nbuy this backdrop?");
					break;
				default:
					break;
				}
			}
		}
	}
}

Sexy::GameObject* Sexy::StoreScreen::ConfirmPurchase()
{
	// The original keeps the result in a local (null unless a fish was bought) and returns it on every path
	GameObject* aProd = NULL;
	mBoughtItemTimer = 108;
	if (mStoreButtonLast == nullptr)
		return aProd;
	mSaveData = true;
	mApp->mCurrentProfile->mStoreScreenBought[mStoreButtonLast->mId] = true;
	mApp->mCurrentProfile->AddShells(-mStoreButtonLast->m0x128);
	if (mApp->mCurrentProfile->mShells < 0)
		mApp->mCurrentProfile->mShells = 0;
	sprintf(mShellsString, "%d", mApp->mCurrentProfile->mShells);
	mShellsLabel->SetLabel(mShellsString);

	// The original's case order (as in ButtonDepress, the backdrop comes last)
	switch (mStoreButtonLast->m0x14c)
	{
	case PRODUCT_FISH:
		aProd = mStoreButtonLast->GetProduct(true);
		if (aProd)
		{
			aProd->mTimeBought = time(NULL);
			aProd->BoughtSetup();
			mStoreButtonLast->Bought();
			return aProd;
		}
		break;
	case PRODUCT_BUBBULATOR:
		mApp->mCurrentProfile->mBubbulatorBought = 1;
		mApp->mBoard->mBubbulatorShown = true;
		mApp->mBoard->mBubbulatorTimer = 0;
		break;
	case PRODUCT_ALIEN_ATTRACTOR:
		mApp->mCurrentProfile->mAlienAttractorBought = 1;
		mApp->mBoard->mAlienAttractorShown = true;
		break;
	case PRODUCT_UPGRADE:
		gFoodType = 2;
		break;
	case PRODUCT_BACKDROP:
	{
		// Read once: the original keeps it in a register across the profile store
		int aBackdropId = mStoreButtonLast->m0x158;
		if ((uint)aBackdropId <= 5)
		{
			mApp->mCurrentProfile->mUnlockedBackgrounds[aBackdropId] = true;
			mApp->mBoard->ChangeBackground(aBackdropId + 1);
		}
		break;
	}
	}
	mStoreButtonLast->Bought();
	return aProd;
}

// Meryl's speech bubble text for a store button, or NULL for an unknown product (no bubble)
const char* Sexy::StoreScreen::GetProductDescription(StoreButtonWidget* theButton)
{
	// The original's case order: the fish case comes after the upgrade
	switch (theButton->m0x14c)
	{
	case PRODUCT_SOLD:
		return "Sorry, this is sold out.\nCheck back tomorrow!";
	case PRODUCT_BUBBULATOR:
		return "Want to keep more fish\nin your tank?  Buy\nThe Bubbulator!";
	case PRODUCT_BACKDROP:
		return "Keep your fish happy\nwith this beautiful\nnew backdrop!";
	case PRODUCT_ALIEN_ATTRACTOR:
		return "Need more excitement\nin your tank?  Buy\nthe alien attractor!\nTrust me.  It\'s safe!";
	case PRODUCT_UPGRADE:
		return "Your fish will\ngrow faster with\nthis food upgrade!";
	case PRODUCT_FISH:
		return GetFishDescription(theButton->GetProduct(false));
	default:
		return NULL;
	}
}

static char gFishDescriptionBuffer[1024];

// A named fish is introduced by name (in a static buffer); otherwise the text follows its special attribute, or its type
const char* Sexy::StoreScreen::GetFishDescription(GameObject* theObject)
{
	if (theObject == NULL)
		return NULL;

	if (theObject->mName.length() != 0)
	{
		if (theObject->mPreNamedTypeId == COOKIE)
			strcpy(gFishDescriptionBuffer, "This is Cookie.\nCookie will feed\nyour more exotic fish.");
		else if (theObject->mName[theObject->mName.length() - 1] == '.')
			sprintf(gFishDescriptionBuffer, "This is %s", theObject->mName.c_str());
		else
			sprintf(gFishDescriptionBuffer, "This is %s.", theObject->mName.c_str());
		return gFishDescriptionBuffer;
	}

	switch (GameObject::GetAttribute(theObject))
	{
	case 0:
		return "You\'ve never seen\na fish like this!";
	case 1:
		return "This fish is a\nvoracious eater.";
	case 2:
		return "This fish goes\nfrom 0 to 60\nin 1.3 seconds!";
	case 3:
		return "This fish is\na musical genius!";
	case 4:
		return "This fish is\n\"forwardly challenged\"\nbut it\'s great otherwise!";
	case 5:
		return "This fish has\ndeveloped a taste\nfor exotic food!";
	case 6:
		return "This fish has a\nvery special diet.";
	case 7:
	case 8:
		return "This fish is extremely \nrare.  We hardly ever\nget them in stock.";
	}

	// The original's case order (its string literals are laid out in this order)
	switch (theObject->mType)
	{
	case TYPE_GUPPY:
	{
		Fish* aFish = (Fish*)theObject;
		if (aFish->mHasSpecialColors)
		{
			if (aFish->mRainbowFish)
				return "This one\'s pretty\ncool, isn\'t it?";
			return "Choose from a dazzling\narray of colors.  C\'mon!\nyou know you want one!";
		}
		return "These adorable little\nfish are a mainstay\nof any aquarium!";
	}
	case TYPE_OSCAR:
	{
		Oscar* aFish = (Oscar*)theObject;
		if (aFish->mHasSpecialColors && aFish->mRainbowFish)
			return "This one\'s pretty\ncool, isn\'t it?";
		return "Specially trained not\nto eat store-bought\nguppies.  I promise!";
	}
	case TYPE_PENTA:
		return "Isn\'t this little guy\njust the cutest?\nAnswer: Yes.";
	case TYPE_GRUBBER:
		return "It won\'t eat\nyour guppies, but\nwatch your fingers!";
	case TYPE_BREEDER:
		return "Take care of this fish,\nand she\'ll give you a\nbaby fish at\nno extra charge!";
	case TYPE_ULTRA:
		return "Many regard this\nto be the\nSUV of fish!";
	case TYPE_BALL_FISH:
		return "Some people think\nthis fish looks\nlike a ball.";
	case TYPE_SYLVESTER_FISH:
		return "Don\'t worry.  This\n\"fish\" has been\ndomesticated.";
	case TYPE_BI_FISH:
		return "This \"fish\" may look\ndead, but he is actually\nquite lively!";
	default:
		return "All of our fish come\nwith a free plastic bag!\nTake one home today!";
	}
}

void Sexy::StoreScreen::InitializeStoreButtons(int theSeed)
{
	long long aSeedBase = (long long)(int)gUnkInt01 + mApp->mDaysSinceLastRun + (long long)theSeed; // Should be okay
	mSeedBase = aSeedBase;
	mRand.SRand((unsigned long)(mApp->mCurrentProfile->m0x98 + aSeedBase));
	mApp->mCurrentProfile->UpdateStoreData(mSeedBase);
	
	CommonFishStoreSlots();
	ColorfulFishStoreSlots();
	SpecialFishStoreSlots();
	ItemsStoreSlots();
	for (int i = 0; i < 8;i++)
	{
		if (!mApp->mCurrentProfile->mStoreScreenBought[i] && mStoreButtons[i]->m0x14c != 0)
			mStoreButtons[i]->SetAvailable();
		else
			mStoreButtons[i]->Bought();
	}
}

void Sexy::StoreScreen::CommonFishStoreSlots()
{
	Fish* aCommonFish = new Fish(0, 0);
	Oscar* aCommonOscar = new Oscar(0, 0);

	mStoreButtons[0]->SetUpButton(aCommonFish, aCommonFish->GetShellCost());
	mStoreButtons[1]->SetUpButton(aCommonOscar, aCommonOscar->GetShellCost());
}

void Sexy::StoreScreen::ColorfulFishStoreSlots()
{
	Fish* aColorfulFish = new Fish(0, 0);
	Oscar* aColorfulOscar = new Oscar(0, 0);

	InitFishColors(aColorfulFish, true);
	InitFishColors(aColorfulOscar, true);

	mStoreButtons[3]->SetUpButton(aColorfulFish, aColorfulFish->GetShellCost());
	mStoreButtons[4]->SetUpButton(aColorfulOscar, aColorfulOscar->GetShellCost());
}

void Sexy::StoreScreen::SpecialFishStoreSlots()
{
	GameObject* aFish1 = GetRandomSpecialFish(false, -1, 9);
	int aVal = Unk01(true);
	GameObject* aFish2 = nullptr;
	if (aVal == 8)
		aFish2 = GetSpecial8Fish();

	if (aFish2 == nullptr)
	{
		aFish2 = GetRandomSpecialFish(true, aFish1->mType, aVal);
		SetUpSpecialFish(aFish2, true);
	}

	mStoreButtons[5]->SetUpButton(aFish1, aFish1->GetShellCost());
	mStoreButtons[6]->SetUpButton(aFish2, aFish2->GetShellCost());
}

void Sexy::StoreScreen::ItemsStoreSlots()
{
	// The original reads mApp->mCurrentProfile again at each use (in the loop too)
	if (mApp->mCurrentProfile->mBubbulatorBought == 0)
		mStoreButtons[2]->SetProductType(PRODUCT_BUBBULATOR, 0, 20000);
	else if(mApp->mCurrentProfile->mAlienAttractorBought == 0)
		mStoreButtons[2]->SetProductType(PRODUCT_ALIEN_ATTRACTOR, 0, 50000);
	else if(gFoodType == 0)
		mStoreButtons[2]->SetProductType(PRODUCT_UPGRADE, 0, 20000);
	else
	{
		GameObject* aProd;
		if (!IsSpecialFishInTank(COOKIE))
			aProd = MakeSpecialFish(COOKIE);
		else
		{
			aProd = GetRandomSpecialFish(false, -1, 9);
			if ((mRand.Next() & 1) == 0)
				SetUpSpecialFish(aProd, false);
		}
		mStoreButtons[2]->SetUpButton(aProd, aProd->GetShellCost());
	}

	// One push_back site, as in the original
	std::vector<int> aBGCandidates;
	for (int i = 0; i < 6; i++)
	{
		if (!mApp->mCurrentProfile->mUnlockedBackgrounds[i] && (i != 5 || mRand.Next() % 5 == 0))
			aBGCandidates.push_back(i);
	}

	if (!aBGCandidates.empty())
	{
		long long aVal = mSeedBase % aBGCandidates.size();
		mStoreButtons[7]->SetProductType(3, aBGCandidates[aVal], 15000);
	}
	else
	{
		GameObject* aProd = GetRandomSpecialFish(true, -1, 9);
		if ((mRand.Next() & 1) == 0)
			SetUpSpecialFish(aProd, false);
		mStoreButtons[7]->SetUpButton(aProd, aProd->GetShellCost());
	}
}

bool Sexy::StoreScreen::IsSpecialFishInTank(int theSpecialFishId)
{
	for (GameObjectSet::iterator it = mApp->mBoard->mGameObjectSet.begin(); it != mApp->mBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mPreNamedTypeId == theSpecialFishId)
			return true;
	}
	return false;
}

void Sexy::StoreScreen::InitFishColors(Fish* theFish, bool canBeRainbow)
{
	bool isRainbow = canBeRainbow && mRand.Next() % 30 == 0;
	theFish->SetFishColors(mRand.Next() % 1000, isRainbow);
}

Sexy::GameObject* Sexy::StoreScreen::GetRandomSpecialFish(bool possibleClassicFish, int theSkipType, int unk2)
{
	if (possibleClassicFish)
	{
		if (mRand.Next() % 100 <= 75)
		{
			Fish* aFish = RandomGuppyOrOscar();
			if (mRand.Next() % 100 <= 75)
				InitFishColors(aFish, true);
			return aFish;
		}
	}

	struct WeightedEntry
	{
		int mWeight;
		int mType;
	};

	WeightedEntry aTable[8];
	aTable[0].mWeight = 26; aTable[0].mType = TYPE_PENTA;
	aTable[1].mWeight = 25; aTable[1].mType = TYPE_GRUBBER;
	aTable[2].mWeight = 25; aTable[2].mType = TYPE_GEKKO;
	aTable[3].mWeight = 10; aTable[3].mType = TYPE_BREEDER;
	aTable[4].mWeight = 5;  aTable[4].mType = TYPE_ULTRA;
	aTable[5].mWeight = 3;  aTable[5].mType = TYPE_BI_FISH;
	aTable[6].mWeight = 3;  aTable[6].mType = TYPE_BALL_FISH;
	aTable[7].mWeight = 3;  aTable[7].mType = TYPE_SYLVESTER_FISH;

	int aLoopCnt = 0;
	while (aLoopCnt < 100)
	{
		int aRand = mRand.Next() % 100;
		int aCurWeightSum = 0;
		int aSelectedObj = 0;
		for (; aSelectedObj < 8; aSelectedObj++)
		{
			aCurWeightSum += aTable[aSelectedObj].mWeight;
			if (aRand < aCurWeightSum)
				break;
		}
		if (aSelectedObj == 8)
			aSelectedObj = 0;

		if (aTable[aSelectedObj].mType != theSkipType && 
			IsSpecialFishAllowed(unk2, aTable[aSelectedObj].mType))
		{
			// The original's case order (its exception states follow it)
			switch (aTable[aSelectedObj].mType)
			{
			case TYPE_PENTA:
				return new Penta(0, 0);
			case TYPE_GRUBBER:
				return new Grubber(0, 0);
			case TYPE_GEKKO:
				return new Gekko(0, 0);
			case TYPE_BREEDER:
				return new Breeder(0, 0);
			case TYPE_ULTRA:
				return new Ultra(0, 0);
			case TYPE_BALL_FISH:
				return new BallFish(0, 0, false);
			case TYPE_SYLVESTER_FISH:
				return new SylvesterFish(0, 0, false);
			case TYPE_BI_FISH:
			{
				BiFish* aFish = new BiFish(0, 0, false);
				aFish->m0x230 = mRand.Next() & 1;
				return aFish;
			}
			default:
				return new Penta(0, 0);
			}
		}

		aLoopCnt++;
	}
	return GetRandomSpecialFish(possibleClassicFish, -1, 9);
}

Sexy::Fish* Sexy::StoreScreen::RandomGuppyOrOscar()
{
	if (mRand.Next() % 2 == 0)
		return new Fish(0, 0);
	else
		return new Oscar(0,0);
}

bool Sexy::StoreScreen::IsSpecialFishAllowed(int unk, int theType)
{
	switch (unk)
	{
	case 1:
		if (theType == TYPE_BALL_FISH) return false;
		if (theType == TYPE_PENTA) return false;
		return true;
	case 4:
		if (theType == TYPE_PENTA) return false;
		if (theType == TYPE_GRUBBER) return false;
		if (theType == TYPE_BALL_FISH) return false;
		return true;
	case 5:
		return (theType != TYPE_BALL_FISH);
	case 6:
		if (theType == TYPE_PENTA) return false;
		if (theType == TYPE_GRUBBER) return false;
		return true;
	case 2:
	case 3:
	default:
		return true;
	}
}

int Sexy::StoreScreen::Unk01(bool deterministic)
{
	struct WeightedEntry
	{
		int mWeight;
		int mType;
	};

	// Filled in at the start, before the table of attributes below, as in the original
	WeightedEntry aTable[8] = {
		{ 48, 5 },
		{ 20, 1 },
		{ 10, 3 },
		{ 5,  2 },
		{ 5,  0 },
		{ 5,  6 },
		{ 5,  4 },
		{ 2,  7 }
	};

	if(!gUnkBool09)
	{
		gUnkBool09 = true;
		for (int i = 0; i < 100; i++)
		{
			if ((i & 1) == 0)
			{
				gUnkIntArray02[i] = 5;
			}
			else
			{
				int mod10 = i % 10;

				if (mod10 == 1 || mod10 == 5) gUnkIntArray02[i] = 3;
				else if (mod10 == 3) gUnkIntArray02[i] = 1;
				else
				{
					int mod20 = i % 20;
					if (mod20 == 7) gUnkIntArray02[i] = 2;
					else if (mod20 == 9) gUnkIntArray02[i] = 0;
					else if (mod20 == 17) gUnkIntArray02[i] = 6;
					else if (mod20 == 19) gUnkIntArray02[i] = 4;
				}
			}
			if(i % 10 == 0)
				gUnkIntArray02[i] = ((i / 10) % 2 != 0) + 7;
		}
	}

	if (deterministic)
	{
		int idx = mSeedBase % 100;
		return gUnkIntArray02[idx];
	}

	int aRand = mRand.Next() % 100;
	int aCurWeightSum = 0;
	int i;
	for (i = 0; i < 8; i++)
	{
		aCurWeightSum += aTable[i].mWeight;
		if (aRand < aCurWeightSum)
			break;
	}
	if (i == 8)
		i = 0;

	return aTable[i].mType;
}

Sexy::GameObject* Sexy::StoreScreen::GetSpecial8Fish()
{
	// As the original: the vector is constructed before the set, and the board's object set is
	// looked up before the two inserts
	std::vector<int> aCandidatesVector;
	std::set<int> aSet;
	GameObjectSet& aGameObjectSet = mApp->mBoard->mGameObjectSet;
	aSet.insert(5);
	aSet.insert(2);

	for (GameObjectSet::iterator it = aGameObjectSet.begin(); it != aGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mPreNamedTypeId != -1)
			aSet.insert(anObj->mPreNamedTypeId);
	}

	for (int i = 0; i < 6; i++)
	{
		if (aSet.find(i) == aSet.end())
			aCandidatesVector.push_back(i);
	}

	GameObject* aRes = nullptr;
	if (!aCandidatesVector.empty())
	{
		int anIdx = mRand.Next() % aCandidatesVector.size();
		aRes = MakeSpecialFish(aCandidatesVector[anIdx]);
	}
	return aRes;
}

Sexy::Fish* Sexy::StoreScreen::MakeSpecialFish(int theId)
{
	Fish* aFish;
	switch (theId)
	{
	case ROCKY:
	{
		aFish = new Fish(0, 0);
		aFish->mHasSpecialColors = true;
		aFish->mColors[0] = Color(0x2288ff);
		aFish->mColors[1] = Color(0xffffff);
		aFish->mVoracious = true;
		aFish->mExoticDietFoodType = EXO_FOOD_ULTRA;
		aFish->mName = "Rocky";
		break;
	}
	case LUDWIG:
	{
		aFish = new Oscar(0, 0);
		aFish->mSinging = true;
		aFish->mName = "Ludwig";
		aFish->mHasSpecialColors = true;
		aFish->mColors[0] = Color(0);
		aFish->mColors[1] = Color(0x8df5be);
		aFish->mColors[2] = Color(0x9ffa);
		break;
	}
	case COOKIE:
	{
		aFish = new Gekko(0, 0);
		aFish->mName = "Cookie";
		aFish->mExoticDietFoodType = 6;
		break;
	}
	case JOHNNYV:
	{
		aFish = new Oscar(0, 0);
		aFish->SetFishColors(mRand.Next() % 1000, true);
		aFish->mName = "Johnny V.";
		aFish->mExoticDietFoodType = EXO_FOOD_PIZZA;
		break;
	}
	case KILGORE:
	{
		aFish = new Ultra(0, 0);
		aFish->mName = "Kilgore";
		aFish->mVoracious = true;
		aFish->mSinging = true;
		break;
	}
	default:
		// The original returns here; the cases above set the id without a null test
		return NULL;
	}
	aFish->mPreNamedTypeId = theId;
	return aFish;
}

void Sexy::StoreScreen::SetUpSpecialFish(GameObject* theObject, bool deterministic)
{
	for (int i = 0; i < 100; i++)
	{
		int aVal = Unk01(deterministic);
		switch (aVal)
		{
		case 0:
			theObject->mInvisible = true;
			break;
		case 1:
			theObject->mVoracious = true;
			break;
		case 2:
			theObject->mSpeedy = true;
			break;
		case 3:
			theObject->mSinging = true;
			break;
		case 4:
			theObject->mForwardlyChallenged = true;
			break;
		case 5:
			SpecialId5SetUp(theObject);
			break;
		case 6:
			SpecialId6SetUp(theObject);
			break;
		case 7:
		case 8:
			SpecialId78SetUp(theObject);
			break;
		}

		if (IsSpecialFishValid(theObject))
			return;

		theObject->ResetSpecialProperties();

		if (i >= 50)
			deterministic = false;
	}
}

void Sexy::StoreScreen::SpecialId5SetUp(GameObject* theObject)
{
	switch (mRand.Next() % 3)
	{
	case 0:
		theObject->mExoticDietFoodType = EXO_FOOD_PIZZA;
		break;
	case 1:
		theObject->mExoticDietFoodType = EXO_FOOD_ICE_CREAM;
		break;
	case 2:
		theObject->mExoticDietFoodType = EXO_FOOD_CHICKEN;
		break;
	}
}

void Sexy::StoreScreen::SpecialId6SetUp(GameObject* theObject)
{
	// The type is read once, before the loop, as in the original
	int aType = theObject->mType;
	while (true)
	{
		switch (mRand.Next() % 3)
		{
		case 0:
			theObject->mExoticDietFoodType = EXO_FOOD_GUPPY;
			break;
		case 1:
			theObject->mExoticDietFoodType = EXO_FOOD_OSCAR;
			break;
		case 2:
			theObject->mExoticDietFoodType = EXO_FOOD_ULTRA;
			break;
		}

		if (aType == TYPE_ULTRA)
		{
			if (theObject->mExoticDietFoodType == EXO_FOOD_OSCAR)
				continue;
			break;
		}

		if (aType == TYPE_OSCAR ||
			aType == TYPE_GRUBBER ||
			aType == TYPE_BI_FISH)
		{
			if (theObject->mExoticDietFoodType == EXO_FOOD_GUPPY)
				continue;
			break;
		}

		if (aType == TYPE_SYLVESTER_FISH)
		{
			theObject->mExoticDietFoodType = EXO_FOOD_GUPPY;
			break;
		}

		break;
	}
}

void Sexy::StoreScreen::SpecialId78SetUp(GameObject* theObject)
{
	for (int i = 0;i < 100;i++)
	{
		int aRandVal = mRand.Next() % 7;
		switch (aRandVal)
		{
		case 0:
			theObject->mInvisible = true;
			theObject->mVoracious = true;
			break;
		case 1:
			theObject->mInvisible = true;
			theObject->mSinging = true;
			break;
		case 2:
			theObject->mVoracious = true;
			theObject->mSinging = true;
			break;
		case 3:
			theObject->mForwardlyChallenged = true;
			theObject->mSpeedy = true;
			break;
		case 4:
			theObject->mVoracious = true;
			theObject->mSpeedy = true;
			break;
		case 5:
			theObject->mInvisible = true;
			theObject->mVoracious = true;
			theObject->mSinging = true;
			break;
		case 6:
		{
			int subTrait = mRand.Next() % 5;
			switch (subTrait)
			{
			case 0: theObject->mInvisible			= true; break;
			case 1: theObject->mVoracious			= true; break;
			case 2: theObject->mSpeedy				= true;	break;
			case 3: theObject->mSinging				= true;	break;
			case 4: theObject->mForwardlyChallenged = true; break;
			}

			if ((mRand.Next() & 1) == 0)
				SpecialId6SetUp(theObject);
			else
				SpecialId5SetUp(theObject);
		}
			break;
		}
		if (IsSpecialFishValid(theObject))
			return;

		theObject->ResetSpecialProperties();
	}
}

bool Sexy::StoreScreen::IsSpecialFishValid(GameObject* theObject)
{
	int dietType = theObject->mExoticDietFoodType;
	int fishType = theObject->mType;

	// Each trait is tested first, then the result so far, as in the original
	bool isValid = true;
	if (theObject->mVoracious)
		isValid = IsSpecialFishAllowed(1, fishType);

	if (theObject->mForwardlyChallenged)
		isValid = isValid && IsSpecialFishAllowed(4, fishType);

	if (dietType == EXO_FOOD_PIZZA || dietType == EXO_FOOD_ICE_CREAM || dietType == EXO_FOOD_CHICKEN)
		isValid = isValid && IsSpecialFishAllowed(5, fishType);

	if (dietType == EXO_FOOD_ULTRA || dietType == EXO_FOOD_OSCAR || dietType == EXO_FOOD_GUPPY)
		isValid = isValid && IsSpecialFishAllowed(6, fishType);

	return isValid;
}

void StoreScreenOverlay::Draw(Sexy::Graphics* g)
{
	mScreen->DrawOverlay(g);
}
