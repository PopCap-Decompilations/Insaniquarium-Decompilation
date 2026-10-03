#ifndef __HATCHSCREEN_H__
#define __HATCHSCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

class HatchScreenOverlay;		// in the global namespace, as in the original (RTTI .?AVHatchScreenOverlay@@)

namespace Sexy
{
	class WinFishApp;
	class BubbleMgr;

	class HatchScreen : public Widget, public ButtonListener
	{
	public:
		WinFishApp* mApp;
		BubbleMgr* mBubbleMgr;
		int m0x94;
		int mPetId;
		int m0x9c;
		int m0xa0;
		int m0xa4;
		int m0xa8; // unused
		DialogButton* mContinueButton;
		DialogButton* mMenuButton;
		HatchScreenOverlay* mOverlay;

	public:
		HatchScreen(WinFishApp* theApp, int thePetId);
		virtual ~HatchScreen();

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);

		virtual void			Update();
		virtual void			Draw(Graphics* g);
		virtual void			DrawOverlay(Graphics* g);

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);
	};
}

class HatchScreenOverlay : public Sexy::Widget
{
public:
	Sexy::HatchScreen* mScreen;

public:
	// Inlined into HatchScreen::HatchScreen (0x51F450) in the original
	HatchScreenOverlay(Sexy::HatchScreen* theScreen)
	{
		mScreen = theScreen;
		mMouseVisible = false;
		mHasAlpha = true;
		mWidth = 640;
		mHeight = 480;
	}

	virtual void			Draw(Sexy::Graphics* g);
};

#endif