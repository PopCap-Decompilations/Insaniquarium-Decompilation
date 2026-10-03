#ifndef __INTERLUDESCREEN_H__
#define __INTERLUDESCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

class InterludeScreenOverlay;	// in the global namespace, as in the original (RTTI .?AVInterludeScreenOverlay@@)

namespace Sexy
{
	class WinFishApp;
	class BubbleMgr;
	class HyperlinkWidget;

	extern bool gInterludeBool01;
	extern int gInterludeInt02;
	extern int gInterludeInt03;
	extern int gInterludeArray01[15];

	class InterludeScreen : public Widget, public ButtonListener
	{
	public:
		WinFishApp*				mApp;
		HyperlinkWidget*		mBackButton;
		InterludeScreenOverlay* mOverlay;
		BubbleMgr*				mBubbleMgr1;
		BubbleMgr*				mBubbleMgr2;
		int						m0xa0;
		int						m0xa4;
		int						m0xa8;
		int						m0xac;
		int						m0xb0;
		float					m0xb4;

	public:
		InterludeScreen(WinFishApp* theApp, int unk);
		virtual ~InterludeScreen();

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
		virtual void			Update();
		virtual void			Draw(Graphics* g);
		virtual void			DrawOverlay(Graphics* g);
		virtual void			MouseDown(int x, int y, int theClickCount);

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);

		void					InitDeadPetSlots();
		void					DrawStringHelper(Graphics* g, SexyString theString, int unk1, int unk2, int unk3);
		static void				DrawAngieHalo(Graphics* g, int theX, int theY, float theAlpha, int theFrame, bool mirror);
		void					DrawPetAngieSpawns(Graphics* g, int unk1, int thePetId, int theX, int theY, int unk5, int unk6, int unk7);
	};
}

class InterludeScreenOverlay : public Sexy::Widget
{
public:
	Sexy::InterludeScreen* mScreen;

public:
	// Inlined into InterludeScreen::InterludeScreen (0x5256E0) in the original
	InterludeScreenOverlay(Sexy::InterludeScreen* theScreen)
	{
		mScreen = theScreen;
		mHasAlpha = true;
		mMouseVisible = false;
	}

	virtual void			Draw(Sexy::Graphics* g);
};

#endif