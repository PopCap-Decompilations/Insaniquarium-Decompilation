#include <SexyAppFramework/WidgetManager.h>

#include "BallFish.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Missle.h"
#include "Shadow.h"
#include "Food.h"
#include "Res.h"

Sexy::BallFish::BallFish()
{
	mType = TYPE_BALL_FISH;
}

Sexy::BallFish::BallFish(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	BallFish::Init(theX, theY);
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

Sexy::BallFish::~BallFish()
{
}

void Sexy::BallFish::CountRequiredFood(int* theFoodReqPtr)
{
	if (mSize == SIZE_SMALL)
		theFoodReqPtr[7]++;
	else if(mSize == SIZE_MEDIUM)
		theFoodReqPtr[5]++;
	else
		theFoodReqPtr[6]++;
}

int Sexy::BallFish::GetShellPrice()
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

void Sexy::BallFish::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	mApp->mBoard->PlaySlurpSound(mVoracious);
	mHunger += 700;
	if (mHunger > 1000)
		mHunger = 1000;
	if (UpdateMentalState())
	{
		mFoodAte++;
		if (mSize < SIZE_LARGE && mFoodNeededToGrow <= mFoodAte)
		{
			mSize++;
			mFoodAte = 0;
			mGrowthAnimationTimer = 10;
			mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
		}
	}
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::BallFish::UpdateStoreAnimation()
{
	UpdateStoreCounters();
	mStoreAnimationIndex = (mStoreAnimationTimer / 4) % 10;
}

void Sexy::BallFish::DrawStoreAnimation(Graphics* g, int justification)
{
	int anXTrans = 0, anYTrans = 0;
	int aRow = 0;
	if (mSize == SIZE_MEDIUM)
		aRow = 2;
	else if (mSize == SIZE_LARGE)
		aRow = 1;
	switch (justification)
	{
	case 0:
		anXTrans = 35;
		anYTrans = 35;
		break;
	case 1:
		anXTrans = 20;
		anYTrans = 30;
		break;
	case 2:
		anXTrans = 30;
		anYTrans = 10;
		break;
	case 3:
		anXTrans = 20;
		anYTrans = 15;
		break;
	case 4:
		anXTrans = -28;
		anYTrans = 10;
		break;
	}

	g->Translate(anXTrans, anYTrans);
	SetStoreColor(g, Color::White);
	g->DrawImage(IMAGE_BALLS, 0, 0, Rect(mStoreAnimationIndex * 50, aRow * 50, 50, 50));
	g->SetColorizeImages(false);
	g->Translate(-anXTrans, -anYTrans);
}

void Sexy::BallFish::DropCoin()
{
	mCoinDropTimer++;
	if (!CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
		return;
	mCoinDropTimer = 0;
	if (RelaxModeCanDrop())
	{
		int aShellType = 1;
		if (mSize == SIZE_MEDIUM)
			aShellType = 2;
		else if (mSize == SIZE_LARGE)
			aShellType = 4;
		mApp->mBoard->DropCoin(mX + 5, mY + 10, aShellType, nullptr, -1.0, 0);
	}
}

bool Sexy::BallFish::Hungry()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();
	if (mHunger < 500)
		return HungryBehavior();
	return false;
}

void Sexy::BallFish::DrawFish(Graphics* g, bool mirror)
{
	int aRow = 0;
	if (mSize == SIZE_MEDIUM)
		aRow = 2;
	else if (mSize == SIZE_LARGE)
		aRow = 1;

	Rect aSrcRect(mAnimationFrameIndexFish * 50, aRow * 50, 50, 50);
	if(mInvisible && mGrowthAnimationTimer == 0)
		if (DrawInvisibleEffect(g, IMAGE_BALLS, aSrcRect, mirror))
			return;

	SetColorHelper(g, Color::White);
	if (mGrowthAnimationTimer > 0)
	{
		// Float constants, evaluated in double and stored in a float, as the original
		float aGrowthVal;
		if (mGrowthAnimationTimer > 3)
			aGrowthVal = (double)(10 - mGrowthAnimationTimer) * 0.7f / 7.0 + 0.5;
		else
			aGrowthVal = (double)mGrowthAnimationTimer * 0.2f / 3.0 + 1.0;

		int aVal = (int)(0.5 * ((aGrowthVal - 1.0) * 160.0));

		Rect aDestRect(-aVal, -aVal, aVal * 2 + 50, aVal * 2 + 50);

		g->SetFastStretch(!mApp->Is3DAccelerated());
		DrawImageMirrorHelper(g, IMAGE_BALLS, aDestRect, aSrcRect, mirror);
	}
	else
		g->DrawImageMirror(IMAGE_BALLS, 0, 0, aSrcRect, mirror);
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, -15, -20, 2);
}

bool Sexy::BallFish::HungryBehavior()
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
			if (aCenterX > aNearestFood->mX + 24) {
				if (mVX > -3.0) mVX -= 1.0;
			}
			else if (aCenterX < aNearestFood->mX + 16) {
				if (mVX < 3.0) mVX += 1.0;
			}
			else if (aCenterX > aNearestFood->mX + 22) {
				if (mVX > -3.0) mVX -= 0.1;
			}
			else if (aCenterX < aNearestFood->mX + 18) {
				if (mVX < 3.0) mVX += 0.1;
			}
			else if (aCenterX > aNearestFood->mX + 20) {
				if (mVX > -3.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 20) {
				if (mVX < 3.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 23) {
				if (mVY > -3.0) mVY -= 1.0;
			}
			else if (aCenterY < aNearestFood->mY + 17) {
				if (mVY < 3.0) mVY += 1.0;
			}
			else if (aCenterY > aNearestFood->mY + 20) {
				if (mVY > -3.0) mVY -= 0.5;
			}
			else if (aCenterY < aNearestFood->mY + 20) {
				if (mVY < 3.0) mVY += 0.5;
			}
		}
		else
		{
			if (aCenterX > aNearestFood->mX + 24) {
				if (mVX > -4.0) mVX -= 1.3;
			}
			else if (aCenterX < aNearestFood->mX + 16) {
				if (mVX < 4.0) mVX += 1.3;
			}
			else if (aCenterX > aNearestFood->mX + 22) {
				if (mVX > -4.0) mVX -= 0.2;
			}
			else if (aCenterX < aNearestFood->mX + 18) {
				if (mVX < 4.0) mVX += 0.2;
			}
			else if (aCenterX > aNearestFood->mX + 20) {
				if (mVX > -4.0) mVX -= 0.05;
			}
			else if (aCenterX < aNearestFood->mX + 20) {
				if (mVX < 4.0) mVX += 0.05;
			}

			if (aCenterY > aNearestFood->mY + 20) {
				if (mVY > -4.0) mVY -= 1.3;
			}
			else if (aCenterY < aNearestFood->mY + 20) {
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

Sexy::GameObject* Sexy::BallFish::FindNearestFood()
{
	int aXCenter = mX + mWidth / 2;
	int aYCenter = mY + mHeight / 2;
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(aXCenter, aYCenter);

	int aDist = 100000000;
	GameObject* aRet = nullptr;
	int aFoodType = GetFoodType();
	Board* aBoard = mApp->mBoard;

	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 && anObj->mType == TYPE_FOOD)
		{
			if (CanBallFishEatFood(anObj, aFoodType))
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
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::BallFish::CollideWithFood()
{
	int aCenterX = mX + mWidth / 2;
	int aCenterY = mY + mHeight / 2;
	if (mExoticDietFoodType != 0)
	{
		if (ExoticFoodCollision(aCenterX, aCenterY) > 0)
			ShowInvisibility();
		return;
	}

	int aFoodType = GetFoodType();
	Board* aBoard = mApp->mBoard;
	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 &&
			anObj->mType == TYPE_FOOD && CanBallFishEatFood(anObj, aFoodType))
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

void Sexy::BallFish::RemoveFromGame(bool removeShadow)
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

void Sexy::BallFish::FishUpdateAnimation()
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

	if (mVX < -1.0)
		mSwimFrameCounter -= 3;
	else if(mVX > 1.0)
		mSwimFrameCounter += 3;
	else if (mVX > 0.5)
		mSwimFrameCounter += 2;
	else if (mVX < -0.5)
		mSwimFrameCounter -= 2;
	else if (mVX >= 0.0)
		mSwimFrameCounter++;
	else
		mSwimFrameCounter--;

	int aFrameSpeed = mSize != SIZE_MEDIUM ? 6 : 12;
	if (mSwimFrameCounter < 0)
		mSwimFrameCounter += aFrameSpeed * 10;

	mAnimationFrameIndexFish = (mSwimFrameCounter / aFrameSpeed) % 10;

	if (mGrowthAnimationTimer > 0)
		mGrowthAnimationTimer--;

	if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
		mPrevVX = mVX;

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::BallFish::Init(int theX, int theY)
{
	mType = TYPE_BALL_FISH;
	mIsGuppy = false;
	mSize = SIZE_SMALL;
	mYMin = 105;
	mYMax = 360;
	mMouseVisible = gUnkBool06;
	mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 250 + 200);
	mWidth = 50;
	mHeight = 50;
}

int Sexy::BallFish::GetFoodType()
{
	switch (mSize)
	{
	case SIZE_SMALL:
		return EXO_FOOD_ICE_CREAM;
	case SIZE_MEDIUM:
		return EXO_FOOD_CHICKEN;
	default:
		return EXO_FOOD_PIZZA;
	}
}

bool Sexy::BallFish::CanBallFishEatFood(GameObject* theFood, int theExoticFoodTypePreffered)
{
	if (theFood->mType != TYPE_FOOD)
		return false;
	Food* aFood = (Food*)theFood;
	if (aFood->mCantEatTimer == 0 && aFood->mExoticFoodType == theExoticFoodTypePreffered)
		return true;
	return false;
}
