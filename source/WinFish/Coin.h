#ifndef __COIN_H__
#define __COIN_H__

#include "GameObject.h"

namespace Sexy
{
	class OtherTypePet;

	enum CoinTypes
	{
		COIN_UNKNOWN,
		COIN_SILVER_C,
		COIN_GOLD_C,
		COIN_STAR,
		COIN_DIAMOND,
		COIN_DIAMOND_PENTA,
		COIN_PEARL,
		COIN_TREASURE,
		COIN_END = COIN_TREASURE,
		SHELL_SILVER,
		SHELL_START = SHELL_SILVER,
		SHELL_GOLD,
		SHELL_STAR,
		SHELL_BLUE,
		SHELL_BLUE_PENTA,
		SHELL_SPIRAL,
		SHELL_SACK,
		SHELL_END = SHELL_SACK,
		COIN_NIKOPEARL,
		COIN_SPECIAL = COIN_NIKOPEARL,
		COIN_NOTE,
		COIN_SHRAPNEL_BOMB,
		COIN_PEANUT
	};

	class Coin : public GameObject
	{
	public:
		double			mXD;
		double			mYD;
		bool			m0x168;
		int				mAnimationTimer;
		int				m0x170;
		int				mAnimationFrame;
		int				mDisappearTimer;
		int				m0x17c;
		int				m0x180;
		int				m0x184;
		double			mVY;
		OtherTypePet*	m0x190;
		int				mCoinType;
		bool			m0x198;
		int				m0x19c;
		int				m0x1a0;
		SexyString		m0x1a4;
		int				mComboCount;

	public:
		Coin();
		Coin(int theX, int theY, int theType, OtherTypePet* unk, double theVel);

		virtual void			Update();
		virtual void			Draw(Graphics* g);
		virtual void			MouseDown(int x, int y, int theClickCount);

		virtual void			Remove();										//[75] forwards to RemoveCoin (a jmp in the original)
		void					RemoveCoin();									// 0x4D5510; called directly
		virtual void			Sync(DataSync* theSync);						//[80]

		bool					IsShell();
		int						GetValue();
		void					ReceiveMoney();
		void					PetCollected();
		void					Collect();
		void					StartDisappearing();
	};
}

#endif