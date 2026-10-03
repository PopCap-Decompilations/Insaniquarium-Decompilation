#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "GameObject.h"

namespace Sexy
{
	class Shadow : public GameObject
	{
	public:
		GameObject* mObjectPtr;
		int mShadowSize;
		int m0x160;
		double m0x168;

	public:
		Shadow();
		Shadow(int theSize, GameObject* theObject);

		virtual void			Update();
		virtual void			Draw(Graphics* g);

		virtual void			VFT74();										//[74]
		virtual void			Remove();										//[75] forwards to RemoveShadow (a jmp in the original)
		void					RemoveShadow();									// 0x4D9080; called directly
		virtual void			Sync(DataSync* theSync);						//[80]
	};

}

#endif