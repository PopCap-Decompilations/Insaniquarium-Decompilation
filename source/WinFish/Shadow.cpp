#include <SexyAppFramework/WidgetManager.h>

#include "Shadow.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Res.h"

Sexy::Shadow::Shadow()
{
	mObjectPtr = nullptr;
	mClip = false;
	mType = TYPE_SHADOW;
}

Sexy::Shadow::Shadow(int theSize, GameObject* theObject)
{
	mObjectPtr = theObject;
	mType = TYPE_SHADOW;
	if (theObject != nullptr)
	{
		theObject->mShadowPtr = this;
		m0x160 = (mObjectPtr->mY - 50) / 2;
	}
	else
		m0x160 = 0;
	m0x168 = 1.0;
	mX = -200;
	mY = mApp->mSeed->Next() % 5 + 410;
	mWidth = 80;
	mHeight = 40;
	mMouseVisible = false;
	mClip = false;
	mShadowSize = theSize;
}

void Sexy::Shadow::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	GameObject::UpdateCounters();
	if (mObjectPtr == nullptr)
		return;

	m0x160 = (mObjectPtr->mY - 50) / 2;
	int aX = mObjectPtr->mX;
	if (mShadowSize == 2)
		aX += 40;
	mX = aX;
	if (mShadowSize == 0 || mShadowSize == 2)
	{
		int aY = 370 - mObjectPtr->mY;
		if (aY < 0)
			aY = 0;
		mY = 415 - (aY*30) / 370;
	}
}

void Sexy::Shadow::Draw(Graphics* g)
{
	GameObject::UpdateFishSongMgr();
	g->SetColorizeImages(true);
	if (mObjectPtr->mInvisible && mObjectPtr->mType >= TYPE_PENTA && mObjectPtr->mType <= TYPE_GRUBBER)
		return; // the original leaves colorize on here

	Color aColor = gUnkColor01;
	// The windows.h min/max macros in the original: tests 0, then the maximum, then 0 again
	double anAlpha;
	if (mShadowSize == 2)
		anAlpha = min(255.0, max(0.0, (2 * m0x160) * m0x168));
	else
		anAlpha = min(150.0, max(0.0, m0x160 * m0x168));
	aColor.mAlpha = (int)anAlpha;
	g->SetColor(aColor);
	if (mShadowSize == 1)
	{
		g->DrawImage(IMAGE_SHADOW, 0, 0);
		g->SetColorizeImages(false);
		return;
	}

	Rect aSrcRect(0, 0, IMAGE_SHADOW->mWidth, IMAGE_SHADOW->mHeight);
	Rect aDestRect(0, 0, IMAGE_SHADOW->mWidth, IMAGE_SHADOW->mHeight);

	if (mObjectPtr != nullptr && mApp->Is3DAccelerated())
	{
		float aScale = (mY - mObjectPtr->mY) / 100.0;
		if (aScale < 0.9f)
			aScale = 0.9f;
		else if (aScale > 1.3f)
			aScale = 1.3f;

		if (mShadowSize == 2)
			aScale += 0.4f;
		double aTransform = aScale - 1.0;

		int aDeltaWidth = (int)(IMAGE_SHADOW->mWidth * aTransform);
		int aDeltaHeight = (int)(aTransform * IMAGE_SHADOW->mHeight);

		aDestRect.mX -= aDeltaWidth;
		aDestRect.mY -= aDeltaHeight;
		aDestRect.mWidth += aDeltaWidth * 2;
		aDestRect.mHeight += aDeltaHeight * 2;
	}
	g->SetFastStretch(false);
	DrawImageMirrorHelper(g, IMAGE_SHADOW, aDestRect, aSrcRect, false);
	g->SetColorizeImages(false);
}

void Sexy::Shadow::VFT74()
{
	if (mObjectPtr != nullptr)
	{
		mObjectPtr->mShadowPtr = nullptr;
		mObjectPtr = nullptr;
	}
}

void Sexy::Shadow::Remove()
{
	RemoveShadow();
}

void Sexy::Shadow::RemoveShadow()
{
	if (mObjectPtr != nullptr)
	{
		mObjectPtr->mShadowPtr = nullptr;
		mObjectPtr = nullptr;
	}
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
}

void Sexy::Shadow::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncLong(mShadowSize);
	theSync->SyncLong(m0x160);
	theSync->SyncDouble(m0x168);
	theSync->SyncPointer((void**) &mObjectPtr);
}
