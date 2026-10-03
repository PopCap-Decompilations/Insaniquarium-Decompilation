#ifndef __HIGHSCOREMGR_H__
#define __HIGHSCOREMGR_H__

#include <SexyAppFramework/Common.h>
#include "DataSync.h"

namespace Sexy
{
	class UserProfile;

	struct HighScoreEntry
	{
		SexyString mUserName;
		int mScore;

		HighScoreEntry() { mScore = 0; }

		void Sync(DataSync* theSync)
		{
			theSync->SyncString(mUserName);
			theSync->SyncLong(mScore);
		}
	};

	typedef std::list<HighScoreEntry> HighScoreList;

	class HighScoreMgr
	{
	public:
		HighScoreList			mTimeTrialScores[4];
		HighScoreList			mChallengeScores[4];
		HighScoreList			mAdventureScores[21]; // One per level

	public:
		HighScoreMgr();
		virtual ~HighScoreMgr();

		void					ReadHighScoresData();
		void					SaveHighScoresData();
		void					SyncData(DataSync* theSync);
		static void				SyncHighScoreList(DataSync* theSync, HighScoreList& theList);

		void					ClearAllScoreLists();
		void					MakeDefaultHighScores();

		HighScoreEntry&			GetPerLevelEntry(int theTank, int theLevel);
		HighScoreList*			GetPerLevelScoresList(int theTank, int theLevel);

		bool					RecordAdventureHighScore(int theTank, int theLevel, UserProfile* theUser, int theScore);
		bool					AddScoreToList(HighScoreList* theList, const SexyString& theUserName, int theScore, int theMaxEntries, bool sortDescending, bool forceReload);
		bool					RecordTimeTrialHighScore(int theTank, UserProfile* theUser, int theScore);
		bool					RecordChallengeHighScore(int theTank, UserProfile* theUser, int theScore);

		static HighScoreList::iterator FindInsertionPos(HighScoreList* theList, int theScore, bool sortDescending);
	};
}

#endif