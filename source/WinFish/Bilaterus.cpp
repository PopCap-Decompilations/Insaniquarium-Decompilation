#include <SexyAppFramework/WidgetManager.h>

#include "Bilaterus.h"
#include "BilaterusHead.h"
#include "BilaterusBone.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Warp.h"
#include "Missle.h"
#include "Shadow.h"
#include "Coin.h"
#include "Res.h"

using namespace Sexy;

Sexy::Bilaterus::Bilaterus()
{
	mType = TYPE_BILATERUS;
	mActiveHead = nullptr;
	mPassiveHead = nullptr;
	mBoneList = new BoneList();
}

Bilaterus::Bilaterus(int theX, int theY)
{
	mType = TYPE_BILATERUS;
	m0x168 = 8;

	mActiveHead = new BilaterusHead(this, theX, theY, true);
	mApp->mWidgetManager->AddWidget(mActiveHead);
	mPassiveHead = new BilaterusHead(this, theX, theY, false);
	mApp->mWidgetManager->AddWidget(mPassiveHead);

	mBoneList = new BoneList();

	for (int i = 0; i < 6; i++)
	{
		BilaterusBone* aBone = new BilaterusBone(this, theX, theY, i);
		mApp->mWidgetManager->AddWidget(aBone);
		mBoneList->push_back(aBone);
	}

	m0x164 = false;
	mMouseVisible = false;
	m0x170 = 0;
	m0x16c = 15;

	if ((mApp->mSeed->Next() & 1) == 0)
		ChangeHead();
	else
		mApp->mBoard->PlaySample(SOUND_RATTLE_ID, 3, 1.0);
}

Bilaterus::~Bilaterus()
{
	for (BoneList::iterator anItr = mBoneList->begin(); anItr != mBoneList->end(); ++anItr)
		delete *anItr;
	delete mBoneList;
	delete mActiveHead;
	delete mPassiveHead;
}

void Sexy::Bilaterus::MarkDirty()
{
	GameObject::MarkDirty();
	if (mActiveHead)
		mActiveHead->MarkDirty();

	for (int i = 0; i < 6; i++)
		(*mBoneList)[i]->MarkDirty();

	if (mPassiveHead)
		mPassiveHead->MarkDirty();
}

void Sexy::Bilaterus::RemovedFromManager(WidgetManager* theWidgetManager)
{
	GameObject::RemovedFromManager(theWidgetManager);
	if (mActiveHead)
		theWidgetManager->RemoveWidget(mActiveHead);

	for (int i = 0; i < 6; i++)
		theWidgetManager->RemoveWidget((*mBoneList)[i]);

	if (mPassiveHead)
		theWidgetManager->RemoveWidget(mPassiveHead);
}

void Bilaterus::Update()
{
	if (mApp->mBoard != nullptr && mApp->mBoard->mPause)
		return;

	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK && mUpdateCnt > 4320)
	{
		// The original tests mActiveHead again on the one-head path (0x4FE366)
		if (mActiveHead != nullptr && mPassiveHead != nullptr)
		{
			mApp->mBoard->SpawnLaserShot(mActiveHead->mWidth / 2 + mActiveHead->mX - 40,
				mActiveHead->mHeight / 2 + mActiveHead->mY - 40);
			mApp->mBoard->SpawnLaserShot(mPassiveHead->mWidth / 2 + mPassiveHead->mX - 40,
				mPassiveHead->mHeight / 2 + mPassiveHead->mY - 40);
			// Midway between the two heads' centres
			mApp->mBoard->SpawnLaserShot((mActiveHead->mX + mPassiveHead->mX + mActiveHead->mWidth / 2 + mPassiveHead->mWidth / 2) / 2 - 40,
				(mActiveHead->mY + mPassiveHead->mY + mActiveHead->mHeight / 2 + mPassiveHead->mHeight / 2) / 2 - 40);
		}
		else if (mActiveHead != nullptr)
		{
			mApp->mBoard->SpawnLaserShot(mActiveHead->mWidth / 2 + mActiveHead->mX - 40,
				mActiveHead->mHeight / 2 + mActiveHead->mY - 40);
		}
		mApp->mBoard->PlaySample(SOUND_SFX_ID, 3, 1.0);
		RemoveHelper02(true);
		return;
	}

	UpdateCounters();
	if (m0x16c != 0)
		m0x16c--;
	else
	{
		m0x170++;
		if (!m0x164 && m0x170 >= 1000)
		{
			ChangeHead();
			m0x170 = 0;
		}

		if (mActiveHead)
			mActiveHead->UpdateHead();

		for (int i = 0; i < 6; i++)
			(*mBoneList)[i]->UpdateBone();

		if (mPassiveHead)
			mPassiveHead->UpdateHead();

		if (mActiveHead && mActiveHead->m0x1d8 <= 0.0)
			mActiveHead->Die(true);
	}
}

void Sexy::Bilaterus::OrderInManagerChanged()
{
	GameObject::OrderInManagerChanged();
	if (mPassiveHead)
		mWidgetManager->BringToFront(mPassiveHead);

	for(int i = 5; i >= 0; i--)
		mWidgetManager->BringToFront((*mBoneList)[i]);

	mWidgetManager->BringToFront(mActiveHead);
}

void Sexy::Bilaterus::CountRequiredFood(int* theFoodReqPtr)
{
	theFoodReqPtr[0]++;
	theFoodReqPtr[3]++;
	theFoodReqPtr[4]++;
}

void Sexy::Bilaterus::VFT74()
{
	mApp->mBoard->Unk12();
}

void Sexy::Bilaterus::Remove()
{
	Remove(true);
}

void Sexy::Bilaterus::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncBool(m0x164);
	theSync->SyncLong(m0x168);
	theSync->SyncLong(m0x16c);
	theSync->SyncLong(m0x170);
	DataReader* aReader = theSync->mReader;
	DataWriter* aWriter = theSync->mWriter;
	if (aReader != nullptr)
	{
		mBoneList = new BoneList();
		if (aReader->ReadBool())
		{
			mActiveHead = new BilaterusHead(this, mX, mY, true);
			mActiveHead->Sync(theSync);
			mApp->mWidgetManager->AddWidget(mActiveHead);
		}
		if (aReader->ReadBool())
		{
			mPassiveHead = new BilaterusHead(this, mX, mY, false);
			mPassiveHead->Sync(theSync);
			mApp->mWidgetManager->AddWidget(mPassiveHead);
		}

		int aNumOfBones = aReader->ReadLong();
		for (int i = 0; i < aNumOfBones; i++)
		{
			mBoneList->push_back(new BilaterusBone(this, mX, mY, i));
			mBoneList->back()->Sync(theSync);
			mApp->mWidgetManager->AddWidget(mBoneList->back());
		}
	}
	else
	{
		aWriter->WriteBool((mActiveHead != nullptr));
		if (mActiveHead)
			mActiveHead->Sync(theSync);
		aWriter->WriteBool((mPassiveHead != nullptr));
		if (mPassiveHead)
			mPassiveHead->Sync(theSync);

		aWriter->WriteLong(mBoneList->size());
		for (int i = 0; i < (int)mBoneList->size(); i++)
			mBoneList->at(i)->Sync(theSync);
	}
}

void Sexy::Bilaterus::ChangeHead()
{
	BilaterusHead* aPrevActive = mActiveHead;
	mActiveHead = mPassiveHead;
	mPassiveHead = aPrevActive;

	if (mActiveHead == nullptr || mPassiveHead == nullptr)
		return;

	mActiveHead->mHeadActive = true;
	mPassiveHead->mHeadActive = false;
	mActiveHead->m0x1cc = -mActiveHead->m0x1cc;
	mPassiveHead->m0x1cc = -mPassiveHead->m0x1cc;
	mActiveHead->mVX = 0;
	mPassiveHead->mVX = 0;

	BoneList* aBoneList = new BoneList();

	int j = 5;
	for (int i = 0; i <= 5; i++)
	{
		(*mBoneList)[j]->mBoneId = i;
		aBoneList->push_back((*mBoneList)[j]);
		j--;
	}

	mBoneList->clear();
	delete mBoneList;

	mBoneList = aBoneList;
	mActiveHead->BHUnk01();
	for (int i = 0; i < 5; i++)
	{
		(*mBoneList)[i]->UpdateFollowPosAndVel();
		(*mBoneList)[i]->m0x1b8 = -(*mBoneList)[i]->m0x1b8;
		(*mBoneList)[i]->m0x170 = 0.0;
	}
	mPassiveHead->BHUnk01();
	mApp->mBoard->PlaySample(SOUND_RATTLE_ID, 3, 1.0);
}

bool Sexy::Bilaterus::Shoot(int theX, int theY)
{
	bool isShot = false;
	if (mActiveHead != nullptr && mActiveHead->TakeHit(theX, theY))
	{
		isShot = true;
		mApp->mBoard->SpawnShot(theX - 40, theY - 40, 2);
	}
	return isShot;
}

void Sexy::Bilaterus::SpawnWarp()
{
	Warp* aWarp = new Warp(mActiveHead->mX - 10, mActiveHead->mY - 70);
	mApp->mBoard->AddGameObject(aWarp, true);
	mApp->mBoard->mWidgetManager->AddWidget(aWarp);
	mApp->mBoard->mWidgetManager->BringToFront(aWarp);
}

void Sexy::Bilaterus::Remove(bool flag)
{
	if (mMisslePtr)
		mMisslePtr->RemoveMissle();

	if (mApp->mBoard->mCyraxPtr == nullptr)
	{
		if (flag && CanDropCoin())
			mApp->mBoard->DropCoin(mX + 25, mY + 25, CoinTypes::COIN_DIAMOND, nullptr, -1.0, 0);
	}

	if (flag)
	{
		for (int i = 0; i < 6; i++)
			mApp->mBoard->SpawnMissle((*mBoneList)[i]->mX, (*mBoneList)[i]->mY, nullptr, Missle::MISSLE_BONE);
		int aType = 5;
		if (mActiveHead->m0x160 == 0)
			aType = 4;
		mApp->mBoard->SpawnMissle(mActiveHead->mX, mActiveHead->mY, nullptr, aType);
	}

	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mShadowPtr)
		mShadowPtr->RemoveShadow();

	mApp->mBoard->Unk12();
	if (flag)
	{
		// Draw order as the original: a shot's type first, then y, then x; a bubble's y before its x
		int aType, aX, aY;
		for (int i = (mApp->mSeed->Next() & 1) + 2; i > 0; i--)
		{
			aY = mApp->mSeed->Next() % 40 + 10 + mY;
			aX = mApp->mSeed->Next() % 40 + 10 + mX;
			mApp->mBoard->SpawnBubble(aX, aY);
			aY = mApp->mSeed->Next() % 20 + 20 + mY;
			aX = mApp->mSeed->Next() % 20 + 20 + mX;
			mApp->mBoard->SpawnBubble(aX, aY);
		}

		for (int i = (mApp->mSeed->Next() % 3) + 4; i > 0; i--)
		{
			aType = mApp->mSeed->Next() % 3 + 3;
			aY = mApp->mSeed->Next() % 40 + 10 + mY;
			aX = mApp->mSeed->Next() % 40 + 10 + mX;
			mApp->mBoard->SpawnShot(aX, aY, aType);
			aType = mApp->mSeed->Next() % 3 + 3;
			aY = mApp->mSeed->Next() % 20 + 20 + mY;
			aX = mApp->mSeed->Next() % 20 + 20 + mX;
			mApp->mBoard->SpawnShot(aX, aY, aType);
		}

		mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
		mApp->mBoard->PlaySample(SOUND_EXPLOSION1_ID, 3, 1.0);
	}
}
