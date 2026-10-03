#ifndef __LARVA_H__
#define __LARVA_H__

#include "GameObject.h"

namespace Sexy
{
	class Larva : public GameObject
	{
	public:
		double mXD;
		double mYD;
		double mVY;
		int mAnimFrame;
		bool mPickedUp;
		bool m0x175;
		
	public:
		Larva();
		Larva(int theX, int theY);

		virtual void			Update();
		virtual void			Draw(Graphics* g);
		virtual void			MouseDown(int x, int y, int theClickCount);

		virtual void			Remove();										//[75] forwards to RemoveLarva (a jmp in the original)
		void					RemoveLarva();									// 0x4D9570 (identical-code-folded with Food/Shot); called directly
		virtual void			Sync(DataSync* theSync);						//[80]

		int						GetValue();
		void					UpdateAnimations();								// 0x4D7E80; tail-called by Update
	};
}

#endif