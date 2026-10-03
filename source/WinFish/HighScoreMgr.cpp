#include "HighScoreMgr.h"
#include "ProfileMgr.h"
#include "WinFishCommon.h"
#include "WinFishApp.h"
#include "DataSync.h"

using namespace Sexy;

Sexy::HighScoreMgr::HighScoreMgr()
{

}

Sexy::HighScoreMgr::~HighScoreMgr()
{
}

void Sexy::HighScoreMgr::ReadHighScoresData()
{
    ClearAllScoreLists();
    Buffer aBuf;
    if (!gSexyApp->ReadBufferFromFile(GetAppDataFolder() + "userdata/highscores.dat", &aBuf, false))
    {
        MakeDefaultHighScores();
    }
    else
    {
        // A truncated or corrupt file falls back to the default scores
        try
        {
            DataReader aDR;
            aDR.OpenMemory(aBuf.GetDataPtr(), aBuf.GetDataLen(), false);
            DataSync aDS(aDR);
            SyncData(&aDS);
        }
        catch (DataReaderException&)
        {
            MakeDefaultHighScores();
        }
    }
}

void Sexy::HighScoreMgr::SaveHighScoresData()
{
    DataWriter aDW;
    aDW.OpenMemory(32);
    DataSync aDS(aDW);
    SyncData(&aDS);
    MkDir(GetAppDataFolder() + "userdata");

    gSexyApp->WriteBytesToFile(GetAppDataFolder() + "userdata/highscores.dat", aDW.mMemoryHandle, aDW.mMemoryPosition);
}

void Sexy::HighScoreMgr::SyncData(DataSync* theSync)
{
    // mReader is read before the version is synced (0x513F9A)
    DataReader* aReader = theSync->mReader;
    int aVer = 2;
    theSync->SyncLong(aVer);
    theSync->mVersion = aVer;
    if (aReader != nullptr)
    {
        ClearAllScoreLists();
        if (aVer != 2)
        {
            MakeDefaultHighScores();
            return;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        SyncHighScoreList(theSync, mTimeTrialScores[i]);
        SyncHighScoreList(theSync, mChallengeScores[i]);
    }

    for (int i = 0; i < 21; i++)
        SyncHighScoreList(theSync, mAdventureScores[i]);
}

void Sexy::HighScoreMgr::SyncHighScoreList(DataSync* theSync, HighScoreList& theList)
{
    // Both pointers are read before the test (0x51308C)
    DataReader* aReader = theSync->mReader;
    DataWriter* aWriter = theSync->mWriter;
    if (aReader != nullptr)
    {
        theList.clear();
        int aSize = aReader->ReadShort();
        for (int i = 0; i < aSize; i++)
        {
            HighScoreEntry anEntry;
            anEntry.Sync(theSync);

            theList.push_back(anEntry);
        }
    }
    else
    {
        aWriter->WriteShort(theList.size());
        for (HighScoreList::iterator it = theList.begin(); it != theList.end(); ++it)
            it->Sync(theSync);
    }
}

void Sexy::HighScoreMgr::ClearAllScoreLists()
{
    for (int i = 0; i < 4; i++)
    {
        mTimeTrialScores[i].clear();
        mChallengeScores[i].clear();
    }
    for (int i = 0; i < 21; i++)
    {
        mAdventureScores[i].clear();
    }
}

struct HighScoreData
{
    const char* mUserName;
    int mScore;
};

void Sexy::HighScoreMgr::MakeDefaultHighScores()
{
    ClearAllScoreLists();

    HighScoreData gTimeTrialDefaults[20] = {
        {"Amy", 1000}, {"Ben", 900}, {"Brenna", 800}, {"Brian", 700}, {"Chad", 600},
        {"Damon", 1000}, {"Dave", 900}, {"Don", 800}, {"Eric", 700}, {"Greg", 600},
        {"Hans", 1000}, {"Jason", 900}, {"Jeff", 800}, {"John", 700}, {"Josh", 600},
        {"Juho", 1000}, {"Kathy", 900}, {"Katrina", 800}, {"Mark", 700}, {"Nick", 600}
    };

    HighScoreData gChalAdvDefaults[20] = {
        {"Shawn", 3300}, {"Sukhbir", 3360}, {"Tysen", 3420}, {"Walter", 3480}, {"jb63", 3540},
        {"MrsDbolt", 3300}, {"Lexi", 3360}, {"GrukX", 3420}, {"cheryl", 3480}, {"Rambobear", 3540},
        {"Kingdom", 3300}, {"Inkmei", 3360}, {"wolfpackmama", 3420}, {"hottentots", 3480}, {"Dyne", 3540},
        {"monkeyboy", 3300}, {"lilith", 3360}, {"HRACH", 3420}, {"Lovedog", 3480}, {"Syrinx", 3540}
    };

    // 21 entries (one per adventure level) but the last one (tank 5) is empty and not added.
    // All three arrays are filled before the loops in the original.
    HighScoreData gAdventureDefaults[21] = {
        {"HomerJay", 3300}, {"Zelda", 3360}, {"lala", 3420}, {"DixieWriter", 3480}, {"samspade", 3540},
        {"baasi1", 3300}, {"DaveDude", 3360}, {"Bubgrl", 3420}, {"Illyria", 3480}, {"hermitcrab", 3540},
        {"kane", 3300}, {"ClaireBear", 3360}, {"wuggies", 3420}, {"jebksb1977", 3480}, {"firedog", 3540},
        {"WoodRat", 3300}, {"Meatboy", 3360}, {"Splam11", 3420}, {"WendigoWolf", 3480}, {"bugsymcd24", 3540},
        {NULL, 0}
    };

    for (int i = 0; i < 2; i++)
    {
        int aDataIndex = 0;

        for (int tankIdx = 0; tankIdx < 4; tankIdx++)
        {
            // The list and the data array are chosen again on every pass (0x513593, 0x5135B0)
            HighScoreList* aCurList = (i == 0) ? &mTimeTrialScores[tankIdx] : &mChallengeScores[tankIdx];

            for (int j = 0; j < 5; j++)
            {
                const HighScoreData* aCurData = (i == 0) ? &gTimeTrialDefaults[aDataIndex] : &gChalAdvDefaults[aDataIndex];

                HighScoreEntry newEntry;
                newEntry.mUserName = aCurData->mUserName;
                newEntry.mScore = aCurData->mScore;

                aCurList->push_back(newEntry);

                aDataIndex++;
            }
        }
    }

    for (int i = 0; i < 20; i++)
    {
        HighScoreEntry newEntry;
        newEntry.mUserName = gAdventureDefaults[i].mUserName;
        newEntry.mScore = gAdventureDefaults[i].mScore;

        mAdventureScores[i].push_back(newEntry);
    }
}

// The best entry of an adventure level (an empty entry when the level has none)
HighScoreEntry& Sexy::HighScoreMgr::GetPerLevelEntry(int theTank, int theLevel)
{
    static HighScoreEntry sEmptyEntry;
    HighScoreList* aList = GetPerLevelScoresList(theTank, theLevel);
    if (aList->size() == 0)
        return sEmptyEntry;

    return aList->front();
}

HighScoreList* Sexy::HighScoreMgr::GetPerLevelScoresList(int theTank, int theLevel)
{
    static HighScoreList sEmptyList;
    int anIdx = GetLevelIndex(theTank, theLevel);
    if(anIdx < 0)
        return &sEmptyList;
    return &mAdventureScores[anIdx];
}

bool Sexy::HighScoreMgr::RecordAdventureHighScore(int theTank, int theLevel, UserProfile* theUser, int theScore)
{
    int anIdx = GetLevelIndex(theTank, theLevel);
    if (anIdx < 0)
        return false;

    int theMaxEntries = 1;
    bool sortDescending = false;
    if (theTank == 5)
    {
        sortDescending = true;
        theMaxEntries = 3;
    }
    theUser->WriteAdventureScore(theTank, theLevel, theScore);
    return AddScoreToList(&mAdventureScores[anIdx], theUser->mUserName, theScore, theMaxEntries, sortDescending, true);
}

// Returns true when the score made it into the list. With forceReload the scores are first reloaded from disk.
bool Sexy::HighScoreMgr::AddScoreToList(HighScoreList* theList, const SexyString& theUserName, int theScore, int theMaxEntries, bool sortDescending, bool forceReload)
{
    while ((int)theList->size() > theMaxEntries && theList->size() != 0)
        theList->pop_back();

    HighScoreList::iterator aPos = FindInsertionPos(theList, theScore, sortDescending);

    if (aPos == theList->end() && (int)theList->size() >= theMaxEntries)
        return false;

    if (forceReload)
    {
        ReadHighScoresData();
        return AddScoreToList(theList, theUserName, theScore, theMaxEntries, sortDescending, false);
    }

    HighScoreEntry newEntry;
    newEntry.mUserName = theUserName;
    newEntry.mScore = theScore;

    theList->insert(aPos, newEntry);

    while ((int)theList->size() > theMaxEntries && theList->size() != 0)
        theList->pop_back();

    SaveHighScoresData();
    return true;
}

bool Sexy::HighScoreMgr::RecordTimeTrialHighScore(int theTank, UserProfile* theUser, int theScore)
{
    if ((uint)(theTank - 1) > 3)
        return false;

    theUser->WriteTimeTrialScore(theTank, theScore);
    return AddScoreToList(&mTimeTrialScores[theTank - 1], theUser->mUserName, theScore, 5, true, true);
}

bool Sexy::HighScoreMgr::RecordChallengeHighScore(int theTank, UserProfile* theUser, int theScore)
{
    if ((uint)(theTank - 1) > 3)
        return false;

    theUser->WriteChallengeScore(theTank, theScore);
    return AddScoreToList(&mChallengeScores[theTank - 1], theUser->mUserName, theScore, 5, false, true);
}

HighScoreList::iterator Sexy::HighScoreMgr::FindInsertionPos(HighScoreList* theList, int theScore, bool sortDescending)
{
    HighScoreList::iterator it = theList->begin();

    while (it != theList->end())
    {
        if (sortDescending)
        {
            if (theScore > it->mScore)
                return it;
        }
        else
        {
            if (theScore < it->mScore)
                return it;
        }

        ++it;
    }

    return it;
}
