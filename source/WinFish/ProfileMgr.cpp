#include <SexyAppFramework/Common.h>

#include "ProfileMgr.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"

using namespace Sexy;

ProfileMgr::ProfileMgr()
{
    Clear();
}

ProfileMgr::~ProfileMgr()
{
}

void Sexy::ProfileMgr::Clear()
{
    mProfilesMap.clear();
    m0x10 = 1;
    m0x14 = 1;
}

void Sexy::ProfileMgr::ReadUsersDat()
{
    Buffer aBuf;

    if (gSexyApp->ReadBufferFromFile(GetAppDataFolder() + "userdata/users.dat", &aBuf, false))
    {
        try
        {
            DataReader aDR;
            aDR.OpenMemory(aBuf.GetDataPtr(), aBuf.GetDataLen(), false);
            DataSync aDS(aDR);
            SyncUsersDat(aDS);
        }
        catch (DataReaderException&)
        {
            Clear();
        }
    }
}

UserProfile* Sexy::ProfileMgr::GetUserProfile(const SexyString& theProfileName)
{
    UserProfilesMap::iterator anIterator = mProfilesMap.find(theProfileName);

    if (anIterator != mProfilesMap.end())
    {
        UserProfile* aProfile = &anIterator->second;
        aProfile->LoadFromMemory();
        aProfile->m0x40 = m0x14;
        m0x14++;
        return aProfile;
    }

    return nullptr;
}

UserProfile* Sexy::ProfileMgr::GetFirstUserProfile()
{
    if (mProfilesMap.empty())
        return nullptr;

    UserProfilesMap::iterator anIterator = mProfilesMap.begin();

    UserProfile* aFirstProfile = &anIterator->second;

    aFirstProfile->LoadFromMemory();
    aFirstProfile->m0x40 = m0x14;
    m0x14++;

    return aFirstProfile;
}

void Sexy::ProfileMgr::SyncUsersDat(DataSync& theDataSync)
{
    DataWriter* aDW = theDataSync.mWriter;
    DataReader* aDR = theDataSync.mReader;
    int aDatVersion = gUsersDatVersion;
    theDataSync.SyncLong(aDatVersion);
    theDataSync.mVersion = aDatVersion;
    if (aDatVersion != gUsersDatVersion)
        return;

    uint aMaxUseSeq = 0;
    uint aMaxId = 0;
    if (aDR)
    {
        mProfilesMap.clear();
        int aProfileMapSize = aDR->ReadShort();
        for (int i = aProfileMapSize; i > 0; i--)
        {
            UserProfile aProf;
            aProf.SyncUsersDat(theDataSync);
            if (aProf.m0x40 > aMaxUseSeq)
                aMaxUseSeq = aProf.m0x40;
            if ((uint)aProf.mId > aMaxId)
                aMaxId = aProf.mId;
            // operator[] default-constructs the entry first, so each profile costs two more Rand() calls in Init()
            mProfilesMap[aProf.mUserName] = aProf;
        }
        m0x10 = aMaxId + 1;
        m0x14 = aMaxUseSeq + 1;
    }
    else
    {
        aDW->WriteShort(mProfilesMap.size());
        for (UserProfilesMap::iterator it = mProfilesMap.begin(); it != mProfilesMap.end(); ++it)
        {
            UserProfile* aProf = &it->second;
            aProf->SyncUsersDat(theDataSync);
        }
    }
}

void Sexy::ProfileMgr::SaveUsersDat()
{
    DataWriter aDW;
    aDW.OpenMemory(32);
    DataSync aDS(aDW);
    SyncUsersDat(aDS);

    MkDir(GetAppDataFolder() + "userdata");

    gSexyApp->WriteBytesToFile(GetAppDataFolder() + "userdata/users.dat", aDW.mMemoryHandle, aDW.mMemoryPosition);
}

UserProfile* Sexy::ProfileMgr::MakeNewUser(const SexyString& theUserName)
{
    std::pair<UserProfilesMap::iterator, bool> aResult = mProfilesMap.insert(UserProfilesMap::value_type(theUserName, UserProfile()));
    if (!aResult.second)
        return nullptr;

    UserProfile* aProf = &aResult.first->second;
    aProf->mUserName = theUserName;
    aProf->mId = m0x10++;
    aProf->m0x40 = m0x14++;

    CapMaxUserCount();

    return aProf;
}

bool Sexy::ProfileMgr::RenameUser(const SexyString& theOldUserName, const SexyString& theNewUserName)
{
    UserProfilesMap::iterator oldIt = mProfilesMap.find(theOldUserName);
    if (oldIt == mProfilesMap.end())
        return false;

    if (_stricmp(theOldUserName.c_str(), theNewUserName.c_str()) == 0)
    {
        // Only the case changes; the map key keeps the old spelling until users.dat is reloaded
        oldIt->second.mUserName = theNewUserName;
    }
    else
    {
        std::pair<UserProfilesMap::iterator, bool> aResult = mProfilesMap.insert(UserProfilesMap::value_type(theNewUserName, oldIt->second));
        if (!aResult.second)
            return false;

        mProfilesMap.erase(oldIt);
        aResult.first->second.mUserName = theNewUserName;
    }

    return true;
}

bool Sexy::ProfileMgr::DeleteUser(const SexyString& theUserName)
{
    UserProfilesMap::iterator anIt = mProfilesMap.find(theUserName);
    if (anIt == mProfilesMap.end())
        return false;

    DeleteProfile(anIt);
    return true;
}

void Sexy::ProfileMgr::DeleteProfile(UserProfilesMap::iterator theItr)
{
    theItr->second.DeleteUserAppData();
    mProfilesMap.erase(theItr);
}

void Sexy::ProfileMgr::DeleteOldestProfile()
{
    if (mProfilesMap.empty())
        return;

    UserProfilesMap::iterator oldest_it = mProfilesMap.begin();

    for (UserProfilesMap::iterator it = mProfilesMap.begin(); it != mProfilesMap.end(); ++it)
        if (it->second.m0x40 < oldest_it->second.m0x40)
            oldest_it = it;

    oldest_it->second.DeleteUserAppData();
    mProfilesMap.erase(oldest_it);
}

void Sexy::ProfileMgr::CapMaxUserCount()
{
    while (mProfilesMap.size() > 200)
        DeleteOldestProfile();
}

void Sexy::UserProfile::DeleteUserAppData()
{
    gSexyApp->EraseFile(GetAppDataFolder() + StrFormat("userdata/user%d.dat", mId));
    gSexyApp->EraseFile(GetAppDataFolder() + StrFormat("userdata/scr%d.dat", mId));

    for (int i = 0; i <= 5; ++i)
        gSexyApp->EraseFile(GetSaveGameFilePath(i, mId));
}

void Sexy::UserProfile::SaveScreenSaver(int unk)
{
    if (unk <= 0)
        return;

    SexyString aFilePath = GetAppDataFolder() + StrFormat("userdata/scr%d.dat", mId);
    int aVal = ReadScreenSaverData(false) + unk;
    DataWriter aDW;
    aDW.OpenMemory(32);
    aDW.WriteLong(m0x88);
    for (int i = 0; i < 50; i++)
        aDW.WriteLong(rand() % 20000);

    aDW.WriteLong(aVal);

    for (int i = 0; i < 50; i++)
        aDW.WriteLong(rand() % 20000);

    MkDir(GetAppDataFolder() + "userdata");

    ((WinFishApp*)gSexyApp)->WriteBytesToFile(aFilePath, aDW.mMemoryHandle, aDW.mMemoryPosition);
    ((WinFishApp*)gSexyApp)->ClearUpdateBacklog(false);
}

void Sexy::UserProfile::Unk01()
{
    int aVal = ReadScreenSaverData(true);
    
    if (aVal > 100000)
        aVal = 100000;
    else if (aVal <= 0)
        return;

    AddShells(aVal);
    m0x88 = Rand();
    Save();
}

void Sexy::UserProfile::WriteAdventureScore(int theTank, int theLevel, int theScore)
{
    int anIdx = GetLevelIndex(theTank, theLevel);
    if (anIdx < 0)
        return;

    if (theTank == 5 && theLevel == 1)
    {
        if (mAdventureScores[anIdx] < theScore)
            mAdventureScores[anIdx] = theScore;
    }
    else
    {
        if (mAdventureScores[anIdx] > theScore || mAdventureScores[anIdx] == -1)
            mAdventureScores[anIdx] = theScore;
    }
}

void Sexy::UserProfile::WriteTimeTrialScore(int theTank, int theScore)
{
    int anIdx = theTank - 1;
    if ((uint)anIdx <= 3 && mTimeTrialScores[anIdx] < theScore)
        mTimeTrialScores[anIdx] = theScore;
}

void Sexy::UserProfile::WriteChallengeScore(int theTank, int theScore)
{
    int anIdx = theTank - 1;
    if ((uint)anIdx <= 3 && (mChallengeScores[anIdx] > theScore || mChallengeScores[anIdx] == -1))
        mChallengeScores[anIdx] = theScore;
}

int Sexy::UserProfile::ReadScreenSaverData(bool doErase)
{
    SexyString aFilePath = GetAppDataFolder() + StrFormat("userdata/scr%d.dat", mId);
    Buffer aBuf;
    bool success = gSexyApp->ReadBufferFromFile(aFilePath, &aBuf, false);
    if (!success)
        return 0;

    if (doErase)
        gSexyApp->EraseFile(aFilePath);

    DataReader aReader;
    aReader.OpenMemory(aBuf.GetDataPtr(), aBuf.GetDataLen(), false);
    DataSync aSync(aReader);

    try
    {
        int aVal = aReader.ReadLong();
        if (aVal != m0x88)
            return 0;

        for (int i = 0; i < 50; i++)
            aReader.ReadLong();

        int resultValue = aReader.ReadLong();

        for (int i = 0; i < 50; i++)
            aReader.ReadLong();

        return resultValue;
    }
    catch (DataReaderException&)
    {
    }

    return 0;
}

void Sexy::UserProfile::UpdateStoreData(long long theNewTime)
{
    if (theNewTime != m0xa0)
    {
        m0xa0 = theNewTime;
        memset(mStoreScreenBought, false, 8);
    }
}

int Sexy::UserProfile::GetAdventureScore(int theTank, int theLevel)
{
    int anIdx = GetLevelIndex(theTank, theLevel);
    if (anIdx < 0)
        return -1;
    return mAdventureScores[anIdx];
}

int Sexy::UserProfile::GetTimeTrialScore(int theTank)
{
    if ((uint)(theTank - 1) <= 3)
        return mTimeTrialScores[theTank-1];
    return -1;
}

int Sexy::UserProfile::GetChallengeScore(int theTank)
{
    if ((uint)(theTank - 1) <= 3)
        return mChallengeScores[theTank-1];
    return -1;
}


Sexy::UserProfile::UserProfile()
{
    Init();
}

void Sexy::UserProfile::LoadFromMemory()
{
    Buffer aBuf;

    if (gSexyApp->ReadBufferFromFile(GetAppDataFolder() + StrFormat("userdata/user%d.dat", mId), &aBuf, false))
    {
        try
        {
            DataReader aDR;
            aDR.OpenMemory(aBuf.GetDataPtr(), aBuf.GetDataLen(), false);
            DataSync aDS(aDR);
            SyncData(aDS);
            SetCheatFlag(6, false);
        }
        catch (DataReaderException&)
        {
            Init();
        }
    }
}

void Sexy::UserProfile::Init()
{
    mTank = 1;
    mLevel = 1;
    mNumOfUnlockedPets = 0;
    m0x4c = 0;
    mCyraxNum = 0;
    mFinishedGameCount = 0;
    m0xb4 = 3;
    mCheatCodeFlags = 0;
    SetCheatFlag(1, true);
    m0x8c = 0;
    mBubbulatorBought = 0;
    mAlienAttractorBought = 0;
    m0x98 = Rand();
    m0x88 = Rand();
    // The three memsets are inlined as dword/word stores in the original (0x501591, 0x5015B1, 0x5015C8)
    memset(m0x78, 0, sizeof(m0x78));
    m0x7c = 0;
    m0x80 = 0;
    for (int i = 0; i < PET_END; i++)
    {
        m0x5a[i] = false;
        mUnlockedPets[i] = false;
    }
    memset(mUnlockedBackgrounds, 0, sizeof(mUnlockedBackgrounds));
    m0xa0 = 0;
    mUnlockedBackgrounds[0] = 1;
    memset(mStoreScreenBought, 0, sizeof(mStoreScreenBought));
    mBonusItemId = 0;
    mShells = 200;
    for (int i = 0; i < 21; i++)
        mAdventureScores[i] = -1;
    for (int i = 0; i < 4; i++)
    {
        mTimeTrialScores[i] = -1;
        mChallengeScores[i] = -1;
    }
    mFinishedGame = false;
    m0x58 = false;
}

void Sexy::UserProfile::SetCheatFlag(char thePos, bool theFlag)
{
    unsigned int mask = 1 << thePos;
    if (theFlag)
        mCheatCodeFlags |= mask;
    else
        mCheatCodeFlags &= ~mask;
}

bool Sexy::UserProfile::ToggleCheatFlag(char theIdx)
{
    unsigned int aMask = 1 << theIdx;
    mCheatCodeFlags ^= aMask;
    return (mCheatCodeFlags & aMask) != 0;
}

void Sexy::UserProfile::SyncUsersDat(DataSync& theDataSync)
{
    theDataSync.SyncString(mUserName);
    theDataSync.SyncLong(m0x40);
    theDataSync.SyncLong(mId);
}

void Sexy::UserProfile::Save()
{
    DataWriter aDW;
    aDW.OpenMemory(32);
    DataSync aDS(aDW);
    SyncData(aDS);

    MkDir(GetAppDataFolder() + "userdata");

    ((WinFishApp*)gSexyApp)->WriteBytesToFile(GetAppDataFolder() + StrFormat("userdata/user%d.dat", mId), aDW.mMemoryHandle, aDW.mMemoryPosition);
}

void Sexy::UserProfile::SyncData(DataSync& theDataSync)
{
    DataReader* theReader = theDataSync.mReader;
    if (theReader)
        Init();

    bool unkFlag = false;
    uint aVer = 0x80000000;
    theDataSync.SyncLong(aVer);

    // mReader is read again here and before m0xa0 below (0x50376D, 0x503801); theReader is used at the start and the end
    if (aVer < 0x7fffffff && theDataSync.mReader)
    {
        unkFlag = true;
        theDataSync.mReader->RollbackBytes(4);
    }

    theDataSync.SyncLong(mShells);
    theDataSync.SyncLong(mTank);
    theDataSync.SyncLong(mLevel);
    theDataSync.SyncBool(mFinishedGame);
    theDataSync.SyncBool(m0x58);
    theDataSync.SyncLong(mBubbulatorBought);
    theDataSync.SyncLong(mAlienAttractorBought);
    theDataSync.SyncLong(m0x8c);
    theDataSync.SyncLong(m0x4c);
    theDataSync.SyncLong(mCyraxNum);
    theDataSync.SyncLong(mFinishedGameCount);
    if (theDataSync.mReader)
        m0xa0 = 0;
    theDataSync.SyncLong((int&)m0xa0);
    theDataSync.SyncLong(m0x98);
    theDataSync.SyncLong(mBonusItemId);
    theDataSync.SyncLong(m0xb4);

    for (int i = 0; i < PET_END; i++)
        theDataSync.SyncBool(mUnlockedPets[i]);
    for (int i = 0; i < 6; i++)
        theDataSync.SyncBool(mUnlockedBackgrounds[i]);
    for (int i = 0; i < 8; i++)
        theDataSync.SyncBool(mStoreScreenBought[i]);

    for (int i = 0; i < 4; i++)
    {
        if (unkFlag)
        {
            int aLoad = m0x78[i] != 0;
            theDataSync.SyncLong(aLoad);
            m0x78[i] = aLoad != 0;
        }
        else
            theDataSync.SyncBool(m0x78[i]);

        theDataSync.SyncLong(mChallengeScores[i]);
        theDataSync.SyncLong(mTimeTrialScores[i]);
    }

    if (!unkFlag)
    {
        theDataSync.SyncLong(m0x7c);
        theDataSync.SyncLong(m0x80);
        theDataSync.SyncLong(mCheatCodeFlags);
    }

    for (int i = 0; i < 21; i++)
    {
        theDataSync.SyncLong(mAdventureScores[i]);
    }

    if (aVer >= 0x80000000)
        theDataSync.SyncLong(m0x88);

    if (theReader)
    {
        UnlockPetsForProgress();

        mNumOfUnlockedPets = 0;
        for (int i = 0; i < PET_END; i++)
            if (mUnlockedPets[i])
                mNumOfUnlockedPets++;
    }
}

void Sexy::UserProfile::UnlockPet(int thePetId, bool unlock)
{
    if (mUnlockedPets[thePetId] != unlock)
    {
        mUnlockedPets[thePetId] = unlock;
        if (!unlock)
            mNumOfUnlockedPets--;
        else
            mNumOfUnlockedPets++;
    }
}

// Unlocks one pet per adventure level already passed (all 20 once the game is finished)
void Sexy::UserProfile::UnlockPetsForProgress()
{
    int aNumPets = mTank * 5 + mLevel - 6;
    if (mTank >= 5)
        aNumPets--;
    if (mFinishedGame)
        aNumPets = 20;
    for (int i = 0; i < aNumPets; i++)
        UnlockPet(i, true);
}

void Sexy::UserProfile::AddShells(int theValue)
{
    mShells += theValue;
    if (mShells > 9999999)
        mShells = 9999999;
    else if (mShells < 0)
        mShells = 0;
}

bool Sexy::UserProfile::IsPetUnlocked(int idx)
{
    return mUnlockedPets[idx];
}

void Sexy::UserProfile::NextLevel()
{
    mLevel++;
    if (mTank == 4 && mLevel == 6)
    {
        mTank = 5;
        mLevel = 1;
    }
    if (mTank == 5 && mLevel >= 2)
    {
        mCyraxNum++;
        mFinishedGame = true;
    }
    if (mLevel > 6 || (mFinishedGame && mLevel > 5))
    {
        mTank++;
        mLevel = 1;
    }
    m0x58 = true;
}

// A separate function in the original (0x5013D0), reached only from ResetAfterFinalTank
void Sexy::UserProfile::RestartAdventure()
{
    mTank = 1;
    mLevel = 1;
    mFinishedGameCount = 0;
    m0x58 = false;
}

// Once Cyrax is beaten (NextLevel moves past 5-1) the adventure starts again at 1-1
void Sexy::UserProfile::ResetAfterFinalTank()
{
    if (mTank == 5 && mLevel >= 2)
        RestartAdventure();
}



SexyString Sexy::UserProfile::GetSaveGameFilePath(int theGameMode, int theUserId)
{
    // An if-chain in the original (cmp 5, 1, 4, 3 at 0x5058AC)
    const char* aGameModeString;
    if (theGameMode == GAMEMODE_VIRTUAL_TANK)
        aGameModeString = "sim";
    else if (theGameMode == GAMEMODE_TIME_TRIAL)
        aGameModeString = "tim";
    else if (theGameMode == GAMEMODE_CHALLENGE)
        aGameModeString = "sur";
    else if (theGameMode == GAMEMODE_SANDBOX)
        aGameModeString = "snd";
    else
        aGameModeString = "adv";

    return GetAppDataFolder() + StrFormat("userdata\\%s%d.dat", aGameModeString, theUserId);
}
