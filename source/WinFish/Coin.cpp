#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>

#include "Coin.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Fish.h"
#include "Oscar.h"
#include "Ultra.h"
#include "Gekko.h"
#include "Penta.h"
#include "Grubber.h"
#include "Breeder.h"
#include "OtherTypePet.h"
#include "Alien.h"
#include "Bilaterus.h"
#include "BilaterusHead.h"
#include "Missle.h"
#include "Res.h"

using namespace Sexy;

Sexy::Coin::Coin()
{
	mClip = false;
	m0x190 = nullptr;
	mType = TYPE_COIN;
}

Sexy::Coin::Coin(int theX, int theY, int theType, OtherTypePet* unk, double theVel)
{
	mXD = theX;
	mType = TYPE_COIN;
	mClip = false;
	m0x1a0 = 0;
	mYD = theY;
	m0x17c = 0;
	mComboCount = 1;
	m0x190 = unk;
	mCoinType = theType;
	mX = mXD;
	mY = mYD;
	mWidth = 72;
	mHeight = 72;
	if (theType == COIN_NOTE)
	{
		mMouseVisible = false;
		mDoFinger = false;
	}
	else
		mDoFinger = true;

	m0x19c = 0;
	mVY = 1.5;
	m0x168 = false;
	m0x198 = false;
	mAnimationTimer = 0;
	m0x170 = 0;
	mDisappearTimer = 0;
	mAnimationFrame = 0;
	if (theVel != -1.0)
		mVY = theVel;
}

void Sexy::Coin::Update()
{
	if (!mApp->mBoard || mApp->mBoard->mPause)
		return;

	UpdateCounters();
	mAnimationTimer++;
	m0x170++;
	if (mAnimationTimer > 79)
		mAnimationTimer = 0;
	if (mCoinType == COIN_SHRAPNEL_BOMB)
	{
		mAnimationFrame = (mAnimationTimer / 2) % 5;
	}
	else if (IsShell() &&
		(mCoinType == COIN_SILVER_C || mCoinType == SHELL_SILVER || mCoinType == COIN_GOLD_C || mCoinType == SHELL_GOLD ||
		 mCoinType == COIN_DIAMOND_PENTA || mCoinType == SHELL_BLUE_PENTA || mCoinType == COIN_DIAMOND || mCoinType == SHELL_BLUE ||
		 mCoinType == COIN_PEARL || mCoinType == SHELL_SPIRAL || mCoinType == COIN_TREASURE || mCoinType == SHELL_SACK))
	{
		mAnimationFrame = (mAnimationTimer / ((mApp->mBoard->mPetsInTank[10] != 0) * 2 + 2)) % 20;
	}
	else
	{
		int aVal = mAnimationTimer;
		if (mApp->mBoard->mPetsInTank[10] == 0)
			aVal /= 2;
		else
			aVal /= 4;

		mAnimationFrame = aVal % 10;
	}

	if (mDisappearTimer != 0)
	{
		if (!m0x198)
		{
			mDisappearTimer--;
			if (mDisappearTimer != 0)
				return;
			RemoveCoin();
			return;
		}
		mDisappearTimer = 0;
	}

	if (!m0x198)
	{
		if (mCoinType == COIN_NIKOPEARL) // 87
		{
			m0x19c++;
			if (m0x19c > 216)
				RemoveCoin();
			return;
		}


		if (mCoinType == COIN_NOTE) // 96
		{
			m0x19c++;
			int aVal = m0x1a0 != 0 ? -50 : 50;
			if ((m0x1a0 >= 3 && m0x19c > 50) || m0x19c > 100 || aVal > mYD)
			{
				RemoveCoin();
				return;
			}
			mYD -= 1.0;
			Move(mXD, mYD);
			return;
		} // 111

		if ((mCoinType != COIN_PEANUT && mCoinType != COIN_DIAMOND_PENTA && (mCoinType < SHELL_SILVER || mCoinType > SHELL_SACK)) || m0x168)
		{
			if (mApp->mBoard->mPetsInTank[10] != 0)
				mYD = mYD + 0.8;
			else if (mApp->mBoard->mIsBonusRound)
				mYD = mVY + mYD;
			else
				mYD = mYD + 1.5;

			if (mYD >= 370.0)
			{
				mYD = 370.0;
				if (mApp->mBoard->mIsBonusRound)
					StartDisappearing();
				m0x19c++;

				int unkval;
				if (mApp->mBoard->mPetsInTank[10] != 0)
					unkval = 200;
				else if (mApp->mRelaxMode)
					unkval = 108;
				else
					unkval = mApp->mBoard->IsFirstLevel() ? 150 : 20;
				if (m0x19c >= unkval)
				{
					if (mCoinType == COIN_SHRAPNEL_BOMB)
					{
						RemoveCoin();
						return;
					}
					StartDisappearing();
				}
			} // 157

			Move(mXD, mYD);
			if (mCoinType == COIN_SHRAPNEL_BOMB && m0x170 > 30)
			{
				// Iterator loops in the original, dereferencing the iterator at every access
				for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); it++)
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Oscar*>::iterator it = mApp->mBoard->mOscarList->begin(); it != mApp->mBoard->mOscarList->end(); it++)
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Ultra*>::iterator it = mApp->mBoard->mUltraList->begin(); it != mApp->mBoard->mUltraList->end(); it++)
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 150 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 150 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Gekko*>::iterator it = mApp->mBoard->mGekkoList->begin(); it != mApp->mBoard->mGekkoList->end(); it++)
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Penta*>::iterator it = mApp->mBoard->mPentaList->begin(); it != mApp->mBoard->mPentaList->end(); it++) // 444
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Grubber*>::iterator it = mApp->mBoard->mGrubberList->begin(); it != mApp->mBoard->mGrubberList->end(); it++) // 497
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}

				for (std::vector<Breeder*>::iterator it = mApp->mBoard->mBreederList->begin(); it != mApp->mBoard->mBreederList->end(); it++) // 497
				{
					if ((*it)->mVirtualTankId < 0)
					{
						if (mXD + 36.0 < (*it)->mX + 70 && mXD + 36.0 > (*it)->mX + 10
							&& mYD + 36.0 < (*it)->mY + 70 && mYD + 36.0 > (*it)->mY + 10)
						{
							(*it)->Die(true);
							RemoveCoin();
							int aShotType = mApp->mSeed->Next() % 3 + 3;
							mApp->mBoard->SpawnShot(mXD, mYD, aShotType);
							return;
						}
					}
				}
			}
			return;
		}

		if (mYD < 120.0)
			m0x168 = true; // 309
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
	}
	else // 332
	{
		if (mApp->mBoard->mIsBonusRound)
		{ // 358
			Point aTarget = mApp->mBoard->GetBonusShellTarget();
			m0x17c++;
			int aXDist = m0x180 - (aTarget.mX + 30);
			int aYDist = m0x184 - (aTarget.mY + 50);
			int aDist = aXDist * aXDist + aYDist * aYDist;
			int aVal = aDist <= 22500 ? 5 : 15;
			if (aVal <= m0x17c)
			{
				RemoveCoin();
				ReceiveMoney();
				return;
			}
			int aSomeDist = aVal - m0x17c;
			mXD = (aSomeDist * m0x180 + (aTarget.mX + 30) * m0x17c) / aVal;
			// 375 iVar3 = iVar3 % iVar9; ?
			mYD = (aSomeDist * m0x184 + (aTarget.mY + 50) * m0x17c) / aVal;
		}
		else
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
				RemoveCoin();
				ReceiveMoney();
				return;
			}
		}
	}

	Move(mXD, mYD);
}

void Sexy::Coin::Draw(Graphics* g)
{
	GameObject::UpdateFishSongMgr();

	int aVal = m0x19c % 8;
	if (aVal >= 8 && mCoinType != COIN_PEARL && mCoinType != COIN_TREASURE && !m0x198)
	{
		g->SetColorizeImages(false);
		return;
	}

	if (mDisappearTimer != 0)
	{
		g->SetColorizeImages(true);
		g->SetColor(Color(255, 255, 255, mDisappearTimer * 255 / 5));
	}

	// An if chain in the original, tested in this order (a switch would be a jump table)
	bool isShell = Coin::IsShell();
	if (mCoinType == COIN_SILVER_C || mCoinType == SHELL_SILVER)
	{
		if (isShell)
			g->DrawImageCel(IMAGE_SHELLS, 20, 20, mAnimationFrame, 0);
		else
			g->DrawImageCel(IMAGE_MONEY, 0, 0, mAnimationFrame, 0);
	}
	else if (mCoinType == COIN_GOLD_C || mCoinType == SHELL_GOLD)
	{
		if (isShell)
			g->DrawImageCel(IMAGE_SHELLS, 20, 20, mAnimationFrame, 1);
		else
			g->DrawImageCel(IMAGE_MONEY, 0, 0, mAnimationFrame, 1);
	}
	else if (mCoinType == COIN_DIAMOND_PENTA || mCoinType == SHELL_BLUE_PENTA || mCoinType == COIN_DIAMOND || mCoinType == SHELL_BLUE)
	{
		if (isShell)
			g->DrawImageCel(IMAGE_SHELLS, 20, 20, mAnimationFrame, 2);
		else
			g->DrawImageCel(IMAGE_MONEY, 0, 0, mAnimationFrame, 3);
	}
	else if (mCoinType == COIN_PEARL || mCoinType == SHELL_SPIRAL)
	{
		if (isShell)
			g->DrawImageCel(IMAGE_SHELLS, 20, 20, mAnimationFrame, 3);
		else
			g->DrawImage(IMAGE_PEARL, 0, 0);
	}
	else if (mCoinType == COIN_TREASURE || mCoinType == SHELL_SACK)
	{
		if (isShell)
			g->DrawImageCel(IMAGE_MONEYBAG, 18, 18, 0, 0);
		else
			g->DrawImageCel(IMAGE_MONEY, 0, 0, mAnimationFrame, 4);
	}
	else if (mCoinType == COIN_PEANUT)
	{
		g->DrawImage(IMAGE_PEANUT, 0, 0);
	}
	else if (mCoinType == COIN_STAR || mCoinType == SHELL_STAR)
	{
		g->DrawImageCel(IMAGE_MONEY, 0, 0, mAnimationFrame, 2);
	}
	else if (mCoinType == COIN_NIKOPEARL && m0x198)
	{
		g->DrawImage(IMAGE_PEARL, 0, 0);
	}
	else if (mCoinType == COIN_NOTE)
	{
		if (m0x1a0 <= 1)
		{
			g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 1);
		}
		else
		{
			if (m0x1a0 < 3)
			{
				g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
				g->SetColor(Color(0xffffff));
			}
			else
			{
				g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
				Color aCol;
				switch (m0x1a0)
				{
				case 3:
					aCol = Color(0xaaaaaa);
					break;
				case 4:
					aCol = Color(0xfddc41);
					break;
				case 5:
					aCol = Color(0x29e7ff);
					break;
				case 6:
					aCol = Color(0x88ff88);
					break;
				case 7:
					aCol = Color(0xffff00);
					break;
				default:
					aCol = Color(0xffff00);
				}
				if (m0x19c > 0)
				{
					int anAlpha = ((50 - m0x19c) * 255) / 50;
					if (anAlpha <= 0)
						return; // colorizing is left on, as the original (0x4DDA76)
					aCol.mAlpha = anAlpha;
				}
				g->SetColor(aCol);
			}
			g->DrawString(m0x1a4, 5, g->GetFont()->GetHeight());
		}
	}
	else if (mCoinType == COIN_SHRAPNEL_BOMB)
	{
		g->DrawImageCel(IMAGE_MISCITEMS, 0, 0, 0);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->DrawImageCel(IMAGE_SPARKS, 10, -10, mAnimationFrame);
		g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	}

	g->SetColorizeImages(false);
}

void Sexy::Coin::MouseDown(int x, int y, int theClickCount)
{
	if (theClickCount < 0)
	{
		mApp->mBoard->CheckMouseDown(mX + x, mY + y);
		return;
	}

	if (mCoinType == COIN_PEANUT && mUpdateCnt < 12)
		return;

	if (mCoinType == COIN_NOTE || m0x198)
		return;
	Board* aBoard = mApp->mBoard;
	if (aBoard->AliensInTank() || aBoard->MisslesInTank())
		aBoard->ShootEnemyUnderCoin(mX + x, mY + y);
	Collect();
}

void Sexy::Coin::Collect()
{
	m0x17c = 1;

	m0x180 = mXD;
	m0x184 = mYD;
	if (mCoinType == COIN_NIKOPEARL)
	{
		m0x198 = true;
		for (std::vector<OtherTypePet*>::iterator it = mApp->mBoard->mOtherTypePetList->begin(); it != mApp->mBoard->mOtherTypePetList->end(); ++it)
		{
			if (m0x190 != 0 && *it == m0x190)
				(*it)->m0x1c0 = true;
		}
		mApp->mBoard->PlaySample(SOUND_PEARL_ID, 3, 1.0);
	}

	mMouseVisible = false;

	if (mCoinType <= SHELL_END || mCoinType == COIN_SHRAPNEL_BOMB || mCoinType == COIN_PEANUT)
	{
		m0x198 = true;
		Board* aBoard = mApp->mBoard;
		if (!aBoard->mIsBonusRound || !aBoard->Unk09(this))
		{
			// An if chain in the original, tested in this order (a switch would be a jump table)
			if (mCoinType == COIN_DIAMOND || mCoinType == COIN_DIAMOND_PENTA || mCoinType == SHELL_BLUE || mCoinType == SHELL_BLUE_PENTA)
				mApp->mBoard->PlayDiamondSound();
			else if (mCoinType == COIN_PEANUT)
				mApp->mBoard->PlayChompSound(false);
			else if (mCoinType == COIN_PEARL || mCoinType == SHELL_SPIRAL)
				mApp->mBoard->PlaySample(SOUND_PEARL_ID, 3, 1.0);
			else if (mCoinType == COIN_TREASURE || mCoinType == SHELL_SACK)
				mApp->mBoard->PlaySample(IsShell() ? SOUND_BONUSCOLLECT_ID : SOUND_TREASURE_ID, 3, 1.0);
			else
				mApp->mBoard->PlayPointsSound();
		}
	}
}

void Sexy::Coin::StartDisappearing()
{
	mDisappearTimer = 5;
}

void Sexy::Coin::Remove()
{
	RemoveCoin();
}

void Sexy::Coin::RemoveCoin()
{
	mApp->mBoard->mWidgetManager->RemoveWidget(this);
	mApp->SafeDeleteWidget(this);
	mApp->mBoard->RemoveGameObjectFromLists(this, false);
	if (mCoinType == COIN_SHRAPNEL_BOMB && !m0x198)
	{
		mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
		int aNumOfSmokes = mApp->mSeed->Next() % 3 + 2;
		for (int i = 0; i < aNumOfSmokes; i++)
		{
			int aType = mApp->mSeed->Next() % 3 + 3;
			int aRandY = (mApp->mSeed->Next() % 30 - 10) + mY;
			int aRandX = (mApp->mSeed->Next() % 30 - 10) + mX;
			mApp->mBoard->SpawnShot(aRandX, aRandY, aType);
		}
	}
}

void Sexy::Coin::Sync(DataSync* theSync)
{
	GameObject::Sync(theSync);
	theSync->SyncDouble(mXD);
	theSync->SyncDouble(mYD);
	theSync->SyncBool(m0x168);
	theSync->SyncLong(mAnimationTimer);
	theSync->SyncLong(m0x170);
	theSync->SyncLong(mAnimationFrame);
	theSync->SyncLong(mDisappearTimer);
	theSync->SyncLong(m0x17c);
	if (m0x17c > 0)
	{
		theSync->SyncLong(m0x180);
		theSync->SyncLong(m0x184);
	}
	theSync->SyncDouble(mVY);
	theSync->SyncLong(mCoinType);
	theSync->SyncBool(m0x198);
	theSync->SyncLong(m0x19c);
	theSync->SyncLong(mComboCount);
	theSync->SyncLong(m0x1a0);
	if (m0x1a0 > 1)
		theSync->SyncString(m0x1a4);
	theSync->SyncPointer((void**) &m0x190);
}

bool Sexy::Coin::IsShell()
{
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK || mApp->mBoard->mIsBonusRound)
		return true;
	return false;
}

int Sexy::Coin::GetValue()
{
	if (mCoinType == COIN_SILVER_C || mCoinType == SHELL_SILVER)
	{
		if (IsShell())
			return mComboCount;
		return 15;
	}
	if (mCoinType == COIN_GOLD_C || mCoinType == SHELL_GOLD)
	{
		if (IsShell())
			return mComboCount * 2;
		return 35;
	}
	if (mCoinType == COIN_STAR || mCoinType == SHELL_STAR)
	{
		if (IsShell())
			return mComboCount * 3;
		return 40;
	}
	if (mCoinType == COIN_DIAMOND || mCoinType == SHELL_BLUE || mCoinType == COIN_DIAMOND_PENTA || mCoinType == SHELL_BLUE_PENTA)
	{
		if (IsShell())
			return mComboCount * 5;
		return 200;
	}
	if (mCoinType == COIN_PEARL || mCoinType == SHELL_SPIRAL)
	{
		if (IsShell())
			return mComboCount * 10;
		return 500;
	}
	if (mCoinType == COIN_TREASURE || mCoinType == SHELL_SACK)
	{
		if (IsShell())
			return mComboCount * 20;
		return 2000;
	}
	if (mCoinType == COIN_SHRAPNEL_BOMB)
	{
		if (IsShell())
			return mComboCount * 5;
		return 150;
	}
	if (mCoinType == COIN_NIKOPEARL)
	{
		if (IsShell())
			return mComboCount * 10;
		return 250;
	}
	if (mCoinType == COIN_PEANUT)
	{
		if (IsShell())
			return mComboCount;
		return 15;
	}
	return 0;
}

void Sexy::Coin::ReceiveMoney()
{
	mApp->mBoard->mShouldSave = true;
	mApp->mBoard->Unk07(GetValue());
}

void Sexy::Coin::PetCollected()
{
	ReceiveMoney();
	if (mCoinType == COIN_DIAMOND || mCoinType == COIN_DIAMOND_PENTA || mCoinType == SHELL_BLUE || mCoinType == SHELL_BLUE_PENTA)
		mApp->mBoard->PlayDiamondSound();
	else if (mCoinType == COIN_PEARL || mCoinType == SHELL_SPIRAL)
		mApp->mBoard->PlaySample(SOUND_PEARL_ID, 3, 1.0);
	else if (mCoinType == COIN_TREASURE || mCoinType == SHELL_SACK)
		mApp->mBoard->PlaySample(IsShell() ? SOUND_BONUSCOLLECT_ID : SOUND_TREASURE_ID, 3, 1.0);
	else
		mApp->mBoard->PlayPointsSound();
}
