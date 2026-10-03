#include <SexyAppFramework/Common.h>
#include <SexyAppFramework/SoundManager.h>
#include <SexyAppFramework/SoundInstance.h>

#include "FishSongMgr.h"
#include "WinFishApp.h"

using namespace Sexy;

FishSongMgr::FishSongMgr()
{
	mTone = -1;
	mToneHi = -1;
	mToneLo = -1;
	mToneSuperHi = -1;
	mCounter = 0;
	mLastUpdateTime = 0;
}

FishSongMgr::~FishSongMgr()
{
}

void FishSongMgr::Update()
{
    DWORD aCurrentTime = GetTickCount();
    if (aCurrentTime == mLastUpdateTime)
        return;
    mLastUpdateTime = aCurrentTime;

    int aFinishedSongSoundID = -1;

    FishSongList::iterator anIterator = mSongList.begin();

    while (anIterator != mSongList.end())
    {
        FishSong& aCurrentSong = *anIterator;

        bool isStillPlaying = aCurrentSong.Update(aCurrentTime, true);

        if (!isStillPlaying)
        {
            if (aCurrentSong.mApplause != -1)
                aFinishedSongSoundID = aCurrentSong.mApplause;

            mSongList.erase(anIterator++);
        }
        else
        {
            ++anIterator;
        }
    }

    // A finished song with applause is followed by the applause sound (as the tone) after a 400 ms rest
    if (aFinishedSongSoundID != -1)
    {
        FishSong aSong;
        aSong.mTone = aFinishedSongSoundID;

        NoteData aNote;
        aNote.mPitch = -10000;
        aNote.mVolume = 1.0;
        aNote.mDuration = 400;
        aSong.mNoteDataVector.push_back(aNote);

        aNote.mPitch = 0;
        aNote.mVolume = 1.0;
        aNote.mDuration = 100;
        aSong.mNoteDataVector.push_back(aNote);
        AddSong(aSong);
    }
}

void Sexy::FishSongMgr::AddSong(const FishSong& theSong)
{
    mSongList.push_back(theSong);

    FishSong& aSong = mSongList.back();
    if (aSong.mTone == -1)
        aSong.mTone = mTone;
    if (aSong.mToneLo == -1)
        aSong.mToneLo = mToneLo;
    if (aSong.mToneHi == -1)
        aSong.mToneHi = mToneHi;
    if (aSong.mToneSuperHi == -1)
        aSong.mToneSuperHi = mToneSuperHi;
    aSong.Init(1.0f, 0);
}

void Sexy::FishSongMgr::StopFishSong(int theSongId)
{
    for (FishSongList::iterator it = mSongList.begin(); it != mSongList.end();)
    {
        if (it->mSongId == theSongId)
            mSongList.erase(it++);
        else
            ++it;
    }
}

bool Sexy::FishSongMgr::IsSongInList(int theSongId)
{
    for (FishSongList::iterator it = mSongList.begin(); it != mSongList.end(); ++it)
    {
        if (it->mSongId == theSongId)
            return true;
    }

    return false;
}

void Sexy::FishSongMgr::PausedGameDelaySong()
{
    DWORD aPausedTime = GetTickCount() - mLastUpdateTime;

    for (FishSongList::iterator it = mSongList.begin(); it != mSongList.end(); ++it)
        it->mStartTime += aPausedTime;
}

// Starts one song per non-empty line and returns the longest one
FishSong* Sexy::FishSongMgr::PlayFishSong(FishSongData* theSongData, int theSongId)
{
    FishSong* aLongestSong = nullptr;
    uint aLongestDur = 0;

    for (int i = 0; i < 3; i++)
    {
        std::vector<NoteData>& aNotes = theSongData->mNoteVectors[i];
        if (aNotes.size() == 0)
            continue;

        mSongList.push_back(FishSong());
        FishSong& aFishSong = mSongList.back();

        aFishSong.mSongId = theSongId;

        for (int j = 0; j < (int)aNotes.size(); j++)
            aFishSong.mNoteDataVector.push_back(aNotes[j]);

        aFishSong.mTone = mTone;
        aFishSong.mToneLo = mToneLo;
        aFishSong.mToneHi = mToneHi;
        aFishSong.mToneSuperHi = mToneSuperHi;
        aFishSong.Init(theSongData->mSpeed, theSongData->mGlobalPitchShift);

        if (aLongestSong == nullptr || (uint)aFishSong.mTotalDurationInSamples > aLongestDur)
        {
            aLongestSong = &aFishSong;
            aLongestDur = aFishSong.mTotalDurationInSamples;
        }
    }

    return aLongestSong;
}

int Sexy::FishSongMgr::Unk01(int theSongId)
{
    FishSongList::iterator anIterator = mSongList.begin();

    uint aVal = 0;
    while (anIterator != mSongList.end())
    {
        if (anIterator->mSongId == theSongId && mLastUpdateTime - anIterator->mStartTime > aVal)
            aVal = mLastUpdateTime - anIterator->mStartTime;

        ++anIterator;
    }
    return aVal;
}

void Sexy::FishSongMgr::ClearSongs()
{
    mSongList.clear();
}

Sexy::FishSong::FishSong()
{
    mCurrentNoteIndex = 0;
    mStartTime = 0;
    mTone = -1;
    mToneHi = -1;
    mToneLo = -1;
    mToneSuperHi = -1;
    mApplause = -1;
    mSongId = -1;
    mTotalDurationInSamples = 0;
}

Sexy::FishSong::~FishSong()
{
}

// Turns the note durations into start times (truncated) and applies the pitch shift
void Sexy::FishSong::Init(float theSpeed, int thePitchShift)
{
    float aTotalDuration = 0.0f;

    for (int i = 0; i < (int)mNoteDataVector.size(); i++)
    {
        NoteData& aNote = mNoteDataVector[i];
        float aThisDur = (float)((double)(uint)aNote.mDuration * theSpeed);
        aNote.mDuration = (int)aTotalDuration;
        aNote.mPitch = aNote.mPitch + (float)thePitchShift;
        aTotalDuration = aTotalDuration + aThisDur;
    }
    mTotalDurationInSamples = (int)aTotalDuration;
    mStartTime = GetTickCount();
}

bool Sexy::FishSong::Update(DWORD theCurrentMusicTick, bool theAllowSound)
{
    DWORD anElapsed = theCurrentMusicTick - mStartTime;

    while (mCurrentNoteIndex < (int)mNoteDataVector.size())
    {
        NoteData& aNote = mNoteDataVector[mCurrentNoteIndex];

        if (anElapsed < (DWORD)aNote.mDuration)
            break;

        // -1000.0 is a double constant (fcompl 0x59D5A0); the other bounds are floats
        if (aNote.mPitch > -1000.0)
        {
            // The high/low tones are only used when they are set; otherwise the base tone plays unshifted.
            // The comparisons use the operators the original tests with (0x503B32-0x503B7E).
            int aSoundId = mTone;
            int aPitchShift = 0;
            if (aNote.mPitch <= -12.0f)
            {
                if (mToneLo >= 0)
                {
                    aPitchShift = 12;
                    aSoundId = mToneLo;
                }
            }
            else if (aNote.mPitch >= 12.0f)
            {
                if (mToneSuperHi >= 0 && aNote.mPitch >= 24.0f)
                {
                    aSoundId = mToneSuperHi;
                    aPitchShift = -24;
                }
                else if (mToneHi >= 0)
                {
                    aPitchShift = -12;
                    aSoundId = mToneHi;
                }
            }

            if (theAllowSound)
            {
                SoundInstance* anInst = gSexyApp->mSoundManager->GetSoundInstance(aSoundId);
                if (anInst)
                {
                    anInst->SetVolume(aNote.mVolume);
                    anInst->AdjustPitch((double)aPitchShift + aNote.mPitch);
                    anInst->Play(false, true);
                }
            }
        }

        mCurrentNoteIndex++;
    }

    return mCurrentNoteIndex < (int)mNoteDataVector.size();
}

FishSongData::FishSongData()
{
    Init();
}

Sexy::FishSongData::~FishSongData()
{
}

void Sexy::FishSongData::Init()
{
    mSpeed = 3;
    mGlobalPitchShift = 0;
    for (int i = 0; i < 3; i++)
    {
        mNoteVectors[i].clear();

        mLinePitchShifts[i] = 0;
        mLineVolumes[i] = 1.0f;
    }
}

// Lines keep their newline (sscanf/Trim ignore it); a note line is split at commas and blank notes are skipped.
// Parse errors only go to gFishSongParseError (LoadFishSongs writes them to fishsongerror.txt).
bool Sexy::FishSongData::Parse(const SexyChar* theFileName)
{
    Init();
    gFishSongParseError = "";
    mSkip = false;
    FILE* aFile = fopen(theFileName, "r");
    if (aFile == nullptr)
    {
        gFishSongParseError = StrFormat("File not found: %s", theFileName);
        return false;
    }

    mCurrentLine = 0;

    int aCurLineNum = 0;
    char aLineBuffer[8192];

    while (!feof(aFile))
    {
        if (gFishSongParseError.size() != 0)
            break;

        aCurLineNum++;
        if (fgets(aLineBuffer, 8000, aFile) == nullptr)
            break;

        if (aLineBuffer[0] == '#')
            continue;

        if (aLineBuffer[0] == '*')
            ParseCommand(aLineBuffer);
        else if (!mSkip)
        {
            SexyChar* token = strtok(aLineBuffer, ",");
            while (token != nullptr)
            {
                SexyString aNoteDataStr = Trim(token);
                if (aNoteDataStr.size() != 0)
                {
                    NoteData aNewNote;
                    mNoteVectors[mCurrentLine].push_back(*aNewNote.Parse(aNoteDataStr.c_str()));

                    NoteData& aNoteInVector = mNoteVectors[mCurrentLine].back();

                    int aPitchShift = mGlobalPitchShift + mLinePitchShifts[mCurrentLine];
                    aNoteInVector.mPitch = (float)((double)aPitchShift + aNoteInVector.mPitch);
                    aNoteInVector.mDuration = (int)((double)(uint)aNoteInVector.mDuration * mSpeed);
                    aNoteInVector.mVolume = mLineVolumes[mCurrentLine];

                    if (gFishSongParseError.size() != 0)
                        break;
                }

                token = strtok(NULL, ",");
            }
        }
    }

    fclose(aFile);

    if (gFishSongParseError.size() != 0)
    {
        gFishSongParseError = StrFormat("%s: %s line %d", gFishSongParseError.c_str(), theFileName, aCurLineNum);
        return false;
    }

    mGlobalPitchShift = 0;
    mSpeed = 1.0f;
    return true;
}

bool Sexy::FishSongData::ParseCommand(const SexyChar* theCommand)
{
    char aCommandKey[512];
    char aCommandValue[512];

    if (sscanf(theCommand + 1, "%s%s", aCommandKey, aCommandValue) != 2)
    {
        if (stricmp(aCommandKey, "on") == 0)
        {
            mSkip = false;
            return true;
        }
        if (stricmp(aCommandKey, "off") == 0)
        {
            mSkip = true;
            return true;
        }
        gFishSongParseError = "Unrecognized command";
        return false;
    }

    if (stricmp(aCommandKey, "skip") == 0)
    {
        mSkip = stricmp(aCommandValue, "true") == 0;
        return true;
    }

    if (mSkip)
        return true;

    if (stricmp(aCommandKey, "line") == 0)
    {
        int aLine = atol(aCommandValue);
        if ((uint)(aLine - 1) > 2)
            gFishSongParseError = "Invalid line";
        else
            mCurrentLine = aLine - 1;
        return true;
    }

    if (stricmp(aCommandKey, "rest") == 0)
    {
        int aLine = atol(aCommandValue);
        if ((uint)(aLine - 1) <= 2)
        {
            uint aCurLineDur = GetLineDuration(mCurrentLine);
            uint aRestLineDur = GetLineDuration(aLine - 1);

            if (aCurLineDur < aRestLineDur)
            {
                NoteData aNewNote;
                aNewNote.mPitch = -10000;
                aNewNote.mDuration = aRestLineDur - aCurLineDur;
                aNewNote.mVolume = 1.0;
                mNoteVectors[mCurrentLine].push_back(aNewNote);
                return true;
            }
        }
        return true;
    }

    if (stricmp(aCommandKey, "attrib") == 0)
    {
        // "*attrib name = value": the name is the second word, the value is everything after '='
        SexyString aValue;
        const char* anEq = strchr(theCommand, '=');
        if (anEq)
            aValue = Trim(SexyString(anEq + 1));

        mProperties[SexyString(aCommandValue)] = aValue;
        return true;
    }

    if (stricmp(aCommandKey, "speed") == 0)
    {
        mSpeed = (float)atof(aCommandValue);
        return true;
    }

    if (stricmp(aCommandKey, "volume") == 0)
    {
        mLineVolumes[mCurrentLine] = (float)atof(aCommandValue);
        return true;
    }

    if (stricmp(aCommandKey, "shift") == 0)
    {
        mGlobalPitchShift = atol(aCommandValue);
        return true;
    }

    if (stricmp(aCommandKey, "localshift") == 0)
    {
        mLinePitchShifts[mCurrentLine] = atol(aCommandValue);
        return true;
    }

    gFishSongParseError = "Unrecognized command";
    return false;
}

bool Sexy::FishSongData::GetProperty(const SexyString& key, SexyString* outValue)
{
    auto& propertiesMap = mProperties;

    auto it = propertiesMap.find(key);

    if (it == propertiesMap.end())
        return false;

    if (outValue != nullptr)
        *outValue = it->second;

    return true;
}

int Sexy::FishSongData::GetLineDuration(int theLine)
{
    int aDur = 0;
    for (std::vector<NoteData>::iterator it = mNoteVectors[theLine].begin(); it != mNoteVectors[theLine].end(); ++it)
        aDur += it->mDuration;

    return aDur;
}

NoteData* Sexy::NoteData::Parse(const SexyChar* theNoteInfo)
{
    mPitch = 0;
    mVolume = 1.0;
    mDuration = 0;

    char aStr1[256];
    char aDurStr[256];
    char aStr3[256];

    int aVal = sscanf(theNoteInfo, "%s%s%s", aStr1, aDurStr, aStr3);
    if (aVal != 2)
    {
        gFishSongParseError = "Invalid note";
        if (aVal == 3)
            gFishSongParseError += " (missing comma?)";
        return this;
    }

    // The case blocks are in the original's order (C D E F G A B, rest)
    int aPitch = 0;
    switch (tolower(aStr1[0]))
    {
    case 'c':
        aPitch = 0;
        break;
    case 'd':
        aPitch = 2;
        break;
    case 'e':
        aPitch = 4;
        break;
    case 'f':
        aPitch = 5;
        break;
    case 'g':
        aPitch = 7;
        break;
    case 'a':
        aPitch = 9;
        break;
    case 'b':
        aPitch = 11;
        break;
    case 'r':
        aPitch = -10000;
        break;
    default:
        gFishSongParseError = "Invalid note";
        return this;
    }

    int aPitchMod = 4;
    if (aStr1[1] != '\0')
    {
        if (isdigit(aStr1[1]))
        {
            aPitchMod = aStr1[1] - '0';
        }
        else
        {
            if (aStr1[1] == '#')
                aPitch++;
            else if (aStr1[1] == 'b')
                aPitch--;
            else
                return this; // no error, and the pitch stays 0

            if (isdigit(aStr1[2]))
                aPitchMod = aStr1[2] - '0';
        }
    }

    if (isdigit(aDurStr[0]))
    {
        mDuration = atol(aDurStr);
    }
    else
    {
        const char* pos = aDurStr;
        while (*pos != '\0')
        {
            // The case blocks are in the original's order (shortest note first)
            int aBaseDur;
            switch (tolower(*pos))
            {
            case 'z':
                aBaseDur = 3;
                break;
            case 't':
                aBaseDur = 6;
                break;
            case 's':
                aBaseDur = 12;
                break;
            case 'e':
                aBaseDur = 24;
                break;
            case 'q':
                aBaseDur = 48;
                break;
            case 'h':
                aBaseDur = 96;
                break;
            case 'w':
                aBaseDur = 192;
                break;
            default:
                gFishSongParseError = "Invalid duration";
                return this;
            }
            pos++;

            if (tolower(*pos) == 't')
            {
                aBaseDur = (aBaseDur * 2) / 3;
                pos++;
            }

            int aCurNoteDur = aBaseDur;

            while (tolower(*pos) == 'd')
            {
                pos++;
                aBaseDur /= 2;
                aCurNoteDur += aBaseDur;
            }

            mDuration += aCurNoteDur;

            if (*pos == '+')
                pos++;
            else
            {
                if (*pos != '\0')
                    gFishSongParseError = "Invalid note (missing comma?)"; // the pitch is still set
                break;
            }
        }
    }

    mPitch = (float)(aPitch + (aPitchMod * 3 - 12) * 4);
    return this;
}
