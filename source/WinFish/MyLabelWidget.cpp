#include <SexyAppFramework/Font.h>

#include "MyLabelWidget.h"
#include "Res.h"

using namespace Sexy;

Sexy::MyLabelWidget::MyLabelWidget()
{
	mLabel = "";
	mAlignment = 0;
	mLabelColor = Color(0x6e, 0xfa, 0x6e, 0xff);
	mLabelFont = FONT_CONTINUUMBOLD12OUTLINE;
}

Sexy::MyLabelWidget::~MyLabelWidget()
{
}

void Sexy::MyLabelWidget::Draw(Graphics* g)
{
	g->SetColor(mLabelColor);
	g->SetFont(mLabelFont);

	int aY = (mHeight - mLabelFont->GetHeight()) / 2 + mLabelFont->GetAscent();

	int aX, aWidth;
	switch (mAlignment)
	{
	case 0:
		aX = 0;
		break;
	case 1:
		aWidth = mLabelFont->StringWidth(mLabel);
		aX = (mWidth - aWidth) / 2;
		break;
	default:
		aWidth = mLabelFont->StringWidth(mLabel);
		aX = mWidth - aWidth;
		break;
	}

	g->DrawString(mLabel, aX, aY);
}

void Sexy::MyLabelWidget::SetLabel(SexyString theLabel)
{
	mLabel = theLabel;
	MarkDirty();
}
