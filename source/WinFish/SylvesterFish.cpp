#include <SexyAppFramework/WidgetManager.h>

#include "SylvesterFish.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Missle.h"
#include "Shadow.h"
#include "Coin.h"
#include "Res.h"

Sexy::SylvesterFish::SylvesterFish()
{
	mType = TYPE_SYLVESTER_FISH;
}

Sexy::SylvesterFish::SylvesterFish(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	SylvesterFish::Init(theX, theY);
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

Sexy::SylvesterFish::~SylvesterFish()
{
}

void Sexy::SylvesterFish::CountRequiredFood(int* theFoodReqPtr)
{
	if (mSize == SIZE_MEDIUM)
		theFoodReqPtr[3]++;
	else
		theFoodReqPtr[4]++;
}

int Sexy::SylvesterFish::GetShellPrice()
{
	int aVal = GameObject::GetShellPrice();
	if (aVal > 0 && mSize == SIZE_LARGE)
		aVal *= 3;
	return aVal;
}

void Sexy::SylvesterFish::OnFoodAte(GameObject* obj)
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

	if (UpdateMentalState())
	{
		mFoodAte++;
		if (mSize == SIZE_MEDIUM && mFoodNeededToGrow <= mFoodAte)
		{
			GrowSylvester(SIZE_LARGE);
			mFoodAte = 0;
			mGrowthAnimationTimer = 10;
			mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
		}
	}

	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::SylvesterFish::DrawStoreAnimation(Graphics* g, int justification)
{
	int anXTrans = 0, anYTrans = 0;
	Image* anImg;
	int aWHSize;
	if (mSize == SIZE_MEDIUM)
	{
		anImg = IMAGE_MINISYLV;
		aWHSize = 80;
		switch (justification)
		{
		case 0:
			anXTrans = 20;
			anYTrans = 17;
			break;
		case 1:
			anXTrans = 15;
			anYTrans = 17;
			break;
		case 2:
			anXTrans = 18;
			anYTrans = 0;
			break;
		case 3:
			anXTrans = 12;
			anYTrans = 5;
			break;
		case 4:
			anXTrans = -40;
			break;
		}
	}
	else
	{
		anImg = IMAGE_SYLV;
		aWHSize = 160;
		switch (justification)
		{
		case 0:
			anXTrans = -20;
			anYTrans = 0;
			break;
		case 1:
			anXTrans = -25;
			anYTrans = 0;
			break;
		case 2:
			anXTrans = -20;
			anYTrans = -10;
			break;
		case 3:
			anXTrans = -30;
			anYTrans = -10;
			break;
		case 4:
			anXTrans = -80;
			anYTrans = -20;
			break;
		}
	}
	
	g->Translate(anXTrans, anYTrans);
	SetStoreColor(g, Color::White);
	g->DrawImage(anImg, 0, 0, Rect(mStoreAnimationIndex * aWHSize, 0, aWHSize, aWHSize));
	g->SetColorizeImages(false);
	g->Translate(-anXTrans, -anYTrans);
}

void Sexy::SylvesterFish::DropCoin()
{
	mCoinDropTimer++;
	if (CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
	{
		mCoinDropTimer = 0;
		if (RelaxModeCanDrop())
			mApp->mBoard->DropCoin(mX + 5, mY + 10, mSize == SIZE_MEDIUM ? COIN_PEARL : COIN_TREASURE, nullptr, -1.0, 0);
	}
}

bool Sexy::SylvesterFish::Hungry()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();
	if (mHunger < 500)
		return HungryBehavior();
	return false;
}

void Sexy::SylvesterFish::DrawFish(Graphics* g, bool mirror)
{
	Image* anImg;
	int aWHSize;
	if (mSize == SIZE_MEDIUM)
	{
		anImg = IMAGE_MINISYLV;
		aWHSize = 80;
	}
	else
	{
		anImg = IMAGE_SYLV;
		aWHSize = 160;
	}
	Rect aSrcRect(mAnimationFrameIndexFish * aWHSize, aWHSize * (mTurnAnimationTimer != 0), aWHSize, aWHSize);
	if(mInvisible && mGrowthAnimationTimer == 0)
		if (DrawInvisibleEffect(g, anImg, aSrcRect, mirror))
			return;

	SetColorHelper(g, Color::White);
	if (mGrowthAnimationTimer > 0)
	{
		// Float constants, evaluated in double and stored in a float, as the original
		float aGrowthVal;
		if (mGrowthAnimationTimer > 3)
			aGrowthVal = (double)(10 - mGrowthAnimationTimer) * 0.7f / 7.0 + 0.6f;
		else
			aGrowthVal = (double)mGrowthAnimationTimer * 0.3f / 3.0 + 1.0;

		int aVal = (int)((aGrowthVal - 1.0) * 160.0 * 0.5);

		Rect aDestRect(-aVal, -aVal, aVal * 2 + 160, aVal * 2 + 160);

		g->SetFastStretch(!mApp->Is3DAccelerated());
		DrawImageMirrorHelper(g, anImg, aDestRect, aSrcRect, mirror);
	}
	else
		g->DrawImageMirror(anImg, 0, 0, aSrcRect, mirror);
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, -10, -15, 2);
}

bool Sexy::SylvesterFish::HungryBehavior()
{
	GameObject* aNearestFood = FindNearestFood();

	int aCenterX = mX + mWidth/2;
	int aCenterY = mY + mHeight/2;

	if (mSize != SIZE_MEDIUM)
		aCenterY -= 30;

	if (mSpecialMovementStateChangeTimer >= 3)
	{
		if (!aNearestFood)
			return false;
		mSpecialMovementStateChangeTimer = 0;
		// The food's position is read in each comparison: mY only after the mVX steering, as the original
		if (mHunger > 300)
		{
			if (aCenterX > aNearestFood->mX + 40) {
				if (mVX > -3.0) mVX -= 1.0;
			}
			else if (aCenterX < aNearestFood->mX + 32) {
				if (mVX < 3.0) mVX += 1.0;
			}
			else if (aCenterX > aNearestFood->mX + 38) {
				if (mVX > -3.0) mVX -= 0.1;
			}
			else if (aCenterX < aNearestFood->mX + 34) {
				if (mVX < 3.0) mVX += 0.1;
			}
			else if (aCenterX > aNearestFood->mX + 36) {
				if (mVX > -3.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 36) {
				if (mVX < 3.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 39) {
				if (mVY > -3.0) mVY -= 1.0;
			}
			else if (aCenterY < aNearestFood->mY + 33) {
				if (mVY < 3.0) mVY += 1.0;
			}
			else if (aCenterY > aNearestFood->mY + 36) {
				if (mVY > -3.0) mVY -= 0.5;
			}
			else if (aCenterY < aNearestFood->mY + 36) {
				if (mVY < 3.0) mVY += 0.5;
			}
		}
		else
		{
			if (aCenterX > aNearestFood->mX + 40) {
				if (mVX > -4.0) mVX -= 1.3;
			}
			else if (aCenterX < aNearestFood->mX + 32) {
				if (mVX < 4.0) mVX += 1.3;
			}
			else if (aCenterX > aNearestFood->mX + 38) {
				if (mVX > -4.0) mVX -= 0.2;
			}
			else if (aCenterX < aNearestFood->mX + 34) {
				if (mVX < 4.0) mVX += 0.2;
			}
			else if (aCenterX > aNearestFood->mX + 36) {
				if (mVX > -4.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 36) {
				if (mVX < 4.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 36) {
				if (mVY > -4.0) mVY -= 1.3;
			}
			else if (aCenterY < aNearestFood->mY + 36) {
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

Sexy::GameObject* Sexy::SylvesterFish::FindNearestFood()
{
	int aXCenter = mX + mWidth / 2;
	int aYCenter = mY + mHeight / 2;
	if (mSize != SIZE_MEDIUM)
		aYCenter -= 30;
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(aXCenter, aYCenter);

	Board* aBoard = mApp->mBoard;
	int aDist = 100000000;
	GameObject* aRet = nullptr;

	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0)
		{
			// A medium Sylvester eats carnivores, a large one ultravores
			bool aCanEat = false;
			switch (anObj->mType)
			{
			case TYPE_OSCAR:
				aCanEat = mSize == SIZE_MEDIUM;
				break;
			case TYPE_ULTRA:
				aCanEat = mSize != SIZE_MEDIUM;
				break;
			}
			if (aCanEat)
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
	}
	if (aDist < 10000)
	{
		ShowInvisibility();
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::SylvesterFish::CollideWithFood()
{
	int aCenterX = mX + mWidth / 2;
	int aCenterY = mY + mHeight / 2;
	if (mSize != SIZE_MEDIUM)
		aCenterY -= 30;
	if (mExoticDietFoodType != 0)
	{
		ExoticFoodCollision(aCenterX, aCenterY);
		return;
	}

	Board* aBoard = mApp->mBoard;
	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0)
		{
			// A medium Sylvester eats carnivores, a large one ultravores (from further away)
			int aDist = 30;
			bool aCanEat = false;
			switch (anObj->mType)
			{
			case TYPE_OSCAR:
				aCanEat = mSize == SIZE_MEDIUM;
				break;
			case TYPE_ULTRA:
				aDist = 70;
				aCanEat = mSize != SIZE_MEDIUM;
				break;
			}
			if (aCanEat)
			{
				int ax = aCenterX - anObj->mWidth / 2 - anObj->mX;
				int ay = aCenterY - anObj->mHeight / 2 - anObj->mY;
				if (ax > -aDist && ax < aDist && ay > -aDist && ay < aDist)
				{
					OnFoodAte(anObj);
					anObj->Remove();
					return;
				}
			}
		}
	}
}

void Sexy::SylvesterFish::RemoveFromGame(bool removeShadow)
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

void Sexy::SylvesterFish::FishUpdateAnimation()
{
	if (mInvisible)
		UpdateInvisible();

	if (mPrevVX > 0.0 && mVX < 0.0)
		mTurnAnimationTimer = 10;
	else if (mPrevVX < 0.0 && mVX > 0.0)
		mTurnAnimationTimer = -10;

	if (mTurnAnimationTimer > 0)
		mTurnAnimationTimer--;
	else if (mTurnAnimationTimer < 0)
		mTurnAnimationTimer++;

	double aSpeedMod = mSpeedMod;
	if (mSpeedy)
	{
		if (mSpeedySpeedState != 0)
			aSpeedMod = 0.8;
		else
			aSpeedMod = 0.3;
	}

	if (mTurnAnimationTimer == 0)
	{
		if (mVX < -1.6 || mVX > 1.6)
		{
			mSwimFrameCounter++;
			if (mSwimFrameCounter <= 3 || mSwimFrameCounter <= 6)
			{
				mAnimationFrameIndexFish = mSwimFrameCounter / 2;
				mXD -= (mVX / aSpeedMod) * 0.5 * 0.5;
				mYD -= (mVY / aSpeedMod) * 0.5 * 0.5;
			}
			else if (mSwimFrameCounter <= 10)
			{
				mAnimationFrameIndexFish = mSwimFrameCounter / 2;
				mXD += (mVX / aSpeedMod) * 0.5 * 1.5;
				mYD += (mVY / aSpeedMod) * 0.5 * 1.5;
			}
			else if (mSwimFrameCounter <= 39)
			{
				mAnimationFrameIndexFish = 6;
				mXD += (mVX / aSpeedMod) * 0.5;
				mYD += (mVY / aSpeedMod) * 0.5;
			}
			else if (mSwimFrameCounter <= 50)
			{
				mAnimationFrameIndexFish = abs(mSwimFrameCounter - 50) / 2;
				mXD += (mVX / aSpeedMod) * 0.5;
				mYD += (mVY / aSpeedMod) * 0.5;
			}
			else
			{
				mAnimationFrameIndexFish = 0;
				mSwimFrameCounter = 0;
			}
		}
		else
		{
			mSwimFrameCounter++;
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
		}
	}
	else if(mTurnAnimationTimer > 0)
	{
		mAnimationFrameIndexFish = 9 - mTurnAnimationTimer;
	}
	else if(mTurnAnimationTimer < 0)
	{
		mAnimationFrameIndexFish = mTurnAnimationTimer + 10;
	}

	if (mGrowthAnimationTimer > 0)
		mGrowthAnimationTimer--;

	if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
		mPrevVX = mVX;

	if (mVoraciousScreamCounter > 100 && mTurnAnimationTimer == 0)
		mAnimationFrameIndexFish = 0;

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::SylvesterFish::Init(int theX, int theY)
{
	mType = TYPE_SYLVESTER_FISH;
	mIsGuppy = false;
	mSize = SIZE_MEDIUM;
	mYMin = 105;
	mYMax = 360;
	mMouseVisible = gUnkBool06;
	mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 250 + 200);
}

void Sexy::SylvesterFish::GrowSylvester(int theNewSize)
{
	if (theNewSize != SIZE_MEDIUM && theNewSize != SIZE_LARGE)
		return;

	mSize = theNewSize;
	if (theNewSize == SIZE_MEDIUM) 
	{
		mWidth = 80;
		mHeight = 80;
		mXD += 40.0;
		mYMin = 105;
		mYMax = 360;
		mXMin = 10;
		mYD += 40.0;
		mXMax = 540;
		if (mShadowPtr)
			mShadowPtr->mShadowSize = 0;
	}
	else
	{
		mWidth = 160;
		mHeight = 160;
		mXD -= 40.0;
		mYMax = 310;
		mYMin = 75;
		mXMin = 0;
		mYD -= 40.0;
		mXMax = 480;
		if (mShadowPtr)
			mShadowPtr->mShadowSize = 2;
	}
	mX = mXD;
	mY = mYD;
}
