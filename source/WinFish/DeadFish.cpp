#include <SexyAppFramework/WidgetManager.h>

#include "DeadFish.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Shadow.h"
#include "Res.h"

using namespace Sexy;

Sexy::DeadFish::DeadFish()
{
	mType = TYPE_DEAD_FISH;
}

Sexy::DeadFish::DeadFish(double theX, double theY, double theVX, double theVY, double theSpeedMod, int theObjType, bool facingRight)
	: GameObject()
{
	mType = TYPE_DEAD_FISH;
	mX = theX;
	mY = theY;
	mXD = theX;
	mYD = theY;
	mObjType = theObjType;
	mVX = theVX;
	mVY = theVY; // stored before the adjusted value, as the original (0x4EEDA1)

	if (theY < 115.0 || theVY < -3.0)
		mVY = theVY - 1.0;
	else
		mVY = theVY - 2.0;

	mSpeedMod = theSpeedMod;
	mWidth = 80;
	mHeight = 80;
	if (theObjType == TYPE_ULTRA)
	{
		mWidth = 160;
		mHeight = 160;
	}

	m0x1a0 = 125;
	mMouseVisible = false;
	mFlipped = facingRight;
	m0x18c = 0;
	m0x190 = 100;
	m0x188 = false;
	mApp->mBoard->mWidgetManager->AddWidget(this); // Weird, being called 2 times for the same object
	m0x198 = 1.0;
}

void Sexy::DeadFish::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	UpdateCounters();

	if (m0x190 >= 0 && m0x190 <= 10)
	{
		m0x190--;
		m0x18c = m0x190;
		if (m0x190 == 10)
			m0x190 = 10; // ??? WHAT
	}
	else
	{
		if (mObjType == 9 || mObjType == 8)
		{
			if (m0x1a0 > 105)
				m0x18c = 9 - (m0x1a0 - 106) / 2;
			else
				m0x18c = 9;
		}
		else
		{
			if (m0x1a0 <= 105)
			{
				if (m0x1a0 == 104 || m0x1a0 == 103)
					m0x18c = 8;
				else if (m0x1a0 == 102 || m0x1a0 == 101)
					m0x18c = 7;
				else
					m0x18c = m0x1a0 > 100 ? 9 : 6;
			}
			else
				m0x18c = 9 - (m0x1a0 - 106) / 2;
		}
	}

	if (m0x1a0 < 105)
	{
		m0x198 -= 0.02;
		if (m0x198 < 0.0)
			m0x198 = 0;
		if (mShadowPtr)
			mShadowPtr->m0x168 = m0x198;
	}
	if (m0x1a0 <= 0)
	{
		RemoveDeadFish();
		return;
	}

	if (m0x190 <= 0) // Ressurection (Angel Pet)
	{
		mApp->mBoard->PlaySample(SOUND_HEAL_ID, 3, 1.0);
		RemoveDeadFish();
		if (mObjType == TYPE_GEKKO)
			mApp->mBoard->RessurectGekko(mX, mY, mFlipped);
		else if (mObjType == TYPE_OSCAR)
			mApp->mBoard->RessurectOscar(mX, mY, mFlipped);
		else if (mObjType == TYPE_GRUBBER)
			mApp->mBoard->RessurectGrubber(mX);
		else if (mObjType == TYPE_PENTA)
			mApp->mBoard->RessurectPenta(mX);
		else if (mObjType >= TYPE_BREEDER && mObjType <= TYPE_BREEDER + 2)
			mApp->mBoard->RessurectBreeder(mX, mY, mObjType - TYPE_BREEDER, mFlipped);
		else if (mObjType == TYPE_ULTRA)
			mApp->mBoard->RessurectUltra(mX, mY, mFlipped);
		else
			mApp->mBoard->RessurectFish(mX, mY, mObjType, mFlipped);

		return;
	}

	if ((mObjType == TYPE_ULTRA && mYD > 300.0) ||
		((mObjType == TYPE_GRUBBER || mObjType == TYPE_PENTA) && mYD > 365.0) ||
		(mYD > 370.0 || m0x1a0 > 105))
	{
		m0x1a0--;
	}

	if (mVX > 0.0)
	{
		mVX -= 0.03;
		if (mVX < 0.0)
			mVX = 0;
	}
	else if (mVX < 0.0)
	{
		mVX += 0.03;
		if (mVX > 0.0)
			mVX = 0;
	}

	if (m0x188)
	{
		if (mVY > -1.5)
			mVY -= 0.05;
		else if (mVY < -1.5)
			mVY += 0.05;
	}
	else
	{
		if (mVY < 2.0)
			mVY += 0.05;
	}

	mXD += mVX / mSpeedMod;
	mYD += mVY / mSpeedMod;
	if (mXD > 540.0)
		mXD = 540.0;
	if (mXD < 10.0)
		mXD = 10.0;

	if (mObjType == TYPE_GRUBBER || mObjType == TYPE_PENTA)
	{
		if(mYD > 370.0)
			mYD = 370.0;
	}
	else if (mObjType == TYPE_ULTRA)
	{
		if (mYD > 310.0)
			mYD = 310.0;
	}
	else
	{
		if (mYD > 380.0)
			mYD = 380.0;
	}

	if (mYD < 85.0)
		mYD = 85.0;

	Move(mXD, mYD);
}

void Sexy::DeadFish::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	int anObjType = mObjType;
	g->SetColorizeImages(true);
	g->SetColor(Color(255, 255, 255, (int)(m0x198 * 255.0)));

	int anYVal = 0;
	if (m0x1a0 < 90)
		anYVal = (90 - m0x1a0) / 2;

	if (anObjType == TYPE_GEKKO)
		g->DrawImageMirror(IMAGE_GEKKO, 0, anYVal, Rect(m0x18c * 80, 400, 80, 80), mFlipped);
	else if (anObjType == TYPE_ULTRA)
	{
		if (m0x1a0 >= 90) // 97
		{
			int aRed = (m0x18c * 5 + 1250) / 5;
			int aGreen = (m0x18c * 40 + 1075) / 5;
			int aBlue = (m0x18c * 160 + 475) / 5;
			if (m0x18c < 5)
			{
				g->SetColor(Color(aRed, aGreen, aBlue, 255));
				g->SetColorizeImages(true);
			}
		}
		g->DrawImageMirror(IMAGE_ULTRA, 0, anYVal, Rect(m0x18c * 160, 480, 160, 160), mFlipped);
	}
	else if (anObjType == TYPE_GRUBBER)
	{
		if (m0x1a0 >= 90)
		{
			int aRed = (m0x18c * 5 + 1250) / 5;
			int aGreen = (m0x18c * 40 + 1075) / 5;
			int aBlue = (m0x18c * 160 + 475) / 5;
			if (m0x18c < 5)
			{
				g->SetColor(Color(aRed, aGreen, aBlue, 255));
				g->SetColorizeImages(true);
			}
		}
		g->DrawImage(IMAGE_GRUBBER, 0, anYVal, Rect(m0x18c * 80, 240, 80, 80));
	}
	else if (anObjType == TYPE_PENTA)
		g->DrawImage(IMAGE_STARCATCHER, 0, anYVal, Rect(m0x18c * 80, 160, 80, 80));
	else if (anObjType == TYPE_OSCAR)
		g->DrawImageMirror(IMAGE_SMALLDIE, 0, anYVal, Rect(m0x18c * 80, 320, 80, 80), mFlipped);
	else if (anObjType <= TYPE_CROWNED_GUPPY)
	{
		if (anObjType >= TYPE_STAR_GUPPY)
			anObjType--;
		g->DrawImageMirror(IMAGE_SMALLDIE, 0, anYVal, Rect(m0x18c * 80, anObjType * 80, 80, 80), mFlipped);
	}
	else if (anObjType >= TYPE_BREEDER) // breeders
		g->DrawImageMirror(IMAGE_HUNGRYBREEDER, 0, anYVal, Rect(m0x18c * 80, (anObjType - TYPE_BREEDER) * 240 + 160, 80, 80), mFlipped);
	g->SetColorizeImages(false);
}

void Sexy::DeadFish::Remove()
{
	RemoveDeadFish();
}

void Sexy::DeadFish::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncBool(mFlipped);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncDouble(mSpeedMod);
	theSync->SyncBool(m0x188);
	theSync->SyncLong(m0x18c);
	theSync->SyncLong(m0x190);
	theSync->SyncLong(m0x194);
	theSync->SyncDouble(m0x198);
	theSync->SyncLong(m0x1a0);
	theSync->SyncLong(mObjType);
}

void Sexy::DeadFish::RemoveDeadFish()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();
}

// Angie touching a fresh corpse starts the countdown to its resurrection
void Sexy::DeadFish::StartRessurection()
{
	if (m0x190 == 100)
		m0x190 = 10;
}
