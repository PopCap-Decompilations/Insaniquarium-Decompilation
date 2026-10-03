#include <SexyAppFramework/WidgetManager.h>

#include "Grubber.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Fish.h"
#include "Missle.h"
#include "Shadow.h"
#include "Res.h"

Sexy::Grubber::Grubber()
{
	mType = TYPE_GRUBBER;
	mClip = false;
}

Sexy::Grubber::Grubber(int theX)
{
	Grubber::Init();
	mXD = theX;
	mX = (int)mXD; // converted back from the double, as the original (0x4EA78B)
}

Sexy::Grubber::Grubber(int theX, int theY)
{
	Grubber::Init();
	mXD = theX;
	mX = theX;
	mY = theY;
	mYD = theY;
}

Sexy::Grubber::~Grubber()
{
}

void Sexy::Grubber::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	if (!HungryLogic())
	{
		if (mSpeedState == 0)
			mTargetVX = -0.5;
		else if (mSpeedState == 1)
			mTargetVX = 0.5;
		else if (mSpeedState == 2)
			mTargetVX = -1.0;
		else if (mSpeedState == 3)
			mTargetVX = 1.0;
		else if (mSpeedState == 4)
			mTargetVX = 0;
		else if (mSpeedState == 5)
			mTargetVX = 0;
		else if (mSpeedState == 6)
			mTargetVX = -2.5;
		else if (mSpeedState == 7)
			mTargetVX = 2.5;

		if (mVX > mTargetVX)
			mVX -= 0.1;
		if (mVX < mTargetVX)
			mVX += 0.1;
	}

	mEatFoodDelayTimer++;
	mSpeedChangeTimer++;
	if ((mSpeedChangeTimer > 20 || mXD <= 10.0 || mXD >= 540.0) && mEatingAnimTimer == 0)
	{
		mSpeedChangeTimer = 0;
		if (mApp->mSeed->Next() % 10 == 0)
			mSpeedState = mApp->mSeed->Next() % 8;
	}

	if (!mApp->mBoard->AliensInTank())
		CoinDrop();
	if (mXD > 560.0)
		mXD = 560.0;
	if (mXD < 10.0)
		mXD = 10.0;
	if (mYD > 355.0)
	{
		mYD = 355.0;
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
	if (mYD < 355.0)
		mVY += 0.4;

	UpdateAnimations();
	double aSpeedMod = mSpeedMod;
	if (mSpeedy)
	{
		if (mSpeedySpeedState != 0)
			aSpeedMod = 1.5;
		else
			aSpeedMod = 0.3;
	}
	mXD += mVX / aSpeedMod;
	mYD += mVY / aSpeedMod;
	Move(mXD, mYD);
}

void Sexy::Grubber::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	DrawGrubber(g);
	if (!mName.empty())
		DrawName(g, false);
}

void Sexy::Grubber::MouseDown(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
}

void Sexy::Grubber::MouseDrag(int x, int y)
{
	// 0x4D8C10, shared with Board/Fish/Penta/Breeder/OtherTypePet: forwards to the empty Widget::MouseDrag
	GameObject::MouseDrag(x, y);
}

void Sexy::Grubber::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[0]++;
}

int Sexy::Grubber::SpecialReturnValue()
{
	return 7;
}

void Sexy::Grubber::Remove()
{
	Remove(true);
}

void Sexy::Grubber::SetPosition(int newX, int newY)
{
	mX = newX;
	mXD = newX;
	mY = newY;
	mYD = newY;
}

void Sexy::Grubber::OnFoodAte(GameObject* obj)
{
	bool hungry = IsHungryVisible();
	Unk02(false);
	mSpeedySpeedState = 100;
	if (mVirtualTankId >= 0)
		UpdateMentalState();
	mApp->mBoard->PlayChompSound(mVoracious);
	mHunger += 1000;
	if (mHunger > 1400)
		mHunger = 1400;

	if (mApp->m0x882 && obj->mType != TYPE_FOOD)
	{
		for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
		{
			int aY = mApp->mSeed->Next() % 20 + obj->mY + 15; // y drawn before x, as the original
			int aX = mApp->mSeed->Next() % 20 + obj->mX + 15;
			mApp->mBoard->SpawnShot(aX, aY, 1);
		}
	}
}

void Sexy::Grubber::UpdateStoreAnimation()
{
	UpdateStoreCounters();
	mStoreAnimationIndex = (mStoreAnimationTimer / 2) % 10;
}

void Sexy::Grubber::DrawStoreAnimation(Graphics* g, int justification)
{
	int anX = 0, anY = 0;
	switch (justification)
	{
	case 0:
		anX = 19;
		anY = 17;
		break;
	case 1:
		anX = 5;
		anY = 13;
		break;
	case 2:
		anX = 15;
		anY = -5;
		break;
	case 3:
		anX = 5;
		anY = 0;
		break;
	case 4:
		anX = -40;
		anY = 0;
		break;
	}
	g->Translate(anX, anY);
	SetStoreColor(g, Color::White);
	g->DrawImage(IMAGE_GRUBBER, 0, 0, Rect(mStoreAnimationIndex * 80, 0, 80, 80));
	g->SetColorizeImages(false);
	g->Translate(-anX, -anY);
}

void Sexy::Grubber::Sync(DataSync* theSync)
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
	theSync->SyncLong(mGrubberAnimIndex);
	theSync->SyncLong(mEatingAnimTimer);
	theSync->SyncLong(mCoinDropTimer);
	theSync->SyncLong(mCoinDropThreshold);
}

void Sexy::Grubber::Init()
{
	mType = TYPE_GRUBBER;
	mClip = false;
	mYD = mApp->mSeed->Next() % 5 + 360;
	mY = mYD;
	mVX = 0;
	mVY = 0;
	mTargetVX = 0;
	mWidth = 80;
	mHeight = 80;
	int aRandSpeedMod = mApp->mSeed->Next() % 3;
	if (aRandSpeedMod == 0)
		mSpeedMod = 2.7;
	else if (aRandSpeedMod == 1)
		mSpeedMod = 2.5;
	else
		mSpeedMod = 2.6;
	mHunger = mApp->mSeed->Next() % 200 + 900;
	// Drawn before the stores below, as the original (0x4D7A55)
	mSpeedState = mApp->mSeed->Next() % 10;
	mEatFoodDelayTimer = 40;
	mSpeedChangeTimer = 0;
	mMovementAnimTimer = 0;
	mGrubberAnimIndex = 0;
	mEatingAnimTimer = 0;
	mCoinDropTimer = 0;
	// A signed modulo in the original (0x4D7AA2), unlike the other DetermineCoinDropT callers
	mCoinDropThreshold = DetermineCoinDropT((int)mApp->mSeed->Next() % 200 + 300);
	mMouseVisible = gUnkBool06;
	mBoughtTimer = 45;
}

bool Sexy::Grubber::HungryLogic()
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

bool Sexy::Grubber::HungryBehavior()
{
	GameObject* aNearestFood = FindNearestFood();
	if (mEatFoodDelayTimer >= 5 && aNearestFood != nullptr)
	{
		mEatFoodDelayTimer = 0;
		double aX = mXD + 40.0; // compared untruncated, as the original
		int aFX = aNearestFood->mX;
		if (mHunger > 300)
		{
			if (aX > aFX + 48)
			{
				if (mVX > -2.5)
					mVX -= 0.8;
			}
			else if (aX < aFX + 24)
			{
				if (mVX < 2.5)
					mVX += 0.8;
			}
			else if (aX > aFX + 36)
			{
				if (mVX > -1.5)
					mVX -= 0.6;
			}
			else if (aX < aFX + 36)
			{
				if (mVX < 1.5)
					mVX += 0.6;
			}
		}
		else
		{
			if (aX > aFX + 48)
			{
				if (mVX > -4.0)
					mVX -= 1.5;
			}
			else if (aX < aFX + 24)
			{
				if (mVX < 4.0)
					mVX += 1.5;
			}
			else if (aX > aFX + 36)
			{
				if (mVX > -2.5)
					mVX -= 0.8;
			}
			else if (aX < aFX + 36)
			{
				if (mVX < 2.5)
					mVX += 0.8;
			}
		}
	}
	if (aNearestFood)
	{
		if (mYD + 40.0 < aNearestFood->mY + (aNearestFood->mHeight > 80 ? 240 : 160) &&
			mYD + 40.0 > aNearestFood->mY - 20 &&
			mXD + 40.0 < aNearestFood->mX + 80 && 
			mXD + 40.0 > aNearestFood->mX && mYD >= 355.0)
		{
			mSpeedySpeedState = 100;
			ShowInvisibility();
			mVY = -14.0;
		}
		CollideWithFood();
	}
	return aNearestFood != nullptr;
}

void Sexy::Grubber::Die(bool flag)
{
	if (flag)
		mApp->mBoard->PlayDieSound(mType);
	Remove(false);
	bool isFacingRight = (mVX > 0.0);
	mApp->mBoard->SpawnDeadFish(mXD, mYD, mVX, 0.0, mSpeedMod, mType, isFacingRight, mShadowPtr); // no vertical speed, as the original
}

void Sexy::Grubber::CollideWithFood()
{
	if (mExoticDietFoodType != 0)
	{
		int aVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight / 2);
		if (mEatingAnimTimer == 0)
		{
			if (aVal == 1)
			{
				ShowInvisibility();
				mEatingAnimTimer = 4;
			}
			else if (aVal == 2)
			{
				ShowInvisibility();
				mEatingAnimTimer = 8;
			}
		}
	}
	else
	{
		for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
		{
			if ((*it)->mVirtualTankId < 0 && (*it)->mCanBeEatenDelay <= 0)
			{
				// The centre is compared untruncated, and the size is tested last, as the original
				if (mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX &&
					mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10 && (*it)->mSize == TYPE_GUPPY)
				{
					OnFoodAte(*it);
					(*it)->RemoveFromGame(true);
					if (mEatingAnimTimer == 0)
					{
						ShowInvisibility();
						mEatingAnimTimer = 12;
					}
					return;
				}
				if (mEatingAnimTimer == 0 && mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX &&
					mYD + 40.0 < (*it)->mY + 160 && mYD + 40.0 > (*it)->mY - 20 && (*it)->mSize == TYPE_GUPPY)
				{
					mEatingAnimTimer = 20;
					ShowInvisibility();
					return; // the original stops here
				}
			}
		}
	}
}

Sexy::GameObject* Sexy::Grubber::FindNearestFood()
{
	if (mExoticDietFoodType != 0)
		return FindNearestExoticFood(mX + mWidth / 2, mY + mHeight / 2);

	int aDist = 100000000;
	GameObject* aRet = nullptr;
	for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
	{
		if ((*it)->mSize == TYPE_GUPPY && (*it)->mVirtualTankId < 0 && (*it)->mCanBeEatenDelay <= 0)
		{
			// Differences in double, truncated afterwards, as the original
			int ax = (int)(((*it)->mX + 40) - (mXD + 40.0));
			int ay = (int)(((*it)->mY + 40) - (mYD + 40.0));
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
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

void Sexy::Grubber::Remove(bool removeShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (removeShadow && mShadowPtr)
		mShadowPtr->RemoveShadow();
	mApp->mBoard->m0x47c++;
	if (mApp->mBoard->IsTankAndLevelNB(3, 1))
	{
		if (mApp->mBoard->mMessageShown[38])
			mApp->mBoard->ShowText("Try luring small guppies down to your guppycrunchers!", false, 39);
		if (mApp->mBoard->mMessageShown[37])
			mApp->mBoard->ShowText("Guppycrunchers and carnivores share the same diet!", false, 38);
		mApp->mBoard->ShowText("Warning! Your guppycruncher has died!", false, 37);
	}
}

void Sexy::Grubber::CoinDrop()
{
	mCoinDropTimer++;
	if (CoinDropTimerPassed(mCoinDropTimer, mCoinDropThreshold))
	{
		mCoinDropTimer = 0;
		if (RelaxModeCanDrop())
			mApp->mBoard->SpawnLarva(mX + 4, mY - 10);
	}
}

void Sexy::Grubber::UpdateAnimations()
{
	UpdateInvisible();
	if (mEatingAnimTimer > 0)
	{
		mEatingAnimTimer--;
		mGrubberAnimIndex = 9 - mEatingAnimTimer / 2;
	}
	else if(mVX >= 1.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer + 1) % 20;
		mGrubberAnimIndex = mMovementAnimTimer / 2;
	}
	else if(mVX <= -1.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer - 1) % 20;
		if (mMovementAnimTimer < 0)
			mMovementAnimTimer += 20;
		mGrubberAnimIndex = mMovementAnimTimer / 2;
	}
	else if (mVX > 0.0)
	{
		mMovementAnimTimer = (mMovementAnimTimer + 1) % 40;
		mGrubberAnimIndex = mMovementAnimTimer / 4;
	}
	else
	{
		mMovementAnimTimer = (mMovementAnimTimer - 1) % 40;
		if (mMovementAnimTimer < 0)
			mMovementAnimTimer += 40;
		mGrubberAnimIndex = mMovementAnimTimer / 4;
	}
	if (mVoraciousScreamCounter > 100)
		mGrubberAnimIndex = 4;
	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::Grubber::DrawGrubber(Graphics* g)
{
	if (gZombieMode)
	{
		g->DrawImageMirror(IMAGE_GRUBBER, 0, 0, Rect((IsHungryVisible() ? 6 : 9) * 80, 240, 80, 80), false);
		if (mMisslePtr)
			DrawCrosshair(g, 0, 0);
		if (IsHungryBlipPointer(900))
			g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
		return;
	}

	// The rect is built before the calls, as the original (0x4E1A92)
	Rect aSrcRect(mGrubberAnimIndex * 80, 0, 80, 80);
	aSrcRect.mY = GetRowToDraw(IsHungryVisible()) * 80;
	if (mInvisible)
		if (DrawInvisibleEffect(g, IMAGE_GRUBBER, aSrcRect, false))
			return;
	SetColorHelper(g, Color::White);
	g->DrawImage(IMAGE_GRUBBER, 0, 0, aSrcRect);
	if (mHungerAnimationTimer != 0)
	{
		// As the original: the hungry row is computed into aSrcRect, but the overlay is
		// drawn from a new rect on row 2 (0x4E1B20-0x4E1B95)
		aSrcRect.mY = GetRowToDraw(true) * 80;
		SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer * 255 / 5));
		g->DrawImage(IMAGE_GRUBBER, 0, 0, Rect(mGrubberAnimIndex * 80, 160, 80, 80));
	}
	g->SetColorizeImages(false);
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (IsHungryBlipPointer(900))
		g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 2);
}

int Sexy::Grubber::GetRowToDraw(bool hungry)
{
	if (mEatingAnimTimer > 0 || mVoraciousScreamCounter > 100)
		return hungry ? 4 : 1;
	return hungry ? 2 : 0;
}
