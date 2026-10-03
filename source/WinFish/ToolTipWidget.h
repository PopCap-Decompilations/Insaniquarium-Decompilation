#ifndef __TOOLTIPWIDGET_H__
#define __TOOLTIPWIDGET_H__

#include "MyLabelWidget.h"

namespace Sexy
{
	class ToolTipWidget : public MyLabelWidget
	{
	public:
		ToolTipWidget(const SexyString& theToolTip);
		virtual ~ToolTipWidget();

		virtual void			Update();
		virtual void			Draw(Graphics* g);

		void					SetToolTip(const SexyString& theToolTip);
	};
}

#endif