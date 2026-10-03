#include "BoxingGlove.h"
#include "WinFishApp.h"
#include "Board.h"
#include "FishSongMgr.h"
#include "FishTypePet.h"
#include "Res.h"

Sexy::BoxingGlove::BoxingGlove(FishTypePet* theWalter, bool right)
{
	mWalter = theWalter;
	mX = -200;
	mY = -200;
	mMouseVisible = false;
	m0x164 = 0;
	mWidth = 160;
	mHeight = 80;
	m0x15c = 30;
	m0x160 = right;
}

void Sexy::BoxingGlove::Update()
{
	if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
		return;

	if (mWalter != nullptr)
	{
		if (mWalter->mEatingAnimationTimer > 35)
			return;
		if (!m0x160)
			mX = mWalter->mX - 60;
		else
			mX = mWalter->mX - 20;
		mY = mWalter->mY;
	}

	m0x15c--;
	if (m0x15c > 0)
	{
		if (m0x164 > 0)
			m0x164--;

		bool hit = false;
		if (m0x15c > 12)
		{
			int anXHitPos = 60;
			if (m0x15c > 20)
				anXHitPos = 30;
			if (m0x15c > 25)
				anXHitPos = 0;
			if (m0x160)
				anXHitPos = mX + anXHitPos + 80;
			else
				anXHitPos = mX - anXHitPos + 80;
			int anYHitPos = mY + 40;
			int aDir = m0x160 * 2 - 1; // computed once, before the loop, as the original (0x4E9B5A)

			// The board is read again here and the set's address kept for the loop (0x4E9B3B)
			Board* aBoard = mApp->mBoard;
			for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
			{
				GameObject* anObj = *it;

				if (anObj != mWalter && anObj->mShown)
				{
					switch (anObj->mType)
					{
					case TYPE_GUPPY:
					case TYPE_OSCAR:
					case TYPE_ULTRA:
					case TYPE_GEKKO:
					case TYPE_SYLVESTER_FISH:
					case TYPE_BALL_FISH:
					case TYPE_BI_FISH:
						if (HandleFishHit(anObj, anXHitPos, anYHitPos, aDir, 5))
							hit = true;
						break;
					case TYPE_FISH_TYPE_PET:
					{
						FishTypePet* aPet = (FishTypePet*)anObj;
						if (aPet->mFishTypePetType != PET_PREGO)
						{
							int aVal = -1;
							switch (aPet->mFishTypePetType)
							{
							case PET_VERT:
							case PET_SHRAPNEL:
							case PET_AMP:
								aVal = 5;
								break;
							case PET_NOSTRADAMUS:
								aVal = 10;
							}
							if (HandleFishHit(anObj, anXHitPos, anYHitPos, aDir, aVal))
								hit = true;
						}
						break;
					}
					}
				}
			}

			if (hit && m0x164 == 0)
			{
				mApp->mBoard->PlayPunchSound(3);
				m0x164 = 5;
			}
		}
	}
}

void Sexy::BoxingGlove::Draw(Graphics* g)
{
	// The original takes the absolute value only at or below 15 (0x4E100E)
	int aSrcCol;
	if (m0x15c > 15)
		aSrcCol = m0x15c - 15;
	else
		aSrcCol = abs(m0x15c - 15);

	// The original tests the upper bound first (0x4E1030)
	int aFrame = 9 - aSrcCol;
	if (aFrame <= 5 && aFrame > 0)
		aFrame = 6;
	if (aFrame > 0)
	{
		Rect aSrcRect = Rect(aFrame * 80, 160, 80, 80);
		if (!m0x160)
			g->DrawImageMirror(IMAGE_WALTER, -3, 2, aSrcRect, false);
		else
			g->DrawImageMirror(IMAGE_WALTER, 83, 2, aSrcRect, true);
	}

	if (!m0x160)
		g->DrawImageMirror(IMAGE_WALTER, aSrcCol * 6 - 20, 0, Rect(400, 160, 80, 80), false);
	else
		g->DrawImageMirror(IMAGE_WALTER, 100 - aSrcCol * 6, 0, Rect(400, 160, 80, 80), true);
}

bool Sexy::BoxingGlove::HandleFishHit(GameObject* theObj, int x, int y, int dir, int unk)
{
	Fish* aFish = (Fish*)theObj;

	if (aFish->mWadsworthVXModCounter <= 0 &&
		(aFish->mType != TYPE_FISH_TYPE_PET || !((FishTypePet*)theObj)->m0x230 || aFish->mUpdateCnt >= 30))
	{
		int aCX = aFish->mWidth / 2 + aFish->mX;
		int aCY = aFish->mHeight / 2 + aFish->mY;

		if (x - 40 < aCX && x + 40 > aCX && y - 40 < aCY && y + 40 > aCY)
		{
			if (unk > 0)
				aFish->mCoinDropTimer = aFish->mCoinDropT - unk;
			aFish->mWadsworthVXModCounter = 50;
			aFish->mUnusedTimer = 180;
			// The original reaches the board through the fish; the glove is not passed (0x4E0F82)
			if (aFish->mSongId != -1)
				aFish->mApp->mBoard->mFishSongMgr->StopFishSong(aFish->mSongId);
			aFish->ShowInvisibility();
			if (Rand() % 2 == 0)
				aFish->mUnusedVXWadsworthAddon = dir * 15;
			else
				aFish->mUnusedVXWadsworthAddon = dir * 10;
			return true;
		}
	}
	return false;
}