#ifndef __SHOT_H__
#define __SHOT_H__

#include "GameObject.h"

namespace Sexy
{
	class Shot : public GameObject
	{
	public:
		int m0x158;
		int m0x15c;
		int mShotType;
		int m0x164;
		int m0x168;

	public:
		Shot();
		Shot(int theX, int theY);
		Shot(int theX, int theY, int theType);

		virtual void			Update();
		virtual void			Draw(Graphics* g);

		virtual void			Remove();										//[75] forwards to RemoveShot (a jmp in the original)
		void					RemoveShot();									// 0x4D9570 (identical-code-folded with Food/Larva); called directly
		virtual void			Sync(DataSync* theSync);						//[80]

		void					SetRespectiveColor(Graphics* g, int theAlpha);
		void					SetRespectiveCoreColor(Graphics* g, int theAlpha);
	};
}

#endif