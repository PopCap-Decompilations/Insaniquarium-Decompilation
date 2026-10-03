#include "BubbleMgr.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Res.h"

using namespace Sexy;

const double PI = 3.141590118408203;

const int BF_RED_TABLE1[30] = {
    125, 255, 135, 175, 195, 255, 255,  60, 255, 190,
    190, 145, 240, 255, 210, 175, 255,  65, 240, 240,
    255, 255, 175, 225, 130, 160, 205, 190, 240, 210
};

const int BF_GREEN_TABLE1[30] = {
    210, 240, 250, 235, 140, 255, 255,  60,  65,  60,
    190, 145, 145, 255, 215, 240, 180,  70, 240, 225,
    230,  40, 215, 175, 165, 230,  50, 140, 250, 250
};

const int BF_BLUE_TABLE1[30] = {
    255,  60, 245, 135,  90,  85, 255,  60,  65, 255,
    190, 175, 235,  40, 175, 175, 110, 160, 240, 125,
      0,   0, 110,  95, 145,  50, 180, 250, 240,  50
};

const int BF_RED_TABLE2[30] = {
     50,  45, 255,  50,  65, 250, 230,   0,  75, 255,
    250,  30, 110, 210, 110,  30, 210, 240,   0, 240,
      0, 255,  15,  65, 125, 145, 255, 250,  80, 110
};

const int BF_GREEN_TABLE2[30] = {
    110,  95,   0, 115, 120, 155, 130,   0,  75,   0,
    250,  25,  10,  10,  95,  80,  10, 240,   5,   5,
    125, 225,  30,  55,  15,   0, 145, 245, 250, 210
};

const int BF_BLUE_TABLE2[30] = {
    210, 195, 125, 210,  65,   0, 250,   0,  75, 125,
    250, 225, 225,   0, 210, 125,   0, 240, 130, 240,
      0,   0,  15, 190,  15, 210, 220, 175, 145,   0
};

BubbleMgr::BubbleMgr()
{
    mBubbleBounds = Rect(0, 0, 640, 480);
    mBubbleFishBounds = Rect(0, 0, 640, 480);
   
    mMaxFish = 0;
    mDefaultBubbleFishVY = 0.0f;

    mMaxBubbles = 10;
    mBubbleSpawnChance = 10;
    mFishSpawnChance = 10;

    mClipBubbleFish = true;
}

BubbleMgr::~BubbleMgr()
{
}

void BubbleMgr::Update()
{
    std::list<Bubble>::iterator itParticle = mBubbleList.begin();
    while (itParticle != mBubbleList.end())
    {
        Bubble* aBubble = &(*itParticle);
        aBubble->Update();

        // A bubble is removed once it has reached the top of the bounds
        if (mBubbleBounds.mY >= aBubble->mY)
            itParticle = mBubbleList.erase(itParticle);
        else
            ++itParticle;
    }

    if ((int)mBubbleList.size() < mMaxBubbles)
        if ((Rand() % 100) < mBubbleSpawnChance)
            SpawnRandomBubble();

    BubbleFishList::iterator itFish = mFishList.begin();
    while (itFish != mFishList.end())
    {
        BubbleFish* aFish = &(*itFish);
        aFish->Update();

        // A fish is removed once it has reached the edge it is swimming towards (two separate
        // erases in the original)
        if (aFish->mVX < 0 && aFish->mX <= mBubbleFishBounds.mX - 80)
            mFishList.erase(itFish++);
        else if (aFish->mVX > 0 && aFish->mX >= mBubbleFishBounds.mX + mBubbleFishBounds.mWidth)
            mFishList.erase(itFish++);
        else
            ++itFish;
    }

    if ((int)mFishList.size() < mMaxFish)
        if ((Rand() % 100) < mFishSpawnChance)
            SpawnRandomBubbleFish();
}

void BubbleMgr::Draw(Graphics* g)
{
    Graphics aBubbleG(*g);
    aBubbleG.ClipRect(mBubbleBounds);
    for (std::list<Bubble>::iterator it = mBubbleList.begin(); it != mBubbleList.end(); ++it)
        it->Draw(&aBubbleG);

    Graphics aFishG(*g);
    if (mClipBubbleFish)
        aFishG.ClipRect(mBubbleFishBounds);
    for (BubbleFishList::iterator it = mFishList.begin(); it != mFishList.end(); ++it)
        it->Draw(&aFishG);
}

void BubbleMgr::SpawnBubble(int theX, int theY)
{
    mBubbleList.push_back(Bubble());

    Bubble& aBubble = mBubbleList.back();
    aBubble.mX = (float)theX;
    aBubble.mY = (float)theY;
}

void BubbleMgr::SpawnRandomBubble()
{
    if (mBubbleBounds.mWidth <= 20)
        return;

    int aRandVal = Rand();
    SpawnBubble(aRandVal % (mBubbleBounds.mWidth-20) + mBubbleBounds.mX, mBubbleBounds.mHeight + mBubbleBounds.mY);
}

void Sexy::BubbleMgr::SpawnRandomBubbleFish()
{
    if (mBubbleFishBounds.mHeight <= 80)
        return;

    mFishList.push_back(BubbleFish());
    BubbleFish* aFishInList = &mFishList.back();

    int aRandVal = Rand();
    aFishInList->mY = aRandVal % (mBubbleFishBounds.mHeight - 80) + mBubbleFishBounds.mY;
    aFishInList->mVY = mDefaultBubbleFishVY;

    aRandVal = Rand();
    if (aRandVal % 2 == 0)
        aFishInList->mX = mBubbleFishBounds.mX - 80;
    else
    {
        aFishInList->mX = mBubbleFishBounds.mWidth + mBubbleFishBounds.mX;
        aFishInList->mVX = -aFishInList->mVX;
    }
}

// Sends every fish away from the middle at full speed, turning the ones that face the other way
void Sexy::BubbleMgr::ScatterFish()
{
    for (BubbleFishList::iterator it = mFishList.begin(); it != mFishList.end(); ++it)
    {
        if (mBubbleFishBounds.mX + mBubbleFishBounds.mWidth / 2 > it->mX)
        {
            if (it->mVX > 0)
                it->mTurnTimer = 5;
            it->mVX = -20.0f;
        }
        else
        {
            if (it->mVX < 0)
                it->mTurnTimer = 5;
            it->mVX = 20.0f;
        }
    }
}

void Sexy::BubbleMgr::SetBubbleBounds(const Rect& theRect)
{
    mBubbleBounds = theRect;
}

void Sexy::BubbleMgr::SetBubbleFishBounds(const Rect& theRect)
{
    mBubbleFishBounds = theRect;
}

void Sexy::BubbleMgr::SetBubbleConfig(int theMaxBubbles, int theBubbleSpawnChance)
{
    mMaxBubbles = theMaxBubbles;
    mBubbleSpawnChance = theBubbleSpawnChance;
}

void Sexy::BubbleMgr::SetBubbleFishConfig(int theMaxBubbleFish, int theBubbleFishSpawnChance)
{
    mMaxFish = theMaxBubbleFish;
    mFishSpawnChance = theBubbleFishSpawnChance;
}

void Sexy::BubbleMgr::SetDefaultBubbleFishVY(float theDefaultVY)
{
    if (mDefaultBubbleFishVY != theDefaultVY)
    {
        mDefaultBubbleFishVY = theDefaultVY;

        BubbleFishList::iterator itFish = mFishList.begin();
        while (itFish != mFishList.end())
        {
            BubbleFish* aFish = &(*itFish);
            aFish->mVY = theDefaultVY;
            ++itFish;
        }
    }
}

void Sexy::BubbleMgr::UpdateALot()
{
    for (int i = 0; i < 500; i++)
        Update();
}

const float gBubbleParticleSpeed1 = 2.0f;
const float gBubbleParticleSpeed2 = 2.5f;   
const float gBubbleParticleSpeed3 = 3.0f;
const float gBubbleParticleSpeed4 = 3.2f;
const float gBubbleParticleSpeed5 = 2.3f;
const float gBubbleParticleSpeed6 = 2.8f;

Bubble::Bubble()
{
    mType = Rand() % 4;
    if (mType == 3)
        mType = Rand() % 4;

    mY = (float)(Rand() % 6 + 400);
    mX = (float)(Rand() % 22 + 150);

    mShakeOffset = 0;

    // An if-chain in the original (not a switch)
    int aSpeedChoice = Rand() % 6;
    if (aSpeedChoice == 0)
        mVY = gBubbleParticleSpeed1;
    else if (aSpeedChoice == 1)
        mVY = gBubbleParticleSpeed2;
    else if (aSpeedChoice == 2)
        mVY = gBubbleParticleSpeed3;
    else if (aSpeedChoice == 3)
        mVY = gBubbleParticleSpeed4;
    else if (aSpeedChoice == 4)
        mVY = gBubbleParticleSpeed5;
    else if (aSpeedChoice == 5)
        mVY = gBubbleParticleSpeed6;
}

void Bubble::Draw(Graphics* g)
{
    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
    g->DrawImageCel(IMAGE_BUBBLES, mShakeOffset + mX, mY, mType);
    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
}

void Bubble::Update()
{
    mShakeOffset = Rand() % 2;
    mY -= mVY;
}

Sexy::BubbleFish::BubbleFish()
{
    // mX and mY are left unset, as in the original
    mVX = 2.0f;
    mUpdateCnt = 0;
    mVY = 0.0f;
    mTurnTimer = 0;

    switch (Rand() % 6)
    {
    case 0:
        mVX = 1.3f;
        break;
    case 1:
        mVX = 1.7f;
        break;
    case 2:
        mVX = 2.0f;
        break;
    case 3:
        mVX = 2.5f;
        break;
    case 4:
        mVX = 3.0f;
        break;
    case 5:
        mVX = 3.5f;
        break;
    default:
        break;
    }
    int aRandVal = Rand() % 10;
    if (aRandVal < 5)
        mSize = 0;
    else
        mSize = (aRandVal >= 8) + 1;

    // Each colour's components are worked out blue first (the Color arguments, right to left)
    aRandVal = Rand() % 30;
    int aBlue = BF_BLUE_TABLE1[aRandVal] + 5;
    if (aBlue > 255) aBlue = 255;
    int aGreen = BF_GREEN_TABLE1[aRandVal] + 5;
    if (aGreen > 255) aGreen = 255;
    int aRed = BF_RED_TABLE1[aRandVal] + 5;
    if (aRed > 255) aRed = 255;
    mColor1 = Color(aRed, aGreen, aBlue, 255);

    aBlue = BF_BLUE_TABLE2[aRandVal] + 5;
    if (aBlue > 255) aBlue = 255;
    aGreen = BF_GREEN_TABLE2[aRandVal] + 5;
    if (aGreen > 255) aGreen = 255;
    aRed = BF_RED_TABLE2[aRandVal] + 5;
    if (aRed > 255) aRed = 255;
    mColor2 = Color(aRed, aGreen, aBlue, 255);
}

void Sexy::BubbleFish::Draw(Graphics* g)
{
    Rect aSrcRect = Rect(((mUpdateCnt / 2) % 10) * 80, mSize * 80, 80, 80);

    int aX = (int)mX;
    float aRad = ((mUpdateCnt * 5) * PI) / 180.0;
    float aSin = (float)sin((double)aRad);
    int aY = (int)(aSin * 3.0 + mY);

    int anImageId = IMAGE_SIMSWIM1_ID;
    bool flip = mVX > 0.0;
    if (mTurnTimer > 0)
    {
        aSrcRect.mX = (5 - mTurnTimer) * 160;
        flip = !flip;
        anImageId = IMAGE_SIMTURN1_ID;
    }

    // Each image is looked up while the DrawImageMirror arguments are being pushed
    g->DrawImageMirror(GetImageById(anImageId), aX, aY, aSrcRect, flip);
    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
    g->SetColorizeImages(true);
    g->SetColor(mColor2);

    g->DrawImageMirror(GetImageById(anImageId + 1), aX, aY, aSrcRect, flip);
    g->SetColor(mColor1);

    g->DrawImageMirror(GetImageById(anImageId + 2), aX, aY, aSrcRect, flip);
    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
    g->SetColorizeImages(false);
}

void Sexy::BubbleFish::Update()
{
    mUpdateCnt++;
    mX = mVX + mX;
    mY = mVY + mY;
    if (mTurnTimer != 0)
        mTurnTimer--;
}
