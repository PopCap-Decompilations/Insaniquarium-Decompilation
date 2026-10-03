#include <SexyAppFramework/WidgetManager.h>

#include "Breeder.h"
#include "WinFishApp.h"
#include "Board.h"
#include "FishTypePet.h"
#include "Shadow.h"
#include "Food.h"
#include "Alien.h"
#include "Missle.h"
#include "Res.h"

#include <time.h>

Sexy::Breeder::Breeder()
{
	mClip = false;
	mType = TYPE_BREEDER;
}

Sexy::Breeder::Breeder(int theX, int theY) 
{
	Breeder::Init(theX, theY);
	mSize = SIZE_SMALL;
	m0x1d4 = mApp->mSeed->Next() % 400 + 1000;
}

Sexy::Breeder::Breeder(int theX, int theY, int theSize, bool velocityRight)
{
	Breeder::Init(theX, theY);
	if (velocityRight)
	{
		mVX = 1.0;
		mPrevVX = 1.0;
	}
	else
	{
		mVX = -1.0;
		mPrevVX = -1.0;
	}
	mSize = theSize;
	if (theSize == 0 || theSize == 1)
		m0x1d4 = mApp->mSeed->Next() % 400 + 1000;
	else
		m0x1d4 = mApp->mSeed->Next() % 200 + 800;
}

Sexy::Breeder::~Breeder()
{
}

void Sexy::Breeder::Update()
{
	if (mApp->mBoard == NULL || mApp->mBoard->mPause)
		return;
	UpdateCounters();

	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK && gUnkInt02 != 0 && mSize < 2)
	{
		// NaN takes the -0.5 path, as the original (0x4F41EF)
		if (mVX >= 0.0)
			mVX = 0.5;
		else
			mVX = -0.5;

		if (mBoughtTimer == 0)
			mVY = 0.0;

		if (mXD > gUnkInt05 + 50)
			mXD -= 5.0;
		else if(mXD < gUnkInt05 - 50)
			mXD += 5.0;
		else if(mXD > gUnkInt05 + 5)
			mXD -= 3.0;
		else if(mXD < gUnkInt05 - 5)
			mXD += 3.0;

		if (mYD > gUnkInt06 + 50)
			mYD -= 4.0;
		else if (mYD < gUnkInt06 - 50)
			mYD += 4.0;
		else if (mYD > gUnkInt06 + 5)
			mYD -= 3.0;
		else if (mYD < gUnkInt06 - 5)
			mYD += 3.0;
	}
	else if (!HungryLogic())
	{
		Board* aBoard = mApp->mBoard;
		if (aBoard->AliensInTank() && aBoard->mPetsInTank[PET_GUMBO] != 0)
		{
			// As the original: the search ends on mApp->mBoard's list, and the iterator is used
			// without an end check (0x4F43A0-0x4F4451)
			std::vector<FishTypePet*>::iterator it;
			for (it = aBoard->mFishTypePetList->begin(); it != mApp->mBoard->mFishTypePetList->end(); it++)
			{
				if ((*it)->mFishTypePetType == PET_GUMBO)
					break;
			}

			if (mXD + 40.0 > (*it)->mX + 50)
			{
				if (mVX > -4.0)
					mVX -= 1.3;
			}
			else if (mXD + 40.0 < (*it)->mX + 30)
			{
				if (mVX < 4.0)
					mVX += 1.3;
			}
			else if (mXD + 40.0 > (*it)->mX + 45)
			{
				if (mVX > -4.0)
					mVX -= 0.2;
			}
			else if (mXD + 40.0 < (*it)->mX + 35)
			{
				if (mVX < 4.0)
					mVX += 0.2;
			}
			else if (mXD + 40.0 > (*it)->mX + 40)
			{
				if (mVX > -4.0)
					mVX -= 0.05;
			}
			else if (mXD + 40.0 < (*it)->mX + 40)
			{
				if (mVX < 4.0)
					mVX += 0.05;
			}

			if (mYD + 40.0 > (*it)->mY + 25)
			{
				if (mVY > -3.0)
					mVY -= 1.0;
			}
			else if (mYD + 40.0 < (*it)->mY + 15)
			{
				if (mVY < 4.0)
					mVY += 1.3;
			}
			else if (mYD + 40.0 > (*it)->mY + 20)
			{
				if (mVY > -3.0)
					mVY -= 0.5;
			}
			else if (mYD + 40.0 < (*it)->mY + 20)
			{
				if (mVY < 4.0)
					mVY += 0.7;
			}

			if (mY <= mYMin && mVY < 0.0)
				mVY = 0.0;
			if (mVXAbs < 5)
				mVXAbs++;
		}
		else
		{
			if (mMovementState > 4)
			{
				if (mBoughtTimer == 0)
				{
					// NaN takes -0.5, as the original (0x4F47E0)
					if (mYD < 115.0)
						mVY = -0.1;
					else
						mVY = -0.5;
				}
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
				if (mBoughtTimer == 0)
					mVY = 0.5;
				if (mSpecialMovementStateChangeTimer >= 40)
				{
					mSpecialMovementStateChangeTimer = 0;
					if (mVX > 0.0)
					{
						if (mVX > 0.5)
							mVX -= 0.5;
					}
					else if (mVX < 0.0)
					{
						if (mVX < -0.5)
							mVX += 0.5;
					}

					mVXAbs = (int)abs(mVX);
				}

				mYD -= 0.25 / mSpeedMod;
			}
			else if (mMovementState == 1)
			{
				if (mBoughtTimer == 0)
					mVY = -0.5;
				if (mSpecialMovementStateChangeTimer >= 40)
				{
					mSpecialMovementStateChangeTimer = 0;
					if (mVX > 1.0)
						mVX--;
					else if (mVX < 1.0)
						mVX++;
					mVXAbs = (int)abs(mVX);
				}

				mYD -= 0.5 / mSpeedMod;
			}
			else if (mMovementState == 2)
			{
				if (mBoughtTimer == 0)
					mVY = -0.5;
				if (mSpecialMovementStateChangeTimer >= 40)
				{
					mSpecialMovementStateChangeTimer = 0;
					if (mVX > -1.0)
						mVX--;
					else if (mVX < -1.0)
						mVX++;

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
						mVX--;
					else if (mVX < -1.0)
						mVX++;

					if (mVY > 3.0)
						mVY--;
					else if (mVY < 3.0)
						mVY++;

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
						mVX--;
					else if (mVX < 1.0)
						mVX++;

					if (mVY > 3.0)
						mVY--;
					else if (mVY < 3.0)
						mVY++;

					if (mVXAbs > 4)
						mVXAbs--;
					else if (mVY < 4.0)
						mVXAbs++;
				}
				if (mYD > 240.0)
					mMovementState = 0;
			}
			else if (mMovementState == -1)
			{
				mVY = -15.0; // rising state, as the original
			}
		}
	}
	mSpecialMovementStateChangeTimer++;
	m0x1b8++;
	if (m0x1b8 > 20)
	{
		m0x1b8 = 0;
		if (mApp->mSeed->Next() % 10 == 0 || mMovementState == -1)
			mMovementState = mApp->mSeed->Next() % 9 + 1;
	}

	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		DropCoin();

	if (mBoughtTimer != 0)
	{
		mBoughtTimer--;
		mVY *= 0.9;
	}
	if (mVX == 0.0)
		mYD += 1.0 / mSpeedMod;
	if (mVX == 1.0)
		mYD += 0.75 / mSpeedMod;
	if (mVX == 2.0)
		mYD += 0.5 / mSpeedMod;
	if (mVX == 3.0)
		mYD += 0.25 / mSpeedMod;

	if (mYMax <= 320)
		mYD -= 0.25;
	if (mXD > (double)mXMax)
		mXD = (double)mXMax;
	if (mXD < (double)mXMin)
		mXD = (double)mXMin;
	if (mYD > (double)mYMax)
		mYD = (double)mYMax;

	if (mBoughtTimer > 0 && mVY > 0.0)
	{
		if (mBoughtTimer > 30)
		{
			int aChance = 2;
			if (mBoughtTimer > 40)
				aChance = 1;

			if (mApp->mSeed->Next() % aChance == 0)
			{
				// The x offset is drawn before the y offset, as the original
				int aXOffset = 15 - mApp->mSeed->Next() % 30;
				int aYOffset = 15 - mApp->mSeed->Next() % 30;
				mApp->mBoard->SpawnBubble(mX + aXOffset + 25, mY + aYOffset + 25);
			}
		}
	}
	else
	{
		if (mYD < (double)mYMin)
			mYD = (double)mYMin;
	}

	if (mYD <= mYMin && mMovementState == -1)
		mMovementState = mApp->mSeed->Next() % 9 + 1;

	if (mXD > mXMax - 5 && mVX > 0.1)
		mVX -= 0.1;
	if (mXD < 15.0 && mVX < -0.1)
		mVX += 0.1;

	UpdateAnimations();

	double aSpeedMod = mSpeedMod;
	if (mSpeedy)
	{
		if (mSpeedySpeedState != 0)
			aSpeedMod = 0.8;
		else
			aSpeedMod = 0.3;
	}

	mXD += mVX / aSpeedMod;
	mYD += mVY / aSpeedMod;

	Move(mXD, mYD);
}

void Sexy::Breeder::Draw(Graphics* g)
{
	UpdateFishSongMgr();

	if (gUnkInt02 != 0 && mXD < gUnkInt05 + 8 && mXD > gUnkInt05 - 8 &&
		mYD < gUnkInt06 + 8 && mYD > gUnkInt06 - 8 && mSize < SIZE_LARGE)
	{
		return;
	}
	// Unlike Fish::Draw, the original does not touch the shadow here

	if (mTurnAnimationTimer != 0)
	{
		if (mTurnAnimationTimer > 0)
			DrawBreeder(g, !mForwardlyChallenged);
		else if (mTurnAnimationTimer < 0)
			DrawBreeder(g, mForwardlyChallenged);
	}
	else if (mVX < 0.0 || ((int)mVX == 0 && mPrevVX < 0.0)) // facing left
		DrawBreeder(g, mForwardlyChallenged);
	else if (mVX > 0.0 || ((int)mVX == 0 && mPrevVX > 0.0)) // facing right
		DrawBreeder(g, !mForwardlyChallenged);
	// mVX == 0 and mPrevVX == 0: the original draws no breeder

	if (mName.size() > 0)
		DrawName(g, false);
}

void Sexy::Breeder::MouseDown(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
	{
		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
		return;
	}
	if (mApp->mBoard->mAlienList->size() == 0 && mApp->mBoard->mBilaterusList->size() == 0 &&
		mApp->mBoard->mMissleList1->size() == 0)
	{
		double absX = x + mXD;
		if (absX < 587.0 && absX > 30.0)
		{
			double absY = y + mYD;
			if (absY < 400.0 && absY > 60.0 &&
				!mApp->mBoard->Unk11(mX + x, mY + y) && mApp->mBoard->Buy(mApp->mBoard->m0x4ac, true))
			{
				if (x >= 10 && x <= 70 && y >= 10 && y <= 70)
					mApp->mBoard->DropFood(mXD + x - 10.0, mYD + y - 10.0, 0, false, (70 - y) / 2, -1);
				else
					mApp->mBoard->DropFood(mXD + x - 10.0, mYD + y - 10.0, 0, false, 0, -1);
				mApp->mBoard->m0x4ec = true;
				mApp->mBoard->m0x3c0 = mApp->mBoard->Unk01();
			}
		}
	}
}

void Sexy::Breeder::MouseUp(int x, int y, int theClickCount)
{
	GameObject::MouseUp(x, y, theClickCount);
	mApp->mBoard->m0x4ec = false;
	mApp->mBoard->m0x4ed = false;
}

void Sexy::Breeder::MouseDrag(int x, int y)
{
	// 0x4D8C10, shared with Board/Fish/Grubber/Penta/OtherTypePet: forwards to the empty Widget::MouseDrag
	GameObject::MouseDrag(x, y);
}

int Sexy::Breeder::GetShellPrice()
{
	int aVal = GameObject::GetShellPrice();
	if (aVal > 0)
	{
		switch (mSize)
		{
		case SIZE_MEDIUM:
			aVal *= 2;
			break;
		case SIZE_LARGE:
			aVal *= 3;
			break;
		}
	}
	return aVal;
}

void Sexy::Breeder::Remove()
{
	RemoveFromGame(true);
}

void Sexy::Breeder::SetPosition(int newX, int newY)
{
	mX = newX;
	mXD = newX;
	mY = newY;
	mYD = newY;
}

void Sexy::Breeder::OnFoodAte(GameObject* obj)
{
	Food* theFood = (Food*)obj;
	bool hungry = IsHungryVisible();
	Unk02(false);
	bool flag = true;
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (mVirtualTankId < 0)
			flag = false;
		else
			flag = UpdateMentalState();
	}

	mApp->mBoard->PlaySlurpSound(mVoracious);
	if (theFood->mFoodType == 0)
	{
		mHunger += 500;
		if (mHunger > 800)
			mHunger = 800;
		if (flag)
			m0x1a4++;
	}
	else if (theFood->mFoodType == 1)
	{
		mHunger += 700;
		if (mHunger > 1000)
			mHunger = 1000;
		if (flag)
			m0x1a4 += (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK ? 2 : 1);
	}
	else
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		{
			mHunger += 500;
			if (mHunger > 800)
				mHunger = 800;
		}
		else
		{
			mHunger += 1100;
			if (mHunger > 1400)
				mHunger = 1400;
		}
		if (flag)
			m0x1a4 += (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK ? 3 : 2);
	}

	if (m0x1a8 <= m0x1a4 && mSize < SIZE_LARGE)
	{
		mSize++;
		if (mSize == SIZE_MEDIUM)
		{
			m0x1d0 = 900;
			if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
				m0x1a8 = mApp->mSeed->Next() % 5 + 5;
			mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_BREEDER, true);
		}
		if (mSize == SIZE_LARGE)
		{
			if (mVirtualTankId >= 0)
				m0x1d0 = 0;
			m0x1d4 = mApp->mSeed->Next() % 200 + 500;
			if(mApp->mGameMode == GAMEMODE_TIME_TRIAL)
				mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_EGG, true);
		}
		m0x1a4 = 0;
		mGrowthAnimationTimer = 10;
		mApp->PlaySample(SOUND_GROW);
	}
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::Breeder::UpdateStoreAnimation()
{
	UpdateStoreCounters();
	mStoreAnimationIndex = (mStoreAnimationTimer / 2) % 10;
}

void Sexy::Breeder::DrawStoreAnimation(Graphics* g, int justification)
{
	int anX = 0, anY = 0;
	switch (justification)
	{
	case 0:
		anX = 18;
		anY = 20;
		break;
	case 1:
		anX = 7;
		anY = 20;
		break;
	case 2:
		anX = 15;
		anY = 0;
		break;
	case 3:
		anX = 12;
		anY = 0;
		break;
	case 4:
		anX = -40;
		anY = -5;
		break;
	}
	g->Translate(anX, anY);
	// The row is read before SetStoreColor, as the original (0x4DD1F0)
	int aRow = mSize * 3;
	SetStoreColor(g, Color::White);
	g->DrawImage(IMAGE_BREEDER, 0, 0, Rect(mStoreAnimationIndex * 80, aRow * 80, 80, 80));
	g->SetColorizeImages(false);
	g->Translate(-anX, -anY);
}

void Sexy::Breeder::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncLong(mXDirection);
	theSync->SyncLong(m0x17c);
	theSync->SyncDouble(mSpeedMod);
	theSync->SyncDouble(mPrevVX);
	theSync->SyncLong(mYMax);
	theSync->SyncLong(mXMin);
	theSync->SyncLong(mYMin);
	theSync->SyncLong(mXMax);
	theSync->SyncLong(mSize);
	theSync->SyncLong(m0x1a4);
	theSync->SyncLong(m0x1a8);
	theSync->SyncLong(mGrowthAnimationTimer);
	theSync->SyncLong(mMovementState);
	theSync->SyncLong(mSpecialMovementStateChangeTimer);
	theSync->SyncLong(m0x1b8);
	theSync->SyncLong(mSwimAnimationTimer);
	theSync->SyncLong(mVXAbs);
	theSync->SyncLong(mAnimationIndex);
	theSync->SyncLong(mTurnAnimationTimer);
	theSync->SyncLong(mEatingAnimationTimer);
	theSync->SyncLong(m0x1d0);
	theSync->SyncLong(m0x1d4);
	theSync->SyncLong(mBoughtTimer);
	if (theSync->mVersion >= 54)
		theSync->SyncLong(m0x1dc);
	else
		m0x1dc = 0;
}

void Sexy::Breeder::Init(int theX, int theY)
{
	mXD = theX;
	mClip = false;
	mYD = theY;
	mType = TYPE_BREEDER;
	m0x1dc = 0;
	mX = mXD;
	mY = mYD;
	mVX = 0;
	mVY = -0.5;
	mWidth = 80;
	mHeight = 80;
	mPrevVX = 1.0;
	if (mApp->mSeed->Next() % 2 == 0)
	{
		mVX = -0.1;
		mPrevVX = -1.0;
	}
	mXDirection = 1;
	m0x17c = 0;
	mYMax = 370;
	mYMin = 95;
	mXMin = 10;
	mXMax = 540;
	int aRand = mApp->mSeed->Next() % 3;
	if (aRand == 0)
		mSpeedMod = 2.0;
	else if (aRand == 1)
		mSpeedMod = 1.8;
	else
		mSpeedMod = 1.6;
	mHunger = mApp->mSeed->Next() % 200 + 400;
	m0x1a4 = 0;
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		m0x1a8 = Rand() % 9 + 12;
	else
		m0x1a8 = mApp->mSeed->Next() % 2 + 4;
	// Drawn before the stores below, as the original (0x4DCD6F)
	mMovementState = mApp->mSeed->Next() % 10;
	mSpecialMovementStateChangeTimer = 40;
	m0x1b8 = 0;
	mSwimAnimationTimer = 0;
	mVXAbs = 0;
	mAnimationIndex = 0;
	mTurnAnimationTimer = 0;
	mEatingAnimationTimer = 0;
	m0x1d0 = 0;
	mMouseVisible = true;
	if (mApp->mBoard != nullptr)
	{
		if(!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
			mMouseVisible = false;
	}
	mGrowthAnimationTimer = 0;
	mBoughtTimer = 0;
}

bool Sexy::Breeder::HungryLogic()
{
	UpdateHungerAnimCounter();
	// The original keeps the alien list across the calls below (0x4F1359, 0x4F13CB)
	std::vector<Alien*>* anAlienList = mApp->mBoard->mAlienList;
	if (anAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();

	if (ShouldDie())
	{
		Die(true);
		return false;
	}
	if (mHunger >= 500)
		return false;
	if (mApp->mBoard->AliensInTank())
	{
		if (anAlienList->empty() || anAlienList->front()->mAlienType == ALIEN_GUS)
			return false;
	}
	return HungryBehavior();
}

void Sexy::Breeder::Die(bool playDieSound)
{
	if (playDieSound)
		mApp->mBoard->PlayDieSound(mType);

	RemoveFromGame(false);
	bool isFacingRight = (mVX > 0.0);
	mApp->mBoard->SpawnDeadFish(mXD, mYD, mVX, mVY, mSpeedMod, mSize + TYPE_BREEDER, isFacingRight, mShadowPtr);
}

bool Sexy::Breeder::HungryBehavior()
{
	GameObject* aNearestFood = FindNearestFood();

	int aCenterX = mXD + 40.0;
	int aCenterY = mYD + 45.0;

	if (mSpecialMovementStateChangeTimer >= 3)
	{
		if (!aNearestFood)
			return false;
		mSpecialMovementStateChangeTimer = 0;
		int aFCX = aNearestFood->mX;
		int aFCY = aNearestFood->mY;
		if (mHunger > 300)
		{
			if (aCenterX > aFCX + 28) {
				if (mVX > -3.0) mVX -= 1.0;
			}
			else if (aCenterX < aFCX + 12) {
				if (mVX < 3.0) mVX += 1.0;
			}
			else if (aCenterX > aFCX + 24) {
				if (mVX > -3.0) mVX -= 0.1;
			}
			else if (aCenterX < aFCX + 16) {
				if (mVX < 3.0) mVX += 0.1;
			}
			else if (aCenterX > aFCX + 20) {
				if (mVX > -3.0) mVX -= 0.05;
			}
			else if (aCenterX < aFCX + 20) {
				if (mVX < 3.0) mVX += 0.05;
			}

			if (aCenterY > aFCY + 26) {
				if (mVY > -2.0) mVY -= 0.6;
			}
			else if (aCenterY < aFCY + 14) {
				if (mVY < 3.0) mVY += 1.0;
			}
			else if (aCenterY > aFCY + 20) {
				if (mVY > -2.0) mVY -= 0.3;
			}
			else if (aCenterY < aFCY + 20) {
				if (mVY < 3.0) mVY += 0.5;
			}
		}
		else
		{
			if (aCenterX > aFCX + 28) {
				if (mVX > -4.0) mVX -= 1.3;
			}
			else if (aCenterX < aFCX + 12) {
				if (mVX < 4.0) mVX += 1.3;
			}
			else if (aCenterX > aFCX + 24) {
				if (mVX > -4.0) mVX -= 0.2;
			}
			else if (aCenterX < aFCX + 16) {
				if (mVX < 4.0) mVX += 0.2;
			}
			else if (aCenterX > aFCX + 20) {
				if (mVX > -4.0) mVX -= 0.05;
			}
			else if (aCenterX < aFCX + 20) {
				if (mVX < 4.0) mVX += 0.05;
			}

			if (aCenterY > aFCY + 26) {
				if (mVY > -3.0) mVY -= 1.0;
			}
			else if (aCenterY < aFCY + 14) {
				if (mVY < 4.0) mVY += 1.3;
			}
			else if (aCenterY > aFCY + 20) {
				if (mVY > -3.0) mVY -= 0.5;
			}
			else if (aCenterY < aFCY + 20) {
				if (mVY < 4.0) mVY += 0.7;
			}
		}
		if (mVXAbs < 5)
			mVXAbs++;
	}
	if (aNearestFood)
		CollideWithFood();
	return aNearestFood != nullptr;
}

void Sexy::Breeder::DropCoin()
{
	if (mSize > SIZE_SMALL)
	{
		m0x1d0++;
		if (mInvisible && m0x1d0 == m0x1d4 - 30 && mSize == SIZE_LARGE)
			ShowInvisibility();

		if (m0x1d4 <= m0x1d0)
		{
			m0x1d0 = 0;
			if (RelaxModeCanDrop())
			{
				if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
				{
					if (GiveBirth())
						return;
				}
				if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
				{
					m0x1d4 = 2160;
					if (mSize != SIZE_LARGE)
						return;

					int aReqs[8];
					int aInTank[8];
					mApp->mBoard->GetExoticFoodsRequiredInTank(aReqs);
					mApp->mBoard->GetExoticFoodsInTank(aInTank);
					if (aReqs[0] - aInTank[0] + 1 <= 0)
						return;
				}
				Fish* aGuppy = mApp->mBoard->SpawnGuppy(mX + 5, mY + 10);

				if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
					aGuppy->mCanBeEatenDelay = mExoticDietFoodType != 1000 ? 40 : 150;
				mApp->mBoard->PlayBirthSound(true);
			}
		}
	}
}

void Sexy::Breeder::DecrementEatingAnimationTimer()
{
	mEatingAnimationTimer--;
}

void Sexy::Breeder::UpdateAnimations()
{
	if (mInvisible)
		UpdateInvisible();
	
	if (mPrevVX > 0.0 && mVX < 0.0)
		mTurnAnimationTimer = 20;
	else if (mPrevVX < 0.0 && mVX > 0.0)
		mTurnAnimationTimer = -20;

	// As the original (and Fish): the eating timer counts down while turning and when it
	// sets the frame, twice on the frame a turn ends.
	if (mTurnAnimationTimer > 0)
	{
		mTurnAnimationTimer--;
		if (mEatingAnimationTimer != 0)
			DecrementEatingAnimationTimer();
	}
	else if (mTurnAnimationTimer < 0)
	{
		mTurnAnimationTimer++;
		if (mEatingAnimationTimer != 0)
			DecrementEatingAnimationTimer();
	}

	if (mTurnAnimationTimer == 0)
	{
		if (mEatingAnimationTimer > 0)
		{
			DecrementEatingAnimationTimer();
			mAnimationIndex = 9 - (mEatingAnimationTimer / 2);
		}
		else
		{
			if (mVXAbs <= 1)
				mSwimAnimationTimer += 1;
			else
				mSwimAnimationTimer += 2;

			if (mSwimAnimationTimer >= 20)
				mSwimAnimationTimer = 0;
			mAnimationIndex = mSwimAnimationTimer / 2;
		}
	}
	else if (mTurnAnimationTimer > 0)
		mAnimationIndex = 9 - (mTurnAnimationTimer / 2);
	else if (mTurnAnimationTimer < 0)
		mAnimationIndex = mTurnAnimationTimer / 2 + 9;

	if (mVX != mPrevVX && mVX != 0.0 && mPrevVX != 0.0)
		mPrevVX = mVX;

	if (mMisslePtr)
		UpdateCrosshairAnimation();

	if (mVoraciousScreamCounter > 100 && mTurnAnimationTimer == 0)
		mAnimationIndex = 4;

	if (mGrowthAnimationTimer > 0)
		mGrowthAnimationTimer--;
}

Sexy::GameObject* Sexy::Breeder::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2);

	// The original keeps the list pointer for the whole loop (0x4EE6F8)
	std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;
	int aDist = 100000000;
	Food* aRet = nullptr;
	for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
	{
		// Differences in double, truncated afterwards, as the original; the distance is
		// tested before the food's type and timer
		int ax = (int)(((*it)->mX + 20) - (mXD + 40.0));
		int ay = (int)(((*it)->mY + 20) - (mYD + 40.0));
		int aNewDist = ax * ax + ay * ay;
		if (aNewDist < aDist && (*it)->mExoticFoodType != 2 && (*it)->mCantEatTimer == 0)
		{
			aDist = aNewDist;
			aRet = *it;
		}
	}
	if (aDist < 10000)
	{
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::Breeder::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		int aExoticFoodVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2);
		if (mEatingAnimationTimer == 0)
		{
			if (aExoticFoodVal == 1)
			{
				mEatingAnimationTimer = 8;
				ShowInvisibility();
			}
			else if (aExoticFoodVal == 2)
			{
				mEatingAnimationTimer = 20;
				ShowInvisibility();
			}
		}
	}
	else
	{
		// The original keeps the list pointer for the whole loop (0x4EE8D6)
		std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;
		for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
		{
			Food* aFood = *it;
			// The centre is compared untruncated, and recomputed for each food, as the original
			double aCenterX = mXD + 40.0;

			if (aCenterX < aFood->mX + 40 && aCenterX > aFood->mX &&
				mYD + 35.0 < aFood->mY + 35 && mYD + 35.0 > aFood->mY &&
				aFood->mExoticFoodType != 2 && aFood->mCantEatTimer == 0)
			{
				OnFoodAte(aFood);
				aFood->RemoveFood();
				if (mEatingAnimationTimer == 0)
				{
					mEatingAnimationTimer = 8;
					ShowInvisibility();
				}
				return; // one pellet per frame, as the original
			}

			if (mEatingAnimationTimer == 0 &&
				aCenterX < aFood->mX + 55 && aCenterX > aFood->mX - 15 &&
				mYD + 35.0 < aFood->mY + 40 && mYD + 35.0 > aFood->mY - 5 &&
				aFood->mCantEatTimer == 0)
			{
				mEatingAnimationTimer = 20;
				ShowInvisibility();
			}
		}
	}
}

bool Sexy::Breeder::CanGiveBirth(int* theVirtualTankId)
{
	if (mVirtualTankId < 0)
		return false;
	if (mSize != SIZE_LARGE)
		return false;
	int anId = mApp->mBoard->GetNextVirtualTankId();
	if (anId < 0 || m0x1dc > 0)
		return false;
	if (theVirtualTankId != nullptr)
		*theVirtualTankId = anId;
	return true;
}

bool Sexy::Breeder::GiveBirth()
{
	int aNextVirtId = 0;
	if (!CanGiveBirth(&aNextVirtId))
		return false;

	Fish* aGuppy = new Fish(mX + 5, mY + 10);
	mApp->mBoard->PlayBirthSound(false);
	aGuppy->BoughtSetup();
	aGuppy->CopyBreederDataVT(this);
	aGuppy->mTimeBought = time(NULL);
	aGuppy->mVirtualTankId = aNextVirtId;
	aGuppy->mName += " JR.";
	aGuppy->mHometownIdx = 0;
	aGuppy->mVirtualFish = true;
	if (aGuppy->mInvisible)
		aGuppy->ShowInvisibility();
	mApp->mBoard->SpawnGameObject(aGuppy, false);
	m0x1dc++;
	return true;
}

void Sexy::Breeder::RemoveFromGame(bool removeShadow)
{
	if (mVirtualTankId >= 0)
	{
		GameObject* aGameObject = mApp->mBoard->GetGameObjectByVirtualId(mVirtualTankId + 100);
		if (aGameObject)
			aGameObject->mVirtualTankId = mVirtualTankId;
	}
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (removeShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
}

void Sexy::Breeder::DrawBreeder(Graphics* g, bool mirror)
{
	if (gZombieMode)
	{
		// The row is read before IsHungryVisible, as the original (0x4DD297)
		g->DrawImageMirror(IMAGE_HUNGRYBREEDER, 0, 0, Rect((IsHungryVisible() ? 6 : 9) * 80, (mSize * 3 + 2) * 80, 80, 80), mirror);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);
		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
	}
	else
	{
		// Both rects are built before the calls, as the original (0x4DD32F)
		Rect aSrcRect(mAnimationIndex * 80, 0, 80, 80);
		Rect aDestRect(0, 0, 80, 80);
		bool hungry = IsHungryVisible();
		aSrcRect.mY = GetRowToDraw(hungry) * 80;
		if (mInvisible && mGrowthAnimationTimer == 0)
			if (DrawInvisibleEffect(g, hungry ? IMAGE_HUNGRYBREEDER : IMAGE_BREEDER, aSrcRect, mirror))
				return;

		if (mGrowthAnimationTimer > 0 && mSize != SIZE_STAR)
		{
			// Float constants, evaluated in double and stored in a float, as the original
			float aGrowthVal;
			if(mGrowthAnimationTimer > 3)
				aGrowthVal = (double)(10 - mGrowthAnimationTimer) * 0.5 / 7.0 + 0.7f;
			else
				aGrowthVal = (double)mGrowthAnimationTimer * 0.2f / 3.0 + 1.0;

			int aVal = (int)(0.5 * ((aGrowthVal - 1.0) * 80.0));

			aDestRect.mX -= aVal;
			aDestRect.mY -= aVal;
			aDestRect.mWidth += aVal * 2;
			aDestRect.mHeight += aVal * 2;

			g->SetFastStretch(!mApp->Is3DAccelerated());
		}

		SetColorHelper(g, Color::White);

		DrawImageMirrorHelper(g, hungry ? IMAGE_HUNGRYBREEDER : IMAGE_BREEDER, aDestRect, aSrcRect, mirror);
		if (mHungerAnimationTimer != 0)
		{
			aSrcRect.mY = GetRowToDraw(true) * 80;
			SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer * 255 / 5));
			DrawImageMirrorHelper(g, IMAGE_HUNGRYBREEDER, aDestRect, aSrcRect, mirror);
		}
		g->SetColorizeImages(false);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);

		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
	}
}

int Sexy::Breeder::GetRowToDraw(bool hungry)
{
	if (mTurnAnimationTimer != 0)
		return mSize * 3 + 1;
	if (mEatingAnimationTimer <= 0 && mVoraciousScreamCounter <= 100)
		return mSize * 3;
	if (hungry)
		return mSize + 9;
	return mSize * 3 + 2;
}
