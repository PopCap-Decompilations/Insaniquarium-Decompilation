#include "BilaterusBone.h"
#include "Bilaterus.h"
#include "BilaterusHead.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Fish.h"
#include "Res.h"

Sexy::BilaterusBone::BilaterusBone(Bilaterus* theBilaterus, int theX, int theY, int theBoneId)
{
	mXD = theX;
	mBilaterus = theBilaterus;
	mYD = theY;
	mBoneId = theBoneId;
	mX = (int)mXD;
	mY = (int)mYD;
	m0x170 = 0;
	m0x178 = -0.5;
	mWidth = 80;
	m0x198 = 1.0;
	mHeight = 80;
	if (mApp->mSeed->Next() % 2 == 0)
	{
		m0x170 = -0.1;
		m0x198 = -1.0;
	}
	m0x1a0 = 370;
	m0x180 = 0;
	m0x1a8 = 95;
	m0x188 = 0;
	m0x1a4 = 10;
	m0x1ac = 540;
	m0x190 = 0.8;
	m0x1c0 = 100;
	m0x1b0 = 0;
	m0x1b4 = 0;
	m0x1c8 = 0;
	m0x1b8 = 0;
	m0x1d0 = 0;
	mMouseVisible = false;
	m0x1d8 = 0;
	m0x1bc = 0;
	m0x1e0 = 0;
	m0x1e8 = 12;
}

Sexy::BilaterusBone::~BilaterusBone()
{
}

void Sexy::BilaterusBone::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	// Facing as the original; with m0x170 and m0x198 both 0 nothing is drawn
	if (m0x170 < 0.0 || ((int)m0x170 == 0 && m0x198 < 0.0))
		DrawBone(g, false);
	else if (m0x170 > 0.0 || ((int)m0x170 == 0 && m0x198 > 0.0))
		DrawBone(g, true);
}

void Sexy::BilaterusBone::OnFoodAte(GameObject* obj)
{
	gUnkInt03 = mApp->mUpdateCount;
	mApp->mBoard->PlayChompSound(false);
	if (mApp->m0x882)
	{
		for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
		{
			// The original draws the y offset before the x offset
			int aShotX, aShotY;
			if (obj->mType == TYPE_ULTRA)
			{
				aShotY = mApp->mSeed->Next() % 20 + 55 + obj->mY;
				aShotX = mApp->mSeed->Next() % 20 + 55 + obj->mX;
			}
			else
			{
				aShotY = mApp->mSeed->Next() % 20 + 15 + obj->mY;
				aShotX = mApp->mSeed->Next() % 20 + 15 + obj->mX;
			}
			mApp->mBoard->SpawnShot(aShotX, aShotY, 1);
		}
	}
}

void Sexy::BilaterusBone::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncLong(mBoneId);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(m0x170);
	theSync->SyncDouble(m0x178);
	theSync->SyncDouble(m0x180);
	theSync->SyncDouble(m0x188);
	theSync->SyncDouble(m0x190);
	theSync->SyncDouble(m0x198);
	theSync->SyncLong(m0x1a0);
	theSync->SyncLong(m0x1a4);
	theSync->SyncLong(m0x1a8);
	theSync->SyncLong(m0x1ac);
	theSync->SyncLong(m0x1b0);
	theSync->SyncLong(m0x1b4);
	theSync->SyncLong(m0x1b8);
	theSync->SyncLong(m0x1bc);
	theSync->SyncLong(m0x1c0);
	theSync->SyncDouble(m0x1c8);
	theSync->SyncDouble(m0x1d0);
	theSync->SyncDouble(m0x1d8);
	theSync->SyncDouble(m0x1e0);
	theSync->SyncLong(m0x1e8);
}

void Sexy::BilaterusBone::UpdateBone()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	if (mBilaterus == nullptr || mBilaterus->mActiveHead == nullptr)
		return;

	m0x1e8++;
	if (m0x1e8 >= 12)
	{
		m0x1e8 = 0;
		if (mBoneId == 0)
		{
			m0x1d8 = mBilaterus->mActiveHead->mVX;
			m0x1e0 = mBilaterus->mActiveHead->mVY;
		}
		else
		{
			m0x1d8 = (*mBilaterus->mBoneList)[mBoneId - 1]->m0x170;
			m0x1e0 = (*mBilaterus->mBoneList)[mBoneId - 1]->m0x178;
		}
	}

	if (mBoneId == 0)
	{
		m0x1c8 = mBilaterus->mActiveHead->mXD;
		m0x1d0 = mBilaterus->mActiveHead->mYD;
	}
	else
	{
		m0x1c8 = (*mBilaterus->mBoneList)[mBoneId - 1]->mXD;
		m0x1d0 = (*mBilaterus->mBoneList)[mBoneId - 1]->mYD;
	}

	if (m0x170 < m0x1d8)
		m0x170 += 0.02;
	if (m0x170 > m0x1d8)
		m0x170 -= 0.02;
	if (m0x178 < m0x1e0)
		m0x178 += 0.02;
	if (m0x178 > m0x1e0)
		m0x178 -= 0.02;

	UpdatePrevVX();
	double aMaxXDist = 30.0;
	if (mBoneId == 0)
		aMaxXDist = 40.0;

	if (mXD < m0x1c8 - aMaxXDist)
		mXD = m0x1c8 - aMaxXDist;
	if (mXD > m0x1c8 + aMaxXDist)
		mXD = m0x1c8 + aMaxXDist;
	if (mYD > m0x1d0 + 20.0)
		mYD = m0x1d0 + 20.0;
	if (mYD < m0x1d0 - 20.0)
		mYD = m0x1d0 - 20.0;

	if (mXD < m0x1a4)
		mXD = m0x1a4;
	if (mXD > m0x1ac)
		mXD = m0x1ac;
	if (mYD > m0x1a0)
		mYD = m0x1a0;
	if (mYD < m0x1a8)
		mYD = m0x1a8;

	mXD += m0x170 / m0x190;
	mYD += m0x178 / m0x190;

	if (m0x1bc > 0)
		m0x1bc--;
	if (m0x1c0 > 0)
		m0x1c0--;

	Move(mXD, mYD);
	if (m0x1c0 <= 0)
		CheckCollision();
}

void Sexy::BilaterusBone::UpdatePrevVX()
{
	if (m0x170 != m0x198 && m0x170 != 0.0 && m0x198 != 0.0)
		m0x198 = m0x170;
}

void Sexy::BilaterusBone::UpdateFollowPosAndVel()
{
	if (mBoneId == 0)
	{
		m0x1d8 = mBilaterus->mActiveHead->mVX;
		m0x1e0 = mBilaterus->mActiveHead->mVY;
		m0x1c8 = mBilaterus->mActiveHead->mXD;
		m0x1d0 = mBilaterus->mActiveHead->mYD;
	}
	else
	{
		m0x1d8 = (*mBilaterus->mBoneList)[mBoneId - 1]->m0x170;
		m0x1e0 = (*mBilaterus->mBoneList)[mBoneId - 1]->m0x178;
		m0x1c8 = (*mBilaterus->mBoneList)[mBoneId - 1]->mXD;
		m0x1d0 = (*mBilaterus->mBoneList)[mBoneId - 1]->mYD;
	}
}

void Sexy::BilaterusBone::CheckCollision()
{
	if (!CanAlienChaseAnyFish())
		return;

	int aCX = mX + mWidth / 2;
	int aCY = mY + mHeight / 2;

	// The original keeps the set's address for the whole loop (0x4E9643)
	Board* aBoard = mApp->mBoard;
	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		if (anObj->mVirtualTankId >= 0 || anObj->mCanBeEatenDelay > 0) continue;

		int aXEatDistObj = 30;
		int aYEatDistObj = 30;
		bool canBeEaten = false;
		switch (anObj->mType)
		{
		case TYPE_GUPPY:
			canBeEaten = !Fish::WadsworthActive(anObj);
			break;
		case TYPE_ULTRA:
			aXEatDistObj = 70;
			aYEatDistObj = 70;
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
			int ax = aCX - anObj->mWidth / 2 - anObj->mX;
			int ay = aCY - anObj->mHeight / 2 - anObj->mY;
			if (ax > -aXEatDistObj && ax < aXEatDistObj && ay > -aYEatDistObj && ay < aYEatDistObj)
			{
				OnFoodAte(anObj);
				anObj->Remove();
				return;
			}
		}
	}
}

void Sexy::BilaterusBone::DrawBone(Graphics* g, bool mirror)
{
	if (mBilaterus->m0x16c != 0)
		return;

	int isEven = mBoneId % 2;
	double aDeltaX;
	if (mBoneId == 0)
		aDeltaX = mXD - mBilaterus->mActiveHead->mXD;
	else
		aDeltaX = mXD - (*mBilaterus->mBoneList)[mBoneId - 1]->mXD;

	// The original's three tests, with their NaN paths (0x4DC563-0x4DC58A)
	int aFrame;
	if (aDeltaX >= 36.0)
		aFrame = 0;
	else if (aDeltaX >= 0.0 || aDeltaX > -30.0)
		aFrame = 5 - (int)(aDeltaX / 6.0);
	else
		aFrame = 0;

	// The original never mirrors a bone; the flag is not passed on
	DrawBoneHelper(g, IMAGE_BILATERUS, aFrame * -80, (-6 - isEven) * 80);
}

void Sexy::BilaterusBone::DrawBoneHelper(Graphics* g, Image* theImage, int theSrcX, int theSrcY)
{
	g->DrawImage(theImage, theSrcX, theSrcY);
	if (m0x1bc > 0)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, m0x1bc * 25));
		g->DrawImage(theImage, theSrcX, theSrcY);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		g->SetColorizeImages(false);
	}
}
