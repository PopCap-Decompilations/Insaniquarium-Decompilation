#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/WidgetManager.h>

#include "ToolTipWidget.h"
#include "Res.h"

Sexy::ToolTipWidget::ToolTipWidget(const SexyString& theToolTip)
	: MyLabelWidget()
{
	mLabelFont = FONT_TINYBOLD;
	mMouseVisible = false;

	SetToolTip(theToolTip);
}

// Sets the text and sizes the tip to fit it
void Sexy::ToolTipWidget::SetToolTip(const SexyString& theToolTip)
{
	mLabel = theToolTip;

	Resize(mX, mY, mLabelFont->StringWidth(mLabel) + 10, mLabelFont->GetHeight() + 12);
}

Sexy::ToolTipWidget::~ToolTipWidget()
{
}

void Sexy::ToolTipWidget::Update()
{
	MyLabelWidget::Update();
	mWidgetManager->BringToFront(this);
}

void Sexy::ToolTipWidget::Draw(Graphics* g)
{
	g->SetColor(Color(0xff, 0xff, 200, 0xff));
	g->FillRect(0, 0, mWidth, mHeight);
	g->SetColor(Color(0, 0, 0, 255));
	g->DrawRect(0, 0, mWidth - 1, mHeight - 1);
	g->SetFont(mLabelFont);

	g->DrawString(mLabel, 5, ((mHeight - mLabelFont->GetHeight()) >> 1) + mLabelFont->GetAscent());
}
