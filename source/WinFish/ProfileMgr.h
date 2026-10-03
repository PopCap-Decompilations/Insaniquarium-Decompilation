#ifndef __PROFILEMGR_H__
#define __PROFILEMGR_H__

#include "SexyAppFramework/Common.h"

namespace Sexy
{
	class DataSync;

	class UserProfile
	{
	public:
		bool mUnlockedPets[24];
		int mNumOfUnlockedPets;
		int mTank;
		int mLevel;
		SexyString mUserName;
		uint m0x40;
		int mId;
		int mShells;
		int m0x4c;
		int mCyraxNum;
		int mFinishedGameCount;
		bool m0x58;
		bool mFinishedGame;
		bool m0x5a[24];
		bool mUnlockedBackgrounds[6];
		bool m0x78[4];
		int m0x7c;
		int m0x80;
		uint mCheatCodeFlags;
		int m0x88;
		int m0x8c;
		int mBubbulatorBought;
		int mAlienAttractorBought;
		int m0x98;

		long long m0xa0;
		bool mStoreScreenBought[8];
		int mBonusItemId;
		int m0xb4;
		int mAdventureScores[21];
		int mTimeTrialScores[4];
		int mChallengeScores[4];

	public:
		UserProfile();

		void						LoadFromMemory();
		void						Init();
		void						SetCheatFlag(char thePos, bool theFlag);
		bool						ToggleCheatFlag(char theIdx);
		void						SyncUsersDat(DataSync& theDataSync);
		void						Save();
		void						SyncData(DataSync& theDataSync);
		void						UnlockPet(int thePetId, bool unlock);
		void						UnlockPetsForProgress();
		void						AddShells(int theValue);
		bool						IsPetUnlocked(int idx);
		void						NextLevel();
		void						RestartAdventure();
		void						ResetAfterFinalTank();
		static SexyString			GetSaveGameFilePath(int theGameMode, int theUserId);
		void						DeleteUserAppData();
		void						SaveScreenSaver(int unk);
		void						Unk01();
		void						WriteAdventureScore(int theTank, int theLevel, int theScore);
		void						WriteTimeTrialScore(int theTank, int theScore);
		void						WriteChallengeScore(int theTank, int theScore);
		int							ReadScreenSaverData(bool doErase);
		void						UpdateStoreData(long long theNewTime);
		int							GetAdventureScore(int theTank, int theLevel);
		int							GetTimeTrialScore(int theTank);
		int							GetChallengeScore(int theTank);
	};

	typedef std::map<SexyString, UserProfile, StringLessNoCase> UserProfilesMap;

	class ProfileMgr
	{
	public:
		UserProfilesMap				mProfilesMap;
		int							m0x10; // next profile id
		uint						m0x14; // next UserProfile::m0x40 use sequence

	public:
		ProfileMgr();
		virtual ~ProfileMgr();

		void						Clear();
		UserProfile*				GetUserProfile(const SexyString& theProfileName);
		UserProfile*				GetFirstUserProfile();
		void						SyncUsersDat(DataSync& theDataSync);
		void						ReadUsersDat();
		void						SaveUsersDat();
		UserProfile*				MakeNewUser(const SexyString& theUserName);
		bool						RenameUser(const SexyString& theOldUserName, const SexyString& theNewUserName);
		bool						DeleteUser(const SexyString& theUserName);
		void						DeleteProfile(UserProfilesMap::iterator theItr);
		void						DeleteOldestProfile();
		void						CapMaxUserCount();
	};
}

#endif