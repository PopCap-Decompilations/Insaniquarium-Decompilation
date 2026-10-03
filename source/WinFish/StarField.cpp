#include <SexyAppFramework/DDImage.h>

#include "StarField.h"
#include "WinFishApp.h"
#include "Board.h"

using namespace Sexy;

Sexy::StarField::StarField()
{
	mMaxStars = 0;
	mNebulaImage = 0;
}

Sexy::StarField::~StarField()
{
	if (mNebulaImage)
		delete mNebulaImage;
}

void Sexy::StarField::Init(int theMaxStars)
{
	mMaxStars = theMaxStars;
	if (mNebulaImage == nullptr)
		mNebulaImage = gSexyApp->GetImage("images/nebula1");

	mStarList.clear();
	if (theMaxStars > 0)
	{
		for (int i = 0; i < theMaxStars; ++i)
		{
			int y = Rand() % 480;
			int x = Rand() % 640;

			AddStar(x, y);
		}
	}
}

void Sexy::StarField::AddStar(int theX, int theY)
{
	mStarList.push_back(Star());

	Star& newStar = mStarList.back();

	newStar.mY = theY;
	newStar.mX = theX;
	newStar.mVY = 0;

	// The original's speeds are one float step away from -1.8f and -2.7f (0xBFE66667 and 0xC02CCCCC)
	switch (rand() % 3)
	{
	case 0:
		newStar.mColorValue = 0x404040;
		newStar.mVX = -0.6f;
		break;
	case 1:
		newStar.mColorValue = 0x909090;
		newStar.mVX = -1.8000001f;
		break;
	case 2:
		newStar.mColorValue = 0xFFFFFF;
		newStar.mVX = -2.6999998f;
		break;
	}
}

void Sexy::StarField::Update()
{
	StarList::iterator it = mStarList.begin();
	while (it != mStarList.end())
	{
		Star& currentStar = *it;
		currentStar.mX = currentStar.mVX + currentStar.mX;
		currentStar.mY = currentStar.mVY + currentStar.mY;

		// A NaN position keeps the star (0x5116D0)
		if (currentStar.mX <= 0.0f)
			it = mStarList.erase(it);
		else
			++it;
	}

	// The list size is read again after each star
	while ((int)mStarList.size() < mMaxStars)
		AddStar(640, Rand() % 480);
}

void Sexy::StarField::Draw(Graphics* g, bool flag)
{
	// The original keeps the app pointer and reads mBoard and the image width again at each use
	WinFishApp* anApp = (WinFishApp*)gSexyApp;
	if (mNebulaImage)
	{
		int aX = (mNebulaImage->mWidth - anApp->mBoard->mGameUpdateCnt / 2 % mNebulaImage->mWidth)-1;
		g->DrawImage(mNebulaImage, aX - mNebulaImage->mWidth, 0);
		g->DrawImage(mNebulaImage, aX, 0);
		g->DrawImage(mNebulaImage, aX + mNebulaImage->mWidth, 0);
		if (flag)
		{
			anApp->mBoard->Unk06(g, mNebulaImage, aX - mNebulaImage->mWidth, 0, 8.0f);
			anApp->mBoard->Unk06(g, mNebulaImage, aX, 0, 8.0f);
			anApp->mBoard->Unk06(g, mNebulaImage, aX + mNebulaImage->mWidth, 0, 8.0f);
		}
	}
	else
	{
		g->SetColor(Color(0));
		g->FillRect(0, 0, 640, 480);
	}
	StarList::iterator it;
	for (it = mStarList.begin(); it != mStarList.end(); ++it)
	{
		Star& aStar = *it;
		g->SetColor(Color(aStar.mColorValue));
		g->FillRect(aStar.mX, aStar.mY, 1, 1);
	}
}
