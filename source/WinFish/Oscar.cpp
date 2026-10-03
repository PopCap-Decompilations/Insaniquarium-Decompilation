#include <SexyAppFramework/WidgetManager.h>

#include "Oscar.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Coin.h"
#include "Missle.h"
#include "Shadow.h"
#include "Res.h"

using namespace Sexy;

Sexy::Oscar::Oscar() : Fish()
{
	mType = TYPE_OSCAR;
}

Sexy::Oscar::Oscar(int theX, int theY) : Fish(theX, theY)
{
	Init();
}

Sexy::Oscar::Oscar(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	Init();
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

void Sexy::Oscar::Init()
{
	mType = TYPE_OSCAR;
	mSize = TYPE_OSCAR;
	mIsGuppy = false;
	mYMin = 105;
	mYMax = 360;
	mHunger = mApp->mSeed->Next() % 200 + 600;
	mMouseVisible = gUnkBool06;
	if (mApp->mRelaxMode)
	{
		SetFishColors(Rand() % 12 + 30, false);
		mHasSpecialColors = true;
	}
}

void Sexy::Oscar::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[0]++;
}

void Sexy::Oscar::OnFoodAte(GameObject* obj)
{
	mApp->mBoard->PlayChompSound(mVoracious);
	bool wasHungry = IsHungryVisible();
	GameObject::Unk02(false);
	if (mVirtualTankId >= 0)
		UpdateMentalState();
	mHunger += 900;
	if (mHunger > 1300)
		mHunger = 1300;

	if (mApp->m0x882 && obj->mType != TYPE_FOOD)
	{
		for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
		{
			int aY = mApp->mSeed->Next() % 20 + obj->mY + 15; // y drawn before x, as the original
			int aX = mApp->mSeed->Next() % 20 + obj->mX + 15;
			mApp->mBoard->SpawnShot(aX, aY, 1);
		}
	}

	UpdateHungerStateIfWasHungry(wasHungry);
	if (mPreNamedTypeId == JOHNNYV)
	{
		int aRandVal = Rand();
		if (aRandVal % 3 == 0)
		{
			mApp->mBoard->MakeNote(mX + mWidth / 2 - 40, mY - 10, 2, "This is my last pizza,");
			mApp->mBoard->MakeNote(mX + mWidth / 2, mY + 5, 2, "I swear!");
		}
	}
}

void Sexy::Oscar::DrawStoreAnimation(Graphics* g, int unk)
{
	int anXTrans = 0, anYTrans = 0;
	switch (unk)
	{
	case 0:
		anXTrans = 17;
		anYTrans = 17;
		break;
	case 1:
		anXTrans = 7;
		anYTrans = 17;
		break;
	case 2:
		anXTrans = 17;
		break;
	case 3:
		anXTrans = 8;
		anYTrans = 2;
		break;
	case 4:
		anXTrans = -40;
		break;
	}
	g->Translate(anXTrans, anYTrans);
	// Each draw builds its own source rect after setting the colour, as the original
	if (mHasSpecialColors)
	{
		SetStoreColor(g, Color::White);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, Rect(mStoreAnimationIndex * 80, 240, 80, 80), false);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);

		SetStoreColor(g, mColors[1]);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, Rect(mStoreAnimationIndex * 80, 80, 80, 80), false);

		SetStoreColor(g, mColors[0]);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, Rect(mStoreAnimationIndex * 80, 0, 80, 80), false);

		SetStoreColor(g, mColors[2]);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, Rect(mStoreAnimationIndex * 80, 160, 80, 80), false);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else
	{
		SetStoreColor(g, Color::White);
		g->DrawImage(IMAGE_SMALLSWIM, 0, 0, Rect(mStoreAnimationIndex * 80, 320, 80, 80));
	}
	g->SetColorizeImages(false);
	g->Translate(-anXTrans, -anYTrans);
}

void Sexy::Oscar::DropCoin()
{
	mCoinDropTimer++;
	if (CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
	{
		mCoinDropTimer = 0;
		if (RelaxModeCanDrop())
			mApp->mBoard->DropCoin(mX + 5, mY + 10, COIN_DIAMOND, nullptr, -1.0, 0);
	}
}

bool Sexy::Oscar::Hungry()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
	{
		UpdateHungerCounter();
	}

	if (ShouldDie())
		Die(true);
	else
	{
		if (mHunger < 500)
			return HungryBehavior();
	}
	return false;
}

void Sexy::Oscar::DrawFish(Graphics* g, bool mirror)
{
	if (gZombieMode)
	{
		bool aHungr = IsHungryVisible();
		g->DrawImageMirror(IMAGE_SMALLDIE, 0, 0, Rect((aHungr ? 6 : 9) * 80, 320, 80, 80), mirror);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);
		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, -10, 2);
		return;
	}

	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, 320, 80, 80);
	bool aHungr = IsHungryVisible();

	if (mInvisible)
	{
		Image* anImg = GetImageToDraw(aHungr);
		if (DrawInvisibleEffect(g, anImg, aSrcRect, mirror))
			return;
	}

	if (mHasSpecialColors && !aHungr)
	{
		SetColorHelper(g, Color(255, 255, 255, 255));

		int aRow = 0;
		if (mTurnAnimationTimer != 0)
			aRow = 4;
		else if (mEatingAnimationTimer > 0 || mVoraciousScreamCounter > 100)
			aRow = 8;

		Rect aSrcRect2 = Rect(mAnimationFrameIndexFish * 80, (aRow * 5 + 15) * 16, 80, 80);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, aSrcRect2, mirror);

		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		SetColorHelper(g, mColors[1]);
		aSrcRect2 = Rect(mAnimationFrameIndexFish * 80, (aRow * 80) + 80, 80, 80);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, aSrcRect2, mirror);
		SetColorHelper(g, mColors[0]);

		aSrcRect2 = Rect(mAnimationFrameIndexFish * 80, (aRow * 80), 80, 80);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, aSrcRect2, mirror);
		SetColorHelper(g, mColors[2]);
		aSrcRect2 = Rect(mAnimationFrameIndexFish * 80, (aRow * 5 + 10) * 16, 80, 80);
		g->DrawImageMirror(IMAGE_SIMCARNIVORE, 0, 0, aSrcRect2, mirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else
	{
		SetColorHelper(g, Color::White);
		g->DrawImageMirror(GetImageToDraw(aHungr), 0, 0, aSrcRect, mirror);
	}

	g->SetColorizeImages(false);
	if (mHungerAnimationTimer != 0)
	{
		Image* anImg = GetImageToDraw(true); // fetched before the colour is set, as the original
		SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer * 255 / 5));
		g->DrawImageMirror(anImg, 0, 0, aSrcRect, mirror);
		g->SetColorizeImages(false);
	}

	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);

	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, 0, -10, 2);
}

bool Sexy::Oscar::HungryBehavior()
{
	GameObject* aFood = FindNearestFood();
	int aX = mXD + 40.0;
	int aY = mYD + 50.0;
	if (mExoticDietFoodType != 0)
		aY += 20;

	if (mSpecialMovementStateChangeTimer >= 3)
	{
		if (aFood == nullptr)
			return false;
		mSpecialMovementStateChangeTimer = 0;
		int aFX = aFood->mX;
		// The food's mY is read only after the mVX steering, as the original
		if (mHunger > 300)
		{
			if (aFX + 48 < aX)
			{
				if (mVX > -4.0)
					mVX -= 1.5;
			}
			else if (aFX + 32 > aX)
			{
				if (mVX < 4.0)
					mVX += 1.5;
			}
			else if (aFX + 44 < aX)
			{
				if (mVX > -4.0)
					mVX -= 0.2;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 4.0)
					mVX += 0.2;
			}
			else if (aFX + 40 < aX)
			{
				if (mVX > -4.0)
					mVX -= 0.1;
			}
			else if (aFX + 40 > aX)
			{
				if (mVX < 4.0)
					mVX += 0.1;
			}

			if (aFood->mY + 46 < aY)
			{
				if (mVY > -3.0)
					mVY -= 0.8;
			}
			else if (aFood->mY + 34 > aY)
			{
				if (mVY < 4.0)
					mVY += 1.3;
			}
			else if (aFood->mY + 40 < aY)
			{
				if (mVY > -3.0)
					mVY -= 0.3;
			}
			else if (aFood->mY + 40 > aY)
			{
				if (mVY < 4.0)
					mVY += 0.5;
			}
		}
		else
		{
			if (aFX + 48 < aX)
			{
				if (mVX > -5.0)
					mVX -= 1.5;
			}
			else if (aFX + 32 > aX)
			{
				if (mVX < 5.0)
					mVX += 1.5;
			}
			else if (aFX + 44 < aX)
			{
				if (mVX > -5.0)
					mVX -= 0.2;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 5.0)
					mVX += 0.2;
			}
			else if (aFX + 40 < aX)
			{
				if (mVX > -5.0)
					mVX -= 0.05;
			}
			else if (aFX + 40 > aX)
			{
				if (mVX < 5.0)
					mVX += 0.05;
			}

			if (aFood->mY + 46 < aY)
			{
				if (mVY > -3.0)
					mVY -= 1.3;
			}
			else if (aFood->mY + 34 > aY)
			{
				if (mVY < 4.0)
					mVY += 1.5;
			}
			else if (aFood->mY + 40 < aY)
			{
				if (mVY > -3.0)
					mVY -= 0.5;
			}
			else if (aFood->mY + 40 > aY)
			{
				if (mVY < 4.0)
					mVY += 0.7;
			}
		}
		if (mVXAbs < 5)
			mVXAbs++;
	}
	if (aFood != nullptr)
		CollideWithFood();
	return aFood != nullptr;
}

GameObject* Sexy::Oscar::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2 + 20);

	// The list is fetched once before the loop, as the original
	std::vector<Fish*>* aFishList = mApp->mBoard->mFishList;
	int aDist = 100000000;
	Fish* aRet = nullptr;
	for (std::vector<Fish*>::iterator it = aFishList->begin(); it != aFishList->end(); ++it)
	{
		// The fish's centre minus this one's, truncated from the doubles, and the distance is
		// tested before the fish, as the original
		int ax = (int)(((*it)->mX + 40) - (mXD + 40.0));
		int ay = (int)(((*it)->mY + 40) - (mYD + 40.0));
		int aNewDist = ax * ax + ay * ay;
		if (aNewDist < aDist && (*it)->mSize == TYPE_GUPPY && !(*it)->CantBeEaten())
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

void Sexy::Oscar::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		int aVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2 + 20);
		if (mEatingAnimationTimer == 0)
		{
			if (aVal == 1)
			{
				mEatingAnimationTimer = 8;
				ShowInvisibility();
			}
			else if (aVal == 2)
			{
				mEatingAnimationTimer = 20;
				ShowInvisibility();
			}
		}
		return;
	}

	// The list is fetched once before the loop, and the fish is read through the iterator for
	// each test, as the original
	std::vector<Fish*>* aFishList = mApp->mBoard->mFishList;
	for (std::vector<Fish*>::iterator it = aFishList->begin(); it != aFishList->end(); ++it)
	{
		if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
			mYD + 45.0 < (*it)->mY + 58 && mYD + 45.0 > (*it)->mY + 22 && (*it)->mSize == TYPE_GUPPY)
		{
			if (!(*it)->CantBeEaten())
			{
				OnFoodAte(*it);
				(*it)->RemoveFromGame(true);
				if (mEatingAnimationTimer == 0)
				{
					ShowInvisibility();
					mEatingAnimationTimer = 8;
				}
				return;
			}
		}
		else if (mEatingAnimationTimer == 0 && mXD + 40.0 < (*it)->mX + 87 && mXD + 40.0 > (*it)->mX - 7
			&& mYD + 45.0 < (*it)->mY + 70 && mYD + 45.0 > (*it)->mY + 10 && (*it)->mSize == TYPE_GUPPY && !(*it)->CantBeEaten())
		{
			mEatingAnimationTimer = 20;
			ShowInvisibility();
		}
	}
}

void Sexy::Oscar::RemoveFromGame(bool aRemoveShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (aRemoveShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
	mApp->mBoard->m0x468++;
	if (mApp->mBoard->IsTankAndLevelNB(1, 3))
	{
		if (mApp->mBoard->mMessageShown[28])
			mApp->mBoard->ShowText("Try feeding small guppies to your carnivores!", false, 29);
		if (mApp->mBoard->mMessageShown[27])
			mApp->mBoard->ShowText("Hint: Carnivores won\'t eat fish food!", false, 28);
		mApp->mBoard->ShowText("Warning! Your carnivore has died!", false, 27);
	}
}

