#ifndef __USERDIALOG_H__
#define __USERDIALOG_H__

#include "MoneyDialog.h"
#include "SexyAppFramework/ButtonListener.h"
#include "SexyAppFramework/ListListener.h"
#include "SexyAppFramework/EditListener.h"

namespace Sexy
{
	class WinFishApp;
	class ListWidget;
	class ScrollbarWidget;

	class UserDialog : public MoneyDialog, public ListListener, public EditListener
	{
	public:
		ListWidget*				mListWidget;
		EditWidget*				mEditWidget;
		ScrollbarWidget*		mScrollbarWidget;
		DialogButton*			mRenameButton;
		DialogButton*			mDeleteButton;
		bool					mTransferShells;

	public:
		UserDialog(WinFishApp* theApp, bool transferShells);
		virtual ~UserDialog();

		virtual void			Draw(Graphics* g);

		virtual void			AddedToManager(WidgetManager* theWidgetManager);
		virtual void			RemovedFromManager(WidgetManager* theWidgetManager);

		virtual void			Resize(int theX, int theY, int theWidth, int theHeight);

		virtual int				GetPreferredHeight(int theWidth);

		virtual void			ButtonPress(int theId);
		virtual void			ButtonDepress(int theId);
		virtual void			ListClicked(int theId, int theIdx, int theClickCount);
		virtual void			EditWidgetText(int theId, const SexyString& theString);
		virtual bool			AllowChar(int theId, SexyChar theChar);

		SexyString				GetSelectedUserName();
		int						GetShellsValue();
		void					RemoveSelectedUser();
		void					RenameSelectedUser(const SexyString& theNewName);

	};
}

#endif