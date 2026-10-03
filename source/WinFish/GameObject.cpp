#include <SexyAppFramework/SexyAppBase.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/MemoryImage.h>

#include "GameObject.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Food.h"
#include "Fish.h"
#include "Missle.h"
#include "Shadow.h"
#include "Oscar.h"
#include "Breeder.h"
#include "Coin.h"
#include "SimFishScreen.h"
#include "Res.h"

#include <ctime>

using namespace Sexy;

const double PI = 3.141590118408203;

GameObject::GameObject()
{
	mApp = (WinFishApp*)gSexyApp;
	mCrosshairAnimationTimer = 0;
	mShadowPtr = NULL;
	mMisslePtr = NULL;
	mHunger = 0;
	mHungerShown = 0;
	mHungerAnimationTimer = 0;
	m0xd8 = 0;
	mHometownIdx = 0;
	mExoticDietFoodType = 0;
	mForwardlyChallenged = false;
	mInvisible = false;
	mInvisibleTimer = 0;
	mSpeedy = false;
	mSpeedySpeedState = 0;
	mVoracious = false;
	mVoraciousScreamCounter = 0;
	m0x10c = 0;
	mSinging = false;
	m0x114 = 0;
	m0x118 = 0;
	mType = TYPE_DEFAULT;
	mHungerShowTreshold = 300;
	mShown = true;
	mTodayBought = true;
	mVirtualTankId = -1;
	mSongId = -1;
	mLikes[0] = 0;
	mLikes[1] = 0;
	mLikes[2] = 0;
	mStoreInvisibilityTimer = 0;
	mStoreAnimationTimer = 0;
	mStoreAnimationIndex = 0;
	mCanBeEatenDelay = 0;
	mBoughtTimeDaySecs = 0;
	mLastMentalStateUpdateTime = 0;
	mTimesFedToday = 0;
	mRandomHappiestMentalId = 0;
	mPreNamedTypeId = -1;
	mMentalState = 3;
}

Sexy::GameObject::~GameObject()
{
	if (mApp->mBoard != nullptr && mSongId != -1)
		mApp->mBoard->StopFishSong(mSongId);
}

void Sexy::GameObject::Update()
{
	Widget::Update();
}

void Sexy::GameObject::UpdateF(float theFrac)
{
}

void Sexy::GameObject::Draw(Graphics* g)
{
}

void Sexy::GameObject::CountRequiredFood(int* theFoodReqPtr)
{
}

int Sexy::GameObject::SpecialReturnValue()
{
	return 0;
}

int Sexy::GameObject::GetShellPrice()
{
	// A fish bought within the last day cannot be sold back.
	if (mTodayBought && gUnkInt01 <= 0 && _time64(NULL) - mTimeBought <= 86400)
		return -1;

	return mShellPrice / 2;
}

void Sexy::GameObject::PrestoMorph(int thePetId)
{
}

void Sexy::GameObject::VFT74()
{
}

void GameObject::Remove()
{

}

void GameObject::SetPosition(int newX, int newY)
{
}

void GameObject::OnFoodAte(GameObject* obj)
{
}

void GameObject::UpdateStoreAnimation()
{
}

void GameObject::DrawStoreAnimation(Graphics* g, int justification)
{

}

void Sexy::GameObject::Sync(DataSync* theSync)
{
	theSync->RegisterPointer(this);
	theSync->SyncLong(mType);
	theSync->SyncLong(mCrosshairAnimationTimer);
	theSync->SyncPointer((void**)&mMisslePtr);
	theSync->SyncLong(mHunger);
	theSync->SyncLong(mHungerAnimationTimer);
	theSync->SyncLong(mHungerShowTreshold);
	theSync->SyncBool(mHungerShown);
	theSync->SyncByte(mCanBeEatenDelay);
	theSync->SyncLong(mX);
	theSync->SyncLong(mY);
	theSync->SyncLong(mWidth);
	theSync->SyncLong(mHeight);
	theSync->SyncBool(mVisible);
	theSync->SyncBool(mMouseVisible);
	theSync->SyncBool(mDisabled);
	theSync->SyncBool(mDoFinger);
	theSync->SyncLong(mVirtualTankId);
	if (mVirtualTankId > -1)
	{
		theSync->SyncString(mName);
		theSync->SyncBool(mShown);
		theSync->SyncBool(mTodayBought);
		if (theSync->mReader)
			mTimeBought = 0;
		theSync->SyncLong((int&)mTimeBought);
		theSync->SyncLong(mShellPrice);
		theSync->SyncLong(mHometownIdx);
		theSync->SyncByte(m0xd8);
		theSync->SyncLong(mExoticDietFoodType);
		theSync->SyncBool(mForwardlyChallenged);
		theSync->SyncBool(mSpeedy);
		theSync->SyncLong(mSpeedySpeedState);
		theSync->SyncBool(mInvisible);
		theSync->SyncLong(mInvisibleTimer);
		theSync->SyncBool(mVoracious);
		theSync->SyncLong(mVoraciousScreamCounter);
		theSync->SyncLong(m0x10c);
		theSync->SyncBool(mSinging);
		theSync->SyncLong(m0x114);
		theSync->SyncLong(m0x118);
		theSync->SyncLong(mSongId);
		if (theSync->mReader)
			mBoughtTimeDaySecs = 0;
		theSync->SyncLong((int&)mBoughtTimeDaySecs);
		if (theSync->mReader)
			mLastMentalStateUpdateTime = 0;
		theSync->SyncLong((int&)mLastMentalStateUpdateTime);
		theSync->SyncLong(mTimesFedToday);
		theSync->SyncLong(mMentalState);
		theSync->SyncLong(mRandomHappiestMentalId);
		theSync->SyncBytes(&mPreNamedTypeId, 4);
		for (int i = 0; i < 3; i++)
			theSync->SyncLong(mLikes[i]);
	}
}

// A GameObject member in the original (0x4D65F0, this in ecx), called by SimFishScreen
const char* Sexy::GameObject::GetMentalStateString()
{
	switch (mMentalState)
	{
	case 0:
		return "Horribly Depressed";
	case 1:
		return "Feeling Neglected";
	case 2:
		return "Quite Hungry";
	case 3:
		return "Contented";
	case 4:
		return "Happy";
	case 5:
		return "Chipper";
	case 6:
	{
		const char* aPossibleStrs[] = {
		"Super Pumped" ,
		"Lovin\' It!",
		"High on Life" ,
		"Totally Stoked",
		"Fish-tastic!"
		};
		return aPossibleStrs[mRandomHappiestMentalId % 5];
	}
	default:
		return "Unknown";
	}
}

void GameObject::UpdateCounters()
{
	Widget::Update();
	if (mVoraciousScreamCounter != 0)
		mVoraciousScreamCounter--;
	if (m0x10c != 0)
		m0x10c--;
	if (mSpeedySpeedState != 0)
		mSpeedySpeedState--;

	if (m0x114 > 0) // Something about music
	{
		ShowInvisibility();
		if (mSongId == -1)
		{
			m0x114--;
			if (m0x114 == 0)
			{ 
				mSongId = mApp->mBoard->PlayFishSong(mType, mPreNamedTypeId);
				if (mSongId != -1)
					m0x114 = 1;
			}
		}
		else
		{
			m0x114++;
			bool aFinished = mApp->mBoard->IsSongPlaying(mSongId);
			if (!aFinished)
			{
				m0x114 = 0;
				mSongId = -1;
			}
			else if (m0x114 == 2 || m0x114 % 30 == 0)
				mApp->mBoard->MakeNote(mX + mWidth / 2 - 20, mY - 50, 1, ""); // Makes the note object going upwards
		}
	}

	if (mCanBeEatenDelay != 0)
		mCanBeEatenDelay--;
}

bool Sexy::GameObject::CoinDropTimerPassed(int& theCurrentTime, int theToPassTime)
{
	if (mApp->mGameMode != GAMEMODE_VIRTUAL_TANK)
		return theCurrentTime >= theToPassTime;

	if (mVirtualTankId < 0 || (mApp->IsScreenSaver() && mApp->mBoard->m0x400 > gUnkInt07))
		return false;

	if (theToPassTime > 360)
	{
		theToPassTime -= mMentalState * 540;
		if (theToPassTime < 360)
			theToPassTime = 360;
	}

	if (!mApp->mBoard->m0x500)
	{
		if (theCurrentTime > theToPassTime)
			theCurrentTime = 0;
		return false;
	}

	if (mHunger >= 0)
		return theCurrentTime >= theToPassTime;

	if (theCurrentTime > theToPassTime)
		theCurrentTime = theToPassTime - 90;
	return false;
}

bool Sexy::GameObject::RelaxModeCanDrop()
{
	return !mApp->mRelaxMode || mHunger > 0;
}

void Sexy::GameObject::UpdateCrosshairAnimation()
{
	mCrosshairAnimationTimer = (mCrosshairAnimationTimer + 1) % 20;
}

void GameObject::ShowInvisibility()
{
	if (mInvisible)
	{
		if (mInvisibleTimer == 0)
			mInvisibleTimer = 1;
		else if (mInvisibleTimer > 10)
			mInvisibleTimer = 10;
	}
}

void Sexy::GameObject::UpdateStoreCounters()
{
	mStoreAnimationTimer++;
	if (mInvisible)
		mStoreInvisibilityTimer++;
}

void Sexy::GameObject::UpdateInvisible()
{
	if (mInvisibleTimer > 0 && ++mInvisibleTimer > 80)
		mInvisibleTimer = 0;
}

void GameObject::UpdateHungerAnimCounter()
{
	if (mHungerAnimationTimer > 0)
	{
		if (mHungerShown)
		{
			mHungerAnimationTimer++;
			if (mHungerAnimationTimer > 5)
				mHungerAnimationTimer = 0;
		}
		else
			mHungerAnimationTimer--;
	}
}

void GameObject::UpdateHungerCounter()
{
	if (mSongId == -1)
		mHunger--;
	if (mHunger == mHungerShowTreshold + 4)
		UpdateHungerState(true);
	else if (mHunger < -1000)
		mHunger = -1000;
}

void GameObject::UpdateHungerState(bool theHungryShown)
{
	if (!mApp->IsScreenSaver())
	{
		mHungerShown = theHungryShown;
		if (theHungryShown)
		{
			if (mTimesFedToday < 3 || mApp->mBoard->mAlwaysShowWhenHungry)
				mHungerAnimationTimer = 1;
		}
		else
			mHungerAnimationTimer = 5;
	}
}

bool GameObject::ShouldDie()
{
	if (mVirtualTankId > -1)
		return false;
	if (mApp->mRelaxMode)
	{
		if (mType != TYPE_GEKKO && mType != TYPE_GRUBBER && mType != TYPE_PENTA)
			return mHunger <= -2160;
		return mHunger <= -4320;
	}
	return mHunger <= 0;
}

int GameObject::ExoticFoodCollision(int theCenterX, int theCenterY)
{
	if (mExoticDietFoodType >= EXO_FOOD_OBJECTS_START)
		return CarnivorousExoticFoodCollision(theCenterX, theCenterY, mExoticDietFoodType - EXO_FOOD_OBJECTS_START);

	std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;

	int aFoodTypeToEat = mExoticDietFoodType;
	if (mExoticDietFoodType == 6)
		aFoodTypeToEat = 0;

	int aReturnValue = 0;

	for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
	{
		Food* aTargetFood = *it;

		if (aTargetFood->mExoticFoodType == aFoodTypeToEat && !aTargetFood->mPickedUp && aTargetFood->mCantEatTimer == 0)
		{
			int aDeltaX = aTargetFood->mX - theCenterX;
			int aDeltaY = aTargetFood->mY - theCenterY;

			int aModifiedDeltaX = aDeltaX + 20;
			int aModifiedDeltaY = aDeltaY + 20;

			if (aModifiedDeltaX > -15 && aModifiedDeltaX < 15 && aModifiedDeltaY > -20 && aModifiedDeltaY < 20)
			{
				OnFoodAte(aTargetFood);
				aTargetFood->RemoveFood();
				return 1;
			}

			if (aModifiedDeltaX > -30 && aModifiedDeltaX < 30 && aModifiedDeltaY > -25 && aModifiedDeltaY < 25)
				aReturnValue = 2;
		}
	}

	return aReturnValue;
}

int Sexy::GameObject::CarnivorousExoticFoodCollision(int theCenterX, int theCenterY, int theID)
{
	Board* aBoard = mApp->mBoard;
	GameObjectSet::iterator anIterator;
	int aRetVal = 0;

	for (anIterator = aBoard->mGameObjectSet.begin(); anIterator != aBoard->mGameObjectSet.end(); ++anIterator)
	{
		GameObject* aTargetObject = *anIterator;

		if ((aTargetObject->mVirtualTankId < 0) &&
			(aTargetObject->mCanBeEatenDelay <= 0) &&
			(aTargetObject->mType == theID) &&
			(aTargetObject != this))
		{
			Fish* aTargetFish = (Fish*)aTargetObject;

			int aHalfFWidth = aTargetFish->mWidth / 2;
			int aHalfFHeight = aTargetFish->mHeight / 2;

			int val1 = theCenterX - aHalfFWidth - aTargetFish->mX;
			int val2 = theCenterY - aHalfFHeight - aTargetFish->mY;

			if ((val1 > -aHalfFWidth) && (val1 < aHalfFWidth) &&
				(val2 > -aHalfFHeight) && (val2 < aHalfFHeight))
			{
				OnFoodAte(aTargetFish);
				aTargetFish->Remove();
				return 1;
			}

			int anExtendedHalfFWidth = aHalfFWidth + 20;
			int anExtendedHalfFHeight = aHalfFHeight + 20;

			if ((val1 > -anExtendedHalfFWidth) && (val1 < anExtendedHalfFWidth) &&
				(val2 > -anExtendedHalfFHeight) && (val2 < anExtendedHalfFHeight))
				aRetVal = 2;
		}
	}

	return aRetVal;
}

void Sexy::GameObject::RemoveHelper02(bool safeDelete)
{
	if (mApp->mBoard->RemoveGameObjectFromLists(this, true))
	{
		if (mMisslePtr)
		{
			mMisslePtr->RemoveHelper02(true);
			mMisslePtr = nullptr;
		}
		if (mShadowPtr)
		{
			mShadowPtr->RemoveHelper02(true);
			mShadowPtr = nullptr;
		}
		VFT74();
		mApp->mBoard->mWidgetManager->RemoveWidget(this);
		if (safeDelete)
			mApp->SafeDeleteWidget(this);
	}
}

void Sexy::GameObject::UpdateFishSongMgr()
{
	if (gUnkBool02)
		mApp->mBoard->FishSongMgrUpdate();
}

void Sexy::GameObject::Unk02(bool flag)
{
	mSpeedySpeedState = 0;
	mApp->mBoard->mShouldSave = true;
	if (!flag)
	{
		if ((mVirtualTankId > -1 || mApp->mRelaxMode) && mHunger < 300)
			mHunger = 300;

		if (mSinging)
		{
			if (m0x118++ % 5 == 0 && mPreNamedTypeId != KILGORE && m0x114 == 0)
			{
				int aVal = m0x10c;
				if (m0x10c < 5)
					aVal = 5;
				m0x114 = aVal;
			}
		}
	}
}

void Sexy::GameObject::Unk03(long long theTodayInSec, __time64_t theCurTime)
{
	if (mVirtualTankId < 0)
		return;

	if (mTodayBought)
	{
		// Also cleared when the clock went back before the purchase time.
		if (theCurTime < mTimeBought || theCurTime - mTimeBought >= 86400)
			mTodayBought = false;
	}

	if (theTodayInSec != mBoughtTimeDaySecs)
	{
		// The original subtracts the low 32 bits (these are day numbers).
		int daysPassed = (int)theTodayInSec - (int)mLastMentalStateUpdateTime - 1;
		if (daysPassed < 0)
		{
			mLastMentalStateUpdateTime = theTodayInSec;
			daysPassed = 0;
		}

		mMentalState -= daysPassed;
		mTimesFedToday = 0;
		mBoughtTimeDaySecs = theTodayInSec;
		if (mMentalState < 0)
			mMentalState = 0;
	}
}

bool Sexy::GameObject::UpdateMentalState()
{
	__time64_t aCurTime = _time64(NULL);
	long long aTime = GetTodayStartSeconds();

	Unk03(aTime, aCurTime);
	
	if (mTimesFedToday < 3)
	{
		mTimesFedToday++;
		if (mTimesFedToday == 3)
		{
			mMentalState++;
			mLastMentalStateUpdateTime = aTime;
			if (mMentalState > 6)
				mMentalState = 6;
		}
		return true;
	}
	return false;
}

void Sexy::GameObject::DrawName(Graphics* g, bool flag)
{
	if (mName.size() > 0 && mApp->mBoard->m0x4fc && (!mInvisible || mInvisibleTimer != 0 || flag))
	{
		int aVal = SpecialReturnValue();
		if (IsHungryVisible())
			g->SetColor(Color(0xd6cf29));
		else g->SetColor(Color(0xffffff));
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
		// The original measures the name again for each DrawString
		g->DrawString(mName, (mWidth - g->GetFont()->StringWidth(mName)) / 2, mHeight + aVal - 2);
		if (mHungerAnimationTimer != 0)
		{
			Color aCol = Color(0xd6cf29);
			aCol.mAlpha = mHungerAnimationTimer * 255 / 5;
			g->SetColor(aCol);
			g->DrawString(mName, (mWidth - g->GetFont()->StringWidth(mName)) / 2, mHeight + aVal - 2);
		}
	}
}

void Sexy::GameObject::DrawCrosshair(Graphics* g, int theX, int theY)
{
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
	g->DrawImageCel(IMAGE_CROSSHAIR, theX, theY, (mCrosshairAnimationTimer / 4)); // Might be incorrect
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
}

int Sexy::GameObject::GetExoticDietOffset()
{
	if (mExoticDietFoodType == 0 || mExoticDietFoodType == 6)
		return 0;
	return 20;
}

// A separate function in the original (0x4D7210), called for each object by Board::GetExoticFoodsRequiredInTank:
// adds the food this object needs to theInfoArray
void Sexy::GameObject::AddRequiredFood(int* theInfoArray)
{
	if (!mShown)
		return;

	int anExDiet = mExoticDietFoodType;
	if (anExDiet == 0)
		CountRequiredFood(theInfoArray);
	else if (anExDiet < EXO_FOOD_OBJECTS_START)
	{
		switch (anExDiet)
		{
		case EXO_FOOD_PIZZA:
			theInfoArray[6]++;
			break;
		case EXO_FOOD_ICE_CREAM:
			theInfoArray[7]++;
			break;
		case EXO_FOOD_CHICKEN:
			theInfoArray[5]++;
			break;
		}
	}
	else
	{
		switch (anExDiet)
		{
		case EXO_FOOD_GUPPY:
			theInfoArray[0]++;
			break;
		case EXO_FOOD_OSCAR:
			theInfoArray[3]++;
			break;
		case EXO_FOOD_ULTRA:
			theInfoArray[4]++;
			break;
		}
	}
}

bool Sexy::GameObject::CantBeEaten()
{
	return mVirtualTankId >= 0 || mCanBeEatenDelay > 0;
}

// Original 0x4D7370 (LTCG: this in eax, theBestDist in edx): true and updates the best
// squared distance if this object (centre at +20, +20) is nearer to (theX, theY).
bool Sexy::GameObject::CheckNearestDist(int* theBestDist, int theX, int theY)
{
	int aDist = (mX - theX + 20) * (mX - theX + 20) + (mY - theY + 20) * (mY - theY + 20);
	if (aDist >= *theBestDist)
		return false;
	*theBestDist = aDist;
	return true;
}

bool Sexy::GameObject::IsHungryVisible()
{
	if (mApp->IsScreenSaver())
		return false;
	return mHunger <= 300 && mHungerAnimationTimer == 0 && (mTimesFedToday < 3 || mApp->mBoard->mAlwaysShowWhenHungry);
}

GameObject* Sexy::GameObject::FindNearestExoticFood(int theX, int theY)
{
	if (mExoticDietFoodType >= EXO_FOOD_OBJECTS_START)
		return FindNearestExoticFoodOther(theX, theY, mExoticDietFoodType - EXO_FOOD_OBJECTS_START);

	std::vector<Food*>* aFoodList = mApp->mBoard->mFoodList;
	int aDist = 100000000;
	Food* aRet = nullptr;

	int aFoodTypeToEat = mExoticDietFoodType;
	if (mExoticDietFoodType == 6)
		aFoodTypeToEat = 0;

	for (std::vector<Food*>::iterator it = aFoodList->begin(); it != aFoodList->end(); ++it)
	{
		Food* aFood = *it;

		if (aFood->mExoticFoodType == aFoodTypeToEat && !aFood->mPickedUp && aFood->mCantEatTimer == 0)
		{
			int ax = aFood->mX - theX + 20;
			int ay = aFood->mY - theY + 20;
			int aNewDist = ax * ax + ay * ay;

			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = aFood;
			}
		}
	}

	if (mType == TYPE_ULTRA && aDist < 40000)
		VoraciousScream(175);
	if (aDist < 10000)
	{
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}
	return aRet;
}

GameObject* Sexy::GameObject::FindNearestExoticFoodOther(int theX, int theY, int theObjType)
{
	Board* aBoard = mApp->mBoard;
	int aDist = 100000000;
	GameObject* aRet = nullptr;

	for (GameObjectSet::iterator it = aBoard->mGameObjectSet.begin(); it != aBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;

		// A fish never takes itself for food (0x4E9F4F)
		if (anObj->mVirtualTankId < 0 && anObj->mCanBeEatenDelay <= 0 && anObj->mType == theObjType && anObj != this)
		{
			int ax = theX - anObj->mWidth / 2 - anObj->mX;
			int ay = theY - anObj->mHeight / 2 - anObj->mY;
			int aNewDist = ax * ax + ay * ay;

			if (aNewDist < aDist)
			{
				aDist = aNewDist;
				aRet = anObj;
			}
		}
	}

	if(mType == TYPE_ULTRA && aDist < 40000)
		VoraciousScream(175);
	else if(theObjType == TYPE_ULTRA && aDist < 22500)
		VoraciousScream(150);

	if (aDist < 10000)
	{
		VoraciousScream(150);
		mSpeedySpeedState = 100;
	}

	return aRet;
}

void Sexy::GameObject::VoraciousScream(int theNum)
{
	if (mVoracious && (mPreNamedTypeId != KILGORE || mSongId != -1) && mVoraciousScreamCounter == 0)
	{
		mApp->mBoard->PlaySample(SOUND_PRIMALSCREAM_ID, 3, 1.0);
		mVoraciousScreamCounter = theNum;
		m0x10c = 80;
	}
}

bool Sexy::GameObject::IsHungryBlipPointer(int theTreshold)
{
	if (mHunger < theTreshold && mApp->mBoard->mPetsInTank[13] != 0 && (mTimesFedToday < 3 || mApp->mBoard->mAlwaysShowWhenHungry))
		return true;
	return false;
}

void Sexy::GameObject::SetColorHelper(Graphics* g, Color& theColor)
{
	g->SetColorizeImages(true);
	if (!mInvisible)
	{
		g->SetColor(theColor);
		return;
	}
	int anAlphaVal = 0;
	if (mInvisibleTimer < 10)
		anAlphaVal = (mInvisibleTimer * 255) / 10;
	else if (mInvisibleTimer < 50)
		anAlphaVal = 255;
	else
		anAlphaVal = ((80 - mInvisibleTimer) * 255) / 30;

	Color aNewColor = Color(theColor.mRed, theColor.mGreen, theColor.mBlue, (theColor.mAlpha * anAlphaVal) / 255);
	g->SetColor(aNewColor);
}

void Sexy::GameObject::UpdateHungerStateIfWasHungry(bool wasHungry)
{
	if (wasHungry)
		if (!IsHungryVisible())
			UpdateHungerState(false);
}



bool Sexy::GameObject::DrawInvisibleEffect(Graphics* g, Image* theImage, Rect& theSrcRect, bool mirror)
{
	int aBackgroundId = mApp->mBoard->mCurrentBackgroundId;
	if (aBackgroundId >= 1 && aBackgroundId <= 6)
	{
		Image* anImg = GetImageById(IMAGE_AQUARIUM1_ID + aBackgroundId - 1);
		DrawInvisibleEffectHelper(g, 0.0f, 0.0f, (MemoryImage*) anImg, mX - 2, mY - 2, (MemoryImage*) theImage, theSrcRect, mirror, mUpdateCnt);
	}
	return mInvisibleTimer == 0;
}

ulong Sexy::GetImagePixel(MemoryImage* theImage, int theX, int theY)
{
	if (theX < 0 || theY < 0)
		return 0;
	if (theX >= theImage->mWidth || theY >= theImage->mHeight)
		return 0;

	int anIndex = theX + theY * theImage->mWidth;
	if (theImage->mColorIndices != NULL)
		return theImage->mColorTable[theImage->mColorIndices[anIndex]];
	return theImage->GetBits()[anIndex];
}

void Sexy::DrawInvisibleEffectHelper(Graphics* g, float theX, float theY, MemoryImage* theBGImage, int theBGX, int theBGY, MemoryImage* theObjImage, Rect& theSrcRect, bool mirror, int theAnimCounter)
{
	MemoryImage anInvisImg = MemoryImage(gSexyApp);
	anInvisImg.Create(theSrcRect.mWidth, theSrcRect.mHeight);
	anInvisImg.SetImageMode(true, true);

	for (int y = 0; y < anInvisImg.mHeight; ++y)
	{
		float aValInRad = (float)((y * 5.0 + theAnimCounter * 5.0) * PI / 180.0);
		float aSinVal = (float)sin(aValInRad);

		int aSampleY = (int)(aSinVal + (double)(theBGY + y));

		if (aSampleY < 0 || aSampleY >= theBGImage->mHeight)
			continue;

		ulong* aBGBits = theBGImage->GetBits();
		ulong* aBGBitForEffect = aBGBits + theBGImage->mWidth * aSampleY + theBGX;

		ulong* anInvisBits = anInvisImg.GetBits();
		ulong* anInvisBitForEffect = anInvisBits + y * anInvisImg.mWidth;

		ulong* anObjBits = theObjImage->GetBits();
		ulong anObjBitPos = (theSrcRect.mY + y) * theObjImage->mWidth + theSrcRect.mX;

		if (mirror) // 77
		{
			// Last pixel of the source row (the original subtracts 4 bytes, i.e. one pixel).
			ulong* anObjBitForEffect = anObjBits + anObjBitPos + theSrcRect.mWidth - 1;
			int aCnt2 = theBGX;
			for (int x = 0; x < anInvisImg.mWidth; ++x)
			{
				if (aCnt2 >= 0 && aCnt2 < theBGImage->mWidth)
				{
					ulong anBitAlpha = *anObjBitForEffect & 0xFF000000;
					anObjBitForEffect--;
					if (anBitAlpha != 0)
					{
						anBitAlpha = *aBGBitForEffect;
						*anInvisBitForEffect = anBitAlpha;
					}
					anInvisBitForEffect++;
					aBGBitForEffect++;
				}
				aCnt2++;
			}
		}
		else // 56
		{
			ulong* anObjBitForEffect = anObjBits + anObjBitPos;
			int aCnt2 = theBGX;
			for (int x = 0; x < anInvisImg.mWidth; ++x)
			{
				if (aCnt2 >= 0 && aCnt2 < theBGImage->mWidth)
				{
					ulong anBitAlpha = *anObjBitForEffect & 0xFF000000;
					anObjBitForEffect++;
					if (anBitAlpha != 0)
					{
						anBitAlpha = *aBGBitForEffect;
						*anInvisBitForEffect = anBitAlpha;
					}
					anInvisBitForEffect++;
					aBGBitForEffect++;
				}
				aCnt2++;
			}
		}
	}

	g->DrawImage(&anInvisImg, (int)theX, (int)theY);
}

void Sexy::DrawImageMirrorHelper(Graphics* g, Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, bool mirror)
{
	if (theDestRect.mWidth == theSrcRect.mWidth && theDestRect.mHeight == theSrcRect.mHeight)
		g->DrawImageMirror(theImage, theDestRect.mX, theDestRect.mY, theSrcRect, mirror);
	else
		g->DrawImageMirror(theImage, theDestRect, theSrcRect, mirror);
}

bool Sexy::GameObject::PrestoRightClicked(int theTimer)
{
	if (theTimer <= 0)
		return true;

	mApp->mBoard->PlaySample(SOUND_BUZZER_ID, 3, 1.0);

	mApp->mBoard->MakeNote(mX + mWidth / 2 - 20, mY - 10, 2, "Recharging...");
	return false;
}

void Sexy::GameObject::DrawPrestoMisc(Graphics* g, int theTimer)
{
	g->SetColor(Color(0xaaffaa));
	g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	int aStrWdth = g->GetFont()->StringWidth("Presto");
	int aX = (mWidth - aStrWdth) / 2;
	g->DrawString("Presto", aX, mHeight - 2);

	if (theTimer > 0)
	{
		Graphics g2(*g);

		g2.SetColor(Color(0xff3333));
		g2.ClipRect(aStrWdth - (aStrWdth * theTimer) / 360 + aX, 0, mWidth, mHeight);

		g2.DrawString("Presto", aX, mHeight - 2);
	}
}

void Sexy::GameObject::DeterminePetSleepy(bool* sleepy)
{
	if (!mApp->IsScreenSaver())
	{
		if (mWidgetManager->mUpdateCnt - mWidgetManager->mLastInputUpdateCnt < 6480)
			*sleepy = false;
		else if (*sleepy == false && Rand() % 60 == 0)
			*sleepy = true;
	}
}

// Static in the original (0x501060, cdecl with the object pushed): the object's store attribute
int Sexy::GameObject::GetAttribute(GameObject* theObject)
{
	if (theObject->mPreNamedTypeId != -1)
		return 8;

	int anAttrib = 9;
	int numOfAttrib = 0;
	if (theObject->mInvisible)
	{
		anAttrib = 0;
		numOfAttrib++;
	}
	if (theObject->mVoracious)
	{
		anAttrib = 1;
		numOfAttrib++;
	}
	if (theObject->mSpeedy)
	{
		anAttrib = 2;
		numOfAttrib++;
	}
	if (theObject->mSinging)
	{
		anAttrib = 3;
		numOfAttrib++;
	}
	if (theObject->mForwardlyChallenged)
	{
		anAttrib = 4;
		numOfAttrib++;
	}
	if (theObject->mExoticDietFoodType == 3 || theObject->mExoticDietFoodType == 4 || theObject->mExoticDietFoodType == 5)
	{
		anAttrib = 5;
		numOfAttrib++;
	}
	if (theObject->mExoticDietFoodType == EXO_FOOD_ULTRA || theObject->mExoticDietFoodType == EXO_FOOD_OSCAR || theObject->mExoticDietFoodType == EXO_FOOD_GUPPY)
	{
		anAttrib = 6;
		numOfAttrib++;
	}
	if (numOfAttrib > 1)
		anAttrib = 7;
	return anAttrib;
}

int Sexy::GameObject::GetShellCost()
{
	int aCost = 100;
	switch (mType)
	{
	case TYPE_GUPPY:
	{
		Fish* aFish = (Fish*)this;
		if (aFish->mHasSpecialColors)
			aCost = aFish->mRainbowFish ? 2500 : 500;
		else
			aCost = 25;
		break;
	}
	case TYPE_OSCAR:
	{
		Oscar* aFish = (Oscar*)this;
		if (aFish->mHasSpecialColors)
			aCost = aFish->mRainbowFish ? 5000 : 1000;
		break;
	}
	case TYPE_ULTRA:
	case TYPE_BI_FISH:
		aCost = 10000;
		break;
	case TYPE_GEKKO:
		aCost = 3500;
		break;
	case TYPE_PENTA:
	case TYPE_GRUBBER:
		aCost = 2500;
		break;
	case TYPE_BREEDER:
		aCost = 5000;
		break;
	case TYPE_SYLVESTER_FISH:
		aCost = 15000;
		break;
	case TYPE_BALL_FISH:
		aCost = 20000;
		break;
	}

	int anAttrib = GetAttribute(this);
	if (anAttrib != 9)
		aCost += 5000;

	switch (anAttrib)
	{
	case 0:
	case 3:
	case 6:
		return 30000;
	case 1:
	case 2:
	case 4:
	case 5:
		return 25000;
	case 7:
		return 40000;
	case 8:
		return 50000;	
	default:
		aCost = (aCost / 5) * 5;
		if (aCost <= 0)
			aCost = 10;
		break;
	}
	return aCost;
}

void Sexy::GameObject::SetStoreColor(Graphics* g, Color& theColor)
{
	g->SetColorizeImages(true);
	if (!mInvisible)
	{
		g->SetColor(theColor);
	}
	else
	{
		int anAlphaMul = mStoreInvisibilityTimer % 300;
		if (anAlphaMul > 150)
			anAlphaMul = 300 - anAlphaMul;
		g->SetColor(Color(theColor.mRed, theColor.mGreen, theColor.mBlue, (theColor.mAlpha * anAlphaMul) / 600 + 10));
	}
}

void Sexy::GameObject::ResetSpecialProperties()
{
	mExoticDietFoodType = 0;
	mVoracious = false;
	mForwardlyChallenged = false;
	mInvisible = false;
	mSinging = false;
	mSpeedy = false;
}

void Sexy::GameObject::BoughtSetup()
{
	long long secs = GetTodayStartSeconds();
	mBoughtTimeDaySecs = secs;
	mLastMentalStateUpdateTime = secs - 1;
	mMentalState = 3;
	mRandomHappiestMentalId = Rand() % 1000;
	m0xd8 = Rand() % 12;

	RandomizeHometownAndLikes();
}

// Picks the Virtual Tank hometown and three different likes
void Sexy::GameObject::RandomizeHometownAndLikes()
{
	// The original reads both table sizes from memory (0x59BDC8, 0x59BDC4)
	mHometownIdx = (Rand() % (NUM_HOMETOWNS - 1)) + 1;

	for (int i = 0; i < 3; i++)
	{
		bool isUnique;
		do
		{
			isUnique = true;
			int rndLike = Rand() % NUM_LIKES;
			mLikes[i] = rndLike;

			// Every earlier like is compared, without stopping at a match
			for (int j = 0; j < i; j++)
			{
				if (mLikes[j] == rndLike)
					isUnique = false;
			}
		} while (!isUnique);
	}
}

bool Sexy::GameObject::CanDropCoin()
{
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (mApp->mBoard->m0x500)
		{
			if (!mApp->IsScreenSaver() || mApp->mBoard->m0x400 <= gUnkInt07)
				return true;
		}
		return false;
	}
	return true;
}

int Sexy::GameObject::DetermineCoinDropT(int supposedTime)
{
	if (mApp->mGameMode == GAMEMODE_VIRTUAL_TANK)
		return Rand() % 200 + 4320;
	return supposedTime;
}

void Sexy::GameObject::CopyBreederDataVT(Breeder* theBreeder)
{
	mName = theBreeder->mName;
	mTimeBought = theBreeder->mTimeBought;
	mShellPrice = theBreeder->mShellPrice;
	m0xd8 = theBreeder->m0xd8;
	mHometownIdx = theBreeder->mHometownIdx;
	mLikes[0] = theBreeder->mLikes[0];
	mLikes[1] = theBreeder->mLikes[1];
	mLikes[2] = theBreeder->mLikes[2];
	mExoticDietFoodType = theBreeder->mExoticDietFoodType;
	mForwardlyChallenged = theBreeder->mForwardlyChallenged;
	mSpeedy = theBreeder->mSpeedy;
	mInvisible = theBreeder->mInvisible;
	mVoracious = theBreeder->mVoracious;
	mSinging = theBreeder->mSinging;
}

bool Sexy::GameObject::RelaxModeCheck(Coin* theCoin)
{
	if (mApp->mRelaxMode)
		if (!mApp->mBoard->mPentaList->empty() && (theCoin->mCoinType == COIN_GOLD_C || theCoin->mCoinType == COIN_SILVER_C))
			return true;
	return false;
}
