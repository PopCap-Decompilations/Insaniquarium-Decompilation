#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>

#include "OtherTypePet.h"
#include "BilaterusHead.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Shadow.h"
#include "Missle.h"
#include "Fish.h"
#include "Oscar.h"
#include "Gekko.h"
#include "Breeder.h"
#include "Ultra.h"
#include "Coin.h"
#include "Res.h"

using namespace Sexy;

Sexy::OtherTypePet::OtherTypePet()
{
	mClip = false;
	mSleepy = false;
	mType = TYPE_OTHER_TYPE_PET;
}

Sexy::OtherTypePet::OtherTypePet(int theX, int theY, int thePetType, int theBackgroundId, bool isPresto)
{
	mClip = false;
	mSleepy = false;
	mIsPresto = isPresto;
	mOtherTypePetType = thePetType;
	mType = TYPE_OTHER_TYPE_PET;
	mMouseVisible = gUnkBool06;
	mPrestoTransformAnimTimer = 0;
	if (!isPresto || thePetType == PET_PRESTO)
		mPrestoTimer = 0;
	else
		mPrestoTimer = 360;

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		mPrestoTimer = 0;

	mXD = theX;
	mYD = theY;

	if (thePetType == PET_STINKY)
	{
		if (!isPresto)
			mYD = 370;
	}
	else if (thePetType == PET_RHUBARB)
	{
		if (!isPresto)
			mYD = 355;
	}
	else if (thePetType == PET_RUFUS)
	{
		if (!isPresto)
			mYD = 365;
	}

	UpdateNikoPosition(theBackgroundId);
	mX = mXD;
	mY = mYD;
	mVX = 0;
	mVY = 0;
	mTargetVX = 0;
	mWidth = 80;
	mHeight = 80;
	mPrevVX = 1;
	mSpeedMod = 2;
	if(mOtherTypePetType == PET_STINKY)
		mSpeedMod = 1.2;
	if(mOtherTypePetType == PET_CLYDE)
		mSpeedMod = 1.5;

	// The draws come before the resets that follow them, as the original
	mMovementState = mApp->mSeed->Next() % 10;
	mChaseEntityTimer = 40;
	mMovementStateChangeTimer = 0;
	mMovementAnimationTimer = 0;
	mTurnAnimationTimer = 0;
	mAnimationIndex = 0;
	mPetSpecialtyTimer = 0;
	m0x1b8 = mApp->mSeed->Next() % 250 + 250;
	m0x1c0 = false;
	mPetAngryTimer = 0;
}

void Sexy::OtherTypePet::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	if ((mIsPresto && (mOtherTypePetType == PET_NIKO || mOtherTypePetType == PET_RUFUS || mOtherTypePetType == PET_RHUBARB || mOtherTypePetType == PET_STINKY))
		&& mYD < 380.0)
		mVY += 0.1;

	if (mApp->mBoard->mTank == 5 || mOtherTypePetType != PET_STINKY) // 32
	{
		if (mOtherTypePetType == PET_CLYDE)
		{
			if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
				DeterminePetSleepy(&mSleepy);

			if (mApp->mBoard->mTank == 5 || !ChaseEnemyBehavior())
			{
				if (mMovementState == 0 || mPetSpecialtyTimer != 0)
					mTargetVX = 0;
				else if(mMovementState == 1)
					mTargetVX = -0.5;
				else if(mMovementState == 2)
					mTargetVX = 0.5;
			}

			if (mTargetVX < mVX)
				mVX -= 0.1;
			if (mTargetVX > mVX)
				mVX += 0.1;

			if (mMovementAnimationTimer == 2 || mMovementAnimationTimer == 3)
			{
				// A NaN mYD takes the second branch in the original, so the test is "<"
				if (mYD < 240.0)
					mVY -= 0.3;
				else
					mVY -= 0.75;
			}
			else
				mVY += 0.03;
		}
		else if (mOtherTypePetType == PET_RUFUS) // 73
		{
			if (mApp->mBoard->mTank == 5 || !ChaseEnemyBehavior())
			{
				if (mMovementState == 0)
					mTargetVX = 0;
				else if (mMovementState == 1)
					mTargetVX = -0.5;
				else if (mMovementState == 2)
					mTargetVX = 0.5;
				else if (mMovementState == 3)
					mTargetVX = -1;
				else if (mMovementState == 4)
					mTargetVX = 1;
				else if (mMovementState == 5)
					mTargetVX = 0;
				else if (mMovementState == 6)
					mTargetVX = 0;
				else if(mMovementState == 7)
					mTargetVX = -2.5;
				else if(mMovementState == 8)
					mTargetVX = 2.5;
			}

			if (mTargetVX < mVX)
				mVX -= 0.1;
			if (mTargetVX > mVX)
				mVX += 0.1;
		}
		else if (mOtherTypePetType == PET_RHUBARB && mApp->mBoard->mTank != 5) // 110
		{
			ChaseEnemyBehavior();
			if (mMovementState == 0)
				mTargetVX = 0;
			else if (mMovementState == 1)
				mTargetVX = -0.5;
			else if (mMovementState == 2)
				mTargetVX = 0.5;
			else if (mMovementState == 3)
				mTargetVX = -1;
			else if (mMovementState == 4)
				mTargetVX = 1;
			else if (mMovementState == 5)
				mTargetVX = 1.5;
			else if (mMovementState == 6)
				mTargetVX = -1.5;
			else if (mMovementState == 7)
				mTargetVX = -2.5;
			else if (mMovementState == 8)
				mTargetVX = 2.5;

			if (mTargetVX < mVX)
				mVX -= 0.1;
			if (mTargetVX > mVX)
				mVX += 0.1;
		}
	}
	else // 146 // IS stinky and isnt bg 5
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		{
			DeterminePetSleepy(&mSleepy);
			if (mSleepy)
				mPetAngryTimer = 0;
		}
		if (!ChaseEnemyBehavior())
		{
			if (mMovementState == 0 || mPetSpecialtyTimer != 0)
				mTargetVX = 0;
			else if(mMovementState == 1)
				mTargetVX = -0.5;
			else if(mMovementState == 2)
				mTargetVX = 0.5;

			if (mTargetVX < mVX)
			{
				mVX -= 0.1;
				if (mVX < mTargetVX)
					mVX = mTargetVX;
			}
			else if (mTargetVX > mVX)
			{
				mVX += 0.1;
				if (mVX > mTargetVX)
					mVX = mTargetVX;
			}
		}

		if (mPetSpecialtyTimer != 0)
			mVX = 0;
	}

	mMovementStateChangeTimer++;
	mChaseEntityTimer++;

	if ((mMovementStateChangeTimer > 20 || (mXD <= 10.0 && mTargetVX <= 0.0)) || (mXD >= 540.0 && mTargetVX >= 0.0))
	{
		mMovementStateChangeTimer = 0;
		if (mApp->mSeed->Next() % 10 == 0)
		{
			if (mOtherTypePetType == PET_RUFUS || mOtherTypePetType == PET_RHUBARB)
				mMovementState = mApp->mSeed->Next() % 9;
			else
				mMovementState = mApp->mSeed->Next() % 3;
		}
	}

	if (mPrestoTimer != 0)
		mPrestoTimer--;
	if (mPrestoTransformAnimTimer < 20)
		mPrestoTransformAnimTimer++;
	 
	UpdatePetSpecialAnimations();
	
	double aXCap;
	if (mOtherTypePetType == PET_STINKY || mOtherTypePetType == PET_CLYDE)
		aXCap = 550;
	else if(mOtherTypePetType == PET_RUFUS)
		aXCap = 560;
	else
		aXCap = 540;

	if (mXD > aXCap)
		mXD = aXCap;
	if (mXD < 10.0)
		mXD = 10;

	if (mYD > 350 && mOtherTypePetType == PET_NIKO)
	{
		mYD = 350;
		mVY = 0;
	}
	else if (mYD > 365 && mOtherTypePetType == PET_RUFUS)
	{
		mYD = 365;
		mVY = 0;
	}
	else if (mYD > 355 && mOtherTypePetType == PET_RHUBARB)
	{
		mYD = 355;
		mVY = 0;
	}
	else if (mYD > 370)
	{
		mYD = 370;
		mVY = 0;
	}

	if (mYD < 95)
		mYD = 95;

	if (mXD > 535 && mVX > 0.1)
		mMovementState = 1;
	if (mXD < 15 && mVX < -0.1)
		mMovementState = 2;
	UpdatePetAnimations();
	double aYChange;
	if (mSleepy)
	{
		mXD += mVX / (mSpeedMod * 5.0);
		aYChange = mVY / (mSpeedMod * 5.0);
	}
	else
	{
		mXD += mVX / mSpeedMod;
		aYChange = mVY / mSpeedMod;
	}
	mYD += aYChange;
	Move(mXD, mYD);
}

void Sexy::OtherTypePet::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	// Niko is tested first and Stinky second, as the original
	if (mOtherTypePetType == PET_NIKO)
		DrawHelper(g, false);
	else if (mOtherTypePetType == PET_STINKY)
	{
		if (mPetSpecialtyTimer > 0)
		{
			if (mPrevVX < 0.0)
				DrawHelper(g, false);
			else
				DrawHelper(g, true);
		}
		else
		{
			if (mTurnAnimationTimer == 0)
			{
				// Facing as the original: drawn once; with mVX and mPrevVX both 0 nothing is drawn
				if (mVX < 0.0 || ((int)mVX == 0 && mPrevVX < 0.0))
					DrawHelper(g, false);
				else if (mVX > 0.0 || ((int)mVX == 0 && mPrevVX > 0.0))
					DrawHelper(g, true);
			}
			else if (mTurnAnimationTimer > 0)
				DrawHelper(g, true);
			else if (mTurnAnimationTimer < 0)
				DrawHelper(g, false);
		}
	}
	else if (mOtherTypePetType == PET_CLYDE || mOtherTypePetType == PET_RUFUS || mOtherTypePetType == PET_RHUBARB)
		DrawHelper(g, false);

	g->SetColorizeImages(false);
}

void Sexy::OtherTypePet::MouseDown(int x, int y, int theClickCount)
{
	bool prestoClicked = HandlePrestoClick(x, y, theClickCount);
	if (!prestoClicked && !gUnkBool07)
	{
		gUnkBool07 = true;
		mApp->mBoard->MouseDown(mX + x, mY + y, theClickCount);
		gUnkBool07 = false;
	}
}

void Sexy::OtherTypePet::MouseDrag(int x, int y)
{
	// 0x4D8C10, shared with Board/Fish/Grubber/Penta/Breeder: forwards to the empty Widget::MouseDrag
	GameObject::MouseDrag(x, y);
}

void Sexy::OtherTypePet::PrestoMorph(int thePetId)
{
	if (thePetId == mOtherTypePetType || !PrestoRightClicked(mPrestoTimer))
		return;

	mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
	mApp->mBoard->SpawnPet(thePetId, mX, mY, true, false);
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();
}

void Sexy::OtherTypePet::Remove()
{
	RemoveOtherTypePet();
}

void Sexy::OtherTypePet::RemoveOtherTypePet()
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();
}

void Sexy::OtherTypePet::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncDouble(mPrevVX);
	theSync->SyncDouble(mTargetVX);
	theSync->SyncDouble(mSpeedMod);
	theSync->SyncLong(mMovementState);
	theSync->SyncLong(mChaseEntityTimer);
	theSync->SyncLong(mMovementStateChangeTimer);
	theSync->SyncLong(mTurnAnimationTimer);
	theSync->SyncLong(mMovementAnimationTimer);
	theSync->SyncLong(mAnimationIndex);
	theSync->SyncLong(mPrestoTransformAnimTimer);
	theSync->SyncLong(mPrestoTimer);
	theSync->SyncLong(mPetSpecialtyTimer);
	theSync->SyncLong(m0x1b8);
	theSync->SyncLong(mPetAngryTimer);
	theSync->SyncBool(m0x1c0);
	theSync->SyncLong(mOtherTypePetType);
	theSync->SyncBool(mIsPresto);
}

void Sexy::OtherTypePet::UpdateNikoPosition(int theBgId)
{
	if (mOtherTypePetType == 1 && !mIsPresto)
	{
		// A compare chain in the original (a switch on 1..5 would be a jump table)
		if (theBgId == 1)
		{
			mXD = 95.0;
			mYD = 253.0;
		}
		else if (theBgId == 2)
		{
			mXD = 175.0;
			mYD = 163.0;
		}
		else if (theBgId == 3)
		{
			mXD = 65.0;
			mYD = 156.0;
		}
		else if (theBgId == 4)
		{
			mXD = 145.0;
			mYD = 260.0;
		}
		else if (theBgId == 5)
		{
			mXD = 67.0;
			mYD = 185.0;
		}
		else
		{
			mXD = 160.0;
			mYD = 176.0;
		}
		mX = (int)mXD;
		mY = (int)mYD;
	}
}

void Sexy::OtherTypePet::UpdatePetAnimations()
{
	// Tested in this order, with Niko as the untested last case, as the original
	if (mOtherTypePetType == PET_STINKY) // 141 - 208
	{
		if (mPrevVX > 0.0 && mVX < 0.0)
			mTurnAnimationTimer = 20;
		else if (mPrevVX < 0.0 && mVX > 0.0)
			mTurnAnimationTimer = -20;

		if (mTurnAnimationTimer > 0)
			mTurnAnimationTimer--;
		else if (mTurnAnimationTimer < 0)
			mTurnAnimationTimer++;

		if (mTurnAnimationTimer != 0)
		{
			if (mTurnAnimationTimer > 0)
				mAnimationIndex = 9 - mTurnAnimationTimer / 2;
			else if (mTurnAnimationTimer < 0)
				mAnimationIndex = mTurnAnimationTimer / 2 + 9;
		}
		else if (abs(mVX) < 0.3)
		{
			mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 40;
			mAnimationIndex = mMovementAnimationTimer / 4;
		}
		else
		{
			mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 20;
			mAnimationIndex = mMovementAnimationTimer / 2;
		}

		if (mPetSpecialtyTimer > 0)
			mAnimationIndex = mPetSpecialtyTimer;

		if (mVX != mPrevVX && mVX != 0.0 && mPrevVX != 0.0 && mPetSpecialtyTimer == 0)
			mPrevVX = mVX;
	}
	else if (mOtherTypePetType == PET_RUFUS)
	{
		if (mVX <= 0.01 && mVX >= -0.01 && mTargetVX == 0.0 && mApp->mBoard->mAlienList->empty())
		{
			mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 60;
			if (mMovementAnimationTimer <= 2)
				mAnimationIndex = mMovementAnimationTimer / 2;
			else if (mMovementAnimationTimer <= 23)
				mAnimationIndex = 2;
			else if (mMovementAnimationTimer <= 32)
				mAnimationIndex = mMovementAnimationTimer / 2 - 10;
			else if (mMovementAnimationTimer <= 53)
				mAnimationIndex = 7;
			else if (mMovementAnimationTimer <= 59)
				mAnimationIndex = mMovementAnimationTimer / 2 - 20;
		}
		else
		{
			mMovementAnimationTimer %= 40;
			if (mVX <= 0.01 && mVX >= -0.01 && mTargetVX == 0.0 && mApp->mBoard->mCyraxPtr == nullptr &&
				GetEntityToChase() != nullptr)
			{
				mMovementAnimationTimer = (mMovementAnimationTimer + 4) % 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else if (mVX >= 1.0)
			{
				mMovementAnimationTimer = (mMovementAnimationTimer + 2) % 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else if (mVX <= -1.0)
			{
				mMovementAnimationTimer -= 2;
				if (mMovementAnimationTimer < 0)
					mMovementAnimationTimer += 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else if (mVX > 0.0)
			{
				mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else
			{
				mMovementAnimationTimer--;
				if (mMovementAnimationTimer < 0)
					mMovementAnimationTimer += 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
		}
	}
	else if (mOtherTypePetType == PET_RHUBARB) // 87
	{
		if (mPetSpecialtyTimer > 0)
		{
			mPetSpecialtyTimer = mPetSpecialtyTimer - 1;
			mAnimationIndex = mPetSpecialtyTimer / 2;
			if (mPetSpecialtyTimer == 10)
				mApp->PlaySample(SOUND_BUTTONCLICK);
		}
		else
		{
			if (mVX >= 1.0)
			{
				mMovementAnimationTimer -= 2;
				if (mMovementAnimationTimer < 0)
					mMovementAnimationTimer += 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else if(mVX <= -1.0)
			{
				mMovementAnimationTimer = (mMovementAnimationTimer + 2) % 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else if (mVX > 0.0)
			{
				mMovementAnimationTimer--;
				if (mMovementAnimationTimer < 0)
					mMovementAnimationTimer += 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
			else
			{
				mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 40;
				mAnimationIndex = mMovementAnimationTimer / 4;
			}
		}
	}
	else if (mOtherTypePetType == PET_CLYDE)
	{
		mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 40;
		if (mMovementAnimationTimer <= 6)
			mAnimationIndex = mMovementAnimationTimer / 2;
		else if (mMovementAnimationTimer <= 15)
			mAnimationIndex = 4;
		else
			mAnimationIndex = mMovementAnimationTimer / 4;
	}
	else // Niko
	{
		mMovementAnimationTimer = (mMovementAnimationTimer + 1) % 19;
	}

	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::OtherTypePet::UpdatePetSpecialAnimations()
{
	if (mOtherTypePetType == PET_STINKY)
	{
		if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
		{
			if (mPetSpecialtyTimer < 9)
				mPetSpecialtyTimer++;
			mPetAngryTimer = 0;
		}
		else if (mPetSpecialtyTimer > 0)
			mPetSpecialtyTimer--;
	}
	else if(mApp->mBoard->mTank != 5 && mOtherTypePetType == PET_NIKO)
	{
		mPetSpecialtyTimer++;
		if (mPetSpecialtyTimer == 1224)
			mApp->mBoard->PlaySample(SOUND_NIKOOPEN_ID, 3, 1.0);
		else if(mPetSpecialtyTimer == 1440)
			mApp->mBoard->PlaySample(SOUND_NIKOCLOSE_ID, 3, 1.0);
		else if (mPetSpecialtyTimer == 1233)
		{
			mApp->mBoard->DropCoin(mX + 1, mY - 2, COIN_NIKOPEARL, this, -1.0, 0);
			m0x1c0 = false;
			mApp->mBoard->SpawnBubble(mX + 11, mY + 5);
			mApp->mBoard->SpawnBubble(mX + 7, mY + 3);
		}
		else if (mPetSpecialtyTimer == 1450)
			mPetSpecialtyTimer = mApp->mSeed->Next() % 50;
	}
}

bool Sexy::OtherTypePet::ChaseEnemyBehavior()
{
	if (mOtherTypePetType == PET_STINKY || mOtherTypePetType == PET_CLYDE)
	{
		if (!mApp->mBoard->mCoinList->empty())
		{
			if (mOtherTypePetType == PET_STINKY && mApp->mBoard->AliensInTank())
				return false;

			if (!mSleepy)
			{
				ChaseEntity();
				return true;
			}
		}
	}
	else if (mOtherTypePetType == PET_RUFUS)
	{
		if (mApp->mBoard->AliensInTank())
		{
			ChaseEntity();
			return true;
		}
	}
	else if (mOtherTypePetType == PET_RHUBARB)
	{
		if (ChaseEntity())
			return true;
	}
	return false;
}

bool Sexy::OtherTypePet::ChaseEntity()
{
	GameObject* anEntity = GetEntityToChase();

	if (mChaseEntityTimer >= 5 && anEntity != nullptr)
	{
		mChaseEntityTimer = 0;
		if (mOtherTypePetType == PET_STINKY) // 15
		{
			int aX = anEntity->mX;
			double aCenterX = mXD + 40.0;

			// Outer bounds first, as the original
			if (aCenterX > aX + 48)
			{
				if (mVX > -2.3)
					mVX -= 1.0;
			}
			else if (aCenterX < aX + 24)
			{
				if (mVX < 2.3)
					mVX += 1.0;
			}
			else if (aCenterX > aX + 40)
			{
				if (mVX > -1.3)
					mVX -= 0.5;
			}
			else if (aCenterX < aX + 32)
			{
				if (mVX < 1.3)
					mVX += 0.5;
			}
			else if (aCenterX > aX + 36)
			{
				if (mVX > -0.3)
					mVX = 0.0;
			}
			else if (aCenterX < aX + 36)
			{
				if (mVX < 0.3)
					mVX = 0.0;
			}

			if (mApp->mBoard->mPetsInTank[PET_NIMBUS] != 0)
				mPetAngryTimer++;
		}
		else if (mOtherTypePetType == PET_CLYDE)
		{
			double aCenterX = mXD + 40.0;
			double aX = anEntity->mX + 36;
			if (aCenterX > aX)
			{
				if (mVX > -2.0)
					mVX -= 1.0;
			}
			else if (aCenterX < aX)
			{
				if (mVX < 2.0)
					mVX += 1.0;
			}
			double aCenterY = mYD + 40.0;
			double aY = anEntity->mY + 36;
			if (aCenterY > aY)
			{
				if (mVY > -2.0)
					mVY -= 1.0;
			}
			else if (aCenterY < aY)
			{
				if (mVY < 2.0)
					mVY += 1.0;
			}
		}
		else if (mOtherTypePetType == PET_RUFUS)
		{
			double aCenterX = mXD + 40.0;
			double aX = anEntity->mX + (anEntity->mType != TYPE_BILATERUS ? 80 : 40);
			if (aCenterX > aX)
			{
				if (mVX > -5.0)
					mVX -= 1.8;
			}
			else if(aCenterX < aX)
			{
				if (mVX < 5.0)
					mVX += 1.8;
			}
		}
		else if (mOtherTypePetType == PET_RHUBARB)
		{
			double aCenterX = mXD + 40.0;
			double aX = anEntity->mX + 40;
			if (aCenterX > aX)
			{
				if (mVX > -5.0)
					mVX -= 1.8;
			}
			else if(aCenterX < aX)
			{
				if (mVX < 5.0)
					mVX += 1.8;
			}
		}
	}
	if (anEntity)
		CollideWithObject();
	return anEntity;
}

GameObject* Sexy::OtherTypePet::GetEntityToChase()
{
	// These three lists are fetched once, up front, as the original; the others are read
	// through mApp->mBoard where they are walked.
	std::vector<Alien*>* anAlienList = mApp->mBoard->mAlienList;
	std::vector<Fish*>* aFishList = mApp->mBoard->mFishList;
	std::vector<Coin*>* aCoinList = mApp->mBoard->mCoinList;

	int aDist = 100000000;
	GameObject* aRetEntity = nullptr;

	// Distances are the object's centre minus the pet's, as the original. Every list is walked with
	// an iterator that is dereferenced again at each access, as the original
	if (mOtherTypePetType == PET_STINKY || mOtherTypePetType == PET_CLYDE)
	{
		bool noPenta = mApp->mBoard->mPentaList->empty();
		for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
		{
			if (noPenta || ((*it)->mCoinType != COIN_STAR && (*it)->mCoinType != SHELL_STAR)) // 104
			{
				if ((*it)->mCoinType != COIN_SHRAPNEL_BOMB && !(*it)->m0x198 && (*it)->mCoinType != COIN_PEANUT)
				{
					if (mOtherTypePetType != PET_CLYDE || !RelaxModeCheck(*it))
					{
						int aX = ((*it)->mX + 36) - (mXD + 40.0);
						int aY = ((*it)->mY + 36) - (mYD + 40.0);
						int aNewDist = aX * aX + aY * aY;

						if (aNewDist < aDist)
						{
							aDist = aNewDist;
							aRetEntity = *it;
						}
					}
				}
			}
		}
	}
	else if (mOtherTypePetType == PET_RUFUS) // 159
	{
		for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
		{
			int aX = ((*it)->mX + 40) - (mXD + 40.0);
			int aY = ((*it)->mY + 40) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}
		for (std::vector<Alien*>::iterator it = anAlienList->begin(); it != anAlienList->end(); ++it)
		{
			int aX = ((*it)->mX + 80) - (mXD + 40.0);
			int aY = ((*it)->mY + 80) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if (!(*it)->mIsPsychosquidHealing && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}
	}
	else if (mOtherTypePetType == PET_RHUBARB) // 252
	{
		int aYClick = mApp->mBoard->AliensInTank() ? 250 : 260;

		for (std::vector<Fish*>::iterator it = aFishList->begin(); it != aFishList->end(); ++it)
		{
			int aX = ((*it)->mX + 40) - (mXD + 40.0);
			int aY = ((*it)->mY + 40) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if ((*it)->mY > aYClick && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}

		for (std::vector<Oscar*>::iterator it = mApp->mBoard->mOscarList->begin(); it != mApp->mBoard->mOscarList->end(); ++it)
		{
			int aX = ((*it)->mX + 40) - (mXD + 40.0);
			int aY = ((*it)->mY + 40) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if ((*it)->mY > aYClick && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}

		for (std::vector<Gekko*>::iterator it = mApp->mBoard->mGekkoList->begin(); it != mApp->mBoard->mGekkoList->end(); ++it)
		{
			int aX = ((*it)->mX + 40) - (mXD + 40.0);
			int aY = ((*it)->mY + 40) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if ((*it)->mY > aYClick && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}

		for (std::vector<Breeder*>::iterator it = mApp->mBoard->mBreederList->begin(); it != mApp->mBoard->mBreederList->end(); ++it)
		{
			int aX = ((*it)->mX + 40) - (mXD + 40.0);
			int aY = ((*it)->mY + 40) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if ((*it)->mY > aYClick && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}

		for (std::vector<Ultra*>::iterator it = mApp->mBoard->mUltraList->begin(); it != mApp->mBoard->mUltraList->end(); ++it)
		{
			int aX = ((*it)->mX + 80) - (mXD + 40.0);
			int aY = ((*it)->mY + 80) - (mYD + 40.0);
			int aNewDist = aX * aX + aY * aY;

			if ((*it)->mY > aYClick - 40 && aNewDist < aDist)
			{
				aDist = aNewDist;
				aRetEntity = *it;
			}
		}
	}

	return aRetEntity;
}

void Sexy::OtherTypePet::DrawHelper(Graphics* g, bool mirror)
{
	// Case bodies in the original's order (Niko's comes first)
	switch (mOtherTypePetType)
	{
	case PET_NIKO:
		DrawNiko(g, mirror);
		break;
	case PET_STINKY:
		DrawStinky(g, mirror);
		break;
	case PET_CLYDE:
		DrawClyde(g, mirror);
		break;
	case PET_RUFUS:
		// A NaN mVX draws the moving row in the original, so the test is on standing still
		if (mVX <= 0.01 && mVX >= -0.01 && mTargetVX == 0.0)
			g->DrawImage(IMAGE_RUFUS, 0, 0, Rect(mAnimationIndex * 80, 80, 80, 80));
		else
			g->DrawImage(IMAGE_RUFUS, 0, 0, Rect(mAnimationIndex * 80, 0, 80, 80));
		break;
	case PET_RHUBARB:
		if (mPetSpecialtyTimer > 0)
			g->DrawImage(IMAGE_RHUBARB, 0, 0, Rect(mAnimationIndex * 80, 80, 80, 80));
		else
			g->DrawImage(IMAGE_RHUBARB, 0, 0, Rect(mAnimationIndex * 80, 0, 80, 80));
		break;
	}

	if (mIsPresto && mPrestoTransformAnimTimer < 20)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(0xaf, 0xaf, 0xaf, 0xff));
		g->DrawImageMirror(IMAGE_PRESTO, 0, 0, Rect((mPrestoTransformAnimTimer / 2) * 80, 160, 80, 80), mirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
	if (mMisslePtr)
		DrawCrosshair(g, 0, 0);
	if (mIsPresto && mOtherTypePetType != PET_PRESTO)
		DrawPrestoMisc(g, mPrestoTimer);
}

void Sexy::OtherTypePet::DrawStinky(Graphics* g, bool mirror)
{
	if (mSleepy)
		g->DrawImage(IMAGE_ZZZ, mirror ? 55 : 25, -14);

	if (mPetAngryTimer > 150)
	{
		g->SetColorizeImages(true);
		g->SetColor(Color(0xff, 0x4b, 0x4b, 0xff));
	}

	int aRow = 0;
	if (mPetSpecialtyTimer != 0)
		aRow = 2;
	else
	{
		if (mTurnAnimationTimer != 0)
		{
			mirror = mTurnAnimationTimer > 0;
			aRow = 1;
		}
	}

	g->DrawImageMirror(IMAGE_STINKY, 0, 0, Rect(mAnimationIndex * 80, aRow * 80, 80, 80), mirror);
	g->SetColorizeImages(false);
}

void Sexy::OtherTypePet::DrawNiko(Graphics* g, bool mirror)
{
	int aCol;
	int aRow;
	if (mPetSpecialtyTimer < 1224)
	{
		aCol = abs(mMovementAnimationTimer - 9);
		aRow = 0;
	}
	else if (mPetSpecialtyTimer < 1234)
	{
		aCol = (mPetSpecialtyTimer - 1224) % 10;
		aRow = 1;
	}
	else
	{
		if (mPetSpecialtyTimer >= 1440 && mPetSpecialtyTimer < 1450)
			aCol = 9 - (mPetSpecialtyTimer - 1440) % 10;
		else
			aCol = 9;
		aRow = m0x1c0 + 1;
	}

	g->DrawImage(IMAGE_NIKO, 0, 0, Rect(aCol * 80, aRow * 80, 80, 80));

	if (!mIsPresto && mApp->mBoard->mCurrentBackgroundId == 3)
		g->DrawImage(IMAGE_NIKOTOWER, 4, 67);
}

void Sexy::OtherTypePet::DrawClyde(Graphics* g, bool mirror)
{
	if (mSleepy)
		g->DrawImage(IMAGE_ZZZ, mirror ? 30 : 50, -27);
	g->DrawImage(IMAGE_CLYDE, 0, 0, Rect(mAnimationIndex * 80, 0, 80, 80));
}

bool Sexy::OtherTypePet::HandlePrestoClick(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
	{
		if (mIsPresto)
		{
			if (PrestoRightClicked(mPrestoTimer))
				mApp->OpenPrestoDialog(this);
			return true;
		}
		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
		return true;
	}
	return false;
}

void Sexy::OtherTypePet::CollideWithObject()
{
	// These three lists are fetched once, up front, as the original (see GetEntityToChase)
	std::vector<Alien*>* anAlienList = mApp->mBoard->mAlienList;
	std::vector<Fish*>* aFishList = mApp->mBoard->mFishList;
	std::vector<Coin*>* aCoinList = mApp->mBoard->mCoinList;

	// Every list is walked with an iterator that is dereferenced again at each access, as the original
	if (mOtherTypePetType == PET_STINKY || mOtherTypePetType == PET_CLYDE)
	{
		bool noPenta = mApp->mBoard->mPentaList->empty();
		for (std::vector<Coin*>::iterator it = aCoinList->begin(); it != aCoinList->end(); ++it)
		{
			if (noPenta || ((*it)->mCoinType != COIN_STAR && (*it)->mCoinType != SHELL_STAR))
			{
				if (!(*it)->m0x198 && (*it)->mCoinType != COIN_PEANUT)
				{
					if (mOtherTypePetType != PET_CLYDE || !RelaxModeCheck(*it))
					{
						if (mXD + 40.0 < (*it)->mX + 56 && mXD + 40.0 > (*it)->mX + 16 &&
							mYD + 40.0 < (*it)->mY + 56 && mYD + 40.0 > (*it)->mY + 16 && (*it)->mCoinType <= SHELL_END)
						{
							(*it)->PetCollected();
							(*it)->RemoveCoin();
							mPetAngryTimer = 0;
							return;
						}
					}
				}
			}
		}
	}
	else if (mOtherTypePetType == PET_RUFUS)
	{
		// The damage is written as "x = x - d": the original dereferences the iterator once to read
		// and once to write (0x4E7D5A/0x4E7D67, 0x4E7F1B/0x4E7F34)
		for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
		{
			if (abs((mXD + 40.0) - ((*it)->mX + 40)) < 10.0 &&
				abs((mYD + 40.0) - ((*it)->mY + 40)) < 10.0)
			{
				(*it)->mActiveHead->m0x1d8 = (*it)->mActiveHead->m0x1d8 - 2.0;
				mApp->mBoard->PlayPunchSound(10);
				mVX = 0.0;
				mTargetVX = 0.0;
			}
		}

		for (std::vector<Alien*>::iterator it = anAlienList->begin(); it != anAlienList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 140 && mXD + 40.0 > (*it)->mX + 30 &&
				mYD + 40.0 < (*it)->mY + 150 && mYD + 40.0 > (*it)->mY + 10 && !(*it)->mIsPsychosquidHealing)
			{
				if ((*it)->mAlienType == ALIEN_DESTRUCTOR)
					(*it)->mHealth = (*it)->mHealth - 0.25;
				else if((*it)->mAlienType == ALIEN_ULYSEES)
					(*it)->mHealth = (*it)->mHealth - 0.5;
				else if((*it)->mAlienType == ALIEN_GUS)
					(*it)->mHealth = (*it)->mHealth - 0.5;
				else
					(*it)->mHealth = (*it)->mHealth - 2.0;

				mApp->mBoard->PlayPunchSound(10);
				mVX = 0.0;
				mTargetVX = 0.0;
			}
		}
	}
	else if (mOtherTypePetType == PET_RHUBARB)
	{
		int anYCap = mApp->mBoard->AliensInTank() ? 260 : 270;

		for (std::vector<Fish*>::iterator it = aFishList->begin(); it != aFishList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX && (*it)->mY > anYCap && mPetSpecialtyTimer == 5)
			{
				(*it)->mBoughtTimer = 50;
				if (mApp->mSeed->Next() % 2 == 0)
					(*it)->mVY = -20.0;
				else
					(*it)->mVY = -30.0;
			}

			if (mXD + 40.0 < (*it)->mX + 90 && mXD + 40.0 > (*it)->mX - 10 && (*it)->mY > anYCap && mPetSpecialtyTimer == 0)
				mPetSpecialtyTimer = 20;
		}
		for (std::vector<Oscar*>::iterator it = mApp->mBoard->mOscarList->begin(); it != mApp->mBoard->mOscarList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX && (*it)->mY > anYCap && mPetSpecialtyTimer == 5)
			{
				(*it)->mBoughtTimer = 50;
				if (mApp->mSeed->Next() % 2 == 0)
					(*it)->mVY = -20.0;
				else
					(*it)->mVY = -30.0;
			}

			if (mXD + 40.0 < (*it)->mX + 90 && mXD + 40.0 > (*it)->mX - 10 && (*it)->mY > anYCap && mPetSpecialtyTimer == 0)
				mPetSpecialtyTimer = 20;
		}
		for (std::vector<Gekko*>::iterator it = mApp->mBoard->mGekkoList->begin(); it != mApp->mBoard->mGekkoList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX && (*it)->mY > anYCap && mPetSpecialtyTimer == 5)
			{
				(*it)->mBoughtTimer = 50;
				if (mApp->mSeed->Next() % 2 == 0)
					(*it)->mVY = -20.0;
				else
					(*it)->mVY = -30.0;
			}

			if (mXD + 40.0 < (*it)->mX + 90 && mXD + 40.0 > (*it)->mX - 10 && (*it)->mY > anYCap && mPetSpecialtyTimer == 0)
				mPetSpecialtyTimer = 20;
		}
		for (std::vector<Breeder*>::iterator it = mApp->mBoard->mBreederList->begin(); it != mApp->mBoard->mBreederList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 80 && mXD + 40.0 > (*it)->mX && (*it)->mY > anYCap && mPetSpecialtyTimer == 5)
			{
				(*it)->mBoughtTimer = 50;
				if (mApp->mSeed->Next() % 2 == 0)
					(*it)->mVY = -20.0;
				else
					(*it)->mVY = -30.0;
			}

			if (mXD + 40.0 < (*it)->mX + 90 && mXD + 40.0 > (*it)->mX - 10 && (*it)->mY > anYCap && mPetSpecialtyTimer == 0)
				mPetSpecialtyTimer = 20;
		}
		for (std::vector<Ultra*>::iterator it = mApp->mBoard->mUltraList->begin(); it != mApp->mBoard->mUltraList->end(); ++it)
		{
			if (mXD + 40.0 < (*it)->mX + 160 && mXD + 40.0 > (*it)->mX && (*it)->mY > anYCap - 40 && mPetSpecialtyTimer == 5)
			{
				(*it)->mBoughtTimer = 50;
				if (mApp->mSeed->Next() % 2 == 0)
					(*it)->mVY = -20.0;
				else
					(*it)->mVY = -30.0;
			}

			if (mXD + 40.0 < (*it)->mX + 170 && mXD + 40.0 > (*it)->mX - 10 && (*it)->mY > anYCap - 40 && mPetSpecialtyTimer == 0)
				mPetSpecialtyTimer = 20;
		}
	}
}
