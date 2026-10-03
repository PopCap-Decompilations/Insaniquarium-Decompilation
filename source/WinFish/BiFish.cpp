#include <SexyAppFramework/WidgetManager.h>

#include "BiFish.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Missle.h"
#include "Shadow.h"
#include "Coin.h"
#include "Res.h"

Sexy::BiFish::BiFish()
{
	m0x230 = 0;
	m0x234 = 0;
	mType = TYPE_BI_FISH;
}

Sexy::BiFish::BiFish(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	BiFish::Init(theX, theY);
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
}

Sexy::BiFish::~BiFish()
{
}

void Sexy::BiFish::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[0]++;
}

void Sexy::BiFish::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	mApp->mBoard->PlayChompSound(mVoracious);
	mHunger += 700;
	if (mHunger > 1000)
		mHunger = 1000;
	if (mApp->m0x882 && obj->mType != TYPE_FOOD)
	{
		for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
		{
			// The original draws the y offset before the x offset
			int aShotX, aShotY;
			if (obj->mType == TYPE_ULTRA)
			{
				aShotY = mApp->mSeed->Next() % 20 + obj->mY + 55;
				aShotX = mApp->mSeed->Next() % 20 + obj->mX + 55;
			}
			else
			{
				aShotY = mApp->mSeed->Next() % 20 + obj->mY + 15;
				aShotX = mApp->mSeed->Next() % 20 + obj->mX + 15;
			}
			mApp->mBoard->SpawnShot(aShotX, aShotY, 1);
		}
	}
	if (mVirtualTankId >= 0)
		UpdateMentalState();
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::BiFish::UpdateStoreAnimation()
{
	UpdateStoreCounters();
	if (mStoreAnimationTimer >= 20)
		mStoreAnimationTimer = 0;
	if (m0x230 == 0)
		mStoreAnimationIndex = mStoreAnimationTimer / 2;
	else
	{
		mStoreAnimationIndex = mStoreAnimationTimer - 10;
		if (mStoreAnimationIndex < 0)
			mStoreAnimationIndex += 1;
		mStoreAnimationIndex = abs(mStoreAnimationIndex);
	}
}

void Sexy::BiFish::DrawStoreAnimation(Graphics* g, int justification)
{
	int anXTrans = 0, anYTrans = 0;
	int aRow = 2;
	if (m0x230 == 0)
	{
		aRow = 5;
		switch (justification)
		{
		case 0:
			anXTrans = 15;
			anYTrans = 20;
			break;
		case 1:
			anXTrans = 5;
			anYTrans = 15;
			break;
		case 2:
			anXTrans = 15;
			anYTrans = 5;
			break;
		case 3:
			anXTrans = 5;
			anYTrans = 5;
			break;
		case 4:
			anXTrans = -40;
			anYTrans = -5;
			break;
		}
	}
	else
	{
		switch (justification)
		{
		case 0:
			anXTrans = 15;
			anYTrans = 20;
			break;
		case 1:
			anXTrans = 5;
			anYTrans = 15;
			break;
		case 2:
			anXTrans = 15;
			anYTrans = 0;
			break;
		case 3:
			anXTrans = 5;
			anYTrans = 0;
			break;
		case 4:
			anXTrans = -40;
			anYTrans = -5;
			break;
		}
	}

	g->Translate(anXTrans, anYTrans);
	SetStoreColor(g, Color::White);
	g->DrawImage(IMAGE_BILATERUS, 0, 0, Rect(mStoreAnimationIndex * 80, aRow * 80, 80, 80));
	g->SetColorizeImages(false);
	g->Translate(-anXTrans, -anYTrans);
}

void Sexy::BiFish::Sync(DataSync* theSync)
{
	Fish::Sync(theSync);
	theSync->SyncLong(m0x230);
	theSync->SyncLong(m0x234);
}

void Sexy::BiFish::DropCoin()
{
	mCoinDropTimer++;
	if (!CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
		return;

	mCoinDropTimer = 0;
	if (RelaxModeCanDrop())
		mApp->mBoard->DropCoin(mX + 5, mY + 10, CoinTypes::COIN_GOLD_C, nullptr, -1.0, 0);
}

bool Sexy::BiFish::Hungry()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();
	if (mHunger < 500)
		return HungryBehavior();
	return false;
}

void Sexy::BiFish::DrawFish(Graphics* g, bool mirror)
{
	int unk1 = 3;
	int unk2 = 4;
	int aRow = 5;
	if (m0x230 == 1)
	{
		unk1 = 0;
		unk2 = 1;
		aRow = 2;
	}
	if (mTurnAnimationTimer != 0)
	{
		aRow = unk2;
		if (m0x234 != 0)
		{
			mirror = !mirror;
			aRow = unk1;
		}
	}
	// The source rect is constructed separately for each draw, as the original
	if (mInvisible)
	{
		Rect aSrcRect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80);
		if (DrawInvisibleEffect(g, IMAGE_BILATERUS, aSrcRect, mirror))
			return;
	}

	SetColorHelper(g, Color::White);
	g->DrawImageMirror(IMAGE_BILATERUS, 0, 0, Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80), mirror);
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, 0, -10, 2);
}

bool Sexy::BiFish::HungryBehavior()
{
	GameObject* aNearestFood = FindNearestFood();

	int aCenterX = mX + mWidth / 2;
	int aCenterY = mY + mHeight / 2;

	if (mSpecialMovementStateChangeTimer >= 3)
	{
		if (!aNearestFood)
			return false;
		mSpecialMovementStateChangeTimer = 0;
		// The food's position is read in each comparison: mY only after the mVX steering, as the original
		if (mHunger > 300)
		{
			if (aCenterX > aNearestFood->mX + 44) {
				if (mVX > -3.0) mVX -= 1.0;
			}
			else if (aCenterX < aNearestFood->mX + 36) {
				if (mVX < 3.0) mVX += 1.0;
			}
			else if (aCenterX > aNearestFood->mX + 42) {
				if (mVX > -3.0) mVX -= 0.1;
			}
			else if (aCenterX < aNearestFood->mX + 38) {
				if (mVX < 3.0) mVX += 0.1;
			}
			else if (aCenterX > aNearestFood->mX + 40) {
				if (mVX > -3.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 40) {
				if (mVX < 3.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 43) {
				if (mVY > -3.0) mVY -= 1.0;
			}
			else if (aCenterY < aNearestFood->mY + 37) {
				if (mVY < 3.0) mVY += 1.0;
			}
			else if (aCenterY > aNearestFood->mY + 40) {
				if (mVY > -3.0) mVY -= 0.5;
			}
			else if (aCenterY < aNearestFood->mY + 40) {
				if (mVY < 3.0) mVY += 0.5;
			}
		}
		else
		{
			if (aCenterX > aNearestFood->mX + 44) {
				if (mVX > -4.0) mVX -= 1.3;
			}
			else if (aCenterX < aNearestFood->mX + 36) {
				if (mVX < 4.0) mVX += 1.3;
			}
			else if (aCenterX > aNearestFood->mX + 42) {
				if (mVX > -4.0) mVX -= 0.2;
			}
			else if (aCenterX < aNearestFood->mX + 38) {
				if (mVX < 4.0) mVX += 0.2;
			}
			else if (aCenterX > aNearestFood->mX + 40) {
				if (mVX > -4.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 40) {
				if (mVX < 4.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 40) {
				if (mVY > -4.0) mVY -= 1.3;
			}
			else if (aCenterY < aNearestFood->mY + 40) {
				if (mVY < 4.0) mVY += 1.3;
			}
		}
		if (mVXAbs < 5)
			mVXAbs++;
	}
	if (aNearestFood)
		CollideWithFood();
	return aNearestFood != nullptr;
}

Sexy::GameObject* Sexy::BiFish::FindNearestFood()
{
	int aXCenter = mX + mWidth / 2;
	int aYCenter = mY + mHeight / 2;
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(aXCenter, aYCenter);

	Board* aBoard = mApp->mBoard;
	int aDist = 100000000;
	GameObject* aRet = nullptr;

	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 && anObj->mType == TYPE_GUPPY)
		{
			int ax = aXCenter - anObj->mWidth / 2 - anObj->mX;
			int ay = aYCenter - anObj->mHeight / 2 - anObj->mY;
			int aNewDist = ax * ax + ay * ay;
			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = anObj;
			}
		}
	}
	if (aDist < 10000)
	{
		VoraciousScream(175);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::BiFish::CollideWithFood()
{
	int aCenterX = mX + mWidth / 2;
	int aCenterY = mY + mHeight / 2;
	if (mExoticDietFoodType != 0)
	{
		if (ExoticFoodCollision(aCenterX, aCenterY) > 0)
			ShowInvisibility();
		return;
	}

	Board* aBoard = mApp->mBoard;
	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 && anObj->mType == TYPE_GUPPY)
		{
			int ax = aCenterX - anObj->mWidth / 2 - anObj->mX;
			int ay = aCenterY - anObj->mHeight / 2 - anObj->mY;
			if (ax > -30 && ax < 30 && ay > -30 && ay < 30)
			{
				ShowInvisibility();
				OnFoodAte(anObj);
				anObj->Remove();
				return;
			}
		}
	}
}

void Sexy::BiFish::RemoveFromGame(bool removeShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (removeShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
	mApp->mBoard->m0x484++;
}

void Sexy::BiFish::FishUpdateAnimation()
{
	if (mInvisible)
		UpdateInvisible();

	if (mPrevVX > 0.0 && mVX < 0.0 && mTurnAnimationTimer == 0)
	{
		mTurnAnimationTimer = 20;
		m0x234 = (m0x234 + 1) % 2;
	}
	else if (mPrevVX < 0.0 && mVX > 0.0 && mTurnAnimationTimer == 0)
	{
		mTurnAnimationTimer = -20;
		m0x234 = (m0x234 + 1) % 2;
	}

	if (mTurnAnimationTimer > 0)
		mTurnAnimationTimer--;
	else if (mTurnAnimationTimer < 0)
		mTurnAnimationTimer++;

	if (mTurnAnimationTimer == 0)
	{
		mSwimFrameCounter++;
		if (mSwimFrameCounter >= 20)
			mSwimFrameCounter = 0;

		if (m0x230 == 0)
		{
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
		}
		else
		{
			mAnimationFrameIndexFish = mSwimFrameCounter - 10;
			if (mAnimationFrameIndexFish < 0)
				mAnimationFrameIndexFish += 1;
			mAnimationFrameIndexFish = abs(mAnimationFrameIndexFish);
		}
	}
	else if (mTurnAnimationTimer > 0)
	{
		mAnimationFrameIndexFish = 9 - mTurnAnimationTimer/2;
	}
	else if (mTurnAnimationTimer < 0)
	{
		mAnimationFrameIndexFish = mTurnAnimationTimer / 2 + 9;
	}

	if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
		mPrevVX = mVX;

	if (mVoraciousScreamCounter > 100 && mTurnAnimationTimer == 0)
		mAnimationFrameIndexFish = (m0x230 != 1 ? 0 : 9);

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::BiFish::Init(int theX, int theY)
{
	mType = TYPE_BI_FISH;
	m0x230 = Rand() % 2;
	m0x234 = 0;
	mIsGuppy = false;
	mSize = SIZE_SMALL;
	mYMin = 105;
	mYMax = 360;
	mMouseVisible = gUnkBool06;
	mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 250 + 200);
	mWidth = 80;
	mHeight = 80;
}
