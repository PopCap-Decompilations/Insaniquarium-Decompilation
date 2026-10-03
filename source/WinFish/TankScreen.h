#ifndef __TANKSCREEN_H__
#define __TANKSCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"
#include "SexyAppFramework/Point.h"

namespace Sexy
{
	class WinFishApp;
	class DialogButton;

	class TankScreen : public Widget, public ButtonListener
	{
	public:
		WinFishApp* mApp;
		DialogButton* mMenuButton;
		DialogButton* mTankButtons[4];
		DialogButton* mStoriesButton;
		Point m0xa8[4]; // tank chooser positions

	public:
		TankScreen(WinFishApp* theApp);
		virtual ~TankScreen();

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
		virtual void			Update();
		virtual void			Draw(Graphics* g);

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);

		void					StartTank(int theTankNum);

		static void				DrawBolts(Graphics* g, int xOffset, int yOffset, bool drawRight, bool drawBottom);
	};
}

#endif