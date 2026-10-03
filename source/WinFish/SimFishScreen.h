#ifndef __SIMFISHSCREEN_H__
#define __SIMFISHSCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

class SimFishScreenOverlay;		// in the global namespace, as in the original (RTTI .?AVSimFishScreenOverlay@@)

namespace Sexy
{
	class WinFishApp;
	class FishButtonWidget;
	class BubbleMgr;
	class MemoryImage;
	class DialogButton;
	class GameObject;

	class SimFishScreen : public Widget, public ButtonListener
	{
	public:
		WinFishApp* mApp;
		BubbleMgr* mBubbleMgr;
		DialogButton* mReturnButton;
		DialogButton* mMenuButton;
		DialogButton* mHideShowButton;
		DialogButton* mSellButton;
		DialogButton* mRenameButton;
		DialogButton* mHideAllButton;
		DialogButton* mShowAllButton;
		SimFishScreenOverlay* mOverlay;
		FishButtonWidget* mObjectButtons[20];
		FishButtonWidget* mSelectedFishButton;
		MemoryImage* mBackgroundImage;
		int m0x10c;
		int m0x110;
		int m0x114;
		bool m0x118;
		bool m0x119;
		bool m0x11a;

	public:
		SimFishScreen(WinFishApp* theApp);
		virtual ~SimFishScreen();

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
		virtual void			Update();
		virtual void			OrderInManagerChanged();
		virtual void			DrawOverlay(Graphics* g);

		virtual void			ButtonPress(int theId, int theClickCount);
		virtual void			ButtonDepress(int theId);
		virtual void			ButtonMouseEnter(int theId);
		virtual void			ButtonMouseLeave(int theId);

		void					CreateFishButton(int theButtonId,GameObject* theObject);
		void					DetermineShowHideForButtons();
		void					RenameSelectedFish(const SexyString& theName);
		void					SellSelectedObject(bool sell);
	};

	// Free (cdecl) functions in the original
	SexyString					GetAdditionalNotes(GameObject* theObject);
	const char*					GetLastSpecialLike(GameObject* theObject);
}

class SimFishScreenOverlay : public Sexy::Widget
{
public:
	Sexy::SimFishScreen* mScreen;

public:
	// Inlined into SimFishScreen::SimFishScreen (0x52F2C0) in the original
	SimFishScreenOverlay(Sexy::SimFishScreen* theScreen)
	{
		mScreen = theScreen;
		mMouseVisible = false;
		mHasAlpha = true;
		mWidth = 640;
		mHeight = 480;
	}

	virtual void			Draw(Sexy::Graphics* g);
};

// Sizes of the hometown and likes tables in SimFishScreen.cpp: const ints in .rdata (0x59BDC8, 0x59BDC4)
// that the original reads from memory at each use
extern const int NUM_HOMETOWNS;
extern const int NUM_LIKES;

#endif