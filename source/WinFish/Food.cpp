#include <SexyAppFramework/WidgetManager.h>

#include "Food.h"
#include "Board.h"
#include "WinFishApp.h"
#include "Res.h"

using namespace Sexy;

Sexy::Food::Food()
{
	mClip = false;
	mType = TYPE_FOOD;
}

Sexy::Food::Food(int theX, int theY, int unk1, bool unk2, int theExoFoodType)
{
	mXD = theX;
	mClip = false;
	mYD = theY;
	mType = TYPE_FOOD;
	mExoticFoodType = theExoFoodType;
	m0x190 = unk2;
	m0x184 = false;
	mFoodType = 0;
	mX = mXD;
	mY = mYD;
	mVY = -2.0;
	mWidth = 40;
	mHeight = 40;
	m0x18c = unk1;
	if (unk1 == 2)
		mVX = 3.0;
	else
		mVX = -3.0;

	if (theExoFoodType == 2)
	{
		mMouseVisible = true;
		mDoFinger = true;
	}
	else
		mMouseVisible = false;

	m0x178 = 0;
	m0x180 = 0;
	m0x17c = mApp->mSeed->Next() % 2 + 3;
	mPickedUp = false;
	mCantEatTimer = 20;
}

Sexy::Food::~Food()
{
}

void Sexy::Food::Update()
{
	if (!mApp->mBoard || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	if (mCantEatTimer)
		mCantEatTimer--;
	if (++m0x178 > m0x17c * 10 - 1)
		m0x178 = 0;
	if (mFoodType == 3)
		mMouseVisible = true;

	if (m0x180)
	{
		m0x180--;
		if (m0x180 != 0)
			return;
		RemoveFood();
		return;
	}

	if (!m0x190 || m0x184)
	{
		if (mPickedUp)
		{
			if (mXD > 550.0)
				mXD = mXD - (mXD - 550.0) / 7.0;
			else if (mXD < 550.0)
				mXD = (550.0 - mXD) / 7.0 + mXD;

			if (mYD > 30.0)
				mYD = mYD - (mYD - 30.0) / 7.0;
			else if (mYD < 30.0)
				mYD = (30.0 - mYD) / 7.0 + mYD;

			if (mY < 40)
			{
				RemoveFood();
				mApp->mBoard->Unk07(1);
				return;
			}

			Move((int)mXD, (int)mYD);
		}
		else
			mYD = mYD + 1.5;
	}
	else if (mYD < 120.0)
		m0x184 = true;
	else if (mYD < 125.0)
		mYD = mYD - 0.5;
	else if (mYD < 130.0)
		mYD = mYD - 1.0;
	else if (mYD < 135.0)
		mYD = mYD - 1.5;
	else if (mYD < 150.0)
		mYD = mYD - 2.5;
	else
		mYD = mYD - 8.0;

	if (m0x18c)
	{
		if (mVY < 0.0)
		{
			mVY = mVY + 0.05;
			mYD = mVY + mYD;
		}

		if (m0x18c == 2)
		{
			if (mVX > 0.0)
			{
				mVX = mVX - 0.05;
				mXD = mVX + mXD;
			}
		}
		else if (m0x18c == 1 && mVX < 0.0)
		{
			mVX = mVX + 0.05;
			mXD = mVX + mXD;
		}

		// Two separate tests in the original (0x4F8DC9): the lower bound is checked after clamping
		if (mXD > 550.0)
			mXD = 550.0;
		if (mXD < 20.0)
			mXD = 20.0;
	}

	if (mYD > 410.0 || (mFoodType == 3 && mYD > 400.0))
	{
		if (mFoodType == 3)
		{
			mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
			int aNumOfSmokes = mApp->mSeed->Next() % 3 + 2;
			for (int i = 0; i < aNumOfSmokes; i++)
			{
				// The original draws the type, then Y, then X.
				int aType = mApp->mSeed->Next() % 3 + 3;
				int aY = mApp->mSeed->Next() % 30 + mY - 10;
				int aX = mApp->mSeed->Next() % 30 + mX - 10;
				mApp->mBoard->SpawnShot(aX, aY, aType);
			}
			RemoveFood();
			return;
		}

		if (mExoticFoodType == 2)
		{
			RemoveFood();
			return;
		}

		StartFadeOut();
		mYD = 410.0;
	}

	Move((int)mXD, (int)mYD);
}

void Sexy::Food::Draw(Graphics* g)
{
	UpdateFishSongMgr();
	if (m0x180)
	{
		g->SetColor(Color(255, 255, 255, 255 * m0x180 / 15));
		g->SetColorizeImages(true);
	}

	switch (mExoticFoodType)
	{
	case EXO_FOOD_SNOT:
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, 200));
		g->DrawImageCel(IMAGE_FOOD, 0, 0, m0x178 / m0x17c, 4);
		g->SetColorizeImages(false);
		break;
	case EXO_FOOD_PIZZA:
		g->DrawImage(IMAGE_PIZZA, 0, 0);
		break;
	case EXO_FOOD_ICE_CREAM:
		g->DrawImage(IMAGE_ICECREAM, 0, 0);
		break;
	case EXO_FOOD_CHICKEN:
		g->DrawImage(IMAGE_CHICKEN, 0, 0);
		break;
	default:
		switch (mFoodType)
		{
		case 0:
			g->DrawImageCel(IMAGE_FOOD, -5, -4, m0x178 / m0x17c, 0);
			break;
		case 1:
			g->DrawImageCel(IMAGE_FOOD, -5, -4, m0x178 / m0x17c, 1);
			break;
		case 2:
			g->DrawImageCel(IMAGE_FOOD, -5, -4, m0x178 / m0x17c, 2);
			break;
		case 3:
			g->DrawImageCel(IMAGE_FOOD, 0, -3, m0x178 / m0x17c, 3);
			break;
		}
		break;
	}

	// The original turns colorizing off after every case (so twice for snot: 0x4E129E, 0x4E12A7)
	g->SetColorizeImages(false);
}

void Sexy::Food::StartFadeOut()
{
	if (m0x180 <= 0)
		m0x180 = 15;
}

void Sexy::Food::MouseDown(int x, int y, int theClickCount)
{
	if (theClickCount >= 0 && mExoticFoodType == 2)
		PickUp();
	else
		mApp->mBoard->CheckMouseDown(x + mX, y + mY);
}

void Sexy::Food::Remove()
{
	RemoveFood();
}

void Sexy::Food::RemoveFood()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
}

void Sexy::Food::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncDouble(mVX);
	theSync->SyncDouble(mVY);
	theSync->SyncLong(m0x178);
	theSync->SyncLong(m0x17c);
	theSync->SyncLong(m0x180);
	theSync->SyncBool(m0x184);
	theSync->SyncBool(mPickedUp);
	theSync->SyncLong(mFoodType);
	theSync->SyncLong(m0x18c);
	theSync->SyncBool(m0x190);
	theSync->SyncLong(mExoticFoodType);
	theSync->SyncLong(mCantEatTimer);
}

void Sexy::Food::PickUp()
{
	if (!mPickedUp)
	{
		ShowFinger(false);
		mDoFinger = false;
		mMouseVisible = false;
		mPickedUp = true;
		mApp->mBoard->PlayPointsSound();
	}
}
