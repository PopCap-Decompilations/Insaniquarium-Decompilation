#ifndef __STORESCREEN_H__
#define __STORESCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

class StoreScreenOverlay;		// in the global namespace, as in the original (RTTI .?AVStoreScreenOverlay@@)

namespace Sexy
{
	class WinFishApp;
	class StoreButtonWidget;
	class MyLabelWidget;
	class DialogButton;
	class GameObject;
	class Fish;

	class StoreScreen : public Widget, public ButtonListener
	{
	public:
		WinFishApp* mApp;
		int m0x90;
		StoreScreenOverlay* mOverlay;
		DialogButton* mBackButton;
		StoreButtonWidget* mStoreButtons[8];
		StoreButtonWidget* mStoreButtonLast;
		MyLabelWidget* mShellsLabel;
		char mShellsString[256];
		int mStoreButtonsX;
		int mStoreButtonsXGap;
		int mMerylBlinkTimer;
		MTRand mRand;
		long long mSeedBase;
		int mBoughtItemTimer;
		bool mSaveData;
		int mOverButtonId;
		int mStoreScreenUpdateCnt;

		enum ProductTypes
		{
			PRODUCT_SOLD,
			PRODUCT_FISH,
			PRODUCT_BUBBULATOR,
			PRODUCT_BACKDROP,
			PRODUCT_ALIEN_ATTRACTOR,
			PRODUCT_UPGRADE
		};

	public:
		StoreScreen(WinFishApp* theApp);
		virtual ~StoreScreen();

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
		virtual void			Update();
		virtual void			OrderInManagerChanged();
		virtual void			DrawOverlay(Graphics* g);
		virtual void			MouseEnter();
		virtual void			MouseMove(int theX, int theY);

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);

		GameObject*				ConfirmPurchase();
		// Members (thiscall, this unused) in the original: DrawOverlay and GetProductDescription pass this in ecx
		const char*				GetProductDescription(StoreButtonWidget* theButton);
		const char*				GetFishDescription(GameObject* theObject);
		void					InitializeStoreButtons(int theSeed);
		void					CommonFishStoreSlots();
		void					ColorfulFishStoreSlots();
		void					SpecialFishStoreSlots();
		void					ItemsStoreSlots();
		bool					IsSpecialFishInTank(int theSpecialFishId);
		void					InitFishColors(Fish* theFish, bool randomColors);
		GameObject*				GetRandomSpecialFish(bool possibleClassicFish, int theSkipType, int unk2);
		Fish*					RandomGuppyOrOscar();
		static bool				IsSpecialFishAllowed(int unk, int theType);
		int						Unk01(bool deterministic);
		GameObject*				GetSpecial8Fish();
		Fish*					MakeSpecialFish(int theId);
		void					SetUpSpecialFish(GameObject* theObject, bool deterministic);
		void					SpecialId5SetUp(GameObject* theObject);
		void					SpecialId6SetUp(GameObject* theObject);
		void					SpecialId78SetUp(GameObject* theObject);
		static bool				IsSpecialFishValid(GameObject* theObject);
	};
}

class StoreScreenOverlay : public Sexy::Widget
{
public:
	Sexy::StoreScreen* mScreen;

public:
	// Inlined into StoreScreen::StoreScreen (0x52B2D0) in the original
	StoreScreenOverlay(Sexy::StoreScreen* theScreen)
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