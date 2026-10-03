#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/SexyVector.h>

#include "Missle.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Bilaterus.h"
#include "BilaterusHead.h"
#include "Fish.h"
#include "Oscar.h"
#include "Ultra.h"
#include "Gekko.h"
#include "Penta.h"
#include "Grubber.h"
#include "Breeder.h"
#include "OtherTypePet.h"
#include "FishTypePet.h"
#include "Res.h"

Sexy::Missle::Missle()
{
    mTarget = 0;
    m0x1a8 = 0;
    mClip = false;
    mType = TYPE_MISSLE;
}

Sexy::Missle::Missle(int x, int y, GameObject* theTarget, int theType)
{
    mXD = x;
    mType = TYPE_MISSLE;
    mYD = y;
    mClip = false;
    mMissleType = theType;
    mTarget = theTarget;
    mX = mXD;
    mY = mYD;
    mVX = 0;
    mVY = 0;
    mWidth = 80;
    mHeight = 80;
    m0x194 = 1;
    m0x198 = 1;
    m0x1a8 = 0;
    m0x1ac = 10;
    m0x17c = 15;
    m0x180 = 0;
    m0x1a0 = Rand() % 4;
    mMouseVisible = false;
    if (mMissleType == MISSLE_BALL)
    {
        mSpeedMod = 0.4;
        mWidth = 50;
        mHeight = 50;
    }
    else
        mSpeedMod = 0.8;
    m0x190 = false;
    bool flag = IsTargetless();
    if (flag)
    {
        // Both values are drawn before either velocity is set, as the original
        int aRandVal = mApp->mSeed->Next() % 6;
        int aRandValY = mApp->mSeed->Next() % 6;
        if (aRandVal == 0)
            mVX = -6;
        else if (aRandVal == 1)
            mVX = -5;
        else if (aRandVal == 2)
            mVX = -4;
        else if (aRandVal == 3)
            mVX = 4;
        else if (aRandVal == 4)
            mVX = 5;
        else if (aRandVal == 5)
            mVX = 6;

        if (aRandValY == 0)
            mVY = -6;
        else if (aRandValY == 1)
            mVY = -5;
        else if (aRandValY == 2)
            mVY = -4;
        else if (aRandValY == 3)
            mVY = 4;
        else if (aRandValY == 4)
            mVY = 5;
        else if (aRandValY == 5)
            mVY = 6;

        m0x190 = true;
    }
}

void Sexy::Missle::Update()
{
    if (mApp->mBoard == nullptr || mApp->mBoard->mPause)
        return;

    UpdateCounters();
    if (m0x1a8 > 0)
    {
        m0x1a8++;
        if (m0x1a8 >= m0x1ac)
        {
            RemoveMissle();
            return;
        }
    }

    if (ChaseTarget()) return;
    
    if (mMissleType == MISSLE_CLASSIC || (mMissleType == MISSLE_ENERGYBALL && !m0x190))
    {
        if (mXD > 550.0)
            mXD = 550.0;
        if (mXD < 10.0)
            mXD = 10.0;
        if (mYD > 370.0)
            mYD = 370.0;
        if (mYD < 95.0)
            mYD = 95.0;
    }
    else
    {
        if ((mMissleType == MISSLE_ENERGYBALL && m0x190) || (mMissleType == MISSLE_BALL || IsTargetless()))
        {
            if (mXD > 580.0 || mXD < -20.0 || mYD > 380.0 || mYD < 45.0)
            {
                RemoveMissle();
                return;
            }
        }
    }

    if (mMissleType == MISSLE_BALL && m0x190)
        mVY += 0.2;

    mXD = mVX / mSpeedMod + mXD;
    mYD = mVY / mSpeedMod + mYD;
    if (m0x17c > 0)
        m0x17c--;
    Move(mXD, mYD);

    if (mMissleType == MISSLE_CLASSIC && mApp->mSeed->Next() % 3 == 0)
    {
        // Integer velocities, x jitter drawn before y jitter, as the original
        int anXJitter = mApp->mSeed->Next() % 5;
        int aY = mY - 16 * (int)mVY + mApp->mSeed->Next() % 5 + 17;
        mApp->mBoard->SpawnShot(mX - 20 * (int)mVX + anXJitter + 17, aY, 8);
    }
    UpdateAnimations();
    MarkDirty();
}

void Sexy::Missle::Draw(Graphics* g)
{
    UpdateFishSongMgr();
    bool doMirror = (mMissleType == MISSLE_ENERGYBALL || mMissleType == MISSLE_BALL || IsTargetless());
    DrawHelper(g, doMirror);
}

void Sexy::Missle::VFT74()
{
    if (mTarget)
    {
        mTarget->mMisslePtr = nullptr;
        mTarget = nullptr;
    }
    if (!IsTargetless())
        mApp->mBoard->Unk12();
}

void Sexy::Missle::Remove()
{
    RemoveMissle();
}

void Sexy::Missle::RemoveMissle()
{
    if (mTarget != nullptr)
    {
        mTarget->mMisslePtr = nullptr;
        mTarget = nullptr;
    }
    mApp->mBoard->mWidgetManager->RemoveWidget(this);
    mApp->SafeDeleteWidget(this);
    mApp->mBoard->RemoveGameObjectFromLists(this, false);
    if (!IsTargetless())
        mApp->mBoard->Unk12();
    
    if (mMissleType != MISSLE_BALL)
    {
        if (mMissleType == MISSLE_ENERGYBALL)
            mApp->mBoard->PlaySample(SOUND_EXPLOSION4_ID, 3, 1.0);
        mApp->mBoard->PlaySample(SOUND_EXPLODE_ID, 3, 1.0);
        for (int i = mApp->mSeed->Next() % 3 + 2; i > 0; i--)
        {
            int aType = mApp->mSeed->Next() % 3 + 3;
            int aRandY = mApp->mSeed->Next() % 30 - 10 + mY;
            int aRandX = mApp->mSeed->Next() % 30 - 10 + mX;
            mApp->mBoard->SpawnShot(aRandX, aRandY, aType);
        }
    }
}

void Sexy::Missle::OnFoodAte(GameObject* theObj)
{
    // The original ignores the argument and acts on the target
    mTarget->mMisslePtr = nullptr;
    GameObject* obj = mTarget; // read again after the store, as the original
    if (obj->mType == TYPE_PENTA)
        ((Penta*)obj)->Die(true);
    else if (obj->mType == TYPE_GRUBBER)
        ((Grubber*)obj)->Die(true);
    else if (obj->mType == TYPE_OSCAR || obj->mType == TYPE_GEKKO || obj->mType == TYPE_ULTRA)
        ((Fish*)obj)->Die(true);
    else if (obj->mType == TYPE_BREEDER)
        ((Breeder*)obj)->Die(true);
    else if (obj->mType == TYPE_OTHER_TYPE_PET)
        ((OtherTypePet*)obj)->RemoveOtherTypePet();
    else if (obj->mType == TYPE_FISH_TYPE_PET)
        ((FishTypePet*)obj)->RemoveFishTypePet();
    else if (obj->mType == TYPE_GUPPY) // tested last in the original (0x4F9441)
        ((Fish*)obj)->Die(true);
    RemoveMissle();
}

void Sexy::Missle::Sync(DataSync* theSync)
{
    GameObject::Sync(theSync);
    theSync->SyncDouble(mXD);
    theSync->SyncDouble(mYD);
    theSync->SyncDouble(mVX);
    theSync->SyncDouble(mVY);
    theSync->SyncLong(m0x178);
    theSync->SyncLong(m0x17c);
    theSync->SyncLong(m0x180);
    theSync->SyncDouble(mSpeedMod);
    theSync->SyncBool(m0x190);
    theSync->SyncLong(m0x194);
    theSync->SyncLong(m0x198);
    theSync->SyncLong(mMissleType);
    theSync->SyncLong(m0x1a0);
    theSync->SyncLong(m0x1a8);
    if (m0x1a8 > 0)
    {
        theSync->SyncLong(m0x1ac);
        theSync->SyncLong(m0x1b0);
        theSync->SyncLong(m0x1b4);
    }
    theSync->SyncPointer((void**) &mTarget);
}

bool Sexy::Missle::IsTargetless()
{
    if (mMissleType == MISSLE_BONE || mMissleType == MISSLE_HEAD1 || mMissleType == MISSLE_HEAD2)
        return true;
    return false;
}

void Sexy::Missle::UpdateAnimations()
{
    if (mMissleType == MISSLE_ENERGYBALL)
        m0x194 = (m0x194 + 1) % 5;
    else if (mMissleType == MISSLE_BALL)
    {
        if (m0x190)
        {
            if (mVX < 0.0)
                m0x180 = (m0x180 + 1) % 20;
            else
            {
                m0x180 = (m0x180 - 1);
                if (m0x180 < 0) m0x180 += 20;
            }
        }
        else
        {
            if (mVX < 0.0)
                m0x180 = (m0x180 + 2) % 20;
            else
            {
                m0x180 = (m0x180 - 2);
                if (m0x180 < 0) m0x180 += 20;
            }
        }

        m0x194 = m0x180 / 2;
    }
    else
    {
        if (IsTargetless())
        {
            m0x194 = (m0x194 + 1) % 10;
            if (m0x194 == 0)
                m0x198++;
        }
        else
        { // 31
            if (mVX < 0.8 && mVX > -0.8)
            {
                if (mVY > 0.0)
                    m0x194 = 12;
                else
                    m0x194 = 4;
            }
            else if (mVX > 0.0)
            {
                if (mVY > 2.5)          m0x194 = 12;
                else if (mVY < -2.5)    m0x194 = 4;
                else if (mVY > 2.0)     m0x194 = 13;
                else if (mVY < -2.0)    m0x194 = 3;
                else if (mVY > 1.5)     m0x194 = 14;
                else if (mVY < -1.5)    m0x194 = 2;
                else if (mVY > 1.0)     m0x194 = 15;
                else if (mVY < -1.0)    m0x194 = 1;
                else                    m0x194 = 0;
            }
            else
            {
                if (mVY > 2.5)          m0x194 = 12;
                else if (mVY < -2.5)    m0x194 = 4;
                else if (mVY > 2.0)     m0x194 = 11;
                else if (mVY < -2.0)    m0x194 = 5;
                else if (mVY > 1.5)     m0x194 = 10;
                else if (mVY < -1.5)    m0x194 = 6;
                else if (mVY > 1.0)     m0x194 = 9;
                else if (mVY < -1.0)    m0x194 = 7;
                else                    m0x194 = 8;
            }
        }
    }
}

void Sexy::Missle::DrawHelper(Graphics* g, bool flag)
{
    if (mMissleType == MISSLE_ENERGYBALL)
    {
        g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
        g->SetColorizeImages(true);
        if (m0x190)
            g->SetColor(Color(175, 175, 50, 255));
        else
            g->SetColor(Color(100, 100, 255, 55));
        g->DrawImageCel(IMAGE_ENERGYBALL, 0, 0, m0x194);
        g->SetColor(Color(100, 100, 255, 255));
        g->DrawImageCel(IMAGE_ENERGYBALL, 0, 0, 5);
        g->SetColorizeImages(false);
        g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
    }
    else if (mMissleType == MISSLE_BALL)
    {
        g->DrawImageCel(IMAGE_BALLS, 0, 0, m0x194, m0x1a0);
    }
    else if (mMissleType == MISSLE_BONE)
    {
        g->DrawImage(IMAGE_BILATERUS, 0, 0, Rect(m0x194 * 80, 480, 80, 80));
    }
    else if (mMissleType == MISSLE_HEAD1 || mMissleType == MISSLE_HEAD2)
    {
        g->DrawImageMirror(IMAGE_BILATERUS, 0, 0, Rect(m0x194 * 80, (mMissleType == MISSLE_HEAD1 ? 4 : 1) * 80, 80, 80), (m0x198 % 2) == 0);
    }
    else
    {
        g->DrawImage(IMAGE_MISSILE, 0, 0, Rect(m0x194 * 80, 0, 80, 80));
    }
    if (m0x1a8 > 0)
    {
        int aTargetX = mX + mWidth / 2;
        int aTargetY = mY + mHeight / 2;

        int anInterpolatedX = InterpolateInt(m0x1b0, aTargetX, m0x1a8, m0x1ac, false);
        int anInterpolatedY = InterpolateInt(m0x1b4, aTargetY, m0x1a8, m0x1ac, false);

        // The original normalizes (target - interpolated), projects 50 px ahead (clamped to the
        // target) and builds a 3 px wide quad from these points, but never draws the quad:
        // only the ball at the interpolated position is drawn.
        SexyVector3 aDir(aTargetX - anInterpolatedX, aTargetY - anInterpolatedY, 0);
        aDir = aDir.Normalize(); // out of line in the original (0x4D7F20)
        int aPerpX = (int)(aDir.y * 3.0);
        int aPerpY = (int)(3.0 * -aDir.x);

        float aStartX = anInterpolatedX;
        float anEndX = aDir.x * 50.0 + aStartX;
        float aStartY = anInterpolatedY;
        float anEndY = aDir.y * 50.0 + aStartY;
        if ((aTargetX > anInterpolatedX && aTargetX < anEndX) || (aTargetX < anInterpolatedX && aTargetX > anEndX) ||
            (aTargetY > anInterpolatedY && aTargetY < anEndY) || (aTargetY < anInterpolatedY && aTargetY > anEndY))
        {
            anEndX = aTargetX;
            anEndY = aTargetY;
        }

        int aDrawX = (int)(aStartX - g->mTransX);
        int aDrawY = (int)(aStartY - g->mTransY);
        anEndX -= g->mTransX;
        anEndY -= g->mTransY;
        Point aQuad[4] = {
            Point(aDrawX + aPerpX, aDrawY + aPerpY),
            Point((int)(anEndX + aPerpX), (int)(anEndY + aPerpY)),
            Point((int)(anEndX - aPerpX), (int)(anEndY - aPerpY)),
            Point(aDrawX - aPerpX, aDrawY - aPerpY)
        };

        g->DrawImageCel(IMAGE_BALLS, aDrawX - 25, aDrawY - 25, m0x1a8 % 10, m0x1a0);
    }
}

bool Sexy::Missle::ChaseTarget()
{
    if ((mTarget == nullptr || mTarget->mMisslePtr == nullptr) && mMissleType != MISSLE_BALL)
    {
        if (!IsTargetless())
        {
            RemoveMissle();
            return true;
        }
        if (mTarget != nullptr)
            return CheckCollision();
        return false;
    }

    if (m0x190)
    {
        if (mTarget != nullptr)
            return CheckCollision();
        return false;
    }

    if (mMissleType == MISSLE_BALL && mApp->mBoard->mBilaterusList->empty() && !mApp->mBoard->AliensInTank())
    {
        if (mTarget != nullptr)
            return CheckCollision();
        return false;
    }

    // Two conditions in the original: the type is tested again here (0x4FF553)
    int aHalfWidth;
    if (mMissleType == MISSLE_BALL && mApp->mBoard->mBilaterusList->empty())
        aHalfWidth = 25;
    else
        aHalfWidth = 40;

    if (mTarget->mType == TYPE_ULTRA || (mMissleType == MISSLE_BALL && mApp->mBoard->mBilaterusList->empty()))
    {
        int aTX = mTarget->mX;
        double aMisCX = mXD + aHalfWidth;
        if (aTX + 200 < aMisCX)
        {
            if (mVX > -1.8)
                mVX -= 0.2;
        }
        else if (aTX - 40 > aMisCX)
        {
            if (mVX < 1.8)
                mVX += 0.2;
        }
        else if (aTX + 110 < aMisCX)
        {
            if (mVX > -1.4)
                mVX -= 0.2;
        }
        else if (aTX + 50 > aMisCX)
        {
            if (mVX < 1.4)
                mVX += 0.2;
        }
        else if (aTX + 80 < aMisCX)
        {
            if (mVX > -0.6)
                mVX -= 0.2;
        }
        else if (aTX + 80 > aMisCX)
        {
            if (mVX < 0.6)
                mVX += 0.2;
        }

        int aTY = mTarget->mY;
        double aMisCY = mYD + aHalfWidth;
        if (aTY + 200 < aMisCY)
        {
            if (mVY > -1.8)
                mVY -= 0.2;
        }
        else if (aTY - 40 > aMisCY)
        {
            if (mVY < 1.8)
                mVY += 0.2;
        }
        else if (aTY + 110 < aMisCY)
        {
            if (mVY > -1.4)
                mVY -= 0.2;
        }
        else if (aTY + 50 > aMisCY)
        {
            if (mVY < 1.4)
                mVY += 0.2;
        }
        else if (aTY + 80 < aMisCY)
        {
            if (mVY > -0.2)
                mVY -= 0.1;
        }
        else if (aTY + 80 > aMisCY)
        {
            if (mVY < 0.2)
                mVY += 0.1;
        }
    }
    else
    {
        // 156
        int aTX = mTarget->mX;
        double aMisCX = mXD + 40.0;
        if (aTX + 80 < aMisCX)
        {
            if (mVX > -1.8)
                mVX -= 0.2;
        }
        else if (aTX > aMisCX)
        {
            if (mVX < 1.8)
                mVX += 0.2;
        }
        else if (aTX + 60 < aMisCX)
        {
            if (mVX > -1.4)
                mVX -= 0.2;
        }
        else if (aTX + 20 > aMisCX)
        {
            if (mVX < 1.4)
                mVX += 0.2;
        }
        else if (aTX + 40 < aMisCX)
        {
            if (mVX > -0.6)
                mVX -= 0.2;
        }
        else if (aTX + 40 > aMisCX)
        {
            if (mVX < 0.6)
                mVX += 0.2;
        }

        int aTY = mTarget->mY;
        double aMisCY = mYD + 40.0;
        if (aTY + 80 < aMisCY)
        {
            if (mVY > -1.8)
                mVY -= 0.2;
        }
        else if (aTY > aMisCY)
        {
            if (mVY < 1.8)
                mVY += 0.2;
        }
        else if (aTY + 60 < aMisCY)
        {
            if (mVY > -1.4)
                mVY -= 0.2;
        }
        else if (aTY + 20 > aMisCY)
        {
            if (mVY < 1.4)
                mVY += 0.2;
        }
        else if (aTY + 40 < aMisCY)
        {
            if (mVY > -0.2)
                mVY -= 0.1;
        }
        else if (aTY + 40 > aMisCY)
        {
            if (mVY < 0.2)
                mVY += 0.1;
        }
    }
    

    if (mTarget != nullptr)
        return CheckCollision();
    return false;
}

bool Sexy::Missle::CheckCollision()
{
    if (mMissleType == MISSLE_ENERGYBALL && m0x190 && !mApp->mRelaxMode)
    {
        // Iterator loops in the original, dereferencing the iterator at every access
        for (std::vector<Alien*>::iterator it = mApp->mBoard->mAlienList->begin(); it != mApp->mBoard->mAlienList->end(); ++it)
        {
            if (mXD + 40.0 < (*it)->mX + 140 && mXD + 40.0 > (*it)->mX + 30
                && mYD + 40.0 < (*it)->mY + 150 && mYD + 40.0 > (*it)->mY + 10 && !(*it)->mIsPsychosquidHealing)
            {
                (*it)->mHealth = (*it)->mHealth - 30.0; // two dereferences, one to read and one to write (0x4FC1F8, 0x4FC211)
                (*it)->mHitFlashTimer = 10;
                if ((*it)->mHealth <= 0.0)
                    (*it)->Remove(true);

                RemoveMissle();
                return true;
            }
        }

        for (std::vector<Bilaterus*>::iterator it = mApp->mBoard->mBilaterusList->begin(); it != mApp->mBoard->mBilaterusList->end(); ++it)
        {
            if (mXD + 25.0 < (*it)->mActiveHead->mX + 70 && mXD + 25.0 > (*it)->mActiveHead->mX + 10
                && mYD + 25.0 < (*it)->mActiveHead->mY + 70 && mYD + 25.0 > (*it)->mActiveHead->mY + 10)
            {
                (*it)->mActiveHead->m0x1d8 = (*it)->mActiveHead->m0x1d8 - 30.0; // two iterator checks (0x4FC3B9, 0x4FC3C3)
                (*it)->mActiveHead->m0x1d0 = 10;
                if ((*it)->mActiveHead->m0x1d8 <= 0.0)
                    (*it)->mActiveHead->Die(true);

                RemoveMissle();
                return true;
            }
        }

        // The original reads Cyrax from the board at every access here
        if (mApp->mBoard->mCyraxPtr)
        {
            if (mXD + 40.0 < mApp->mBoard->mCyraxPtr->mX + 140 && mXD + 40.0 > mApp->mBoard->mCyraxPtr->mX + 30 &&
                mYD + 40.0 < mApp->mBoard->mCyraxPtr->mY + 150 && mYD + 40.0 > mApp->mBoard->mCyraxPtr->mY + 10)
            {
                mApp->mBoard->mCyraxPtr->mHealth -= 30.0;
                mApp->mBoard->mCyraxPtr->mHitFlashTimer = 10;
                // A killing blow here is a latent bug of the original: Cyrax's CyraxDieCleanup
                // already passes this ball (still in mMissleList1) to SafeDeleteWidget through
                // RemoveHelper02, and RemoveMissle below queues it a second time (Alien::Remove,
                // then the shared RemoveMissle call, as in the original)
                if (mApp->mBoard->mCyraxPtr->mHealth <= 0.0)
                    mApp->mBoard->mCyraxPtr->Remove(true);
                RemoveMissle();
                return true;
            }

            for (std::vector<FishTypePet*>::iterator it = mApp->mBoard->mFishTypePetList->begin(); it != mApp->mBoard->mFishTypePetList->end(); ++it)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->RemoveFishTypePet();
                    mApp->mBoard->PlayDieSound(-1);
                    break;
                }
            }
            for (std::vector<OtherTypePet*>::iterator it = mApp->mBoard->mOtherTypePetList->begin(); it != mApp->mBoard->mOtherTypePetList->end(); ++it)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->RemoveOtherTypePet();
                    mApp->mBoard->PlayDieSound(-1);
                    return false;
                }
            }
            return false;
        }

        // Without Cyrax in the tank, a deflected energy ball hits the player's fish instead
        for (std::vector<Fish*>::iterator it = mApp->mBoard->mFishList->begin(); it != mApp->mBoard->mFishList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (gWadsworthTimer != 0 && (*it)->mSize < SIZE_LARGE)
                    continue;

                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Oscar*>::iterator it = mApp->mBoard->mOscarList->begin(); it != mApp->mBoard->mOscarList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Penta*>::iterator it = mApp->mBoard->mPentaList->begin(); it != mApp->mBoard->mPentaList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Ultra*>::iterator it = mApp->mBoard->mUltraList->begin(); it != mApp->mBoard->mUltraList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 150 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 150 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Grubber*>::iterator it = mApp->mBoard->mGrubberList->begin(); it != mApp->mBoard->mGrubberList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Gekko*>::iterator it = mApp->mBoard->mGekkoList->begin(); it != mApp->mBoard->mGekkoList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }

        for (std::vector<Breeder*>::iterator it = mApp->mBoard->mBreederList->begin(); it != mApp->mBoard->mBreederList->end(); ++it)
        {
            if ((*it)->mVirtualTankId < 0)
            {
                if (mXD + 40.0 < (*it)->mX + 70 && mXD + 40.0 > (*it)->mX + 10 &&
                    mYD + 40.0 < (*it)->mY + 70 && mYD + 40.0 > (*it)->mY + 10)
                {
                    (*it)->Die(true);
                    break;
                }
            }
        }
        return false;
    }

    if (mMissleType == MISSLE_BALL)
    { //307
        if (!m0x190)
        {
            bool hit = false;
            double aCX = mXD + 25.0;
            double aCY = mYD + 25.0;
            if (mTarget->mType == TYPE_BILATERUS)
            {
                if(aCX < mTarget->mX + 70 && aCX > mTarget->mX + 10 && 
                    aCY < mTarget->mY + 70 && aCY > mTarget->mY + 10)
                    hit = true;
            }
            else
            {
                if (aCX < mTarget->mX + 140 && aCX > mTarget->mX + 20 &&
                    aCY < mTarget->mY + 140 && aCY > mTarget->mY + 20)
                    hit = true;
            }

            if (hit)
            {
                GameObject* aTar = mTarget;
                m0x190 = true;
                mTarget->mMisslePtr = nullptr;
                mTarget = nullptr;

                int aDamage = 55;
                if (m0x1a0 < 4)
                {
                    mApp->mBoard->PlaySample(SOUND_SFX_ID, 3, 1.0);
                    mSpeedMod = 1.5;
                    aDamage = 15;
                }
                else
                {
                    mApp->mBoard->PlayPunchSound(3);
                    mSpeedMod = 0.6;
                }

                mVX = -mVX;
                mVY = -mVY;
                if (aTar->mType == TYPE_BILATERUS)
                {
                    Bilaterus* aBil = (Bilaterus*)aTar;
                    if (aBil->mActiveHead)
                    {
                        aBil->mActiveHead->m0x1d8 -= aDamage;
                        if (aBil->mActiveHead->m0x1d8 <= 0.0)
                        {
                            aBil->mActiveHead->Die(true);
                            return false;
                        }
                    }
                }
                else if (aTar->mType == TYPE_ALIEN)
                {
                    Alien* anAlien = (Alien*)aTar;
                    if (!anAlien->mIsPsychosquidHealing)
                    {
                        anAlien->mHealth -= aDamage;
                        if (anAlien->mHealth <= 0.0)
                        {
                            anAlien->Remove(true);
                            return false;
                        }
                    }
                }
                return false;
            }
        }
    } // 403
    else if (!IsTargetless()) // 407
    {
        bool hit = false;
        if (mTarget->mType == TYPE_ULTRA)
        {
            if (mXD + 40.0 < mTarget->mX + 140 && mXD + 40.0 > mTarget->mX + 8 &&
                mYD + 40.0 < mTarget->mY + 140 && mYD + 40.0 > mTarget->mY + 4)
                hit = true;
        }
        else
        {
            if (mXD + 40.0 < mTarget->mX + 70 && mXD + 40.0 > mTarget->mX + 10 &&
                mYD + 40.0 < mTarget->mY + 70 && mYD + 40.0 > mTarget->mY + 10)
                hit = true;
        }
        if (hit)
        {
            OnFoodAte(mTarget);
            return true;
        }
    }
    return false;
}

bool Sexy::Missle::Shot(int x, int y)
{
    if (mMissleType == MISSLE_ENERGYBALL)
    {
        if (x > mXD + 10.0 && x < mXD + 70.0 && y > mYD + 10.0 && y < mYD + 70.0)
        {
            if (m0x17c < 1)
            {
                mVX = ((x - mXD) - 40.0) / -5.0;
                mVY = ((y - mYD) - 40.0) / -5.0;
                if (mVX < 1.0 && mVX >= 0.0)
                    mVX = 1.0;
                if (mVX > -1.0 && mVX <= 0.0)
                    mVX = -1.0;
                if (mVY < 1.0 && mVY >= 0.0)
                    mVY = 1.0;
                if (mVY > -1.0 && mVY <= 0.0)
                    mVY = -1.0;
                m0x190 = 1;
            }
            mApp->mBoard->SpawnShot(x - 40, y - 40, 2);
            return true;
        }
    }
    else if (mMissleType == MISSLE_CLASSIC)
    {
        double aCX = mXD + 40.0;
        double aCY = mYD + 40.0;
        if (x > aCX - 30.0 && x < aCX + 30.0 && y > aCY - 30.0 && y < aCY + 30.0)
        {
            if (m0x17c < 1)
            {
                RemoveMissle();
                mApp->mBoard->SpawnShot(x - 40, y - 40, 2);
                return true;
            }
        }
    }
    return false;
}
