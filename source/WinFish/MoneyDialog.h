#ifndef __MONEYDIALOG_H__
#define __MONEYDIALOG_H__

#include "SexyAppFramework/Dialog.h"
#include "SexyAppFramework/ButtonListener.h"

namespace Sexy
{
	class WinFishApp;
	class EditWidget;

	class MoneyDialog : public Dialog
	{
	public:
		WinFishApp* mApp;
		int mEnableButtonsTimer;

	public:
		MoneyDialog(WinFishApp* theApp, Image* theComponentImage, Image* theButtonComponentImage,
			int theId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
		virtual ~MoneyDialog();

		int						GetContentX();
		int						GetContentWidth();
		int						GetBodyY();

		virtual void			Update();

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);

		virtual void			DisableButtons(int theEnableButtonsTimer);		//[76]
		virtual void			CheckboxChecked(int theId, bool checked);		//[77]
	};
}

#endif