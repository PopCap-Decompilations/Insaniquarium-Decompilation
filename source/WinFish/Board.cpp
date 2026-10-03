#include <SexyAppFramework/ButtonWidget.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/SexyMatrix.h>
#include <SexyAppFramework/trivertex.h>
#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/ImageFont.h>
#include <SexyAppFramework/DDImage.h>
#include <SexyAppFramework/BassMusicInterface.h>
#include <SexyAppFramework/SoundManager.h>
#include <SexyAppFramework/SoundInstance.h>
#include <SexyAppFramework/Buffer.h>

#include <ImageLib/ImageLib.h>

#include "Board.h"
#include "WinFishApp.h"
#include "Bilaterus.h"
#include "BilaterusHead.h"
#include "MenuButtonWidget.h"
#include "StarField.h"
#include "MyLabelWidget.h"
#include "WinFishCommon.h"
#include "HighScoreMgr.h"
#include "ProfileMgr.h"
#include "BubbleMgr.h"
#include "MessageWidget.h"
#include "Res.h"

#include "Fish.h"
#include "Oscar.h"
#include "Grubber.h"
#include "Penta.h"
#include "Gekko.h"
#include "Ultra.h"
#include "Breeder.h"
#include "Coin.h"
#include "Food.h"
#include "Missle.h"
#include "OtherTypePet.h"
#include "FishTypePet.h"
#include "Alien.h"
#include "DeadFish.h"
#include "DeadAlien.h"
#include "Warp.h"
#include "Shot.h"
#include "Larva.h"
#include "Shadow.h"
#include "SylvesterFish.h"
#include "BallFish.h"
#include "BiFish.h"

#include <chrono>
#include <ctime>

namespace Sexy
{
	int gUnkInt02 = 0;
	int gUnkInt05 = 0;
	int gUnkInt06 = 0;
	bool gMerylActive = false;
	bool gUnkBool02 = false;
	bool gZombieMode = false;
	int gWadsworthTimer = 0;
	int gWadsworthX = 0, gWadsworthY = 0;
	int gFoodType = 0;
	Color gUnkColor01 = Color(0, 0, 0, 0);
	bool gUnkBool07 = false;
	bool gUnkBool08 = false;
}

using namespace Sexy;

const double PI = 3.141590118408203;

Board::Board(WinFishApp* theApp)
{
	mApp = theApp;

	// Decomp
	m0x2a6 = true;
	m0x2a7 = false;
	mStarField = new StarField();
	if ((mApp->mCurrentProfile->mCheatCodeFlags >> 3 & 1) != 0)
		mStarField->Init(1000);

	mCheatCodes[CC_WAVY] = new CheatCode("wavy");
	mCheatCodes[CC_PREGO] = new CheatCode("prego");
	mCheatCodes[CC_VOID] = new CheatCode("void");
	mCheatCodes[CC_SPACE] = new CheatCode("space");
	mCheatCodes[CC_ZOMBIE] = new CheatCode("zombie");
	mCheatCodes[CC_BETATEST] = new CheatCode("welovebetatesters");
	mCheatCodes[CC_SUPERMEGA] = new CheatCode("supermegaultra");
	mCheatCodes[CC_TIME] = new CheatCode("time");
	
	mFishSongMgr = new FishSongMgr();
	mFishSongMgr->mTone = SOUND_TONE;
	mFishSongMgr->mToneLo = SOUND_TONELO;
	mFishSongMgr->mToneHi = SOUND_TONEHI;
	mFishSongMgr->mToneSuperHi = SOUND_TONESUPERHI;

	// In the original's order (the destructor frees them in the same order)
	mDeadFishList = new std::vector<DeadFish*>();
	mDeadAlienList = new std::vector<DeadAlien*>();
	mFishList = new std::vector<Fish*>();
	mOscarList = new std::vector<Oscar*>();
	mUltraList = new std::vector<Ultra*>();
	mFoodList = new std::vector<Food*>();
	mCoinList = new std::vector<Coin*>();
	mOtherTypePetList = new std::vector<OtherTypePet*>();
	mFishTypePetList = new std::vector<FishTypePet*>();
	mAlienList = new std::vector<Alien*>();
	mSmallAlienList = new std::vector<Alien*>();
	mShotList = new std::vector<Shot*>();
	mPentaList = new std::vector<Penta*>();
	mGrubberList = new std::vector<Grubber*>();
	mLarvaList = new std::vector<Larva*>();
	mGekkoList = new std::vector<Gekko*>();
	mMissleList1 = new std::vector<Missle*>();
	mMissleList2 = new std::vector<Missle*>();
	mBreederList = new std::vector<Breeder*>();
	mShadowList = new std::vector<Shadow*>();
	mNoteList = new std::vector<Coin*>();
	mNikoPearlCoinList = new std::vector<Coin*>();
	mWarpList = new std::vector<Warp*>();
	mBilaterusList = new std::vector<Bilaterus*>();
	mSpecialFishList = new std::vector<Fish*>();

	mBoardOverlay1 = new BoardOverlay(this, 0);
	mBoardOverlay2 = new BoardOverlay(this, 1);

	mBubbleMgr = new BubbleMgr();
	mBubbleMgr->SetBubbleBounds(Rect(0, 82, 640, 398));
	mBubbleMgr->SetBubbleConfig(0, 0);
	gZombieMode = mApp->mCurrentProfile->mCheatCodeFlags >> 4 & 1;
	mCyraxPtr = nullptr;
	memset(mMessageShown, 0, sizeof(mMessageShown));
	for (int i = 0; i < PET_END; i++) // 24 inline stores in the original, not a memset call
		mPetsInTank[i] = 0;

	mCurrentBackgroundId = 0;
	m0x3ec = -1;

	int aVal = Unk01();
	m0x3b4 = aVal;
	m0x3b8 = aVal;
	m0x3bc = 0;
	m0x3b0 = 0;
	m0x3c0 = 0;
	m0x3c4 = 0;

	mPause = false;

	for (int i = 0; i < 3; i++)
		mTankLightingSpeeds[i] = mApp->mSeed->Next() % 640;

	mMenuButtons[SLOT_GUPPY] = NULL;
	mMenuButtons[SLOT_BREEDER] = NULL;
	mMenuButtons[SLOT_FOODLVL] = NULL;
	mMenuButtons[SLOT_FOODLIMIT] = NULL;
	mMenuButtons[SLOT_OSCAR] = NULL;
	mMenuButtons[SLOT_POTION] = NULL;
	mMenuButtons[SLOT_STARCATCHER] = NULL;
	mMenuButton = NULL;
	mMessageWidget = NULL;
	mMenuButtons[SLOT_GEKKO] = NULL;
	mMenuButtons[SLOT_GRUBBER] = NULL;
	mMenuButtons[SLOT_WEAPON] = NULL;
	mMenuButtons[SLOT_ULTRA] = NULL;
	mMenuButtons[SLOT_EGG] = NULL;
	mBackButton = NULL;
	mMoneyLabel = NULL;

	m0x4ec = false;
	m0x4ed = false;
	m0x4f0 = 0;

	mDropFoodDelay = 0;

	m0x4f4 = false;
	m0x4f8 = 0;

	mMenuButton = new ButtonWidget(123, this);
	mMenuButton->mDoFinger = true;
	mMenuButton->mButtonImage = IMAGE_BLANK;
	mMenuButton->mOverImage = IMAGE_OPTIONSBUTTON;
	mMenuButton->mDownImage = IMAGE_OPTIONSBUTTOND;
	mMenuButton->Resize(525, 3, 101, 29);

	mMoneyLabel = new MyLabelWidget();
	mMoneyLabel->mAlignment = 2;
	mMoneyLabel->mMouseVisible = false;
	mMoneyLabel->mX = 535;
	mMoneyLabel->mY = 40;
	mMoneyLabel->mLabelFont = FONT_CONTINUUMBOLD12;
	mMoneyLabel->mHeight = mMoneyLabel->mLabelFont->GetHeight();
	mMoneyLabel->mWidth = 80;

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		mMoneyLabel->mLabelColor = Color(250, 155, 150, 255);
	else
		mMoneyLabel->mLabelColor = Color(180, 255, 90, 255);

	mMessageWidget = new MessageWidget(mApp, "");

	mShouldSave = false;
	mTank = 0;
	mLevel = 0;
	mIsBonusRound = false;
	mGameUpdateCnt = 0;
	m0x44c = 0;
	mBubbulatorTimer = 0;
	m0x2c4 = 0;
	m0x3f4 = 0;
	m0x3f8 = 0;
	m0x3fc = 0;
	mLastCoinTypeClicked = -1;
	mCoinComboCount = 0;
	m0x400 = 0;
	m0x4fc = false;
	mBubbulatorShown = false;
	mAlienAttractorShown = false;
	mAlwaysShowWhenHungry = false;
	m0x500 = true;
	m0x441 = false;
	m0x2b4 = 0;
	for (int i = 0; i < 64; i++)
		mSoundPlayedTimerArray[i] = -1000;
	mHasVirtualTankFish = false;
}

Board::~Board()
{
	if(mMoneyLabel)
		delete mMoneyLabel;
	if (mMenuButton)
		delete mMenuButton;
	if (mMessageWidget)
		delete mMessageWidget;

	delete mDeadFishList;
	delete mDeadAlienList;
	delete mFishList;
	delete mOscarList;
	delete mUltraList;
	delete mFoodList;
	delete mCoinList;
	delete mOtherTypePetList;
	delete mFishTypePetList;
	delete mAlienList;
	delete mSmallAlienList;
	delete mShotList;
	delete mPentaList;
	delete mGrubberList;
	delete mLarvaList;
	delete mGekkoList;
	delete mMissleList1;
	delete mMissleList2;
	delete mBreederList;
	delete mShadowList;
	delete mNoteList;
	delete mNikoPearlCoinList;
	delete mWarpList;
	delete mBilaterusList;
	delete mSpecialFishList;

	delete mFishSongMgr;

	if (mBoardOverlay1)
		delete mBoardOverlay1;
	if (mBoardOverlay2)
		delete mBoardOverlay2;
	if (mBubbleMgr)
		delete mBubbleMgr;
	for (int i = 0; i < 8; i++)
	{
		if (mCheatCodes[i])
			delete mCheatCodes[i];
	}
	if (mStarField)
		delete mStarField;
	mFishSongMgr = nullptr;
}

bool Board::IsFirstLevel()
{
	if (mTank == 1 && mLevel == 1 && !mApp->mCurrentProfile->mFinishedGame)
		return true;
	return false;
}

void Board::ShowText(std::string aText, bool aFlag, int anID)
{
	// The original lays out the id path first (the -1 path is the else block)
	if (anID != -1)
	{
		if (!mMessageShown[anID])
		{
			mMessageWidget->mMessageId = anID;
			mMessageWidget->mMessage = aText;
			mMessageWidget->mIsBlinking = aFlag;
			if (aFlag)
				mMessageWidget->mMessageTimer = 500;
			mMessageShown[anID] = true;
			mMessageWidget->mMessageTimer = 185;
		}
	}
	else
	{
		mMessageWidget->mMessageId = -1;
		mMessageWidget->mMessage = aText;
		mMessageWidget->mIsBlinking = aFlag;
		if (aFlag)
			mMessageWidget->mMessageTimer = 500;
		else
			mMessageWidget->mMessageTimer = 100;
	}
	mMessageWidget->mColor1 = Color(0xb4, 0xfa, 0x5a, 0xff);
	mMessageWidget->mColor2 = Color(0, 0x4b, 0, 0xff);
}

bool Sexy::Board::AliensInTank()
{
	return !mAlienList->empty() || !mBilaterusList->empty();
}

bool Sexy::Board::MisslesInTank()
{
	return !mMissleList1->empty();
}

bool Sexy::Board::FishInTank()
{
	return !mFishList->empty();
}

bool Sexy::Board::FoodInTank()
{
	return !mFoodList->empty();
}

bool Sexy::Board::UltrasInTank()
{
	return !mUltraList->empty();
}

bool Sexy::Board::GrubbersInTank()
{
	return !mGrubberList->empty();
}

bool Sexy::Board::GekkosInTank()
{
	return !mGekkoList->empty();
}

bool Sexy::Board::BreedersInTank()
{
	return !mBreederList->empty();
}

// A click on a coin that also lies on a missile, alien or Bilaterus head is passed on
// to the board as a shot at that (absolute) position.
bool Sexy::Board::ShootEnemyUnderCoin(int theX, int theY)
{
	bool aHit = false;
	for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end() && !aHit; ++it)
		if ((*it)->Contains(theX, theY))
			aHit = true;
	for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end() && !aHit; ++it)
		if ((*it)->Contains(theX, theY))
			aHit = true;
	for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end() && !aHit; ++it)
	{
		Bilaterus* aBilaterus = *it;
		if (aBilaterus->mActiveHead && aBilaterus->mActiveHead->Contains(theX, theY))
			aHit = true;
	}
	if (!aHit || gUnkBool04)
		return false;
	gUnkBool04 = true;
	MouseDown(theX, theY, 1);
	gUnkBool04 = false;
	return true;
}

Point Sexy::Board::GetBonusShellTarget()
{
	return Point(260, 265);
}

// A separate cdecl function in the original (0x53C320; identical-code folding left one body for every
// vector type): erases the first element equal to theObject, if there is one
template <class T>
static void RemoveFromVector(std::vector<T*>* theVector, T* theObject)
{
	typename std::vector<T*>::iterator anIt = std::find(theVector->begin(), theVector->end(), theObject);
	if (anIt != theVector->end())
		theVector->erase(anIt);
}

bool Board::RemoveGameObjectFromLists(GameObject* theObject, bool aFlag)
{
	bool unkbool = true;
	switch (theObject->mType)
	{
	case TYPE_GUPPY:
		RemoveFromVector(mFishList, (Fish*)theObject);
		break;
	case TYPE_OSCAR:
		RemoveFromVector(mOscarList, (Oscar*)theObject);
		break;
	case TYPE_ULTRA:
		RemoveFromVector(mUltraList, (Ultra*)theObject);
		break;
	case TYPE_GEKKO:
		RemoveFromVector(mGekkoList, (Gekko*)theObject);
		break;
	case TYPE_PENTA:
		RemoveFromVector(mPentaList, (Penta*)theObject);
		break;
	case TYPE_GRUBBER:
		RemoveFromVector(mGrubberList, (Grubber*)theObject);
		break;
	case TYPE_BREEDER:
		RemoveFromVector(mBreederList, (Breeder*)theObject);
		break;
	case TYPE_OTHER_TYPE_PET:
	{
		OtherTypePet* aPet = (OtherTypePet*)theObject;
		if (aPet->mOtherTypePetType >= PET_STINKY && aPet->mOtherTypePetType < PET_END)
			mPetsInTank[aPet->mOtherTypePetType]--;
		RemoveFromVector(mOtherTypePetList, aPet);
		break;
	}
	case TYPE_FISH_TYPE_PET:
	{
		FishTypePet* aPet = (FishTypePet*)theObject;
		if (aPet->mFishTypePetType >= PET_STINKY && aPet->mFishTypePetType < PET_END)
			mPetsInTank[aPet->mFishTypePetType]--;
		RemoveFromVector(mFishTypePetList, aPet);
		break;
	}
	case TYPE_ALIEN:
	{
		Alien* anAlien = (Alien*)theObject;
		if (anAlien == mCyraxPtr) return false;
		if(anAlien->mAlienType == ALIEN_MINI_SYLV)
			RemoveFromVector(mSmallAlienList, anAlien);
		else
			RemoveFromVector(mAlienList, anAlien);
		break;
	}
	case TYPE_BILATERUS:
		RemoveFromVector(mBilaterusList, (Bilaterus*)theObject);
		break;
	case TYPE_COIN:
	{
		Coin* aCoin = (Coin*)theObject;
		if (aCoin->m0x1a0 > 1 || aCoin->mCoinType == COIN_NIKOPEARL)
			RemoveFromVector(mNikoPearlCoinList, aCoin);
		if (aCoin->mCoinType == COIN_NOTE)
			RemoveFromVector(mNoteList, aCoin);
		else
			RemoveFromVector(mCoinList, aCoin);
		break;
	}
	case TYPE_DEAD_ALIEN:
		RemoveFromVector(mDeadAlienList, (DeadAlien*)theObject);
		break;
	case TYPE_DEAD_FISH:
		RemoveFromVector(mDeadFishList, (DeadFish*)theObject);
		break;
	case TYPE_FOOD:
		RemoveFromVector(mFoodList, (Food*)theObject);
		break;
	case TYPE_LARVA:
		RemoveFromVector(mLarvaList, (Larva*)theObject);
		break;
	case TYPE_MISSLE:
	{
		Missle* aMis = (Missle*)theObject;
		if (aMis->IsTargetless())
			RemoveFromVector(mMissleList2, aMis);
		else
			RemoveFromVector(mMissleList1, aMis);
		unkbool = false;
		break;
	}
	case TYPE_SHADOW:
		RemoveFromVector(mShadowList, (Shadow*)theObject);
		unkbool = false;
		break;
	case TYPE_SHOT:
		RemoveFromVector(mShotList, (Shot*)theObject);
		unkbool = false;
		break;
	case TYPE_WARP:
		RemoveFromVector(mWarpList, (Warp*)theObject);
		unkbool = false;
		break;
	// three separate case bodies in the original's jump table (0x541E92, 0x541EA4, 0x541EB6)
	case TYPE_SYLVESTER_FISH:
		RemoveFromVector(mSpecialFishList, (Fish*)theObject);
		break;
	case TYPE_BALL_FISH:
		RemoveFromVector(mSpecialFishList, (Fish*)theObject);
		break;
	case TYPE_BI_FISH:
		RemoveFromVector(mSpecialFishList, (Fish*)theObject);
		break;
	}

	if (mGameObjectSet.erase(theObject) != 1 && aFlag)
		return false;
	if (unkbool)
		m0x2a7 = true;
	return true;
}

void Sexy::Board::SpawnDeadFish(int theX, int theY, double theVX, double theVY, double theSpeedMod, int theType, bool facingRight, Shadow* theShadowPtr)
{
	DeadFish* aDeadFish = new DeadFish(theX, theY, theVX, theVY, theSpeedMod, theType, facingRight);
	AddGameObject(aDeadFish, false);
	mWidgetManager->AddWidget(aDeadFish);
	aDeadFish->mShadowPtr = theShadowPtr;
	if (theShadowPtr)
		theShadowPtr->mObjectPtr = aDeadFish;
	SortGameObjects();
}

void Sexy::Board::SpawnDeadAlien(int theX, int theY, int theAnimationIndex, int theAlienType, bool facingRight)
{
	DeadAlien* anAlien = new DeadAlien(theX, theY, theAnimationIndex, theAlienType, facingRight);
	AddGameObject(anAlien, false);
	mWidgetManager->AddWidget(anAlien);
	SortGameObjects();
}

void Sexy::Board::SpawnGameObject(GameObject* theObject, bool randomPosition)
{
	if (randomPosition)
	{
		// The original draws x before y
		int aX = mApp->mSeed->Next() % 520 + 20;
		int aY = mApp->mSeed->Next() % 265 + 105;
		theObject->SetPosition(aX, aY);
	}
	AddGameObject(theObject, true);
	mWidgetManager->AddWidget(theObject);
	MakeShadowForGameObject(theObject);
}

void Sexy::Board::SpawnVirtualTankFood(int theFoodType)
{
	GameObject* aFood = nullptr;
	switch (theFoodType)
	{
	case 0:
		aFood = SpawnGuppyAsFood();
		break;
	case 1:
		aFood = SpawnStarAsFood();
		break;
	case 2:
		aFood = SpawnLarvaAsFood();
		break;
	case 3:
		aFood = SpawnOscarAsFood();
		break;
	case 4:
		aFood = SpawnUltraAsFood();
		break;
	case 5:
		aFood = SpawnExoticFood(EXO_FOOD_CHICKEN);
		break;
	case 6:
		aFood = SpawnExoticFood(EXO_FOOD_PIZZA);
		break;
	case 7:
		aFood = SpawnExoticFood(EXO_FOOD_ICE_CREAM);
		break;
	default:
		return;
	}
	if (aFood)
		aFood->mCanBeEatenDelay = 40;
}

GameObject* Sexy::Board::SpawnGuppyAsFood()
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	PlaySplashSound();
	return SpawnGuppyBought();
}

GameObject* Sexy::Board::SpawnStarAsFood()
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	// the original draws x (mSeed) before y (Rand), both before operator new
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = Rand() % 50 + 105;
	Coin* aCoin = new Coin(aX, aY, 3, nullptr, -1.0);
	aCoin->mMouseVisible = false;
	AddGameObject(aCoin, false);
	mWidgetManager->AddWidget(aCoin);
	SortGameObjects();
	return aCoin;
}

GameObject* Sexy::Board::SpawnLarvaAsFood()
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	// the original draws x (mSeed) before y (Rand), both before operator new
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = 360 - Rand() % 50;
	Larva* aLarva = new Larva(aX, aY);
	aLarva->m0x175 = true;
	AddGameObject(aLarva, false);
	mWidgetManager->AddWidget(aLarva);
	SortGameObjects();
	return aLarva;
}

GameObject* Sexy::Board::SpawnOscarAsFood()
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	PlaySplashSound();
	return SpawnOscarBought();
}

GameObject* Sexy::Board::SpawnUltraAsFood()
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	PlaySample(SOUND_SPLASHBIG_ID, 3, 1.0);
	return SpawnUltraBought();
}

GameObject* Sexy::Board::SpawnExoticFood(int theType)
{
	PlaySample(SOUND_GROW_ID, 3, 1.0);
	int aX = mApp->mSeed->Next() % 450 + 50; // drawn before operator new in the original
	Food* aFood = new Food(aX, 115, 0, 0, theType);
	AddGameObject(aFood, true);
	mWidgetManager->AddWidget(aFood);
	SortGameObjects();
	return aFood;
}

void Sexy::Board::CyraxSpawnMiniAlien(int theX, int theY)
{
	Alien* aMiniAlien = new Alien(theX + 40, theY + 40, ALIEN_MINI_SYLV);
	AddGameObject(aMiniAlien, true);
	mWidgetManager->AddWidget(aMiniAlien);
	MakeShadowForGameObject(aMiniAlien);
	SortGameObjects();
}

void Sexy::Board::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mBoardOverlay1);
	theWidgetManager->AddWidget(mBoardOverlay2);
}

void Board::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mBoardOverlay1);
	theWidgetManager->RemoveWidget(mBoardOverlay2);
	RemoveWidgetHelper(mMenuButton);
	mMenuButton = 0;
	RemoveWidgetHelper(mMoneyLabel);
	mMoneyLabel = 0;
	RemoveWidgetHelper(mMessageWidget);
	mMessageWidget = 0;
	RemoveWidgetHelper(mMenuButtons[SLOT_GRUBBER]);
	mMenuButtons[SLOT_GRUBBER] = 0;
	RemoveWidgetHelper(mMenuButtons[SLOT_GEKKO]);
	mMenuButtons[SLOT_GEKKO] = 0;
	RemoveWidgetHelper(mMenuButtons[SLOT_ULTRA]);
	mMenuButtons[SLOT_ULTRA] = 0;
	RemoveWidgetHelper(mMenuButtons[SLOT_WEAPON]);
	mMenuButtons[SLOT_WEAPON] = 0;
	RemoveWidgetHelper(mMenuButtons[SLOT_EGG]);
	mMenuButtons[SLOT_EGG] = 0;
	RemoveWidgetHelper(mBackButton);
	mBackButton = 0;
	ResetLevel(theWidgetManager);
}

void Board::Update()
{
	Widget::Update();

	int aLastMX = mApp->mWidgetManager->mLastMouseX;
	int aLastMY = mApp->mWidgetManager->mLastMouseY;

	if (mDropFoodDelay != 0)
		mDropFoodDelay--;
	// a signed comparison of the song count in the original (0x54768A)
	if (mPause || (gUnkBool02 = true, (int)mFishSongMgr->mSongList.size() <= 0))
		gUnkBool02 = false;
	if (mPause)
		return;

	mFishSongMgr->Update();

	if (((mApp->mCurrentProfile->mCheatCodeFlags >> 3) & 1))
		mStarField->Update();
	mBubbleMgr->Update();
	bool isScrSvr = mApp->IsScreenSaver();
	if (isScrSvr && mUpdateCnt % 10800 == 0 && mApp->mScreenSaverRotateBackdrops) // 72 line ghidra
	{
		std::vector<int> aCandidateBGS;

		for (int i = 0; i < 6; i++)
			if (mApp->mCurrentProfile->mUnlockedBackgrounds[i] && (i + 1 != mCurrentBackgroundId))
				aCandidateBGS.push_back(i);

		if (!aCandidateBGS.empty())
		{
			int aRandIdx = Rand() % aCandidateBGS.size();
			if (m0x3ec == -1)
				m0x3ec = mCurrentBackgroundId;
			ChangeBackground(aCandidateBGS[aRandIdx] + 1);
			UpdateNikoPosition();
		}
	}
	
	mApp->mPlaytimeCounter++;

	if (mApp->mGameMode == GAMEMODE_CHALLENGE && m0x2e0 < m0x2dc && mGameUpdateCnt > 10800) // 105
	{
		mApp->PlaySample(SOUND_BUTTONCLICK);
		for (int i = 0; i < SLOT_END; i++) MakeAndUnlockMenuButton(i, true);
	}

	if (m0x2a7) // 113
	{
		m0x2a7 = false;
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			WidgetSetupVT();
		Unk14(m0x2a6);
	}

	if (m0x2b4 != 0)
		m0x2b4--;

	if (mBubbulatorShown) //123
	{
		int aTimer = mBubbulatorTimer % 500;
		int aTimer2 = 0;
		if (aTimer < 60)
			aTimer2 = aTimer % 20;
		m0x44c = aTimer2;

		if (aTimer2 == 13)
		{
			if (aTimer < 20)
				PlaySample(SOUND_BUBBLES_ID, 3, 1.0);
			Point aPos = GetBubbleSpawnCoords();
			int aNumOfBubbles = Rand() % 5 + 5;
			for (int i = 0; i < aNumOfBubbles; i++)
			{
				// The original draws the x offset before the y offset
				int aDX = Rand() % 41 - 20;
				int aDY = Rand() % 10;
				SpawnBubble(aPos.mX + aDX + 25, aPos.mY - aDY - 15);
			}
		}
		mBubbulatorTimer++;
	}

	if (mApp->mRelaxMode) //149
	{
		if (gFoodType != 2)
		{
			if (mFishList->size() >= 5)
				gFoodType = 2;
		}
		if (gFoodLimit < 9)
		{
			if (gFoodLimit < (int)mFishList->size())
				gFoodLimit = mFishList->size();
		}
	}

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL) //176 for highscores
	{
		m0x3b0 = (Unk01() - m0x3b8) / 1000;
		if (m0x3bc - m0x3b0 < 0)
		{
			mShouldSave = false;
			Unk04();
			mApp->mHighScoreMgr->RecordTimeTrialHighScore(mTank, mApp->mCurrentProfile, mMoney);
			mApp->DoTimesUpDialog();
			return;
		}
	}

	if (mApp->mGameMode == GAMEMODE_CHALLENGE) //190
		m0x3b0 = (Unk01()-m0x3b8)/1000;

	if (mIsBonusRound)
	{
		if (!mBonusRoundStarted)
		{
			if (mGameUpdateCnt - m0x2a8 >= 160)
				Unk05();
		}
		else // 200
		{
			m0x3b0 = (Unk01() - m0x3b8) / 1000;
			if (m0x3bc - m0x3b0 < 0)
			{
				if (mCoinList->empty() && m0x3f8++ > 100)
				{
					mApp->mCurrentProfile->NextLevel();
					if (mLevel <= 5)
						mApp->mHighScoreMgr->RecordAdventureHighScore(mTank, mLevel, mApp->mCurrentProfile, m0x3fc);
					mApp->SwitchToBonusScreen();
					return;
				}
			}
			else if(mGameUpdateCnt % 10 == 0)
			{
				BonusRoundDropShell();
			}
		}
	}

	if (mApp->mGameMode == GAMEMODE_CHALLENGE && (mSlotUnlocked[0] || mSlotUnlocked[1])) // 223
	{
		int aUnk01; // not set for other tanks in the original either (0x547BFB reads the stack slot)
		if (mTank == 1)
			aUnk01 = 150;
		else if(mTank == 2)
			aUnk01 = 150;
		else if(mTank == 3)
			aUnk01 = 175;
		else if(mTank == 4)
			aUnk01 = 200;

		if (m0x454 <= 0 && m0x43c <= 5)
		{
			if (mGameUpdateCnt % aUnk01 == 0)
			{
				for (int i = 0; i < SLOT_END; i++)
				{
					MenuButtonWidget* aBtn = GetMenuButtonById(i);
					if (aBtn)
					{
						mSlotPrices[i] = mSlotPrices[i] + m0x374[i];
						if (mSlotPrices[i] > 99999)
							mSlotPrices[i] = 99999;
						aBtn->SetSlotPrice(mSlotPrices[i]);
					}
				}
				SetMenuButtonsPriceColor(Color(0xfa, 0x9b, 0x6e, 255));
			}
			else if (aUnk01 - 10 < mGameUpdateCnt % aUnk01)
				SetMenuButtonsPriceColor(Color(0xfa, 0x6e, 0x37, 255));
			else
				SetMenuButtonsPriceColor(Color(0xfa, 0x9b, 0x6e, 255));
		}
		else
		{
			int aVal = m0x454 % 14;
			if (aVal >= 7)
				aVal = 14 - aVal;

			SetMenuButtonsPriceColor(Color(aVal * 14 + 110, 250, aVal * 14 + 110, 255));
		}
	}

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && mApp->mCurrentProfile->mShells == 0 && mGameUpdateCnt == 150) // 286
		ShowText("Here you\'ll be able to build your own custom fish tank!", false, 44);

	// Holding the mouse button down keeps feeding / shooting at the position read at the start of Update
	if (m0x4ec)
	{
		if (!mWidgetManager->IsLeftButtonDown())
			m0x4ec = false;
		else if ((mGameUpdateCnt % (16 - gFoodLimit) == 0) && (m0x3c0 + 200 < Unk01()) &&
			(aLastMX >= 31 && aLastMX <= 586) && (aLastMY >= 61 && aLastMY <= 399) && Buy(m0x4ac, false))
		{ // 301
			DropFood(aLastMX - 10, aLastMY - 10, 0, false, 20, -1);
		}
	}

	if (m0x4ed) // 305
	{
		if (!mWidgetManager->IsLeftButtonDown())
			m0x4ed = false;
		else if(m0x3e4 > 11 && mGameUpdateCnt%(11 - m0x3e4 / 2) == 0 && m0x3c4 + 100 < Unk01() && aLastMY > 40)
		{ // 311
			for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end(); it++)
			{
				if ((*it)->Shoot(aLastMX, aLastMY))
					break;
			}

			for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end(); ++it)
			{
				if ((*it)->Shot(aLastMX, aLastMY))
					break;
			}

			for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end(); ++it)
			{
				if ((*it)->Shot(aLastMX, aLastMY))
					break;
			}

			SpawnLaserShot(aLastMX - 40, aLastMY - 40);
			PlayZapSound();
		}
	}

	if (m0x450 > 0)
		m0x450--;
	if (m0x454 > 0)
		m0x454--;
	if (m0x458 > 0)
		m0x458--;

	mTankLightingSpeeds[0] -= 1.6;
	mTankLightingSpeeds[1] += 1.8;
	mTankLightingSpeeds[2] += 2.2;

	// The original's clamp loop runs over 10 floats although there are 3 lighting values, so it also treats the
	// next seven ints (m0x3e4 .. m0x3fc) as floats. Only extreme values (>= 0x44200001 or <= -8388608) change.
	// It walks them with a float pointer (0x548063), as here.
	float* aValues = mTankLightingSpeeds;
	for (int i = 0; i < 10; i++)
	{
		if (aValues[i] > 640)
			aValues[i] = 0;
		else if (aValues[i] < 0)
			aValues[i] = 640;
	}

	if (mApp->mGameMode == GAMEMODE_SANDBOX && m0x2e0 < m0x2dc && mTank != 5) // 424
	{
		for (int i = 0; i < SLOT_END; i++)
			MakeAndUnlockMenuButton(i, true);
		UpdateSlotPrice(SLOT_EGG, 0);
	}
	SortGameObjects();

	if (!mPause)
	{
		mGameUpdateCnt++;
		if (mGameUpdateCnt < 0)
			mGameUpdateCnt = 0x40000000;

		if (mGameUpdateCnt % 2048 == 0)
			Unk13();

		if (!mBubbulatorShown && mApp->mSeed->Next() % 1000 == 0)
		{
			// 3 to 6 bubbles
			for (int i = (mApp->mSeed->Next() & 3) + 2; i >= 0; i--)
				SpawnRandomBubble();
			PlaySample(SOUND_BUBBLES_ID, 3, 1.0);
		}
	}

	if (m0x2c4 != 0) // 458
		m0x2c4--;


	// the original skips the alien, game-over and message code when paused, or in a virtual tank without the
	// alien attractor, and goes on to the shared tail below (0x5481D7)
	if (!mPause && (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK || mAlienAttractorShown))
	{
		if (mAlienList->empty() && mBilaterusList->empty() && mMissleList1->empty())
		{ // 469
			if (mAlienExpect != 0 && mCyraxPtr == nullptr && !m0x4f4)
			{
				mAlienTimer--;
				if (mAlienAttractorShown && mAlienTimer == 33)
					PlaySample(SOUND_UNLEASH_ID, 3, 1.0);
			}
			if (mAlienTimer == 30 && mPetsInTank[PET_NOSTRADAMUS] != 0 && mTank != 5)
				NostradamusSneezeEffect();

			if (mApp->mGameMode == GAMEMODE_CHALLENGE && mAlienTimer == 2999)
			{ // 480
				int aDifIncreaseProb; // not set for other tanks in the original either (0x548315 reads the stack slot)
				if (mTank == 1)
					aDifIncreaseProb = 4;
				else if (mTank == 2)
					aDifIncreaseProb = 5;
				else if (mTank == 3)
					aDifIncreaseProb = 5;
				else if (mTank == 4)
					aDifIncreaseProb = 6;

				if (m0x45c % aDifIncreaseProb == 0 && m0x45c != 0)
				{
					mMessageWidget->mMessage = "WARNING! ALIEN DIFFICULTY INCREASED!";
					mMessageWidget->mIsBlinking = true;
					mMessageWidget->mMessageTimer = 274;
					mMessageWidget->mColor1 = Color(255, 50, 50, 255);
					mMessageWidget->mColor2 = Color(100, 20, 20, 255);
				}
			}
			else
			{ // 516
				if (mAlienTimer == 276)
				{
					// The original reads and sets these message flags through mApp->mBoard
					if (!mApp->mRelaxMode && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
					{
						if (!mApp->mBoard->mMessageShown[3] && IsTankAndLevelNB(1, 2))
						{
							mApp->DoDialogUnkF(DIALOG_INFO, true, "DANGER!", "A vicious alien is about to enter your tank! Defeat it with your laser weapon by clicking on it!", "Click to Continue", Dialog::BUTTONS_FOOTER);
							mApp->mBoard->mMessageShown[3] = true;
						}
						else if (!mApp->mBoard->mMessageShown[4] && IsTankAndLevelNB(1, 2))
						{
							mApp->DoDialogUnkF(DIALOG_INFO, true, "BATTLE TIP", "Shoot the alien\'s head to push it downwards! Shoot its tail to deflect it upwards!", "Click to Continue", Dialog::BUTTONS_FOOTER);
							mApp->mBoard->mMessageShown[4] = true;
						}
						else if (!mApp->mBoard->mMessageShown[17] && IsTankAndLevelNB(2, 3))
						{
							mApp->DoDialogUnkF(DIALOG_INFO, true, "WARNING!", "A new breed of alien is fast approaching!  Lasers can\'t hurt this baddie, so find another way to defeat it!", "Click to Continue", Dialog::BUTTONS_FOOTER);
							mApp->mBoard->mMessageShown[17] = true;
						}
					}
				}
				else if (mAlienTimer == 275)
				{
					if(!mApp->mBoard->mMessageShown[46] && mLevel == 4 && mTank == 4 && (mAlienExpect == 9 || mAlienExpect == 10
						|| mAlienExpect == 11 || mAlienExpect == 12))
						mMessageWidget->mMessage = "FOURTEEN ALIEN SIGNATURES DETECTED";
					else
					{
						// Cyrax is tested before the multiple-alien types in the original (0x5487DB)
						if (mAlienExpect == ALIEN_GUS)
							mMessageWidget->mMessage = "ALIEN SIGNATURE TYPE-G DETECTED";
						else if (mAlienExpect == ALIEN_DESTRUCTOR)
							mMessageWidget->mMessage = "ALIEN SIGNATURE TYPE-D DETECTED";
						else if (mAlienExpect == ALIEN_ULYSEES)
							mMessageWidget->mMessage = "ALIEN SIGNATURE TYPE-U DETECTED";
						else if (mAlienExpect == ALIEN_PSYCHOSQUID)
							mMessageWidget->mMessage = "ALIEN SIGNATURE TYPE-P DETECTED";
						else if (mAlienExpect == ALIEN_BILATERUS)
							mMessageWidget->mMessage = "ALIEN SIGNATURE TYPE-II DETECTED";
						else if (mAlienExpect == ALIEN_CYRAX)
							mMessageWidget->mMessage = StrFormat("%s OF DOOM APPROACHING", GetCyraxEndGameString(mApp->mCurrentProfile->mCyraxNum + 1));
						else if (mAlienExpect == 9 || mAlienExpect == 10 || mAlienExpect == 11 || mAlienExpect == 12)
							mMessageWidget->mMessage = "MULTIPLE ALIEN SIGNATURES DETECTED";
						else
							mMessageWidget->mMessage = "ENEMY APPROACHING";
					}
					mMessageWidget->mIsBlinking = false;
					mMessageWidget->mMessageTimer = 274;
					mMessageWidget->mColor1 = Color(255, 50, 50, 255);
					mMessageWidget->mColor2 = Color(100, 20, 20, 255);
					mApp->SomeMusicFunc(true);
					PlaySample(SOUND_AWOOGA_ID, 3, 1.0);
					if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
						DetermineAlienSpawnCoordsVT();
					else
					{
						mCrosshair1X = mApp->mSeed->Next() % 450 + 20;
						mCrosshair1Y = mApp->mSeed->Next() % 195 + 105;
						mCrosshair2X = mApp->mSeed->Next() % 450 + 20;
						mCrosshair2Y = mApp->mSeed->Next() % 195 + 105;
					}
				}
				// the original tests mAlienTimer > 0 and < 275 first, then <= 0 again (0x5489B1, 0x548A4F)
				else if (mAlienTimer > 0 && mAlienTimer < 275)
				{
					if (!mApp->mBoard->mMessageShown[46] && mLevel == 4 && mTank == 4 && (mAlienExpect == 9 || mAlienExpect == 10 ||
						mAlienExpect == 11 || mAlienExpect == 12) && mAlienTimer == 100)
					{
						mMessageWidget->mMessage = "JUST KIDDING. ONLY TWO!";
						mApp->mBoard->mMessageShown[46] = true;
					}
					if (mAlienTimer == 1)
						mApp->PlayMusic(1, 1);
				}
				else if (mAlienTimer <= 0)
				{
					m0x2c4 = 35;
					m0x4ed = false;
					m0x4ec = false;
					SpawnAlien(mAlienExpect, mCrosshair1X, mCrosshair1Y, true);
					if (mApp->mGameMode == GAMEMODE_CHALLENGE)
					{
						int aDifIncreaseProb; // not set for other tanks in the original either (0x548AD0 reads the stack slot)
						if (mTank == 1)
							aDifIncreaseProb = 4;
						else if (mTank == 2)
							aDifIncreaseProb = 5;
						else if (mTank == 3)
							aDifIncreaseProb = 5;
						else if (mTank == 4)
							aDifIncreaseProb = 6;

						int aVal = m0x45c / aDifIncreaseProb;
						if (mAlienExpect == 12)
							aVal--;

						for (int i = 0; i < aVal; i++)
						{
							mCrosshair1X = mApp->mSeed->Next() % 450 + 20;
							mCrosshair1Y = mApp->mSeed->Next() % 195 + 105;
							mCrosshair2X = mApp->mSeed->Next() % 450 + 20;
							mCrosshair2Y = mApp->mSeed->Next() % 195 + 105;
							SpawnAlien(mAlienExpect, mCrosshair1X, mCrosshair1Y, false);
						}
					}

					if (mApp->mRelaxMode) // 754
						RelaxModeConfig();
					else if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
						SetAlienExpectVT();
					// a flat chain in the original: mTank is tested again for each level (0x548CF0, 0x548D57, 0x548D86)
					else if (mTank == 1 && mLevel == 5)
						mAlienExpect = ((mApp->mSeed->Next() % 2 != 0) ? ALIEN_BALROG : 9);
					else if (mTank == 2 && mLevel == 5)
					{
						if(mAlienExpect == 9)
							mAlienExpect = ((mApp->mSeed->Next() % 2 != 0) ? ALIEN_GUS : ALIEN_DESTRUCTOR);
						if (mApp->mSeed->Next() % 10 == 0)
							mAlienExpect = (mAlienExpect != ALIEN_DESTRUCTOR) + ALIEN_GUS;
						else if (mApp->mSeed->Next() % 20 == 0)
							mAlienExpect = 9;
					}
					else if (mTank == 3 && mLevel == 2)
					{
						if(mApp->mSeed->Next() % 10 == 0)
							mAlienExpect = (mAlienExpect != ALIEN_DESTRUCTOR) + ALIEN_GUS;
					}
					else if (mTank == 3 && mLevel == 5)
						mAlienExpect = (mApp->mSeed->Next() % 2 != 0 ? ALIEN_ULYSEES : ALIEN_PSYCHOSQUID);
					else if (mTank == 4 && mLevel == 3)
						mAlienExpect = (mApp->mSeed->Next() % 2 != 0 ? ALIEN_GUS : 10);
					else if (mTank == 4 && mLevel == 4)
						mAlienExpect = (mApp->mSeed->Next() % 2 != 0 ? ALIEN_BILATERUS : 11);
					else if (mTank == 4 && mLevel == 5)
					{
						int anAlienChance = mApp->mSeed->Next() % 5;
						if (anAlienChance <= 1)
							mAlienExpect = 10;
						else
							mAlienExpect = (anAlienChance > 3 ? ALIEN_BILATERUS : 11);
						if (mApp->mGameMode == GAMEMODE_CHALLENGE && m0x45c >= 5 && mAlienExpect == ALIEN_BILATERUS)
							mAlienExpect = 12;
					} // 818
					m0x45c++;
					mAlienTimer = 3000;
				}
			}
		}

		if (!mIsBonusRound && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			if (!HasAnyFish() && mTank != 5)
			{
				PauseGame(true);
				// the first-level dialog comes first in the original (its unwind states precede the other's)
				if (IsFirstLevel())
				{
					mApp->DoDialogUnkF(DIALOG_FIRST_LVL_GAME_OVER, true, "YOUR LAST FISH HAS DIED!", "Normally this would end your game, but this time we\'ll give you another fish to keep playing! Make sure you keep it well fed!", "Click to Continue", Dialog::BUTTONS_FOOTER);
				}
				else
				{
					mShouldSave = false;
					Unk04();
					mApp->DoDialogUnkF(DIALOG_GAME_OVER, true, "GAME OVER", "Oops!  All of your fish have died!", "Click to Continue", Dialog::BUTTONS_FOOTER);
				}
			}
			if(mTank == 5)
			{
				if (mFishTypePetList->empty() && mOtherTypePetList->empty())
				{
					PauseGame(true);
					mShouldSave = false;
					if (mCyraxPtr != nullptr && mCyraxPtr->mMaxHealth - mCyraxPtr->mHealth > 1000.0)
						mApp->mCurrentProfile->mFinishedGameCount++;
					mApp->DoDialogUnkF(DIALOG_GAME_OVER, true, "GAME OVER", "Oops!  All of your pets have died!", "Click to Continue", Dialog::BUTTONS_FOOTER);

				}
			}
		}

		if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			if (mApp->mGameMode == GAMEMODE_ADVENTURE && IsTankAndLevelNB(1, 1) && mGameUpdateCnt == 150)
				ShowText("Here are your first fish! Take good care of them!", false, 1);
			else if(mApp->mGameMode == GAMEMODE_ADVENTURE && IsTankAndLevelNB(1,2) && mGameUpdateCnt == 150)
				ShowText("On this level you will meet your 1st alien opponent!", false, 20);
			else if(mApp->mGameMode == GAMEMODE_TIME_TRIAL && mGameUpdateCnt == 150)
				ShowText("How much money can you earn before time runs out?", false, 53);
			else if(mApp->mGameMode == GAMEMODE_CHALLENGE && mGameUpdateCnt == 150)
				ShowText("Can you fend off the increasingly hungry aliens?", false, 51);
			else if(mApp->mGameMode == GAMEMODE_SANDBOX && mGameUpdateCnt == 150)
				ShowText("Make sure caps-lock is off and GO TO TOWN!!", false, 48);
		}
	}

	if (mLevel == 1 && mTank == 1 && m0x4e6)
	{
		if (mMoney > mSlotPrices[SLOT_EGG])
			m0x4e8++;
		if (m0x4e8 > 1500)
			mMessageShown[40] = true;
	}

	if (mTank == 5 && mCyraxPtr == nullptr && m0x4f4 && !mIsBonusRound && !mSlotUnlocked[SLOT_EGG])
		MakeAndUnlockMenuButton(SLOT_EGG, true);

	MarkDirty();
}

void Board::Draw(Graphics* g)
{
	DrawTankBackground(g);

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (!mApp->IsScreenSaver())
			g->DrawImage(IMAGE_TROPHYBAR, 0, 0);
	}
	else
		g->DrawImage(IMAGE_MENUBAR, 0, 0);

	if (m0x450 > 0 && m0x450 % 20 < 10)
		g->DrawImage(IMAGE_MONEYFLASH, 545, 39);

	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	g->SetColor(Color(0x6e, 0xfa, 0x6e, 0xff));
	if (mMessageShown[2] && mLevel == 1 && mTank == 1 && !mApp->mCurrentProfile->mFinishedGame) // 52
	{
		g->DrawImage(IMAGE_HELPARROW, 40, 70);
		g->DrawString("Click here to", 65, 95);
		g->DrawString("buy fish!", 83, 107);
	} // 65
	else if (mMessageShown[40] && mLevel == 1 && mTank == 1 && !mApp->mCurrentProfile->mFinishedGame) //66
	{
		g->DrawImage(IMAGE_HELPARROW, 460, 70);
		g->DrawString("Click here to", 485, 95);
		g->DrawString("buy egg piece!", 480, 107);
	}
	else if (mMessageShown[15] && mLevel == 2 && mTank == 1 && !mApp->mCurrentProfile->mFinishedGame)
	{
		g->DrawImage(IMAGE_HELPARROW, 110, 70);
		g->DrawString("Click here to", 135, 95);
		g->DrawString("upgrade!", 152, 107);
	}
	else if (mTank == 5 && m0x4f4 && !mIsBonusRound)
	{
		g->DrawImage(IMAGE_HELPARROW, 460, 70);
		g->DrawString("Click here to", 485, 95);
		g->DrawString("end the level!", 480, 107);
	}
	else if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && !mHasVirtualTankFish && !mApp->GetDialog(39) && !mApp->IsScreenSaver())
	{
		g->DrawImage(IMAGE_HELPARROW, 100, 70);
		g->DrawString("Click here to", 125, 95);
		g->DrawString("buy fish!", 140, 107);
	} // 132

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		return;

	g->SetFont(FONT_CONTINUUMBOLD12);
	// An if-chain in the original; each branch builds its colour for SetColor (the calls are tail-merged)
	if (mCurrentBackgroundId == 2)
		g->SetColor(Color(0xdc, 0x96, 0x96, 0xff));
	else if (mCurrentBackgroundId == 4)
		g->SetColor(Color(0x3c, 0xb4, 0x50, 0xff));
	else if (mCurrentBackgroundId == 3)
		g->SetColor(Color(0x7d, 200, 0xd7, 0xff));
	else
		g->SetColor(Color(0xa5, 0x8c, 0x50, 0xff));

	if (mApp->mRelaxMode)
	{
		g->DrawString("Relax", 15, 470);
	}
	else
	{
		// The original formats every label with sprintf into a stack buffer
		char aStr[256];
		if (mApp->mGameMode == GAMEMODE_CHALLENGE)
		{
			sprintf(aStr, "Challenge");
			g->DrawString(aStr, 15, 470);
		}
		else if(mApp->mGameMode == GAMEMODE_TIME_TRIAL)
		{
			sprintf(aStr, "Time Trial");
			g->DrawString(aStr, 15, 470);
		}
		else if(mApp->mGameMode == GAMEMODE_SANDBOX)
		{
			sprintf(aStr, "Sandbox");
			g->DrawString(aStr, 15, 470);
		}
		else if (mIsBonusRound && !mApp->mCurrentProfile->mFinishedGame)
		{
			sprintf(aStr, "Bonus Round");
			g->DrawString(aStr, 15, 470);
		}
		else if (mTank != 5)
		{
			sprintf(aStr, "Tank %d-%d", mTank, mLevel);
			g->DrawString(aStr, 15, 470);
		}
	}

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		int aTime = m0x3bc - m0x3b0;
		int aMins = aTime / 60;
		int aSecs = aTime % 60;
		if (aMins < 0) aMins = 0; if (aSecs < 0) aSecs = 0;
		char aStr[256];
		if (mGameUpdateCnt >= 347)
		{
			sprintf(aStr, "Time Remaining: %d:%02d", aMins, aSecs);
			g->DrawString(aStr, 465, 470);
		}
		else
		{
			// The label width is measured first, on a temporary string
			int aLabelWidth = g->GetFont()->StringWidth("Time Remaining: ");
			sprintf(aStr, "%d:%02d", aMins, aSecs);
			g->DrawString(aStr, aLabelWidth + 465, 470);
		}
	}
	else if (((mApp->mCurrentProfile->mCheatCodeFlags >> 7) & 1) != 0)
	{
		if (!mIsBonusRound)
		{
			if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
			{
				int aTime = (Unk01() - m0x3b8) / 1000;
				int aMins = aTime / 60;
				int aSecs = aTime % 60;
				if (aMins < 0)
					aMins = 0;
				if (aSecs < 0)
					aSecs = 0;
				char aStr[256];
				sprintf(aStr, "Time: %d:%02d", aMins, aSecs);
				g->DrawString(aStr, 540, 470);
			}
		}
	}
	// Shared by every path above, including the time cheat during a bonus round (0x53FEE9, 0x53FF99)
	if (mIsBonusRound)
		DrawBonusRound(g); // 276

	if (mAlienTimer <= 0 || mAlienTimer > 225 || mPetsInTank[13] == 0 || mTank == 5 || mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		return;
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	mCrosshair1Y = 280;
	g->DrawImageCel(IMAGE_CROSSHAIR, mCrosshair1X + 40, mCrosshair1Y + 40, (mGameUpdateCnt / 4) % 5);
	if (mAlienExpect == 9 || mAlienExpect == 10 || mAlienExpect == 11 || mAlienExpect == 12)
	{
		if (mAlienExpect == 11)mCrosshair2Y = 280;
		g->DrawImageCel(IMAGE_CROSSHAIR, mCrosshair2X + 40, mCrosshair2Y + 40, (mGameUpdateCnt / 4 + 2) % 5);
	}
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);

	if (mAlienTimer == 225)
		PlaySample(SOUND_SONAR_ID, 3, 1.0);
}

void Board::DrawOverlay(Graphics* g, int thePriority)
{
	switch (thePriority) // sub 0 / je, sub 1 / jne in the original
	{
	case 0:
		DrawOverlay0(g);
		break;
	case 1:
		DrawOverlay1(g);
		break;
	}
}

void Sexy::Board::KeyChar(SexyChar theChar)
{
	if (mApp->mGameMode == GAMEMODE_SANDBOX)
	{
		bool gusInTank = false;
		if (!mAlienList->empty())
			if (mAlienList->front()->mAlienType == ALIEN_GUS)
				gusInTank = true;

		if (theChar == '1')
			SpawnGuppyBought();
		else if (theChar == '2')
			SpawnOscarBought();
		else if (theChar == '3')
			SpawnStarGuppyBought();
		else if (theChar == '4')
			SpawnPentaBought();
		else if (theChar == '5')
			SpawnGrubberBought();
		else if (theChar == '6')
			SpawnGekkoBought();
		else if (theChar == '7')
			SpawnBreederBought();
		else if (theChar == '8')
			SpawnUltraBought();
		else if (theChar == 'z')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_STRONG_SYLV;
				SpawnAlien(ALIEN_STRONG_SYLV, true);
			}
		}
		else if (theChar == 'x')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_BALROG;
				SpawnAlien(ALIEN_BALROG, true);
			}
		}
		else if (theChar == 'c')
		{
			mAlienExpect = ALIEN_GUS;
			SpawnAlien(ALIEN_GUS, true);
		}
		else if (theChar == 'v')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_DESTRUCTOR;
				SpawnAlien(ALIEN_DESTRUCTOR, true);
			}
		}
		else if (theChar == 'b')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_ULYSEES;
				SpawnAlien(ALIEN_ULYSEES, true);
			}
		}
		else if (theChar == 'n')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_PSYCHOSQUID;
				SpawnAlien(ALIEN_PSYCHOSQUID, true);
			}
		}
		else if (theChar == 'm')
		{
			if (!gusInTank)
			{
				mAlienExpect = ALIEN_BILATERUS;
				SpawnAlien(ALIEN_BILATERUS, true);
			}
		}
		else if (theChar == 'q')
			SpawnPet(PET_STINKY, -1, -1, false, false);
		else if (theChar == 'w')
			SpawnPet(PET_NIKO, -1, -1, false, false);
		else if (theChar == 'e')
			SpawnPet(PET_ITCHY, -1, -1, false, false);
		else if (theChar == 'r')
			SpawnPet(PET_PREGO, -1, -1, false, false);
		else if (theChar == 't')
			SpawnPet(PET_ZORF, -1, -1, false, false);
		else if (theChar == 'y')
			SpawnPet(PET_CLYDE, -1, -1, false, false);
		else if (theChar == 'u')
			SpawnPet(PET_VERT, -1, -1, false, false);
		else if (theChar == 'i')
			SpawnPet(PET_RUFUS, -1, -1, false, false);
		else if (theChar == 'o')
			SpawnPet(PET_MERYL, -1, -1, false, false);
		else if (theChar == 'p')
			SpawnPet(PET_WADSWORTH, -1, -1, false, false);
		else if (theChar == 'a')
			SpawnPet(PET_SEYMOUR, -1, -1, false, false);
		else if (theChar == 's')
			SpawnPet(PET_SHRAPNEL, -1, -1, false, false);
		else if (theChar == 'd')
			SpawnPet(PET_GUMBO, -1, -1, false, false);
		else if (theChar == 'f')
			SpawnPet(PET_BLIP, -1, -1, false, false);
		else if (theChar == 'g')
			SpawnPet(PET_RHUBARB, -1, -1, false, false);
		else if (theChar == 'h')
			SpawnPet(PET_NIMBUS, -1, -1, false, false);
		else if (theChar == 'j')
			SpawnPet(PET_AMP, -1, -1, false, false);
		else if (theChar == 'k')
			SpawnPet(PET_GASH, -1, -1, false, false);
		else if (theChar == 'l')
			SpawnPet(PET_ANGIE, -1, -1, false, false);
		else if (theChar == ';')
			SpawnPet(PET_PRESTO, -1, -1, false, false);
		else if (theChar == 'A')
			SpawnPet(PET_BRINKLEY, -1, -1, false, false);
		else if (theChar == 'S')
			SpawnPet(PET_NOSTRADAMUS, -1, -1, false, false);
		else if (theChar == 'D')
			SpawnPet(PET_WALTER, -1, -1, false, false);
		else if (theChar == 'F')
			SpawnPet(PET_STANLEY, -1, -1, false, false);
		else if (theChar == '+' || theChar == '-')
		{
			ChangeBackground(theChar == '+' ? mCurrentBackgroundId + 1 : mCurrentBackgroundId - 1);
			UpdateNikoPosition();
		}
	}
	else
	{
		for (int i = 0; i < 8; i++) // a signed int bound in the original (0x54A2F4)
			if (mCheatCodes[i]->CheckCodeActivated(theChar))
				if (DoCheatCode(i))
					return;
		if (theChar == 'b' || theChar == 'B')
		{
			if (mBubbulatorShown)
			{
				if (mBubbulatorTimer > 60)
					mBubbulatorTimer = 40;
			}
			else
			{
				// 3 to 6 bubbles
				for (int i = (mApp->mSeed->Next() & 3) + 2; i >= 0; i--)
					SpawnRandomBubble();
			}
		}
	}
	if (theChar == ' ')
		mApp->DoLostFocusDialog();
}

void Board::KeyDown(KeyCode theKey)
{
	if (!mApp->mDebugKeysEnabled)
		for (int i = 0; i < 8; i++) // a signed compare in the original
			if (mCheatCodes[i]->CheckCodeActivated(theKey))
				DoCheatCode(i);
}

void Board::MouseDown(int x, int y, int theClickCount)
{
	Widget::MouseDown(x, y, theClickCount);

	if (mIsBonusRound)
	{
		if (theClickCount >= 0)
		{
			if (!mBonusRoundStarted)
				Unk05();
			return;
		}
	}

	if (theClickCount < 0)
	{
		if (y > 40)
			CheckMouseDown(x, y);
		return;
	}

	if (mDropFoodDelay > 0 && (m0x3d0 - x) * (m0x3d0 - x) + (m0x3d4 - y) * (m0x3d4 - y) > 2500)
		mDropFoodDelay = 0;

	bool playZapSound = false;
	if (!mSmallAlienList->empty() && mCyraxPtr && y > 40)
	{
		for (std::vector<Alien*>::iterator it = mSmallAlienList->begin(); it != mSmallAlienList->end(); it++)
		{
			if ((*it)->Shot(x, y))
				break;
		}
		playZapSound = true;
		SpawnLaserShot(x - 40, y - 40);
	}

	if (mCyraxPtr)
	{
		mCyraxPtr->Shot(x, y);
		playZapSound = true;
		SpawnLaserShot(x - 40, y - 40);
	}

	if (!mAlienList->empty() || !mBilaterusList->empty() || (mCyraxPtr && !MisslesInTank()))
	{
		if (!mBilaterusList->empty() && y > 40)
		{
			for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end(); ++it)
			{
				if ((*it)->Shoot(x, y))
					break;
			}
			playZapSound = true;
			SpawnLaserShot(x - 40, y - 40);
			m0x4ed = true;
			m0x3c4 = Unk01();
		}

		if (!mAlienList->empty())
		{
			// the original reads mCyraxPtr before the first alien (0x543D44)
			bool hasCyrax = mCyraxPtr != nullptr;
			if (mAlienList->front()->mAlienType == ALIEN_GUS && !hasCyrax)
			{
				if (x >= 31 && x <= 586 && y >= 61 && y <= 379)
				{
					if (mAlienList->front()->FoodDroppedAtAlien(x, y))
						DropFood(x - 10, y - 10, 0, false, 20, -1);
					else
						DropFood(x - 10, y - 10, 0, false, 0, -1);
					m0x4ec = true;
					m0x3c0 = Unk01();
				}
			}
			else if (y > 40)
			{
				for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end(); ++it)
				{
					if ((*it)->Shot(x, y))
						break;
				}
				for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end(); ++it)
				{
					if ((*it)->Shot(x, y))
						break;
				}
				SpawnLaserShot(x - 40, y - 40);
				m0x4ed = true;
				m0x3c4 = Unk01();
				PlayZapSound();
				return;
			}
		}
	}
	else if (!mMissleList1->empty() || mCyraxPtr)
	{
		if (y > 40)
		{
			for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end(); ++it)
			{
				if ((*it)->Shot(x, y))
					break;
			}
			SpawnLaserShot(x - 40, y - 40);
			m0x4ed = true;
			m0x3c4 = Unk01();
			PlayZapSound();
			return;
		}
	}
	else if ((mTank != 5 || mApp->mGameMode == GAMEMODE_VIRTUAL_TANK) && x >= 31 && x <= 586 && y >= 61 && y <= 399 &&
		mDropFoodDelay <= 0)
	{
		if (m0x440 || Buy(m0x4ac, true))
			DropFood(x - 10, y - 10, 0, false, 0, -1);
		// Also when the pellet could not be bought, as the original: holding the button keeps trying
		m0x4ec = true;
		m0x3c0 = Unk01();
	}

	if (playZapSound)
		PlayZapSound();
}

void Board::MouseUp(int x, int y, int theClickCount)
{
	Widget::MouseUp(x, y, theClickCount);
	m0x4ec = false;
	m0x4ed = false;
}

void Board::MouseMove(int x, int y)
{
	// 0x4D8C10 (Board vtable slot 52), shared with the MouseDrag forwarders: the empty Widget::MouseMove
	Widget::MouseMove(x, y);
}

void Board::MouseDrag(int x, int y)
{
	// 0x4D8C10 (Board vtable slot 58): forwards to the empty Widget::MouseDrag
	Widget::MouseDrag(x, y);
}

void Board::MouseLeave()
{
	// 0x538000 (Board vtable slot 51, a jmp): forwards to the empty Widget::MouseLeave
	Widget::MouseLeave();
}

void Sexy::Board::ButtonPress(int theId)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK && theId < SLOT_END)
	{
		HandleBuySlotPressed(theId);
		return;
	}
	switch (theId)
	{
	case 10:
		mApp->SwitchToStoreScreen();
		return;
	case 11:
		mApp->SwitchToSimFishScreen();
		return;
	case 12:
		mApp->SwitchToPetsScreen();
		return;
	case 13:
		mApp->SwitchToSimSetupScreen();
		return;
	case 14:
		if (!mApp->KillFoodDialog())
		{
			HandleFeedButton();
			return;
		}
		break;
	case 15:
		mApp->LeaveGameBoard();
		return;
	}
}

void Sexy::Board::ButtonDepress(int theId)
{
	if (theId == 123)
		mApp->DoOptionsDialog(false);
}

void Board::DrawOverlay0(Graphics* g)
{
	if (mBubbulatorShown)
	{
		Point aPos = GetBubbleSpawnCoords();
		g->SetColorizeImages(true);
		g->SetColor(Color(0));
		g->DrawImage(IMAGE_SHADOW, aPos.mX + 10, aPos.mY + 64);
		g->SetColorizeImages(false);
		g->DrawImageCel(IMAGE_BUBBULATOR, aPos.mX, aPos.mY, m0x44c);
	}
	if (mAlienAttractorShown)
	{
		Point aPos = GetAlienAttractorSpawnCoords();
		g->SetColor(Color(255, 255, 128, 160));
		if (mAlienTimer < 30 || m0x2c4 > 0)
		{
			// The beam grows over the last 5 frames before the aliens arrive and rises from the attractor
			int aRow = m0x2c4;
			if (aRow <= 0)
				aRow = 30 - mAlienTimer;
			if (aRow > 5)
				aRow = 5;
			int aBeamHeight = 450 * aRow / 5;
			g->DrawImageBox(Rect(aPos.mX - IMAGE_AA_BEAM->mWidth / 2 + 40, aPos.mY - aBeamHeight + 40, IMAGE_AA_BEAM->mWidth, aBeamHeight), IMAGE_AA_BEAM);
		}
		DrawAlienAttractorMisc(g, aPos.mX, aPos.mY, mAlienTimer, m0x2c4);
	}
	if (m0x4fc)
	{
		for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
		{
			GameObject* anObj = *it;
			if (anObj->mShown && anObj->mInvisible && anObj->mInvisibleTimer == 0)
			{
				g->Translate(anObj->mX, anObj->mY);
				anObj->DrawName(g, true);
				g->Translate(-anObj->mX, -anObj->mY);
			}
		}
	}
}

void Board::SpawnBubble(int theX, int theY)
{
	if ((int)mBubbleMgr->mBubbleList.size() < 30) // a signed compare in the original
		mBubbleMgr->SpawnBubble(theX, theY);
}

void Sexy::Board::SpawnRandomBubble()
{
	if ((int)mBubbleMgr->mBubbleList.size() < 50) // a signed compare in the original
	{
		// The original draws x before y
		int aX = mApp->mSeed->Next() % 22 + 150;
		int aY = mApp->mSeed->Next() % 6 + 400;
		mBubbleMgr->SpawnBubble(aX, aY);
	}
}

void Board::AddGameObject(GameObject* theObject, bool theFlag)
{
	bool someFlag = true;
	switch (theObject->mType)
	{
	case TYPE_GUPPY:
		mFishList->push_back((Fish*) theObject);
		break;
	case TYPE_OSCAR:
		mOscarList->push_back((Oscar*)theObject);
		break;
	case TYPE_ULTRA:
		mUltraList->push_back((Ultra*)theObject);
		break;
	case TYPE_GEKKO:
		mGekkoList->push_back((Gekko*)theObject);
		break;
	case TYPE_PENTA:
		mPentaList->push_back((Penta*)theObject);
		break;
	case TYPE_GRUBBER:
		mGrubberList->push_back((Grubber*)theObject);
		break;
	case TYPE_BREEDER:
		mBreederList->push_back((Breeder*)theObject);
		break;
	case TYPE_OTHER_TYPE_PET:
	{
		OtherTypePet* aPet = (OtherTypePet*)theObject;
		if (aPet->mOtherTypePetType >= PET_STINKY && aPet->mOtherTypePetType < PET_END)
			mPetsInTank[aPet->mOtherTypePetType]++;
		mOtherTypePetList->push_back(aPet);
		break;
	}
	case TYPE_FISH_TYPE_PET:
	{
		FishTypePet* aPet = (FishTypePet*)theObject;
		if (aPet->mFishTypePetType >= PET_STINKY && aPet->mFishTypePetType < PET_END)
			mPetsInTank[aPet->mFishTypePetType]++;
		mFishTypePetList->push_back(aPet);
		break;
	}
	case TYPE_ALIEN:
	{
		Alien* anAlien = (Alien*)theObject;
		if (anAlien->mAlienType == ALIEN_MINI_SYLV)
			mSmallAlienList->push_back(anAlien);
		else
			mAlienList->push_back(anAlien);
		break;
	}
	case TYPE_BILATERUS:
		mBilaterusList->push_back((Bilaterus*) theObject);
		break;
	case TYPE_COIN:
	{
		Coin* aCoin = (Coin*)theObject;
		if (aCoin->m0x1a0 > 1 || aCoin->mCoinType == COIN_NIKOPEARL)
			mNikoPearlCoinList->push_back(aCoin);
		else if (aCoin->mCoinType == COIN_NOTE)
			mNoteList->push_back(aCoin);
		else
			mCoinList->push_back(aCoin);
		break;
	}
	case TYPE_DEAD_ALIEN:
		mDeadAlienList->push_back((DeadAlien*)theObject);
		break;
	case TYPE_DEAD_FISH:
		mDeadFishList->push_back((DeadFish*)theObject);
		break;
	case TYPE_FOOD:
		mFoodList->push_back((Food*)theObject);
		break;
	case TYPE_LARVA:
		mLarvaList->push_back((Larva*)theObject);
		break;
	case TYPE_MISSLE:
	{
		Missle* aMis = (Missle*)theObject;
		if (aMis->IsTargetless())
			mMissleList2->push_back((Missle*)theObject);
		else
			mMissleList1->push_back((Missle*)theObject);
		someFlag = false;
		break;
	}
	case TYPE_SHADOW:
		mShadowList->push_back((Shadow*)theObject);
		someFlag = false;
		break;
	case TYPE_SHOT:
		mShotList->push_back((Shot*)theObject);
		someFlag = false;
		break;
	case TYPE_WARP:
		mWarpList->push_back((Warp*)theObject);
		someFlag = false;
		break;
	case TYPE_SYLVESTER_FISH:
		mSpecialFishList->push_back((SylvesterFish*)theObject);
		break;
	case TYPE_BALL_FISH:
		mSpecialFishList->push_back((BallFish*)theObject);
		break;
	case TYPE_BI_FISH:
		mSpecialFishList->push_back((BiFish*)theObject);
		break;
	}

	mGameObjectSet.insert(theObject);
	if (someFlag)
		m0x2a7 = true;
}

void Board::MakeShadowForGameObject(GameObject* theObject)
{
	if (theObject->mShadowPtr == nullptr)
	{
		switch (theObject->mType)
		{
		case TYPE_GUPPY:
		case TYPE_OSCAR:
		case TYPE_GEKKO:
		case TYPE_BREEDER:
		case TYPE_DEAD_FISH:
		case TYPE_BALL_FISH:
		case TYPE_BI_FISH:
			SpawnShadow(0, theObject);
			break;
		case TYPE_ULTRA:
			SpawnShadow(2, theObject);
			break;
		case TYPE_PENTA:
		case TYPE_GRUBBER:
			SpawnShadow(1, theObject);
			break;
		case TYPE_OTHER_TYPE_PET:
		{
			OtherTypePet* aPet = (OtherTypePet*)theObject;
			if(aPet->mOtherTypePetType == PET_CLYDE)
				SpawnShadow(0, theObject);
			else if(aPet->mOtherTypePetType != PET_NIKO)
				SpawnShadow(1, theObject);
			break;
		}
		case TYPE_FISH_TYPE_PET:
		{
			FishTypePet* aPet = (FishTypePet*)theObject;
			if(aPet->mFishTypePetType == PET_AMP)
				SpawnShadow(2, theObject);
			else
				SpawnShadow(aPet->mFishTypePetType == PET_BRINKLEY, theObject);
			break;
		}
		case TYPE_ALIEN:
		{
			Alien* anAlien = (Alien*)theObject;
			SpawnShadow(anAlien->mAlienType == ALIEN_MINI_SYLV ? 0 : 2, theObject);
			break;
		}
		case TYPE_BILATERUS:
		case TYPE_COIN:
		case TYPE_DEAD_ALIEN:
		case TYPE_FOOD:
		case TYPE_LARVA:
		case TYPE_MISSLE:
		case TYPE_SHADOW:
		case TYPE_SHOT:
		case TYPE_WARP:
			break;
		case TYPE_SYLVESTER_FISH:
		{
			SylvesterFish* aFish = (SylvesterFish*)theObject;
			SpawnShadow(aFish->mSize == 1 ? 0 : 2, theObject);
			break;
		}
		default:
			SpawnShadow(0, theObject);
		}
	}
}

void Board::SortGameObjects()
{
	if (mWidgetManager)
	{
		mWidgetManager->MarkAllDirty();
		mWidgetManager->BringToFront(mBoardOverlay1);
	}

	// The visibility loops read the iterator again for each access, as the original does
	for (std::vector<Shadow*>::iterator it = mShadowList->begin(); it != mShadowList->end(); ++it) // 21
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<DeadFish*>::iterator it = mDeadFishList->begin(); it != mDeadFishList->end(); ++it) // 60
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<DeadAlien*>::iterator it = mDeadAlienList->begin(); it != mDeadAlienList->end(); ++it) // 99
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Coin*>::iterator it = mNoteList->begin(); it != mNoteList->end(); ++it) // 138
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Warp*>::iterator it = mWarpList->begin(); it != mWarpList->end(); ++it) // 177
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Penta*>::iterator it = mPentaList->begin(); it != mPentaList->end(); ++it) // 216
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Grubber*>::iterator it = mGrubberList->begin(); it != mGrubberList->end(); ++it) // 255
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Food*>::iterator it = mFoodList->begin(); it != mFoodList->end(); ++it) // 294
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it) // 333
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Oscar*>::iterator it = mOscarList->begin(); it != mOscarList->end(); ++it) // 372
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Larva*>::iterator it = mLarvaList->begin(); it != mLarvaList->end(); ++it) // 411
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Gekko*>::iterator it = mGekkoList->begin(); it != mGekkoList->end(); ++it) // 455
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Ultra*>::iterator it = mUltraList->begin(); it != mUltraList->end(); ++it) // 489
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Breeder*>::iterator it = mBreederList->begin(); it != mBreederList->end(); ++it) // 528
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Fish*>::iterator it = mSpecialFishList->begin(); it != mSpecialFishList->end(); ++it) // 567
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it) // 606
	{
		Fish* anObj = *it;
		if(anObj->mSize == 4)
			mWidgetManager->BringToFront(anObj);
	}

	if (!mCyraxPtr) // 627
	{
		for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end(); ++it) // 633
		{
			if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
				mWidgetManager->BringToFront(*it);
		}
		for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end(); ++it) // 672
		{
			if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
				mWidgetManager->BringToFront(*it);
		}
	}

	for (std::vector<OtherTypePet*>::iterator it = mOtherTypePetList->begin(); it != mOtherTypePetList->end(); ++it) // 712
	{
		if ((*it)->mOtherTypePetType == 1 && !(*it)->mIsPresto)
			mWidgetManager->PutBehind(*it, mBoardOverlay1);
		else
			mWidgetManager->BringToFront(*it);
		(*it)->MarkDirty();
	}

	for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it) // 759
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end(); ++it) // 798
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Missle*>::iterator it = mMissleList2->begin(); it != mMissleList2->end(); ++it) // 837
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	if (mCyraxPtr) //871
	{
		mWidgetManager->BringToFront(mCyraxPtr);
		mCyraxPtr->MarkDirty();

		for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end(); ++it) // 879
		{
			if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
				mWidgetManager->BringToFront(*it);
		}

		for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end(); ++it) // 918
		{
			if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
				mWidgetManager->BringToFront(*it);
		}
	}


	for (std::vector<Alien*>::iterator it = mSmallAlienList->begin(); it != mSmallAlienList->end(); ++it) // 958
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Shot*>::iterator it = mShotList->begin(); it != mShotList->end(); ++it) // 997
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	for (std::vector<Coin*>::iterator it = mCoinList->begin(); it != mCoinList->end(); ++it) // 1036
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	if (mLastCoinTypeClicked != -1)
		for (std::vector<Coin*>::iterator it = mCoinList->begin(); it != mCoinList->end(); ++it) // 1076
		{
			Coin* anObj = *it;
			if (anObj->mCoinType == mLastCoinTypeClicked)
				mWidgetManager->BringToFront(anObj);
		}

	for (std::vector<Coin*>::iterator it = mNikoPearlCoinList->begin(); it != mNikoPearlCoinList->end(); ++it) // 1103
	{
		if (!(*it)->mInvisible || (*it)->mInvisibleTimer != 0)
			mWidgetManager->BringToFront(*it);
	}

	if(mMessageWidget)
		mWidgetManager->BringToFront(mMessageWidget);
	mWidgetManager->BringToFront(mBoardOverlay2);

	// end() is recomputed from mApp on every iteration in the original
	for (DialogList::iterator it = mApp->mDialogList.begin(); it != mApp->mDialogList.end(); ++it)
	{
		Dialog* aDia = *it;
		mWidgetManager->BringToFront(aDia);
		aDia->MarkDirty();
	}
}

void Sexy::Board::PauseGame(bool shouldPause)
{
	if (mPause != shouldPause)
	{
		if (shouldPause)
		{
			if (!mApp->IsScreenSaver())
				mPause = true;
		}
		else
		{
			m0x4ec = false;
			m0x4ed = false;
			UpdateMoneyLabelText();
			WidgetSetupVT();
			Unk13();
			DetermineAlienSpawnCoordsVT();
			UpdateNikoPosition();
			if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
				UpdateHasVirtualTankFish();
			mFishSongMgr->PausedGameDelaySong();
			mPause = false;
		}
	}
}

// A separate function in the original (0x53DC10): after the game is beaten, a finished level is replayed as its bonus level
void Sexy::Board::StartBonusLevel()
{
	mShouldSave = true;
	ResetLevel(mWidgetManager);
	mApp->StopMusic();
	mApp->PlayMusic(2, 54);
	InitBonusLevel();
}

void Sexy::Board::StartGame()
{
	mPause = false;
	PlaySample(SOUND_BUBBLES_ID, 3, 1.0);
	for (int i = (mApp->mSeed->Next() & 3) + 2; i >= 0; i--)
		SpawnRandomBubble();

	// the loops read mApp->mCurrentProfile again for every pet (0x549930, 0x549960)
	if ((mApp->mCurrentProfile->mFinishedGame && (mApp->mGameMode == GAMEMODE_TIME_TRIAL ||
		mApp->mGameMode == GAMEMODE_CHALLENGE)) || mApp->mCurrentProfile->mNumOfUnlockedPets > 3)
	{
		for (int i = PET_STINKY; i < PET_END; i++)
			if (mApp->mCurrentProfile->m0x5a[i])
				SpawnPet(i, -1, -1, false, false);
	}
	else
	{
		for (int i = PET_STINKY; i < PET_PRESTO; i++)
			if (mApp->mCurrentProfile->IsPetUnlocked(i))
				SpawnPet(i, -1, -1, false, false);
	}

	if (mApp->mRelaxMode)
	{
		for (int i = 0; i < 2; i++)
		{
			int aX = mApp->mSeed->Next() % 520 + 20;
			int aY = mApp->mSeed->Next() % 265 + 105;
			SpawnGuppy(aX, aY);
		}
	}
	else if (mTank == 4)
	{
		// The original draws x before y
		int aX = mApp->mSeed->Next() % 520 + 20;
		int aY = mApp->mSeed->Next() % 265 + 105;
		SpawnBreeder(aX, aY);
		mBreederList->front()->m0x1a4 = 2;
	}
	else if(mTank == 5)
	{
		mAlienTimer = 300;
		for (int i = PET_STINKY; i < PET_GASH; i++)
			SpawnPet(i, -1, -1, false, false);

		SpawnPet(PET_GASH, -1, -1, false, false);
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			int aX = mApp->mSeed->Next() % 520 + 20;
			int aY = mApp->mSeed->Next() % 265 + 105;
			SpawnGuppy(aX, aY);
		}

		// the original walks the first two fish with one iterator (0x549AFD)
		std::vector<Fish*>::iterator anItr = mFishList->begin();
		(*anItr)->mFoodAte = 2;
		if (mLevel == 1 && mTank == 1)
			(*anItr)->mBeginner = true;
		++anItr;
		(*anItr)->mFoodAte = 2;
		if (mLevel == 1 && mTank == 1)
			(*anItr)->mBeginner = true;
	}

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		if (mTank == 1)
			m0x3bc = 300;
		else if(mTank == 2 ||mTank == 3 ||mTank == 4)
			m0x3bc = 600;
	}
	bool aGameFin = mApp->mCurrentProfile->mFinishedGame;
	// a flat chain in the original: the adventure mode is tested again for each level (0x549C48, 0x549C78)
	if (mApp->mGameMode == GAMEMODE_ADVENTURE && mLevel == 1 && mTank == 1)
	{
		if (!aGameFin)
			ShowText("Welcome to the Insaniquarium!", false, 0);
		else if (mApp->mCurrentProfile->mCyraxNum == 1) // Only the first time the adventure is replayed
			ShowText("Welcome to the Bonus Adventure!", false, 0);
	}
	else if (mApp->mGameMode == GAMEMODE_ADVENTURE && mLevel == 2 && mTank == 1 && !aGameFin)
		ShowText("Welcome to the next level!", false, 16);
	else if (mApp->mGameMode == GAMEMODE_ADVENTURE && mLevel == 1 && mTank == 2 && !aGameFin)
		ShowText("Welcome to the second tank! Now things get tricky!", false, 19);
	else if (mApp->mGameMode == GAMEMODE_CHALLENGE)
		ShowText("Welcome to Challenge mode!", false, 50);
	else if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
		ShowText(StrFormat("Welcome to Time Trial mode!", m0x3bc / 60), false, 52); // the original passes the minutes, unused
	else if (mApp->mGameMode == GAMEMODE_SANDBOX)
		ShowText("You\'ve discovered the top-secret SANDBOX mode!", false, 47);

	mGameUpdateCnt = 0;
	m0x450 = 0;
	m0x454 = 0;
	m0x45c = 0;
	m0x458 = 0;
	int aTime = Unk01();
	m0x4e8 = 0; // stored after Unk01 in the original (0x549D4D)
	m0x3b4 = aTime;
	m0x3b8 = aTime;
	m0x4e6 = false;
}

void Sexy::Board::SaveCurrentGame()
{
	SexyString aFileStr = UserProfile::GetSaveGameFilePath(mApp->mGameMode, mApp->mCurrentProfile->mId);

	if (NeedSaveGame())
	{
		MkDir(GetAppDataFolder() + "userdata"); // operator+ in the original (0x42D420), not append
		SaveGame(aFileStr);
	}
	else
		mApp->EraseFile(aFileStr);
}

// A separate function in the original (0x537BF0); also used by the options dialog
bool Sexy::Board::NeedSaveGame()
{
	return mShouldSave && mApp->mGameMode != GAMEMODE_SANDBOX;
}

void Sexy::Board::SaveGame(const SexyString& theSavePath)
{
	DataWriter aDW;
	aDW.OpenMemory(32);
	DataSync aDS(aDW);
	aDS.mVersion = 54; // stored before the version is written in the original (0x5469E6)
	aDW.WriteLong(54); // Game Save File Version
	SyncGameData(aDS);
	aDW.WriteLong(_time64(NULL));
	aDS.SyncPointers();
	mApp->WriteBytesToFile(theSavePath, aDW.mMemoryHandle, aDW.mMemoryPosition);
	mApp->ClearUpdateBacklog();
	if (mApp->IsScreenSaver())
	{
		mApp->mCurrentProfile->SaveScreenSaver(m0x400);
		m0x400 = 0;
	}
}

// A separate function in the original (0x537B00; LTCG passes theSync in ebx): creates the object whose
// type is next in the save and syncs it
static GameObject* ReadGameObject(DataSync& theSync)
{
	DataReader* aReader = theSync.mReader;
	int aType = aReader->ReadLong();
	aReader->RollbackBytes(4);
	GameObject* anObj = Board::CreateGameObject(aType);
	if (anObj == nullptr)
		throw DataReaderException(); // Usually when aType is -1
	anObj->Sync(&theSync);
	return anObj;
}

// A separate cdecl function in the original (0x5387F0): syncs an optional alien pointer (the Cyrax)
static void SyncAlienPtr(DataSync& theSync, Alien*& theAlien)
{
	// Both read at entry: the reader is used again after the delete
	DataReader* aReader = theSync.mReader;
	DataWriter* aWriter = theSync.mWriter;
	if (aReader)
	{
		if (theAlien)
			delete theAlien;
		if (aReader->ReadBool())
		{
			theAlien = new Alien();
			theAlien->Sync(&theSync);
		}
		else
			theAlien = nullptr;
	}
	else
	{
		aWriter->WriteBool(theAlien != nullptr);
		if (theAlien)
			theAlien->Sync(&theSync);
	}
}

bool Sexy::Board::SyncGameData(DataSync& theSync)
{
	theSync.SyncLong(mTank);
	theSync.SyncLong(mLevel);

	if (mApp->mGameMode == GAMEMODE_ADVENTURE && theSync.mReader != nullptr
		&& (mTank != mApp->mCurrentProfile->mTank || mLevel != mApp->mCurrentProfile->mLevel))
		return false;

	theSync.SyncBool(mIsBonusRound);
	theSync.SyncBool(m0x2a6);

	DataReader* aDR = theSync.mReader;
	DataWriter* aDW = theSync.mWriter;

	int aBGId = -1;

	if (mApp->IsScreenSaver() && aDW && m0x3ec != -1)
	{
		aBGId = mCurrentBackgroundId;
		mCurrentBackgroundId = m0x3ec;
		UpdateNikoPosition();
	}

	// the original tests the reader here (0x546305), not the writer
	if (aDR)
	{
		ResetLevel(mWidgetManager);
		// Signed as the original: a count of 0x80000000 or more loads no objects
		int aNumOfObjects = (int)aDR->ReadLong();
		for (int i = 0; i < aNumOfObjects; i++)
		{
			GameObject* anObj = ReadGameObject(theSync);

			if (!anObj->mShown)
			{
				mGameObjectSet.insert(anObj);
			}
			else
			{
				AddGameObject(anObj, false);
				mWidgetManager->AddWidget(anObj);
				MakeShadowForGameObject(anObj);
			}
		}
	}
	else
	{
		ulong aNumOfObjects = 0;
		ulong aPos = aDW->mMemoryPosition;
		aDW->WriteLong(0);

		for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
		{
			GameObject* anObj = *it;
			if (anObj->mType != TYPE_SHADOW)
			{
				aNumOfObjects++;
				anObj->Sync(&theSync);
			}
		}

		aDW->SetLong(aNumOfObjects, aPos);
	}

	SyncAlienPtr(theSync, mCyraxPtr);

	if (mCyraxPtr)
		mWidgetManager->AddWidget(mCyraxPtr);

	mMessageWidget->Sync(theSync);

	for (int i = 0; i < 24; i++)
		theSync.SyncLong(mPetsInTank[i]);

	theSync.SyncBool(mBonusRoundStarted);
	theSync.SyncLong(m0x2a8);
	theSync.SyncLong(mAlienExpect);
	theSync.SyncLong(mAlienTimer);
	theSync.SyncLong(m0x2c4);
	theSync.SyncLong(mDropFoodDelay);
	theSync.SyncLong(mCrosshair1X);
	theSync.SyncLong(mCrosshair1Y);
	theSync.SyncLong(mCrosshair2X);
	theSync.SyncLong(mCrosshair2Y);
	theSync.SyncLong(m0x2dc);
	theSync.SyncLong(m0x3b0);
	theSync.SyncLong(m0x3b4);
	theSync.SyncLong(m0x3b8);
	theSync.SyncLong(m0x3bc);
	theSync.SyncLong(m0x3c0);
	theSync.SyncLong(m0x3c4);
	theSync.SyncLong(m0x3e4);
	theSync.SyncLong(mCurrentBackgroundId);
	theSync.SyncLong(mMoney);
	theSync.SyncLong(m0x3f4);
	theSync.SyncLong(m0x3f8);
	theSync.SyncLong(m0x3fc);
	theSync.SyncLong(mLastCoinTypeClicked);
	theSync.SyncLong(mCoinComboCount);
	theSync.SyncLong(m0x43c);
	theSync.SyncBool(m0x440);
	theSync.SyncLong(mGameUpdateCnt);
	theSync.SyncLong(mBubbulatorTimer);
	theSync.SyncLong(m0x44c);
	theSync.SyncLong(m0x450);
	theSync.SyncLong(m0x454);
	theSync.SyncLong(m0x458);
	theSync.SyncLong(m0x45c);
	theSync.SyncLong(m0x460);
	theSync.SyncLong(m0x464);
	theSync.SyncLong(m0x468);
	theSync.SyncLong(mMaxOscarCountEver);
	theSync.SyncLong(mMaxUltraCountEver);
	theSync.SyncLong(m0x474);
	theSync.SyncLong(m0x478);
	theSync.SyncLong(m0x47c);
	theSync.SyncLong(m0x480);
	theSync.SyncLong(m0x484);
	theSync.SyncLong(mGuppiesDeadCount);
	theSync.SyncLong(mMaxFishCountEver);
	theSync.SyncLong(m0x490);
	theSync.SyncLong(mMaxPentaCountEver);
	theSync.SyncLong(m0x498);
	theSync.SyncLong(m0x49c);
	theSync.SyncLong(m0x4a0);
	theSync.SyncLong(m0x4a4);
	theSync.SyncLong(m0x4a8);
	theSync.SyncLong(m0x4ac);
	theSync.SyncBool(m0x4fc);
	theSync.SyncBool(mBubbulatorShown);
	theSync.SyncBool(mAlienAttractorShown);
	theSync.SyncBool(mAlwaysShowWhenHungry);
	theSync.SyncBool(m0x500);
	for (int i = 0; i < 54; i++) // a signed int bound in the original (0x5467C2)
		theSync.SyncBool(mMessageShown[i]);
	theSync.SyncBool(m0x4e6);
	theSync.SyncLong(m0x4e8);
	theSync.SyncLong(m0x4f0);
	theSync.SyncBool(m0x4f4);
	theSync.SyncLong(gWadsworthTimer);
	theSync.SyncLong(gWadsworthX);
	theSync.SyncLong(gWadsworthY);
	theSync.SyncLong(gUnkInt02);
	theSync.SyncLong(gUnkInt05);
	theSync.SyncLong(gUnkInt06);
	theSync.SyncBool(gMerylActive);
	theSync.SyncLong(gFoodType);
	theSync.SyncLong(gFoodLimit);
	SyncColor(&theSync, gUnkColor01);
	// from here the original reads theSync.mReader again (0x54687B, 0x5468CB), not aDR
	if (theSync.mReader)
		m0x2e0 = 0;

	for (int i = 0; i < SLOT_END; i++)
	{
		theSync.SyncLong(mSlotNumber[i]);
		theSync.SyncLong(mSlotPrices[i]);
		theSync.SyncLong(m0x344[i]);
		theSync.SyncLong(m0x374[i]);
		theSync.SyncBool(mSlotUnlocked[i]);

		if (theSync.mReader && mSlotUnlocked[i])
			MakeAndUnlockMenuButton(i, false);
	}

	if (aBGId != -1)
	{
		mCurrentBackgroundId = aBGId;
		UpdateNikoPosition();
	}

	if (gWadsworthTimer < 0 || (gWadsworthTimer != 0 && mAlienList->size() == 0 && mBilaterusList->size() == 0))
		gWadsworthTimer = 0;
	if (gMerylActive && mPetsInTank[8] == 0)
		gMerylActive = false;
	Unk03();
	return true;
}

GameObject* Sexy::Board::CreateGameObject(int theType)
{
	switch (theType)
	{
	case TYPE_GUPPY:
		return new Fish();
		break;
	case TYPE_OSCAR:
		return new Oscar();
		break;
	case TYPE_ULTRA:
		return new Ultra();
		break;
	case TYPE_GEKKO:
		return new Gekko();
		break;
	case TYPE_PENTA:
		return new Penta();
		break;
	case TYPE_GRUBBER:
		return new Grubber();
		break;
	case TYPE_BREEDER:
		return new Breeder();
		break;
	case TYPE_BILATERUS:
		return new Bilaterus();
		break;
	case TYPE_DEAD_ALIEN:
		return new DeadAlien();
		break;
	case TYPE_DEAD_FISH:
		return new DeadFish();
		break;
	case TYPE_FOOD:
		return new Food();
		break;
	case TYPE_LARVA:
		return new Larva();
		break;
	case TYPE_MISSLE:
		return new Missle();
		break;
	case TYPE_SHADOW:
		return new Shadow();
		break;
	case TYPE_SHOT:
		return new Shot();
		break;
	case TYPE_WARP:
		return new Warp();
		break;
	case TYPE_OTHER_TYPE_PET:
		return new OtherTypePet();
		break;
	case TYPE_FISH_TYPE_PET:
		return new FishTypePet();
		break;
	case TYPE_ALIEN:
		return new Alien();
		break;
	case TYPE_COIN:
		return new Coin();
		break;
	case TYPE_SYLVESTER_FISH:
		return new SylvesterFish();
		break;
	case TYPE_BALL_FISH:
		return new BallFish();
		break;
	case TYPE_BI_FISH:
		return new BiFish();
		break;
	}
	return nullptr;
}

bool Sexy::Board::LoadGame(const SexyString& theSavePath)
{
	if (!mApp->IsScreenSaver())
		mApp->mCurrentProfile->Unk01();

	Buffer aBuf;
	if (!mApp->ReadBufferFromFile(theSavePath, &aBuf, false))
		return false;

	DataReader aDR;
	aDR.OpenMemory(aBuf.GetDataPtr(), aBuf.GetDataLen(), false);
	DataSync aSync(aDR);

	ulong aTimeOfSave;
	try
	{
		int aVer = aDR.ReadLong();
		if (aVer < 53)
			return false;
		aSync.mVersion = aVer;
		if (!SyncGameData(aSync))
			return false;
		aTimeOfSave = aDR.ReadLong();
		aSync.SyncPointers();
	}
	catch (DataReaderException&)
	{
		// As the original: a truncated or damaged save resets the level and does not load
		ResetLevel(mWidgetManager);
		return false;
	}

	if(mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		// The saved time is unsigned; a clock set back also clears the objects that don't belong to the tank
		__time64_t aCurrentTime = _time64(nullptr);
		__time64_t aSaveTime = aTimeOfSave;
		if (aCurrentTime < aSaveTime || aCurrentTime - aSaveTime > 120)
			Unk15();
	}
	StartMusic();
	mWidgetManager->AddWidget(mMenuButton);
	mWidgetManager->AddWidget(mMoneyLabel);
	mWidgetManager->AddWidget(mMessageWidget);
	UpdateMoneyLabelText();
	ApplyShadowsIf3D();
	mApp->ClearUpdateBacklog();
	mShouldSave = true;
	SortGameObjects();
	if (mApp->IsScreenSaver())
	{
		// The original multiplies by 1/172.8 as a double constant, stores the result and then clamps the global
		int aTimeVal = mGameUpdateCnt / 36;
		gUnkInt07 = (int)(aTimeVal * 0.005787037037037037 + 5000.0);
		if (gUnkInt07 > 50000)
			gUnkInt07 = 50000;
		else if (gUnkInt07 < 5000)
			gUnkInt07 = 5000;
	}
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		MakeVirtualTankButtons();
		WidgetSetupVT();
		Unk13();
		Unk14(m0x2a6);
		DoVirtualTankDialog();
	}
	return true;
}

void Sexy::Board::InitLevel()
{
	mGameUpdateCnt = 0;

	for (int i = 0; i < SLOT_END; i++)
	{
		mSlotNumber[i] = -1;
		mSlotPrices[i] = 0;
		m0x374[i] = -1;
		mSlotUnlocked[i] = false;
	}

	if (mApp->mGameMode == GAMEMODE_ADVENTURE)
	{
		mTank = mApp->mCurrentProfile->mTank;
		mLevel = mApp->mCurrentProfile->mLevel;
	}
	else
	{
		mTank = mApp->mTankGameModeChoose;
		mLevel = 5;
	}

	if (mApp->mGameMode == GAMEMODE_SANDBOX)
		mMoney = 999999;
	else
		mMoney = 200;

	m0x4ac = 5;
	mAlienTimer = 3000;
	mBonusRoundStarted = false;
	mDropFoodDelay = 0;
	m0x2a8 = 0;
	m0x43c = 1;
	m0x3e4 = 2;
	mAlienExpect = ALIEN_STRONG_SYLV;
	gFoodType = 0;
	gFoodLimit = 1;
	gMerylActive = false;
	gWadsworthTimer = 0;
	gUnkInt02 = 0;
	m0x440 = false;
	if (mApp->mRelaxMode)
	{
		gFoodType = 0;
		gFoodLimit = 3;
		m0x3e4 = 8;
		mMoney = 200;
		gUnkInt03 = 0;
	}
	m0x460 = 0;
	m0x464 = 0;
	m0x468 = 0;
	mMaxOscarCountEver = 0;
	mMaxUltraCountEver = 0;
	m0x474 = 0;
	m0x478 = 0;
	m0x47c = 0;
	m0x480 = 0;
	m0x484 = 0;
	mGuppiesDeadCount = 0;
	mMaxFishCountEver = 0;
	m0x490 = 0;
	mMaxPentaCountEver = 0;
	m0x498 = 0;
	m0x49c = 0;
	m0x4a0 = 0;
	m0x4a4 = 0;
	m0x4a8 = 0;
	mWidgetManager->AddWidget(mMenuButton);
	mWidgetManager->AddWidget(mMoneyLabel);
	mWidgetManager->AddWidget(mMessageWidget);
	UpdateMoneyLabelText();
	memset(mMessageShown, 0, 54);
	DeterminePricesAndSlots();
	m0x2dc = 0;
	m0x2e0 = 0;
	for (int i = 0; i < SLOT_END; i++)
	{
		if (mSlotNumber[i] >= 0)
			m0x2dc++;
		if (m0x374[i] == -1)
			m0x374[i] = mSlotPrices[i] / 100;
		m0x344[i] = mSlotPrices[i];
	}

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		MakeVirtualTankButtons();
	mWidgetManager->MarkAllDirty();
}

void Sexy::Board::InitBonusLevel()
{
	mMessageWidget->mMessageTimer = 0;
	m0x2a8 = mGameUpdateCnt;
	mAlienExpect = ALIEN_NONE;
	mPause = false;
	mIsBonusRound = true;
	mBonusRoundStarted = false;
	gFoodType = 0;
	gFoodLimit = 1;
	gMerylActive = false;
	gWadsworthTimer = 0;
	gUnkInt02 = 0;
	m0x440 = false;
	m0x450 = 0;
	m0x454 = 0;
	m0x45c = 0;
	m0x458 = 0;
	switch (mTank)
	{
	case 2:
		m0x3bc = 15;
		break;
	case 3:
		m0x3bc = 20;
		break;
	case 4:
		m0x3bc = 25;
		break;
	case 1: // the original's jump table covers 1..4; tank 1 shares the default body
	default:
		m0x3bc = 10;
		break;
	}

	int aLvl = mLevel;
	if (aLvl > 6)
		aLvl = 6;
	else if (aLvl < 1)
		aLvl = 1;

	m0x3bc += aLvl - 1;
	m0x3f4 = 0;
	m0x3f8 = 0;
	mLastCoinTypeClicked = -1;
	mCoinComboCount = 0;
	int aVal = Unk01();
	m0x3b4 = aVal;
	m0x3b8 = aVal;
}

void Sexy::Board::StartMusic()
{
	if (mAlienTimer != 1 && mAlienList->empty() && mBilaterusList->empty() && !mCyraxPtr)
	{
		if (mAlienTimer > 1 && mAlienTimer <= 275)
		{
			mApp->StopMusic();
			if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK || mAlienAttractorShown)
				mApp->SomeMusicFunc(false);
		}
		else if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			mApp->StopMusic();
		else
			mApp->StartGameMusic();
	}
	else
	{
		mApp->StopMusic();
		mApp->PlayMusic(1, 3);
	}
}

int Sexy::Board::Unk01()
{
	return mApp->mFrameTime * mGameUpdateCnt;
}

int Sexy::Board::Unk02()
{
	return (Unk01() - m0x3b8) / 1000;
}

void Sexy::Board::Unk03()
{
	switch (mCurrentBackgroundId)
	{
	case 1:
		gUnkColor01 = Color(66, 48, 41);
		break;
	case 2:
		gUnkColor01 = Color(154, 77, 111);
		break;
	case 3:
		gUnkColor01 = Color(47, 39, 124);
		break;
	case 4:
		gUnkColor01 = Color(15, 73, 58);
		break;
	case 5:
		gUnkColor01 = Color(110, 83, 72);
		break;
	case 6:
		gUnkColor01 = Color(50, 0, 59);
		break;
	default:
		break;
	}

	if ((mApp->mCurrentProfile->mCheatCodeFlags >> 2 & 1))
		gUnkColor01 = Color(0x333333);
}

void Sexy::Board::Unk04()
{
	int something = Unk10();
	mMoney += something;
	if (mMoney < 0)
		mMoney = 0;
	else if (mMoney > 9999999)
		mMoney = 9999999;
	UpdateMoneyLabelText();
}

void Sexy::Board::Unk05()
{
	if (mLevel != 6)
		mApp->StartGameMusic();
	mBonusRoundStarted = true;
	int aVal = Unk01();
	m0x3b4 = aVal;
	m0x3b8 = aVal;
	m0x3b0 = 0;
	m0x2a8 = mGameUpdateCnt;
}

void Sexy::Board::Unk06(Graphics* g, Image* theImage, int theX, int theY, float theVal)
{
	int aWidth = theImage->mWidth; // read once, before the loop
	int aStartY = -81 - theY;
	int aCounter = 0;
	for (int i = 0; i < 180; i += 9)
	{
		// Unsigned as the original, so only strips 4 to 18 are drawn
		if ((unsigned int)(aStartY + theY) <= 358)
		{
			// Each branch builds its source rect before the angle and sine (the original evaluates the
			// Draw call's arguments right to left)
			if (mApp->Is3DAccelerated())
			{
				Rect aSrcRect(0, aCounter, aWidth, 24);
				// Angle and sine are stored as floats; the original uses 3.14159f and the CRT double sin
				float aRad = (double)((mGameUpdateCnt + i) * 5) * 3.14159f / 180.0;
				float aSinVal = (float)sin((double)aRad);
				aSinVal = aSinVal * theVal;
				aSinVal = (float)((double)aSinVal + theY); // the y is rounded to a float here
				g->DrawImageF(theImage, (float)theX, aSinVal, aSrcRect);
			}
			else
			{
				Rect aSrcRect(0, aCounter, aWidth, 24);
				float aRad = (double)((mGameUpdateCnt + i) * 5) * 3.14159f / 180.0;
				float aSinVal = (float)sin((double)aRad);
				aSinVal = aSinVal * theVal;
				g->DrawImage(theImage, theX, (int)((double)aSinVal + theY), aSrcRect);
			}
		}
		theY += 24;
		aCounter += 24;
	}
}

void Sexy::Board::Unk07(int unk)
{
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (!mApp->IsScreenSaver())
			mApp->mCurrentProfile->AddShells(unk);
		m0x400 += unk;
		UpdateMoneyLabelText();
		return;
	}
	if (mIsBonusRound)
	{
		m0x3f4 += unk;
		UpdateMoneyLabelText();
		return;
	}
	mMoney += unk;
	m0x460 += unk;
	if (mMoney > 9999999)
		mMoney = 9999999;
	UpdateMoneyLabelText();
}

bool Sexy::Board::Unk08(int x, int y)
{
	if (gUnkBool03)
		return false;

	gUnkBool03 = true;
	GameObject* aPetGotIt = nullptr;

	for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it)
	{
		FishTypePet* aPet = *it;
		if (aPet->m0x230 && aPet->Contains(x, y))
		{
			aPetGotIt = aPet;
			break;
		}
	}

	for (std::vector<OtherTypePet*>::iterator it = mOtherTypePetList->begin(); it != mOtherTypePetList->end(); ++it)
	{
		OtherTypePet* aPet = *it;
		if (aPet->mIsPresto && aPet->Contains(x, y))
		{
			aPetGotIt = aPet;
			break;
		}
	}

	if(aPetGotIt)
		aPetGotIt->MouseDown(x - aPetGotIt->mX, y - aPetGotIt->mY, -1);

	gUnkBool03 = false;
	return aPetGotIt != nullptr;
}

bool Sexy::Board::Unk09(Coin* theCoin)
{
	if (!mIsBonusRound)
		return false;

	if (theCoin->mCoinType != mLastCoinTypeClicked)
	{
		mLastCoinTypeClicked = theCoin->mCoinType;
		mCoinComboCount = 0;
	}

	int aSound = 0;
	int aUnk01 = 0;
	int aValue = 0;
	switch (theCoin->mCoinType) // a jump table on mCoinType - 1 in the original
	{
	case COIN_SILVER_C:
		aSound = SOUND_POINTS;
		aUnk01 = 0;
		aValue = 1;
		break;
	case COIN_GOLD_C:
		aSound = SOUND_POINTS;
		aUnk01 = 1;
		aValue = 2;
		break;
	case COIN_DIAMOND:
		aSound = SOUND_DIAMOND;
		aUnk01 = 2;
		aValue = 5;
		break;
	case COIN_PEARL:
		aSound = SOUND_PEARL;
		aUnk01 = 3;
		aValue = 10;
		break;
	case COIN_TREASURE:
		aSound = SOUND_BONUSCOLLECT;
		aUnk01 = 4;
		aValue = 20;
		break;
	default:
		return false;
	}

	mCoinComboCount++;
	// the max-chain branch comes first in the original (its unwind states precede the other branch's)
	if (mCoinComboCount >= 10)
	{
		theCoin->mComboCount = 25;
		MakeNote(theCoin->mX, theCoin->mY, aUnk01 + 3, StrFormat("+%d (MAX CHAIN!!)", 25 * aValue));

		FishSong aFishSong;
		aFishSong.mTone = aSound;
		for (int i = 0; i < 5; i++)
		{
			NoteData aNote;
			aNote.mPitch = 9;
			aNote.mVolume = 1.0;
			aNote.mDuration = 200;
			aFishSong.mNoteDataVector.push_back(aNote);
		}
		mFishSongMgr->AddSong(aFishSong);
		mCoinComboCount = 1;
		mLastCoinTypeClicked = -1;
	}
	else
	{
		theCoin->mComboCount = mCoinComboCount;
		MakeNote(theCoin->mX, theCoin->mY, aUnk01 + 3, StrFormat("+%d", mCoinComboCount* aValue));
		SoundInstance* aSndInst = mApp->mSoundManager->GetSoundInstance(aSound);

		if (aSndInst)
		{
			aSndInst->SetVolume(1.0);
			aSndInst->AdjustPitch(mCoinComboCount - 1);
			aSndInst->Play(false, true);
		}
	}
	return true;
}

int Sexy::Board::Unk10()
{
	int aValue = 0;
	for (std::vector<Coin*>::iterator it = mCoinList->begin(); it != mCoinList->end(); ++it)
	{
		Coin* aCoin = *it;
		if (aCoin->m0x198)
			aValue += aCoin->GetValue();
	}

	for (std::vector<Coin*>::iterator it = mNikoPearlCoinList->begin(); it != mNikoPearlCoinList->end(); ++it)
	{
		Coin* aCoin = *it;
		if (aCoin->m0x198)
			aValue += aCoin->GetValue();
	}

	for (std::vector<Larva*>::iterator it = mLarvaList->begin(); it != mLarvaList->end(); ++it)
	{
		Larva* aLarva = *it;
		if (aLarva->mPickedUp)
			aValue += aLarva->GetValue();
	}
	return aValue;
}

bool Sexy::Board::Unk11(int x, int y)
{
	for (std::vector<Food*>::iterator it = mFoodList->begin(); it != mFoodList->end(); ++it)
	{
		Food* aFood = *it;
		if (aFood->mExoticFoodType == 2 && !aFood->mPickedUp)
		{
			if (aFood->Contains(x, y))
			{
				aFood->PickUp();
				return true;
			}
		}
	}
	return false;
}

void Sexy::Board::Unk12()
{
	if (mMissleList1->empty() && mAlienList->empty() && mBilaterusList->empty())
	{
		// The original writes these through mApp->mBoard
		mApp->mBoard->m0x4ed = false;
		mApp->mBoard->m0x4ec = false;
		mApp->mBoard->mDropFoodDelay = 36;
		if (mCyraxPtr == nullptr)
			mApp->SomeMusicPlayFunc(true);
		Unk14(true);
	}
}

void Sexy::Board::Unk13()
{
	long long aTodaySecs = GetTodayStartSeconds();
	long long aCurrentTime = time(NULL);

	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		anObj->Unk03(aTodaySecs, aCurrentTime);
	}
}

void Sexy::Board::Unk14(bool unk)
{
	m0x2a6 = unk;
	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it)
	{
		Fish* aFish = *it;
		aFish->mMouseVisible = unk;
	}
	for (std::vector<Breeder*>::iterator it = mBreederList->begin(); it != mBreederList->end(); ++it)
	{
		Breeder* aBreeder = *it;
		aBreeder->mMouseVisible = unk;
	}

	if (mTank != 5 || mLevel != 1)
	{
		for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it)
		{
			FishTypePet* aPet = *it;
			if(aPet->mFishTypePetType == PET_AMP && aPet->mCoinDropTimer >= aPet->mCoinDropT)
				aPet->mMouseVisible = unk;
			else if(aPet->mFishTypePetType == PET_WALTER)
				aPet->mMouseVisible = unk;
		}
	}
}

void Sexy::Board::Unk15()
{
	GameObjectSet aSet;
	
	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mType != TYPE_SHADOW && 
			(anObj->mType != TYPE_COIN || ((Coin*)anObj)->m0x190 == 0) && 
			anObj->mVirtualTankId < 0)
			aSet.insert(anObj);
	}

	for (GameObjectSet::iterator it = aSet.begin(); it != aSet.end(); ++it)
	{
		GameObject* anObj = *it;
		anObj->RemoveHelper02(true);
	}
}

void Sexy::Board::DetermineAlienSpawnCoordsVT()
{
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		return;

	Point aPos = GetAlienAttractorSpawnCoords();
	mCrosshair1X = aPos.mX - 39;
	mCrosshair1Y = aPos.mY - 180;
	if (mAlienExpect == 8)
		mCrosshair1X += 40;

	mCrosshair2X = mApp->mSeed->Next() % 450 + 20;
	mCrosshair2Y = mApp->mSeed->Next() % 195 + 105;
}

int Sexy::Board::GetNextVirtualTankId()
{
	int aMaxObjects = mApp->mCurrentProfile->mBubbulatorBought != 0 ? 20 : 10;

	// Only the first aMaxObjects entries are cleared (the original's `if (n > 0) memset(occupied, 0, n)`)
	bool occupied[20];
	for (int i = 0; i < aMaxObjects; i++)
		occupied[i] = false;

	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mVirtualTankId >= 0 && anObj->mVirtualTankId < aMaxObjects)
			occupied[anObj->mVirtualTankId] = true;
	}

	for (int i = 0; i < aMaxObjects; i++)
		if (!occupied[i])
			return i;
	return -1;
}

bool Sexy::Board::HasAnyFish()
{
	if (mFishList->empty() && mOscarList->empty() && mPentaList->empty() && !UltrasInTank()
		&& !GrubbersInTank() && !GekkosInTank() && !BreedersInTank())
		return false;
	return true;
}

void Sexy::Board::HandleBuyEgg()
{
	MenuButtonWidget* aBtn = GetMenuButtonById(SLOT_EGG);
	if (!aBtn)
		return;

	if (mApp->mGameMode == GAMEMODE_SANDBOX)
	{
		mApp->LeaveGameBoard();
		return;
	}

	if (!Buy(mSlotPrices[SLOT_EGG], true))
		return;

	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		std::vector<int> anAvailablePets;
		for (int i = 0; i < PET_END; i++)
		{
			if (mPetsInTank[i] == 0 && i != PET_PRESTO && mApp->mCurrentProfile->IsPetUnlocked(i))
				anAvailablePets.push_back(i);
		}

		if (anAvailablePets.size() != 0)
		{
			int aRandPetIdx = Rand() % (int)anAvailablePets.size(); // signed division in the original (0x545A55)
			SpawnPet(anAvailablePets[aRandPetIdx], -1, -1, false, false);
			m0x498++;
			UpdateSlotPrice(SLOT_EGG, mSlotPrices[SLOT_EGG]*2);
		}

		if (anAvailablePets.size() <= 1)
			aBtn->SetMaxedOut();

		PlaySample(SOUND_BUY_ID, 3, 1.0);

		return;
	}

	if (mMessageShown[40])
	{
		mMessageShown[40] = false;
		m0x4e6 = true;
		m0x4e8 = 0;
	}
	
	if (IsTankAndLevelNB(1, 1) && mMessageWidget->mMessageTimer <= 0 && m0x43c == 1)
		ShowText("Collect 2 more egg pieces to finish level!", false, 10);

	m0x43c++;

	if (mApp->mGameMode == GAMEMODE_CHALLENGE)
	{
		m0x454 = 500;
		for (int i = 0; i < SLOT_END; i++)
		{
			if (i != SLOT_EGG)
				UpdateSlotPrice(i, m0x344[i]);
		}
		if(m0x43c <= 3)
			ShowText("Prices have been reset!", false, -1);
	}

	MenuButtonSetupNoVT(SLOT_EGG, true);
	PlaySample(SOUND_BUY_ID, 3, 1.0);

	if (m0x43c <= 3)
		return;

	m0x3fc = Unk02();
	mShouldSave = false;
	Unk04();

	if (mApp->mGameMode == GAMEMODE_CHALLENGE)
	{
		if (mTank >= 1 && mTank <= 4)
			mApp->mCurrentProfile->m0x78[mTank-1] = true;
		mApp->mHighScoreMgr->RecordChallengeHighScore(mTank, mApp->mCurrentProfile, m0x3fc);
		mApp->SwitchToBonusScreen();
		return;
	}

	// read before RecordAdventureHighScore writes the profile (0x545C74)
	bool aFinishedGame = mApp->mCurrentProfile->mFinishedGame;
	int aScore = m0x3fc;
	if (mTank == 5)
	{
		int aCnt = 0;
		gPetsDiedOnBossLevel = 0;
		for (int i = 0; i < 18; i++)
		{
			if (mPetsInTank[i] == 0 && i != PET_ANGIE) // always true here, but the original tests it (0x545CA5)
			{
				gDeadPetsIdArray[aCnt] = i;
				aCnt++;
				gPetsDiedOnBossLevel = aCnt;
			}
		}

		aScore = mOtherTypePetList->size() + mFishTypePetList->size();
	}
	mApp->mHighScoreMgr->RecordAdventureHighScore(mTank, mLevel, mApp->mCurrentProfile, aScore);

	if (aFinishedGame)
	{
		if (mTank == 5 && mLevel == 1)
		{
			mApp->mCurrentProfile->NextLevel();
			mApp->SwitchToBonusScreen();
			return;
		}
		StartBonusLevel();
		return;
	}

	mApp->mCurrentProfile->NextLevel();
	int aPetId = -1;
	if (mTank == 4 && mLevel == 5)
		aPetId = 999;
	else if(mTank == 5)
	{
		aPetId = PET_PRESTO;
		mApp->mCurrentProfile->UnlockPet(aPetId, true);
	}
	else
	{
		aPetId = mTank * 5 - 6 + mLevel;
		if(aPetId >= PET_STINKY && aPetId < PET_END)
			mApp->mCurrentProfile->UnlockPet(aPetId, true);
	}

	if (aPetId == PET_PRESTO)
		mApp->mCurrentProfile->AddShells(5000);

	mApp->SwitchToHatchScreen(aPetId);
}

void Sexy::Board::HandleFeedButton()
{
	int aReqFoods[8]; // not cleared here in the original: both callees clear them
	int aFoodsInTank[8];
	GetExoticFoodsRequiredInTank(aReqFoods);
	GetExoticFoodsInTank(aFoodsInTank);
	int aNumOfButtonsToShow = 0;
	int ivar4 = 0;
	for (int i = 0; i < 8; i++)
	{
		if (aReqFoods[i] > 0)
			aNumOfButtonsToShow++;
		if (aReqFoods[i] - aFoodsInTank[i] > 0)
			ivar4++;
	}

	if (ivar4 != 0)
	{
		if (aNumOfButtonsToShow > 1)
		{
			mApp->DoFoodDialog();
		}
		else
		{
			for (int i = 0;i < 8;i++)
				if (aReqFoods[i] - aFoodsInTank[i] > 0)
					SpawnVirtualTankFood(i);
		}
	}
}

void Sexy::Board::UpdateHasVirtualTankFish()
{
	mHasVirtualTankFish = false;
	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		if ((*it)->mVirtualTankId >= 0)
		{
			mHasVirtualTankFish = true;
			return;
		}
	}
}

void Sexy::Board::BonusRoundDropShell()
{
	int aCount = (mApp->mSeed->Next() & 1) + 1;
	int aProbs[] = { 20, 30, 20, 10, 0 };

	for (int i = 0; i < aCount; i++)
	{
		int aCoinType = 1;

		// The original draws y before x, then the coin type
		int aY = mApp->mSeed->Next() % 10 + 50;
		int aX = (mApp->mSeed->Next() % 520) + 20;
		int aRandVal = Rand() % 100;
		int aProbIndex = 0;
		int aChance = 100;
		while (aProbIndex < 4)
		{
			aChance -= aProbs[aProbIndex];
			if (aChance <= aRandVal) break;
			aProbIndex++;
		}

		switch (aProbIndex)
		{
		case 0: aCoinType = 2; break;
		case 1: aCoinType = 4; break;
		case 2: aCoinType = 6; break;
		case 3: aCoinType = 7; break;
		default: aCoinType = 1; break;
		}

		double aVY = (double)(Rand() % 10) / 10.0 * 3.0 + 1.0;

		DropCoin(aX, aY, aCoinType, nullptr, aVY, 0);
	}
	m0x454 = 500;
}

void Sexy::Board::NostradamusSneezeEffect()
{
	if (Rand() % 100 >= 50)
		return;

	int aNostradamusCnt = 0;
	for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it)
	{
		FishTypePet* aPet = *it;
		if (aPet->mFishTypePetType == PET_NOSTRADAMUS)
		{
			aNostradamusCnt++;
			for (int i = 0; i < 5; i++)
			{
				// The original draws y before x
				int aY = Rand() % 21 + aPet->mY + 40;
				int aX = Rand() % 21 + aPet->mX + 30;
				SpawnBubble(aX, aY);
			}
		}
	}

	m0x2b4 = 30;
	PlaySample(SOUND_BUBBLES_ID, 3, 1.0);

	FishSong aFishSong;
	aFishSong.mTone = SOUND_SNEEZE;

	if (aNostradamusCnt > 5)
		aNostradamusCnt = 5;

	for (int i = 0; i < aNostradamusCnt; i++)
	{
		NoteData aNote;
		aNote.mPitch = 0;
		aNote.mVolume = 1.0;
		aNote.mDuration = Rand() % 150 + 100;
		aFishSong.mNoteDataVector.push_back(aNote);
	}
	mFishSongMgr->AddSong(aFishSong);

	mAlienTimer = 635;
	mApp->SomeMusicPlayFunc(false);
	ShowText("Attack Postponed by Sneeze of Power!", false, -1);
}

void Sexy::Board::PlayChompSound(bool flag)
{
	if (!CanPlaySound(SOUND_CHOMP_ID, 3))
		return;

	int aSoundId = SOUND_CHOMP2_ID - ((mApp->mSeed->Next() % 3) != 0);
	if (flag)
		aSoundId = SOUND_SLURP_ID;

	SoundInstance* aSndInst = mApp->mSoundManager->GetSoundInstance(GetSoundById(aSoundId));
	if (aSndInst)
	{
		if (flag)
			aSndInst->AdjustPitch(-18);
		aSndInst->Play(false, true);
	}
}

void Sexy::Board::PlaySlurpSound(bool isVoracious)
{
	if (!CanPlaySound(SOUND_SLURP_ID, 3))
		return;

	// One draw decides: 0 = slurp 3, 1 = slurp 2, otherwise slurp
	unsigned long aRand = mApp->mSeed->Next() % 5;
	int aSound;
	if (aRand == 0)
		aSound = SOUND_SLURP3;
	else
	{
		aSound = SOUND_SLURP2;
		if (aRand != 1)
			aSound = SOUND_SLURP;
	}

	SoundInstance* aSndInst = mApp->mSoundManager->GetSoundInstance(aSound);
	if (aSndInst)
	{
		if (isVoracious)
			aSndInst->AdjustPitch(-14);
		aSndInst->Play(false, true);
	}
}

void Sexy::Board::PlayDiamondSound()
{
	PlaySample(SOUND_DIAMOND_ID, 3, 1.0);
}

void Sexy::Board::PlayPointsSound()
{
	if (!CanPlaySound(SOUND_POINTS_ID, 3))
		return;

	int aSoundChoose = mApp->mSeed->Next() % 4;
	if (aSoundChoose != 0 && !mIsBonusRound)
	{
		if (aSoundChoose == 1)
			mApp->PlaySample(SOUND_POINTS2);
		else if(aSoundChoose == 2)
			mApp->PlaySample(SOUND_POINTS3);
		else
			mApp->PlaySample(SOUND_POINTS4);
		return;
	}
	mApp->PlaySample(SOUND_POINTS);
}

void Sexy::Board::PlaySplashSound()
{
	unsigned long aRand = mApp->mSeed->Next() % 3;
	if(aRand == 0)
		mApp->PlaySample(SOUND_SPLASH);
	else if(aRand == 1)
		mApp->PlaySample(SOUND_SPLASH2);
	else 
		mApp->PlaySample(SOUND_SPLASH3);
}

void Sexy::Board::PlayDieSound(int theObjType)
{
	if (!CanPlaySound(SOUND_DIE_ID, 3))
		return;

	int aPitch = 0;
	switch (theObjType) // case bodies in the original's layout order
	{
	case TYPE_GEKKO:
	case TYPE_SYLVESTER_FISH:
	case TYPE_BALL_FISH:
	case TYPE_BI_FISH:
		aPitch = -3;
		break;
	case TYPE_BREEDER:
		aPitch = 5;
		break;
	case TYPE_OSCAR:
	case TYPE_GRUBBER:
		aPitch = -6;
		break;
	case TYPE_PENTA:
		aPitch = 4;
		break;
	case TYPE_ULTRA:
		aPitch = -12;
		break;
	}

	SoundInstance* inst = mApp->mSoundManager->GetSoundInstance(SOUND_DIE);
	if (inst)
	{
		inst->AdjustPitch(aPitch);
		inst->Play(false, true);
	}
}

// A separate function in the original (0x538700): the price colour of the first seven store buttons
void Sexy::Board::SetMenuButtonsPriceColor(Color theColor)
{
	for (int i = 0; i < 7; i++)
	{
		if (mMenuButtons[i] && mMenuButtons[i]->mMouseVisible)
			mMenuButtons[i]->SetPriceTextColor(theColor);
	}
}

void Sexy::Board::PlayBirthSound(bool flag)
{
	int aFlags = mApp->mCurrentProfile->mCheatCodeFlags;
	if ((aFlags >> 6 & 1))
		PlaySample(SOUND_FART2_ID, 3, 1.0);
	else if (flag)
	{
		if ((aFlags >> 5 & 1))
			PlaySample(SOUND_FART_ID, 3, 1.0);
		else
			PlaySample(SOUND_SFX_ID, 3, 1.0);
	}
	else if ((aFlags >> 1 & 1))
		PlaySample(SOUND_FART_ID, 3, 1.0);
	else
		PlaySample(SOUND_BABY_ID, 3, 1.0);
}

void Sexy::Board::PlayPunchSound(int theDelay)
{
	PlaySample(SOUND_PUNCH_ID, theDelay, 1.0);
}

void Sexy::Board::PlayZapSound()
{
	if ((mApp->mSeed->Next() & 1) != 0)
	{
		PlaySample(SOUND_ZAP_ID, 3, 1.0);
		PlaySample(SOUND_ZZAM_ID, 3, 1.0);
	}
	else
		PlaySample(SOUND_ZAP_ID, 3, 1.0);

	if (mApp->mSeed->Next() % 10 == 0)
		PlaySample(SOUND_RICOCHET_ID, 3, 1.0);
}

bool Sexy::Board::CanPlaySound(int theSoundId, int theTimePassed)
{
	// Unsigned as the original: ids below SOUND_START_ID are rejected too
	int aNum = theSoundId - SOUND_START_ID;
	if ((unsigned int)aNum > 63)
		return false;
	if (mUpdateCnt - mSoundPlayedTimerArray[aNum] < theTimePassed)
		return false;
	mSoundPlayedTimerArray[aNum] = mUpdateCnt;
	return true;
}

void Sexy::Board::ResetLevel(WidgetManager* theWidgetManager)
{
	mGameObjectSet.clear();
	memset(mPetsInTank, 0, sizeof(mPetsInTank));
	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mFishList->clear();

	for (std::vector<DeadFish*>::iterator it = mDeadFishList->begin(); it != mDeadFishList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mDeadFishList->clear();

	for (std::vector<DeadAlien*>::iterator it = mDeadAlienList->begin(); it != mDeadAlienList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mDeadAlienList->clear();

	for (std::vector<Coin*>::iterator it = mCoinList->begin(); it != mCoinList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mCoinList->clear();

	for (std::vector<Oscar*>::iterator it = mOscarList->begin(); it != mOscarList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mOscarList->clear();

	for (std::vector<Ultra*>::iterator it = mUltraList->begin(); it != mUltraList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mUltraList->clear();

	for (std::vector<Gekko*>::iterator it = mGekkoList->begin(); it != mGekkoList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mGekkoList->clear();

	for (std::vector<Grubber*>::iterator it = mGrubberList->begin(); it != mGrubberList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mGrubberList->clear();

	for (std::vector<Penta*>::iterator it = mPentaList->begin(); it != mPentaList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mPentaList->clear();

	for (std::vector<Breeder*>::iterator it = mBreederList->begin(); it != mBreederList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mBreederList->clear();

	for (std::vector<Shot*>::iterator it = mShotList->begin(); it != mShotList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mShotList->clear();

	for (std::vector<Food*>::iterator it = mFoodList->begin(); it != mFoodList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mFoodList->clear();

	for (std::vector<Shadow*>::iterator it = mShadowList->begin(); it != mShadowList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mShadowList->clear();

	for (std::vector<Coin*>::iterator it = mNoteList->begin(); it != mNoteList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mNoteList->clear();

	for (std::vector<Coin*>::iterator it = mNikoPearlCoinList->begin(); it != mNikoPearlCoinList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mNikoPearlCoinList->clear();

	for (std::vector<Warp*>::iterator it = mWarpList->begin(); it != mWarpList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mWarpList->clear();

	for (std::vector<Missle*>::iterator it = mMissleList1->begin(); it != mMissleList1->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mMissleList1->clear();

	for (std::vector<Missle*>::iterator it = mMissleList2->begin(); it != mMissleList2->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mMissleList2->clear();

	for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mFishTypePetList->clear();

	for (std::vector<OtherTypePet*>::iterator it = mOtherTypePetList->begin(); it != mOtherTypePetList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mOtherTypePetList->clear();

	for (std::vector<Larva*>::iterator it = mLarvaList->begin(); it != mLarvaList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mLarvaList->clear();

	for (std::vector<Alien*>::iterator it = mAlienList->begin(); it != mAlienList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mAlienList->clear();

	for (std::vector<Alien*>::iterator it = mSmallAlienList->begin(); it != mSmallAlienList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mSmallAlienList->clear();

	for (std::vector<Bilaterus*>::iterator it = mBilaterusList->begin(); it != mBilaterusList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mBilaterusList->clear();

	for (std::vector<Fish*>::iterator it = mSpecialFishList->begin(); it != mSpecialFishList->end(); ++it)
	{
		theWidgetManager->RemoveWidget(*it);
		mApp->SafeDeleteWidget(*it);
	}
	mSpecialFishList->clear();

	RemoveWidgetHelper(mCyraxPtr);
	mCyraxPtr = nullptr;
	for (int i = 0; i < 7; i++)
	{
		RemoveWidgetHelper(mMenuButtons[i]);
		mMenuButtons[i] = 0;
	}
	m0x2dc = 0;
	m0x2e0 = 0;
	memset(mSlotUnlocked, 0, sizeof(mSlotUnlocked));	
}

void Sexy::Board::DeterminePricesAndSlots()
{
	// The original lays out the relax branch first
	if (mApp->mRelaxMode)
	{
		mSlotPrices[SLOT_GUPPY] = 100;
		mSlotPrices[SLOT_OSCAR] = 1000;
		mSlotPrices[SLOT_ULTRA] = 10000;
		mSlotPrices[SLOT_STARCATCHER] = 750;
		mSlotPrices[SLOT_GRUBBER] = 750;
		mSlotPrices[SLOT_GEKKO] = 2000;
		mSlotNumber[SLOT_EGG] = 6;
		switch (mTank)
		{
		case 1:
			mSlotPrices[SLOT_EGG] = 5000;
			mSlotNumber[SLOT_OSCAR] = 3;
			mSlotNumber[SLOT_GUPPY] = 0;
			break;
		case 2:
			mSlotPrices[SLOT_EGG] = 5000;
			mSlotNumber[SLOT_STARCATCHER] = 3;
			mSlotNumber[SLOT_GUPPY] = 0;
			break;
		case 3:
			mSlotPrices[SLOT_EGG] = 10000;
			mSlotNumber[SLOT_GRUBBER] = 3;
			mSlotNumber[SLOT_GEKKO] = 5;
			mSlotNumber[SLOT_GUPPY] = 0;
			break;
		case 4:
			// The original falls through into tank 1's tail here
			mSlotPrices[SLOT_EGG] = 25000;
			mSlotNumber[SLOT_ULTRA] = 5;
			mSlotNumber[SLOT_OSCAR] = 3;
			mSlotNumber[SLOT_GUPPY] = 0;
			break;
		default:
			break;
		}
		for (int i = 0; i < SLOT_END; i++)
			MakeAndUnlockMenuButton(i, true);
		RelaxModeConfig();
	}
	else
	{
		if (mTank == 1)
		{
			mSlotPrices[SLOT_OSCAR] = 1000;
			mSlotPrices[SLOT_WEAPON] = 1000;
			mSlotNumber[SLOT_GUPPY] = 0;
			mSlotNumber[SLOT_FOODLVL] = 1;
			mSlotNumber[SLOT_FOODLIMIT] = 2;
			mSlotNumber[SLOT_OSCAR] = 3;
			mSlotNumber[SLOT_WEAPON] = 5;
			mSlotNumber[SLOT_EGG] = 6;
			mSlotPrices[SLOT_GUPPY] = 100;
			mSlotPrices[SLOT_FOODLVL] = 200;
			mSlotPrices[SLOT_FOODLIMIT] = 300;
			switch (mLevel)
			{
			case 1:
				mAlienExpect = ALIEN_NONE;
				mSlotPrices[SLOT_EGG] = 150;
				mSlotNumber[SLOT_FOODLVL] = -1;
				mSlotNumber[SLOT_FOODLIMIT] = -1;
				mSlotNumber[SLOT_OSCAR] = -1;
				mSlotNumber[SLOT_WEAPON] = -1;
				break;
			case 2:
				mAlienExpect = ALIEN_WEAK_SYLV;
				mSlotPrices[SLOT_EGG] = 500;
				mAlienTimer = 1750;
				mSlotNumber[SLOT_OSCAR] = -1;
				mSlotNumber[SLOT_WEAPON] = -1;
				break;
			case 3:
				mAlienExpect = ALIEN_STRONG_SYLV;
				mSlotPrices[SLOT_EGG] = 2000;
				break;
			case 4:	
				mAlienExpect = ALIEN_BALROG;
				mSlotPrices[SLOT_EGG] = 3000;
				break;
			case 5:	
				mAlienExpect = ALIEN_BALROG;
				mSlotPrices[SLOT_EGG] = 5000;
				break;
			}
		}
		else if (mTank == 2)
		{
			mSlotNumber[SLOT_GUPPY] = 0;
			mSlotNumber[SLOT_FOODLVL] = 1;
			mSlotNumber[SLOT_FOODLIMIT] = 2;
			mSlotNumber[SLOT_POTION] = 3;
			mSlotNumber[SLOT_STARCATCHER] = 4;
			mSlotNumber[SLOT_WEAPON] = 5;
			mSlotNumber[SLOT_EGG] = 6;
			mSlotPrices[SLOT_GUPPY] = 100;
			mSlotPrices[SLOT_FOODLVL] = 200;
			mSlotPrices[SLOT_FOODLIMIT] = 300;
			mSlotPrices[SLOT_POTION] = 250;
			mSlotPrices[SLOT_STARCATCHER] = 750;
			mSlotPrices[SLOT_WEAPON] = 1000;
			switch (mLevel)
			{
			case 1:
				mAlienExpect = ALIEN_STRONG_SYLV;
				mSlotPrices[SLOT_EGG] = 750;
				mSlotNumber[SLOT_WEAPON] = -1;
				mSlotNumber[SLOT_STARCATCHER] = -1;
				break;
			case 2:
				mAlienExpect = ALIEN_BALROG;
				mSlotPrices[SLOT_EGG] = 3000;
				break;
			case 3:
				mAlienExpect = ALIEN_GUS;
				mSlotPrices[SLOT_EGG] = 5000;
				break;
			case 4:
				mAlienExpect = ALIEN_DESTRUCTOR;
				mSlotPrices[SLOT_EGG] = 7500;
				break;
			case 5:
				// The price is stored before the draw
				mSlotPrices[SLOT_EGG] = 10000;
				mAlienExpect = mApp->mSeed->Next() % 2 + ALIEN_GUS;
				break;
			}
		}
		else if (mTank == 3)
		{
			mSlotNumber[SLOT_GUPPY] = 0;
			mSlotPrices[SLOT_GEKKO] = 2000;
			mSlotPrices[SLOT_WEAPON] = 2000;
			mSlotNumber[SLOT_FOODLVL] = 1;
			mSlotNumber[SLOT_FOODLIMIT] = 2;
			mSlotNumber[SLOT_GRUBBER] = 3;
			mSlotNumber[SLOT_GEKKO] = 4;
			mSlotNumber[SLOT_WEAPON] = 5;
			mSlotNumber[SLOT_EGG] = 6;
			mSlotPrices[SLOT_GUPPY] = 100;
			mSlotPrices[SLOT_FOODLVL] = 200;
			mSlotPrices[SLOT_FOODLIMIT] = 300;
			mSlotPrices[SLOT_GRUBBER] = 750;
			switch (mLevel)
			{
			case 1:
				mAlienExpect = ALIEN_BALROG;
				mSlotPrices[SLOT_EGG] = 1000;
				mSlotNumber[SLOT_WEAPON] = -1;
				mSlotNumber[SLOT_GEKKO] = -1;
				break;
			case 2:
				// The price is stored before the draw
				mSlotPrices[SLOT_EGG] = 5000;
				mAlienExpect = ALIEN_GUS + mApp->mSeed->Next() % 2;
				break;
			case 3:
				mAlienExpect = ALIEN_PSYCHOSQUID;
				mSlotPrices[SLOT_EGG] = 7500;
				break;
			case 4:
				mAlienExpect = ALIEN_ULYSEES;
				mSlotPrices[SLOT_EGG] = 10000;
				break;
			case 5:
				// The price is stored before the draw
				mSlotPrices[SLOT_EGG] = 15000;
				mAlienExpect = ALIEN_ULYSEES + mApp->mSeed->Next() % 2;
				break;
			}
		}
		else if (mTank == 4)
		{
			mSlotNumber[SLOT_BREEDER] = 0;
			mSlotPrices[SLOT_BREEDER] = 200;
			mSlotPrices[SLOT_FOODLVL] = 200;
			mSlotNumber[SLOT_FOODLVL] = 1;
			mSlotNumber[SLOT_FOODLIMIT] = 2;
			mSlotNumber[SLOT_OSCAR] = 3;
			mSlotNumber[SLOT_ULTRA] = 4;
			mSlotNumber[SLOT_WEAPON] = 5;
			mSlotNumber[SLOT_EGG] = 6;
			mSlotPrices[SLOT_FOODLIMIT] = 300;
			mSlotPrices[SLOT_OSCAR] = 1000;
			mSlotPrices[SLOT_ULTRA] = 10000;
			mSlotPrices[SLOT_WEAPON] = 5000;
			switch (mLevel)
			{
			case 1:
				mAlienExpect = ALIEN_BALROG;
				mSlotPrices[SLOT_EGG] = 3000;
				mSlotNumber[SLOT_WEAPON] = -1;
				mSlotNumber[SLOT_ULTRA] = -1;
				break;
			case 2:
				mAlienExpect = 8;
				mSlotPrices[SLOT_EGG] = 25000;
				break;
			case 3:
				mAlienExpect = ALIEN_GUS;
				mSlotPrices[SLOT_EGG] = 50000;
				break;
			case 4:
				mAlienExpect = 8;
				mSlotPrices[SLOT_EGG] = 75000;
				break;
			case 5:
				mAlienExpect = 8;
				mSlotPrices[SLOT_EGG] = 99999;
				break;
			}
		}
		else if (mTank == 5)
		{
			mSlotNumber[SLOT_EGG] = 6;
			mSlotPrices[SLOT_EGG] = 0;
			m0x43c = 3;
			mAlienExpect = ALIEN_CYRAX;
		}
	}

	// Three separate tests in the original (each one runs after the previous block)
	if (mApp->mGameMode == GAMEMODE_CHALLENGE)
	{
		if (mTank == 2)
			mSlotPrices[SLOT_EGG] = 5000;
		else if (mTank == 3)
			mSlotPrices[SLOT_EGG] = 15000;
		else if (mTank == 4)
			mSlotPrices[SLOT_EGG] = 25000;
	}
	if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
	{
		mSlotPrices[SLOT_EGG] = 100;
	}
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		ChangeBackground(mApp->mCurrentProfile->m0x8c + 1);
		return;
	}

	if (mTank == 1)
		ChangeBackground(1);
	else if (mTank == 2)
		ChangeBackground(2);
	else if (mTank == 3)
		ChangeBackground(4);
	else if (mTank == 4)
		ChangeBackground(5);
	else if (mTank == 5)
		ChangeBackground(6);
}

void Sexy::Board::HandleBuySlotPressed(int theSlotId)
{
	MenuButtonWidget* aBtn = GetMenuButtonById(theSlotId);
	if (!aBtn)
		return;

	int aPrice = mSlotPrices[theSlotId];

	switch (theSlotId)
	{
	case SLOT_GUPPY:
		if (IsTankAndLevelNB(1, 1) && aPrice > mMoney)
			ShowText("You can\'t afford new fish yet! Collect more money!", false, 14);

		if (!Buy(aPrice, true))
			return;

		SpawnGuppyBought();
		if (mMessageShown[2]) mMessageShown[2] = false;

		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		break;
	case SLOT_BREEDER:
		if (!Buy(aPrice, true))
			return;
		SpawnBreederBought();
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		break;
	case SLOT_FOODLVL:
		if (gFoodType < 2 && Buy(aPrice, true))
		{
			if (mMessageShown[15])
				mMessageShown[15] = false;
			gFoodType++;
			MenuButtonSetupNoVT(SLOT_FOODLVL, true);

			if (!mSlotUnlocked[SLOT_FOODLIMIT] && mTank == 1 && mLevel == 2)
			{
				MakeAndUnlockMenuButton(SLOT_FOODLIMIT, true);
				MakeAndUnlockMenuButton(SLOT_EGG, true);
				if (!mApp->mCurrentProfile->mFinishedGame)
					ShowText("Upgrade Food Quantity to drop more food at once!", false, 12);
			}

			PlaySample(SOUND_BUY_ID, 3, 1.0);
		}
		break;
	case SLOT_FOODLIMIT:
		if (gFoodLimit < 9 && Buy(aPrice, true))
		{
			if(mTank == 1 && gFoodLimit == 1 && !mApp->mCurrentProfile->mFinishedGame)
				ShowText("Hold down mouse button to Auto-Feed!", false, -1);

			gFoodLimit++;
			MenuButtonSetupNoVT(SLOT_FOODLIMIT, true);

			PlaySample(SOUND_BUY_ID, 3, 1.0);
		}
		break;
	case SLOT_OSCAR:
		if (!Buy(aPrice, true))
			return;
		SpawnOscarBought();
		// Tank 4 unlocks the ultravore (and the weapon) from level 2; its first level only the egg
		if (mTank != 4)
			MakeAndUnlockMenuButton(SLOT_WEAPON, true);
		else if (mLevel > 1)
		{
			MakeAndUnlockMenuButton(SLOT_ULTRA, true);
			MakeAndUnlockMenuButton(SLOT_WEAPON, true);
		}
		MakeAndUnlockMenuButton(SLOT_EGG, true);
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		break;
	case SLOT_POTION:
		if (!m0x440 && Buy(aPrice, true))
		{
			if (!m0x441)
			{
				m0x441 = true;
				if (mLevel < 3)
				{
					ShowText("Click on tank to drop star potion!", 0, -1);
					mMessageWidget->mMessageTimer = 180;
				}
			}
			m0x440 = true;
			PlaySample(SOUND_BUY_ID, 3, 1.0);
		}
		break;
	case SLOT_STARCATCHER:
		if (!Buy(aPrice, true))
			return;
		SpawnPentaBought();
		MakeAndUnlockMenuButton(SLOT_WEAPON, true);
		MakeAndUnlockMenuButton(SLOT_EGG, true);
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		break;
	case SLOT_GRUBBER:
		if (!Buy(aPrice, true))
			return;
		SpawnGrubberBought();
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		if (mLevel == 1)
			MakeAndUnlockMenuButton(SLOT_EGG, true);
		else
			MakeAndUnlockMenuButton(SLOT_GEKKO, true);
		break;
	case SLOT_GEKKO:
		if (!Buy(aPrice, true))
			return;
		m0x4a4++;
		SpawnGekkoBought();
		MakeAndUnlockMenuButton(SLOT_WEAPON, true);
		MakeAndUnlockMenuButton(SLOT_EGG, true);
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySplashSound();
		break;
	case SLOT_ULTRA:
		if (!Buy(aPrice, true))
			return;
		SpawnUltraBought();
		MakeAndUnlockMenuButton(SLOT_WEAPON, true);
		MakeAndUnlockMenuButton(SLOT_EGG, true);
		PlaySample(SOUND_GROW_ID, 3, 1.0);
		PlaySample(SOUND_SPLASHBIG_ID, 3, 1.0);
		break;
	case SLOT_WEAPON:
		if (m0x3e4 < 12 && Buy(aPrice, true))
		{
			m0x3e4++;
			MenuButtonSetupNoVT(SLOT_WEAPON, true);
			PlaySample(SOUND_BUY_ID, 3, 1.0);
		}
		break;
	case SLOT_EGG:
		HandleBuyEgg();
		break;
	}
}

void Sexy::Board::RelaxModeConfig()
{
	if (mTank == 1)
	{
		int aPossibleAliens[] = { ALIEN_STRONG_SYLV, ALIEN_BALROG };
		mAlienExpect = aPossibleAliens[Rand() % 2];
	}
	else if (mTank == 2)
	{
		int aPossibleAliens[] = { ALIEN_STRONG_SYLV, ALIEN_BALROG, ALIEN_GUS, ALIEN_DESTRUCTOR};
		mAlienExpect = aPossibleAliens[Rand() % 4];
	}
	else if (mTank == 3)
	{
		int aPossibleAliens[] = {  ALIEN_BALROG, ALIEN_GUS, ALIEN_DESTRUCTOR, \
			ALIEN_PSYCHOSQUID, ALIEN_ULYSEES};
		mAlienExpect = aPossibleAliens[Rand() % 5];
	}
	else if (mTank == 4)
	{
		int aPossibleAliens[] = { ALIEN_BALROG, ALIEN_GUS, ALIEN_DESTRUCTOR, \
			ALIEN_PSYCHOSQUID, ALIEN_ULYSEES, ALIEN_BILATERUS };
		mAlienExpect = aPossibleAliens[Rand() % 6];
	}
}

void Sexy::Board::MakeVirtualTankButtons()
{
	if (mMenuButtons[SLOT_GRUBBER])
		return;

	if (mMessageWidget)
	{
		mMessageWidget->mX = 20;
		mMessageWidget->mWidth = 600;
		if (mApp->IsScreenSaver())
			mMessageWidget->mY = 60;
	}

	mMenuButtons[SLOT_GRUBBER] = new MenuButtonWidget(mWidgetManager, 10, this, "Visit Store");
	mMenuButtons[SLOT_GRUBBER]->mButtonImage = IMAGE_MENUBTNU;
	mMenuButtons[SLOT_GRUBBER]->mOverImage = IMAGE_MENUBTNO;
	mMenuButtons[SLOT_GRUBBER]->mDownImage = IMAGE_MENUBTND;
	mMenuButtons[SLOT_GRUBBER]->Resize(70, 2, 58, 60);
	mWidgetManager->AddWidget(mMenuButtons[SLOT_GRUBBER]);

	mMenuButtons[SLOT_GRUBBER]->SetPriceText("STORE");
	mMenuButtons[SLOT_GRUBBER]->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 2);

	mMenuButtons[SLOT_GEKKO] = new MenuButtonWidget(mWidgetManager, 11, this, "Add/Remove Fish");
	mMenuButtons[SLOT_GEKKO]->mButtonImage = IMAGE_MENUBTNU;
	mMenuButtons[SLOT_GEKKO]->mOverImage = IMAGE_MENUBTNO;
	mMenuButtons[SLOT_GEKKO]->mDownImage = IMAGE_MENUBTND;
	mMenuButtons[SLOT_GEKKO]->Resize(144, 2, 58, 60);
	mWidgetManager->AddWidget(mMenuButtons[SLOT_GEKKO]);
	mMenuButtons[SLOT_GEKKO]->SetPriceText("FISH");
	mMenuButtons[SLOT_GEKKO]->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 3);

	mMenuButtons[SLOT_ULTRA] = new MenuButtonWidget(mWidgetManager, 12, this, "Add/Remove Pets");
	mMenuButtons[SLOT_ULTRA]->mButtonImage = IMAGE_MENUBTNU;
	mMenuButtons[SLOT_ULTRA]->mOverImage = IMAGE_MENUBTNO;
	mMenuButtons[SLOT_ULTRA]->mDownImage = IMAGE_MENUBTND;
	mMenuButtons[SLOT_ULTRA]->Resize(216, 2, 58, 60);
	mWidgetManager->AddWidget(mMenuButtons[SLOT_ULTRA]);
	mMenuButtons[SLOT_ULTRA]->SetPriceText("PETS");
	mMenuButtons[SLOT_ULTRA]->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 4);

	mMenuButtons[SLOT_WEAPON] = new MenuButtonWidget(mWidgetManager, 13, this, "Tank Options");
	mMenuButtons[SLOT_WEAPON]->mButtonImage = IMAGE_MENUBTNU;
	mMenuButtons[SLOT_WEAPON]->mOverImage = IMAGE_MENUBTNO;
	mMenuButtons[SLOT_WEAPON]->mDownImage = IMAGE_MENUBTND;
	mMenuButtons[SLOT_WEAPON]->Resize(364, 2, 58, 60);
	mWidgetManager->AddWidget(mMenuButtons[SLOT_WEAPON]);
	mMenuButtons[SLOT_WEAPON]->SetPriceText("TANK");
	mMenuButtons[SLOT_WEAPON]->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 5);

	mMenuButtons[SLOT_EGG] = new MenuButtonWidget(mWidgetManager, 14, this, "Special Food");
	mMenuButtons[SLOT_EGG]->mButtonImage = IMAGE_MENUBTNU;
	mMenuButtons[SLOT_EGG]->mOverImage = IMAGE_MENUBTNO;
	mMenuButtons[SLOT_EGG]->mDownImage = IMAGE_MENUBTND;
	mMenuButtons[SLOT_EGG]->Resize(290, 2, 58, 60);
	mWidgetManager->AddWidget(mMenuButtons[SLOT_EGG]);
	mMenuButtons[SLOT_EGG]->SetPriceText("FEED");
	mMenuButtons[SLOT_EGG]->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 1);
	
	mBackButton = new MenuButtonWidget(mWidgetManager, 15, this, "Back to Main Menu");
	mBackButton->mButtonImage = IMAGE_MENUBTNU;
	mBackButton->mOverImage = IMAGE_MENUBTNO;
	mBackButton->mDownImage = IMAGE_MENUBTND;
	mBackButton->Resize(438, 2, 58, 60);
	mWidgetManager->AddWidget(mBackButton);
	mBackButton->SetPriceText("BACK");
	mBackButton->Configure(IMAGE_TROPHYBUTTONS, 5, 1, 0, 0);
}

void Sexy::Board::StartVirtualTank()
{
	mAlienExpect = ALIEN_NONE;
	mPause = false;
	mShouldSave = true;

	gFoodType = 0;
	gFoodLimit = 1;
	gMerylActive = false;
	gWadsworthTimer = 0;
	gUnkInt02 = 0;

	m0x440 = false;
	mGameUpdateCnt = 0;
	m0x450 = 0;
	m0x454 = 0;
	m0x45c = 0;
	m0x458 = 0;

	int aVal = Unk01();
	m0x3b4 = aVal;
	m0x3b8 = aVal;

	m0x4fc = true;
	SetAlienExpectVT();
	WidgetSetupVT();
	DoVirtualTankDialog();
}

void Sexy::Board::SetAlienExpectVT()
{
	int aPossibleAliens[7] = {2,3,4,5,7,6,8};
	mAlienExpect = aPossibleAliens[Rand() % 7];
	DetermineAlienSpawnCoordsVT();
}

void Sexy::Board::WidgetSetupVT()
{
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		return;

	m0x3e4 = 8;

	int fishListFreeCapacity = mFishList->size();
	int breederListFreeCapacity = mBreederList->size(); // May be incorrect
	gFoodLimit = fishListFreeCapacity + breederListFreeCapacity;
	if (gFoodLimit < 5)
		gFoodLimit = 5;
	else if (gFoodLimit > 9)
		gFoodLimit = 9;

	if (mApp->mCurrentProfile->mBubbulatorBought == 0)
		mBubbulatorShown = false;
	if (mApp->mCurrentProfile->mAlienAttractorBought == 0)
		mAlienAttractorShown = false;

	if (mMenuButtons[SLOT_EGG])
	{
		bool aVisible = false;
		int foods[8]; // filled by GetExoticFoodsRequiredInTank
		GetExoticFoodsRequiredInTank(foods);
		// Eight separate tests in the original (no loop)
		if (foods[0] > 0)
			aVisible = true;
		if (foods[1] > 0)
			aVisible = true;
		if (foods[2] > 0)
			aVisible = true;
		if (foods[3] > 0)
			aVisible = true;
		if (foods[4] > 0)
			aVisible = true;
		if (foods[5] > 0)
			aVisible = true;
		if (foods[6] > 0)
			aVisible = true;
		if (foods[7] > 0)
			aVisible = true;
		mMenuButtons[SLOT_EGG]->SetVisible(aVisible);
	}
	if (mMenuButtons[SLOT_GEKKO])
	{
		bool aVisible = false;
		// The original walks the whole set (no break)
		for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
		{
			GameObject* anObj = *it;
			if (anObj->mVirtualTankId >= 0 && anObj->mVirtualTankId < 108)
				aVisible = true;
		}
		mMenuButtons[SLOT_GEKKO]->SetVisible(aVisible);
	}

	if (mMenuButtons[SLOT_ULTRA])
		mMenuButtons[SLOT_ULTRA]->SetVisible(mApp->mCurrentProfile->mNumOfUnlockedPets > 0);

	if (mApp->IsScreenSaver())
	{
		mMenuButtons[SLOT_GRUBBER]->SetVisible(false);
		mMenuButtons[SLOT_GEKKO]->SetVisible(false);
		mMenuButtons[SLOT_ULTRA]->SetVisible(false);
		mMenuButtons[SLOT_WEAPON]->SetVisible(false);
		mMenuButtons[SLOT_EGG]->SetVisible(false);
		mBackButton->SetVisible(false);
		mMoneyLabel->SetVisible(false);
		mMenuButton->SetVisible(false);
	}
}

void Sexy::Board::ChangeBackground(int theBGId)
{
	if (theBGId < 1)
		theBGId = 1;
	else if (theBGId > 6)
		theBGId = 6;

	mCurrentBackgroundId = theBGId;
	Unk03();
}

bool Sexy::Board::DoCheatCode(int theCheatCode)
{
	bool aStatus = mApp->mCurrentProfile->ToggleCheatFlag(theCheatCode);
	if (theCheatCode == CC_WAVY)
		ShowText(aStatus ? "Wavy Mode Enabled" : "Wavy Mode Disabled", false, -1);
	else if (theCheatCode == CC_PREGO)
		ShowText(aStatus ? "Original Prego Sound Enabled" : "Original Prego Sound Disabled", false, -1);
	else if (theCheatCode == CC_BETATEST)
		ShowText(aStatus ? "Original Breeder Sound Enabled" : "Original Breeder Sound Disabled", false, -1);
	else if (theCheatCode == CC_SUPERMEGA)
		ShowText(aStatus ? "Ultra Prego Sound Enabled" : "Ultra Prego Sound Disabled", false, -1);
	else if (theCheatCode == CC_VOID)
	{
		mApp->mCurrentProfile->SetCheatFlag(CC_SPACE, false);
		ApplyShadowsIf3D();
		Unk03();
		ShowText(aStatus ? "Void Mode Enabled" : "Void Mode Disabled", false, -1);
	}
	else if (theCheatCode == CC_SPACE)
	{
		mApp->mCurrentProfile->SetCheatFlag(CC_VOID, false);
		ApplyShadowsIf3D();
		Unk03();
		ShowText(aStatus ? "Space Mode Enabled" : "Space Mode Disabled", false, -1);
		if (aStatus)
			mStarField->Init(1000);
	}
	else if (theCheatCode == CC_ZOMBIE)
	{
		gZombieMode = aStatus;
		ShowText(aStatus ? "Zombie Mode Enabled" : "Zombie Mode Disabled", false, -1);
	}
	return true;
}

GameObject* Sexy::Board::GetPrestoPet()
{
	for (std::vector<FishTypePet*>::iterator it = mFishTypePetList->begin(); it != mFishTypePetList->end(); ++it)
	{
		FishTypePet* aFPet = *it;

		if (aFPet->m0x230)
			return aFPet;
	}

	for (std::vector<OtherTypePet*>::iterator it = mOtherTypePetList->begin(); it != mOtherTypePetList->end(); ++it)
	{
		OtherTypePet* aOPet = *it;

		if (aOPet->mIsPresto)
			return aOPet;
	}

	return nullptr;
}

void Sexy::Board::ApplyShadowsIf3D()
{
	bool isVisible = false;
	if (mApp->Is3DAccelerated() || ((mApp->mCurrentProfile->mCheatCodeFlags >> 2) & 1) != 0)
		isVisible = true;
	if(((mApp->mCurrentProfile->mCheatCodeFlags >> 3) & 1) != 0)
		isVisible = false;

	for (std::vector<Shadow*>::iterator it = mShadowList->begin(); it != mShadowList->end(); ++it)
	{
		Shadow* aShad = *it;
		aShad->mVisible = isVisible;
	}
}

void Sexy::Board::UpdateNikoPosition()
{
	for (std::vector<OtherTypePet*>::iterator it = mOtherTypePetList->begin(); it != mOtherTypePetList->end(); ++it)
	{
		OtherTypePet* aPet = *it;

		aPet->UpdateNikoPosition(mCurrentBackgroundId);
	}
}

MenuButtonWidget* Sexy::Board::MakeAndUnlockMenuButton(int theBtnId, bool flag)
{
	int aBtnSlot = mSlotNumber[theBtnId];

	if (aBtnSlot < 0)
		return nullptr;

	// The button is kept in a local: the original does not read mMenuButtons again after creating it
	MenuButtonWidget* aBtn = mMenuButtons[aBtnSlot];
	if (aBtn == nullptr)
	{
		const int aBtnXCoords[] = { 18, 87, 144, 217, 290, 363, 436 };
		// The tooltip is fetched before the allocation, as the original does
		const char* aToolTip = GetMenuButtonToolTip(theBtnId);
		aBtn = new MenuButtonWidget(mWidgetManager, theBtnId, this, aToolTip);
		mMenuButtons[aBtnSlot] = aBtn;

		mWidgetManager->AddWidget(aBtn);
		aBtn->mButtonImage = IMAGE_MENUBTNU;
		aBtn->mDownImage = IMAGE_MENUBTND;
		aBtn->mOverImage = IMAGE_MENUBTNO;
		aBtn->Resize(aBtnXCoords[aBtnSlot], 3, 58, 60);
		MenuButtonSetupNoVT(theBtnId, flag);
		mSlotUnlocked[theBtnId] = true;
		m0x2e0++;
	}

	return aBtn;
}

void Sexy::Board::MenuButtonSetupNoVT(int theBtnId, bool flag)
{
	MenuButtonWidget* aBtn = GetMenuButtonById(theBtnId);
	if (!aBtn)
		return;

	aBtn->SetSlotPrice(mSlotPrices[theBtnId]);
	// Each case makes its own Configure call; the original merges their tails into one call site
	switch (theBtnId)
	{
	case 0:
		aBtn->Configure(IMAGE_SMALLSWIM, -12, -18, 0, -1);
		break;
	case 1:
		aBtn->Configure(IMAGE_SCL_BREEDER, 8, 2, 0, -1);
		break;
	case 2:
		if (gFoodType == 0)
			aBtn->Configure(IMAGE_FOOD, 9, 3, 1, 0);
		else if (gFoodType == 1)
			aBtn->Configure(IMAGE_FOOD, 9, 3, 2, 0);
		else
			aBtn->SetMaxedOut();
		break;
	case 3:
		if (gFoodLimit >= 9)
			aBtn->SetMaxedOut();
		else
		{
			int aNextLimit = gFoodLimit + 1;
			if (aNextLimit >= 9)
				aNextLimit = 9;
			aBtn->SetSlotText(StrFormat("%d", aNextLimit));
		}
		break;
	case 4:
		aBtn->Configure(IMAGE_SCL_OSCAR, 8, 3, 0, -1);
		break;
	case 5:
		aBtn->Configure(IMAGE_FOOD, 10, 1, 3, 0);
		break;
	case 6:
		aBtn->Configure(IMAGE_SCL_STARCATCHER, 8, 2, 0, -1);
		break;
	case 7:
		aBtn->Configure(IMAGE_SCL_GRUBBER, 8, 1, 0, -1);
		break;
	case 8:
		aBtn->Configure(IMAGE_SCL_GEKKO, 10, 2, 0, -1);
		break;
	case 9:
		aBtn->Configure(IMAGE_SCL_ULTRA, 10, 2, 0, -1);
		break;
	case 10:
		aBtn->Configure(IMAGE_LASERUPGRADES, 7, 3, 0, m0x3e4 - 2);
		if (m0x3e4 >= 12)
		{
			if (flag)
			{
				ShowText("Hold down mouse button for Rapid Fire!", false, -1);
			}
			aBtn->SetMaxedOut();
		}
		break;
	case 11:
		if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
		{
			aBtn->Configure(IMAGE_EGGPIECES, 7, 3, 0, 2);
			aBtn->SetSlotText("?");
			int i;
			for (i = 0; i < 24; i++)
			{
				if (mPetsInTank[i] == 0 && i != 19 && mApp->mCurrentProfile->IsPetUnlocked(i))
					break;
			}
			if (i == 24)
				aBtn->SetMaxedOut();
		}
		else if (mApp->mGameMode == GAMEMODE_SANDBOX)
			aBtn->Configure(IMAGE_EGGPIECES, 7, 3, 0, 2);
		else if (m0x43c > 0 && m0x43c <= 3)
			aBtn->Configure(IMAGE_EGGPIECES, 7, 3, 0, m0x43c - 1);
		break;
	}
	if (flag)
		aBtn->StartHatchAnimation();
}

MenuButtonWidget* Sexy::Board::GetMenuButtonById(int theBtnId)
{
	int aSlotId = mSlotNumber[theBtnId];
	if (aSlotId < 0)
		return nullptr;
	return mMenuButtons[aSlotId];
}

void Sexy::Board::PlaySample(int theSoundId, int theUpdatesBeforePlayingAgain, double theVolume)
{
	int theIdx = theSoundId - 262;
	if ((unsigned int)theIdx <= 63 && mUpdateCnt - mSoundPlayedTimerArray[theIdx] >= theUpdatesBeforePlayingAgain)
	{
		mSoundPlayedTimerArray[theIdx] = mUpdateCnt;
		SoundInstance* anInstance = mApp->mSoundManager->GetSoundInstance(GetSoundById(theSoundId));
		if (anInstance)
		{
			anInstance->SetVolume(theVolume);
			anInstance->Play(false, true);
		}
	}
}

Point Sexy::Board::GetBubbleSpawnCoords()
{
	switch (mCurrentBackgroundId)
	{
	case 1:
		return Point(470, 300);
	case 2:
	case 3:
		return Point(550, 315);
	case 4:
		return Point(10, 325);
	case 5:
		return Point(540, 315);
	case 6:
		return Point(360, 315);
	default:
		return Point(480, 300);
	}
}

Point Sexy::Board::GetAlienAttractorSpawnCoords()
{
	switch (mCurrentBackgroundId)
	{
	case 1:
		return Point(267, 320);
	case 2:
		return Point(255, 310);
	case 3:
		return Point(260, 310);
	case 4:
		return Point(270, 345);
	case 5:
		return Point(292, 320);
	case 6:
		return Point(210, 352);
	default:
		return Point(480, 300);
	}
}

void Sexy::Board::UpdateMoneyLabelText()
{
	if (!mMoneyLabel)
		return;

	// The formatted string is built straight into SetLabel's by-value argument (no copy)
	mMoneyLabel->SetLabel(StrFormat("%d", mApp->mGameMode == GAMEMODE_VIRTUAL_TANK ? mApp->mCurrentProfile->mShells : mMoney));
}

bool Sexy::Board::Buy(int theCost, bool playBuzzer)
{
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		return true;

	if (mApp->mRelaxMode && mMoney < 5)
		mMoney = 5;

	// Read once: the original keeps this value across the Unk10 call and takes the cost from it
	int aMoney = mMoney;
	if (theCost > aMoney && theCost > aMoney + Unk10())
	{
		if (playBuzzer)
		{
			PlaySample(SOUND_BUZZER_ID, 3, 1.0);
			m0x450 = 70;
		}
		else if(m0x450 == 0)
			m0x450 = 70;

		if (mMoney == 0 && IsFirstLevel())
			mApp->DoDialogUnkF(DIALOG_OUT_OF_MONEY_LOAN, true, "OUT OF MONEY!", "You\'ve run out of money!  Because you\'re new, we\'ll float you a loan, but be careful next time!", "Click to Continue", Dialog::BUTTONS_FOOTER);

		return false;
	}

	mMoney = aMoney - theCost;
	if (mApp->mRelaxMode && mMoney < 5 && mMoney >= 0)
		mMoney = 5;
	UpdateMoneyLabelText();

	return true;
}

// A separate function in the original (0x53C280): gives back the price of a pellet that could not be dropped
void Sexy::Board::RefundFoodPrice()
{
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
	{
		if (!m0x440 && !AliensInTank())
			mMoney += m0x4ac;
		if (mMoney > 9999999)
			mMoney = 9999999;
		UpdateMoneyLabelText();
	}
}

void Sexy::Board::DropFood(int theX, int theY, int unk1, bool unk2, int theFoodCantEatTimer, int theFoodTypeOverride)
{
	if (unk1 == 0)
	{
		// Food with m0x18c set or exotic food does not count against the limit
		int aCnt = 0;
		for (std::vector<Food*>::iterator it = mFoodList->begin(); it != mFoodList->end(); ++it)
		{
			if ((*it)->m0x18c != 0 || (*it)->mExoticFoodType != 0)
				aCnt++;
		}
		if ((int)mFoodList->size() >= gFoodLimit + aCnt)
		{
			RefundFoodPrice();
			if (IsTankAndLevelNB(1, 1) && mMessageWidget->mMessageTimer <= 0)
				ShowText("You can only drop 1 food pellet at a time for now", false, 6);
			else if(IsTankAndLevelNB(1, 2) && mMessageWidget->mMessageTimer <= 0 && mSlotUnlocked[3])
				ShowText("Upgrade Food Quantity to drop more food at once!", false, 13);
			SortGameObjects();
			return;
		}
	}

	Food* aFood = new Food(theX, theY, unk1, unk2, 0);
	aFood->mFoodType = gFoodType;
	if (unk1 != 0)
		aFood->mFoodType = 1;

	// the original tests m0x440 before unk1 (0x543469); both PlaySample calls share one call site
	if (m0x440)
	{
		if (unk1 == 0)
		{
			if (!unk2)
			{
				aFood->mFoodType = 3;
				m0x440 = false;
			}
			else
				PlaySample(SOUND_DROPFOOD_ID, 3, 1.0);
		}
	}
	else if (unk1 == 0)
		PlaySample(SOUND_DROPFOOD_ID, 3, 1.0);

	if(theFoodTypeOverride != -1)
		aFood->mFoodType = theFoodTypeOverride;
	if(theFoodCantEatTimer >= 0)
		aFood->mCantEatTimer = theFoodCantEatTimer;

	AddGameObject(aFood, true);
	mWidgetManager->AddWidget(aFood);
	SortGameObjects();
}

void Sexy::Board::NostradamusDropFood(int theX, int theY)
{
	Food* aFood = new Food(theX, theY, 0, 0, 2);
	AddGameObject(aFood, true);
	mWidgetManager->AddWidget(aFood);
	SortGameObjects();
}

void Sexy::Board::DropCoin(int theX, int theY, int theType, OtherTypePet* thePet, double theSpeed, int theUpdateCnt)
{
	Coin* aCoin = new Coin(theX, theY, theType, thePet, theSpeed);
	aCoin->mUpdateCnt = theUpdateCnt;
	AddGameObject(aCoin, false);
	mWidgetManager->AddWidget(aCoin);
	SortGameObjects();
}

bool Sexy::Board::IsTankAndLevelNB(int theTank, int theLevel)
{
	return mTank == theTank && mLevel == theLevel && !mApp->mCurrentProfile->mFinishedGame;
}

void Sexy::Board::CheckMouseDown(int theX, int theY)
{
	bool isSomething = Unk08(theX, theY);
	if (isSomething)
		return;
	mApp->PlaySample(SOUND_TAPGLASS);
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		return;

	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it)
	{
		Fish* aFish = *it;
		aFish->ClickedBehavior(theX, theY);
	}

	for (std::vector<Oscar*>::iterator it = mOscarList->begin(); it != mOscarList->end(); ++it)
	{
		Oscar* anOscar = *it;
		anOscar->ClickedBehavior(theX, theY);
	}

	for (std::vector<Gekko*>::iterator it = mGekkoList->begin(); it != mGekkoList->end(); ++it)
	{
		Gekko* aGekko = *it;
		aGekko->ClickedBehavior(theX, theY);
	}

	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* aCurObj = *it;
		if (aCurObj->mSongId != -1 && aCurObj->Contains(theX, theY))
		{
			mFishSongMgr->ClearSongs();
			return;
		}
	}

}

void Sexy::Board::UpdateSlotPrice(int theSlotId, int thePrice)
{
	if (thePrice > 99999)
		thePrice = 99999;
	mSlotPrices[theSlotId] = thePrice;
	MenuButtonWidget* aBtn = GetMenuButtonById(theSlotId);
	if (aBtn) aBtn->SetSlotPrice(thePrice);
}

// A separate function in the original (0x538100)
const char* Sexy::Board::GetMenuButtonToolTip(int theBtnId)
{
	switch (theBtnId)
	{
	case 0:
		return "buy guppy";
	case 1:
		return "buy breeder";
	case 2:
		return "upgrade food quality";
	case 3:
		return "upgrade food quantity";
	case 4:
		return "buy carnivore";
	case 5:
		return "buy star potion";
	case 6:
		return "buy starcatcher";
	case 7:
		return "buy guppycruncher";
	case 8:
		return "buy beetlemuncher";
	case 9:
		return "buy ultravore";
	case 10:
		return "upgrade weapon";
	case 11:
		switch (mApp->mGameMode)
		{
		case GAMEMODE_TIME_TRIAL:
			return "buy random pet";
		case GAMEMODE_SANDBOX:
			return "end level";
		default:
			return "buy egg piece";
		}
	default:
		return "";
	}
}

void Sexy::Board::ResetMessageWidget(int unk)
{
	if (mMessageWidget && mMessageWidget->mMessageId == unk)
		mMessageWidget->mMessageTimer = 0;
}

void Sexy::Board::MakeNote(int theX, int theY, int unk, const SexyString& theText)
{
	Coin* aCoin = new Coin(theX, theY, COIN_NOTE, nullptr, -1.0);
	aCoin->m0x1a0 = unk;
	aCoin->m0x1a4 = theText;
	AddGameObject(aCoin, false);
	mWidgetManager->AddWidget(aCoin);
	SortGameObjects();
}

int Sexy::Board::PlayFishSong(int theObjType, int theSpecObjType)
{
	// mFishSongMgr is read from the board each time (the original reloads it for PlayFishSong)
	if (mFishSongMgr->mSongList.size() != 0)
		return -1;

	int aSongId = mFishSongMgr->mCounter;
	mFishSongMgr->mCounter++;

	FishSongData* aSongData = GetSongData(theSpecObjType);

	if (aSongData == nullptr)
		return -1;

	SexyString aTitle; // default-constructed in the original
	bool titleFound = aSongData->GetProperty("title", &aTitle);

	if (titleFound)
	{
		if (aSongData->GetProperty("long", nullptr))
			aTitle.append(" (extended)");

		ShowText(aTitle, false, -1);
		mMessageWidget->mMessageTimer = 180;
	}

	gUnkBool02 = true;

	FishSong* aSong = mFishSongMgr->PlayFishSong(aSongData, aSongId);

	// One condition in the original: the "long" string is only built (and freed) when the first tests pass
	bool shouldAddApplause = aSong != nullptr && theSpecObjType != 4 && theSpecObjType != 5 && aSongData->GetProperty("long", nullptr);

	if (shouldAddApplause)
		aSong->mApplause = SOUND_APPLAUSE;

	return aSongId;
}

bool Sexy::Board::IsSongPlaying(int theSongId)
{
	return mFishSongMgr->IsSongInList(theSongId);
}

void Sexy::Board::FishSongMgrUpdate()
{
	mFishSongMgr->Update();
}

void Sexy::Board::StopFishSong(int theSongId)
{
	if (mFishSongMgr)
		mFishSongMgr->StopFishSong(theSongId);
}

void Sexy::Board::DoVirtualTankDialog()
{
	if (!mApp->IsScreenSaver() && mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		UpdateHasVirtualTankFish();
		if (!mHasVirtualTankFish)
			mApp->DoVirtualDialog();
	}
}

void Sexy::Board::GetExoticFoodsRequiredInTank(int* theInfoArray)
{
	memset(theInfoArray, 0, 8 * sizeof(int));
	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
		(*it)->AddRequiredFood(theInfoArray);
}

void Sexy::Board::GetExoticFoodsInTank(int* theInfoArray)
{
	memset(theInfoArray, 0, 8 * sizeof(int));
	for (std::vector<Fish*>::iterator it = mFishList->begin(); it != mFishList->end(); ++it)
	{
		Fish* anObj = *it;
		if (anObj->mVirtualTankId < 0 && anObj->mSize == SIZE_SMALL)
			theInfoArray[0]++;
	}
	for (std::vector<Oscar*>::iterator it = mOscarList->begin(); it != mOscarList->end(); ++it)
	{
		Oscar* anObj = *it;
		if (anObj->mVirtualTankId < 0)
			theInfoArray[3]++;
	}
	for (std::vector<Ultra*>::iterator it = mUltraList->begin(); it != mUltraList->end(); ++it)
	{
		Ultra* anObj = *it;
		if (anObj->mVirtualTankId < 0)
			theInfoArray[4]++;
	}
	for (std::vector<Coin*>::iterator it = mCoinList->begin(); it != mCoinList->end(); ++it)
	{
		Coin* anObj = *it;
		if (anObj->mCoinType == COIN_STAR)
			theInfoArray[1]++;
	}

	theInfoArray[2] = mLarvaList->size();

	for (std::vector<Food*>::iterator it = mFoodList->begin(); it != mFoodList->end(); ++it)
	{
		Food* anObj = *it;
		switch (anObj->mExoticFoodType)
		{
		case 3:
			theInfoArray[6]++;
			break;
		case 4:
			theInfoArray[7]++;
			break;
		case 5:
			theInfoArray[5]++;
			break;
		}
	}
}

// A separate function in the original (0x53A770), called by PetsScreen: removes the first object with this virtual tank id
void Sexy::Board::RemoveGameObjectByVirtualId(int theId)
{
	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mVirtualTankId == theId)
		{
			anObj->RemoveHelper02(false);
			mApp->SafeDeleteWidget(anObj);
			return;
		}
	}
}

GameObject* Sexy::Board::GetGameObjectByVirtualId(int theId)
{
	for (GameObjectSet::iterator it = mGameObjectSet.begin(); it != mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mVirtualTankId == theId)
			return anObj;
	}
	return nullptr;
}

void Sexy::Board::HideObject(GameObject* theObj, bool hide)
{
	if (theObj->mType == TYPE_BREEDER)
	{
		GameObject* anObj = GetGameObjectByVirtualId(theObj->mVirtualTankId + 100);
		if (anObj)
			HideObject(anObj, hide);
	}
	if (theObj->mShown != hide)
	{
		if (hide)
		{
			mGameObjectSet.erase(theObj);
			AddGameObject(theObj, true);
			mWidgetManager->AddWidget(theObj);
			MakeShadowForGameObject(theObj);
			theObj->mShown = true;
			m0x2a7 = true;
		}
		else
		{
			if (theObj->mSongId != -1)
				StopFishSong(theObj->mSongId);
			theObj->RemoveHelper02(false);
			mGameObjectSet.insert(theObj);
			theObj->mShown = false;
			m0x2a7 = true;
		}
	}
}

Fish* Sexy::Board::SpawnGuppy(int theX, int theY)
{
	Fish* aFish = new Fish(theX, theY);
	AddGameObject(aFish, true);
	mWidgetManager->AddWidget(aFish);
	if ((int)mFishList->size() > mMaxFishCountEver)
		mMaxFishCountEver = mFishList->size();
	MakeShadowForGameObject(aFish);
	SortGameObjects();
	return aFish;
}

GameObject* Sexy::Board::SpawnGuppyBought()
{
	// The original draws x before y
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Fish* aFish = new Fish(aX, aY);
	aFish->mVY = mApp->mSeed->Next() % 5 + 18;
	aFish->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	aFish->mYD = 30;
	aFish->mY = 30;
	AddGameObject(aFish, true);
	mWidgetManager->AddWidget(aFish);
	if ((int)mFishList->size() > mMaxFishCountEver)
		mMaxFishCountEver = mFishList->size();
	MakeShadowForGameObject(aFish);
	SortGameObjects();
	return aFish;
}

void Sexy::Board::SpawnStarGuppyBought()
{
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Fish* aFish = new Fish(aX, aY);
	aFish->mVY = mApp->mSeed->Next() % 5 + 18;
	aFish->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	aFish->mYD = 30;
	aFish->mY = 30;
	AddGameObject(aFish, true);
	mWidgetManager->AddWidget(aFish);
	if ((int)mFishList->size() > mMaxFishCountEver)
		mMaxFishCountEver = mFishList->size();
	aFish->mSize = SIZE_STAR;
	MakeShadowForGameObject(aFish);
	SortGameObjects();
}

void Sexy::Board::SpawnBreeder(int theX, int theY)
{
	Breeder* aBreeder = new Breeder(theX, theY);
	AddGameObject(aBreeder, true);
	mWidgetManager->AddWidget(aBreeder);
	MakeShadowForGameObject(aBreeder);
	SortGameObjects();
}

void Sexy::Board::SpawnBreederBought()
{
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Breeder* aBreeder = new Breeder(aX, aY);
	aBreeder->mVY = mApp->mSeed->Next() % 5 + 18;
	aBreeder->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	aBreeder->mYD = 30;
	aBreeder->mY = 30;
	AddGameObject(aBreeder, true);
	mWidgetManager->AddWidget(aBreeder);
	MakeShadowForGameObject(aBreeder);
	SortGameObjects();
}

GameObject* Sexy::Board::SpawnOscarBought()
{
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Oscar* anOscar = new Oscar(aX, aY);
	anOscar->mVY = mApp->mSeed->Next() % 5 + 23;
	anOscar->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	anOscar->mYD = 40;
	anOscar->mY = 40;
	AddGameObject(anOscar, true);
	mWidgetManager->AddWidget(anOscar);
	if ((int)mOscarList->size() > mMaxOscarCountEver)
		mMaxOscarCountEver = mOscarList->size();
	MakeShadowForGameObject(anOscar);
	SortGameObjects();
	return anOscar;
}

GameObject* Sexy::Board::SpawnUltraBought()
{
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Ultra* anUltra = new Ultra(aX, aY);
	anUltra->mVY = mApp->mSeed->Next() % 5 + 25;
	anUltra->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	anUltra->mYD = 40;
	anUltra->mY = 40;
	AddGameObject(anUltra, true);
	mWidgetManager->AddWidget(anUltra);
	if ((int)mUltraList->size() > mMaxUltraCountEver)
		mMaxUltraCountEver = mUltraList->size();
	MakeShadowForGameObject(anUltra);
	SortGameObjects();
	return anUltra;
}

void Sexy::Board::SpawnGekkoBought()
{
	int aX = mApp->mSeed->Next() % 520 + 20;
	int aY = mApp->mSeed->Next() % 265 + 105;
	Gekko* aGekko = new Gekko(aX, aY);
	aGekko->mVY = mApp->mSeed->Next() % 5 + 23;
	aGekko->mBoughtTimer = mApp->mSeed->Next() % 10 + 45; // drawn before the mYD/mY stores in the original
	aGekko->mYD = 40;
	aGekko->mY = 40;
	AddGameObject(aGekko, true);
	mWidgetManager->AddWidget(aGekko);
	MakeShadowForGameObject(aGekko);
	SortGameObjects();
}

void Sexy::Board::SpawnPentaBought()
{
	Penta* aPenta = new Penta(mApp->mSeed->Next() % 520 + 20);
	aPenta->mYD = 65;
	aPenta->mY = 65;
	AddGameObject(aPenta, true);
	mWidgetManager->AddWidget(aPenta);
	if ((int)mPentaList->size() > mMaxPentaCountEver)
		mMaxPentaCountEver = mPentaList->size();
	MakeShadowForGameObject(aPenta);
	SortGameObjects();
}

void Sexy::Board::SpawnGrubberBought()
{
	Grubber* aGrubber = new Grubber(mApp->mSeed->Next() % 520 + 20);
	aGrubber->mYD = 65;
	aGrubber->mY = 65;
	AddGameObject(aGrubber, true);
	mWidgetManager->AddWidget(aGrubber);
	MakeShadowForGameObject(aGrubber);
	SortGameObjects();
}

GameObject* Sexy::Board::SpawnPet(int thePetType, int theX, int theY, bool flag1, bool flag2)
{
	if (thePetType == PET_PRESTO)
		flag1 = true;
	if (theX == -1)
	{
		theX = mApp->mSeed->Next() % 265 + 105;
		theY = mApp->mSeed->Next() % 520 + 20;
	}
	GameObject* aPet = nullptr;
	if (thePetType == PET_STINKY || thePetType == PET_NIKO ||
		thePetType == PET_RUFUS || thePetType == PET_CLYDE || thePetType == PET_RHUBARB)
		aPet = new OtherTypePet(theX, theY, thePetType, mCurrentBackgroundId, flag1);
	else
		aPet = new FishTypePet(theX, theY, thePetType, flag1);
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && !flag2)
	{
		aPet->mVirtualTankId = (flag1 ? PET_PRESTO : thePetType) + 1000;
	}
	AddGameObject(aPet, true);
	mWidgetManager->AddWidget(aPet);
	MakeShadowForGameObject(aPet);
	SortGameObjects();
	return aPet;
}

void Sexy::Board::SpawnLarva(int theX, int theY)
{
	Larva* aLarva = new Larva(theX, theY);
	AddGameObject(aLarva, false);
	mWidgetManager->AddWidget(aLarva);
	SortGameObjects();
}

void Sexy::Board::SpawnAlien(int theType, bool unk)
{
	// The original draws x before y
	int aX = mApp->mSeed->Next() % 450 + 20;
	int aY = mApp->mSeed->Next() % 195 + 105;
	SpawnAlien(theType, aX, aY, unk);
}

void Sexy::Board::SpawnAlien(int theType, int theX, int theY, bool unk)
{
	// Checked by CanAlienChaseAnyFish: in relax mode aliens leave the fish alone for a while after spawning
	gUnkInt03 = mApp->mUpdateCount - 216;
	if (theType == 9)
	{
		SpawnAlien(ALIEN_WEAK_SYLV, theX, theY, unk);
		SpawnAlien(ALIEN_BALROG, mCrosshair2X, mCrosshair2Y, unk);
	}
	else if (theType == 10)
	{
		SpawnAlien(ALIEN_PSYCHOSQUID, theX, theY, unk);
		SpawnAlien(ALIEN_BALROG, mCrosshair2X, mCrosshair2Y, unk);
	}
	else if (theType == 11)
	{
		SpawnAlien(ALIEN_DESTRUCTOR, theX, theY, unk);
		SpawnAlien(ALIEN_ULYSEES, mCrosshair2X, mCrosshair2Y, unk);
	}
	else if (theType == 12)
	{
		SpawnAlien(ALIEN_BALROG, theX, theY, unk);
		SpawnAlien(ALIEN_BILATERUS, mCrosshair2X, mCrosshair2Y, unk);
	}
	else if (theType == ALIEN_BILATERUS)
	{
		Bilaterus* aBil = new Bilaterus(theX, theY);
		aBil->SpawnWarp();
		mWidgetManager->AddWidget(aBil);
		AddGameObject(aBil, true);
	}
	else if (theType == ALIEN_CYRAX)
	{
		mShouldSave = true;
		Alien* anAlien = new Alien(theX, theY, ALIEN_CYRAX);
		anAlien->SpawnWarp();
		mCyraxPtr = anAlien;
		mWidgetManager->AddWidget(anAlien);
		MakeShadowForGameObject(anAlien);
		m0x3e4 = 10;
	}
	else
	{
		Alien* anAlien = new Alien(theX, theY, theType);
		anAlien->SpawnWarp();
		AddGameObject(anAlien, true);
		mWidgetManager->AddWidget(anAlien);
		MakeShadowForGameObject(anAlien);
	}
	Unk14(false);
	SortGameObjects();
	if (unk)
	{
		if (theType == ALIEN_GUS)
			mApp->PlaySample(SOUND_GUFFAW);
		else if(theType == ALIEN_DESTRUCTOR)
			mApp->PlaySample(SOUND_INTERFER);
		else if(theType == ALIEN_ULYSEES)
			mApp->PlaySample(SOUND_ROAR2);
		else if(theType == ALIEN_PSYCHOSQUID)
			mApp->PlaySample(SOUND_ROAR3);
		else if(theType == ALIEN_CYRAX)
			mApp->PlaySample(SOUND_EVILLAFF);
		else if(theType == ALIEN_WEAK_SYLV || theType == ALIEN_STRONG_SYLV || theType == ALIEN_BALROG)
			mApp->PlaySample(SOUND_ROAR);
	}
	return;
}

void Sexy::Board::SpawnShot(int theX, int theY, int theType)
{
	if (mShotList->size() >= 20)
		return;

	Shot* aShot = new Shot(theX, theY, theType);
	AddGameObject(aShot, false);
	mWidgetManager->AddWidget(aShot);
	mWidgetManager->BringToFront(aShot);
	SortGameObjects();
}

void Sexy::Board::SpawnLaserShot(int theX, int theY)
{
	Shot* aShot = new Shot(theX, theY);
	AddGameObject(aShot, false);
	mWidgetManager->AddWidget(aShot);
	mWidgetManager->BringToFront(aShot);
	SortGameObjects();
	m0x3d0 = theX + 40;
	m0x3d4 = theY + 40;
}

void Sexy::Board::SpawnMissle(int theX, int theY, GameObject* theTarget, int theType)
{
	Missle* aMissle = new Missle(theX, theY, theTarget, theType);
	AddGameObject(aMissle, false);
	mWidgetManager->AddWidget(aMissle);
	mWidgetManager->BringToFront(aMissle);
	mWidgetManager->BringToFront(mBoardOverlay2);
	if (!aMissle->IsTargetless())
		theTarget->mMisslePtr = aMissle;
}

void Sexy::Board::SpawnShadow(int theSize, GameObject* theObject)
{
	if (mShadowList->size() >= 40)
		return;

	Shadow* aShad = new Shadow(theSize, theObject);
	aShad->mVisible = mApp->Is3DAccelerated();
	AddGameObject(aShad, false);
	mWidgetManager->AddWidget(aShad);
	SortGameObjects();
}

void Sexy::Board::RessurectFish(int theX, int theY, int theSize, bool flipped)
{
	Fish* aFish = new Fish(theX, theY, theSize, flipped);
	AddGameObject(aFish, true);
	mWidgetManager->AddWidget(aFish);
	if ((int)mFishList->size() > mMaxFishCountEver)
		mMaxFishCountEver = mFishList->size();
	MakeShadowForGameObject(aFish);
	SortGameObjects();
}

void Sexy::Board::RessurectOscar(int theX, int theY, bool flipped)
{
	Oscar* anOscar = new Oscar(theX, theY, flipped);
	AddGameObject(anOscar, true);
	mWidgetManager->AddWidget(anOscar);
	if ((int)mOscarList->size() > mMaxOscarCountEver)
		mMaxOscarCountEver = mOscarList->size();
	MakeShadowForGameObject(anOscar);
	SortGameObjects();
}

void Sexy::Board::RessurectUltra(int theX, int theY, bool flipped)
{
	Ultra* anUltra = new Ultra(theX, theY, flipped);
	AddGameObject(anUltra, true);
	mWidgetManager->AddWidget(anUltra);
	MakeShadowForGameObject(anUltra);
	SortGameObjects();
}

void Sexy::Board::RessurectGekko(int theX, int theY, bool flipped)
{
	Gekko* aGekko = new Gekko(theX, theY, flipped);
	AddGameObject(aGekko, true);
	mWidgetManager->AddWidget(aGekko);
	MakeShadowForGameObject(aGekko);
	SortGameObjects();
}

void Sexy::Board::RessurectPenta(int theX)
{
	Penta* aPenta = new Penta(theX);
	AddGameObject(aPenta, true);
	mWidgetManager->AddWidget(aPenta);
	if ((int)mPentaList->size() > mMaxPentaCountEver)
		mMaxPentaCountEver = mPentaList->size();
	MakeShadowForGameObject(aPenta);
	SortGameObjects();
}

void Sexy::Board::RessurectGrubber(int theX)
{
	Grubber* aGrubber = new Grubber(theX);
	AddGameObject(aGrubber, true);
	mWidgetManager->AddWidget(aGrubber);
	MakeShadowForGameObject(aGrubber);
	SortGameObjects();
}

void Sexy::Board::RessurectBreeder(int theX, int theY, int theSize, bool flipped)
{
	Breeder* aBreeder = new Breeder(theX, theY, theSize, flipped);
	AddGameObject(aBreeder, true);
	mWidgetManager->AddWidget(aBreeder);
	MakeShadowForGameObject(aBreeder);
	SortGameObjects();
}

void Board::DrawOverlay1(Graphics* g)
{
	mBubbleMgr->Draw(g);
	if (!mApp->IsScreenSaver())
		return;

	g->SetColor(Color(0));
	g->FillRect(0, 0, 640, 60);
	g->FillRect(0, 445, 640, 35);
	SexyString aWarning = "Warning: Insaniquarium is running.  Shells collected here may not be saved.";
	g->SetFont(FONT_CONTINUUMBOLD14);
	g->SetColor(Color(0xffffff));
	int aStrWdth = g->GetFont()->StringWidth(aWarning);
	if (mApp->mScreenSaverUnk01)
	{
		// Two calls in the original (tail-merged), odd seconds first
		if ((mUpdateCnt / 1000 % 2) != 0)
			DrawStringWithOutline(g, aWarning, 320 - aStrWdth / 2, 465, FONT_CONTINUUMBOLD14OUTLINE, 0x29558c);
		else
			DrawStringWithOutline(g, aWarning, 320 - aStrWdth / 2, 40, FONT_CONTINUUMBOLD14OUTLINE, 0x29558c);
	}
	g->SetFont(FONT_CONTINUUMBOLD14);

	SexyString aShellsCollectedStr = StrFormat("%d Shells Collected", m0x400);
	aStrWdth = g->GetFont()->StringWidth(aShellsCollectedStr);
	int aStrY; // not initialised in the original (the switch covers every value)
	int aStrX;
	switch (mUpdateCnt / 1000 % 4)
	{
	case 0:
		aStrX = 630 - aStrWdth;
		aStrY = 468;
		break;
	case 1:
		aStrX = 630 - aStrWdth;
		aStrY = 40;
		break;
	case 2:
		aStrX = 10;
		aStrY = 468;
		break;
	case 3:
		aStrX = 10;
		aStrY = 40;
		break;
	}
	if (mApp->mScreenSaverShowMoney)
	{
		g->SetFont(FONT_CONTINUUMBOLD14);
		g->SetColor(Color(0xffffff));
		DrawStringWithOutline(g, aShellsCollectedStr, aStrX, aStrY, FONT_CONTINUUMBOLD14OUTLINE, 0x29558c);
	}
	if (mApp->mScreenSaverPeriodicDim)
	{
		int aTimer = mUpdateCnt % 3000;
		if (aTimer >= 1320 && aTimer <= 1680)
		{
			int anAlpha = 200;
			if (aTimer <= 1350)
			{
				anAlpha = InterpolateInt(0, 200, aTimer - 1320, 30, false);
			}
			else if (aTimer >= 1650)
			{
				anAlpha = InterpolateInt(200, 0, aTimer - 1650, 30, false);
			}
			g->SetColor(Color(0, 0, 0, anAlpha));
			g->FillRect(0, 60, 640, 385);
		}
	}
}

void Sexy::Board::DrawTankBackground(Graphics* g)
{
	if (mApp->mCurrentProfile->mCheatCodeFlags >> 2 & 1)
	{
		g->SetColor(Color(0xffffff));
		g->FillRect(0, 0, 640, 480);
		return;
	}
	if (mApp->mCurrentProfile->mCheatCodeFlags >> 3 & 1)
	{
		mStarField->Draw(g, mApp->mCurrentProfile->mCheatCodeFlags & 1);
		return;
	}
	int aRandTransX = 0;
	int aRandTransY = 0;
	if (m0x2b4 != 0 && m0x2b4 < 20 && !mPause)
	{
		aRandTransX = rand() % 5 - 2;
		aRandTransY = rand() % 5 - 2;
		g->SetColor(Color(0));
		g->FillRect(0, 0, 640, 480);
		g->Translate(aRandTransX, aRandTransY);
	}
	Image* anAquariumImg = GetImageById(mCurrentBackgroundId + IMAGE_AQUARIUM1_ID - 1);
	g->DrawImage(anAquariumImg, 0, 0);
	// The profile is read again here (the original reloads it from mApp after the draws)
	if (mApp->mCurrentProfile->mCheatCodeFlags & 1)
	{
		float aVal;
		if (mApp->Is3DAccelerated())
			aVal = 1.2f;
		else
			aVal = 2.0f;

		Unk06(g, anAquariumImg, 0, 0, aVal);
	}

	DrawTankWaves(g, 88, mGameUpdateCnt);

	if (mApp->Is3DAccelerated())
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, 40));
		g->DrawImage(IMAGE_TANKLIGHTING, mTankLightingSpeeds[0], 365);
		g->DrawImage(IMAGE_TANKLIGHTING, mTankLightingSpeeds[0] - 640.0, 365);

		g->SetColor(Color(255, 255, 255, 100));
		g->DrawImage(IMAGE_TANKLIGHTING, mTankLightingSpeeds[1], 365);
		g->DrawImage(IMAGE_TANKLIGHTING, mTankLightingSpeeds[1] - 640.0, 365);

		g->SetColor(Color(255, 255, 255, 70));
		g->DrawImageMirror(IMAGE_TANKLIGHTING, mTankLightingSpeeds[2], 365, true);
		g->DrawImageMirror(IMAGE_TANKLIGHTING, mTankLightingSpeeds[2] - 640.0, 365, true);
		g->SetColorizeImages(false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);

		// An if-chain in the original (cmp/jne per value, no jump table)
		if (mCurrentBackgroundId == 1)
			g->DrawImage(IMAGE_TANKMASK1, 0, 365);
		else if (mCurrentBackgroundId == 2)
			g->DrawImage(IMAGE_TANKMASK2, 0, 365);
		else if (mCurrentBackgroundId == 3)
			g->DrawImage(IMAGE_TANKMASK3, 0, 365);
		else if (mCurrentBackgroundId == 4)
			g->DrawImage(IMAGE_TANKMASK4, 0, 365);
		else if (mCurrentBackgroundId == 5)
			g->DrawImage(IMAGE_TANKMASK5, 0, 365);
		else if (mCurrentBackgroundId == 6)
			g->DrawImage(IMAGE_TANKMASK6, 0, 365);
	}
	g->Translate(-aRandTransX, -aRandTransY);
}

void Sexy::Board::DrawBonusRound(Graphics* g)
{
	int aBonusTimer = mGameUpdateCnt - m0x2a8; // ivar11
	Color aColor = g->GetColor();//loc1a4, 1a0, 19c, 198

	Point aTarget = GetBonusShellTarget();
	int anXBucketPos = aTarget.mX; // loc16c
	int anYBucketPos = aTarget.mY; // loc168
	if (!mBonusRoundStarted)
	{
		if (aBonusTimer >= 130)
		{
			if (aBonusTimer < 150)
			{
				// The progress is stored as a float; the product is taken in double, as the original
				float aProgress = (aBonusTimer - 130) / 20.0;
				aProgress = aProgress * aProgress;
				g->DrawImage(IMAGE_BONUSBUCKET, anXBucketPos, (int)((double)(anYBucketPos + 5) * aProgress));
			}
			else
			{
				int anYBucketPos2 = anYBucketPos;
				if (aBonusTimer < 152)
				{
					float aProgress = (aBonusTimer - 150) * 0.5;
					anYBucketPos2 = (int)((double)(anYBucketPos + 5) - aProgress * 5.0);
				}
				g->DrawImage(IMAGE_BONUSBUCKET, anXBucketPos, anYBucketPos2);
			}
		}
	}
	else
	{
		g->DrawImage(IMAGE_BONUSBUCKET, anXBucketPos, anYBucketPos);
		if (m0x3f8 >= 60 || (m0x3f8 / 10) % 2 == 0)
		{
			g->SetFont(FONT_CONTINUUMBOLD14);
			SexyString aStr = StrFormat("%d", m0x3f4);
			int aStrWdth = g->GetFont()->StringWidth(aStr);
			g->SetColor(Color(0xf0a33b));
			DrawStringWithOutline(g, aStr, (anXBucketPos - aStrWdth / 2) + 55, anYBucketPos + 151, FONT_CONTINUUMBOLD14OUTLINE, 0x7b2000);
		}
	}

	if (!mBonusRoundStarted || aBonusTimer < 20)
	{
		int anAlphaVal = 255;
		if (mBonusRoundStarted)
			anAlphaVal = 255 - (aBonusTimer * 255) / 20;
		else if (aBonusTimer < 10)
			anAlphaVal = (aBonusTimer * 255) / 10;
		g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
		g->SetColor(Color(255, 255, 0, anAlphaVal));

		// Both widths are measured on temporary strings (built and freed around each StringWidth call)
		int aStrWdth = g->GetFont()->StringWidth("BONUS ROUND");
		int aCenteredStrX = (mWidth - aStrWdth) / 2;
		if (!mBonusRoundStarted && aBonusTimer < 8)
			aCenteredStrX += ((8 - aBonusTimer) * aStrWdth * -2) / 8;
		DrawCoolBonusString(g, "BONUS ROUND", aCenteredStrX, 235);
		g->SetFont(FONT_CONTINUUMBOLD14);
		g->SetColor(Color(180, 250, 90, anAlphaVal));

		aStrWdth = g->GetFont()->StringWidth("Collect as many shells as you can!");
		aCenteredStrX = (mWidth - aStrWdth) / 2;
		int anXStrOffset = 0;
		if (!mBonusRoundStarted && aBonusTimer < 8)
			anXStrOffset = ((8 - aBonusTimer) * aStrWdth * 2) / 8;

		{
			// A third copy of the text, built before the outline colour is computed and freed right after the call
			SexyString aStr = "Collect as many shells as you can!";
			DrawStringWithOutline(g, aStr, aCenteredStrX + anXStrOffset, 260, FONT_CONTINUUMBOLD14OUTLINE, Color(0, 75, 0, anAlphaVal).ToInt());
		}

		int anInterpolatedVal = (mBonusRoundStarted ? 0 : InterpolateInt(520, 0, aBonusTimer, 16, false)) + 280;

		g->SetColorizeImages(true);
		#define NUM_OF_SHELLS 5
		int aShellValues[NUM_OF_SHELLS] = { 1,2,5,10,20 };
		int aShellYOffsets[NUM_OF_SHELLS] = { 0,0,0,0,-12 };
		g->SetFont(FONT_CONTINUUMBOLD12OUTLINE);
		for (int i = 0; i < NUM_OF_SHELLS; i++)
		{
			Image* anImg = IMAGE_SHELLS;
			int aRow = i;
			if (i == 4)
			{
				anImg = IMAGE_MONEYBAG;
				aRow = 0;
			}

			// The value string is formatted before anything is drawn
			SexyString aShellValStr = StrFormat("%d", aShellValues[i]);

			g->SetColor(Color(255,255,255,anAlphaVal));
			g->DrawImageCel(anImg, i * 60 + aCenteredStrX, aShellYOffsets[i] + anInterpolatedVal, 0, aRow);

			g->SetColor(Color(255, 255, 255, anAlphaVal));
			int aStrWdth = g->GetFont()->StringWidth(aShellValStr);
			int aStrY = anInterpolatedVal + 50;
			g->DrawString(aShellValStr, (28 - aStrWdth) / 2 + i * 60 + aCenteredStrX, aStrY);
		}
		// The original leaves colorizing on here; the countdown below turns it off
		int anImgId = (148 - aBonusTimer) / 36;
		int ivar11 = (148 - aBonusTimer) % 36;
		// One unsigned compare in the original (cmp 2, ja)
		if (!mBonusRoundStarted && aBonusTimer < 148 && (unsigned int)anImgId <= 2)
		{
			Image* aNumImg = GetImageById(anImgId + IMAGE_BONUS1_ID);
			int aScaleTimer = 36 - ivar11; // loc150
			int ivar9 = aScaleTimer - 5;
			int anAlphaCnt = ivar11; // loc158
			if (ivar11 > 20)
				anAlphaCnt = 20;
			if (ivar9 < 0)
				ivar9 = 0;
			else if (ivar9 > 10)
				ivar9 = 10;
			if (aScaleTimer > 5)
				aScaleTimer = 5;

			// As the original: 0.8f and 0.2f in double arithmetic, the scale and the height stored as floats
			float aScale = (double)aScaleTimer * 0.8f / 5.0 + 0.2f;
			int aScaledWidth = (int)((double)aNumImg->mWidth * aScale);
			float aHeightF = (float)aNumImg->mHeight;
			int aScaledHeight = (int)((double)aHeightF * aScale);
			int aHalfScaledHeight = (int)((double)aHeightF * (aScale - 1.0) * 0.5);

			// The image size and the board width are read again after SetFastStretch, as the original does
			g->SetFastStretch(!mApp->Is3DAccelerated());
			Rect aSrcRect = Rect(0, 0, aNumImg->mWidth, aNumImg->mHeight);
			Rect aDestRect = Rect((mWidth - aScaledWidth) / 2, 90 - aHalfScaledHeight, aScaledWidth, aScaledHeight);
			g->SetColorizeImages(true);
			g->SetColor(Color(255, 255, 255, (anAlphaCnt*255) / 20));
			DrawImageMirrorHelper(g, aNumImg, aDestRect, aSrcRect, false);
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->SetColor(Color(255, 255, 255, ivar9 * anAlphaCnt * 255 / 200 / 2));
			DrawImageMirrorHelper(g, aNumImg, aDestRect, aSrcRect, false);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
			g->SetColorizeImages(false);
		}
	}
	g->SetFont(FONT_CONTINUUMBOLD12);
	g->SetColor(aColor);
	int aTimeInSecs = m0x3bc;
	if (mBonusRoundStarted)
		aTimeInSecs -= m0x3b0;

	int aMins = aTimeInSecs / 60;
	int aSecs = aTimeInSecs % 60;
	if (aMins < 0)
		aMins = 0;
	if (aSecs < 0)
		aSecs = 0;

	// sprintf into a stack buffer in the original (not StrFormat); DrawString gets a temporary string
	char aTimeRStr[256];
	sprintf(aTimeRStr, "Time Remaining: %d:%02d", aMins, aSecs);

	g->DrawString(aTimeRStr, 465, 470);
}

void Sexy::Board::DrawCoolBonusString(Graphics* g, const char* theString, int theX, int theY)
{
	double aWobbleBase = mGameUpdateCnt * 0.25;
	char aPrevChar = 0;
	for (int i = 0; theString[i] != 0; i++)
	{
		char aChar = theString[i];

		// The one-character string is built before the wobble is computed, as in the original
		SexyString aCharStr;
		aCharStr += aChar;

		// As the original: 3.14159f, angle and sine stored as floats, the CRT double sin, y summed in double
		float anAngle = ((double)i + aWobbleBase) * 35.0 * 3.14159f / 180.0;
		float aSinVal = (float)sin((double)anAngle);

		int aWobbleY = (int)(aSinVal * 5.0 + (float)theY);

		g->DrawString(aCharStr, theX, aWobbleY);

		int aCharWidth = g->GetFont()->CharWidthKern(aChar, aPrevChar);
		theX += aCharWidth;

		aPrevChar = aChar;
	}
}

BoardOverlay::BoardOverlay(Board* theBoard, int thePriority)
{
	mBoard = theBoard;
	mPriority = thePriority;
	mHasAlpha = true;
	mMouseVisible = false;
	mWidth = 640;
	mHeight = 480;
}

BoardOverlay::~BoardOverlay()
{
}

void BoardOverlay::Draw(Graphics* g)
{
	mBoard->DrawOverlay(g, mPriority);
}
