#include <SexyAppFramework/WidgetManager.h>

#include "DeadAlien.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Alien.h"
#include "Res.h"

Sexy::DeadAlien::DeadAlien()
{
	mType = TYPE_DEAD_ALIEN;
}

Sexy::DeadAlien::DeadAlien(double theX, double theY, int theAnimIndex, int theAlienType, bool facingRight)
{
	mType = TYPE_DEAD_ALIEN;
	mX = theX;
	mY = theY;
	mXD = theX;
	mYD = theY;
	mVX = 0;
	mVY = 0;
	mAlienType = theAlienType;
	mWidth = 160;
	mHeight = 160;
	mTimer = 125;
	m0x19c = 5;
	mMouseVisible = false;
	mFacingRight = facingRight;
	mDeathAnimationIdx = theAnimIndex;
	mApp->mBoard->mWidgetManager->AddWidget(this);
	mOpacity = 1.0;
}

void Sexy::DeadAlien::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	if (mTimer <= 0)
	{
		RemoveDeadAlien();
		return;
	}

	mTimer--;
	if (mTimer < 105)
	{
		mOpacity -= 0.02;
		if (mOpacity < 0.0)
			mOpacity = 0.0;
	}

	if (mTimer % 5 == 0 && mTimer > 50 && DeadAlienUnk01())
	{
		int anInt = InterpolateInt(25, 100, mTimer - 50, 75, false);
		if (anInt < 25)
			anInt = 25;
		else if (anInt > 100)
			anInt = 100;
		mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, anInt / 100.0);
	}

	if (mTimer % 7 == 0 && mTimer > 50)
	{
		// For each spawn the original draws the type (if any) first, then y, then x.
		int aY = mApp->mSeed->Next() % 70 + mY + 30;
		int aX = mApp->mSeed->Next() % 70 + mX + 30;
		mApp->mBoard->SpawnBubble(aX, aY);
		aY = mApp->mSeed->Next() % 50 + mY + 50;
		aX = mApp->mSeed->Next() % 50 + mX + 50;
		mApp->mBoard->SpawnBubble(aX, aY);

		int aType = mApp->mSeed->Next() % 3 + 3;
		aY = mApp->mSeed->Next() % 50 + mY + 20;
		aX = mApp->mSeed->Next() % 50 + mX + 20;
		mApp->mBoard->SpawnShot(aX, aY, aType);
		aType = mApp->mSeed->Next() % 3 + 3;
		aY = mApp->mSeed->Next() % 30 + mY + 40;
		aX = mApp->mSeed->Next() % 30 + mX + 40;
		mApp->mBoard->SpawnShot(aX, aY, aType);

		aType = (mApp->mSeed->Next() & 1) + 6;
		aY = mApp->mSeed->Next() % 50 + mY + 20;
		aX = mApp->mSeed->Next() % 50 + mX + 20;
		mApp->mBoard->SpawnShot(aX, aY, aType);
		aType = (mApp->mSeed->Next() & 1) + 6;
		aY = mApp->mSeed->Next() % 30 + mY + 40;
		aX = mApp->mSeed->Next() % 30 + mX + 40;
		mApp->mBoard->SpawnShot(aX, aY, aType);
	}

	// NaN takes the += path, as the original (0x4F63E3)
	if (mVX > 0.0)
		mVX -= 0.03;
	else
		mVX += 0.03;

	if (mVY < 2.0)
		mVY += 0.05;
	if (mXD > 490.0)
		mXD = 490.0;
	if (mXD < -10.0)
		mXD = -10.0;

	if (mAlienType == ALIEN_DESTRUCTOR || mAlienType == ALIEN_ULYSEES)
	{
		if (mYD > 280.0)
			mYD = 280.0;
		else if (mYD > 310.0) // unreachable, as in the original
			mYD = 310.0;
	}

	mXD += mVX;
	mYD += mVY;
	Move(mXD, mYD);
}

void Sexy::DeadAlien::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	int anAlienType = mAlienType;
	g->SetColorizeImages(true);
	g->SetColor(Color(255, 255, 255, (int)(mOpacity * 255.0)));
	// Not initialized in the original; every alien type that leaves a DeadAlien is listed
	int anImgId;
	if (anAlienType == ALIEN_WEAK_SYLV || anAlienType == ALIEN_STRONG_SYLV)
		anImgId = IMAGE_SYLV_ID;
	if (anAlienType == ALIEN_BALROG)
		anImgId = IMAGE_BALROG_ID;
	else if (anAlienType == ALIEN_DESTRUCTOR)
		anImgId = IMAGE_DESTRUCTOR_ID;
	else if (anAlienType == ALIEN_ULYSEES)
		anImgId = IMAGE_ULYSSES_ID;
	else if (anAlienType == ALIEN_PSYCHOSQUID)
		anImgId = IMAGE_PSYCHOSQUID_ID;
	else if (anAlienType == ALIEN_CYRAX)
		anImgId = IMAGE_BOSS_ID;

	// The image and the rect are fetched and built again for every draw, as the original
	g->DrawImageMirror(GetImageById(anImgId), 0, 0, Rect(mDeathAnimationIdx * 160, 0, 160, 160), mFacingRight);
	if (mTimer > 115)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColor(Color(255, 255, 255, (mTimer - 115) * 20));
		for (int i = 0; i < 5; i++)
		{
			g->DrawImageMirror(GetImageById(anImgId), 0, 0, Rect(mDeathAnimationIdx * 160, 0, 160, 160), mFacingRight);
		}
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	g->SetColorizeImages(false);
}

void Sexy::DeadAlien::Remove()
{
	RemoveDeadAlien();
}

void Sexy::DeadAlien::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncBool(mFacingRight);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncDouble(m0x180);
	theSync->SyncLong(mDeathAnimationIdx);
	theSync->SyncDouble(mOpacity);
	theSync->SyncLong(mTimer);
	theSync->SyncLong(m0x19c);
	theSync->SyncLong(mAlienType);
}

void Sexy::DeadAlien::RemoveDeadAlien()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
}

bool Sexy::DeadAlien::DeadAlienUnk01()
{
	int aMaxNum = 0;
	for (std::vector<DeadAlien*>::iterator it = mApp->mBoard->mDeadAlienList->begin(); it != mApp->mBoard->mDeadAlienList->end(); ++it)
	{
		if ((*it)->mTimer > aMaxNum)
			aMaxNum = (*it)->mTimer;
	}
	return mTimer >= aMaxNum;
}
