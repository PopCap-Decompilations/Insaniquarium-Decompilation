#include <SexyAppFramework/WidgetManager.h>

#include "Gekko.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Coin.h"
#include "Larva.h"
#include "Missle.h"
#include "Shadow.h"
#include "Res.h"

Sexy::Gekko::Gekko()
{
	mType = TYPE_GEKKO;
}

Sexy::Gekko::Gekko(int theX, int theY) : Fish(theX, theY)
{
	Gekko::Init(theX, theY);
}

Sexy::Gekko::Gekko(int theX, int theY, bool velocityRight) : Fish(theX, theY)
{
	Gekko::Init(theX, theY);
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

Sexy::Gekko::~Gekko()
{
}

void Sexy::Gekko::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[2]++;
}

void Sexy::Gekko::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	if (mVirtualTankId >= 0)
		UpdateMentalState();
	if (obj->mType == TYPE_FOOD)
		mApp->mBoard->PlaySlurpSound(mVoracious);
	else
		mApp->mBoard->PlayChompSound(mVoracious);
	mHunger += 700;
	if (mHunger > 1000)
		mHunger = 1000;
	UpdateHungerStateIfWasHungry(hungry);
}

void Sexy::Gekko::DrawStoreAnimation(Graphics* g, int justification)
{
	int anXTrans = 0, anYTrans = 0;
	switch (justification)
	{
	case 0:
		anXTrans = 23;
		anYTrans = 20;
		break;
	case 1:
		anXTrans = 15;
		anYTrans = 17;
		break;
	case 2:
		anXTrans = 20;
		break;
	case 3:
		anXTrans = 12;
		anYTrans = 5;
		break;
	case 4:
		anXTrans = -40;
		break;
	}
	g->Translate(anXTrans, anYTrans);
	// Cookie gets a colour of his own (plain white), as the original
	SetStoreColor(g, mPreNamedTypeId == COOKIE ? Color(0xffffff) : Color::White);
	g->DrawImage(IMAGE_GEKKO, 0, 0, Rect(mStoreAnimationIndex * 80, 0, 80, 80));
	g->SetColorizeImages(false);
	g->Translate(-anXTrans, -anYTrans);
}

void Sexy::Gekko::DropCoin()
{
	mCoinDropTimer++;
	if (mPreNamedTypeId == COOKIE)
	{
		if (mCoinDropTimer > 540 || mCoinDropTimer == 504)
		{
			// Both arrays are filled (cleared first) by the Board, as the original
			int aReqs[8];
			int aInTank[8];
			mApp->mBoard->GetExoticFoodsRequiredInTank(aReqs);
			mApp->mBoard->GetExoticFoodsInTank(aInTank);
			std::vector<int> aFoodCandidates;
			for (int i = 0; i < 8;i++)
			{
				if (aReqs[i] - aInTank[i] > 0 && (i != 0 || mApp->mBoard->mPetsInTank[3] == 0))
					aFoodCandidates.push_back(i);
			}

			if (!aFoodCandidates.empty())
			{
				// A signed remainder, and the candidate is fetched before the timer is tested, as the original
				int aRandIdx = Rand() % (int)aFoodCandidates.size();
				int aFoodType = aFoodCandidates[aRandIdx];

				if (mCoinDropTimer > 540)
				{
					mApp->mBoard->SpawnVirtualTankFood(aFoodType);
					if (mGrowthAnimationTimer == 0)
						mGrowthAnimationTimer = 36;
				}
				else if(mCoinDropTimer == 504)
					mGrowthAnimationTimer = 72;
			}
			if (mCoinDropTimer > 540)
				mCoinDropTimer = 0;
		}
	}
	else
	{
		if (CoinDropTimerPassed(mCoinDropTimer, mCoinDropT))
		{
			mCoinDropTimer = 0;
			if (RelaxModeCanDrop())
				mApp->mBoard->DropCoin(mX + 5, mY + 10, COIN_PEARL, nullptr, -1.0, 0);
		}
	}
}

bool Sexy::Gekko::Hungry()
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

void Sexy::Gekko::DrawFish(Graphics* g, bool mirror)
{
	DrawGekko(g, mirror);
	if (mGrowthAnimationTimer > 0)
	{
		if(((mGrowthAnimationTimer / 8) % 2) == 0)
		{
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			DrawGekko(g, mirror);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}
	}
}

bool Sexy::Gekko::HungryBehavior()
{
	GameObject* aFood = FindNearestFood();
	int aX = mXD + 40.0;
	int aY = GetExoticDietOffset() + (mYD + 45.0); // the offset is added to (mYD + 45.0), as the original

	if (mSpecialMovementStateChangeTimer >= 3)
	{
		if (aFood == nullptr)
			return false;
		mSpecialMovementStateChangeTimer = 0;
		int aFX = aFood->mX;
		// The food's mY is read only after the mVX steering, as the original
		if (mHunger > 300)
		{
			if (aFX + 40 < aX)
			{
				if (mVX > -3.0)
					mVX -= 1.0;
			}
			else if (aFX + 32 > aX)
			{
				if (mVX < 3.0)
					mVX += 1.0;
			}
			else if (aFX + 38 < aX)
			{
				if (mVX > -3.0)
					mVX -= 0.1;
			}
			else if (aFX + 34 > aX)
			{
				if (mVX < 3.0)
					mVX += 0.1;
			}
			else if (aFX + 36 < aX)
			{
				if (mVX > -3.0)
					mVX -= 0.05;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 3.0)
					mVX += 0.05;
			}

			if (aFood->mY + 39 < aY)
			{
				if (mVY > -3.0)
					mVY -= 1.0;
			}
			else if (aFood->mY + 33 > aY)
			{
				if (mVY < 3.0)
					mVY += 1.0;
			}
			else if (aFood->mY + 36 < aY)
			{
				if (mVY > -3.0)
					mVY -= 0.5;
			}
			else if (aFood->mY + 36 > aY)
			{
				if (mVY < 3.0)
					mVY += 0.5;
			}
		}
		else
		{
			if (aFX + 40 < aX)
			{
				if (mVX > -4.0)
					mVX -= 1.3;
			}
			else if (aFX + 32 > aX)
			{
				if (mVX < 4.0)
					mVX += 1.3;
			}
			else if (aFX + 38 < aX)
			{
				if (mVX > -4.0)
					mVX -= 0.2;
			}
			else if (aFX + 34 > aX)
			{
				if (mVX < 4.0)
					mVX += 0.2;
			}
			else if (aFX + 36 < aX)
			{
				if (mVX > -4.0)
					mVX -= 0.05;
			}
			else if (aFX + 36 > aX)
			{
				if (mVX < 4.0)
					mVX += 0.05;
			}

			if (aFood->mY + 36 < aY)
			{
				if (mVY > -4.0)
					mVY -= 1.3;
			}
			else if (aFood->mY + 36 > aY)
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

Sexy::GameObject* Sexy::Gekko::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2 + GetExoticDietOffset());

	// Both lists are fetched before the loops, as the original
	Board* aBoard = mApp->mBoard;
	std::vector<Larva*>* aLarvaList = aBoard->mLarvaList;
	std::vector<Coin*>* aCoinList = aBoard->mCoinList;
	int aCenterX = mX + mWidth / 2;
	int aCenterY = mY + mHeight / 2;
	int aDist = 100000000;
	GameObject* aRet = nullptr;
	for (std::vector<Larva*>::iterator it = aLarvaList->begin(); it != aLarvaList->end(); ++it)
	{
		if (!(*it)->mPickedUp && (*it)->CheckNearestDist(&aDist, aCenterX, aCenterY))
			aRet = *it;
	}
	for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
	{
		Coin* aFood = *it;
		if (aFood->mCoinType == CoinTypes::COIN_PEANUT && !aFood->m0x198 && aFood->CheckNearestDist(&aDist, aCenterX, aCenterY))
			aRet = aFood;
	}
	if (aDist < 10000)
	{
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::Gekko::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		int aVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2 + GetExoticDietOffset());
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
		// Both lists are fetched before the loops, as the original
		Board* aBoard = mApp->mBoard;
		std::vector<Coin*>* aCoinList = aBoard->mCoinList;
		std::vector<Larva*>* aLarvaList = aBoard->mLarvaList;
		for (std::vector<Larva*>::iterator it = aLarvaList->begin(); it != aLarvaList->end(); ++it)
		{
			if (!(*it)->mPickedUp && FoodCollided(*it))
				return;
		}
		for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
		{
			Coin* aFood = *it;
			if (aFood->mCoinType == CoinTypes::COIN_PEANUT && !aFood->m0x198 && FoodCollided(aFood))
				return;
		}
	}
}

void Sexy::Gekko::RemoveFromGame(bool removeShadow)
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

void Sexy::Gekko::Init(int theX, int theY) // the position is unused, as in the original
{
	mType = TYPE_GEKKO;
	mIsGuppy = false;
	mSize = TYPE_GEKKO;
	mYMin = 105;
	mYMax = 360;
	mMouseVisible = gUnkBool06;
	mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 250 + 200);
}

void Sexy::Gekko::DrawGekko(Graphics* g, bool mirror)
{
	if (gZombieMode)
	{
		bool aHungr = IsHungryVisible();
		g->DrawImageMirror(IMAGE_GEKKO, 0, 0, Rect((aHungr ? 6 : 9) * 80, 400, 80, 80), mirror);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);
		if (IsHungryBlipPointer(500))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
		return;
	}

	// The rect is built with row 0 before the hunger check sets its row, as the original
	Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, 0, 80, 80);
	bool aHungr = IsHungryVisible();
	aSrcRect.mY = GetDrawRow(aHungr) * 80;

	if (mInvisible)
		if (DrawInvisibleEffect(g, IMAGE_GEKKO, aSrcRect, mirror))
			return;

	// Cookie gets a colour of his own (plain white) unless hungry, as the original
	SetColorHelper(g, (mPreNamedTypeId == COOKIE && !aHungr) ? Color(0xffffff) : Color::White);
	g->DrawImageMirror(IMAGE_GEKKO, 0, 0, aSrcRect, mirror);
	if (mHungerAnimationTimer != 0)
	{
		aSrcRect.mY = GetDrawRow(true) * 80;
		SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer * 255 / 5));
		g->DrawImageMirror(IMAGE_GEKKO, 0, 0, aSrcRect, mirror);
	}
	g->SetColorizeImages(false);

	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);

	if (IsHungryBlipPointer(500))
		g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
}

int Sexy::Gekko::GetDrawRow(bool isHungry)
{
	if (mTurnAnimationTimer != 0)
		return isHungry ? 4 : 1;
	if (mEatingAnimationTimer <= 0 && mVoraciousScreamCounter <= 100)
		return isHungry ? 3 : 0;
	return isHungry ? 6 : 2;
}

bool Sexy::Gekko::FoodCollided(GameObject* theObj)
{
	// The food's mY is read only once its x range matched, as the original
	if (mXD + 40.0 < theObj->mX + 54 && mXD + 40.0 > theObj->mX + 18 &&
		mYD + 40.0 < theObj->mY + 54 && mYD + 40.0 > theObj->mY + 18)
	{
		OnFoodAte(theObj);
		theObj->Remove();
		if (mEatingAnimationTimer == 0)
		{
			ShowInvisibility();
			mEatingAnimationTimer = 8;
		}
		return true;
	}
	else if (mEatingAnimationTimer == 0 && mXD + 40.0 < theObj->mX + 76 && mXD + 40.0 > theObj->mX - 4
		&& mYD + 40.0 < theObj->mY + 66 && mYD + 40.0 > theObj->mY + 6)
	{
		ShowInvisibility();
		mEatingAnimationTimer = 20;
	}
	return false;
}
