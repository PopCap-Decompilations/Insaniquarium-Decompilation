#include <SexyAppFramework/WidgetManager.h>

#include "Fish.h"
#include "WinFishApp.h"
#include "Board.h"
#include "ProfileMgr.h"
#include "FishTypePet.h"
#include "Shadow.h"
#include "Food.h"
#include "Alien.h"
#include "Coin.h"
#include "MessageWidget.h"
#include "Missle.h"
#include "Res.h"

using namespace Sexy;

const int OSCAR_RED_TABLE1[12] =
{ 165, 40, 30, 255, 145, 40, 150, 200, 130, 220, 185, 40 };
const int OSCAR_GREEN_TABLE1[12] =
{ 210, 40, 30, 155, 210, 40, 125, 125, 190, 190, 100, 40 };
const int OSCAR_BLUE_TABLE1[12] =
{ 215, 40, 30, 0, 145, 40, 85, 215, 85, 140, 240, 40 };

const int OSCAR_RED_TABLE2[12] =
{ 75, 20, 40, 255, 20, 75, 160, 150, 245, 180, 105, 75 };
const int OSCAR_GREEN_TABLE2[12] =
{ 75, 70, 40, 65, 75, 70, 65, 80, 170, 170, 15, 75 };
const int OSCAR_BLUE_TABLE2[12] =
{ 75, 110, 40, 0, 120, 70, 25, 175, 0, 155, 245, 75 };

const int OSCAR_RED_TABLE3[12] =
{ 40, 50, 75, 250, 35, 255, 90, 170, 245, 140, 40, 255 };
const int OSCAR_GREEN_TABLE3[12] =
{ 125, 150, 75, 255, 150, 255, 70, 255, 245, 120, 40, 110 };
const int OSCAR_BLUE_TABLE3[12] =
{ 130, 225, 75, 0, 30, 255, 40, 255, 140, 80, 40, 0 };

const int FISH_RED_TABLE1[30] =
{ 125, 255, 135, 175, 195, 255, 255, 60,  255, 190,
    190, 145, 240, 255, 210, 175, 255, 65,  240, 240,
    255, 255, 175, 225, 130, 160, 205, 190, 240, 210 };
const int FISH_GREEN_TABLE1[30] =
{ 210, 240, 250, 235, 140, 255, 255, 60,  65,  60,
    190, 145, 145, 255, 215, 240, 180, 70,  240, 225,
    230, 40,  215, 175, 165, 230, 50,  140, 250, 250 };
const int FISH_BLUE_TABLE1[30] =
{ 255, 60,  245, 135, 90,  85,  255, 60,  65,  255,
    190, 175, 235, 40,  175, 175, 110, 160, 240, 125,
    0,   0,   110, 95,  145, 50,  180, 250, 240, 50 };

const int FISH_RED_TABLE2[30] =
{ 50,  45,  255, 50,  65,  250, 230, 0,   75,  255,
    250, 30,  110, 210, 110, 30,  210, 240, 0,   240,
    0,   255, 15,  65,  125, 145, 255, 250, 80,  110
}; 
const int FISH_GREEN_TABLE2[30] =
{ 110, 95, 0,   115, 120, 155, 130, 0,   75,  0,
    250, 25, 10,  10,  95,  80,  10,  240, 5,   5,
    125, 225, 30,  55,  15,  0,   145, 245, 250, 210 };
const int FISH_BLUE_TABLE2[30] =
{ 210, 195, 125, 210, 65,  0,   250, 0,   75,  125,
    250, 225, 225, 0,   210, 125, 0,   240, 130, 240,
    0,   0,   15,  190, 15,  210, 220, 175, 145, 0 };

//int Sexy::gWadsworthTimer;
//int Sexy::gWadsworthX;
//int Sexy::gWadsworthY;

// The colours are only default-constructed (black, alpha 255) in each constructor, as the original
Fish::Fish() : GameObject()
{
    mUnusedVXWadsworthAddon = 0;
    mClip = false;
    mType = TYPE_GUPPY;
    mWadsworthVXModCounter = 0;
    mUnusedTimer = 0;
    mVirtualFish = 0;
    mRainbowFishDeterminant = 0;
}

Fish::Fish(int theX, int theY) : GameObject()
{
    Init(theX, theY);
    mSize = SIZE_SMALL;
}

Fish::Fish(int theX, int theY, int theSize, bool theVelDirection) : GameObject()
{
    Init(theX, theY);
    mSize = theSize;
    mVX = theVelDirection ? 1.0 : -1.0;
    mPrevVX = mVX;
}

Fish::~Fish()
{
}

void Fish::Update()
{
    if (mApp->mBoard == NULL || mApp->mBoard->mPause)
        return;
    UpdateCounters();
    if (gWadsworthTimer && mIsGuppy && mSize < 2) // 358 line decomp wadsworth
    {
        if (mWadsworthVXModCounter == 0)
        {
            if (mVX >= 0.0)
                mVX = 0.5;
            else
                mVX = -0.5;
        }
        if (mBoughtTimer == 0)
            mVY = 0.0;

        if (mXD > gWadsworthX + 50)
            mXD -= 5.0;
        else if (mXD < gWadsworthX - 50)
            mXD += 5.0;
        else if (mXD > gWadsworthX + 5)
            mXD -= 3.0;
        else if (mXD < gWadsworthX - 5)
            mXD += 3.0;

        if (mYD > gWadsworthY + 50)
            mYD -= 4.0;
        else if (mYD < gWadsworthY - 50)
            mYD += 4.0;
        else if (mYD > gWadsworthY + 5)
            mYD -= 3.0;
        else if (mYD < gWadsworthY - 5)
            mYD += 3.0;
    }
    else if (!Hungry())
    {
        Board* aBoard = mApp->mBoard;
        if (aBoard->AliensInTank() && aBoard->mPetsInTank[12] && mIsGuppy) // 236 line decomp follow Gumbo
        {
            // The pet list is walked with checked iterators (its end read afresh from mApp->mBoard),
            // Gumbo is read through the iterator for each test, and each bound is an int addition
            // converted to double, as the original
            std::vector<FishTypePet*>::iterator it;
            for (it = aBoard->mFishTypePetList->begin(); it != mApp->mBoard->mFishTypePetList->end(); it++)
            {
                if ((*it)->mFishTypePetType == PET_GUMBO)
                    break;
            }

            if (mXD + 40.0 > (*it)->mX + 50)
            {
                if (mVX > -4.0)
                    mVX -= 1.3;
            }
            else if (mXD + 40.0 < (*it)->mX + 30)
            {
                if (mVX < 4.0)
                    mVX += 1.3;
            }
            else if (mXD + 40.0 > (*it)->mX + 45)
            {
                if (mVX > -4.0)
                    mVX -= 0.2;
            }
            else if (mXD + 40.0 < (*it)->mX + 35)
            {
                if (mVX < 4.0)
                    mVX += 0.2;
            }
            else if (mXD + 40.0 > (*it)->mX + 40)
            {
                if (mVX > -4.0)
                    mVX -= 0.05;
            }
            else if (mXD + 40.0 < (*it)->mX + 40)
            {
                if (mVX < 4.0)
                    mVX += 0.05;
            }

            if (mYD + 40.0 > (*it)->mY + 25)
            {
                if (mVY > -3.0)
                    mVY -= 1.0;
            }
            else if (mYD + 40.0 < (*it)->mY + 15)
            {
                if (mVY < 4.0)
                    mVY += 1.3;
            }
            else if (mYD + 40.0 > (*it)->mY + 20)
            {
                if (mVY > -3.0)
                    mVY -= 0.5;
            }
            else if (mYD + 40.0 < (*it)->mY + 20)
            {
                if (mVY < 4.0)
                    mVY += 0.7;
            }

            if (mY <= mYMin && mVY < 0.0)
                mVY = 0.0;
            if (mVXAbs < 5)
                mVXAbs++;
        }
        else if (mMovementState == 123)
        {
            mVXAbs = (int)abs(mVX);
            mMovementStateChangeTimer = 0;
            if (mSpecialMovementStateChangeTimer > 30)
            {
                if (abs(mVX) > 2.0)
                    mVX *= 0.95;
                if (abs(mVY) > 2.0)
                    mVY *= 0.95;
            }
            if (mSpecialMovementStateChangeTimer > 50)
            {
                mMovementState = mApp->mSeed->Next() % 9 + 1;
                mSpecialMovementStateChangeTimer = 0;
                if (mXD > mXMax - 20 || mXD < mXMin + 20 || Rand() % 3 == 0)
                    mVX *= -1.0;
            }
        }
        else if (mMovementState > 4)
        {
            if(mBoughtTimer == 0)
            {
                if (mYD < 115.0)
                    mVY = -0.1;
                else
                    mVY = -0.5;
            }
            if (mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;
                if (mXDirection == 1)
                {
                    if (mVX >= 0.0)
                        mVX += 1.0;
                    else
                        mVX += 2.0;

                    mVXAbs = (int)abs(mVX);
                    if (mXD > 250.0)
                    {
                        mXDirection = -1;
                        mVX -= 2.0;
                    }
                }
                else if (mXDirection == -1)
                {
                    if (mVX <= 0.0)
                        mVX -= 1.0;
                    else
                        mVX -= 2.0;

                    mVXAbs = (int)abs(mVX);
                    if (mXD < 175.0)
                    {
                        mXDirection = 1;
                        mVX += 2.0;
                    }
                }
            }
        }
        else if (mMovementState == 0)
        {
            if (mBoughtTimer == 0)
                mVY = 0.5;
            if(mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;
                if (mVX > 0.0)
                {
                    if (mVX > 0.5)
                        mVX -= 0.5;
                }
                else if (mVX < 0.0)
                {
                    if (mVX < -0.5)
                        mVX += 0.5;
                }

                mVXAbs = (int)abs(mVX);
            }

            mYD -= 0.25 / mSpeedMod;
        }
        else if (mMovementState == 1)
        {
            if (mBoughtTimer == 0)
                mVY = -0.5;
            if (mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;
                if (mVX > 1.0)
                    mVX--;
                else if (mVX < 1.0)
                    mVX++;
                mVXAbs = (int)abs(mVX);
            }

            mYD -= 0.5 / mSpeedMod;
        }
        else if (mMovementState == 2)
        {
            if (mBoughtTimer == 0)
                mVY = -0.5;
            if (mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;
                if (mVX > -1.0)
                    mVX--;
                else if (mVX < -1.0)
                    mVX++;

                mVXAbs = (int)abs(mVX);
            }

            mYD -= 0.5 / mSpeedMod;
        }
        else if (mMovementState == 3)
        {
            if (mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;
                if (mVX > -1.0)
                    mVX--;
                else if (mVX < -1.0)
                    mVX++;

                if (mVY > 3.0)
                    mVY--;
                else if (mVY < 3.0)
                    mVY++;

                if (mVXAbs > 4)
                    mVXAbs--;
                else if (mVY < 4.0)
                    mVXAbs++;
            }
            if (mYD > 240.0)
                mMovementState = 0;
        }
        else if (mMovementState == 4)
        {
            if (mSpecialMovementStateChangeTimer >= 40)
            {
                mSpecialMovementStateChangeTimer = 0;

                if (mVX > 1.0)
                    mVX--;
                else if (mVX < 1.0)
                    mVX++;

                if (mVY > 3.0)
                    mVY--;
                else if (mVY < 3.0)
                    mVY++;

                if (mVXAbs > 4)
                    mVXAbs--;
                else if (mVY < 4.0)
                    mVXAbs++;
            }
            if (mYD > 240.0)
                mMovementState = 0;
        }
    }

    mSpecialMovementStateChangeTimer++;
    mMovementStateChangeTimer++;
    if (mMovementStateChangeTimer > 20)
    {
        mMovementStateChangeTimer = 0;
        if (mApp->mSeed->Next() % 10 == 0)
            mMovementState = mApp->mSeed->Next() % 9 + 1;
    }

    if (!mApp->mBoard->AliensInTank())
        DropCoin();

    if (mBoughtTimer != 0)
    {
        mBoughtTimer--;
        mVY *= 0.9;
    }
    if (mWadsworthVXModCounter != 0)
    {
        // The damped addon is stored first and then added, as the original
        mWadsworthVXModCounter--;
        mUnusedVXWadsworthAddon *= 0.9;
        mXD += mUnusedVXWadsworthAddon;
    }
    if (mUnusedTimer > 0)
    {
        mUnusedTimer--;
    }
    if (mVX == 0.0)
        mYD += 1.0 / mSpeedMod;
    if (mVX == 1.0)
        mYD += 0.75 / mSpeedMod;
    if (mVX == 2.0)
        mYD += 0.5 / mSpeedMod;
    if (mVX == 3.0)
        mYD += 0.25 / mSpeedMod;

    if (mYMax <= 320)
        mYD -= 0.25;
    if (mXD > (double)mXMax)
        mXD = (double)mXMax;
    if (mXD < (double)mXMin)
        mXD = (double)mXMin;
    if (mYD > (double)mYMax)
        mYD = (double)mYMax;

    if (mBoughtTimer > 0 && mVY > 0.0)
    {
        if (mBoughtTimer > 30)
        {
            int aChance = 2;
            int aSpawnX;
            int aSpawnY;
            if (mBoughtTimer > 40)
                aChance = 1;

            // All from mApp->mSeed; for each bubble the original draws the x offset before the
            // y offset, then adds both to the position
            if (mApp->mSeed->Next() % aChance == 0)
            {
                if (mSize == SIZE_SMALL)
                {
                    if ((mApp->mSeed->Next() & 1) == 0)
                    {
                        aSpawnX = 15 - mApp->mSeed->Next() % 30;
                        aSpawnY = 15 - mApp->mSeed->Next() % 30;
                        mApp->mBoard->SpawnBubble(mX + aSpawnX + 25, mY + aSpawnY + 25);
                    }
                }
                else if (mType == TYPE_ULTRA)
                {
                    aSpawnX = 60 - mApp->mSeed->Next() % 120;
                    aSpawnY = 60 - mApp->mSeed->Next() % 120;
                    mApp->mBoard->SpawnBubble(mX + aSpawnX + 65, mY + aSpawnY + 65);
                    aSpawnX = 40 - mApp->mSeed->Next() % 80;
                    aSpawnY = 40 - mApp->mSeed->Next() % 80;
                    mApp->mBoard->SpawnBubble(mX + aSpawnX + 65, mY + aSpawnY + 65);
                    aSpawnX = 40 - mApp->mSeed->Next() % 80;
                    aSpawnY = 40 - mApp->mSeed->Next() % 80;
                    mApp->mBoard->SpawnBubble(mX + aSpawnX + 65, mY + aSpawnY + 65);
                }
                else
                {
                    aSpawnX = 30 - mApp->mSeed->Next() % 60;
                    aSpawnY = 30 - mApp->mSeed->Next() % 60;
                    mApp->mBoard->SpawnBubble(mX + aSpawnX + 25, mY + aSpawnY + 25);
                    aSpawnX = 20 - mApp->mSeed->Next() % 40;
                    aSpawnY = 20 - mApp->mSeed->Next() % 40;
                    mApp->mBoard->SpawnBubble(mX + aSpawnX + 25, mY + aSpawnY + 25);
                }
            }
        }
    }
    else if (mYD < (double)mYMin)
        mYD = (double)mYMin;

    if (mXD > mXMax - 5 && mVX > 0.1)
        mVX -= 0.1;
    if (mXD < 15 && mVX < -0.1)
        mVX += 0.1;

    FishUpdateAnimation();

    double aSpeedMod = mSpeedMod;
    if (mSpeedy)
    {
        if (mSpeedySpeedState != 0)
            aSpeedMod = 0.8;
        else
            aSpeedMod = 0.3;
    }

    mXD += mVX / aSpeedMod;
    mYD += mVY / aSpeedMod;

    Move(mXD, mYD);
}

void Fish::Draw(Graphics* g)
{
    UpdateFishSongMgr();

    if (gWadsworthTimer != 0 && mXD < gWadsworthX + 8 && mXD > gWadsworthX - 8 &&
        mYD < gWadsworthY + 8 && mYD > gWadsworthY - 8 && mIsGuppy && mSize < 2)
    {
        if (mShadowPtr)
            mShadowPtr->m0x168 = 0.0;
        return;
    }
    if(mShadowPtr)
        mShadowPtr->m0x168 = 1.0;

    if (mTurnAnimationTimer == 0)
    {
        if (mVX < 0.0 || ((int)mVX == 0 && mPrevVX < 0.0)) // facing left
            DrawFish(g, mForwardlyChallenged);
        else if (mVX > 0.0 || ((int)mVX == 0 && mPrevVX > 0.0)) // facing right
            DrawFish(g, !mForwardlyChallenged);
        // mVX == 0 and mPrevVX == 0: the original draws no fish
    }
    else if (mTurnAnimationTimer > 0)
        DrawFish(g, !mForwardlyChallenged);
    else if (mTurnAnimationTimer < 0)
        DrawFish(g, mForwardlyChallenged);

    if (mName.size() > 0)
        DrawName(g, false);
}

void Sexy::Fish::MouseDown(int x, int y, int theClickCount)
{
    if (theClickCount < 0)
    {
        mApp->mBoard->CheckMouseDown(mX + x, mY + y);
        return;
    }

    // mApp->mBoard is read afresh after each call, as the original
    if (mApp->mBoard->mAlienList->size() == 0 && mApp->mBoard->mBilaterusList->size() == 0 &&
        mApp->mBoard->mMissleList1->size() == 0 && mIsGuppy)
    {
        double absX = x + mXD;
        if (absX < 587.0 && absX > 30.0)
        {
            double absY = y + mYD;
            if (absY < 400.0 && absY > 60.0 && !mApp->mBoard->Unk11(mX + x, mY + y) &&
                mApp->mBoard->Buy(mApp->mBoard->m0x4ac, true))
            {
                // The drop point is truncated from the doubles, as the original
                if (x >= 10 && x <= 70 && y >= 10 && y <= 70)
                    mApp->mBoard->DropFood(x + mXD - 10, y + mYD - 10, 0, false, (70 - y) / 2, -1);
                else
                    mApp->mBoard->DropFood(x + mXD - 10, y + mYD - 10, 0, false, 0, -1);
                mApp->mBoard->m0x4ec = true;
                mApp->mBoard->m0x3c0 = mApp->mBoard->Unk01();
            }
        }
    }
}

void Sexy::Fish::MouseUp(int x, int y, int theClickCount)
{
    GameObject::MouseUp(x, y, theClickCount);
    mApp->mBoard->m0x4ec = false;
    mApp->mBoard->m0x4ed = false;
}

void Sexy::Fish::MouseDrag(int x, int y)
{
    // 0x4D8C10, shared with Board/Grubber/Penta/Breeder/OtherTypePet: forwards to the empty Widget::MouseDrag
    GameObject::MouseDrag(x, y);
}

int Sexy::Fish::SpecialReturnValue()
{
    if(mType != TYPE_BALL_FISH && mType != TYPE_BI_FISH)
        return 0;
    return 10;
}

int Sexy::Fish::GetShellPrice()
{
    int aVal = GameObject::GetShellPrice();
    if (mVirtualFish && aVal < 0)
        aVal = mShellPrice / 2;

    if (mIsGuppy && aVal > 0)
    {
        switch (mSize)
        {
        case SIZE_MEDIUM:
            aVal *= 2;
            break;
        case SIZE_LARGE:
            aVal *= 3;
            break;
        case SIZE_CROWNED:
            aVal *= 5;
            break;
        }
    }
    return aVal;
}

void Sexy::Fish::Remove()
{
    RemoveFromGame(true);
}

void Sexy::Fish::SetPosition(int newX, int newY)
{
    mX = newX;
    mXD = newX;
    mY = newY;
    mYD = newY;
}

void Sexy::Fish::OnFoodAte(GameObject* obj)
{
    bool isHungry = IsHungryVisible();

    mApp->mBoard->PlaySlurpSound(mVoracious);
    Food* aFood = (Food*)obj;
    bool unkflag01 = aFood->mExoticFoodType == 2;
    Unk02(unkflag01);

    // unkflag02: whether the food counts towards growing
    bool unkflag02 = true;
    if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
    {
        if (mVirtualTankId >= 0 && !unkflag01) // 41
        {
            unkflag02 = UpdateMentalState();
            if (mPreNamedTypeId == ROCKY)
                unkflag02 = false;
        }
        else
            unkflag02 = false;
    }

    if (!unkflag01) // from 91 to 188
    {
        if (aFood->mFoodType == 0)
        {
            mHunger += 500;
            if (mBeginner) mHunger += 200;
            if (mHunger > 800) mHunger = 800;
            if (unkflag02) mFoodAte++;
        }
        else if (aFood->mFoodType == 1)
        {
            mHunger += 700;
            if (mHunger > 1000) mHunger = 1000;
            if (unkflag02) mFoodAte += (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK) + 1;
        }
        else if (aFood->mFoodType == 3)
        {
            if (mSize == 0 || mSize == 1)
            {
                mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
                // For each shot the original draws the type, then y, then x
                for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
                {
                    int aType = mApp->mSeed->Next() % 3 + 3;
                    int aY = mApp->mSeed->Next() % 30 + mY - 10;
                    int aX = mApp->mSeed->Next() % 30 + mX - 10;
                    mApp->mBoard->SpawnShot(aX, aY, aType);
                }

                if (mApp->mBoard->mTank == 2 && mApp->mBoard->mLevel < 3 && !mApp->mCurrentProfile->mFinishedGame)
                {
                    if (mApp->mBoard->mMessageShown[32])
                        mApp->mBoard->ShowText("Hint: Feed star potions to BIG guppies!", false, 33);
                    if (mApp->mBoard->mMessageShown[31])
                        mApp->mBoard->ShowText("Hint: Star potions are for big guppies only!", false, 32);
                    if (mApp->mBoard->mMessageShown[30])
                        mApp->mBoard->ShowText("Hint: Only certain fish can handle star potions!", false, 31);

                    mApp->mBoard->ShowText("Warning! Use star potions carefully!", false, 30);
                }

                mApp->mBoard->m0x49c++;
                Die(true);
            }
            else // 156
            {
                mHunger += 1100;
                if (mSize == SIZE_LARGE) // a big guppy becomes a star guppy
                {
                    if (mHunger > 1400)
                        mHunger = 1400;
                    mSize = SIZE_STAR;
                    mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
                }
                else if(mHunger > 1400)
                    mHunger = 1400;
            }
        }
        else
        {
            if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
            {
                mHunger += 500;
                if (mHunger > 800) mHunger = 800;
            }
            else
            {
                mHunger += 1100;
                if (mHunger > 1400) mHunger = 1400;
            }
            if (unkflag02)
                mFoodAte += (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK) + 2;
        }
    }
    else if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK) // 50
    {
        if (mHunger > 30)
            mHunger = 30;

        mApp->mBoard->MakeNote(mX + mWidth / 2 - 20, mY - 5, 2, "YUK!");
    }
    else if (mSize <= 2) // 71
    {
        RemoveFromGame(true);
        FishTypePet* aPet = (FishTypePet*) mApp->mBoard->SpawnPet(PET_NOSTRADAMUS, mX, mY, false, true);
        if (aPet)
            aPet->m0x240 = 1;
        if (!mApp->mBoard->HasAnyFish())
            mApp->mBoard->SpawnGuppyBought();
        return;
    }

    if (mFoodAte >= mFoodNeededToGrow)
    {
        if (mSize < 2)
        {
            mSize++;
            if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
                FishOnGrow();

            mFoodAte = 0;
            mGrowthAnimationTimer = 10;
            mApp->mBoard->PlaySample(SOUND_GROW_ID, 3, 1.0);
        }
        if (mFoodAte >= mFoodNeededToGrow * (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK ? 15 : 8)
            && mSize >= SIZE_LARGE && mSize != SIZE_CROWNED && !mHasSpecialColors)
        {
            mSize = SIZE_CROWNED;
            mApp->mBoard->PlaySample(SOUND_CROWNED_ID, 3, 1.0);
        }
    }

    UpdateHungerStateIfWasHungry(isHungry);
}

void Sexy::Fish::UpdateStoreAnimation()
{
    mUpdateCnt++;
    UpdateStoreCounters();
    if (mRainbowFish)
        UpdateRainbow();

    mStoreAnimationIndex = (mStoreAnimationTimer / 2) % 10;
}

void Sexy::Fish::DrawStoreAnimation(Graphics* g, int justification)
{
    int anXTrans = 0, anYTrans = 0;
    switch (justification)
    {
    case 0:
        anXTrans = 19;
        anYTrans = 20;
        break;
    case 1:
        anXTrans = 10;
        anYTrans = 17;
        break;
    case 2:
        anXTrans = 19;
        break;
    case 3:
        anXTrans = 5;
        break;
    case 4:
        anXTrans = -40;
        anYTrans = -5;
        break;
    }
    g->Translate(anXTrans, anYTrans);
    int aRow = mSize;
    if (mSize > TYPE_BIG_GUPPY)
    {
        if (mSize == TYPE_STAR_GUPPY)
            aRow = 2;
        else
        {
            aRow = 3;
            if (mSize != TYPE_CROWNED_GUPPY)
                aRow = justification;
        }
    }

    if (mHasSpecialColors)
    {
        Rect aSrcRect(mStoreAnimationIndex * 80, aRow * 80, 80, 80);
        SetStoreColor(g, Color::White);
        g->DrawImageMirror(GetImageById(IMAGE_SIMSWIM1_ID), 0, 0, aSrcRect, false);
        g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
        SetStoreColor(g, mColors[1]);
        g->DrawImageMirror(GetImageById(IMAGE_SIMSWIM2_ID), 0, 0, aSrcRect, false);
        SetStoreColor(g, mColors[0]);
        g->DrawImageMirror(GetImageById(IMAGE_SIMSWIM3_ID), 0, 0, aSrcRect, false);
        g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
    }
    else
    {
        SetStoreColor(g, Color::White);
        g->DrawImage(IMAGE_SMALLSWIM, 0, 0, Rect(mStoreAnimationIndex * 80, aRow * 80, 80, 80));
    }
    g->SetColorizeImages(false);
    g->Translate(-anXTrans, -anYTrans);
}

void Sexy::Fish::Sync(DataSync* theSync)
{
    GameObject::Sync(theSync);
    theSync->SyncDouble(mXD);
    theSync->SyncDouble(mYD);
    theSync->SyncDouble(mVX);
    theSync->SyncDouble(mVY);
    theSync->SyncLong(mXDirection);
    theSync->SyncLong(m0x17c);
    theSync->SyncDouble(mSpeedMod);
    theSync->SyncDouble(mPrevVX);
    theSync->SyncLong(mYMax);
    theSync->SyncLong(mXMin);
    theSync->SyncLong(mYMin);
    theSync->SyncLong(mXMax);
    theSync->SyncLong(mSize);
    theSync->SyncLong(mFoodAte);
    theSync->SyncLong(mFoodNeededToGrow);
    theSync->SyncLong(mMovementState);
    theSync->SyncLong(mSpecialMovementStateChangeTimer);
    theSync->SyncLong(mMovementStateChangeTimer);
    theSync->SyncLong(mSwimFrameCounter);
    theSync->SyncLong(mVXAbs);
    theSync->SyncLong(mAnimationFrameIndexFish);
    theSync->SyncLong(mTurnAnimationTimer);
    theSync->SyncLong(mEatingAnimationTimer);
    theSync->SyncLong(mGrowthAnimationTimer);
    theSync->SyncBool(mHasSpecialColors);
    if(mHasSpecialColors)
    {
        theSync->SyncBool(mRainbowFish);
        SyncColor(theSync, mColors[0]);
        SyncColor(theSync, mColors[1]);
        SyncColor(theSync, mColors[2]);
        if (theSync->mReader && mRainbowFish)
            UpdateRainbow();
    }
    theSync->SyncLong(mBoughtTimer);
    theSync->SyncLong(mCoinDropTimer);
    theSync->SyncLong(mCoinDropT);
    theSync->SyncBool(mBeginner);
    theSync->SyncBool(mIsGuppy);
    if(mVirtualTankId >= 0)
        theSync->SyncBool(mVirtualFish);
}

bool Fish::Hungry()
{
    UpdateHungerAnimCounter();
    // The original keeps the alien list across the calls below, and tests the lists inline here
    std::vector<Alien*>* anAlienList = mApp->mBoard->mAlienList;
    if (anAlienList->empty() && mApp->mBoard->mBilaterusList->empty())
        UpdateHungerCounter();

    if (ShouldDie() && !mBeginner)
    {
        Die(true);
        return false;
    }
    if (mHunger == -1 && mBeginner)
    {
        if (mApp->mBoard->mMessageShown[23])
            mApp->mBoard->ShowText("Fish are hungry! Click to drop food!", true, 24);
        if (mApp->mBoard->mMessageShown[8])
            mApp->mBoard->ShowText("Fish are hungry! Click to drop food!", true, 23);

        mApp->mBoard->ShowText("Fish are hungry! Click to drop food!", true, 8);
        return false;
    }
    if (mHunger == -200 && mBeginner && !mApp->mBoard->mMessageShown[21])
    {
        mApp->mBoard->ShowText("Fish are REALLY hungry! Click to feed!", true, 21);
        mApp->mBoard->PlaySample(SOUND_AWOOGA_ID, 3, 1.0);
        return false;
    }
    if (mHunger == -400 && mBeginner)
    {
        mApp->mBoard->ShowText("Fish are about to die of hunger! Click on aquarium!", true, 22);
        return false;
    }
    if (mHunger <= -500 && mBeginner)
    {
        Die(true);
        return false;
    }
    if (mHunger >= 500)
        return false;
    if (mApp->mBoard->AliensInTank())
    {
        if (anAlienList->empty() || anAlienList->front()->mAlienType == ALIEN_GUS)
            return false;
    }
    return HungryBehavior();
}

void Sexy::Fish::DrawFish(Graphics* g, bool mirror)
{
    int aRow = mSize;
    if (mSize > 2)
    {
        if (mSize == 3)
            aRow = 2;
        else
        {
            aRow = 3;
            if (mSize != 4)
                aRow = mirror;
        }
    }
    if (gZombieMode)
    {
        bool aHungr = IsHungryVisible();
        g->DrawImageMirror(IMAGE_SMALLDIE, 0, 0, Rect((aHungr ? 6 : 9) * 80, aRow * 80, 80, 80), mirror);
        if (mMisslePtr)
            DrawCrosshair(g, 0, 0);
        if (IsHungryBlipPointer(500))
            g->DrawImageCel(IMAGE_MISCITEMS, 0, -5, 2);
        return;
    }

    bool aHungr = IsHungryVisible();
    Image* anImg = GetImageToDraw(aHungr);
    Rect aSrcRect = Rect(mAnimationFrameIndexFish * 80, aRow * 80, 80, 80);
    Rect aDestRect = Rect(0, 0, 80, 80);
    if (mInvisible && mGrowthAnimationTimer == 0)
        if(DrawInvisibleEffect(g, anImg, aSrcRect, mirror))
             return;
    if (mGrowthAnimationTimer > 0 && mSize != TYPE_STAR_GUPPY)
    {
        // Float constants, evaluated in double and stored in a float, as the original
        float aGrowthVal;
        if (mGrowthAnimationTimer > 3)
            aGrowthVal = (double)(10 - mGrowthAnimationTimer) * 0.7f / 7.0 + 0.5;
        else
            aGrowthVal = (double)mGrowthAnimationTimer * 0.2f / 3.0 + 1.0;

        int aVal = (int)(0.5 * ((aGrowthVal - 1.0) * 80.0));

        aDestRect.mX -= aVal;
        aDestRect.mY -= aVal;
        aDestRect.mWidth += aVal*2;
        aDestRect.mHeight += aVal*2;

        g->SetFastStretch(!mApp->Is3DAccelerated());
    }

    // Each branch turns colorizing off itself: the normal draw does it once more after the
    // hunger overlay only when that overlay is drawn, as the original
    if (mSize == TYPE_STAR_GUPPY)
    {
        SetColorHelper(g, Color(255, 255, 255, 155));
        DrawImageMirrorHelper(g, anImg, aDestRect, aSrcRect, mirror);
        if (!aHungr || mHungerAnimationTimer != 0)
        {
            g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
            SetColorHelper(g, Color(255, 255, 255, mHungerAnimationTimer != 0 ? ((5 - mHungerAnimationTimer) * 200) / 5 : 200));
            DrawImageMirrorHelper(g, anImg, aDestRect, aSrcRect, mirror);
            g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
        }
        g->SetColorizeImages(false);
    }
    else
    {
        if (mHasSpecialColors && !aHungr)
        {
            int anImgId;
            if (mTurnAnimationTimer != 0)
                anImgId = IMAGE_SIMTURN1_ID;
            else if (mEatingAnimationTimer > 0 || mVoraciousScreamCounter > 100)
                anImgId = IMAGE_SIMEAT1_ID;
            else
                anImgId = IMAGE_SIMSWIM1_ID;

            SetColorHelper(g, Color(255, 255, 255, 255));
            DrawImageMirrorHelper(g, GetImageById(anImgId), aDestRect, aSrcRect, mirror);

            g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
            SetColorHelper(g, mColors[1]);
            DrawImageMirrorHelper(g, GetImageById(anImgId + 1), aDestRect, aSrcRect, mirror);

            SetColorHelper(g, mColors[0]);
            DrawImageMirrorHelper(g, GetImageById(anImgId + 2), aDestRect, aSrcRect, mirror);
            g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
        }
        else
        {
            SetColorHelper(g, Color::White);
            DrawImageMirrorHelper(g, anImg, aDestRect, aSrcRect, mirror);
        }
        g->SetColorizeImages(false);
        if (mHungerAnimationTimer != 0)
        {
            Image* anImgtoDraw = GetImageToDraw(true);
            Color aClr = Color(255, 255, 255, (mHungerAnimationTimer * 255) / 5);
            SetColorHelper(g, aClr);
            DrawImageMirrorHelper(g, anImgtoDraw, aDestRect, aSrcRect, mirror);
            g->SetColorizeImages(false);
        }
    }

    if (mMisslePtr)
        DrawCrosshair(g, 0, 0);
    if (IsHungryBlipPointer(500))
        g->DrawImageCel(IMAGE_MISCITEMS, 0, -5, 2);
}

bool Fish::HungryBehavior()
{
    GameObject* aNearestFood = FindNearestFood();

    int aCenterX = mXD + 40.0;
    int aCenterY = mYD + 40.0;

    if (mExoticDietFoodType != 0)
        aCenterY += 20;

    if (mSpecialMovementStateChangeTimer >= 3)
    {
        if (!aNearestFood)
            return false;
        mSpecialMovementStateChangeTimer = 0;
        int aFCX = aNearestFood->mX + aNearestFood->mWidth / 2;
        int aFCY = aNearestFood->mY + aNearestFood->mHeight / 2;
        if (mHunger > 300)
        {
            if (aCenterX > aFCX + 8) {
                if (mVX > -3.0) mVX -= 1.0;
            }
            else if (aCenterX < aFCX - 8) {
                if (mVX < 3.0) mVX += 1.0;
            }
            else if (aCenterX > aFCX + 4) {
                if (mVX > -3.0) mVX -= 0.1;
            }
            else if (aCenterX < aFCX - 4) {
                if (mVX < 3.0) mVX += 0.1;
            }
            else if (aCenterX > aFCX) {
                if (mVX > -3.0) mVX -= 0.05;
            }
            else if (aCenterX < aFCX) {
                if (mVX < 3.0) mVX += 0.05;
            }

            if (aCenterY > aFCY + 6) {
                if (mVY > -2.0) mVY -= 0.6;
            }
            else if (aCenterY < aFCY - 6) {
                if (mVY < 3.0) mVY += 1.0;
            }
            else if (aCenterY > aFCY) {
                if (mVY > -2.0) mVY -= 0.3;
            }
            else if (aCenterY < aFCY) {
                if (mVY < 3.0) mVY += 0.5;
            }
        }
        else
        {
            if (aCenterX > aFCX + 8) {
                if (mVX > -4.0) mVX -= 1.3;
            }
            else if (aCenterX < aFCX - 8) {
                if (mVX < 4.0) mVX += 1.3;
            }
            else if (aCenterX > aFCX + 4) {
                if (mVX > -4.0) mVX -= 0.2;
            }
            else if (aCenterX < aFCX - 4) {
                if (mVX < 4.0) mVX += 0.2;
            }
            else if (aCenterX > aFCX) {
                if (mVX > -4.0) mVX -= 0.05;
            }
            else if (aCenterX < aFCX) {
                if (mVX < 4.0) mVX += 0.05;
            }

            if (aCenterY > aFCY + 6) {
                if (mVY > -3.0) mVY -= 1.0;
            }
            else if (aCenterY < aFCY - 6) {
                if (mVY < 4.0) mVY += 1.3;
            }
            else if (aCenterY > aFCY) {
                if (mVY > -3.0) mVY -= 0.5;
            }
            else if (aCenterY < aFCY) {
                if (mVY < 4.0) mVY += 0.7;
            }
        }
        if (mVXAbs < 5)
            mVXAbs++;
    }
    if (aNearestFood)
        CollideWithFood();
    return aNearestFood != nullptr;
}

GameObject* Fish::FindNearestFood()
{
    if (mExoticDietFoodType != 0)
        return FindNearestExoticFood(mX + mWidth/2, mY + mHeight/2 + 20);

    // The list is fetched once before the loop, as the original
    std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;
    int aDist = 100000000;
    Food* aRet = nullptr;
    for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
    {
        Food* aFood = *it;

        // The food's centre minus this one's, in double and truncated afterwards; the distance
        // is computed before the food is checked, as the original
        int ax = (int)((aFood->mX + 20) - (mXD + 40.0));
        int ay = (int)((aFood->mY + 20) - (mYD + 40.0));
        int aNewDist = ax * ax + ay * ay;
        if (CanEatFood(aFood) && aNewDist < aDist)
        {
            aDist = aNewDist;
            aRet = aFood;
        }
    }
    if (aDist < 10000)
    {
        VoraciousScream(150);
        mSpeedySpeedState = 100;
    }
    return aRet;
}

void Fish::CollideWithFood()
{
    if (mExoticDietFoodType != 0)
    {
        int aExoticFoodVal = ExoticFoodCollision(mX + mWidth / 2, mY + mHeight/2 + 20);
        if (mEatingAnimationTimer == 0)
        {
            if (aExoticFoodVal == 1)
            {
                ShowInvisibility();
                mEatingAnimationTimer = 8;
            }
            else if (aExoticFoodVal == 2)
            {
                ShowInvisibility();
                mEatingAnimationTimer = 20;
            }
        }
    }
    else
    {
        // The list is fetched once before the loop; the centre is compared untruncated, the
        // far bound first, as the original
        std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;
        for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
        {
            Food* aFood = *it;

            if (mXD + 40.0 < aFood->mX + 35 && mXD + 40.0 > aFood->mX + 5 &&
                mYD + 40.0 < aFood->mY + 35 && mYD + 40.0 > aFood->mY && CanEatFood(aFood))
            {
                OnFoodAte(aFood);
                aFood->RemoveFood();
                if (mEatingAnimationTimer == 0)
                {
                    ShowInvisibility();
                    mEatingAnimationTimer = 8;
                }
                return;
            }

            if (mEatingAnimationTimer == 0 && mXD + 40.0 < aFood->mX + 50 && mXD + 40.0 > aFood->mX - 10 &&
                mYD + 40.0 < aFood->mY + 40 && mYD + 40.0 > aFood->mY - 5 && CanEatFood(aFood))
            {
                ShowInvisibility();
                mEatingAnimationTimer = 20;
            }
        }
    }
}

void Fish::DropCoin()
{
    if (mSize <= 0 && mVirtualTankId < 0)
        return;

    mCoinDropTimer++;
    if (!CoinDropTimerPassed(mCoinDropTimer, gMerylActive ? 35 : mCoinDropT))
        return;

    mCoinDropTimer = 0;

    if (RelaxModeCanDrop())
    {
        // Rocky always drops a treasure chest and Santa once grown; a virtual fish maps its size
        // to a coin, any other fish drops the coin numbered by its size, as the original
        if (mPreNamedTypeId == ROCKY || mPreNamedTypeId == SANTA)
        {
            if (mPreNamedTypeId != SANTA || mSize >= TYPE_BIG_GUPPY)
                mApp->mBoard->DropCoin(mX + 5, mY + 10, 7, nullptr, -1.0, 0);
        }
        else if (mVirtualFish)
        {
            int aCoinType;
            switch (mSize)
            {
            case TYPE_MEDIUM_GUPPY:
                aCoinType = COIN_DIAMOND;
                break;
            case TYPE_BIG_GUPPY:
                aCoinType = COIN_PEARL;
                break;
            case TYPE_CROWNED_GUPPY:
                aCoinType = 7;
                break;
            default:
                aCoinType = COIN_GOLD_C;
                break;
            }
            mApp->mBoard->DropCoin(mX + 5, mY + 10, aCoinType, nullptr, -1.0, 0);
        }
        else if (mSize > 0)
            mApp->mBoard->DropCoin(mX + 5, mY + 10, mSize, nullptr, -1.0, 0);
    }

    if (mApp->mBoard->IsFirstLevel())
    {
        if (mApp->mBoard->mMessageShown[25])
            mApp->mBoard->ShowText("Click on coins for extra money!", true, 26);
        else if(mApp->mBoard->mMessageShown[5])
            mApp->mBoard->ShowText("Click on coins for extra money!", true, 25);
        else
            mApp->mBoard->ShowText("Click on coins for extra money!", true, 5);
    }
}

void Fish::DecrementEatingAnimationTimer()
{
    mEatingAnimationTimer--;
}

void Fish::FishUpdateAnimation()
{
    if (mRainbowFish)
        UpdateRainbow();
    if (mInvisible)
        UpdateInvisible();

    if (mPrevVX > 0 && mVX < 0)
        mTurnAnimationTimer = 20;
    else if (mPrevVX < 0 && mVX > 0)
        mTurnAnimationTimer = -20;

    // The eating timer only counts down while turning, or below while it sets the frame;
    // on the frame a turn ends it is decremented twice, as in the original.
    if (mTurnAnimationTimer > 0)
    {
        mTurnAnimationTimer--;
        if (mEatingAnimationTimer != 0)
            DecrementEatingAnimationTimer();
    }
    else if (mTurnAnimationTimer < 0)
    {
        mTurnAnimationTimer++;
        if (mEatingAnimationTimer != 0)
            DecrementEatingAnimationTimer();
    }

    if (mTurnAnimationTimer == 0)
    {
        if (mEatingAnimationTimer > 0)
        {
            DecrementEatingAnimationTimer();
            mAnimationFrameIndexFish = 9 - (mEatingAnimationTimer / 2);
        }
        else
        {
            if (mVXAbs <= 1)
                mSwimFrameCounter += 1;
            else
                mSwimFrameCounter += 2;

            if (mSwimFrameCounter >= 20)
                mSwimFrameCounter = 0;
            mAnimationFrameIndexFish = mSwimFrameCounter / 2;
        }
    }
    else if (mTurnAnimationTimer > 0)
        mAnimationFrameIndexFish = 9 - (mTurnAnimationTimer / 2);
    else if (mTurnAnimationTimer < 0)
        mAnimationFrameIndexFish = (mTurnAnimationTimer / 2) + 9;

    if (mGrowthAnimationTimer > 0)
        mGrowthAnimationTimer--;

    if (mVX != mPrevVX && mVX != 0 && mPrevVX != 0)
        mPrevVX = mVX;

    if (mVoraciousScreamCounter > 100 && mTurnAnimationTimer == 0)
        mAnimationFrameIndexFish = 4;

    if (mMisslePtr)
        UpdateCrosshairAnimation();
}

void Sexy::Fish::SetFishColors(int theRandIdx, bool rainbow)
{
    mHasSpecialColors = true;
    mRainbowFish = rainbow;
    // Each colour's components are computed blue first (the constructor's arguments, right to left)
    // and passed to the four-argument Color constructor with alpha 255, as the original
    if (mType == TYPE_OSCAR)
    {
        int aRandIndex = theRandIdx % 12;
        if (aRandIndex < 0)
            aRandIndex = -aRandIndex;
        int aBlue = OSCAR_BLUE_TABLE1[aRandIndex] + 5;
        if (aBlue > 255) aBlue = 255;
        int aGreen = OSCAR_GREEN_TABLE1[aRandIndex] + 5;
        if (aGreen > 255) aGreen = 255;
        int aRed = OSCAR_RED_TABLE1[aRandIndex] + 5;
        if (aRed > 255) aRed = 255;
        mColors[0] = Color(aRed, aGreen, aBlue, 255);

        aBlue = OSCAR_BLUE_TABLE2[aRandIndex] + 5;
        if (aBlue > 255) aBlue = 255;
        aGreen = OSCAR_GREEN_TABLE2[aRandIndex] + 5;
        if (aGreen > 255) aGreen = 255;
        aRed = OSCAR_RED_TABLE2[aRandIndex] + 5;
        if (aRed > 255) aRed = 255;
        mColors[1] = Color(aRed, aGreen, aBlue, 255);

        aBlue = OSCAR_BLUE_TABLE3[aRandIndex] + 5;
        if (aBlue > 255) aBlue = 255;
        aGreen = OSCAR_GREEN_TABLE3[aRandIndex] + 5;
        if (aGreen > 255) aGreen = 255;
        aRed = OSCAR_RED_TABLE3[aRandIndex] + 5;
        if (aRed > 255) aRed = 255;
        mColors[2] = Color(aRed, aGreen, aBlue, 255);
    }
    else
    {
        int aRandIndex = theRandIdx % 30;
        if (aRandIndex < 0)
            aRandIndex = -aRandIndex;
        int aBlue = FISH_BLUE_TABLE1[aRandIndex] + 5;
        if (aBlue > 255) aBlue = 255;
        int aGreen = FISH_GREEN_TABLE1[aRandIndex] + 5;
        if (aGreen > 255) aGreen = 255;
        int aRed = FISH_RED_TABLE1[aRandIndex] + 5;
        if (aRed > 255) aRed = 255;
        mColors[0] = Color(aRed, aGreen, aBlue, 255);

        aBlue = FISH_BLUE_TABLE2[aRandIndex] + 5;
        if (aBlue > 255) aBlue = 255;
        aGreen = FISH_GREEN_TABLE2[aRandIndex] + 5;
        if (aGreen > 255) aGreen = 255;
        aRed = FISH_RED_TABLE2[aRandIndex] + 5;
        if (aRed > 255) aRed = 255;
        mColors[1] = Color(aRed, aGreen, aBlue, 255);
    }
}

void Fish::RemoveFromGame(bool aRemoveShadow)
{
    if (mMisslePtr)
        mMisslePtr->RemoveMissle();

    mApp->mBoard->mWidgetManager->RemoveWidget(this);
    mApp->SafeDeleteWidget(this);
    mApp->mBoard->RemoveGameObjectFromLists(this, false);
    if (aRemoveShadow && mShadowPtr)
        mShadowPtr->RemoveShadow();
    mApp->mBoard->mGuppiesDeadCount++;
}

void Fish::Die(bool flag)
{
    if (flag)
        mApp->mBoard->PlayDieSound(mType);

    RemoveFromGame(false);
    bool isFacingRight = (mVX > 0.0);
    mApp->mBoard->SpawnDeadFish(mXD, mYD, mVX, mVY, mSpeedMod, mSize, isFacingRight, mShadowPtr);
}

void Fish::Init(int theX, int theY)
{
    mXD = theX;
    mYD = theY;
    mClip = false;
    mType = TYPE_GUPPY;
    mVirtualFish = false;
    mX = mXD; // converted back from the doubles, as the original
    mY = mYD;
    mVX = 0.0;
    mVY = -0.5;
    mWidth = 80;
    mHeight = 80;
    mPrevVX = 1.0;
    if ((mApp->mSeed->Next() & 1) == 0)
    {
        mVX = -0.1;
        mPrevVX = -1.0;
    }
    mXDirection = 1;
    m0x17c = 0;
    mYMax = 370;
    mYMin = 95;
    mXMin = 10;
    mXMax = 540;
    unsigned long aSpeedRand = mApp->mSeed->Next() % 3;
    if (aSpeedRand == 0)
        mSpeedMod = 2.0;
    else if (aSpeedRand == 1)
        mSpeedMod = 1.8;
    else
        mSpeedMod = 1.6;
    unsigned long aHungerRand = mApp->mSeed->Next() % 200; // drawn before mFoodAte is cleared, as the original
    mFoodAte = 0;
    mHunger = aHungerRand + 400;
    if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
    {
        mFoodNeededToGrow = Rand() % 9 + 12;
    }
    else
    {
        mFoodNeededToGrow = mApp->mSeed->Next() % 3 + 4;
    }
    unsigned long aMovementRand = mApp->mSeed->Next(); // drawn before the coin timer, as the original
    mSpecialMovementStateChangeTimer = 40;
    mMovementStateChangeTimer = 0;
    mSwimFrameCounter = 0;
    mVXAbs = 0;
    mAnimationFrameIndexFish = 0;
    mTurnAnimationTimer = 0;
    mEatingAnimationTimer = 0;
    mGrowthAnimationTimer = 0;
    mHasSpecialColors = false;
    mRainbowFish = false;
    mRainbowFishDeterminant = 0;
    mCoinDropTimer = 0;
    mMovementState = aMovementRand % 10;
    mCoinDropT = DetermineCoinDropT(mApp->mSeed->Next() % 200 + 150);
    mBeginner = false;
    mMouseVisible = true;
    if (mApp->mBoard != nullptr)
    {
        mBeginner = mApp->mBoard->IsFirstLevel();
        if (!mApp->mBoard->mAlienList->empty() || !mApp->mBoard->mBilaterusList->empty())
            mMouseVisible = false;
    }
    mBoughtTimer = 0;
    mWadsworthVXModCounter = 0;
    mUnusedTimer = 0;
    mUnusedVXWadsworthAddon = 0;
    mIsGuppy = 1;
}

void Fish::UpdateRainbow()
{
    if (mRainbowFishDeterminant == 0)
        mRainbowFishDeterminant = Rand() % 255 + 1;

    int aValue = mUpdateCnt + mRainbowFishDeterminant;
    int aHue = abs(aValue);
    
    // The first hue goes to mColors[1], the second to mColors[0], as the original
    Color aColor = Color(mApp->HSLToRGB(aHue % 255, 200, 128));
    mColors[1] = aColor;
    aColor = Color(mApp->HSLToRGB((aHue + 60) % 255, 200, 128));
    mColors[0] = aColor;
    aColor = Color(mApp->HSLToRGB((aHue + 120) % 255, 200, 128));
    mColors[2] = aColor;
}

bool Fish::CanEatFood(Food* theFood)
{
    if (theFood->mPickedUp != 0 || theFood->mCantEatTimer != 0)
        return false;
    if (theFood->mExoticFoodType != 2)
        return (theFood->mExoticFoodType == 0);
    else
    {
        if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
            return true;
        if (mSize <= TYPE_BIG_GUPPY && mHunger < 108)
            return true;
        return false;
    }
}

Image* Sexy::Fish::GetImageToDraw(bool theFlag)
{
    if (mTurnAnimationTimer != 0)
        return GetImageById(theFlag ? IMAGE_HUNGRYTURN_ID : IMAGE_SMALLTURN_ID);
    if(mEatingAnimationTimer <= 0 && mVoraciousScreamCounter <= 100)
        return GetImageById(theFlag ? IMAGE_HUNGRYSWIM_ID : IMAGE_SMALLSWIM_ID);

    return GetImageById(theFlag ? IMAGE_HUNGRYEAT_ID : IMAGE_SMALLEAT_ID);
}

void Sexy::Fish::ClickedBehavior(int theX, int theY)
{
    if (mTurnAnimationTimer != 0 || mEatingAnimationTimer != 0 || mSpecialMovementStateChangeTimer < 3)
        return;

    if (Rand() % 75 > 100) // Old code :)
        return;

    int aCenterX = theX - (mX + 40);
    int aCenterY = theY - (mY + 40);

    if (aCenterX * aCenterX + aCenterY * aCenterY >= 8100)
        return;

    mMovementState = 123;
    mSpecialMovementStateChangeTimer = 0;
    mMovementStateChangeTimer = 0;

    int aDirX;
    int aDirY;

    // Clicked at its front edge: turn around; otherwise keep going the same way
    if (mVX < 0.0 && theX < mX + 20)
        aDirX = 1;
    else if (mVX > 0.0 && theX > mX + mWidth - 20)
        aDirX = -1;
    else
        aDirX = (mVX < 0.0) ? -1 : 1;

    aDirY = (theY < mY + 40) ? 1 : -1;

    if (Rand() % 100 <= 25)
        aDirY = -aDirY;

    switch (Rand() % 2)
    {
    case 0:
        mVX = 4.0;
        break;
    case 1:
        mVX = 6.0;
        break;
    }
    switch (Rand() % 2)
    {
    case 0:
        mVY = 1.0;
        break;
    case 1:
        mVY = 3.0;
        break;
    }

    mVX = aDirX * mVX;
    mVY = aDirY * mVY;

    if (mPrevVX > 0.0 && mVX < 0.0)
        mTurnAnimationTimer = 20;
    else if (mPrevVX < 0.0 && mVX > 0.0)
        mTurnAnimationTimer = -20;

    mPrevVX = mVX;
}

void Sexy::Fish::FishOnGrow()
{
    // The tank, level and finished flag are kept in locals; mApp->mBoard is read afresh at each
    // use, as the original
    int aTank = mApp->mBoard->mTank;
    int aLevel = mApp->mBoard->mLevel;
    bool aFinishedGame = mApp->mCurrentProfile->mFinishedGame;
    if (mSize == 1)
    {
        if (!mApp->mBoard->mSlotUnlocked[SlotTypes::SLOT_GUPPY])
        {
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_GUPPY, true);
            if (aTank == 1 && aLevel == 1 && !aFinishedGame)
            {
                mApp->mBoard->mMessageShown[2] = true;
                if (mApp->mBoard->mMessageWidget->mMessageTimer <= 0)
                {
                    mApp->mBoard->ShowText("Your fish has grown! Good work!", false, 9);
                    return;
                }
            }
        }
    }
    else if (mSize == 2) // 33
    {
        if (mApp->mGameMode == GAMEMODE_TIME_TRIAL)
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_EGG, true);
        if (aTank == 1)
        {
            if (aLevel == 1)
            {
                if (!mApp->mBoard->mSlotUnlocked[SlotTypes::SLOT_EGG])
                {
                    mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_EGG, true);
                    mApp->mBoard->mMessageShown[2] = false;
                    mApp->mBoard->mMessageShown[40] = true;
                    if (!aFinishedGame && mApp->mBoard->mMessageWidget->mMessageTimer <= 0)
                    {
                        mApp->mBoard->ShowText("Buy 3 egg pieces to complete level!", false, 7);
                        return;
                    }
                }
            }
            else if (aLevel == 2)
            {
                if (!mApp->mBoard->mSlotUnlocked[SlotTypes::SLOT_FOODLVL])
                {
                    mApp->mBoard->mMessageShown[15] = true;
                    if (!aFinishedGame)
                        mApp->mBoard->ShowText("Upgrade Food Quality to make food more nourishing!", false, 11);
                    mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLVL, true);
                    return;
                }
            }
            else
            { //53
                mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLVL, true);
                mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLIMIT, true);
                mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_OSCAR, true);
            }
        }
        else if (aTank == 2)
        {
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLVL, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLIMIT, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_POTION, true);
            if (aLevel == 1)
            {
                mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_EGG, true);
                return;
            }
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_STARCATCHER, true);
            return;
        }
        else if (aTank == 3)
        {
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLVL, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLIMIT, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_GRUBBER, true);
            return;
        }
        else if (aTank == 4)
        {
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLVL, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_FOODLIMIT, true);
            mApp->mBoard->MakeAndUnlockMenuButton(SlotTypes::SLOT_OSCAR, true);
            return;
        }
    }
}

// A free cdecl function taking the object in the original (0x500FF0)
bool Sexy::Fish::WadsworthActive(GameObject* theObject)
{
    if (gWadsworthTimer != 0 && theObject->mType == TYPE_GUPPY)
        return ((Fish*)theObject)->mSize < SIZE_LARGE;
    return false;
}


