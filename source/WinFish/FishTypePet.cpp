#include <SexyAppFramework/WidgetManager.h>

#include "FishTypePet.h"
#include "WinFishApp.h"
#include "Board.h"
#include "BilaterusHead.h"
#include "Shadow.h"
#include "Missle.h"
#include "Coin.h"
#include "Breeder.h"
#include "DeadFish.h"
#include "Food.h"
#include "BoxingGlove.h"
#include "Res.h"

int gConst01 = 360; // 0x5DF034: a writable .data global in the original, read from memory at each use

Sexy::FishTypePet::FishTypePet()
{
	mClip = false;
	m0x245 = false;
	m0x24c = nullptr;
	mType = TYPE_FISH_TYPE_PET;
}

Sexy::FishTypePet::FishTypePet(int theX, int theY, int thePetType, bool flag)
	: Fish(theX, theY)
{
	mClip = false;
	m0x245 = false;
	mFishTypePetType = thePetType;
	mType = TYPE_FISH_TYPE_PET;
	m0x230 = flag;
	m0x248 = 0;
	if (!flag || thePetType == PET_PRESTO)
		m0x238 = 0;
	else
		m0x238 =  360;

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		m0x238 = 0;

	m0x23c = 0;
	m0x258 = 0;
	m0x240 = 0;
	m0x264 = 0;
	mIsGuppy = false;
	m0x250 = false;
	mEatingAnimationTimer = 0;
	m0x260 = 0;
	m0x24c = nullptr;
	m0x244 = true;
	// Read here and kept for Gash's start timer below, as the original
	bool aNotVirtualTank = mApp->mGameMode != GAMEMODE_VIRTUAL_TANK;
	mMouseVisible = gUnkBool06;

	// The first six are one chain on the parameter; the rest are separate tests of the member, as the original
	if (thePetType == PET_WALTER)
	{
		mMouseVisible = true;
		mDoFinger = true;
		mMouseInsets.mTop = 10;
		mMouseInsets.mBottom = 15;
	}
	else if (thePetType == PET_PREGO)
	{
		mCoinDropT = 930;
		mYMax = 360;
	}
	else if (thePetType == PET_ZORF)
	{
		mCoinDropT = 65;
		mSpeedMod = 3.0;
		mYMax = 270;
	}
	else if (thePetType == PET_MERYL)
	{
		mCoinDropT = mApp->mGameMode != GAMEMODE_VIRTUAL_TANK ? 1400 : 4320;
	}
	else if (thePetType == PET_WADSWORTH)
	{
		mCoinDropT = 120;
		mSpeedMod = 4.0;
	}
	else if (thePetType == PET_VERT)
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			mCoinDropT = Rand() % 200 + 1080;
		else
			mCoinDropT = 216;
	}

	if (mFishTypePetType == PET_STANLEY)
		mCoinDropT = 100;
	if (mFishTypePetType == PET_NOSTRADAMUS)
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			mCoinDropT = Rand() % 200 + 1080;
		else
			mCoinDropT = mApp->mSeed->Next() % 300 + 300;
	}
	if (mFishTypePetType == PET_SHRAPNEL)
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			mCoinDropT = Rand() % 200 + 1080;
		else
			mCoinDropT = mApp->mSeed->Next() % 20 + 633;
	}
	if (mFishTypePetType == PET_NIMBUS)
	{
		mSpeedMod = 0.5;
		mYMin = 320;
	}
	if (mFishTypePetType == PET_AMP)
	{
		mSpeedMod = 0.5;
		mCoinDropT = 3000;
		mWidth = 160;
		mHeight = 80;
		mXMax = 460;
		mCoinDropTimer = 300;
		mMouseInsets.mTop = 0;
		mMouseInsets.mBottom = 25;
	}
	if (mFishTypePetType == PET_GASH)
	{
		mCoinDropT = 1570;
		mCoinDropTimer = aNotVirtualTank ? -1550 : 0;
		if (m0x230)
			mCoinDropTimer = 1520;
	}
}

Sexy::FishTypePet::~FishTypePet()
{
	WadsworthAndMerylFunc01();
	if (m0x24c)
		delete m0x24c;
}

void Sexy::FishTypePet::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	GameObject::UpdateCounters();

	if (mFishTypePetType == PET_BLIP && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
	{
		// The board is read here and kept across the calls, as the original
		Board* aBoard = mApp->mBoard;
		if (aBoard->m0x2e0 < aBoard->m0x2dc && aBoard->mTank != 5 && aBoard->mGameUpdateCnt > 200
			&& (!m0x230 || mUpdateCnt > 720))
		{
			aBoard->PlaySample(SOUND_SONAR_ID, 3, 1.0);
			for (int i = 0; i < SlotTypes::SLOT_END; i++)
				aBoard->MakeAndUnlockMenuButton(i, true);
		}
	}

	if (m0x24c != nullptr)
	{
		m0x24c->Update();
		if (m0x24c->m0x15c <= 0)
		{
			if(m0x24c)
				delete m0x24c;
			m0x24c = nullptr;
		}
	}

	if (m0x264 != 0)
	{
		m0x264--;
		if (mFishTypePetType == PET_WALTER && m0x264 == 0)
			mDoFinger = true;
	}

	if (m0x23c != 0)
		m0x23c--;

	if (mApp->mBoard->mTank == 5 || !Hungry())
	{ // 72
		// States above 4 are tested first, and -1 is the only negative one handled, as the original
		if (mMovementState > 4)
		{
			if (mYD < 115)
				mVY = -0.1;
			else
				mVY = -0.5;

			if (mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;

				if (mXDirection == 1)
				{
					if (mVX >= 0.0)
						mVX += 1.0;
					else
						mVX += 2.0;

					mVXAbs = (int)abs(mVX);
					if (mXD > 250.0)
					{
						mXDirection = -1;
						mVX -= 2.0;
					}
				}
				else if (mXDirection == -1)
				{
					if (mVX <= 0.0)
						mVX -= 1.0;
					else
						mVX -= 2.0;

					mVXAbs = (int)abs(mVX);
					if (mXD < 175.0)
					{
						mXDirection = 1;
						mVX += 2.0;
					}
				}
			}
		}
		else if (mMovementState == 0)
		{
			if (m0x264 <= 0 || mFishTypePetType != PET_WALTER)
				mVY = 0.5;
			else
				mVY = 1.0;

			if(mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;
				if (mVX > 0.0)
				{
					if (mVX > 0.5)
						mVX -= 0.5;
				}
				else if (mVX < 0.0 && mVX < -0.5)
					mVX += 0.5;
				mVXAbs = (int)abs(mVX);
			}
			mYD -= 0.25 / mSpeedMod;
		}
		else if (mMovementState == 1)
		{
			mVY = -0.5;
			if (mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;

				if (mVX > 1.0)
					mVX -= 1.0;
				else if (mVX < 1.0)
					mVX += 1.0;
				mVXAbs = (int)abs(mVX);
			}
			mYD -= 0.5 / mSpeedMod;
		}
		else if (mMovementState == 2)
		{
			mVY = -0.5;
			if (mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;
				if (mVX > -1.0)
					mVX -= 1.0;
				else if (mVX < -1.0)
					mVX += 1.0;
				mVXAbs = (int)abs(mVX);
			}
			mYD -= 0.5 / mSpeedMod;
		}
		else if (mMovementState == 3)
		{
			if (mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;
				if (mVX > -1.0)
					mVX -= 1.0;
				else if (mVX < -1.0)
					mVX += 1.0;

				if (mVY > 3.0)
					mVY -= 1.0;
				else if (mVY < 3.0)
					mVY += 1.0;

				if (mVXAbs > 4)
					mVXAbs--;
				else if (mVY < 4.0)
					mVXAbs++;
			}

			if (mYD > 240.0)
				mMovementState = 0;
		}
		else if (mMovementState == 4)
		{
			if (mSpecialMovementStateChangeTimer >= 40)
			{
				mSpecialMovementStateChangeTimer = 0;

				if (mVX > 1.0)
					mVX -= 1.0;
				else if (mVX < 1.0)
					mVX += 1.0;

				if (mVY > 3.0)
					mVY -= 1.0;
				else if (mVY < 3.0)
					mVY += 1.0;

				if (mVXAbs > 4)
					mVXAbs--;
				else if (mVY < 4.0)
					mVXAbs++;
			}

			if (mYD > 240.0)
				mMovementState = 0;
		}
		else if (mMovementState == -1)
			mVY = -10;
	}

	mSpecialMovementStateChangeTimer++;
	mMovementStateChangeTimer++;

	if (mMovementStateChangeTimer > 20 || (mFishTypePetType == PET_NOSTRADAMUS && mYMax - 50 < mYD))
	{
		mMovementStateChangeTimer = 0;
		if (mApp->mSeed->Next() % 10 == 0 && (m0x264 == 0 || mFishTypePetType != PET_WALTER))
			mMovementState = mApp->mSeed->Next() % 9 + 1;
	}

	if ((!mApp->mBoard->AliensInTank() && mApp->mBoard->mBilaterusList->empty()) ||
		(mFishTypePetType == PET_STANLEY && (mApp->mBoard->AliensInTank() || !mApp->mBoard->mBilaterusList->empty())))
		DropCoin();

	if (mWadsworthVXModCounter != 0)
	{
		mWadsworthVXModCounter--;
		mUnusedVXWadsworthAddon *= 0.9;
		mXD += mUnusedVXWadsworthAddon;

		if (m0x230 && mWadsworthVXModCounter <= 40 && mFishTypePetType != PET_PRESTO)
		{ // 280
			m0x238 = 0;
			PrestoMorph(PET_PRESTO);
			return;
		}
	}

	if (mUnusedTimer > 0)
		mUnusedTimer--;

	// Two separate tests of the type, as the original
	if (mFishTypePetType != PET_AMP)
	{
		if (mVX == 0.0)
			mYD += 1.0 / mSpeedMod;
		if (mVX == 1.0)
			mYD += 0.75 / mSpeedMod;
		if (mVX == 2.0)
			mYD += 0.5 / mSpeedMod;
		if (mVX == 3.0)
			mYD += 0.25 / mSpeedMod;
	}
	if (mFishTypePetType == PET_AMP)
	{
		if (mVY > 0.5)
			mVY = 0.5;
		else if (mVY < -0.5)
			mVY = -0.5;
	}

	if (m0x238 != 0)
		m0x238--;

	if ((m0x230 || mFishTypePetType == PET_NOSTRADAMUS) && m0x248 < 20)
		m0x248++;

	// Each test repeats the type, and 2 and 3 are separate tests adding the same, as the original
	if (mFishTypePetType == PET_BRINKLEY && abs(mVX) <= 0.0)
		mYD += 1.0;
	else if (mFishTypePetType == PET_BRINKLEY && abs(mVX) <= 1.0)
		mYD += 0.75;
	else if (mFishTypePetType == PET_BRINKLEY && abs(mVX) <= 2.0)
		mYD += 0.5;
	else if (mFishTypePetType == PET_BRINKLEY && abs(mVX) <= 3.0)
		mYD += 0.5;

	if (mXD > mXMax)
		mXD = mXMax;
	if (mXD < mXMin)
		mXD = mXMin;

	if(mYD > mYMax && m0x230 && mFishTypePetType == PET_ZORF)
		mMovementState = 2;
	else if (mYD > mYMax)
			mYD = mYMax;

	if (mYD < mYMin && m0x230 && mFishTypePetType == PET_NIMBUS)
	{
		mMovementState = 0;
		mVY += 0.25;
	}
	else if (mYD < mYMin)
			mYD = mYMin;

	if (mYD <= mYMin && mMovementState == -1)
		mMovementState = mApp->mSeed->Next() % 9 + 1;

	if (mXMax - 5 < mXD && mVX > 0.1)
		mVX -= 0.1;
	if (mXD < 15.0 && mVX < -0.1)
		mVX += 0.1;

	FishUpdateAnimation();
	if (m0x245)
	{
		mXD += mVX / (mSpeedMod * 5.0);
		mYD += mVY / (mSpeedMod * 5.0);
	}
	else
	{
		mXD += mVX / mSpeedMod;
		mYD += mVY / mSpeedMod;
	}
	Move(mXD, mYD);
}

void Sexy::FishTypePet::MouseDown(int x, int y, int theClickCount)
{
	if (!HandleMouseDown(x, y, theClickCount) && !gUnkBool08)
	{
		gUnkBool08 = true;
		mApp->mBoard->MouseDown(mX + x, mY + y, theClickCount);
		gUnkBool08 = false;
	}
}

void Sexy::FishTypePet::CountRequiredFood(int* theFoodReqPtr)
{
	if (mFishTypePetType == PET_GASH || mFishTypePetType == PET_AMP)
		theFoodReqPtr[0]++;
}

void Sexy::FishTypePet::PrestoMorph(int thePetId)
{
	if (thePetId == mFishTypePetType || !PrestoRightClicked(m0x238))
		return;

	mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
	GameObject* aPet = mApp->mBoard->SpawnPet(thePetId, mX, mY, true, false);
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();

	if ((mFishTypePetType == PET_WALTER && thePetId == PET_PRESTO) ||
		(mFishTypePetType == PET_PRESTO && thePetId == PET_WALTER))
	{
		FishTypePet* aFishPet = (FishTypePet*)aPet;
		aFishPet->m0x264 = m0x264;
		aFishPet->m0x260 = m0x260;
	}
	WadsworthAndMerylFunc01();
}

void Sexy::FishTypePet::Remove()
{
	RemoveFishTypePet();
}

void Sexy::FishTypePet::RemoveFishTypePet()
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();
}

void Sexy::FishTypePet::Sync(DataSync* theSync)
{
	Fish::Sync(theSync);
	theSync->SyncBool(m0x230);
	theSync->SyncLong(mFishTypePetType);
	theSync->SyncLong(m0x23c);
	theSync->SyncLong(m0x240);
	theSync->SyncBool(m0x244);
	theSync->SyncBool(m0x250);
	theSync->SyncDouble(m0x258);
	theSync->SyncLong(m0x248);
	theSync->SyncLong(m0x238);
	theSync->SyncLong(m0x260);
	theSync->SyncLong(m0x264);
}

void Sexy::FishTypePet::DropCoin()
{
	if (mApp->mBoard->mTank == 5 || mApp->mBoard->mPause)
		return;

	int anExoticFoodReqsInTank[8];
	int anExoticFoodInTank[8];

	if (mFishTypePetType == PET_PREGO)
	{
		mCoinDropTimer++;
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && mCoinDropTimer == mCoinDropT - 210)
		{
			 mApp->mBoard->GetExoticFoodsRequiredInTank(anExoticFoodReqsInTank);
			 mApp->mBoard->GetExoticFoodsInTank(anExoticFoodInTank);
			 if (anExoticFoodReqsInTank[0] - anExoticFoodInTank[0] + 1 <= 0)
				 mCoinDropTimer -= 300;
		}
		if (mCoinDropTimer >= mCoinDropT)
		{
			mCoinDropTimer = 0;
			Fish* aGuppy = mApp->mBoard->SpawnGuppy(mX + 7, mY + 25);
			if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
				aGuppy->mCanBeEatenDelay = 40;
			mApp->mBoard->PlayBirthSound(false);
			if (mApp->mBoard->mFishList->size() > 10)
			{
				mCoinDropT = 2000;
			}
			else
			{
				mCoinDropT = mApp->mBoard->mFishList->size() > 5 ? 1230 : 930;
			}
		}
	}
	else if (mFishTypePetType == PET_VERT)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer >= mCoinDropT)
		{
			mCoinDropTimer = 0;
			if(CanDropCoin())
				mApp->mBoard->DropCoin(mX + 15, mY + 10, COIN_GOLD_C, nullptr, -1.0, 0);
		}
	}
	else if (mFishTypePetType == PET_NOSTRADAMUS)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer >= mCoinDropT)
		{
			mCoinDropTimer = 0;
			mApp->mBoard->NostradamusDropFood(mX + 15, mY + 10);
		}
	}
	else if (mFishTypePetType == PET_SHRAPNEL)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer >= mCoinDropT)
		{
			mCoinDropTimer = 0;
			mApp->mBoard->DropCoin(mX + 15, mY + 10, COIN_SHRAPNEL_BOMB, nullptr, -1.0, 0);
			mApp->mBoard->PlaySample(SOUND_UNLEASH_ID, 3, 1.0);
		}
	}
	else if (mFishTypePetType == PET_ZORF)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer >= mCoinDropT)
		{
			bool foundHungry = false;
			// Iterator loops in the original, dereferencing the iterator at every access
			for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); it++)
			{
				if ((*it)->mHunger < 300 && (*it)->mExoticDietFoodType == 0)
				{
					foundHungry = true;
					break;
				}
			}

			for (std::vector<Breeder*>::iterator it = mApp->mBoard->mBreederList->begin(); it != mApp->mBoard->mBreederList->end(); it++)
			{
				if ((*it)->mHunger < 300 && (*it)->mExoticDietFoodType == 0)
				{
					foundHungry = true;
					break;
				}
			}

			if (mApp->mBoard->mPetsInTank[PET_BRINKLEY] != 0)
			{
				m0x240++;
				int aModeVal = (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK ? 4 : 2);
				if (m0x240 % aModeVal != 0 && !foundHungry)
				{
					mCoinDropTimer = 0;
					return;
				}
			}
			else if (!foundHungry)
				return;

			mCoinDropTimer = -10;
			if (mVX < 0.0)
				mApp->mBoard->DropFood(mX + 15, mY + 10, 1, false, 0, -1);
			else
				mApp->mBoard->DropFood(mX + 15, mY + 10, 2, false, 0, -1);
		}
	}
	else if (mFishTypePetType == PET_MERYL)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer == mCoinDropT - 100)
		{
			gMerylActive = true;
			mApp->mBoard->DropCoin(mX + 15, mY - 5, COIN_NOTE, nullptr, -1.0, 0);
			mApp->mBoard->PlaySample(SOUND_SING_ID, 3, 1.0);
		}
		else if(mCoinDropTimer >= mCoinDropT)
		{
			gMerylActive = false;
			mCoinDropTimer = 0;
		}
	}
	else if (mFishTypePetType == PET_GASH)
	{
		mCoinDropTimer++;
	}
	else if (mFishTypePetType == PET_AMP)
	{
		mCoinDropTimer++;
		if (mCoinDropTimer >= mCoinDropT)
		{
			if(mCoinDropTimer == mCoinDropT)
				mApp->mBoard->PlaySample(SOUND_EEL2_ID, 3, 1.0);
			mMouseVisible = true;
			mDoFinger = true;
		}
	}
	else if (mFishTypePetType == PET_STANLEY)
	{
		if (mApp->mBoard->AliensInTank())
		{
			if (m0x260 < 10)
				m0x260++;
		}
		else if (m0x260 > 0)
			m0x260--;
		mCoinDropTimer++;
		StanleyFunction();
		if (mCoinDropTimer > mCoinDropT)
		{
			mCoinDropTimer = 0;
			if (m0x23c == 0)
				m0x23c = 50;

			// The facing is tested before the target is looked up, as the original
			if (mVX < 0.0)
			{
				GameObject* anObj = FindNearestFood();
				if (anObj != nullptr)
				{
					mApp->mBoard->SpawnMissle(mX, mY, anObj, Missle::MISSLE_BALL);
					mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
					mEatingAnimationTimer = 10;
				}
			}
			else
			{
				GameObject* anObj = FindNearestFood();
				if (anObj != nullptr)
				{
					mApp->mBoard->SpawnMissle(mX + 40, mY, anObj, Missle::MISSLE_BALL);
					mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
					mEatingAnimationTimer = 10;
				}
			}
		}
	}
}

bool Sexy::FishTypePet::Hungry()
{
	// Fetched up front (before DeterminePetSleepy), as the original
	std::vector<Coin*>* aCoinList = mApp->mBoard->mCoinList;
	std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;

	if (mFishTypePetType == PET_ANGIE && !mApp->mBoard->mDeadFishList->empty())
	{
		HungryBehavior();
		return true;
	}

	if (mFishTypePetType == PET_NIMBUS)
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
			DeterminePetSleepy(&m0x245);
		if ((!aCoinList->empty() || (!aFoodList->empty() && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK))
			&& !m0x245)
		{
			mSpeedMod = 0.5;
			HungryBehavior();
			return true;
		}
		mSpeedMod = 1.8;
	}

	if (mFishTypePetType == PET_ITCHY && mApp->mBoard->AliensInTank())
	{
		HungryBehavior();
		return true;
	}

	if (mFishTypePetType == PET_BRINKLEY)
	{
		if (!mApp->mBoard->AliensInTank() && mApp->mBoard->FoodInTank() && m0x23c == 0)
		{
			HungryBehavior();
			return true;
		}
	}

	if (mFishTypePetType == PET_GASH)
	{
		if (mApp->mBoard->AliensInTank() || (mCoinDropT < mCoinDropTimer && mApp->mBoard->FishInTank()))
		{
			HungryBehavior();
			return true;
		}
	}

	if (mFishTypePetType == PET_GUMBO && mApp->mBoard->AliensInTank())
	{
		HungryBehavior();
		return true;
	}

	if (mFishTypePetType == PET_WADSWORTH)
	{
		if (mCoinDropTimer > 0)
			mCoinDropTimer--;
		else
		{
			if (m0x250 && WadsworthTimerResetCheck())
				mCoinDropTimer = 100;
		}

		if ((!mApp->mBoard->AliensInTank() && !mApp->mBoard->MisslesInTank()) ||
			(mApp->mGameMode == GAMEMODE_CHALLENGE && mApp->mBoard->m0x45c > 1 && WadsworthFishesCheck()))
		{
			if (m0x250)
			{
				m0x250 = false;
				gWadsworthTimer--;
				if (gWadsworthTimer < 0)
					gWadsworthTimer = 0;
				mCoinDropTimer = 30;
				mApp->mBoard->SpawnBubble(mX + 11, mY + 5);
				mApp->mBoard->SpawnBubble(mX + 4, mY + 2);
			}
		}
		else
		{
			if (!m0x250)
			{
				gWadsworthTimer++;
				m0x250 = true;
				mCoinDropTimer = mCoinDropT;
			}
			gWadsworthX = mX;
			gWadsworthY = mY;
		}

	}

	return false;
}

void Sexy::FishTypePet::DrawFish(Graphics* g, bool mirror)
{
	if (m0x24c != nullptr)
	{
		g->Translate(m0x24c->mX - g->mTransX, m0x24c->mY - g->mTransY);
		m0x24c->Draw(g);
		g->Translate(mX - g->mTransX, mY - g->mTransY);
	}
	if (m0x230)
	{
		g->SetColor(Color(0xffffff));
	}

	// Case bodies in the original's order
	switch (mFishTypePetType)
	{
	case PET_WADSWORTH:
		DrawWadsworth(g, mirror);
		break;
	case PET_PREGO:
		DrawPrego(g, mirror);
		break;
	case PET_AMP:
		DrawAmp(g, mirror);
		break;
	case PET_GUMBO:
		DrawGumbo(g, mirror);
		break;
	case PET_ANGIE:
		DrawAngie(g, mirror);
		break;
	case PET_MERYL:
		DrawMeryl(g, mirror);
		break;
	case PET_ITCHY:
		DrawItchy(g, mirror);
		break;
	case PET_GASH:
		DrawGash(g, mirror);
		break;
	case PET_SHRAPNEL:
		DrawShrapnel(g, mirror);
		break;
	case PET_ZORF:
		DrawZorf(g, mirror);
		break;
	case PET_WALTER:
		DrawWalter(g, mirror);
		break;
	case PET_STANLEY:
		DrawStanley(g, mirror);
		break;
	case PET_BRINKLEY:
		DrawCommonPet(g, IMAGE_BRINKLEY, mirror);
		break;
	case PET_VERT:
		DrawCommonPet(g, IMAGE_VERT, mirror);
		break;
	case PET_NIMBUS:
		DrawCommonPet(g, IMAGE_NIMBUS, mirror);
		break;
	case PET_PRESTO:
		DrawCommonPet(g, IMAGE_PRESTO, mirror);
		break;
	case PET_SEYMOUR:
		DrawCommonPet(g, IMAGE_SEYMOUR, mirror);
		break;
	case PET_BLIP:
		DrawCommonPet(g, IMAGE_BLIP, mirror);
		break;
	case PET_NOSTRADAMUS:
	{
		int anXTrans = 0;
		int anYTrans = 0;
		if (mApp->mBoard->m0x2b4 != 0 && mApp->mBoard->m0x2b4 < 25 && !mApp->mBoard->mPause)
		{
			anXTrans = rand() % 21 - 10;
			anYTrans = rand() % 21 - 10;
			g->Translate(anXTrans, anYTrans);
		}
		DrawCommonPet(g, IMAGE_NOSTRADAMUS, mirror);
		g->Translate(-anXTrans, -anYTrans);
		break;
	}
	}

	if ((m0x230 || mFishTypePetType == PET_NOSTRADAMUS) && m0x248 < 20)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(0xaf, 0xaf, 0xaf, 0xff));
		g->DrawImageMirror(IMAGE_PRESTO, 0, 0, Rect((m0x248/2) * 80, 160, 80, 80), mirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (m0x230 && mFishTypePetType != PET_PRESTO)
		DrawPrestoMisc(g, m0x238);
}

bool Sexy::FishTypePet::HungryBehavior()
{
	GameObject* aFood = FindNearestFood();
	if (mSpecialMovementStateChangeTimer >= 5 && aFood != nullptr)
	{
		mSpecialMovementStateChangeTimer = 0;
		// Itchy is tested on its own; the chain below starts again at Gash, as the original
		if (mFishTypePetType == PET_ITCHY)
		{
			double aCX = mXD + 40.0;
			int aDist = (aFood->mType == TYPE_BILATERUS) ? 40 : 80;
			if (aCX > aFood->mX + aDist)
			{
				if (mVX > -10.0)
					mVX -= 2.5;
			}
			else if (aCX < aFood->mX + aDist)
			{
				if (mVX < 10.0)
					mVX += 2.5;
			}

			double aCY = mYD + 40.0;
			if (aCY > aFood->mY + aDist)
			{
				if (mVY > -4.0)
					mVY -= 1.5;
			}
			else if (aCY < aFood->mY + aDist)
			{
				if (mVY < 4.0)
					mVY += 1.5;
			}
			if (mVXAbs < 5)
				mVXAbs++;
		}

		if (mFishTypePetType == PET_GASH)
		{
			double aCX = mXD + 40.0;
			if (mApp->mBoard->AliensInTank())
			{
				int aDist = (aFood->mType == TYPE_BILATERUS) ? 40 : 80;
				if (aCX > aFood->mX + aDist)
				{
					if (mVX > -9.0)
						mVX -= 2.5;
				}
				else if (aCX < aFood->mX + aDist)
				{
					if (mVX < 9.0)
						mVX += 2.5;
				}

				double aCY = mYD + 40.0;
				if (aCY > aFood->mY + aDist)
				{
					if (mVY > -4.0)
						mVY -= 1.5;
				}
				else if (aCY < aFood->mY + aDist)
				{
					if (mVY < 4.0)
						mVY += 1.5;
				}
			}
			else // Chase Guppy
			{
				if (aCX > aFood->mX + 40)
				{
					if (mVX > -8.0)
						mVX -= 2.5;
				}
				else if (aCX < aFood->mX + 40)
				{
					if (mVX < 8.0)
						mVX += 2.5;
				}

				double aCY = mYD + 40.0;
				if (aCY > aFood->mY + 40)
				{
					if (mVY > -4.0)
						mVY -= 1.5;
				}
				else if (aCY < aFood->mY + 40)
				{
					if (mVY < 4.0)
						mVY += 1.5;
				}
			}
			if (mVXAbs < 5)
				mVXAbs++;
		}
		else if (mFishTypePetType == PET_GUMBO)
		{
			int aDist = (aFood->mType == TYPE_BILATERUS) ? 40 : 80;
			if (aFood->mY + aDist > 260 && mVY > -8.0)
				mVY -= 2.0;
			else if (aFood->mY + aDist < 300 && mVY < 8.0)
				mVY += 2.0;

			if (aFood->mX + aDist > 290 && mVX > -8.0)
				mVX -= 2.0;
			else if (aFood->mX + aDist < 330 && mVX < 8.0)
				mVX += 2.0;
		}
		else if (mFishTypePetType == PET_ANGIE)
		{
			DeadFish* aDeadFish = (DeadFish*) aFood;
			int aDist = 40;
			if (aDeadFish->mObjType == TYPE_ULTRA)
				aDist = 80;
			int aFX = aFood->mX + aDist;
			double aCX = mXD + 40.0;
			if (aCX > aFX + 4)
			{
				if (mVX > -3.0)
					mVX -= 1.0;
			}
			else if (aCX < aFX - 4)
			{
				if (mVX < 3.0)
					mVX += 1.0;
			}
			else if (aCX > aFX + 2)
			{
				if (mVX > -3.0)
					mVX -= 0.1;
			}
			else if (aCX < aFX - 2)
			{
				if (mVX < 3.0)
					mVX += 0.1;
			}
			else if (aCX > aFX)
			{
				if (mVX > -3.0)
					mVX -= 0.05;
			}
			else if (aCX < aFX)
			{
				if (mVX < 3.0)
					mVX += 0.05;
			}

			int aFY = aFood->mY + aDist;
			double aCY = mYD + 40.0;
			if (aCY > aFY + 3)
			{
				if (mVY > -2.0)
					mVY -= 0.6;
			}
			else if (aCY < aFY - 3)
			{
				if (mVY < 3.0)
					mVY += 1.0;
			}
			else if (aCY > aFY)
			{
				if (mVY > -2.0)
					mVY -= 0.3;
			}
			else if (aCY < aFY)
			{
				if (mVY < 3.0)
					mVY += 0.5;
			}

			if (mVXAbs < 5)
				mVXAbs++;
		}
		else if (mFishTypePetType == PET_NIMBUS)
		{
			double aCX = mXD + 40.0;
			if (aCX > aFood->mX + 54)
			{
				if (mVX > -2.3)
					mVX -= 0.5;
			}
			else if (aCX < aFood->mX + 18)
			{
				if (mVX < 2.3)
					mVX += 0.5;
			}
			else if (aCX > aFood->mX + 44)
			{
				if (mVX > -1.3)
					mVX -= 0.1;
			}
			else if (aCX < aFood->mX + 28)
			{
				if (mVX < 1.3)
					mVX += 0.1;
			}
			else if (aCX > aFood->mX + 36)
			{
				if (mVX > -0.3)
					mVX = 0.0;
			}
			else if (aCX < aFood->mX + 36)
			{
				if (mVX < 0.3)
					mVX = 0.0;
			}

			double aCY = mYD + 40.0;
			if (aCY > aFood->mY + 42)
			{
				if (mVY > -2.0)
					mVY -= 0.6;
			}
			else if (aCY < aFood->mY + 30)
			{
				if (mVY < 3.0)
					mVY += 1.0;
			}
			else if (aCY > aFood->mY + 36)
			{
				if (mVY > -2.0)
					mVY -= 0.3;
			}
			else if (aCY < aFood->mY + 36)
			{
				if (mVY < 3.0)
					mVY += 0.5;
			}
		}
		else if (mFishTypePetType == PET_BRINKLEY)
		{
			double aCX = mXD + 40.0;
			if (aCX > aFood->mX + 28)
			{
				if (mVX > -3.0)
					mVX -= 1.0;
			}
			else if (aCX < aFood->mX + 12)
			{
				if (mVX < 3.0)
					mVX += 1.0;
			}
			else if (aCX > aFood->mX + 24)
			{
				if (mVX > -3.0)
					mVX -= 0.1;
			}
			else if (aCX < aFood->mX + 16)
			{
				if (mVX < 3.0)
					mVX += 0.1;
			}
			else if (aCX > aFood->mX + 20)
			{
				if (mVX > -3.0)
					mVX -= 0.05;
			}
			else if (aCX < aFood->mX + 20)
			{
				if (mVX < 3.0)
					mVX += 0.05;
			}

			double aCY = mYD + 40.0;
			if (aCY > aFood->mY + 26)
			{
				if (mVY > -2.0)
					mVY -= 0.6;
			}
			else if (aCY < aFood->mY + 14)
			{
				if (mVY < 3.0)
					mVY += 1.0;
			}
			else if (aCY > aFood->mY + 20)
			{
				if (mVY > -2.0)
					mVY -= 0.3;
			}
			else if (aCY < aFood->mY + 20)
			{
				if (mVY < 3.0)
					mVY += 0.5;
			}

			if (mVXAbs < 5)
				mVXAbs++;
		}
	}
	else if (mFishTypePetType == PET_NIMBUS && aFood == nullptr)
		mSpeedMod = 1.8;

	if (aFood)
		CollideWithFood();
	return aFood != nullptr;
}

Sexy::GameObject* Sexy::FishTypePet::FindNearestFood()
{
	// Every distance below is taken as the original: the object's centre minus the pet's, truncated
	// to int, squared (x in floating point, y in int) into a float, then a float square root.
	int aDist = 10000;
	GameObject* aRet = nullptr;
	// Itchy and Stanley are tested on their own; the chain below starts again at Gash, as the original
	// Every list is walked with an iterator that is dereferenced again at each access, as the original
	if (mFishTypePetType == PET_ITCHY || mFishTypePetType == PET_STANLEY)
	{
		if (mFishTypePetType == PET_STANLEY)
		{
			for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
			{
				int ax = ((*it)->mX + 40) - (mXD + 40.0);
				int ay = ((*it)->mY + 40) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if (aNewDist < aDist)
				{
					if ((*it)->mMisslePtr == nullptr)
					{
						aDist = aNewDist;
						aRet = *it;
					}
				}
			}
		}
		else
		{
			for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
			{
				int ax = ((*it)->mX + 40) - (mXD + 40.0);
				int ay = ((*it)->mY + 40) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if (aNewDist < aDist)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}
		}

		for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
		{
			int ax = ((*it)->mX + 80) - (mXD + 40.0);
			int ay = ((*it)->mY + 80) - (mYD + 40.0);
			float aDistSq = (double)ax * ax + ay * ay;
			int aNewDist = sqrtf(aDistSq);
			if (!(*it)->mIsPsychosquidHealing && aNewDist < aDist)
			{
				if ((*it)->mMisslePtr == nullptr || mFishTypePetType != PET_STANLEY)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}
		}
	}

	if (mFishTypePetType == PET_GASH)
	{
		if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
		{
			for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
			{
				int ax = ((*it)->mX + 40) - (mXD + 40.0);
				int ay = ((*it)->mY + 40) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if (aNewDist < aDist)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}

			for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
			{
				int ax = ((*it)->mX + 80) - (mXD + 40.0);
				int ay = ((*it)->mY + 80) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if (!(*it)->mIsPsychosquidHealing && aNewDist < aDist)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}
		}
		else if (mCoinDropTimer > mCoinDropT)
		{
			if (!mApp->mBoard->mFishList->empty())
			{
				for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
				{
					if ((*it)->mVirtualTankId < 0)
					{
						int ax = ((*it)->mX + 40) - (mXD + 40.0);
						int ay = ((*it)->mY + 40) - (mYD + 40.0);
						float aDistSq = (double)ax * ax + ay * ay;
						int aNewDist = sqrtf(aDistSq);
						if (aNewDist < aDist)
						{
							aDist = aNewDist;
							aRet = *it;
						}
					}
				}
			}
		}
	}
	else if (mFishTypePetType == PET_GUMBO)
	{
		for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
		{
			int ax = ((*it)->mX + 40) - (mXD + 40.0);
			int ay = ((*it)->mY + 40) - (mYD + 40.0);
			float aDistSq = (double)ax * ax + ay * ay;
			int aNewDist = sqrtf(aDistSq);
			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = *it;
			}
		}

		for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
		{
			int ax = ((*it)->mX + 80) - (mXD + 40.0);
			int ay = ((*it)->mY + 80) - (mYD + 40.0);
			float aDistSq = (double)ax * ax + ay * ay;
			int aNewDist = sqrtf(aDistSq);
			if (aNewDist < aDist) // Gumbo also leads to a healing Psychosquid, as the original
			{
				aDist = aNewDist;
				aRet = *it;
			}
		}
	}
	else if (mFishTypePetType == PET_ANGIE)
	{
		for (std::vector<DeadFish*>::iterator it = mApp->mBoard->mDeadFishList->begin(); it != mApp->mBoard->mDeadFishList->end(); ++it)
		{
			if ((*it)->m0x1a0 > 95)
			{
				int ax = ((*it)->mX + 40) - (mXD + 40.0);
				int ay = ((*it)->mY + 40) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if (aNewDist < aDist)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}
		}
	}
	else if (mFishTypePetType == PET_NIMBUS)
	{
		// Asked once, before either list is walked, as the original
		bool isScrSvr = mApp->IsScreenSaver();
		if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty() &&
			mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			for (std::vector<Food*>::iterator it = mApp->mBoard->mFoodList->begin(); it != mApp->mBoard->mFoodList->end(); ++it)
			{
				int ax = ((*it)->mX + 20) - (mXD + 40.0);
				if (ax * ax < 250) // food right above or below counts as 50 away, as the original
					ax = 50;
				int ay = ((*it)->mY + 20) - (mYD + 40.0);
				float aDistSq = (double)ax * ax + ay * ay;
				int aNewDist = sqrtf(aDistSq);
				if ((*it)->mExoticFoodType == 0 && (*it)->m0x18c == 0 && aNewDist < aDist)
				{
					aDist = aNewDist;
					aRet = *it;
				}
			}
		}

		int aPentaCnt = mApp->mBoard->mPentaList->size();

		for (std::vector<Coin*>::iterator it = mApp->mBoard->mCoinList->begin(); it != mApp->mBoard->mCoinList->end(); ++it)
		{
			if ((*it)->mCoinType != COIN_SHRAPNEL_BOMB)
			{
				if (((*it)->mCoinType == COIN_STAR || (*it)->mCoinType == SHELL_STAR) && aPentaCnt != 0)
					continue;

				if (!(*it)->m0x198 && (*it)->mCoinType != COIN_PEANUT && !RelaxModeCheck(*it) &&
					(!isScrSvr || (*it)->mUpdateCnt <= gUnkInt11))
				{
					int ax = ((*it)->mX + 36) - (mXD + 40.0);
					if (ax * ax < 250) // as for food
						ax = 50;
					int ay = ((*it)->mY + 36) - (mYD + 40.0);
					float aDistSq = (double)ax * ax + ay * ay;
					int aNewDist = sqrtf(aDistSq);
					if (aNewDist < aDist)
					{
						aDist = aNewDist;
						aRet = *it;
					}
				}
			}
		}
	}
	else if (mFishTypePetType == PET_BRINKLEY)
	{
		for (std::vector<Food*>::iterator it = mApp->mBoard->mFoodList->begin(); it != mApp->mBoard->mFoodList->end(); ++it)
		{
			int ax = ((*it)->mX + 20) - (mXD + 40.0);
			int ay = ((*it)->mY + 20) - (mYD + 40.0);
			float aDistSq = (double)ax * ax + ay * ay;
			int aNewDist = sqrtf(aDistSq);
			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = *it;
			}
		}
	}
	return aRet;
}

void Sexy::FishTypePet::CollideWithFood()
{
	// Fetched up front, as the original; only Brinkley walks it (Nimbus re-reads mApp->mBoard->mFoodList)
	std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;

	// Iterator loops in the original. Most take the object into a local once; the tests marked
	// below dereference the iterator again, as the original
	if (mFishTypePetType == PET_ITCHY)
	{
		for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
		{
			Bilaterus* aBil = *it;
			int aBilX = aBil->mX + 40;
			int aBilY = aBil->mY + 40;
			if (abs(mXD + 40.0 - aBilX) < 20.0 && abs(mYD + 40.0 - aBilY) < 20.0)
			{
				aBil->mActiveHead->m0x1d8 -= 1.0;
				mApp->mBoard->PlayPunchSound(11);
			}
		}

		for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
		{
			Alien* anAlien = *it;
			int anAlienX = anAlien->mX + 80;
			int anAlienY = anAlien->mY + 80;
			if (abs(mXD + 40.0 - anAlienX) < 20.0 && abs(mYD + 40.0 - anAlienY) < 20.0)
			{
				if (!(*it)->mIsPsychosquidHealing) // dereferenced again (0x4FE959)
				{
					DamageAlien(anAlien);
					break;
				}
			}
		}
	}
	else if (mFishTypePetType == PET_GASH)
	{
		if (mApp->mBoard->AliensInTank())
		{
			for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
			{
				Bilaterus* aBil = *it;
				int aBilX = aBil->mX + 40;
				int aBilY = aBil->mY + 40;
				double aDistX = abs(mXD + 40.0 - aBilX);
				double aDistY = abs(mYD + 40.0 - aBilY);
				if (aDistX < 20.0 && aDistY < 20.0)
				{
					aBil->mActiveHead->m0x1d8 -= 3.0;
					mApp->mBoard->PlayPunchSound(9);
					if (aBil->mActiveHead->m0x1d8 <= 0.0)
					{
						aBil->mActiveHead->Die(true);
						break;
					}
				}
				else if(aDistX < 30.0 && aDistY < 30.0 && mEatingAnimationTimer == 0)
				{
					mEatingAnimationTimer = 10;
					break;
				}
			}

			for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
			{
				Alien* anAlien = *it;
				int anAlienX = anAlien->mX + 80;
				int anAlienY = anAlien->mY + 80;
				if (abs(mXD + 40.0 - anAlienX) < 20.0 && abs(mYD + 40.0 - anAlienY) < 20.0)
				{
					if (!(*it)->mIsPsychosquidHealing) // dereferenced again (0x4FEBF7)
					{
						DamageAlien(anAlien);
						break;
					}
				}

				if (abs(mXD + 40.0 - anAlienX) < 30.0 && abs(mYD + 40.0 - anAlienY) < 30.0 && mEatingAnimationTimer == 0)
				{
					mEatingAnimationTimer = 10;
					break;
				}
			}
		}
		else if(mCoinDropT < mCoinDropTimer && mApp->mBoard->FishInTank())
		{
			for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); it++)
			{
				Fish* aFish = *it;

				if (aFish->mVirtualTankId < 0)
				{
					int aFishX = aFish->mX + 40;
					int aFishY = aFish->mY + 40;
					double aDistX = abs(mXD + 40.0 - aFishX);
					double aDistY = abs(mYD + 40.0 - aFishY);
					if (aDistX < 20.0 && aDistY < 20.0)
					{
						(*it)->RemoveFromGame(true); // dereferenced again (0x4FEDCA)
						mApp->mBoard->PlayChompSound(false);
						mCoinDropTimer = 0;
						break;
					}
					if (aDistX < 30.0 && aDistY < 30.0 && mEatingAnimationTimer == 0)
					{
						mEatingAnimationTimer = 10;
						break;
					}
				}
			}
		}
	}
	else if (mFishTypePetType == PET_ANGIE)
	{
		for (std::vector<DeadFish*>::iterator it = mApp->mBoard->mDeadFishList->begin(); it != mApp->mBoard->mDeadFishList->end(); ++it)
		{
			DeadFish* aFish = *it;

			int aFishX;
			int aFishY;
			double anAbsDist;
			if (aFish->mObjType == TYPE_ULTRA)
			{
				aFishX = aFish->mX + 80;
				aFishY = aFish->mY + 80;
				anAbsDist = 60.0;
			}
			else
			{
				aFishX = aFish->mX + 40;
				aFishY = aFish->mY + 40;
				anAbsDist = 30.0;
			}
			double aDistX = abs(mXD + 40.0 - aFishX);
			double aDistY = abs(mYD + 40.0 - aFishY);
			if (aDistX < anAbsDist && aDistY < anAbsDist && (*it)->m0x1a0 > 95) // dereferenced again (0x4FEF3A)
			{
				aFish->StartRessurection();
				break;
			}
		}
	}
	else if (mFishTypePetType == PET_NIMBUS)
	{
		bool isScrSvr = mApp->IsScreenSaver();
		bool noPenta = mApp->mBoard->mPentaList->empty();
		for (std::vector<Coin*>::iterator it = mApp->mBoard->mCoinList->begin(); it != mApp->mBoard->mCoinList->end(); ++it)
		{
			Coin* aCoin = *it;

			int aCoinX = aCoin->mX + 36;
			int aCoinY = aCoin->mY + 36;

			// The update count is read through the iterator again (0x4FF106), as the original
			if (abs(mXD + 40.0 - aCoinX) < 30.0 && abs(mYD + 40.0 - aCoinY) < 30.0 &&
				aCoin->mCoinType != COIN_SHRAPNEL_BOMB && ((aCoin->mCoinType != COIN_STAR && aCoin->mCoinType != SHELL_STAR) || noPenta) &&
				!aCoin->m0x198 && aCoin->mCoinType != COIN_PEANUT && (!RelaxModeCheck(aCoin) && (!isScrSvr || (*it)->mUpdateCnt <= gUnkInt11)))
			{
				// Read before the coin is removed, as the original
				int aShellType = aCoin->mCoinType;
				if (aShellType <= COIN_END)
					aShellType += COIN_END;
				int aCoinPosX = aCoin->mX;
				int anUpdateCnt = aCoin->mUpdateCnt;
				aCoin->RemoveCoin();
				mApp->mBoard->DropCoin(aCoinPosX, mYD - 25.0, aShellType, nullptr, -1.0, anUpdateCnt);
				break;
			}
		}

		if (!mApp->mBoard->AliensInTank() && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			for (std::vector<Food*>::iterator it = mApp->mBoard->mFoodList->begin(); it != mApp->mBoard->mFoodList->end(); ++it)
			{
				Food* aFood = *it;

				int aFoodX = aFood->mX + 20;
				int aFoodY = aFood->mY + 20;
				double aDistX = abs(mXD + 40.0 - aFoodX);
				double aDistY = abs(mYD + 40.0 - aFoodY);
				if (aDistX < 30.0 && aDistY < 30.0 && aFood->m0x18c == 0)
				{
					// Read before the food is removed, as the original
					int aFoodPosX = aFood->mX;
					int aFoodType = aFood->mFoodType;
					aFood->RemoveFood();
					mApp->mBoard->DropFood(aFoodPosX, mYD - 30.0, 0, true, 0, aFoodType);
					break;
				}
			}
		}
	}
	else if (mFishTypePetType == PET_BRINKLEY)
	{
		// The iterator is dereferenced at every access, and mYD + 40.0 is read again for each test
		// (0x4FF396, 0x4FF3C5), as the original
		for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 35 && mXD + 40.0 > (*it)->mX + 5 &&
				mYD + 40.0 < (*it)->mY + 35 && mYD + 40.0 > (*it)->mY)
			{
				mApp->mBoard->PlaySlurpSound(false);
				(*it)->RemoveFood();
				m0x240++;
				m0x23c = mApp->mGameMode == GAMEMODE_VIRTUAL_TANK ? 108 : 45;
				if (m0x240 % 3 == 0 && CanDropCoin())
				{
					int aCoinType = 10;
					if (m0x240 % 135 == 0)
						aCoinType = 14;
					else if (m0x240 % 45 == 0)
						aCoinType = 13;
					else if (m0x240 % 15 == 0)
						aCoinType = 11;

					mApp->mBoard->DropCoin(mX + 5, mY - 10, aCoinType, nullptr, -1.0, 0);
				}
				break;
			}
		}
	}
}

void Sexy::FishTypePet::FishUpdateAnimation()
{
	// The (prev > 0, vx < 0) turn is tested first, as the original
	if (mPrevVX > 0.0 && mVX < 0.0)
	{
		if (mFishTypePetType == PET_PREGO && mCoinDropTimer > mCoinDropT - 220)
			mVX = 0;
		else if (mFishTypePetType == PET_WALTER && m0x24c != 0)
			mVX = 0;
		else
			mTurnAnimationTimer = 20;
	}
	else if (mPrevVX < 0.0 && mVX > 0.0)
	{
		if (mFishTypePetType == PET_PREGO && mCoinDropTimer > mCoinDropT - 220)
			mVX = 0;
		else if (mFishTypePetType == PET_WALTER && m0x24c != 0)
			mVX = 0;
		else
			mTurnAnimationTimer = -20;
	}

	if (mTurnAnimationTimer > 0)
	{
		mTurnAnimationTimer--;
		if (mTurnAnimationTimer == 0)
			mEatingAnimationTimer = 0;
	}
	else if (mTurnAnimationTimer < 0)
	{
		mTurnAnimationTimer++;
		if (mTurnAnimationTimer == 0)
			mEatingAnimationTimer = 0;
	}

	if (mTurnAnimationTimer != 0)
	{
		// Nimbus has its own copy of the same two tests, as the original
		if (mFishTypePetType == PET_NIMBUS)
		{
			if (mTurnAnimationTimer > 0)
				mAnimationFrameIndexFish = 9 - mTurnAnimationTimer / 2;
			else if (mTurnAnimationTimer < 0)
				mAnimationFrameIndexFish = mTurnAnimationTimer / 2 + 9;
		}
		else if (mTurnAnimationTimer > 0)
			mAnimationFrameIndexFish = 9 - mTurnAnimationTimer / 2;
		else if (mTurnAnimationTimer < 0)
			mAnimationFrameIndexFish = mTurnAnimationTimer / 2 + 9;
	}
	else
	{ // 70
		// The counter limits are ">= 20/40/60/80" and the falling frames an abs(), as the original
		if (mFishTypePetType == PET_VERT)
		{
			mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			if (mSwimFrameCounter < 10)
				mAnimationFrameIndexFish = mSwimFrameCounter;
			else
				mAnimationFrameIndexFish = abs(mSwimFrameCounter - 19);
		}
		else if (mFishTypePetType == PET_WALTER)
		{
			if (mEatingAnimationTimer > 0)
			{
				mEatingAnimationTimer--;
				if (mEatingAnimationTimer >= 30)
					mAnimationFrameIndexFish = 9 - (mEatingAnimationTimer - 20) / 2;
				else if (mEatingAnimationTimer >= 10)
					mAnimationFrameIndexFish = 4;
				else
					mAnimationFrameIndexFish = mEatingAnimationTimer / 2;
			}
			else
			{
				if (mVXAbs <= 1)
					mSwimFrameCounter++;
				else
					mSwimFrameCounter += 2;
				if (mSwimFrameCounter >= 40)
					mSwimFrameCounter = 0;
				if (mSwimFrameCounter < 20)
					mAnimationFrameIndexFish = mSwimFrameCounter / 2;
				else
					mAnimationFrameIndexFish = abs(mSwimFrameCounter / 2 - 19);
			}
		}
		else if (mFishTypePetType == PET_NIMBUS || mFishTypePetType == PET_BRINKLEY)
		{
			if (mVXAbs > 1 && !m0x245)
				mSwimFrameCounter += 2;
			else
				mSwimFrameCounter++;
			if (mSwimFrameCounter >= 40)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 4;
		}
		else if (mFishTypePetType == PET_STANLEY)
		{ // 137
			if (m0x260 > 0 && m0x260 < 10)
				mAnimationFrameIndexFish = m0x260;
			else
			{
				if (mEatingAnimationTimer > 0)
				{
					mEatingAnimationTimer--;
					mAnimationFrameIndexFish = mEatingAnimationTimer;
				}
				else
				{
					if (mVXAbs <= 1)
						mSwimFrameCounter++;
					else
						mSwimFrameCounter += 2;
					if (mSwimFrameCounter >= 40)
						mSwimFrameCounter = 0;
					mAnimationFrameIndexFish = mSwimFrameCounter / 4;
				}
			}
		}
		else if (mFishTypePetType == PET_GASH)
		{
			if (mEatingAnimationTimer > 0)
			{
				mEatingAnimationTimer--;
				mAnimationFrameIndexFish = mEatingAnimationTimer;
			}
			else
			{
				mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
				if (mSwimFrameCounter >= 20)
					mSwimFrameCounter = 0;
				mAnimationFrameIndexFish = mSwimFrameCounter / 2;
			}
		}
		else if (mFishTypePetType == PET_GUMBO)
		{
			mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
			if (mApp->mBoard->AliensInTank())
			{
				m0x258 += 0.1;
				if (m0x258 >= 1.0)
					m0x258 = -1;
			}
		}
		else if (mFishTypePetType == PET_SHRAPNEL)
		{
			if (mVXAbs <= 2)
				mSwimFrameCounter++;
			else
				mSwimFrameCounter += 2;
			if (mSwimFrameCounter >= 60)
				mSwimFrameCounter = 0;
			// The frame is stored between the step and the wrap test, as the original
			m0x258 += 0.1;
			mAnimationFrameIndexFish = mSwimFrameCounter / 6;
			if (m0x258 >= 1.0)
				m0x258 = -1;
		}
		else if (mFishTypePetType == PET_ANGIE)
		{
			mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			m0x258 += 0.01; // the halo pulses slower than Gumbo's light
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
			if (m0x258 >= 1.0)
				m0x258 = -1;
		}
		else if (mFishTypePetType == PET_SEYMOUR)
		{
			mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
			if (mSwimFrameCounter >= 80)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 8;
		}
		else if (mFishTypePetType == PET_ZORF)
		{
			mSwimFrameCounter++;
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
			if (mCoinDropTimer == -1)
				mSwimFrameCounter = 10;
		}
		else if (mFishTypePetType == PET_AMP)
		{
			mSwimFrameCounter++;
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
			if (mCoinDropTimer >= mCoinDropT)
				m0x258 += 0.1;
			else
				m0x258 += 0.5;
			if (m0x258 >= 1.0)
				m0x258 = -1;
		}
		else
		{
			mSwimFrameCounter += (mVXAbs > 1 ? 2 : 1);
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
		}

		if (mFishTypePetType == PET_MERYL && mAnimationFrameIndexFish == 0)
		{
			if (m0x244)
				m0x244 = false;
			else
				m0x244 = ((mApp->mSeed->Next() % 10) == 0);
		}
	}

	// mVX is tested against 0 before mPrevVX, as the original
	if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
		mPrevVX = mVX;

	if(mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::FishTypePet::WadsworthAndMerylFunc01()
{
	if (mFishTypePetType == PET_WADSWORTH && m0x250)
	{
		m0x250 = false;
		gWadsworthTimer--;
		if (gWadsworthTimer < 0)
			gWadsworthTimer = 0;
	}
	if (mFishTypePetType == PET_MERYL)
		gMerylActive = false;
}

bool Sexy::FishTypePet::HandleMouseDown(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
	{
		if (m0x230)
		{
			if (PrestoRightClicked(m0x238))
				mApp->OpenPrestoDialog(this);
			return true;
		}

		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
		return true;
	}

	if (mFishTypePetType == PET_AMP && mCoinDropTimer >= mCoinDropT)
	{
		if (m0x260 >= 2)
		{
			mMouseVisible = gUnkBool06;
			mDoFinger = false;
			ShowFinger(false);
			m0x258 = 0;

			int aMaxDropCount = 0;
			int aCounter = 0;
			std::vector<Fish*> aFishVector;
			// Iterator loops in the original; this one dereferences the iterator at every access
			for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
			{
				if ((*it)->mVirtualTankId < 0)
				{
					aMaxDropCount++;
					aFishVector.push_back(*it);
				}
			}

			aMaxDropCount = (aMaxDropCount - 20) / 2 + 20;
			if (aMaxDropCount < 20)
				aMaxDropCount = 20;

			for (std::vector<Fish*>::iterator it = aFishVector.begin(); it != aFishVector.end(); ++it)
			{
				Fish* aFish = *it;

				aFish->Die(false);
				if (aCounter < aMaxDropCount)
					mApp->mBoard->DropCoin((int)(aFish->mXD + 15.0), (int)(aFish->mYD + 10.0), COIN_DIAMOND_PENTA, nullptr, -1, 0);

				mApp->mBoard->SpawnShot((int)aFish->mXD, (int)aFish->mYD, mApp->mSeed->Next() % 3 + 3);
				aCounter++;
			}

			std::vector<FishTypePet*> aFTPVector;
			for (std::vector<FishTypePet*>::iterator it = mApp->mBoard->mFishTypePetList->begin(); it != mApp->mBoard->mFishTypePetList->end(); ++it)
			{
				FishTypePet* aPet = *it;

				if (!aPet->m0x230 && aPet->mFishTypePetType == PET_NOSTRADAMUS && aPet->m0x240 > 0)
					aFTPVector.push_back(aPet);
			}

			for (std::vector<FishTypePet*>::iterator it = aFTPVector.begin(); it != aFTPVector.end(); ++it)
			{
				FishTypePet* aPet = *it;

				aPet->RemoveHelper02(true);
				mApp->mBoard->DropCoin((int)(aPet->mXD + 15.0), (int)(aPet->mYD + 10.0), COIN_DIAMOND_PENTA, nullptr, -1, 0);
				mApp->mBoard->SpawnShot((int)aPet->mXD, (int)aPet->mYD, mApp->mSeed->Next() % 3 + 3);
				aCounter++;
			}

			if (!mApp->mBoard->HasAnyFish() && mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
				mApp->mBoard->SpawnGuppyBought();
			mApp->mBoard->PlaySample(SOUND_EEL1_ID, 3, 1.0);
			if (aCounter > 0)
				mApp->mBoard->PlayDieSound(-1);
			if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
				mCoinDropT += 200;
			mCoinDropTimer = -20;
			m0x260 = 0;
		}
		else
		{
			m0x260++;
			mApp->mBoard->PlaySample(SOUND_EEL3_ID, 3, 1.0);
		}
		return true;
	}

	if (mFishTypePetType == PET_WALTER && mEatingAnimationTimer == 0 && m0x24c == nullptr
		&& m0x264 == 0 && mTurnAnimationTimer == 0)
	{
		// Facing as the original; with mVX and mPrevVX both 0 no glove is thrown
		if (mVX < 0.0 || ((int)mVX == 0 && mPrevVX < 0.0))
			m0x24c = new BoxingGlove(this, false);
		else if (mVX > 0.0 || ((int)mVX == 0 && mPrevVX > 0.0))
			m0x24c = new BoxingGlove(this, true);

		if (m0x24c != nullptr)
		{
			if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
			{
				m0x260++;
				if (m0x260 >= 5)
				{
					m0x264 = gConst01;
					m0x260 = 0;
					mDoFinger = false;
					mMovementState = 0;
					ShowFinger(false);
				}
			}

			mAnimationFrameIndexFish = 0;
			mEatingAnimationTimer = 40;
			mApp->mBoard->PlaySample(SOUND_SFX_ID, 3, 1.0);
			mApp->mBoard->SortGameObjects();
		}

		return true;
	}


	return false;
}

bool Sexy::FishTypePet::WadsworthTimerResetCheck()
{
	if (gWadsworthTimer > 1)
		return false;

	for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
	{
		Fish* aFish = *it;

		int aX = aFish->mX + 40;
		int aY = aFish->mY + 40;
		if (abs(mXD + 40.0 - aX) > 10.0 && abs(mYD + 40.0 - aY) > 10.0 && aFish->mSize < TYPE_BIG_GUPPY)
			return true;
	}
	return false;
}

bool Sexy::FishTypePet::WadsworthFishesCheck()
{
	Board* aB = mApp->mBoard;
	if (aB->mOscarList->empty() && aB->mPentaList->empty() && aB->mGrubberList->empty() && !aB->GekkosInTank() &&
		!aB->BreedersInTank() && !aB->UltrasInTank())
	{
		// The walk starts from the cached board but re-reads mApp->mBoard for the end, as the original
		for (std::vector<Fish*>::iterator it = aB->mFishList->begin(); it != mApp->mBoard->mFishList->end(); it++)
		{
			Fish* aFish = *it;
			if (aFish->mSize >= TYPE_BIG_GUPPY)
				return false;
		}
		return true;
	}
	return false;
}

void Sexy::FishTypePet::StanleyFunction()
{
	// Walks the aliens and only dereferences each one, as the original (the checks remain)
	for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
	{
		Alien* anAl = *it;
	}

	if (m0x23c <= 0)
	{
		int aDist = 100000000;
		int loc8 = 0;
		Missle* aClosestMis = nullptr;
		for (std::vector<Missle*>::iterator it = mApp->mBoard->mMissleList1->begin(); it != mApp->mBoard->mMissleList1->end(); ++it)
		{
			Missle* aMis = *it;
			if (aMis->m0x17c <= 10 && aMis->m0x1a8 <= 0 && (aMis->mMissleType == Missle::MISSLE_ENERGYBALL ||
				aMis->mMissleType == Missle::MISSLE_CLASSIC) && aMis->mTarget != nullptr)
			{
				int aPrevloc8 = loc8;
				if (aMis->mMissleType == Missle::MISSLE_CLASSIC)
				{
					if (loc8 > 0) continue;
				}
				else if (!aMis->m0x190)
				{
					if (loc8 > 1) continue;
					loc8 = 1;
				}
				else
					loc8 = 2;

				GameObject* aTar = aMis->mTarget;
				int ax = (aTar->mWidth / 2 + aTar->mX) - (aMis->mWidth / 2 + aMis->mX);
				int ay = (aTar->mHeight / 2 + aTar->mY) - (aMis->mHeight / 2 + aMis->mY);
				int aNewDist = ax * ax + ay * ay;
				if (aNewDist < aDist || loc8 != aPrevloc8)
				{
					aClosestMis = aMis;
					aDist = aNewDist;
				}
			}
		}

		// Dead in the original too: m0x23c cannot have become positive here
		if (m0x23c > 0)
		{
			if (loc8 >= 2)
			{
				m0x23c -= 2;
				if (m0x23c < 0)
					m0x23c = 0;
			}
		}
		else if (aClosestMis != nullptr)
		{
			int aVal = Rand() % 100;
			if (aVal < 10)
				m0x23c = 30;
			else
				m0x23c = (aVal >= 60) ? 90 : 60;

			mCoinDropTimer = 0;
			mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);

			int aStanleyCenterX = mX + mWidth / 2;
			int aStanleyShootY = mY + 10;

			aClosestMis->m0x1b0 = aStanleyCenterX;
			aClosestMis->m0x1b4 = aStanleyShootY;
			aClosestMis->m0x1a8 = 1;

			int aAlienCenterX = aClosestMis->mX + aClosestMis->mWidth / 2;
			int aAlienCenterY = aClosestMis->mY + aClosestMis->mHeight / 2;

			int dX = aStanleyCenterX - aAlienCenterX;
			int dY = aStanleyShootY - aAlienCenterY;

			// The distance is kept in a float, as the original
			float aDist = sqrt((double)(dX * dX + dY * dY));
			aClosestMis->m0x1ac = (int)(aDist * 0.0625);

			if (mEatingAnimationTimer == 0) mEatingAnimationTimer = 10;
		}
	}
}

void Sexy::FishTypePet::DamageAlien(Alien* theAlien)
{
	if (mFishTypePetType == PET_ITCHY)
	{
		if (theAlien->mAlienType == ALIEN_DESTRUCTOR || theAlien->mAlienType == ALIEN_GUS)
			theAlien->mHealth -= 0.25;
		else
			theAlien->mHealth -= 1.0;
		mApp->mBoard->PlayPunchSound(11);
	}
	else if (mFishTypePetType == PET_GASH)
	{
		if (theAlien->mAlienType == ALIEN_DESTRUCTOR || theAlien->mAlienType == ALIEN_GUS)
			theAlien->mHealth -= 0.5;
		else
			theAlien->mHealth -= 3.0;
		if (theAlien->mHealth <= 0.0)
			theAlien->Remove(true);
		mApp->mBoard->PlayPunchSound(9);
	}
}

void Sexy::FishTypePet::DrawItchy(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
		{
			aRow = 3;
			mirror = mTurnAnimationTimer > 0;
		}
		else
		{
			aRow = 1;
			mirror = mTurnAnimationTimer > 0;
		}
	}
	else if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
		aRow = 2;
	g->DrawImageMirror(IMAGE_ITCHY, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawPrego(Graphics* g, bool mirror)
{
	int aFrame = mAnimationFrameIndexFish;
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		mirror = mTurnAnimationTimer > 0;
		aRow = 2;
	}
	else if (mCoinDropTimer > mCoinDropT - 200)
	{
		if (mCoinDropTimer < mCoinDropT - 190)
		{
			aRow = 3;
			aFrame = (mCoinDropTimer - mCoinDropT + 200) / 2;
		}
		else if (mCoinDropTimer > mCoinDropT - 5)
		{
			aRow = 3;
			aFrame = mCoinDropTimer - mCoinDropT + 10;
		}
		else
			aRow = 1;
	}
	g->DrawImageMirror(IMAGE_PREGO, 0, 0, Rect(aFrame * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawZorf(Graphics* g, bool mirror)
{
	int aRow = 0;
	int aFrame = mAnimationFrameIndexFish;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	else if (mCoinDropTimer < 0)
	{
		aRow = 2;
		aFrame = mCoinDropTimer + 10;
	}
	g->DrawImageMirror(IMAGE_ZORF, 0, 0, Rect(aFrame * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawMeryl(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 2;
		mirror = mTurnAnimationTimer > 0;
	}
	else if (mCoinDropTimer > mCoinDropT - 100)
		aRow = 1;
	else if (mAnimationFrameIndexFish <= 4 && m0x244)
		aRow = 3;
	g->DrawImageMirror(IMAGE_MERYL, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawWadsworth(Graphics* g, bool mirror)
{
	int aFrame = mAnimationFrameIndexFish;
	int aRow = 0;
	if (mCoinDropTimer > 0)
	{
		aRow = 2;
		if (m0x250)
		{
			if (mCoinDropTimer > mCoinDropT - 10)
				aFrame = 9 - (mCoinDropT - mCoinDropTimer - 1) / 2;
			else if (mCoinDropTimer > 10)
				aFrame = 5;
			else
				aFrame = (mCoinDropTimer - 1) / 2;
		}
		else
		{
			if (mCoinDropTimer > 20)
				aFrame = 9 - (29 - mCoinDropTimer) / 2;
			else if (mCoinDropTimer > 10)
				aFrame = 5;
			else
				aFrame = (mCoinDropTimer - 1) / 2;
		}
	}
	else if (mTurnAnimationTimer != 0)
	{
		mirror = mTurnAnimationTimer > 0;
		aRow = 1;
	}
	g->DrawImageMirror(IMAGE_WADSWORTH, 0, 0, Rect(aFrame * 80, aRow * 80, 80, 80), mirror);
	if (!m0x250)
	{
		if ((!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty() || !mApp->mBoard->mMissleList1->empty())
			&& mApp->mBoard->mTank != 5)
				g->DrawImage(IMAGE_ZZZ, 40, -15);
	}
}

void Sexy::FishTypePet::DrawShrapnel(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80);
	g->DrawImageMirror(IMAGE_SHRAPNEL, 0, 0, aSrcRect, mirror);
	if (mCoinDropTimer > mCoinDropT - 50)
	{
		g->SetColorizeImages(true);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColor(Color(255, 255, 255, (int)(abs(m0x258) * 255.0)));
		g->DrawImageMirror(IMAGE_SHRAPNEL, 0, 0, aSrcRect, mirror);
		g->SetColorizeImages(false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
}

void Sexy::FishTypePet::DrawGumbo(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80);
	g->DrawImageMirror(IMAGE_GUMBO, 0, 0, aSrcRect, mirror);

	if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
	{
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 0, (int)(abs(m0x258) * 255.0)));
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->DrawImageMirror(IMAGE_GUMBOLIGHT, 0, 0, aSrcRect, mirror);
		g->SetColorizeImages(false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
}

void Sexy::FishTypePet::DrawAmp(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		mirror = mTurnAnimationTimer > 0;
		aRow = 1;
	}
	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 160, aRow * 60, 160, 60);
	g->DrawImageMirror(IMAGE_AMP, 0, 0, aSrcRect, mirror);
	if (mCoinDropTimer >= mCoinDropT)
	{
		g->SetColorizeImages(true);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		// The alpha is clamped as a double before truncation, as the original
		double anAlpha = abs(m0x258) * 2 * 255.0;
		if (anAlpha > 255.0)
			anAlpha = 255.0;
		if (m0x260 != 0)
		{
			if (m0x260 == 1)
				g->SetColor(Color(255, 200, 100, (int)anAlpha));
			else
				g->SetColor(Color(255, 100, 100, (int)anAlpha));
		}
		else
			g->SetColor(Color(255, 255, 200, (int)anAlpha));
		g->DrawImageMirror(IMAGE_AMPCHARGE, 0, 0, aSrcRect, mirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
	else if (mCoinDropTimer < 0 && !mApp->mBoard->AliensInTank())
	{
		g->SetColorizeImages(true);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		double anAlpha = abs(m0x258) * 2 * 255.0;
		if (anAlpha > 255.0)
			anAlpha = 255.0;
		g->SetColor(Color(255, 255, 200, (int)anAlpha));
		g->DrawImageMirror(IMAGE_AMPCHARGE, 0, 0, aSrcRect, mirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
}

void Sexy::FishTypePet::DrawGash(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	else if (mEatingAnimationTimer > 0)
		aRow = 2;
	g->DrawImageMirror(IMAGE_GASH, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawAngie(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80);
	g->DrawImageMirror(IMAGE_ANGIE, 0, 0, aSrcRect, mirror);

	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	g->SetColorizeImages(true);
	g->SetColor(Color(255, 255, 255, (int)(abs(m0x258) * 255.0)));
	g->DrawImageMirror(IMAGE_HALO, 0, 0, aSrcRect, mirror);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	g->SetColorizeImages(false);
}

void Sexy::FishTypePet::DrawStanley(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
	{
		if (m0x260 < 10)
			aRow = 5;
		else if (mEatingAnimationTimer != 0)
			aRow = 4;
		else if (mTurnAnimationTimer != 0)
		{
			aRow = 3;
			mirror = mTurnAnimationTimer > 0;
		}
		else
			aRow = 2;
	}
	else if (m0x260 != 0)
		aRow = 5;
	else if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	g->DrawImageMirror(IMAGE_STANLEY, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawWalter(Graphics* g, bool mirror)
{
	if (m0x264 > 0 && m0x264 < gConst01 - 25)
		g->DrawImage(IMAGE_ZZZ, mirror ? 50 : 30, -15);
	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	else if (mEatingAnimationTimer > 0)
		aRow = 2;
	g->DrawImageMirror(IMAGE_WALTER, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

void Sexy::FishTypePet::DrawCommonPet(Graphics* g, Image* theImage, bool mirror)
{
	if (m0x245)
		g->DrawImage(IMAGE_ZZZ, mirror ? 50 : 30, -10);

	int aRow = 0;
	if (mTurnAnimationTimer != 0)
	{
		aRow = 1;
		mirror = mTurnAnimationTimer > 0;
	}
	g->DrawImageMirror(theImage, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
}

