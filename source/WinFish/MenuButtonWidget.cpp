#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>

#include "MenuButtonWidget.h"
#include "ToolTipWidget.h"
#include "WinFishApp.h"
#include "Board.h"
#include "Res.h"

using namespace Sexy;

Sexy::MenuButtonWidget::MenuButtonWidget(WidgetManager* theWidgetManager, int theId, ButtonListener* 
	theButtonListener, SexyString theTooltipText)
	: ButtonWidget(theId, theButtonListener)
{
	mManager = theWidgetManager;
	mDoFinger = true;
	mClip = false;
	mToolTipXOffset = 15;
	mToolTipYOffset = 60;
	mSlotImage = 0;
	m0x13c = 0;
	m0x138 = 0;
	mToolTipWidget = new ToolTipWidget(theTooltipText);
	mPriceTextColor = Color(0x6e, 0xfa, 0x6e);
}

Sexy::MenuButtonWidget::~MenuButtonWidget()
{
	if (mToolTipWidget)
		delete mToolTipWidget;
}

void Sexy::MenuButtonWidget::RemovedFromManager(WidgetManager* theWidgetManager)
{
	ButtonWidget::RemovedFromManager(theWidgetManager);
	if (mToolTipWidget)
		mManager->RemoveWidget(mToolTipWidget);
}

void Sexy::MenuButtonWidget::Update()
{
	Widget::Update();
	WinFishApp* anApp = (WinFishApp*)gSexyApp;
	if (!anApp->mBoard || !anApp->mBoard->mPause)
	{
		m0x138++;
		if (m0x13c > 0)
			m0x13c--;
	}
}

void Sexy::MenuButtonWidget::Draw(Graphics* g)
{
	bool loc11 = false;
	if (mMouseVisible && m0x13c <= 6)
	{
		ButtonWidget::Draw(g);

		WinFishApp* anApp = (WinFishApp*)gSexyApp;
		int aY = anApp->mBoard ? anApp->mBoard->mGameUpdateCnt : 0;

		if (mSlotImage != 0)
		{
			if (mSlotImageCol >= 0)
				g->DrawImageCel(mSlotImage, mSlotImageX, mSlotImageY, mSlotImageCol, mSlotImageRow);
			else
				g->DrawImageCel(mSlotImage, mSlotImageX, mSlotImageY, (aY / 2) % mSlotImage->mNumCols, mSlotImageRow);
		}

		// The original measures with the font it sets, not g->GetFont()
		Font* aFont = FONT_CONTINUUMBOLD12OUTLINE;
		g->SetFont(aFont);
		g->SetColor(Color(0x6e, 0xfa, 0x6e));
		g->DrawString(m0x15c, (mWidth - aFont->StringWidth(m0x15c)) / 2, aFont->GetAscent() + 9);
		loc11 = true;
	}

	Font* aTinyFont = FONT_TINY;
	g->SetFont(aTinyFont);
	g->SetColor(mPriceTextColor);

	g->DrawString(mPriceText, (mWidth - aTinyFont->StringWidth(mPriceText)) / 2, aTinyFont->GetAscent() + 45);

	if (m0x13c != 0 && m0x13c <= 6)
	{
		g->DrawImageCel(IMAGE_HATCHANIMATION, 0, 0, (6 - m0x13c) / 2);
	}
	if (loc11)
	{
		g->DrawImage(IMAGE_MENUBUTTONREFLECTION, 0,0);
		return;
	}

	Graphics gClip(*g);

	gClip.ClipRect(0, 40, mWidth, mHeight - 40);
	gClip.DrawImage(IMAGE_MENUBUTTONREFLECTION, 0, 0);
}

void Sexy::MenuButtonWidget::MouseEnter()
{
	ButtonWidget::MouseEnter();
	mToolTipWidget->Resize(mToolTipXOffset + mX, mToolTipYOffset + mY, mToolTipWidget->mWidth, mToolTipWidget->mHeight);
	mManager->AddWidget(mToolTipWidget);
}

void Sexy::MenuButtonWidget::MouseLeave()
{
	ButtonWidget::MouseLeave();
	mManager->RemoveWidget(mToolTipWidget);
}

void Sexy::MenuButtonWidget::MouseDown(int theX, int theY, int theClickCount)
{
	Widget::MouseDown(theX, theY, theClickCount);
	mManager->RemoveWidget(mToolTipWidget);
}

void Sexy::MenuButtonWidget::SetPriceTextColor(const Color& theColor)
{
	mPriceTextColor = theColor;
}

void Sexy::MenuButtonWidget::Configure(Image* theImage, int theSlotImageX, int theSlotImageY, int theSlotImageRow, int theSlotImageCol)
{
	mSlotImage = theImage;
	mSlotImageX = theSlotImageX;
	mSlotImageY = theSlotImageY;
	mSlotImageRow = theSlotImageRow;
	mSlotImageCol = theSlotImageCol;
}

void Sexy::MenuButtonWidget::StartHatchAnimation()
{
	m0x13c = 8;
}

void Sexy::MenuButtonWidget::SetSlotText(const SexyString& theText)
{
	m0x15c = theText;
}

void Sexy::MenuButtonWidget::SetSlotPrice(int thePrice)
{
	if (mMouseVisible)
	{
		if (thePrice > 0)
			mPriceText = StrFormat("$%d", thePrice);
		else
			mPriceText = "";
	}
}

void Sexy::MenuButtonWidget::SetPriceText(const SexyString& theText)
{
	mPriceText = theText;
}

void Sexy::MenuButtonWidget::SetMaxedOut()
{
	mDoFinger = false;
	mMouseVisible = false;
	ShowFinger(false);
	m0x15c = "";
	mPriceText = "MAX";
}
