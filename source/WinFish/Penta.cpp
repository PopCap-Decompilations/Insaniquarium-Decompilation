#include <SexyAppFramework/WidgetManager.h>

#include "Penta.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Shadow.h"
#include "Missle.h"
#include "Coin.h"
#include "Res.h"

Sexy::Penta::Penta()
{
	mClip = false;
	mType = TYPE_PENTA;
}

Sexy::Penta::Penta(int theX)
{
	Penta::Init();
	mXD = theX;
	mX = (int)mXD; // converted back from the double, as the original (0x4EC0CB)
}

Sexy::Penta::Penta(int theX, int theY)
{
	Penta::Init();
	mXD = theX;
	mX = theX;
	mY = theY;
	mYD = theY;
}

Sexy::Penta::~Penta()
{
}

void Sexy::Penta::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	if (!HungryLogic())
	{
		if (mSpeedState == 0)
			mTargetVX = 0;
		else if(mSpeedState == 1)
			mTargetVX = -0.5;
		else if (mSpeedState == 2)
			mTargetVX = 0.5;
		else if (mSpeedState == 3)
			mTargetVX = -1.0;
		else if (mSpeedState == 4)
			mTargetVX = 1.0;
		else if (mSpeedState == 5)
			mTargetVX = 0;
		else if (mSpeedState == 6)
			mTargetVX = 0;
		else if (mSpeedState == 7)
			mTargetVX = -2.5;
		else if (mSpeedState == 8)
			mTargetVX = 2.5;

		if (mVX > mTargetVX)
			mVX -= 0.1;
		if (mVX < mTargetVX)
			mVX += 0.1;
	}

	mEatFoodDelayTimer++;
	mSpeedChangeTimer++;
	if (mSpeedChangeTimer > 20 || mXD <= 10.0 || mXD >= 540.0)
	{
		mSpeedChangeTimer = 0;
		if (mApp->mSeed->Next() % 10 == 0)
			mSpeedState = mApp->mSeed->Next() % 9;
	}

	if (mXD > 550.0)
		mXD = 550.0;
	if (mXD < 10.0)
		mXD = 10.0;
	if (mYD > 359.0)
	{
		mYD = 359.0;
		mVY = 0.0;
	}
	if (mBoughtTimer > 0)
	{
		int aVal = 5;
		if (mBoughtTimer > 35)
			aVal = 8;
		if (mApp->mSeed->Next() % aVal == 0)
		{
			// The x offset is drawn before the y offset, as the original
			int aXOffset = 30 - mApp->mSeed->Next() % 60;
			int aYOffset = 30 - mApp->mSeed->Next() % 60;
			mApp->mBoard->SpawnBubble(mX + aXOffset + 25, mY + aYOffset + 25);
		}
		mBoughtTimer--;
	}
	else
	{
		if (mYD < 95.0)
			mYD = 95.0;
	}

	if (mXD > 535.0 && mVX > 0.1)
		mVX -= 0.1;
	if (mXD < 15.0 && mVX < -0.1)
		mVX += 0.1;
	if (mYD < 359.0)
		mVY += 0.4;

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

void Sexy::Penta::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	DrawPenta(g);
	if (!mName.empty())
		DrawName(g, false);
}

void Sexy::Penta::MouseDown(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
}

void Sexy::Penta::MouseDrag(int x, int y)
{
	// 0x4D8C10, shared with Board/Fish/Grubber/Breeder/OtherTypePet: forwards to the empty Widget::MouseDrag
	GameObject::MouseDrag(x, y);
}

void Sexy::Penta::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[1]++;
}

int Sexy::Penta::SpecialReturnValue()
{
	return 4;
}

void Sexy::Penta::Remove()
{
	Remove(true);
}

void Sexy::Penta::SetPosition(int newX, int newY)
{
	mX = newX;
	mXD = newX;
	mY = newY;
	mYD = newY;
}

void Sexy::Penta::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	if (mVirtualTankId >= 0)
		UpdateMentalState();
	mHunger += 900;
	if (mHunger > 1300)
		mHunger = 1300;

	if (DropCoin())
		mApp->mBoard->PlaySample(SOUND_BUY_ID, 3, 1.0);
	else
		mApp->mBoard->PlaySlurpSound(mVoracious);
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::Penta::UpdateStoreAnimation()
{
	UpdateStoreCounters();
	mStoreAnimationIndex = (mStoreAnimationTimer / 2) % 10;
}

void Sexy::Penta::DrawStoreAnimation(Graphics* g, int justification)
{
	int anX = 0, anY = 0;
	switch (justification)
	{
	case 0:
		anX = 20;
		anY = 17;
		break;
	case 1:
		anX = 5;
		anY = 17;
		break;
	case 2:
		anX = 15;
		anY = -5;
		break;
	case 3:
		anX = 5;
		anY = -1;
		break;
	case 4:
		anX = -40;
		anY = 0;
		break;
	}
	g->Translate(anX, anY);
	SetStoreColor(g, Color::White);
	g->DrawImage(IMAGE_STARCATCHER, 0, 0, Rect(mStoreAnimationIndex * 80, 0, 80, 80));
	g->SetColorizeImages(false);
	g->Translate(-anX, -anY);
}

void Sexy::Penta::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncDouble(mSpeedMod);
	theSync->SyncLong(mBoughtTimer);
	theSync->SyncDouble(mTargetVX);
	theSync->SyncLong(mSpeedState);
	theSync->SyncLong(mEatFoodDelayTimer);
	theSync->SyncLong(mSpeedChangeTimer);
	theSync->SyncLong(mMovementAnimTimer);
	theSync->SyncLong(mPentaAnimIndex);
	theSync->SyncLong(mCoinDropTimer);
	theSync->SyncLong(m0x1a8);
}

void Sexy::Penta::Init()
{
	mClip = false;
	mType = TYPE_PENTA;
	mYD = mApp->mSeed->Next() % 5 + 360;
	mY = mYD;
	mVX = 0;
	mVY = 0;
	mTargetVX = 0;
	mWidth = 80;
	mHeight = 80;
	// One unsigned remainder, tested twice, as the original (0x4D8D8C)
	int aRandVal = mApp->mSeed->Next() % 3;
	if (aRandVal == 0)
		mSpeedMod = 2.7;
	else if (aRandVal == 1)
		mSpeedMod = 2.5;
	else
		mSpeedMod = 2.6;

	mHunger = mApp->mSeed->Next() % 200 + 900;
	// The speed state is drawn before the timers are reset, as the original (0x4D8DE5)
	mSpeedState = mApp->mSeed->Next() % 10;
	mSpeedChangeTimer = 0;
	mMovementAnimTimer = 0;
	mPentaAnimIndex = 0;
	mCoinDropTimer = 0;
	mEatFoodDelayTimer = 40;
	m0x1a8 = 1;
	mMouseVisible = gUnkBool06;
	mBoughtTimer = 45;
}

bool Sexy::Penta::HungryLogic()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();

	if (ShouldDie())
		Die(true);
	else
	{
		if (mHunger < 900)
			return HungryBehavior();
	}
	return false;
}

bool Sexy::Penta::HungryBehavior()
{
	GameObject* aNearestFood = FindNearestFood();
	if (mEatFoodDelayTimer >= 5 && aNearestFood != nullptr)
	{
		mEatFoodDelayTimer = 0;
		double aX = mXD + 40.0; // compared untruncated, as the original
		int aFX = aNearestFood->mX;
		// The original tests the slower, less hungry case first (0x4EFF9A)
		if (mHunger > 300)
		{
			if (aX > aFX + 46)
			{
				if (mVX > -4.5)
					mVX -= 1.8;
			}
			else if (aX < aFX + 26)
			{
				if (mVX < 4.5)
					mVX += 1.8;
			}
			else if (aX > aFX + 36)
			{
				if (mVX > -1.5)
					mVX -= 1.0;
			}
			else if (aX < aFX + 36)
			{
				if (mVX < 1.5)
					mVX += 1.0;
			}
		}
		else
		{
			if (aX > aFX + 46)
			{
				if (mVX > -6.0)
					mVX -= 2.5;
			}
			else if(aX < aFX + 26)
			{
				if (mVX < 6.0)
					mVX += 2.5;
			}
			else if (aX > aFX + 36)
			{
				if (mVX > -2.5)
					mVX -= 1.2;
			}
			else if (aX < aFX + 36)
			{
				if (mVX < 2.5)
					mVX += 1.2;
			}
		}
	}
	if (aNearestFood)
		CollideWithFood();
	return aNearestFood != nullptr;
}

void Sexy::Penta::Die(bool flag)
{
	if (flag)
		mApp->mBoard->PlayDieSound(mType);
	Remove(false);
	bool isFacingRight = (mVX > 0.0);
	mApp->mBoard->SpawnDeadFish(mXD, mYD, mVX, 0.0, mSpeedMod, mType, isFacingRight, mShadowPtr); // no vertical speed, as the original
}

void Sexy::Penta::DrawPenta(Graphics* g)
{
	if (gZombieMode)
	{
		g->DrawImageMirror(IMAGE_STARCATCHER, 0, 0, Rect((IsHungryVisible() ? 4 : 9) * 80, 160, 80, 80), false);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);
		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, -10, 2);
		return;
	}

	// Only the row is kept; each draw builds its own rectangle, as the original (0x4E2EEE)
	int aRow = IsHungryVisible() ? 80 : 0;
	if (mInvisible)
	{
		Rect aSrcRect(mPentaAnimIndex * 80, aRow, 80, 80);
		if (DrawInvisibleEffect(g, IMAGE_STARCATCHER, aSrcRect, false))
			return;
	}
	SetColorHelper(g, Color::White);
	g->DrawImage(IMAGE_STARCATCHER, 0, 0, Rect(mPentaAnimIndex * 80, aRow, 80, 80));
	if (mHungerAnimationTimer != 0)
	{
		SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer * 255 / 5));
		g->DrawImage(IMAGE_STARCATCHER, 0, 0, Rect(mPentaAnimIndex * 80, 80, 80, 80));
	}
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, 0, -10, 2);
}

void Sexy::Penta::Remove(bool removeShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (removeShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
	mApp->mBoard->m0x490++;
	if (mApp->mRelaxMode)
	{
		if (ShouldDie())
			mApp->mBoard->ShowText("The purple guys eat coins!", false, 34);
	}
	else
	{
		if (mApp->mBoard->IsTankAndLevelNB(2, 2) && ShouldDie())
		{
			if(mApp->mBoard->mMessageShown[35])
				mApp->mBoard->ShowText("Hint: Star potions allow guppies to produce stars!", false, 36);
			if(mApp->mBoard->mMessageShown[34])
				mApp->mBoard->ShowText("Hint: Starcatchers need stars to stay alive!", false, 35);
			mApp->mBoard->ShowText("Warning! Your starcatcher has died!", false, 34);
		}
	}
}

bool Sexy::Penta::DropCoin()
{
	mCoinDropTimer++;
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && mCoinDropTimer < 3)
		return false;

	mCoinDropTimer = 0;
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && (!CanDropCoin() || !mApp->mBoard->m0x500))
		return false;
	if (!RelaxModeCanDrop())
		return false;

	mApp->mBoard->DropCoin(mX + 5, mY - 12, COIN_DIAMOND_PENTA, nullptr, -1.0, 0);
	return true;
}

Sexy::GameObject* Sexy::Penta::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2);

	// The original keeps the list pointer for the whole loop (0x4EC1C5)
	std::vector<Coin*>* aCoinList = mApp->mBoard->mCoinList;
	int aDist = 100000000;
	GameObject* aRet = nullptr;
	for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
	{
		if (IsCoinValid(*it))
		{
			// Coin minus fish, in double, truncated afterwards, as the original
			int ax = (int)(((*it)->mX + 36) - (mXD + 40.0));
			int ay = (int)(((*it)->mY + 36) - (mYD + 40.0));
			int aNewDist = ax * ax + ay * ay;

			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = *it;
			}
		}
	}
	if (aDist < 10000)
	{
		mSpeedySpeedState = 100;
	}
	return aRet;
}

bool Sexy::Penta::IsCoinValid(Coin* aCoin)
{
	if (aCoin->m0x198)
		return false;
	int aType = aCoin->mCoinType;
	if (mApp->mRelaxMode)
	{
		if (aType == 2 || aType == 1)
			return true;
	}
	else
	{
		if (aType == 3 || aType == 10 || aType == 18)
			return true;
	}
	return false;
}

void Sexy::Penta::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		if (ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2) > 0)
			ShowInvisibility();
	}
	else
	{
		// The original keeps the list pointer for the whole loop (0x4EC344)
		std::vector<Coin*>* aCoinList = mApp->mBoard->mCoinList;
		for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
		{
			// The position is tested before the coin itself, as the original
			if (mXD + 40.0 < (*it)->mX + 56 && mXD + 40.0 > (*it)->mX + 16 &&
				mYD + 40.0 < (*it)->mY + 66 && mYD + 40.0 > (*it)->mY + 16 &&
				IsCoinValid(*it))
			{
				Coin* aCoin = *it; // read once, before the calls (0x4EC47B)
				ShowInvisibility();
				aCoin->RemoveCoin();
				OnFoodAte(aCoin);
				return;
			}
		}
	}
}

void Sexy::Penta::UpdateAnimations()
{
	if (mInvisible)
		UpdateInvisible();

	if (mVX >= 1.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer + 1) % 20;
		mPentaAnimIndex = mMovementAnimTimer / 2;
	}
	else if (mVX <= -1.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer - 1) % 20;
		if (mMovementAnimTimer < 0)
			mMovementAnimTimer += 20;
		mPentaAnimIndex = mMovementAnimTimer / 2;
	}
	else if (mVX > 0.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer + 1) % 40;
		mPentaAnimIndex = mMovementAnimTimer / 4;
	}
	else
	{
		mMovementAnimTimer = (mMovementAnimTimer - 1) % 40;
		if (mMovementAnimTimer < 0)
			mMovementAnimTimer += 40;
		mPentaAnimIndex = mMovementAnimTimer / 4;
	}

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}
