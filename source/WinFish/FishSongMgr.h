#ifndef __FISHSONGMGR_H__
#define __FISHSONGMGR_H__

#include "SexyAppFramework/Common.h"

namespace Sexy
{
	struct NoteData
	{
		float				mPitch;
		double				mVolume;
		int					mDuration;

		NoteData* 			Parse(const SexyChar* theNoteInfo);
	};

	class FishSongData
	{
	public:
		std::vector<NoteData>				mNoteVectors[3];
		float 								mLineVolumes[3];
		float 								mSpeed;
		int 								mGlobalPitchShift;
		int 								mLinePitchShifts[3];
		bool								mSkip;
		int									mCurrentLine;
		std::map<SexyString, SexyString, Sexy::StringLessNoCase>	mProperties;

	public:
		FishSongData();
		~FishSongData();

		void 				Init();

		bool 				Parse(const SexyChar* theFileName);
		bool 				ParseCommand(const SexyChar* theCommand);

		bool 				GetProperty(const SexyString& key, SexyString* outValue);
		int 				GetLineDuration(int theLine);
	};

	class FishSong
	{
	public:
		std::vector<NoteData> mNoteDataVector;
		int					mCurrentNoteIndex;
		DWORD				mStartTime;
		int 				mTotalDurationInSamples;
		int 				mTone;
		int 				mToneHi;
		int 				mToneLo;
		int 				mToneSuperHi;
		int 				mApplause;
		int 				mSongId;

	public:
		FishSong();
		~FishSong();

		void				Init(float theSpeed, int thePitchShift);
		bool				Update(DWORD theCurrentMusicTick, bool theAllowSound);
	};

	// The songs are stored by value
	typedef std::list<FishSong> FishSongList;

	class FishSongMgr
	{
	public:
		FishSongList		mSongList;
		int					mTone;
		int					mToneHi;
		int					mToneSuperHi;
		int					mToneLo;
		int					mCounter;
		DWORD				mLastUpdateTime;

	public:
		FishSongMgr();
		~FishSongMgr();

		void				Update();
		void				AddSong(const FishSong& theSong);
		void				StopFishSong(int theSongId);
		bool				IsSongInList(int theSongId);
		void				PausedGameDelaySong();
		FishSong*			PlayFishSong(FishSongData* theSongData, int theSongId);
		int					Unk01(int theSongId);
		void				ClearSongs();
	};
}

#endif