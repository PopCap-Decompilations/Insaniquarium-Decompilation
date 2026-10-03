#include <SexyAppFramework/WidgetManager.h>

#include "Ultra.h"
#include "WinFishApp.h"
#include "Board.h"
#include "FishSongMgr.h"
#include "Oscar.h"
#include "Missle.h"
#include "Shadow.h"
#include "Coin.h"
#include "Res.h"

Sexy::Ultra::Ultra()
{
	mType = TYPE_ULTRA;
}

Sexy::Ultra::Ultra(int theX, int theY) : Fish(theX, theY)
{
	Ultra::Init(theX, theY);
}

Sexy::Ultra::Ultra(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	Ultra::Init(theX, theY);
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

Sexy::Ultra::~Ultra()
{
}

void Sexy::Ultra::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[3]++;
}

void Sexy::Ultra::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	if (mVirtualTankId >= 0)
	{
		UpdateMentalState();
		if (mPreNamedTypeId == KILGORE && mSongId != -1)
			mApp->mBoard->StopFishSong(mSongId);
	}
	mApp->mBoard->PlayChompSound(mVoracious);
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
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::Ultra::DrawStoreAnimation(Graphics* g, int justification)
{
	int anX = 0, anY = 0;
	switch (justification)
	{
	case 0:
		anX = 13;
		anY = -22;
		break;
	case 1:
		anX = 3;
		anY = -22;
		break;
	case 2:
		anX = 3;
		anY = -42;
		break;
	case 3:
		anX = 10;
		anY = -40;
		break;
	case 4:
		anX = -70;
		anY = -45;
		break;
	}
	g->Translate(anX, anY);
	// Kilgore's colour is constructed only for him; otherwise Color::White itself is passed, as the original
	SetStoreColor(g, mPreNamedTypeId == KILGORE ? Color(0xffaaff) : Color::White);
	g->DrawImageCel(IMAGE_ULTRA, 0, 0, mStoreAnimationIndex, 0);
	g->SetColorizeImages(false);
	g->Translate(-anX, -anY);
}

void Sexy::Ultra::DropCoin()
{
	mCoinDropTimer++;
	if (CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
	{
		mCoinDropTimer = 0;
		if (RelaxModeCanDrop())
			mApp->mBoard->DropCoin(mX + 40, mY + 90, COIN_TREASURE, nullptr, -1.0, 0);
	}
}

bool Sexy::Ultra::Hungry()
{
	UpdateHungerAnimCounter();
	if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		UpdateHungerCounter();
	if (ShouldDie())
		Die(true);
	else
	{
		if (mHunger < 500)
		{
			if (mPreNamedTypeId == KILGORE)
			{
				if (mHunger == 290 && m0x118 % 5 == 0 && mSongId == -1)
				{
					m0x114 = 5;
					return HungryBehavior();
				}
				if (mHunger >= 290 || ((m0x114 != 0 || mSongId != -1) && (unsigned int)mApp->mBoard->mFishSongMgr->Unk01(mSongId) <= 35000))
					return false;
			}
			return HungryBehavior();
		}
	}
	return false;
}

void Sexy::Ultra::DrawFish(Graphics* g, bool mirror)
{
	if (gZombieMode)
	{
		bool aHungr = IsHungryVisible();
		if (aHungr)
		{
			g->SetColorizeImages(true);
			g->SetColor(Color(0xfad75f));
		}
		g->DrawImageMirror(IMAGE_ULTRA, 0, 0, Rect(1440, 480, 160, 160), mirror);
		g->SetColorizeImages(false);
		if (mMisslePtr)
			DrawCrosshair(g, 40, 40);
		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 40, 0, 2);
		return;
	}

	Rect aSrcRect(mAnimationFrameIndexFish * 160, 0, 160, 160);
	if (mTurnAnimationTimer != 0)
		aSrcRect.mY = 320;
	else if (mEatingAnimationTimer > 0 || mVoraciousScreamCounter > 100)
		aSrcRect.mY = 160;
	else
		aSrcRect.mY = 0;

	Image* anImg = IMAGE_ULTRA; // read once and used for every draw below, as the original

	if (mInvisible)
		if (DrawInvisibleEffect(g, anImg, aSrcRect, mirror))
			return;

	if (IsHungryVisible())
	{
		SetColorHelper(g, Color(0xfa, 0xd7, 0x5f, 255));
		g->DrawImageMirror(anImg, 0, 0, aSrcRect, mirror);
	}
	else
	{
		// Kilgore's colour is constructed only for him; otherwise Color::White itself is passed, as the original
		SetColorHelper(g, mPreNamedTypeId == KILGORE ? Color(0xffaaff) : Color::White);
		g->DrawImageMirror(anImg, 0, 0, aSrcRect, mirror);
		if (mHungerAnimationTimer != 0)
		{
			SetColorHelper(g, Color(0xfa, 0xd7, 0x5f, mHungerAnimationTimer * 255 / 5));
			g->DrawImageMirror(anImg, 0, 0, aSrcRect, mirror);
		}
	}
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 40, 40);
	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, 40, 0, 2);
}

bool Sexy::Ultra::HungryBehavior()
{
	GameObject* aFood = FindNearestFood();
	int aX = mXD + 80.0;
	int aY = mYD + 100.0;
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
			if (aFX + 44 < aX)
			{
				if (mVX > -4.0)
					mVX -= 1.5;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 4.0)
					mVX += 1.5;
			}
			else if (aFX + 42 < aX)
			{
				if (mVX > -4.0)
					mVX -= 0.2;
			}
			else if (aFX + 38 > aX)
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

			if (aFood->mY + 43 < aY)
			{
				if (mVY > -3.0)
					mVY -= 0.8;
			}
			else if (aFood->mY + 37 > aY)
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
			if (aFX + 44 < aX)
			{
				if (mVX > -5.0)
					mVX -= 1.5;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 5.0)
					mVX += 1.5;
			}
			else if (aFX + 42 < aX)
			{
				if (mVX > -5.0)
					mVX -= 0.2;
			}
			else if (aFX + 38 > aX)
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

			if (aFood->mY + 40 < aY)
			{
				if (mVY > -3.0)
					mVY -= 1.3;
			}
			else if (aFood->mY + 40 > aY)
			{
				if (mVY < 4.0)
					mVY += 1.3;
			}
		}
		if (mVXAbs < 5)
			mVXAbs++;
	}
	if (aFood != nullptr)
		CollideWithFood();
	return aFood != nullptr;
}

Sexy::GameObject* Sexy::Ultra::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2 + 20);

	// The list is fetched once before the loop, as the original
	std::vector<Oscar*>* anOscarList = mApp->mBoard->mOscarList;
	int aDist = 100000000;
	Oscar* aRet = nullptr;
	for (std::vector<Oscar*>::iterator it = anOscarList->begin(); it != anOscarList->end(); ++it)
	{
		Oscar* aFish = *it;

		if (aFish->mVirtualTankId < 0 && aFish->mCanBeEatenDelay <= 0)
		{
			// The carnivore's centre minus this one's, truncated from the doubles, as the original
			int ax = (int)((aFish->mX + 40) - (mXD + 80.0));
			int ay = (int)((aFish->mY + 40) - (mYD + 80.0));
			int aNewDist = ax * ax + ay * ay;

			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = aFish;
			}
		}
	}
	if (aDist < 40000)
	{
		VoraciousScream(175);
	}
	if (aDist < 10000)
	{
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::Ultra::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		int aVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2 + 20);
		if (mEatingAnimationTimer == 0)
		{
			if (aVal == 1)
			{
				ShowInvisibility();
				mEatingAnimationTimer = 8;
			}
			else if (aVal == 2)
			{
				ShowInvisibility();
				mEatingAnimationTimer = 20;
			}
		}
	}
	else
	{
		// The list is fetched once before the loop, and the carnivore is read through the
		// iterator for each test, as the original
		std::vector<Oscar*>* anOscarList = mApp->mBoard->mOscarList;
		for (std::vector<Oscar*>::iterator it = anOscarList->begin(); it != anOscarList->end(); ++it)
		{
			if (!(*it)->CantBeEaten())
			{
				if (mXD + 80.0 < (*it)->mX + 120 && mXD + 80.0 > (*it)->mX - 40 &&
					mYD + 90.0 < (*it)->mY + 80 && mYD + 90.0 > (*it)->mY)
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
				else if (mEatingAnimationTimer == 0 && mXD + 80.0 < (*it)->mX + 150 && mXD + 80.0 > (*it)->mX - 70
					&& mYD + 90.0 < (*it)->mY + 100 && mYD + 90.0 > (*it)->mY - 20)
				{
					ShowInvisibility();
					mEatingAnimationTimer = 20;
				}
			}
		}
	}
}

void Sexy::Ultra::RemoveFromGame(bool aRemoveShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (aRemoveShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
	mApp->mBoard->m0x474++;
}

void Sexy::Ultra::FishUpdateAnimation()
{
	if (mInvisible)
		UpdateInvisible();

	if (mPrevVX > 0.0 && mVX < 0.0)
		mTurnAnimationTimer = 20;
	else if (mPrevVX < 0.0 && mVX > 0.0)
		mTurnAnimationTimer = -20;

	// As the original: the eating timer counts down while turning and when it sets the frame
	// (twice on the frame a turn ends); a frame that shows eating does not advance the swim cycle.
	if (mTurnAnimationTimer > 0)
	{
		mTurnAnimationTimer--;
		if (mEatingAnimationTimer > 0)
			mEatingAnimationTimer--;
	}
	else if (mTurnAnimationTimer < 0)
	{
		mTurnAnimationTimer++;
		if (mEatingAnimationTimer > 0)
			mEatingAnimationTimer--;
	}

	if (mTurnAnimationTimer == 0)
	{
		if (mEatingAnimationTimer > 0)
		{
			mEatingAnimationTimer--;
			mAnimationFrameIndexFish = 9 - mEatingAnimationTimer / 2;
		}
		else
		{
			if (mVXAbs <= 1)
				mSwimFrameCounter++;
			else
				mSwimFrameCounter += 2;
			if (mSwimFrameCounter >= 20)
				mSwimFrameCounter = 0;
			mAnimationFrameIndexFish = mSwimFrameCounter / 2;
		}
	}
	else
	{
		mEatingAnimationTimer = 0;
		if (mTurnAnimationTimer > 0)
			mAnimationFrameIndexFish = 9 - mTurnAnimationTimer / 2;
		else if (mTurnAnimationTimer < 0)
			mAnimationFrameIndexFish = mTurnAnimationTimer / 2 + 9;
	}

	if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
		mPrevVX = mVX;

	if (mVoraciousScreamCounter > 100 && mTurnAnimationTimer == 0)
		mAnimationFrameIndexFish = 4;

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::Ultra::Init(int theX, int theY) // the position is unused, as in the original
{
	mType = TYPE_ULTRA;
	mSize = TYPE_ULTRA;
	mWidth = 160;
	mHeight = 160;
	mIsGuppy = false;
	mYMax = 310;
	mYMin = 75;
	mXMin = 0;
	mXMax = 480;
	mHunger = mApp->mSeed->Next() % 200 + 600;
	mMouseVisible = gUnkBool06;
	mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 250 + 200);
}
