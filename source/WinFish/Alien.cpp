#include <SexyAppFramework/WidgetManager.h>

#include "Alien.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "ProfileMgr.h"
#include "Missle.h"
#include "Food.h"
#include "Fish.h"
#include "Shadow.h"
#include "Warp.h"
#include "Coin.h"
#include "Res.h"


Sexy::Alien::Alien()
{
	mType = TYPE_ALIEN;
	mClip = false;
}

Sexy::Alien::Alien(int theX, int theY, int theAlienType)
{
	mXD = theX;
	mYD = theY;
	mType = TYPE_ALIEN;
	mClip = false;
	mX = mXD;
	mY = mYD;
	mTargetVX = 0;
	mTargetVY = 0;
	mWidth = 160;
	mHeight = 160;
	if (mApp->mSeed->Next() % 2 == 0)
	{
		mVX = -3;
		mPrevVX = -1;
	}
	else
	{
		mVX = 3;
		mPrevVX = 1;
	}

	mVY = 0;
	mAnimIndex = 1;
	mAlienType = theAlienType;
	mMouseVisible = false;
	mTurnAnimTimer = 0;
	mHitFlashTimer = 0;
	mChaseTargetDelay = 100;
	mMissleLaunchTimer = 0;
	mMissleReloadTime = 75;
	mSpecialAnimTimer = 0;
	mCyraxSpawnAlienTimer = 0;
	mCyraxSpawnAlienThreshold = 300;
	mCyraxSpawnMiniTimer = 0;
	mCyraxSpawnMiniThreshold = 75;
	mIsPsychosquidHealing = false;
	if (theAlienType == ALIEN_WEAK_SYLV)
	{
		mSpeedMod = 2.0;
		mHealth = 50.0;
	}
	else if (theAlienType == ALIEN_STRONG_SYLV)
	{
		mSpeedMod = 1.6;
		mHealth = 60.0;
	}
	else if (theAlienType == ALIEN_MINI_SYLV)
	{
		mSpeedMod = (mApp->mSeed->Next() % 5) / 10.0 + 0.8; // drawn before the stores below (0x4ED117)
		mWidth = mHeight = 80;
		mChaseTargetDelay = 40;
		mHitFlashTimer = 30;
		mHealth = 1.0;
	}
	else if (theAlienType == ALIEN_BALROG)
	{
		mSpeedMod = 1.2;
		mHealth = 130.0;
	}
	else if (theAlienType == ALIEN_CYRAX)
	{
		mSpeedMod = 2.0;
		int aVal = mApp->mCurrentProfile->mFinishedGameCount - 1;
		if (aVal < 0)
			aVal = 0;
		else if (aVal > 20)
			aVal = 20;
		mHealth = (5000 - (aVal * 2500) / 20);
	}
	else if (theAlienType == ALIEN_GUS)
	{
		mSpeedMod = 1.6;
		mHealth = 100.0;
	}
	else if (theAlienType == ALIEN_DESTRUCTOR)
	{
		mSpeedMod = 1.2;
		mHealth = 150.0;
		if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			mY = 280;
			mYD = 280.0;
		}
	}
	else if (theAlienType == ALIEN_ULYSEES)
	{
		mSpeedMod = 3.5;
		mHealth = 220.0;
		if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		{
			mY = 280;
			mYD = 280.0;
		}
	}
	else if (theAlienType == ALIEN_PSYCHOSQUID)
	{
		mSpeedMod = 0.5;
		mIsPsychosquidHealing = false;
		mPsychosquidSwitchTimer = 200;
		mHealth = 260.0;
		mPsychosquidSwitchThreshold = 400;
		mWasPsychosquidHealing = false;
	}

	if (mApp->mRelaxMode)
	{
		if (mAlienType == 4)
			mHealth = Rand() % 50 + 70;
		else
			mHealth = Rand() % 100 + 150;
		mMissleReloadTime = 144;
	}

	mMovementState = mApp->mSeed->Next() % 10; // drawn before the stores below (0x4ED313)
	mMaxHealth = mHealth;
	m0x1d4 = 40;
	mMovementChangeTimer = 20;
	mSwimAnimTimer = 0;
	mSpawnAnimationTimer = 15;
}

void Sexy::Alien::Update()
{
	if (mApp->mBoard == NULL || mApp->mBoard->mPause)
		return;

	GameObject::UpdateCounters();

	if ((mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_ULYSEES) && mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (mYD >= 280.0)
		{
			mYD = 280.0;
			mVY = 0;
		}
		else
			mVY = mSpeedMod * 0.4 + mVY;
	}

	int local8 = 0;
	if (mSpawnAnimationTimer != 0)
	{
		if (mSpawnAnimationTimer <= 10)
		{
			if (mVX >= 0.0)
				local8 = 3;
			else
				local8 = -3;
		}
		mSpawnAnimationTimer--;
		if (mSpawnAnimationTimer > 8)
			return;
	}

	bool flag1 = AlienUnk01();
	if (!flag1)
	{
		if (mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_ULYSEES)
		{
			if (mMovementState == 0)
				mTargetVX = -1;
			else if(mMovementState == 1)
				mTargetVX = 1;
			else if(mMovementState == 2)
				mTargetVX = -1;
			else if(mMovementState == 3)
				mTargetVX = 1;
		}
		else
		{
			if (mMovementState == 0)
				mTargetVX = -1.5;
			else if (mMovementState == 1)
				mTargetVX = 1.5;
			else if (mMovementState == 2)
				mTargetVY = -1.5;
			else if (mMovementState == 3)
				mTargetVY = 1.5;

			if (mVY > mTargetVY)
				mVY -= 0.1;
			if (mVY < mTargetVY)
				mVY += 0.1;
		}
		if (mVX > mTargetVX)
			mVX -= 0.1;
		if (mVX < mTargetVX)
			mVX += 0.1;
		m0x1d4++;
		if (mSpecialAnimTimer == 0)
		{
			mMovementChangeTimer++;
			if (mMovementChangeTimer > 20)
			{
				mMovementChangeTimer = 0;
				if (mApp->mSeed->Next() % 10 == 0)
					mMovementState = mApp->mSeed->Next() % 4;
			}
		}
	}

	if (mAlienType == ALIEN_PSYCHOSQUID)
	{
		mPsychosquidSwitchTimer++;
		if (mPsychosquidSwitchTimer >= mPsychosquidSwitchThreshold && !mApp->mRelaxMode) // Psychosquid Switch
		{
			mIsPsychosquidHealing = !mIsPsychosquidHealing;
			if (mIsPsychosquidHealing)
				mWasPsychosquidHealing = true;

			mSpecialAnimTimer = 10;
			mPsychosquidSwitchTimer = mApp->mSeed->Next() % 100;
			if (!mIsPsychosquidHealing)
			{
				mSpeedMod = 0.5;
				mApp->mBoard->PlaySample(SOUND_ROAR3_ID, 3, 1.0);
				mApp->mBoard->ResetMessageWidget(-1);
			}
			else
				mSpeedMod = 2.0;
		}
	}

	if (mAlienType == ALIEN_MINI_SYLV)
	{
		if (mXD > 540.0)
			mXD = 540.0;
		if (mXD < 10.0)
			mXD = 10.0;
		if (mYD > 370.0)
			mYD = 370.0;
		if (mYD < 95.0)
			mYD = 95.0;
	}
	else
	{
		if (mXD > 490.0)
			mXD = 490.0;
		if (mXD < -10.0)
			mXD = -10.0;
		if (mYD > 290.0)
			mYD = 290.0;
		if (mYD < 85.0)
			mYD = 85.0;
	}

	if (mAlienType == ALIEN_ULYSEES)
	{
		if (mSpecialAnimTimer <= 0)
		{
			int aVal = mAnimIndex % 5;
			if (aVal < 1 || aVal > 2)
			{
				if (aVal > 3 || aVal < 1)
				{
					if (mVX > 0.9)
						mVX -= 0.8;
					else if (mVX < -0.9)
						mVX += 0.8;
				}
			}
			else if (mVX > 0.0)
				mVX += 0.55;
			else if (mVX < 0.0)
				mVX -= 0.55;
		}
		else if (mVX > 0.1)
			mVX = 0.1;
		else if (mVX < -0.1)
			mVX = -0.1;
	}

	mXD = mVX / mSpeedMod + mXD + local8; // summed in this order, as the original
	mYD = mVY / mSpeedMod + mYD;

	if (mHitFlashTimer > 0)
		mHitFlashTimer--;
	if (mChaseTargetDelay > 0)
		mChaseTargetDelay--;

	if (mAlienType == ALIEN_DESTRUCTOR && TankHasFish())
	{
		mMissleLaunchTimer++;
		if (mMissleLaunchTimer > mMissleReloadTime)
		{
			bool missleLaunched = false;
			// Each facing looks for its own target, after the facing test (0x4FDB50, 0x4FDC17)
			if (mVX >= 0.0)
			{
				GameObject* aTarget = FindObjectToShoot(true);
				if (aTarget)
				{
					missleLaunched = true;
					mApp->mBoard->SpawnMissle(mX + 90, mY - 35, aTarget, Missle::MISSLE_CLASSIC);
					mApp->mBoard->PlaySample(SOUND_MISSLE_ID, 3, 1.0);
					if (!mApp->mRelaxMode && (aTarget = FindObjectToShoot(true), aTarget != nullptr))
					{
						mApp->mBoard->SpawnMissle(mX + 105, mY - 30, aTarget, Missle::MISSLE_CLASSIC);
						if (aTarget = FindObjectToShoot(true), aTarget != nullptr)
							mApp->mBoard->SpawnMissle(mX + 120, mY - 25, aTarget, Missle::MISSLE_CLASSIC);
					}
				}
			}
			else
			{
				GameObject* aTarget = FindObjectToShoot(true);
				if (aTarget)
				{
					missleLaunched = true;
					mApp->mBoard->SpawnMissle(mX - 10, mY - 35, aTarget, Missle::MISSLE_CLASSIC);
					mApp->mBoard->PlaySample(SOUND_MISSLE_ID, 3, 1.0);
					if (!mApp->mRelaxMode && (aTarget = FindObjectToShoot(true), aTarget != nullptr))
					{
						mApp->mBoard->SpawnMissle(mX - 25, mY - 30, aTarget, Missle::MISSLE_CLASSIC);
						if (aTarget = FindObjectToShoot(true), aTarget != nullptr)
							mApp->mBoard->SpawnMissle(mX - 40, mY - 25, aTarget, Missle::MISSLE_CLASSIC);
					}
				}
			}

			mMissleLaunchTimer = 0;
			if (mApp->mRelaxMode)
				mMissleReloadTime = mApp->mSeed->Next() % 50 + 360;
			else
				mMissleReloadTime = mApp->mSeed->Next() % 50 + 150;

			if (missleLaunched)
				mSpecialAnimTimer = 10;
		}
	}
	else if (mAlienType == ALIEN_ULYSEES && TankHasFish()) // not tested after the Destructor block (0x4FDD2F)
	{
		mMissleLaunchTimer++;
		if (mMissleLaunchTimer <= mMissleReloadTime)
		{
			if (mMissleReloadTime - 15 == mMissleLaunchTimer)
			{
				if (mTurnAnimTimer != 0)
					mMissleLaunchTimer -= 2;
				else if (FindObjectToShoot(false))
					mSpecialAnimTimer = 40;
				else
					mMissleLaunchTimer = 0;
			}
		}
		else
		{
			if (mVX >= 0.0)
			{
				GameObject* aTarget = FindObjectToShoot(true);
				if (aTarget)
				{
					mApp->mBoard->SpawnMissle(mX + 70, mY - 24, aTarget, Missle::MISSLE_ENERGYBALL);
					mApp->mBoard->PlaySample(SOUND_UNLEASH_ID, 3, 1.0);
					if (!mApp->mRelaxMode && (aTarget = FindObjectToShoot(true), aTarget != nullptr))
						mApp->mBoard->SpawnMissle(mX + 78, mY - 16, aTarget, Missle::MISSLE_ENERGYBALL);
				}
			}
			else
			{
				GameObject* aTarget = FindObjectToShoot(true);
				if (aTarget)
				{
					mApp->mBoard->SpawnMissle(mX + 10, mY - 24, aTarget, Missle::MISSLE_ENERGYBALL);
					mApp->mBoard->PlaySample(SOUND_UNLEASH_ID, 3, 1.0);
					if (!mApp->mRelaxMode && (aTarget = FindObjectToShoot(true), aTarget != nullptr))
						mApp->mBoard->SpawnMissle(mX + 2, mY - 16, aTarget, Missle::MISSLE_ENERGYBALL);
				}
			}
			mMissleLaunchTimer = 0;

			if (mApp->mRelaxMode)
				mMissleReloadTime = mApp->mSeed->Next() % 50 + 360;
			else
				mMissleReloadTime = mApp->mSeed->Next() % 50 + 150;
		}
	}

	Move(mXD, mYD);
	UpdateAnimations();
	if (mHealth > 0.0)
	{
		if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && mUpdateCnt > 4320)
		{
			mApp->mBoard->SpawnLaserShot(mWidth / 2 - 40 + mX, mHeight / 2 - 40 + mY);
			mApp->mBoard->PlaySample(SOUND_SFX_ID, 3, 1.0);
			RemoveHelper02(true);
			return;
		}
	}
	else
	{
		Remove(true);
	}
}

void Sexy::Alien::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	if (mVX < 0.0)
		DrawAlien(g, false);
	else
		DrawAlien(g, true);
}

void Sexy::Alien::CountRequiredFood(int* theFoodReqPtr)
{
	if (mAlienType == ALIEN_GUS)
	{
		theFoodReqPtr[7] += 3;
		theFoodReqPtr[5] += 3;
		theFoodReqPtr[6] += 3;
	}
	else
	{
		theFoodReqPtr[0]++;
		theFoodReqPtr[3]++;
		theFoodReqPtr[4]++;
	}
}

void Sexy::Alien::VFT74()
{
	CyraxDieCleanup();
	mApp->mBoard->Unk12();
}

void Sexy::Alien::Remove()
{
	Remove(true);
}

void Sexy::Alien::OnFoodAte(GameObject* obj)
{
	gUnkInt03 = mApp->mUpdateCount;
	mApp->mBoard->PlayChompSound(false);
	if (mAlienType == ALIEN_GUS)
	{
		mHitFlashTimer = 10;
		mHealth -= 15.0;
	}

	if (mApp->m0x882)
	{
		for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
		{
			// The original draws the y offset before the x offset
			int x, y;
			if (obj->mType == TYPE_ULTRA)
			{
				y = mApp->mSeed->Next() % 20 + 55 + obj->mY;
				x = mApp->mSeed->Next() % 20 + 55 + obj->mX;
			}
			else
			{
				y = mApp->mSeed->Next() % 20 + 15 + obj->mY;
				x = mApp->mSeed->Next() % 20 + 15 + obj->mX;
			}
			mApp->mBoard->SpawnShot(x, y, 1);
		}
	}
}

void Sexy::Alien::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncLong(mSpawnAnimationTimer);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncDouble(mTargetVX);
	theSync->SyncDouble(mTargetVY);
	theSync->SyncLong(mPsychosquidSwitchTimer);
	theSync->SyncLong(mPsychosquidSwitchThreshold);
	theSync->SyncBool(mWasPsychosquidHealing);
	theSync->SyncLong(mChaseTargetDelay);
	theSync->SyncLong(mMissleLaunchTimer);
	theSync->SyncLong(mMissleReloadTime);
	theSync->SyncLong(mSpecialAnimTimer);
	theSync->SyncLong(mSwimAnimTimer);
	theSync->SyncLong(mTurnAnimTimer);
	theSync->SyncDouble(mSpeedMod);
	theSync->SyncLong(mAnimIndex);
	theSync->SyncDouble(mPrevVX);
	theSync->SyncLong(mMovementState);
	theSync->SyncLong(m0x1d4);
	theSync->SyncLong(mMovementChangeTimer);
	theSync->SyncLong(mAlienType);
	theSync->SyncDouble(mHealth);
	theSync->SyncDouble(mMaxHealth);
	theSync->SyncLong(mHitFlashTimer);
	theSync->SyncBool(mIsPsychosquidHealing);
	if (mAlienType == ALIEN_CYRAX)
	{
		theSync->SyncLong(mCyraxSpawnAlienTimer);
		theSync->SyncLong(mCyraxSpawnMiniTimer);
		theSync->SyncLong(mCyraxSpawnAlienThreshold);
		theSync->SyncLong(mCyraxSpawnMiniThreshold);
	}
}

bool Sexy::Alien::Shot(int x, int y)
{
	if (x <= mXD || x >= mXD + 160.0 || y <= mYD || y >= mYD + 160.0 || mHitFlashTimer > 0)
		return false;

	if (mAlienType == ALIEN_PSYCHOSQUID && mIsPsychosquidHealing)
	{
		if (mApp->mBoard->mCyraxPtr == nullptr)
		{
			mHealth += mApp->mBoard->m0x3e4 * 3;
			mApp->mBoard->ShowText("Stop shooting! Alien regains health!", true, -1);
		}
	}
	else if (mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_ULYSEES)
	{
		mHealth -= mApp->mBoard->m0x3e4 * 2 + 2;
		mApp->mBoard->SpawnShot(x - 40, y - 40, 2);
		mApp->mBoard->PlaySample(SOUND_HIT_ID, 3, 1.0);
	}
	else
	{
		mHealth -= mApp->mBoard->m0x3e4 * 3;
		mApp->mBoard->SpawnShot(x - 40, y - 40, 2);
		mApp->mBoard->PlaySample(SOUND_HIT_ID, 3, 1.0);
		if (mAlienType == ALIEN_PSYCHOSQUID && mHealth < 100.0 && !mWasPsychosquidHealing && !mApp->mRelaxMode)
		{
			mIsPsychosquidHealing = true;
			mWasPsychosquidHealing = true;
			mSpecialAnimTimer = 10;
			mPsychosquidSwitchTimer = mApp->mSeed->Next() % 100;
		}
	}

	if (mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_ULYSEES)
	{
		if (mSpecialAnimTimer == 0)
		{
			if (x < mXD + 60.0)
				mVX = mSpeedMod * 3.0;
			else if(x > mXD + 100.0)
				mVX = mSpeedMod * -3.0;
		}
	}
	else if (mAlienType == ALIEN_PSYCHOSQUID)
	{
		if (x < mXD + 60.0 && y < mYD + 60.0)
		{
			double aSpeed = mSpeedMod * 3.5;
			mVY = aSpeed; // mVY is stored first here, mVX first in the -3.5 case (0x4FABAE)
			mVX = aSpeed;
		}
		else if (x < mXD + 60.0 && y < mYD + 100.0)
		{
			mVX = mSpeedMod * 4.0;
		}
		else if (x < mXD + 60.0)
		{
			mVX = mSpeedMod * 3.5;
			mVY = mSpeedMod * -3.5;
		}
		else if (x < mXD + 100.0 && y < mYD + 60.0)
		{
			mVY = mSpeedMod * 4.0;
		}
		else if (x > mXD + 100.0 && y > mYD + 100.0)
		{
			double aSpeed = mSpeedMod * -3.5;
			mVX = aSpeed;
			mVY = aSpeed;
		}
		else if (x > mXD + 100.0 && y < mYD + 60.0)
		{
			mVX = mSpeedMod * -3.5;
			mVY = mSpeedMod * 3.5;
		}
		else if (x > mXD + 100.0)
		{
			mVX = mSpeedMod * -4.0;
		}
		else if (y > mYD + 100.0)
		{
			mVY = mSpeedMod * -4.0;
		}
	}
	else
	{
		if (x < mXD + 60.0 && y < mYD + 60.0)
		{
			double aSpeed = mSpeedMod * 2.5;
			mVY = aSpeed; // mVY is stored first here, mVX first in the -2.5 case (0x4FA862)
			mVX = aSpeed;
		}
		else if (x < mXD + 60.0 && y < mYD + 100.0)
		{
			mVX = mSpeedMod * 3.0;
		}
		else if (x < mXD + 60.0)
		{
			mVX = mSpeedMod * 2.5;
			mVY = mSpeedMod * -2.5;
		}
		else if (x < mXD + 100.0 && y < mYD + 60.0)
		{
			mVY = mSpeedMod * 3.0;
		}
		else if (x > mXD + 100.0 && y > mYD + 100.0)
		{
			double aSpeed = mSpeedMod * -2.5;
			mVX = aSpeed;
			mVY = aSpeed;
		}
		else if (x > mXD + 100.0 && y < mYD + 60.0)
		{
			mVX = mSpeedMod * -2.5;
			mVY = mSpeedMod * 2.5;
		}
		else if (x > mXD + 100.0)
		{
			mVX = mSpeedMod * -3.0;
		}
		else if (y > mYD + 100.0)
		{
			mVY = mSpeedMod * -3.0;
		}
	}

	if (mHealth <= 0.0)
	{
		Remove(true);
		return true;
	}
	mHitFlashTimer = 10;
	return false;
}

bool Sexy::Alien::TankHasFish()
{
	if (mApp->mBoard->mCyraxPtr == nullptr)
		return mApp->mBoard->HasAnyFish();
	return true;
}

bool Sexy::Alien::AlienUnk01()
{
	// One condition in the original: a Destructor, Ulysses or healing Psychosquid falls through to
	// the Cyrax test below (0x4F045E), as does a tank without prey
	if ((TankHasFish() || (mAlienType == ALIEN_GUS && !mApp->mBoard->mFoodList->empty()) || mApp->mBoard->mCyraxPtr != nullptr) &&
		mAlienType != ALIEN_CYRAX && mAlienType != ALIEN_DESTRUCTOR && mAlienType != ALIEN_ULYSEES && !mIsPsychosquidHealing)
	{
		if (FindNearestFood())
		{
			ChaseBehavior();
			return true;
		}
		return false;
	}

	if (mAlienType == ALIEN_CYRAX)
	{
		if (mApp->mBoard->mAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
		{
			mCyraxSpawnAlienTimer++;
			if (mCyraxSpawnAlienTimer >= mCyraxSpawnAlienThreshold)
			{
				mCyraxSpawnAlienTimer = 0;
				mApp->mBoard->mAlienExpect = mApp->mSeed->Next() % 6 + 4;
				mApp->mBoard->SpawnAlien(mApp->mBoard->mAlienExpect, true);
			}
		}
		mCyraxSpawnMiniTimer++;
		if (mCyraxSpawnMiniTimer >= mCyraxSpawnMiniThreshold)
		{
			mCyraxSpawnMiniTimer = 0;
			mApp->mBoard->CyraxSpawnMiniAlien(mX, mY);
		}
	}
	return false;
}

void Sexy::Alien::CyraxDieCleanup()
{
	if (mAlienType == ALIEN_CYRAX)
	{
		mApp->mBoard->mCyraxPtr = nullptr;
		mApp->mBoard->m0x4f4 = true;

		std::vector<GameObject*> anObjVector;

		// Iterator loops in the original
		for (std::vector<Alien*>::iterator it = mApp->mBoard->mSmallAlienList->begin(); it != mApp->mBoard->mSmallAlienList->end(); ++it)
			anObjVector.push_back(*it);

		for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
			anObjVector.push_back(*it);

		for (std::vector<Missle*>::iterator it = mApp->mBoard->mMissleList1->begin(); it != mApp->mBoard->mMissleList1->end(); ++it)
			anObjVector.push_back(*it);

		for (std::vector<GameObject*>::iterator it = anObjVector.begin(); it != anObjVector.end(); ++it)
			(*it)->RemoveHelper02(true);
	}
}

// The flag is unused, as in the original (true when launching, false for Ulysses' charge-up check)
Sexy::GameObject* Sexy::Alien::FindObjectToShoot(bool theLaunching)
{
	int aCX = mX + mWidth / 2;
	int aCY = mY + mHeight / 2;
	mGusFoundFood = false;
	int aLongestDist = 0;
	GameObject* aTarget = nullptr;
	for (GameObjectSet::iterator it = mApp->mBoard->mGameObjectSet.begin(); it != mApp->mBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 && anObj->mMisslePtr == nullptr)
		{
			bool canAttack = false;
			switch (anObj->mType)
			{
			case TYPE_GUPPY:
				canAttack = !Fish::WadsworthActive(anObj);
				break;
			case TYPE_OSCAR:
			case TYPE_ULTRA:
			case TYPE_GEKKO:
			case TYPE_PENTA:
			case TYPE_GRUBBER:
			case TYPE_BREEDER:
				canAttack = true;
				break;
			case TYPE_OTHER_TYPE_PET:
			case TYPE_FISH_TYPE_PET:
				canAttack = (mApp->mBoard->mCyraxPtr != nullptr);
				break;
			default:
				continue;
			}

			if (canAttack)
			{
				int ax = aCX - (anObj->mWidth / 2 + anObj->mX);
				int ay = aCY - (anObj->mHeight / 2 + anObj->mY);
				int aDist = ax * ax + ay * ay;
				if (aDist > aLongestDist)
				{
					aLongestDist = aDist;
					aTarget = anObj;
				}
			}
		}
	}
	return aTarget;
}

Sexy::GameObject* Sexy::Alien::FindNearestFood()
{
	int aCX = mX + mWidth / 2;
	int aCY = mY + mHeight / 2;
	mGusFoundFood = false;
	int aClosestDist = 100000000;
	GameObject* aRet = nullptr;

	if (mAlienType == ALIEN_GUS)
	{
		for (std::vector<Food*>::iterator it = mApp->mBoard->mFoodList->begin(); it != mApp->mBoard->mFoodList->end(); ++it)
		{
			Food* aFood = *it;

			if (aFood->mCantEatTimer <= 0 && aFood->mExoticFoodType != 2)
			{
				int ax = aCX - (aFood->mX + aFood->mWidth / 2);
				int ay = aCY - (aFood->mY + aFood->mHeight / 2);
				int aDist = ax * ax + ay * ay;
				if (aDist < aClosestDist)
				{
					aClosestDist = aDist - 2500;
					aRet = aFood;
					mGusFoundFood = true;
				}
			}
		}
	}

	if (!CanAlienChaseAnyFish() || (mAlienType == ALIEN_GUS && mChaseTargetDelay > 0))
		return aRet;

	for (GameObjectSet::iterator it = mApp->mBoard->mGameObjectSet.begin(); it != mApp->mBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0)
		{
			bool canAttack = false;
			switch (anObj->mType)
			{
			case TYPE_GUPPY:
				canAttack = !Fish::WadsworthActive(anObj);
				break;
			case TYPE_OSCAR:
			case TYPE_ULTRA:
			case TYPE_GEKKO:
			case TYPE_PENTA:
			case TYPE_GRUBBER:
			case TYPE_BREEDER:
				canAttack = true;
				break;
			case TYPE_OTHER_TYPE_PET:
			case TYPE_FISH_TYPE_PET:
				canAttack = (mApp->mBoard->mCyraxPtr != nullptr);
				break;
			default:
				continue;
			}

			if (canAttack)
			{
				int ax = aCX - (anObj->mWidth / 2 + anObj->mX);
				int ay = aCY - (anObj->mHeight / 2 + anObj->mY);
				int aDist = ax * ax + ay * ay;
				if (aDist < aClosestDist)
				{
					aClosestDist = aDist;
					aRet = anObj;
				}
			}
		}
	}
	return aRet;
}

bool Sexy::Alien::ChaseBehavior()
{
	GameObject* aTarget = FindNearestFood();

	// Without a target the original still makes the tests below (0x4ED471 jumps to 0x4ED739)
	if (aTarget != nullptr)
	{
		if (mAlienType == ALIEN_GUS && mGusFoundFood)
		{
			if (mXD + 80.0 > aTarget->mX + 20)
			{
				if (mVX > -1.8)
					mVX -= 0.1;
			}
			else if (mXD + 80.0 < aTarget->mX + 20)
			{
				if (mVX < 1.8)
					mVX += 0.1;
			}

			if (mYD + 80.0 > aTarget->mY + 20)
			{
				if (mVY > -1.8)
					mVY -= 0.1;
			}
			else if (mYD + 80.0 < aTarget->mY + 20)
			{
				if (mVY < 3.0)
					mVY += 0.3;
			}
		}
		else if (mAlienType == ALIEN_MINI_SYLV)
		{
			if (mXD + 40.0 > aTarget->mX + 40)
			{
				if (mVX > -1.8)
					mVX -= 0.1;
			}
			else if (mXD + 40.0 < aTarget->mX + 40)
			{
				if (mVX < 1.8)
					mVX += 0.1;
			}

			if (mYD + 40.0 > aTarget->mY + 40)
			{
				if (mVY > -1.8)
					mVY -= 0.1;
			}
			else if (mYD + 40.0 < aTarget->mY + 40)
			{
				if (mVY < 1.8)
					mVY += 0.1;
			}
		}
		else
		{
			if (mXD + 80.0 > aTarget->mX + 40)
			{
				if (mVX > -1.8)
					mVX -= 0.1;
			}
			else if (mXD + 80.0 < aTarget->mX + 40)
			{
				if (mVX < 1.8)
					mVX += 0.1;
			}

			if (mYD + 80.0 > aTarget->mY + 40)
			{
				if (mVY > -1.8)
					mVY -= 0.1;
			}
			else if (mYD + 80.0 < aTarget->mY + 40)
			{
				if (mVY < 1.8)
					mVY += 0.1;
			}
		}
	}

	if ((mChaseTargetDelay <= 0 || mAlienType == ALIEN_GUS) && aTarget != nullptr)
		CheckCollision();
	return aTarget != nullptr;
}

void Sexy::Alien::CheckCollision()
{
	int aCX = mX + mWidth / 2;
	int aCY = mY + mHeight / 2;
	int aXEatDist = (mAlienType == ALIEN_MINI_SYLV ? 30 : 45);
	int aYEatDist = (mAlienType == ALIEN_MINI_SYLV ? 30 : 65);

	if (mAlienType == ALIEN_GUS)
	{
		for (std::vector<Food*>::iterator it = mApp->mBoard->mFoodList->begin(); it != mApp->mBoard->mFoodList->end(); ++it)
		{
			Food* aFood = *it;

			if (mXD + 80.0 < aFood->mX + 65 && mXD + 80.0 > aFood->mX - 25 &&
				mYD + 80.0 < aFood->mY + 65 && mYD + 80.0 > aFood->mY - 25 &&
				aFood->mCantEatTimer == 0 && mHitFlashTimer <= 5)
			{
				if (mApp->mRelaxMode)
				{
					mHealth -= 8.0;
				}
				else
				{
					if (aFood->mFoodType == 3)
					{
						mHealth -= 20.0;
						mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
						for (int i = mApp->mSeed->Next() % 3 + 1; i > 0; i--)
						{
							// The original draws the type first, then y, then x
							int aType = mApp->mSeed->Next() % 3 + 3;
							int aY = mApp->mSeed->Next() % 40 + 10 + mY;
							int aX = mApp->mSeed->Next() % 40 + 10 + mX;
							mApp->mBoard->SpawnShot(aX, aY, aType);
							aType = mApp->mSeed->Next() % 3 + 3;
							aY = mApp->mSeed->Next() % 20 + 20 + mY;
							aX = mApp->mSeed->Next() % 20 + 20 + mX;
							mApp->mBoard->SpawnShot(aX, aY, aType);
						}
					}
					else
					{
						if (aFood->mExoticFoodType == EXO_FOOD_PIZZA)
							mHealth -= 30.0;
						else if(aFood->mExoticFoodType == EXO_FOOD_CHICKEN)
							mHealth -= 25.0;
						else if(aFood->mExoticFoodType == EXO_FOOD_ICE_CREAM)
							mHealth -= 20.0;
						else
							mHealth -= (*it)->mFoodType * 2 + 4; // the original dereferences the iterator again here (0x4E9291)
					}
				}

				mHitFlashTimer = 10;
				mApp->mBoard->PlaySlurpSound(false);
				aFood->RemoveFood();
				break; // one pellet per frame, as the original
			}
		}
	}

	if (CanAlienChaseAnyFish() && (mAlienType != ALIEN_GUS || mChaseTargetDelay <= 0))
	{
		for (GameObjectSet::iterator it = mApp->mBoard->mGameObjectSet.begin(); it != mApp->mBoard->mGameObjectSet.end(); ++it)
		{
			GameObject* anObj = *it;
			if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0)
			{
				int aXEatDistObj = aXEatDist;
				int aYEatDistObj = aYEatDist;
				bool canBeEaten = false;
				switch (anObj->mType)
				{
				case TYPE_GUPPY:
					canBeEaten = !Fish::WadsworthActive(anObj);
					break;
				case TYPE_ULTRA:
					aXEatDistObj = 70;
					aYEatDistObj = 85;
				case TYPE_OSCAR:
				case TYPE_GEKKO:
				case TYPE_PENTA:
				case TYPE_GRUBBER:
				case TYPE_BREEDER:
					canBeEaten = true;
					break;
				case TYPE_OTHER_TYPE_PET:
				case TYPE_FISH_TYPE_PET:
					canBeEaten = (mApp->mBoard->mCyraxPtr != nullptr);
					break;
				default:
					continue;
				}

				if (canBeEaten)
				{
					int ax = aCX - (anObj->mWidth / 2 + anObj->mX);
					int ay = aCY - (anObj->mHeight / 2 + anObj->mY);
					if (ax > -aXEatDistObj && ax < aXEatDistObj && ay > -aYEatDistObj && ay < aYEatDistObj)
					{
						OnFoodAte(anObj);
						anObj->Remove();
						return;
					}
				}
			}
		}
	}
}

void Sexy::Alien::DrawAlien(Graphics* g, bool mirror)
{
	Rect aSrcRect;
	int local10 = mAnimIndex;
	bool shouldMirror = mirror; 
	int ivar4 = mSpecialAnimTimer;
	int aSize = 160; 
	aSrcRect.mY = 0;
	int aWidth = 160; 
	if (ivar4 != 0 && (mAlienType == ALIEN_PSYCHOSQUID || mAlienType == ALIEN_ULYSEES))
		aSrcRect.mY = (mAlienType == ALIEN_PSYCHOSQUID) * 2 + 2;
	else if (mTurnAnimTimer != 0)
	{
		aSrcRect.mY = 1;
		shouldMirror = mTurnAnimTimer > 0;
	}
	aSrcRect.mX = local10;
	Image* anAlienImage = nullptr;
	switch (mAlienType)
	{
	case ALIEN_WEAK_SYLV:
	case ALIEN_STRONG_SYLV:
		anAlienImage = IMAGE_SYLV;
		break;
	case ALIEN_BALROG:
		anAlienImage = IMAGE_BALROG;
		break;
	case ALIEN_GUS:
		anAlienImage = IMAGE_GUS;
		if (mHitFlashTimer > 0 && mApp->mBoard->mCyraxPtr == nullptr)
		{
			shouldMirror = mirror;
			aSrcRect.mY = 2;
			aSrcRect.mX = mHitFlashTimer; // TODO Fix This Causes debug assert (must me < 10)
		}
		break;
	case ALIEN_DESTRUCTOR:
		anAlienImage = IMAGE_DESTRUCTOR;
		break;
	case ALIEN_ULYSEES:
		anAlienImage = IMAGE_ULYSSES;
		break;
	case ALIEN_PSYCHOSQUID:
		anAlienImage = IMAGE_PSYCHOSQUID;
		if (ivar4 == 0 && mIsPsychosquidHealing)
			aSrcRect.mY = (mTurnAnimTimer != 0) + 2;
		break;
	case ALIEN_MINI_SYLV:
		aSize = 80;
		aWidth = 80;
		anAlienImage = IMAGE_MINISYLV;
		break;
	case ALIEN_CYRAX:
		anAlienImage = IMAGE_BOSS;
		break;
	default:
		return;
	}

	aSrcRect.mX *= aSize;
	aSrcRect.mY *= aSize;
	aSrcRect.mWidth = aSize;
	aSrcRect.mHeight = aSize;

	if (mSpawnAnimationTimer > 0)
	{
		if (mSpawnAnimationTimer >= 10)
			return;
		// Divided in double, stored in a float and scaled in double, as the original
		float aShrinkRatio = (double)mSpawnAnimationTimer / 10.0;
		if (aShrinkRatio <= 0.0)
			return;

		int aInsetSize = (int)(aShrinkRatio * (double)aWidth);

		int aX = aInsetSize / 2;
		int aY = aInsetSize / 2;
		int aW = aSize - aInsetSize;
		int aH = aSize - aInsetSize;

		Rect aDestRect(aX, aY, aW, aH);

		g->SetFastStretch(!mApp->Is3DAccelerated());
		g->DrawImageMirror(anAlienImage, aDestRect, aSrcRect, shouldMirror);
		return;
	}

	g->DrawImageMirror(anAlienImage, 0, 0, aSrcRect, shouldMirror);

	if (((mHitFlashTimer > 0 && mAlienType != ALIEN_MINI_SYLV) &&
		(mAlienType != ALIEN_GUS || mApp->mBoard->mCyraxPtr != nullptr)) 
		&& !mIsPsychosquidHealing)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, mHitFlashTimer * 25));
		g->DrawImageMirror(anAlienImage, 0, 0, aSrcRect, shouldMirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
	else if(mSpecialAnimTimer > 0 && mSpecialAnimTimer < 10 && mAlienType == ALIEN_DESTRUCTOR)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		int anAlphaMod = mSpecialAnimTimer;
		if (anAlphaMod > 5)
			anAlphaMod = 10 - anAlphaMod;
		g->SetColor(Color(255, 255, 255, (anAlphaMod * 255) / 5));
		g->DrawImageMirror(anAlienImage, 0, 0, aSrcRect, shouldMirror);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}

	if (mAlienType == ALIEN_CYRAX || (mApp->mBoard->mPetsInTank[PET_BLIP] != 0 && mApp->mBoard->mTank != 5))
	{
		Image* aBarImg = IMAGE_HEALTHBAR;
		int aY = mHeight - 3;
		int aBarWidth = (int)((double)aBarImg->mWidth * mHealth / mMaxHealth);
		if (aBarWidth < 0) aBarWidth = 0;
		else if (aBarWidth > aBarImg->mWidth) aBarWidth = aBarImg->mWidth; // not tested after clamping to 0 (0x4DB179)
		Rect aSrcRect(0, 0, aBarWidth, aBarImg->mHeight);
		g->DrawImage(aBarImg, 10, aY, aSrcRect);
		g->DrawImage(IMAGE_HEALTHBARTUBE, 10, aY);
	}
}

void Sexy::Alien::UpdateAnimations()
{
	if (mPrevVX > 0.0 && mVX < 0.0)
	{
		mTurnAnimTimer = 10;
		if (mAlienType == ALIEN_ULYSEES)
			mTurnAnimTimer = 20;
	}
	else if (mPrevVX < 0.0 && mVX > 0.0)
	{
		mTurnAnimTimer = -10;
		if (mAlienType == ALIEN_ULYSEES)
			mTurnAnimTimer = -20;
	}

	if (mTurnAnimTimer > 0)
		mTurnAnimTimer--;
	else if (mTurnAnimTimer < 0)
		mTurnAnimTimer++;

	if (mSpecialAnimTimer > 0 && --mSpecialAnimTimer == 0)
		mSwimAnimTimer = 0;

	int aFrame = mSpecialAnimTimer;
	if (aFrame <= 0)
	{
		aFrame = mTurnAnimTimer;
		if (aFrame == 0)
		{
			if (mAlienType == ALIEN_ULYSEES)
			{
				if (mVX < -8.0 || mVX > 8.0)
					aFrame = mSwimAnimTimer + 2;
				else
					aFrame = mSwimAnimTimer + 1;
				mSwimAnimTimer = aFrame % 80;
				mAnimIndex = mSwimAnimTimer / 8;
			}
			else
			{
				if (mAlienType == ALIEN_CYRAX || mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_PSYCHOSQUID ||
					(mVX >= -1.6 && mVX <= 1.6))
				{
					mSwimAnimTimer++;
					if (mSwimAnimTimer >= 20)
						mSwimAnimTimer = 0;
					mAnimIndex = mSwimAnimTimer/2;
				}
				else
				{
					mSwimAnimTimer++;
					if (mSwimAnimTimer <= 3)
					{
						mXD -= mVX / mSpeedMod * 0.5 * 0.5;
						mYD -= mVY / mSpeedMod * 0.5 * 0.5;
						mAnimIndex = mSwimAnimTimer/2;
					}
					else if (mSwimAnimTimer <= 6)
					{
						mXD -= mVX / mSpeedMod * 0.5 * 0.5;
						mYD -= mVY / mSpeedMod * 0.5 * 0.5;
						mAnimIndex = mSwimAnimTimer/2;
					}
					else if (mSwimAnimTimer <= 10)
					{
						mXD += mVX / mSpeedMod * 0.5 * 1.5;
						mYD += mVY / mSpeedMod * 0.5 * 1.5;
						mAnimIndex = mSwimAnimTimer/2;
					}
					else if (mSwimAnimTimer <= 39)
					{
						mAnimIndex = 6;
						mXD += mVX / mSpeedMod * 0.5;
						mYD += mVY / mSpeedMod * 0.5;
					}
					else if (mSwimAnimTimer <= 50)
					{
						mXD += mVX / mSpeedMod * 0.5;
						mYD += mVY / mSpeedMod * 0.5;
						mAnimIndex = abs(mSwimAnimTimer-50)/2;
					}
					else
					{
						mAnimIndex = 0;
						mSwimAnimTimer = 0;
					}
				}
			}
		}
		else if (mAlienType == ALIEN_ULYSEES)
		{
			if (aFrame > 0) mAnimIndex = 9 - aFrame / 2;
			else if (aFrame < 0) mAnimIndex = aFrame / 2 + 9;
		}
		else
		{
			if (aFrame > 0)
				mAnimIndex = 9 - aFrame;
			else if (aFrame < 0)
				mAnimIndex = aFrame + 10;
		}
	}
	else
	{
		if (mAlienType == ALIEN_DESTRUCTOR)
			mAnimIndex = mSpecialAnimTimer;
		else if (mAlienType == ALIEN_ULYSEES)
		{
			if (aFrame > 20)
			{
				aFrame = aFrame - 20;
				mAnimIndex = 9 - aFrame / 2;
			}
			else
				mAnimIndex = (aFrame - 1) / 2;
		}
		else if (mAlienType == ALIEN_PSYCHOSQUID)
		{
			if (mIsPsychosquidHealing)
				mAnimIndex = 10 - aFrame;
			else
				mAnimIndex = aFrame;
		}
	}

	if (mVX != mPrevVX && mVX != 0.0 && mPrevVX != 0.0)
		mPrevVX = mVX;
	if (mMisslePtr)
		UpdateCrosshairAnimation();
}

void Sexy::Alien::Remove(bool removeShadow)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();
	if (mApp->mBoard->mCyraxPtr == nullptr && removeShadow && CanDropCoin())
		mApp->mBoard->DropCoin(mX + 25, mY + 25, COIN_DIAMOND, nullptr, -1.0, 0);

	RemoveAlien();

	mApp->mBoard->Unk12();
	if (removeShadow)
	{
		// Draw order as the original: a shot's type first, then y, then x; a bubble's y before its x
		int aType, aX, aY;
		if (mAlienType == ALIEN_GUS)
		{
			for (int i = (mApp->mSeed->Next() & 1) + 2; i > 0; i--)
			{
				aY = mApp->mSeed->Next() % 70 + 30 + mY;
				aX = mApp->mSeed->Next() % 70 + 30 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
				aY = mApp->mSeed->Next() % 50 + 50 + mY;
				aX = mApp->mSeed->Next() % 50 + 50 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
			}

			for (int i = mApp->mSeed->Next() % 3 + 4; i > 0; i--)
			{
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 50 + 20 + mY;
				aX = mApp->mSeed->Next() % 50 + 20 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 30 + 40 + mY;
				aX = mApp->mSeed->Next() % 30 + 40 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			for (int i = (mApp->mSeed->Next() & 1) + 2; i > 0; i--)
			{
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 50 + 20 + mY;
				aX = mApp->mSeed->Next() % 50 + 20 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 30 + 40 + mY;
				aX = mApp->mSeed->Next() % 30 + 40 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
			mApp->mBoard->PlaySample(SOUND_EXPLOSION1_ID, 3, 1.0);
		}
		else if (mAlienType == ALIEN_MINI_SYLV)
		{
			for (int i = (mApp->mSeed->Next() & 1) + 2; i > 0; i--)
			{
				aY = mApp->mSeed->Next() % 35 + mY;
				aX = mApp->mSeed->Next() % 35 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
				aY = mApp->mSeed->Next() % 25 + 10 + mY;
				aX = mApp->mSeed->Next() % 25 + 10 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
			}

			for (int i = mApp->mSeed->Next() % 3 + 4; i > 0; i--)
			{
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 25 + mY;
				aX = mApp->mSeed->Next() % 25 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 15 + 10 + mY;
				aX = mApp->mSeed->Next() % 15 + 10 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			for (int i = (mApp->mSeed->Next() & 1) + 2; i > 0; i--)
			{
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 25 + mY;
				aX = mApp->mSeed->Next() % 25 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 15 + 10 + mY;
				aX = mApp->mSeed->Next() % 15 + 10 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			mApp->mBoard->PlaySample(SOUND_EXPLOSION4_ID, 3, 1.0);
		}
		else
		{
			for (int i = (mApp->mSeed->Next() & 1); i > 0; i--)
			{
				aY = mApp->mSeed->Next() % 70 + 30 + mY;
				aX = mApp->mSeed->Next() % 70 + 30 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
				aY = mApp->mSeed->Next() % 50 + 50 + mY;
				aX = mApp->mSeed->Next() % 50 + 50 + mX;
				mApp->mBoard->SpawnBubble(aX, aY);
			}

			for (int i = mApp->mSeed->Next() % 3; i > 0; i--)
			{
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 50 + 20 + mY;
				aX = mApp->mSeed->Next() % 50 + 20 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = mApp->mSeed->Next() % 3 + 3;
				aY = mApp->mSeed->Next() % 30 + 40 + mY;
				aX = mApp->mSeed->Next() % 30 + 40 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			for (int i = (mApp->mSeed->Next() & 1); i > 0; i--)
			{
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 50 + 20 + mY;
				aX = mApp->mSeed->Next() % 50 + 20 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
				aType = (mApp->mSeed->Next() & 1) + 6;
				aY = mApp->mSeed->Next() % 30 + 40 + mY;
				aX = mApp->mSeed->Next() % 30 + 40 + mX;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}

			mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
			mApp->mBoard->PlaySample(SOUND_EXPLOSION1_ID, 3, 1.0);
			bool facingRight = (mVX > 0.0);
			mApp->mBoard->SpawnDeadAlien(mXD, mYD, mAnimIndex, mAlienType, facingRight);
		}
	}
	CyraxDieCleanup();
}

void Sexy::Alien::RemoveAlien()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();
}

void Sexy::Alien::SpawnWarp()
{
	if (mAlienType == ALIEN_MINI_SYLV)
		return;

	// The original tests the facing but builds the same warp either way: two new expressions
	// (EH states 0 and 1) sharing one constructor call (0x4ED3A4)
	Warp* aWarp;
	if (mVX > 0.0)
		aWarp = new Warp(mX + 30, mY - 40);
	else
		aWarp = new Warp(mX + 30, mY - 40);
	mApp->mBoard->AddGameObject(aWarp, true);
	mApp->mBoard->mWidgetManager->AddWidget(aWarp);
	mApp->mBoard->mWidgetManager->BringToFront(aWarp);
}

bool Sexy::Alien::FoodDroppedAtAlien(int theX, int theY)
{
	int aCX = mX + mWidth / 2;
	int aCY = mY + mHeight / 2;
	if (aCX < theX + 75 && aCX > theX - 35 && aCY < theY + 75 && aCY > theY - 35)
		return true;
	return false;
}
