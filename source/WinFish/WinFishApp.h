#ifndef __WINFISHAPP_H__
#define __WINFISHAPP_H__

#include "SexyApp.h"
#include <SexyAppFramework/MTRand.h>
#include <SexyAppFramework/Color.h>
#include <SexyAppFramework/KeyCodes.h>

namespace Sexy
{
	class Board;
	class TitleScreen;
	class GameSelector;
	class PetsScreen;
	class StoreScreen;
	class HatchScreen;
	class BonusScreen;
	class InterludeScreen;
	class StoryScreen;
	class SimSetupScreen;
	class SimFishScreen;
	class TankScreen;
	class HighScoreScreen;
	class HelpScreen;

	class WorkerThread;
	class HighScoreMgr;
	class ProfileMgr;
	class UserProfile;
	class FishSongMgr;
	class FishSongData;

	class GameObject;
	class Fish;
	class OtherTypePet;

	extern bool gUnkBool01;
	extern bool gDoHundredUpdates;
	extern bool gUnkBool03;
	extern bool gUnkBool04;
	extern bool gUnkBool05;
	extern bool gUnkBool06;
	extern bool gUnkBool09;
	extern bool gCanRemapMusic;
	extern bool gFishSongsLoaded;
	extern bool gScreenSaverStarted;
	extern int gUpdateFramesCounter;
	extern bool gFirstVirtualTankEnter;
	extern int gUsersDatVersion;
	extern int gFoodLimit;
	extern int gUnkInt01;
	extern FishSongData* gKilgoreSongDataPtr;
	extern FishSongData* gTestSongDataPtr;
	extern int gUnkInt03;
	extern int gUnkInt07;
	extern int gPetsDiedOnBossLevel;
	extern int gUnkInt09;
	extern int gUnkInt10;
	extern int gUnkInt11;
	extern int gLastStoryId;
	extern int gDeadPetsIdArray[24];
	extern int gUnkIntArray02[100];
	extern SexyString gFishSongParseError;

	extern std::vector<FishSongData> gSongsVector1; // every parsed song (by value); the other lists point into it
	extern std::vector<FishSongData*> gSongsVector2;
	extern std::vector<FishSongData*> gLudwigSongs;
	extern std::vector<FishSongData*> gSantaSongs;
	extern std::map<SexyString, FishSongData*, Sexy::StringLessNoCase> gLongSongsMap;

	extern int gUnkInt04;
	extern int gLudwigSongId;
	extern int gSantaSongId;
	extern int gSongs2Id;

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	// Characters are matched case-insensitively; a key is stored as a 0 byte followed by the key code
	struct CheatCode
	{
		SexyString		mCheatString;
		uint			mCurrentIndex;

		CheatCode() { mCurrentIndex = 0; }
		CheatCode(const SexyString& theStr) { SetCode(theStr); }

		void SetCode(const SexyString& theStr)
		{
			mCheatString = theStr;
			mCurrentIndex = 0;
		}

		void AddKey(KeyCode theKey)
		{
			mCheatString.append(1, '\0');
			mCheatString.append(1, (char)theKey);
		}

		void AddChar(SexyChar theChar)
		{
			mCheatString.append(1, theChar);
		}

		bool CheckCodeActivated(SexyChar theChar)
		{
			if (mCheatString.size() == 0)
				return false;
			if (tolower((uchar)mCheatString[mCurrentIndex]) != tolower((uchar)theChar))
			{
				mCurrentIndex = 0;
				return false;
			}
			if (++mCurrentIndex != mCheatString.size())
				return false;
			mCurrentIndex = 0;
			return true;
		}

		bool CheckCodeActivated(KeyCode theKey)
		{
			if ((int)mCheatString.size() <= (int)(mCurrentIndex + 1))
				return false;
			if (mCheatString[mCurrentIndex] != '\0')
				return false;
			if (mCheatString[mCurrentIndex + 1] != (char)theKey)
			{
				mCurrentIndex = 0;
				return false;
			}
			mCurrentIndex += 2;
			if (mCurrentIndex != mCheatString.size())
				return false;
			mCurrentIndex = 0;
			return true;
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	enum GameModes
	{
		GAMEMODE_ADVENTURE,
		GAMEMODE_TIME_TRIAL,
		GAMEMODE_PLACEHOLDER,
		GAMEMODE_SANDBOX,
		GAMEMODE_CHALLENGE,
		GAMEMODE_VIRTUAL_TANK
	};

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	enum DialogIds
	{
		DIALOG_OPTIONS = 1,
		DIALOG_REGISTER,
		DIALOG_UPDATE_ASK = 5,
		DIALOG_UPDATE_CHECK,
		DIALOG_INVALID_CODE,
		DIALOG_URL_INFO = 9,
		DIALOG_QUIT_GAME = 11,
		DIALOG_INFO = 14,
		DIALOG_FISH_NAMING,
		DIALOG_FIRST_LVL_GAME_OVER,
		DIALOG_GAME_OVER,
		DIALOG_LOST_FOCUS,
		DIALOG_OUT_OF_MONEY_LOAN,
		DIALOG_LEAVE_GAME = 22,
		DIALOG_THANKS_FOR_REGISTER,
		DIALOG_USER_DIALOG,
		DIALOG_NEW_USER,
		DIALOG_ARE_YOU_SURE_DELETE,
		DIALOG_RENAME,
		DIALOG_INFO_NEW_USER,
		DIALOG_NAME_CONFLICT,
		DIALOG_CONTINUE_GAME,
		DIALOG_PET_DIALOG,
		DIALOG_FOOD,
		DIALOG_RESTART_GAME = 34,
		DIALOG_CONFIRM_PURCHASE,
		DIALOG_SELL_DIALOG,
		DIALOG_SCREENSAVER,
		DIALOG_TIMES_UP,
		DIALOG_VIRTUAL,
		DIALOG_GIVE_SHELLS,
		DIALOG_END_ID
	};

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	class WinFishApp : public SexyApp
	{
	public:
		Board*							mBoard;
		TitleScreen*					mTitleScreen;
		GameSelector*					mGameSelector;
		Image**							mPrestoMenuData; // RESOURCE_ID_MAX images by resource id (the scaled pet images at IMAGE_STINKY_ID + pet)
		time_t							mDaysSinceFirstRun;
		time_t							mDaysSinceLastRun;
		int								mLDAccum;
		std::list<void*>				m0x754;
		std::list<void*>				m0x760;
		SexyString						mScreenSaverRegKey;
		SexyString						mScreenSaverRegPath;
		int								m0x7a4;
		int								m0x7a8;
		int								m0x7ac;
		MTRand*							mSeed;
		WorkerThread*					mWorkerThread;
		SexyString						mReferId;
		SexyString						m0x7d8;
		SexyString						m0x7f4;
		PetsScreen*						mPetsScreen;
		StoreScreen*					mStoreScreen;
		HatchScreen*					mHatchScreen;
		BonusScreen*					mBonusScreen;
		InterludeScreen*				mInterludeScreen;
		StoryScreen*					mStoryScreen;
		SimSetupScreen*					mSimSetupScreen;
		SimFishScreen*					mSimFishScreen;
		TankScreen*						mTankScreen;
		HighScoreScreen*				mHighScoreScreen;
		HelpScreen*						mHelpScreen;
		bool							mScreenSaverEnabled;
		bool							mScreenSaverSound;
		bool							mScreenSaverRotateBackdrops;
		bool							mScreenSaverPeriodicDim;
		bool							mScreenSaverShowMoney;
		bool							mScreenSaverUnk01;
		bool							mScreenSaverPowerSave;
		SexyString						mScreenSaverOldPath;
		SexyString						mScreenSaverUserName;
		bool							m0x878;
		int								mGameMode;
		bool							mRelaxMode;
		bool							m0x881;
		bool							m0x882;
		bool							mGameNotPlayed;
		bool							m0x884;
		int								mTankGameModeChoose;
		int								mCurrentSongId;
		int								mCurrentSongOffset;
		int								mPlaytimeCounter;
		int								mPlaytimeCounter2;
		int								mMaxExecutions;
		int								mMaxPlays;
		int								mMaxTime;
		bool							m0x8a8;
		bool							m0x8a9;
		bool							m0x8aa;
		bool							m0x8ab;
		HighScoreMgr*					mHighScoreMgr;
		ProfileMgr*						mProfileMgr;
		OtherTypePet*					mPrestoPtr;
		UserProfile*					mCurrentProfile;

	public:
		WinFishApp();

		virtual ~WinFishApp();
		virtual void Init();

		bool						IsScreenSaver(); // hides SexyAppBase::IsScreenSaver, which it calls
		int							GetCurrentTank();
		void						PlayMusic(int theSongId, int theSongOffset = 0, bool noLoop = false);
		void						SomeMusicFunc(bool flag);
		void						FadeOutMusic(int theSongId, bool stopSong, double theSpeed);
		void						FadeInMusic(int theSongId, int theSongOffset, double theSpeed, bool noLoop);
		static void					RemapMusicTrack(int* theSongId, int* theSongOffset);
		bool						IsSongPlaying(int theSongId, int theSongOffset);
		void						StopMusic();
		void						StartGameMusic();
		void						SomeMusicPlayFunc(bool flag);
		void						StartScreenSaver();
		static void					SetScreenSaver(const char* thePath);
		void						ReadSSFromRegistry();
		void						WriteSSToRegistry();
		static void					GetScreenSaverFilePath(SexyString& theScrPath, SexyString& theExePath);
		static SexyString			GetSystemScreenSaverPath();
		bool						ShouldKillDialog();
		void						CleanDialogs();
		bool						KillFoodDialog();
		void						ApplyPrestoMorph(int thePetId);
		bool						SaveCurrentUserData();
		void						SaveVirtualTankAndUserData();
		void						ApplyGiveShells(bool doApply);
		void						ApplyScreenSaverSettings();
		void						ApplyFishNameDialog();
		void						ApplySellGameObject(bool sell);
		void						ConfirmFishPurchase(bool confirmed);
		void						InitializeSpecialNamedFish(GameObject* theObject);
		static bool					DoScrCopy(const SexyString& theExePath);
		bool						CanShowRegisterDialog();
		void						CloseNameWarningDialog(int theDialogId);
		void						TimesUpDialogDone();

		void						SwitchToBoard(bool theFlag1, bool theFlag2);
		void						SwitchToGameSelector();
		void						SwitchToHelpScreen(bool instructions);
		void						SwitchToHighScoreScreen();
		void						SwitchToPetsScreen();
		void						SwitchToHatchScreen(int thePetId);
		void						SwitchToInterludeScreen();
		void						SwitchToSimSetupScreen();
		void						SwitchToTankScreen();
		void						SwitchToStoryScreen(int unk);
		void						SwitchToStoreScreen();
		void						SwitchToSimFishScreen();
		void						SwitchToBonusScreen();

		void						RemoveBoard();
		void						RemoveGameSelector();
		void						RemoveHelpScreen();
		void						RemoveHighScoreScreen();
		void						RemovePetsScreen();
		void						RemoveHatchScreen();
		void						RemoveSimSetupScreen();
		void						RemoveTankScreen();
		void						RemoveStoryScreen();
		void						RemoveStoreScreen();
		void						RemoveSimFishScreen();
		void						RemoveBonusScreen();
		void						RemoveInterludeScreen();

		void						LeaveGameBoard();

		void						UpdatePlayData();
		bool						CheckTrialEnded();

		void						DoTrialVersionExpiredDialog();
		void						DoUpdateDialog();
		void						DoContinueDialog();
		void						DoNewUserDialog();
		void						DoOptionsDialog(bool theFlag);
		void						DoQuitDialog();
		void						DoWhoAreYouDialog();
		void						DoGiveDialog();
		void						DoDeleteWarningDialog(SexyString& theName);
		void						DoRenameDialog(const SexyString& theUserName);
		void						DoUpdateCheckDialog();
		void						DoLeaveGameDialog();
		void						DoLostFocusDialog();
		void						DoVirtualDialog();
		void						DoScreenSaverDialog();
		void						DoAreYouSureSellDialog(const SexyString& theLine);
		void						OpenPrestoDialog(GameObject* thePet);
		void						DoConfirmPurchaseDialog(const SexyString& theLine);
		void						DoFishNamingDialog(const SexyString& theLine, bool isBreeder, const SexyString& theName, bool flag); // isBreeder is unused
		void						DoFoodDialog();
		void						DoTimesUpDialog();
		void						DoRegisterDialog();
		void						DoInvalidCodeDialog();

		Dialog*						DoDialogUnkF(int theId, bool isModal, const SexyString& theDiaHeader, const SexyString& theDiaLines, const SexyString& theDiaFooter, int theBtnMode);

		void						MakeNewUser(bool makeUser);
		void						DeleteUser(bool doDelete);
		void						RenameUser(bool doRename);
		void						StartGame();
		bool						LoadBoardGame();
		void						CreateBoard();
		void						UserDialogOkPressed(bool applyChanges);
		bool						IsBonusLevel();
		bool						ApplyOptionsSettings();
		void						RestartLevel();

		virtual void				Start();
		virtual void				Shutdown();
		virtual void				ReadFromRegistry();
		virtual void				WriteToRegistry();
		virtual bool				ChangeDirHook(const char* theIntendedPath);
		virtual void				LogScreenSaverError(const std::string& theError);
		virtual void				UpdateFrames();
		virtual void				LoadingThreadProc();
		virtual void				LoadingThreadCompleted();
		virtual MusicInterface*		CreateMusicInterface(HWND theHWnd);
		virtual void				URLOpenFailed(const std::string& theURL);
		virtual void				URLOpenSucceeded(const std::string& theURL);
		virtual bool				OpenURL(const std::string& theURL, bool shutdownOnOpen = false);
		virtual Dialog*				DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
		virtual bool				KillDialog(int theDialogId);
		virtual Dialog*				NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode);
		virtual void				PreDisplayHook(); // [26] empty: skips SexyApp's beta validation
		virtual void				SetMusicVolume(double theVolume);
		virtual void				ModalOpen();
		virtual void				LostFocus();
		virtual bool				DebugKeyDown(int theKey);

		virtual void				ButtonDepress(int theId);

		// WinFishApp Virtual Functions
		virtual void				TitleScreenIsFinished(); // [104]
		virtual void				Blank(); // [105] empty
		virtual void				WFAShowResourceError(bool doExit = false); // [106]
	};
}

#endif