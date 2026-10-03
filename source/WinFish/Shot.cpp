#include <SexyAppFramework/WidgetManager.h>

#include "Shot.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Res.h"

using namespace Sexy;

Sexy::Shot::Shot()
{
	mType = TYPE_SHOT;
}

Sexy::Shot::Shot(int theX, int theY)
{
	mX = theX;
	mY = theY;
	mWidth = 80;
	mHeight = 80;
	m0x158 = 0;
	m0x15c = 0;
	m0x164 = 0;
	mMouseVisible = false;
	mShotType = 0;
	mType = TYPE_SHOT;
	m0x168 = 255;
}

Sexy::Shot::Shot(int theX, int theY, int theType)
{
	mX = theX;
	mY = theY;
	mType = TYPE_SHOT;
	m0x158 = 0;
	m0x15c = 0;
	if (mShotType == 0 || mShotType == 2)
		m0x164 = 0;
	else
		m0x164 = mApp->mSeed->Next() % 3;

	mMouseVisible = false;
	mShotType = theType;
	m0x168 = mApp->mSeed->Next() % 200 + 50;
	if (mShotType == 3) // tested first in the original (0x4EC722)
		mWidth = mHeight = 80;
	else if(mShotType == 7 || mShotType == 4)
		mWidth = mHeight = 60;
	else if(mShotType == 6 || mShotType == 8)
		mWidth = mHeight = 40;
	else if (mShotType == 1)
	{
		mWidth = 50;
		mHeight = 30;
	}
	else
		mWidth = mHeight = 80;
}

void Sexy::Shot::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	GameObject::UpdateCounters();
	if (m0x164 != 0)
	{
		m0x164--;
		return;
	}

	m0x158++;
	int aFrameLimit = 19;
	if (mShotType >= 6 && mShotType <= 7)
	{
		mY -= 2;
		aFrameLimit = 39;
	}
	else if(mShotType == 0)
	{
		aFrameLimit = 14;
	}
	else if(mShotType == 2)
	{
		aFrameLimit = 29;
	}
	else if(mShotType == 1)
	{
		aFrameLimit = 10;
	}
	else
	{
		if (m0x168 > 150)
			m0x158++;
		aFrameLimit = 19;
	}

	if (m0x158 > aFrameLimit)
		RemoveShot();

	m0x15c = m0x158 / 2;
	if (mShotType == 0)
		m0x15c = m0x158;
	m0x15c--;
}

void Sexy::Shot::Draw(Graphics* g)
{
	GameObject::UpdateFishSongMgr();
	if (m0x164 != 0)
		return;

	g->SetColorizeImages(true);
	g->SetColor(Color(255, 255, 255, m0x168));
	if (mShotType == 0)
	{
		int aRow = 0;
		if (mApp->mBoard->m0x3e4 >= 10)
			aRow = 2;
		else if (mApp->mBoard->m0x3e4 > 6)
			aRow = 1;

		aRow *= 2;

		SetRespectiveColor(g, 100);
		g->DrawImageCel(IMAGE_LASERS, 0, 0, m0x15c, aRow);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		SetRespectiveColor(g, m0x168);
		g->DrawImageCel(IMAGE_LASERS, 0, 0, m0x15c, aRow);
		SetRespectiveCoreColor(g, m0x168);
		g->DrawImageCel(IMAGE_LASERS, 0, 0, m0x15c, aRow + 1);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else if (mShotType == 2) // tested before 1, as the original
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else if (mShotType == 1)
	{
		g->DrawImageCel(IMAGE_CHOMP, 0, 0, m0x15c);
	}
	else if (mShotType == 3)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->DrawImageCel(IMAGE_EXPLOSION, 0, 0, m0x15c);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else if (mShotType == 4)
	{
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->DrawImageCel(IMAGE_EXPLOSIONSMALL, 0, 0, m0x15c);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}
	else if (mShotType == 5)
	{
		g->DrawImageCel(IMAGE_EXPLOSIONTINY, 0, 0, m0x15c);
	}
	else if (mShotType == 6) // smoke doesn't reset the draw mode in the original (0x4D9523)
	{
		g->DrawImageCel(IMAGE_SMOKETINY, 0, 0, m0x15c);
	}
	else if (mShotType == 7)
	{
		g->DrawImageCel(IMAGE_SMOKESMALL, 0, 0, m0x15c);
	}
	else if (mShotType == 8)
	{
		g->DrawImageCel(IMAGE_SMOKEMISSILE, 0, 0, m0x15c);
	}
	g->SetColorizeImages(false);
}

void Sexy::Shot::Remove()
{
	RemoveShot();
}

void Sexy::Shot::RemoveShot()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
}

void Sexy::Shot::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncLong(m0x158);
	theSync->SyncLong(m0x15c);
	theSync->SyncLong(mShotType);
	theSync->SyncLong(m0x164);
	theSync->SyncLong(m0x168);
}

void Sexy::Shot::SetRespectiveColor(Graphics* g, int theAlpha)
{
	// An if chain tested from 2 to 12 in the original (a switch would be a jump table); 11 shares 2's code there
	int aVal = mApp->mBoard->m0x3e4;
	if (aVal == 2)
		g->SetColor(Color(50, 180, 200, theAlpha));
	else if (aVal == 3)
		g->SetColor(Color(255, 0, 0, theAlpha));
	else if (aVal == 4)
		g->SetColor(Color(255, 55, 0, theAlpha));
	else if (aVal == 5)
		g->SetColor(Color(255, 200, 0, theAlpha));
	else if (aVal == 6)
		g->SetColor(Color(125, 255, 90, theAlpha));
	else if (aVal == 7)
		g->SetColor(Color(10, 200, 35, theAlpha));
	else if (aVal == 8)
		g->SetColor(Color(5, 150, 250, theAlpha));
	else if (aVal == 9)
		g->SetColor(Color(160, 0, 255, theAlpha));
	else if (aVal == 10)
		g->SetColor(Color(255, 0, 0, theAlpha));
	else if (aVal == 11)
		g->SetColor(Color(50, 180, 200, theAlpha));
	else if (aVal == 12)
		g->SetColor(Color(235, 0, 215, theAlpha));
}

void Sexy::Shot::SetRespectiveCoreColor(Graphics* g, int theAlpha)
{
	// An if chain in the original (tested 4, 6, 7, 10, 11, 12), not a switch
	int aVal = mApp->mBoard->m0x3e4;
	if (aVal == 4)
		g->SetColor(Color(255, 255, 100, theAlpha));
	else if (aVal == 6)
		g->SetColor(Color(255, 255, 0, theAlpha));
	else if (aVal == 7)
		g->SetColor(Color(255, 255, 100, theAlpha));
	else if (aVal == 10)
		g->SetColor(Color(100, 255, 255, m0x168));
	else if (aVal == 11)
		g->SetColor(Color(180, 225, 70, m0x168));
	else if (aVal == 12)
		g->SetColor(Color(0, 255, 0, m0x168));
	else
		g->SetColor(Color(255, 255, 255, m0x168));
}
