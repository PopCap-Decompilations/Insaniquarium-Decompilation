#include <SexyAppFramework/SEHCatcher.h>
#include <SexyAppFramework/SexyAppBase.h>
#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/ResourceManager.h>
#include <SexyAppFramework/ImageFont.h>
#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/SWTri.h>
#include <SexyAppFramework/Checkbox.h>
#include <SexyAppFramework/BassMusicInterface.h>
#include <SexyAppFramework/SoundManager.h>
#include <SexyAppFramework/Buffer.h>
#include <SexyAppFramework/Color.h>
#include <SexyAppFramework/ListWidget.h>

#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "InternetManager.h"
#include "Res.h"

#include "Fish.h"
#include "Bilaterus.h"

#include "Board.h"
#include "TitleScreen.h"
#include "HelpScreen.h"
#include "StoreScreen.h"
#include "HatchScreen.h"
#include "GameSelector.h"
#include "BonusScreen.h"
#include "HighScoreScreen.h"
#include "InterludeScreen.h"
#include "StoryScreen.h"
#include "SimFishScreen.h"
#include "TankScreen.h"
#include "PetsScreen.h"
#include "SimSetupScreen.h"

#include "WorkerThread.h"
#include "HighScoreMgr.h"
#include "ProfileMgr.h"
#include "FishSongMgr.h"

#include "MoneyDialog.h"
#include "ContinueDialog.h"
#include "NewUserDialog.h"
#include "OptionsDialog.h"
#include "UserDialog.h"
#include "UpdateCheckDialog.h"
#include "VirtualDialog.h"
#include "PetDialog.h"
#include "ScreenSaverDialog.h"
#include "FishNamingDialog.h"
#include "FoodDialog.h"
#include "RegisterDialog.h"

#include <chrono>
#include <ctime>

namespace Sexy
{
	bool gUnkBool01 = false;
	bool gDoHundredUpdates = false;
	bool gUnkBool03 = false;
	bool gUnkBool04 = false;
	bool gUnkBool05 = false;
	bool gUnkBool06 = false;
	bool gUnkBool09 = false;
	bool gCanRemapMusic = true;
	bool gFishSongsLoaded = false;
	int gUsersDatVersion = 14;
	bool gScreenSaverStarted;
	int gUpdateFramesCounter = 0;
	bool gFirstVirtualTankEnter = false;
	int gFoodLimit = 1;
	int gUnkInt01 = 0;
	FishSongData* gKilgoreSongDataPtr = nullptr;
	FishSongData* gTestSongDataPtr = nullptr;
	int gUnkInt03 = 0;
	int gUnkInt07 = 5000;
	int gPetsDiedOnBossLevel = 0;
	int gUnkInt09 = -1;
	int gUnkInt10 = -1;
	int gUnkInt11 = 2160;
	int gLastStoryId = 0;
	int gDeadPetsIdArray[24] = {0};
	int gUnkIntArray02[100] = {0};
	SexyString gFishSongParseError;
	std::vector<FishSongData> gSongsVector1;
	std::vector<FishSongData*> gSongsVector2;
	std::vector<FishSongData*> gLudwigSongs;
	std::vector<FishSongData*> gSantaSongs;
	std::map<SexyString, FishSongData*, Sexy::StringLessNoCase> gLongSongsMap;
	int gUnkInt04 = 0;
	int gLudwigSongId = 0;
	int gSantaSongId = 0;
	int gSongs2Id = 0;
}

using namespace Sexy;

void LoadFishSongs(bool flag)
{
	if (!flag && gFishSongsLoaded)
		return;
	gFishSongsLoaded = true;

	Sexy::gSongsVector1.clear();
	Sexy::gSongsVector2.clear();
	Sexy::gLudwigSongs.clear();
	Sexy::gLongSongsMap.clear();

	gKilgoreSongDataPtr = nullptr;
	gTestSongDataPtr = nullptr;

	WIN32_FIND_DATAA aFindFileData;
	HANDLE hFind = FindFirstFileA("fishsongs\\*.txt", &aFindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return;

	bool hasOpenedErrorFile = false;
	FILE* anErrorFile = nullptr;

	do
	{
		SexyString aFilePath = "fishsongs\\";
		aFilePath.append(aFindFileData.cFileName);

		gSongsVector1.push_back(FishSongData());
		FishSongData* aNewSong = &gSongsVector1.back();

		if (!aNewSong->Parse(aFilePath.c_str()))
		{
			if (!hasOpenedErrorFile)
			{
				hasOpenedErrorFile = true;
				anErrorFile = fopen("fishsongerror.txt", "w");
			}
			if (anErrorFile != nullptr)
				fprintf(anErrorFile, "%s - %s\n", aFindFileData.cFileName, gFishSongParseError.c_str());

			gSongsVector1.pop_back(); // VS2005's pop_back tests empty() itself

			// The original has no else here: it runs the '_' check below for the removed song too, and for a
			// *_long.txt/*_short.txt file it indexes that song's property map, whose head pointer the destructor
			// has nulled, so the original crashes. WinFish skips the property code for a song that failed to parse.
		}
		else
		{
			char* aProperty = strchr(aFindFileData.cFileName, '_');
			if (aProperty != 0)
			{
				*aProperty = '\0';
				if (stricmp(aProperty + 1, "long.txt") == 0)
					aNewSong->mProperties["long"] = aFindFileData.cFileName;
				else if(stricmp(aProperty + 1, "short.txt") == 0)
					aNewSong->mProperties["short"] = aFindFileData.cFileName;
			}
		}

	} while (FindNextFileA(hFind, &aFindFileData) != 0);

	FindClose(hFind);
	if(anErrorFile != nullptr)
		fclose(anErrorFile);

	for (std::vector<FishSongData>::iterator it = gSongsVector1.begin(); it != gSongsVector1.end(); ++it)
	{
		FishSongData* aSong = &*it;

		bool addToSongsVec2 = true;
		SexyString aValue;

		if (aSong->GetProperty("test", nullptr)) // Is test
			gTestSongDataPtr = aSong;
		else if (aSong->GetProperty("long", &aValue))
		{ // 329
			gLongSongsMap[aValue] = aSong;
			addToSongsVec2 = false;
		}
		else if (aSong->GetProperty("beethovenrare", nullptr))
		{ // 321
			gLudwigSongs.push_back(aSong);
			addToSongsVec2 = false;
		}
		else if (aSong->GetProperty("beethoven", nullptr))
		{ // 312
			gLudwigSongs.push_back(aSong);
		}
		else if (aSong->GetProperty("santarare", nullptr))
		{ // 304
			gSantaSongs.push_back(aSong);
			addToSongsVec2 = false;
		}
		else if (aSong->GetProperty("santa", nullptr))
		{ // 299
			gSantaSongs.push_back(aSong);
		}

		if (aSong->GetProperty("kilgore", nullptr))
			gKilgoreSongDataPtr = aSong;

		if (addToSongsVec2)
			gSongsVector2.push_back(aSong);
	}
}

void LoadFishSongsTaskWrapper(void* userData)
{
	LoadFishSongs(0);
	gUnkBool05 = false;
}

// On Vista, moves folders from the game folder to the application data folder.
// (Framework code in the original: 0x41CC00 ctor, 0x41CC40 Add, 0x41CED0 Move, 0x41CC20 dtor.)
class UserDataMover
{
public:
	typedef std::pair<std::string, std::string> MovePair;
	typedef std::list<MovePair> MoveList;

	MoveList					mMoveList;

public:
	void Add(const std::string& theFolderName)
	{
		if (CheckForVista())
		{
			std::string aFromPath = RemoveTrailingSlash(GetCurDir()) + "\\" + theFolderName;
			std::string aToPath = GetAppDataFolder() + theFolderName;
			if (FileExists(aFromPath) && !FileExists(aToPath))
				mMoveList.push_back(MovePair(aFromPath, aToPath));
		}
	}

	void Move()
	{
		for (MoveList::iterator anItr = mMoveList.begin(); anItr != mMoveList.end(); ++anItr)
		{
			// The original dereferences the iterator once (one checked-iterator test) for both paths
			const MovePair& aMovePair = *anItr;
			const std::string& aFromPath = aMovePair.first;
			const std::string& aToPath = aMovePair.second;

			if (MoveFileExA(aFromPath.c_str(), aToPath.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH))
				continue;

			// Could not move it, so copy it instead (nothing is overwritten and the source is left in place)
			WIN32_FIND_DATAA aFindData;
			HANDLE aFindHandle = FindFirstFileA(aFromPath.c_str(), &aFindData);
			if (aFindHandle == INVALID_HANDLE_VALUE)
				continue;

			if ((aFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				FindClose(aFindHandle);

				std::list<std::string> aDirList;
				aDirList.push_back("");
				while (aDirList.size() != 0)
				{
					std::string aDir = aDirList.front();
					aDirList.pop_front();

					HANDLE aDirFindHandle = FindFirstFileA((aFromPath + "\\" + aDir + "\\*.*").c_str(), &aFindData);
					if (aDirFindHandle == INVALID_HANDLE_VALUE)
						continue;

					MkDir(aToPath + "\\" + aDir);
					do
					{
						std::string aFileName = aFindData.cFileName;
						if (aFileName != ".." && aFileName != ".")
						{
							if ((aFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
							{
								if (aDir.size() == 0)
									aDirList.push_back(aFileName);
								else
									aDirList.push_back(aDir + "\\" + aFileName);
							}
							else
							{
								CopyFileA((aFromPath + "\\" + aDir + "\\" + aFileName).c_str(), (aToPath + "\\" + aDir + "\\" + aFileName).c_str(), TRUE);
							}
						}
					}
					while (FindNextFileA(aDirFindHandle, &aFindData));
					FindClose(aDirFindHandle);
				}
			}
			else
			{
				FindClose(aFindHandle);
				CopyFileA(aFromPath.c_str(), aToPath.c_str(), TRUE);
			}
		}
	}
};

// Takes a song id (in ECX in the original, 0x54B150)
static void RemapFadeMusicTrack(int* theSongId)
{
	if (gCanRemapMusic && (*theSongId == 0 || *theSongId == 2))
		*theSongId = 4;
}

// Both test the handle for NULL rather than INVALID_HANDLE_VALUE, so a failed open falls through to
// Get/SetFileTime, which then fails
static bool GetFileWriteTime(const char* theFileName, FILETIME* theWriteTime)
{
	HANDLE aFileHandle = CreateFileA(theFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (aFileHandle == NULL)
		return false;

	BOOL aResult = GetFileTime(aFileHandle, NULL, NULL, theWriteTime);
	CloseHandle(aFileHandle);
	return aResult != FALSE;
}

static bool SetFileWriteTime(const char* theFileName, FILETIME* theWriteTime)
{
	HANDLE aFileHandle = CreateFileA(theFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (aFileHandle == NULL)
		return false;

	BOOL aResult = SetFileTime(aFileHandle, NULL, NULL, theWriteTime);
	CloseHandle(aFileHandle);
	return aResult != FALSE;
}

WinFishApp::WinFishApp()
{
	mBoard = NULL;
	mPrestoMenuData = NULL;
	mGameSelector = NULL;
	mTitleScreen = NULL;
	mPetsScreen = NULL;
	mStoreScreen = NULL;
	mInterludeScreen = NULL;
	mStoryScreen = NULL;
	mHatchScreen = NULL;
	mBonusScreen = NULL;
	mSimFishScreen = NULL;
	mTankScreen = NULL;
	mSimSetupScreen = NULL;
	mHighScoreScreen = NULL;
	mHelpScreen = NULL;

	mProdName = "Insaniquarium";
	
	mTitle = StringToSexyStringFast("Insaniquarium Deluxe " + mProductVersion);
	
	mRegKey = "PopCap\\Insaniquarium";
	mScreenSaverRegKey = "ScreenSaver\\";
	mScreenSaverRegPath = mScreenSaverRegKey;
	mCustomCursorsEnabled = false;
	mCurrentProfile = NULL;

	int aSeed = Rand(); // drawn before the MTRand is allocated
	mSeed = new MTRand(aSeed);

	mMusicVolume = 1.5;
	m0x7a4 = -1;
	m0x7a8 = -1;
	m0x7ac = -1;
	m0x8a8 = false;
	m0x8a9 = false;
	m0x8aa = false;
	m0x8ab = false;
	mGameNotPlayed = true;
	mFrameTime = 28;
	mGameMode = 0;
	mAutoEnable3D = true;
	m0x881 = true;
	m0x882 = true;
	mRelaxMode = false;
	m0x884 = false;
	mPlaytimeCounter2 = 0; 
	mPlaytimeCounter = 0; 
	mMaxExecutions = 0; 
	mMaxPlays = 0; 
	mMaxTime = 0; 
	mCompletedLoadingThreadTasks = 0;

	mProfileMgr = new ProfileMgr();

	mHighScoreMgr = new HighScoreMgr();

	mPrestoPtr = NULL;

	mScreenSaverSound = true;
	mScreenSaverRotateBackdrops = true;
	mScreenSaverPeriodicDim = true;
	mScreenSaverShowMoney = false;
	mScreenSaverPowerSave = false;
	mScreenSaverEnabled = false;
	mScreenSaverUnk01 = false;

	mTankGameModeChoose = 1;

	m0x878 = false;

	time_t aTodayTimeSec = GetTodayStartSeconds();
	mDaysSinceLastRun = aTodayTimeSec;
	mDaysSinceFirstRun = aTodayTimeSec;
	mLDAccum = 0;

	mWorkerThread = new WorkerThread();

	mCurrentSongId = -1;
	mCurrentSongOffset = -1;
}


Sexy::WinFishApp::~WinFishApp()
{
	if (mBoard)
		SaveCurrentUserData();

	StopMusic();

	for (int i = 0; i < DIALOG_END_ID; i++)
		KillDialog(i);

	if (mBoard != NULL)
	{
		mBoard->SaveCurrentGame();
		mWidgetManager->RemoveWidget(mBoard);
		delete mBoard;
		mBoard = NULL;
	}

	if (mTitleScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mTitleScreen);
		delete mTitleScreen;
	}

	if (mStoreScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mStoreScreen);
		delete mStoreScreen;
	}

	if (mHatchScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mHatchScreen);
		delete mHatchScreen;
	}

	if (mBonusScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mBonusScreen);
		delete mBonusScreen;
	}

	if (mHighScoreScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mHighScoreScreen);
		delete mHighScoreScreen;
	}

	if (mHelpScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mHelpScreen);
		delete mHelpScreen;
	}

	if (mInterludeScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mInterludeScreen);
		delete mInterludeScreen;
	}

	if (mStoryScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mStoryScreen);
		delete mStoryScreen;
	}

	if (mSimFishScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mSimFishScreen);
		delete mSimFishScreen;
	}

	if (mTankScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mTankScreen);
		delete mTankScreen;
	}

	if (mPetsScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mPetsScreen);
		delete mPetsScreen;
	}

	if (mSimSetupScreen != NULL)
	{
		mWidgetManager->RemoveWidget(mSimSetupScreen);
		delete mSimSetupScreen;
	}

	if (mGameSelector != NULL)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		delete mGameSelector;
	}

	delete mProfileMgr;
	delete mHighScoreMgr;
	delete mSeed;
	delete mWorkerThread;

	if (mPrestoMenuData != NULL)
	{
		for (int i = 0; i < RESOURCE_ID_MAX; i++)
			delete mPrestoMenuData[i];
		delete[] mPrestoMenuData;
	}

	mResourceManager->DeleteResources("");
}

void Sexy::WinFishApp::Init()
{
	DoParseCmdLine();
	if (IsScreenSaver())
		SEHCatcher::mShowUI = false;

	SexyApp::Init();

	if (CheckForVista())
	{
		MkDir(GetAppDataFolder());
		AllowAllAccess(GetAppDataFolder());

		// Moves the userdata folder from the game folder to the application data folder
		UserDataMover aMover;
		aMover.Add("userdata");
		aMover.Move();
	}

	if (!IsScreenSaver())
	{
		char aModuleFileName[260];
		GetModuleFileNameA(NULL, aModuleFileName, sizeof(aModuleFileName));
		SexyString aModulePath = aModuleFileName;

		if (CheckForVista())
			aModulePath = RemoveTrailingSlash(mChangeDirTo) + "\\" + GetFileName(aModulePath);

		RegistryWriteString(mScreenSaverRegKey + "Directory", GetFileDir(aModulePath));

		bool aShouldCopy = GetBoolean("ScrCopy", false);

		if (!aShouldCopy)
		{
			SexyString aScrExeName = GetString("ScrExe", "");

			if (aScrExeName.length() != 0) // If WinFish_Scr.exe exists in string map
			{
				aScrExeName = GetFileDir(aModulePath) + "\\" + aScrExeName;

				//if (!FileExists(aScrExeName))
					//CopyFileA(aModulePath.c_str(), aScrExeName.c_str(), FALSE);

				RegistryWriteString(mScreenSaverRegKey + "Executable", aScrExeName);
			}
			else
				RegistryWriteString(mScreenSaverRegKey + "Executable", aModulePath);
		}
	}

	// The original merges the four identical error exits into one WFAShowResourceError call
	if (!mResourceManager->ParseResourcesFile("properties\\resources.xml"))
	{
		WFAShowResourceError(true);
		return;
	}

	if (!mResourceManager->LoadResources("Init"))
	{
		WFAShowResourceError(true);
		return;
	}

	if (!ExtractInitResources(mResourceManager) || !LoadImageById(mResourceManager, IMAGE_STINKY_ID))
	{
		WFAShowResourceError(true);
		return;
	}

	mProfileMgr->ReadUsersDat();
	mHighScoreMgr->ReadHighScoresData();

	SexyString aUserName;

	bool aSuccessSSUser = RegistryReadString(mScreenSaverRegPath + "User", &mScreenSaverUserName);
	if (aSuccessSSUser && IsScreenSaver() && mScreenSaverUserName.size() != 0)
		mCurrentProfile = mProfileMgr->GetUserProfile(mScreenSaverUserName);

	bool aSuccessCurUser = mCurrentProfile == NULL && RegistryReadString("CurUser", &aUserName);
	if (aSuccessCurUser)
		mCurrentProfile = mProfileMgr->GetUserProfile(aUserName);

	if (mCurrentProfile == NULL)
		mCurrentProfile = mProfileMgr->GetFirstUserProfile();

	if (IsScreenSaver() && mCurrentProfile == NULL)
	{
		LogScreenSaverError("No User Profile");
		mLoadingFailed = true;
	}
	else
	{
		mMaxExecutions = GetInteger("MaxExecutions", 0);
		mMaxPlays = GetInteger("MaxPlays", 0);
		mMaxTime = GetInteger("MaxTime", 60);

		SetMusicVolume(mMusicVolume);

		Image* aImg = LoadMaskImage(IMAGE_TITLEPAGE, IMAGE_TITLEPAGEMASK, 384, 388);
		ReplaceImageById(mResourceManager, IMAGE_TITLEPAGEMASK_ID, aImg);

		mPrestoMenuData = new Image*[RESOURCE_ID_MAX];
		for (int i = 0; i < RESOURCE_ID_MAX; i++)
			mPrestoMenuData[i] = NULL;

		mTitleScreen = new TitleScreen(this);
		mTitleScreen->Resize(0, 0, mWidth, mHeight);
		mWidgetManager->AddWidget(mTitleScreen);


		if (!IsScreenSaver())
		{
			mMusicInterface->LoadMusic(1, "music\\Alien.mo3");
			mMusicInterface->LoadMusic(3, "music\\Lullaby.mo3");
			mMusicInterface->LoadMusic(4, "music\\Insaniq2.mo3");
			PlayMusic(2, 45);
		}
		SetCursorImage(CURSOR_POINTER, IMAGE_CURSOR_POINTER);
		SetCursorImage(CURSOR_HAND, IMAGE_CURSOR_HAND);
		SetCursorImage(CURSOR_DRAGGING, IMAGE_CURSOR_DRAGGING);
		SetCursorImage(CURSOR_TEXT, IMAGE_CURSOR_TEXT);
	}
}

void Sexy::WinFishApp::LoadingThreadProc()
{
	const char* aResNames[] = {"Register", "LoadingThread"}; // a string is built from a name for each call that takes one

	int arraySize = sizeof(aResNames) / sizeof(aResNames[0]);
	for (int i = 0; i < arraySize; i++)
		mNumLoadingThreadTasks += mResourceManager->GetNumResources(aResNames[i]);

	mNumLoadingThreadTasks += 33;


	for (int i = 0; i < arraySize; i++)
	{
		mResourceManager->StartLoadResources(aResNames[i]);

		while (mResourceManager->LoadNextResource())
		{
			if (mShutdown)
				return;
			mCompletedLoadingThreadTasks++;
		}

		if (mShutdown)
			return;

		if (mResourceManager->HadError() || !ExtractResourcesByName(mResourceManager, aResNames[i]))
		{
			WFAShowResourceError(false);
			mLoadingFailed = true;
			return;
		}

		m0x878 = true;
	}

	MemoryImage* aVertCreaseMImage = new MemoryImage(this);
	MemoryImage* aHorzCreaseMImage = new MemoryImage(this);
	aVertCreaseMImage->Create(IMAGE_VERTCREASE->mWidth, 60);
	aHorzCreaseMImage->Create(60, IMAGE_HORZCREASE->mHeight);
	Graphics aGVert(aVertCreaseMImage);
	Graphics aGHorz(aHorzCreaseMImage);

	int aHghtOrWdth = 0;
	while (aHghtOrWdth < 60)
	{
		aGVert.DrawImage(IMAGE_VERTCREASE, 0, aHghtOrWdth);
		aHghtOrWdth += IMAGE_VERTCREASE->mHeight;
	}
	aHghtOrWdth = 0;
	while (aHghtOrWdth < 60)
	{
		aGHorz.DrawImage(IMAGE_HORZCREASE, aHghtOrWdth, 0);
		aHghtOrWdth += IMAGE_HORZCREASE->mWidth;
	}

	ReplaceImageById(mResourceManager, IMAGE_VERTCREASE_ID, aVertCreaseMImage);
	ReplaceImageById(mResourceManager, IMAGE_HORZCREASE_ID, aHorzCreaseMImage);

	int aImgId = IMAGE_TANKMASK1_ID;
	int aImgYOffset = 365;
	int aImgCounter = 0;

	while (aImgCounter < 6)
	{
		Image* aMaskAquaImg = GetImageById(aImgId);
		Image* aAquaImg = GetImageById(aImgId-6);
		Image* aReplacementImg = LoadMaskImage(aAquaImg, aMaskAquaImg, 0, aImgYOffset);
		ReplaceImageById(mResourceManager, aImgId, aReplacementImg);
		if (mShutdown)
			return;

		mCompletedLoadingThreadTasks++;
		aImgId++;
		aImgCounter = aImgId - IMAGE_TANKMASK1_ID;
	}

	aImgId = IMAGE_SCL_STINKY_ID;
	aImgCounter = 0;
	while (aImgCounter < 24)
	{
		Image* aSclImg = GetImageById(aImgId + aImgCounter);
		MemoryImage* aSclMImage = new MemoryImage(this);
		aSclMImage->Create(40, 40);
		Graphics aGScl(aSclMImage);
		aGScl.SetFastStretch(false);

		if (aImgId + aImgCounter == IMAGE_SCL_AMP_ID)
		{
			int aHght = aSclImg->mHeight;
			if (aHght >= 24)
				aHght = 24;
			int aWdth = aSclImg->mWidth;
			if (aWdth >= 64)
				aWdth = 64;
			Rect aDestRect(0, 10, aSclMImage->mWidth, aSclMImage->mHeight-20);
			Rect aSrcRect(0,0, aWdth, aHght);
			aGScl.DrawImage(aSclImg, aDestRect, aSrcRect);
		}
		else
		{
			int aHght = aSclImg->mHeight;
			if (aHght >= 60)
				aHght = 60;
			int aWdth = aSclImg->mWidth;
			if (aWdth >= 60)
				aWdth = 60;
			Rect aDestRect(0, 0, aSclMImage->mWidth, aSclMImage->mHeight);
			Rect aSrcRect(0, 0, aWdth, aHght);
			aGScl.DrawImage(aSclImg, aDestRect, aSrcRect);
		}

		mPrestoMenuData[IMAGE_STINKY_ID + aImgCounter] = aSclMImage;
		if (mShutdown)
			return;
		mCompletedLoadingThreadTasks++;
		aImgCounter++;
	}
}

void Sexy::WinFishApp::LoadingThreadCompleted()
{
	if (IsScreenSaver())
		return;

	if (ShouldCheckForUpdate())
		DoUpdateDialog();
}

MusicInterface* Sexy::WinFishApp::CreateMusicInterface(HWND theHWnd)
{
	if (IsScreenSaver())
		return new MusicInterface;

	return SexyApp::CreateMusicInterface(theHWnd);
}

void Sexy::WinFishApp::URLOpenFailed(const std::string& theURL)
{
	SexyApp::URLOpenFailed(theURL);
	KillDialog(DIALOG_URL_INFO);
	CopyToClipboard(theURL);

	DoDialog(DIALOG_URL_INFO, true, "Open Browser", "Please open the following URL in your browser\n\n" + theURL + "\n\nFor your convenience, this URL has already been copied to your clipboard.", "OK", Dialog::BUTTONS_FOOTER);
}

void Sexy::WinFishApp::URLOpenSucceeded(const std::string& theURL)
{
	SexyApp::URLOpenSucceeded(theURL);
	KillDialog(DIALOG_URL_INFO);
}

bool Sexy::WinFishApp::OpenURL(const std::string& theURL, bool shutdownOnOpen)
{
	DoDialog(DIALOG_URL_INFO, true, "Opening Browser", "Opening Browser", "", Dialog::BUTTONS_NONE);
	DrawDirtyStuff();
	return SexyApp::OpenURL(theURL, shutdownOnOpen);
}

Dialog* Sexy::WinFishApp::DoDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	return SexyApp::DoDialog(theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);
}

bool Sexy::WinFishApp::KillDialog(int theDialogId)
{
	if (SexyApp::KillDialog(theDialogId))
	{
		Widget* aTheScr = NULL;
		if (mDialogMap.size() == 0 && ((aTheScr = mStoreScreen) != NULL || 
			(aTheScr = mBoard) != NULL || (aTheScr = mGameSelector) != NULL))
			mWidgetManager->SetFocus(aTheScr);

		if (mBoard != NULL)
			if (!ShouldKillDialog())
				mBoard->PauseGame(false);

		return true;
	}
	return false;
}

Dialog* Sexy::WinFishApp::NewDialog(int theDialogId, bool isModal, const SexyString& theDialogHeader, const SexyString& theDialogLines, const SexyString& theDialogFooter, int theButtonMode)
{
	MoneyDialog* aDialog = new MoneyDialog(this, IMAGE_DIALOG, IMAGE_DIALOGBUTTON, 
		theDialogId, isModal, theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);
	int aStrWdth = aDialog->mHeaderFont->StringWidth(theDialogHeader);
	aStrWdth += 150;
	if (aStrWdth < 348)
		aStrWdth = 348;
	int aPrefHeight = aDialog->GetPreferredHeight(aStrWdth);
	int aMinHeight = aDialog->mComponentImage->mHeight * 2 / 3 + 10;
	if (aPrefHeight < aMinHeight)
		aPrefHeight = aMinHeight;
	aDialog->Resize(143, 142, aStrWdth, aPrefHeight);
	return aDialog;
}

void Sexy::WinFishApp::SetMusicVolume(double theVolume)
{
	SexyApp::SetMusicVolume(theVolume);
}

void Sexy::WinFishApp::ModalOpen()
{
	if (mBoard != NULL)
		if (ShouldKillDialog())
			mBoard->PauseGame(true); 
}

void WinFishApp::LostFocus()
{
	if (mBoard != NULL && mGameMode != GAMEMODE_VIRTUAL_TANK)
		DoLostFocusDialog();
}

bool Sexy::WinFishApp::DebugKeyDown(int theKey)
{
	return SexyApp::DebugKeyDown(theKey);
}

void Sexy::WinFishApp::ButtonDepress(int theId)
{
	int anIdVar1 = theId % 10000;
	if (anIdVar1 >= 2000 && anIdVar1 < 3000)
	{
		anIdVar1 = theId - 2000;
		if (anIdVar1 > 10006)
		{
			if (anIdVar1 == 20006)
			{
				KillDialog(20006);
				KillDialog(DIALOG_UPDATE_CHECK);
			}
			else
				KillDialog(anIdVar1);
		}
		else if (anIdVar1 == 10006)
		{
			KillDialog(10006);
			KillDialog(DIALOG_UPDATE_CHECK);
			OpenURL(mInternetManager->GetUpdateURL(), true);
		}
		else
		{
			switch (anIdVar1)
			{
			case 0:
				KillDialog(0);
				SwitchToGameSelector();
				break;
			case DIALOG_OPTIONS:
				ApplyOptionsSettings();
				break;
			case DIALOG_REGISTER:
			{
				RegisterDialog* aDia = (RegisterDialog*)GetDialog(DIALOG_REGISTER);
				if (aDia)
				{
					SexyString aName = aDia->GetName();
					SexyString aCode = aDia->GetCode();
					if (aCode.length() == 0)
						OpenRegisterPage();
					else
					{
						if (!Validate(aName, aCode))
						{
							DoInvalidCodeDialog();
						}
						else
						{
							KillDialog(DIALOG_REGISTER);
							KillDialog(3);
							mUserName = aName;
							mRegCode = aCode;
							mIsRegistered = true;
							mWidgetManager->MarkAllDirty();
							if (mTitleScreen != nullptr)
								mTitleScreen->RegisterSuccessful();
							else
								DoDialog(DIALOG_THANKS_FOR_REGISTER, true, "Thanks!", "Thank you for registering Insaniquarium!", "Ok", Dialog::BUTTONS_FOOTER);
						}
					}
				}
				break;
			}
			case 4:
				KillDialog(4);
				StartGame();
				break;
			case DIALOG_UPDATE_ASK:
				KillDialog(DIALOG_UPDATE_ASK);
				DoUpdateCheckDialog();
				break;
			case DIALOG_QUIT_GAME:
				KillDialog(DIALOG_QUIT_GAME);
				Shutdown();
				break;
			case 13:
				KillDialog(13);
			case 3:
				DoRegisterDialog();
				break;
			case DIALOG_INFO:
				KillDialog(DIALOG_INFO);
				break;
			case DIALOG_FISH_NAMING:
				ApplyFishNameDialog();
				break;
			case DIALOG_FIRST_LVL_GAME_OVER:
				KillDialog(DIALOG_FIRST_LVL_GAME_OVER);
				mBoard->SpawnGuppyBought();
				break;
			case DIALOG_GAME_OVER:
				KillDialog(DIALOG_GAME_OVER);
				RemoveBoard();
				SwitchToGameSelector();
				break;
			case DIALOG_LOST_FOCUS:
				KillDialog(DIALOG_LOST_FOCUS);
				break;
			case DIALOG_OUT_OF_MONEY_LOAN:
				KillDialog(DIALOG_OUT_OF_MONEY_LOAN);
				mBoard->Unk07(100);
				break;
			case 20:
				KillDialog(20);
				RemovePetsScreen();
				StartGame();
				break;
			case 21:
				KillDialog(21);
				if (mBoard)
					mBoard->BonusRoundDropShell();
				break;
			case DIALOG_LEAVE_GAME:
				KillDialog(DIALOG_LEAVE_GAME);
				LeaveGameBoard();
				break;
			case DIALOG_USER_DIALOG:
				UserDialogOkPressed(true);
				break;
			case DIALOG_NEW_USER:
				MakeNewUser(true);
				break;
			case DIALOG_ARE_YOU_SURE_DELETE:
				DeleteUser(true);
				break;
			case DIALOG_RENAME:
				RenameUser(true);
				break;
			case DIALOG_INFO_NEW_USER:
			case DIALOG_NAME_CONFLICT:
				CloseNameWarningDialog(anIdVar1);
				break;
			case DIALOG_RESTART_GAME:
				RestartLevel();
				break;
			case DIALOG_CONFIRM_PURCHASE:
				ConfirmFishPurchase(true);
				break;
			case DIALOG_SELL_DIALOG:
				ApplySellGameObject(true);
				break;
			case DIALOG_SCREENSAVER:
				ApplyScreenSaverSettings();
				break;
			case DIALOG_TIMES_UP:
				TimesUpDialogDone();
				break;
			case DIALOG_GIVE_SHELLS:
				ApplyGiveShells(true);
				break;
			default:
				KillDialog(anIdVar1);
				break;
			}
		}
	}
	else if (anIdVar1 >= 3000 && anIdVar1 < 4000)
	{
		anIdVar1 = theId - 3000;
		if (anIdVar1 > 10006)
			KillDialog(anIdVar1);
		else if (anIdVar1 == 10006)
		{
			KillDialog(10006);
			KillDialog(DIALOG_UPDATE_CHECK);
		}
		else
		{
			switch (anIdVar1)
			{
			case 3:
				KillDialog(3);
				Shutdown();
				break;
			case 4:
				KillDialog(4);
				break;
			case DIALOG_USER_DIALOG:
				UserDialogOkPressed(false);
				break;
			case DIALOG_NEW_USER:
				MakeNewUser(false);
				break;
			case DIALOG_ARE_YOU_SURE_DELETE:
				DeleteUser(false);
				break;
			case DIALOG_RENAME:
				RenameUser(false);
				break;
			case DIALOG_PET_DIALOG:
				ApplyPrestoMorph(-1);
				break;
			case DIALOG_CONFIRM_PURCHASE:
				ConfirmFishPurchase(false);
				break;
			case DIALOG_SELL_DIALOG:
				ApplySellGameObject(false);
				break;
			case DIALOG_TIMES_UP:
				TimesUpDialogDone();
				break;
			case DIALOG_GIVE_SHELLS:
				ApplyGiveShells(false);
				break;
			default:
				KillDialog(anIdVar1);
				break;
			}
		}
	}
}

void Sexy::WinFishApp::PlayMusic(int theSongId, int theSongOffset, bool noLoop)
{
	mCurrentSongOffset = theSongOffset;
	mCurrentSongId = theSongId;

	RemapMusicTrack(&theSongId, &theSongOffset);

	mMusicInterface->PlayMusic(theSongId, theSongOffset, noLoop);
}

void Sexy::WinFishApp::SomeMusicFunc(bool flag)
{
	if (mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		flag = false;
		StopMusic();
	}
	else if (flag)
	{
		if (GetCurrentTank() == 1 || GetCurrentTank() == 2)
			FadeOutMusic(0, false, 0.008);
		else if (GetCurrentTank() == 3 || GetCurrentTank() == 4)
			FadeOutMusic(2, false, 0.008);
	}
	else
		StopMusic();

	PlayMusic(1, 0);
	if (flag)
	{
		mMusicInterface->StopMusic(1);
		FadeInMusic(1, -1, 0.002, false);
	}
}

void Sexy::WinFishApp::FadeOutMusic(int theSongId, bool stopSong, double theSpeed)
{
	if (mCurrentSongId == theSongId)
		mCurrentSongId = -1;

	RemapFadeMusicTrack(&theSongId);

	mMusicInterface->FadeOut(theSongId, stopSong, theSpeed);
}

void Sexy::WinFishApp::FadeInMusic(int theSongId, int theSongOffset, double theSpeed, bool noLoop)
{
	mCurrentSongOffset = theSongOffset;
	mCurrentSongId = theSongId;
	RemapMusicTrack(&theSongId, &theSongOffset);
	mMusicInterface->FadeIn(theSongId, theSongOffset, theSpeed, noLoop);
}

void Sexy::WinFishApp::RemapMusicTrack(int* theSongId, int* theSongOffset)
{
	if (!gCanRemapMusic)
		return;

	int aOriginalSongId = *theSongId;
	int aOriginalOffset = *theSongOffset;

	// Each case stores the offset before the song id
	if (aOriginalSongId == 0)
	{
		if (aOriginalOffset == 22)
		{
			*theSongOffset = 0;
			*theSongId = 4;
			return;
		}
		if (aOriginalOffset == 0)
		{
			*theSongOffset = 12;
			*theSongId = 4;
			return;
		}
	}

	if (aOriginalSongId == 2)
	{
		if (aOriginalOffset == 32)
		{
			*theSongOffset = 25;
			*theSongId = 4;
			return;
		}
		if (aOriginalOffset == 45)
		{
			*theSongOffset = 37;
			*theSongId = 4;
			return;
		}
		if (aOriginalOffset == 0)
		{
			*theSongOffset = 45;
			*theSongId = 4;
			return;
		}
		if (aOriginalOffset == 54)
		{
			*theSongOffset = 49;
			*theSongId = 4;
			return;
		}
		if (aOriginalOffset == 5)
		{
			*theSongOffset = 50;
			*theSongId = 4;
			return;
		}
	}

	if ((aOriginalSongId == 0) && (aOriginalOffset == 13))
	{
		*theSongOffset = 58;
		*theSongId = 4;
		return;
	}
}

void Sexy::WinFishApp::Start()
{
	if (mLoadingFailed)
	{
		if (IsScreenSaver() && !gScreenSaverStarted)
			StartScreenSaver();
	}
	else
		SexyApp::Start();
}

void Sexy::WinFishApp::Shutdown()
{
	if (mShutdown)
		return;

	if (mBoard != NULL)
	{
		SaveCurrentUserData();
		RemoveBoard();
	}
	UpdatePlayData();

	time_t aTodaysDate = GetTodayStartSeconds();
	int aSecondsPlayed = mUpdateCount / 36;
	mLDAccum += aSecondsPlayed;

	if ((mLDAccum >= 3600) || (aTodaysDate >= mDaysSinceFirstRun))
	{
		mDaysSinceFirstRun = aTodaysDate;
		mLDAccum = 0;
	}

	SexyApp::Shutdown();
}

void Sexy::WinFishApp::StartScreenSaver()
{
	mScreenSaverEnabled = false;
	gScreenSaverStarted = true;
	SetScreenSaver(mScreenSaverOldPath.c_str());
}

void Sexy::WinFishApp::SetScreenSaver(const char* thePath)
{
	OSVERSIONINFOA aVersionInfo;
	ZeroMemory(&aVersionInfo, sizeof(OSVERSIONINFOA));
	aVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&aVersionInfo);

	if (aVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		WritePrivateProfileStringA("boot", "SCRNSAVE.EXE", strlen(thePath) != 0 ? thePath : NULL, "SYSTEM.INI");
	}
	else
	{
		HKEY aKey = NULL;
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_WRITE, &aKey) == ERROR_SUCCESS)
		{
			RegSetValueExA(aKey, "SCRNSAVE.EXE", 0, REG_SZ, (const BYTE*)thePath, (DWORD)strlen(thePath) + 1);
			RegCloseKey(aKey);
		}
	}
}

void Sexy::WinFishApp::ReadFromRegistry()
{
	SexyApp::ReadFromRegistry();

	mScreenSaverSound = GetBoolean("ScrSound", true);
	mScreenSaverPowerSave = GetBoolean("ScrPowersave", false);

	char aUserNameBuffer[1024];
	DWORD aUserNameSize = sizeof(aUserNameBuffer);
	if (GetUserNameA(aUserNameBuffer, &aUserNameSize))
	{
		mScreenSaverRegPath.append(aUserNameBuffer, strlen(aUserNameBuffer));
		mScreenSaverRegPath += '\\';
	}

	DemoSyncString(&mScreenSaverRegPath);

	RegistryReadString(mScreenSaverRegPath + "OldPath", &mScreenSaverOldPath);
	RegistryReadBoolean(mScreenSaverRegPath + "Enabled", &mScreenSaverEnabled);
	ReadSSFromRegistry();

	if (IsScreenSaver())
	{
		if (mScreenSaverPowerSave)
		{
			mIsWindowed = true;
			mFullScreenWindow = true;
		}
		else
		{
			mIsWindowed = false;
			mAllowMonitorPowersave = false;
		}
		if (!mScreenSaverSound)
			mNoSoundNeeded = true;
		return;
	}

	RegistryReadInteger("ldaccum", &mLDAccum);

	// "ldinfo" is read into the low 32 bits of mDaysSinceFirstRun
	if (!RegistryReadInteger("ldinfo", (int*)&mDaysSinceFirstRun) || mDaysSinceFirstRun > mDaysSinceLastRun)
		mDaysSinceLastRun = mDaysSinceFirstRun;
	else
		mLDAccum = 0;
}

void Sexy::WinFishApp::WriteToRegistry()
{
	if (mCurrentProfile)
	{
		if (!IsScreenSaver())
			RegistryWriteString("CurUser", mCurrentProfile->mUserName);
		mCurrentProfile->Save();
	}

	if (!IsScreenSaver())
	{
		SexyApp::WriteToRegistry();
		RegistryWriteInteger("ldinfo", mDaysSinceFirstRun);
		RegistryWriteInteger("ldaccum", mLDAccum);
	}
}

void Sexy::WinFishApp::ReadSSFromRegistry()
{
	RegistryReadBoolean(mScreenSaverRegPath + "Sound", &mScreenSaverSound);
	RegistryReadBoolean(mScreenSaverRegPath + "RotateBackDrops", &mScreenSaverRotateBackdrops);
	RegistryReadBoolean(mScreenSaverRegPath + "PeriodicDim", &mScreenSaverPeriodicDim);
	RegistryReadBoolean(mScreenSaverRegPath + "ShowMoney", &mScreenSaverShowMoney);
	RegistryReadBoolean(mScreenSaverRegPath + "Powersave", &mScreenSaverPowerSave);

	SexyString anExePath;
	SexyString aScrPath;
	GetScreenSaverFilePath(aScrPath, anExePath);
	SexyString aSystemScrPath = GetSystemScreenSaverPath();

	if (mScreenSaverEnabled)
	{
		if (aSystemScrPath != aScrPath)
			mScreenSaverEnabled = false;
	}
	else
	{
		if (aSystemScrPath == aScrPath)
			mScreenSaverEnabled = true;
	}
}

// Gets the path of the installed .scr (theScrPath, as a short path) and of the game executable (theExePath)
void Sexy::WinFishApp::GetScreenSaverFilePath(SexyString& theScrPath, SexyString& theExePath)
{
	char aModuleFileName[MAX_PATH + 4];
	aModuleFileName[0] = '\0';
	GetModuleFileNameA(NULL, aModuleFileName, MAX_PATH);

	if (CheckForVista())
		theExePath = RemoveTrailingSlash(gSexyApp->mChangeDirTo) + "\\" + GetFileName(aModuleFileName, false);
	else
		theExePath = aModuleFileName;

	if (CheckForVista())
		theScrPath = RemoveTrailingSlash(gSexyApp->mChangeDirTo);
	else
		theScrPath = GetFileDir(theExePath, false);

	theScrPath.append("\\");
	theScrPath.append("Insaniquarium.scr");

	if (GetShortPathNameA(theScrPath.c_str(), aModuleFileName, MAX_PATH) != 0)
		theScrPath.assign(aModuleFileName, strlen(aModuleFileName));
}

SexyString Sexy::WinFishApp::GetSystemScreenSaverPath()
{
	DWORD aType = 0;
	OSVERSIONINFOA aVersionInfo;
	memset(&aVersionInfo, 0, sizeof(aVersionInfo));
	aVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	GetVersionExA(&aVersionInfo);

	char aPathBuffer[MAX_PATH + 4];
	aPathBuffer[0] = '\0';
	DWORD aLen = MAX_PATH + 1;

	if (aVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		GetPrivateProfileStringA("boot", "SCRNSAVE.EXE", "", aPathBuffer, MAX_PATH + 1, "SYSTEM.INI");
		return aPathBuffer;
	}

	HKEY aKey = NULL;
	aType = REG_SZ;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &aKey) != ERROR_SUCCESS)
		return aPathBuffer;

	RegQueryValueExA(aKey, "SCRNSAVE.EXE", NULL, &aType, (LPBYTE)aPathBuffer, &aLen);
	aPathBuffer[aLen] = '\0';
	RegCloseKey(aKey);

	if (aType == REG_SZ)
		return aPathBuffer;
	return "";
}

bool Sexy::WinFishApp::ShouldKillDialog()
{
	if ((mStoreScreen == nullptr) && (mPetsScreen == nullptr) &&
		(mSimSetupScreen == nullptr) && (mSimFishScreen == nullptr))
	{
		if (mDialogList.size() == 0)
			return false;

		Dialog* aTopDialog = mDialogList.back();

		if (aTopDialog->mId == 31 || aTopDialog->mId == 32)
			return false;
	}
	return true;
}

void Sexy::WinFishApp::DoLeaveGameDialog()
{
	Dialog* aDia = DoDialog(DIALOG_LEAVE_GAME, true, "Leave Game?", "Do you want to return to the\nmain menu?\n\nYour game will be saved.", "", Dialog::BUTTONS_YES_NO);
	aDia->mYesButton->mLabel = "LEAVE";
	aDia->mNoButton->mLabel = "CANCEL";
}

void Sexy::WinFishApp::DoLostFocusDialog()
{
	if (mBoard != NULL)
	{
		if (!IsScreenSaver() && !mBoard->mPause)
		{
			mBoard->PauseGame(true);
			CleanDialogs();
			DoDialog(DIALOG_LOST_FOCUS, true, "GAME PAUSED", "Click to resume game", "Resume Game", Dialog::BUTTONS_FOOTER);
		}
	}
}

void Sexy::WinFishApp::DoVirtualDialog()
{
	KillDialog(DIALOG_VIRTUAL);

	VirtualDialog* aDia = new VirtualDialog(this);
	aDia->Resize(320 - aDia->mWidth / 2, 70, aDia->mWidth, aDia->mHeight);

	AddDialog(DIALOG_VIRTUAL, aDia);
}

void Sexy::WinFishApp::DoScreenSaverDialog()
{
	ReadSSFromRegistry();
	ScreenSaverDialog* aDia = new ScreenSaverDialog(this);
	int aPrefHght = aDia->GetPreferredHeight(420);
	aDia->Resize(110, 40, 420, aPrefHght);
	AddDialog(DIALOG_SCREENSAVER, aDia);
}

void Sexy::WinFishApp::DoAreYouSureSellDialog(const SexyString& theLine)
{
	Dialog* aDia = DoDialog(DIALOG_SELL_DIALOG, true, "ARE YOU SURE?", theLine, "", Dialog::BUTTONS_OK_CANCEL);
	aDia->mYesButton->mLabel = "Sell";
	if (mSimFishScreen != nullptr)
		aDia->Resize(aDia->mX, 250, aDia->mWidth, aDia->mHeight);
	else
		aDia->Resize(aDia->mX, 60, aDia->mWidth, aDia->mHeight);
}

void Sexy::WinFishApp::OpenPrestoDialog(GameObject* thePet)
{
	CleanDialogs();
	mPrestoPtr = (OtherTypePet*) thePet;
	Dialog* aDia = new PetDialog(this);
	AddDialog(DIALOG_PET_DIALOG, aDia);
}

void Sexy::WinFishApp::DoConfirmPurchaseDialog(const SexyString& theLine)
{
	Dialog* aDia = DoDialog(DIALOG_CONFIRM_PURCHASE, true, "Confirm Purchase", theLine, "", Dialog::BUTTONS_OK_CANCEL);

	aDia->mYesButton->mLabel = "BUY";
	int aPrefHght = aDia->GetPreferredHeight(350);
	aDia->Resize(aDia->mX, aDia->mY, 350, aPrefHght);
}

void Sexy::WinFishApp::DoFishNamingDialog(const SexyString& theLine, bool isBreeder, const SexyString& theName, bool flag)
{
	FishNamingDialog* aDia = new FishNamingDialog(this, theLine);
	aDia->m0x168 = flag;
	aDia->mEditWidget->SetText(theName, true);
	aDia->mEditWidget->EnforceMaxPixels();
	aDia->mEditWidget->mCursorPos = aDia->mEditWidget->mString.length();
	aDia->mEditWidget->mHilitePos = 0;
	if (mStoreScreen != nullptr)
		aDia->Resize(218, 200, 400, aDia->GetPreferredHeight(400));
	else
		aDia->Resize(120, 150, 400, aDia->GetPreferredHeight(400));
	AddDialog(DIALOG_FISH_NAMING, aDia);
}

void Sexy::WinFishApp::DoFoodDialog()
{
	CleanDialogs();
	FoodDialog* aDia = new FoodDialog(this);
	AddDialog(DIALOG_FOOD, aDia);
}

void Sexy::WinFishApp::DoTimesUpDialog()
{
	CleanDialogs();
	DoDialogUnkF(DIALOG_TIMES_UP, true, "TIME\'S UP!", StrFormat("Your %d minutes are up!", mBoard->m0x3bc / 60), "Click for Results", Dialog::BUTTONS_FOOTER);
}

void Sexy::WinFishApp::DoRegisterDialog()
{
	if (!CanShowRegisterDialog())
		return;

	if (mIsRegistered)
	{
		DoDialog(DIALOG_INFO, true, "Already Registered", "You have already registered Insaniquarium.", "OK", Dialog::BUTTONS_FOOTER);
	}
	else
	{
		RegisterDialog* aDia = new RegisterDialog(this);
		int aPrefHght = aDia->GetPreferredHeight(420);
		aDia->Resize((mWidth - 420) / 2, 32, 420, aPrefHght);
		AddDialog(DIALOG_REGISTER ,aDia);
	}
}

void Sexy::WinFishApp::DoInvalidCodeDialog()
{
	DoDialog(DIALOG_INVALID_CODE, true, "Invalid Code", 
		"The license code you entered is not valid for that name.\n\nMake sure the name and registration number are entered correctly.", 
		"OK", Dialog::BUTTONS_FOOTER);
}

Dialog* Sexy::WinFishApp::DoDialogUnkF(int theId, bool isModal, const SexyString& theDiaHeader, const SexyString& theDiaLines, const SexyString& theDiaFooter, int theBtnMode)
{
	MoneyDialog* aDia = (MoneyDialog*) DoDialog(theId, isModal, theDiaHeader, theDiaLines, theDiaFooter, theBtnMode);
	aDia->DisableButtons(30);
	return aDia;
}

void Sexy::WinFishApp::CleanDialogs()
{
	ApplyPrestoMorph(-1);
	KillFoodDialog();
}

bool Sexy::WinFishApp::KillFoodDialog()
{
	return KillDialog(DIALOG_FOOD);
}

// Only once enough of the loading thread's resources are in
bool Sexy::WinFishApp::CanShowRegisterDialog()
{
	return mCompletedLoadingThreadTasks > 29;
}

// Closes the "Enter Your Name" / "Name Conflict" message and gives the name box back its focus
void Sexy::WinFishApp::CloseNameWarningDialog(int theDialogId)
{
	KillDialog(theDialogId);
	NewUserDialog* aDia = (NewUserDialog*)GetDialog(theDialogId != DIALOG_INFO_NEW_USER ? DIALOG_RENAME : DIALOG_NEW_USER);
	if (aDia)
		mWidgetManager->SetFocus(aDia->mEditWidget);
}

void Sexy::WinFishApp::TimesUpDialogDone()
{
	KillDialog(DIALOG_TIMES_UP);
	SwitchToBonusScreen();
}

bool Sexy::WinFishApp::IsScreenSaver()
{
	return SexyApp::IsScreenSaver();
}

void Sexy::WinFishApp::ApplyPrestoMorph(int thePetId)
{
	KillDialog(DIALOG_PET_DIALOG);
	if (thePetId >= 0 && mPrestoPtr && mBoard)
	{
		GameObject* aPet = mBoard->GetPrestoPet();
		if (aPet)
			aPet->PrestoMorph(thePetId);
	}
	mPrestoPtr = nullptr;
}

bool Sexy::WinFishApp::SaveCurrentUserData()
{
	if (mCurrentProfile)
		mCurrentProfile->Save();
	return true;
}

void Sexy::WinFishApp::SaveVirtualTankAndUserData()
{
	if (mGameMode != GAMEMODE_VIRTUAL_TANK)
		return;
	SaveCurrentUserData();
	if (mBoard)
		mBoard->SaveCurrentGame();
}

void Sexy::WinFishApp::ApplyGiveShells(bool doApply)
{
	UserDialog* aDia = (UserDialog*)GetDialog(DIALOG_GIVE_SHELLS);
	if (!aDia)
		return;

	if (doApply && mCurrentProfile)
	{
		int aShells = aDia->GetShellsValue();

		if (mCurrentProfile->mShells < aShells)
		{
			DoDialog(DIALOG_INFO, true, "Not Enough Shells", "You don\'t have that many shells to transfer.", "OK", Dialog::BUTTONS_FOOTER);
			return;
		}

		UserProfile* aProf = mProfileMgr->GetUserProfile(aDia->GetSelectedUserName());
		if (!aProf)
		{
			DoDialog(DIALOG_INFO, true, "Choose User", "Please choose a user from the list.", "OK", Dialog::BUTTONS_FOOTER);
			return;
		}
		if (aProf != mCurrentProfile)
		{
			aProf->LoadFromMemory();
			aProf->AddShells(aShells);
			mCurrentProfile->AddShells(-aShells);
			aProf->Save();
			mCurrentProfile->Save();
		}
	}

	KillDialog(DIALOG_GIVE_SHELLS);
}

void Sexy::WinFishApp::ApplyScreenSaverSettings()
{
	ScreenSaverDialog* aDia = (ScreenSaverDialog*)GetDialog(DIALOG_SCREENSAVER);
	if (aDia == nullptr)
		return;

	bool anEnabled = aDia->mSSEnabledCB->mChecked;

	SexyString anExePath;
	SexyString aScrPath;
	GetScreenSaverFilePath(aScrPath, anExePath);

	if (anEnabled)
	{
		SexyString aSystemScrPath = GetSystemScreenSaverPath();
		if (aSystemScrPath != aScrPath)
			mScreenSaverOldPath = aSystemScrPath;

		if (FileExists(aScrPath))
		{
			bool copyFailed = GetBoolean("ScrCopy", false) && !DoScrCopy(anExePath);

			if (copyFailed)
			{
				Popup("Failed to make screensaver.dat");
				mScreenSaverEnabled = false;
			}
			else
			{
				BOOL anActive = FALSE;
				SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, &anActive, 0);
				if (!anActive)
					SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, SPIF_UPDATEINIFILE);
				SetScreenSaver(aScrPath.c_str());
				mScreenSaverEnabled = true;
			}
		}
		else
			mScreenSaverEnabled = false;
	}
	else
		StartScreenSaver();

	mScreenSaverSound = aDia->mSSSoundCB->mChecked;
	mScreenSaverRotateBackdrops = aDia->mSSRotateBackdropsCB->mChecked;
	mScreenSaverPeriodicDim = aDia->mSSPeriodicDimCB->mChecked;
	mScreenSaverShowMoney = aDia->mSSShowMoneyCB->mChecked;
	mScreenSaverPowerSave = aDia->mSSPowerSaveCB->mChecked;

	if (anEnabled && mCurrentProfile != nullptr)
		mScreenSaverUserName = mCurrentProfile->mUserName;
	else
		mScreenSaverUserName = "";

	RegistryWriteString(mScreenSaverRegPath + "User", mScreenSaverUserName);
	WriteSSToRegistry();
	KillDialog(DIALOG_SCREENSAVER);
}

void Sexy::WinFishApp::WriteSSToRegistry()
{
	RegistryWriteString(mScreenSaverRegPath + "OldPath", mScreenSaverOldPath);
	RegistryWriteBoolean(mScreenSaverRegPath + "Enabled", mScreenSaverEnabled);
	RegistryWriteBoolean(mScreenSaverRegPath + "Sound", mScreenSaverSound);
	RegistryWriteBoolean(mScreenSaverRegPath + "RotateBackDrops", mScreenSaverRotateBackdrops);
	RegistryWriteBoolean(mScreenSaverRegPath + "PeriodicDim", mScreenSaverPeriodicDim);
	RegistryWriteBoolean(mScreenSaverRegPath + "ShowMoney", mScreenSaverShowMoney);
	RegistryWriteBoolean(mScreenSaverRegPath + "Powersave", mScreenSaverPowerSave);
}

void Sexy::WinFishApp::ApplyFishNameDialog()
{
	FishNamingDialog* aDia = (FishNamingDialog*)GetDialog(DIALOG_FISH_NAMING);
	SexyString aFishName = aDia->mEditWidget->mString;
	inlineTrim(aFishName); // the " \t\r\n" default argument is the only other string the original builds
	if (aFishName.length() == 0)
	{
		DoDialog(DIALOG_INFO, true, "Invalid Name", "Names must be one or more letters in length", "OK", Dialog::BUTTONS_FOOTER);
	}
	else
	{
		KillDialog(DIALOG_FISH_NAMING);
		if (mStoreScreen != nullptr)
		{
			GameObject* aProduct = mStoreScreen->ConfirmPurchase();
			if (aProduct)
			{
				aProduct->mName = aFishName;
				InitializeSpecialNamedFish(aProduct);
				mBoard->SpawnGameObject(aProduct, true);
				mWidgetManager->BringToFront(mStoreScreen);
			}
		}
		else if (mSimFishScreen)
		{
			mSimFishScreen->RenameSelectedFish(aFishName);
			mWidgetManager->BringToFront(mSimFishScreen);
		}
	}
}

void Sexy::WinFishApp::ApplySellGameObject(bool sell)
{
	if (mSimFishScreen)
		mSimFishScreen->SellSelectedObject(sell);
	else if (mSimSetupScreen)
		mSimSetupScreen->SellBackground(sell);
	KillDialog(DIALOG_SELL_DIALOG);
}

void Sexy::WinFishApp::ConfirmFishPurchase(bool confirmed)
{
	KillDialog(DIALOG_CONFIRM_PURCHASE);
	if (confirmed)
	{
		if (mStoreScreen)
		{
			mStoreScreen->ConfirmPurchase();
		}
		else if (mBonusScreen)
		{
			mBonusScreen->ConfirmPurchase();
		}
	}
}

void Sexy::WinFishApp::InitializeSpecialNamedFish(GameObject* theObject)
{
	const char* aName = theObject->mName.c_str();
	if (stricmp(aName, "1SingingFish") == 0)
	{
		theObject->mSinging = true;
	}
	else if (stricmp(aName, "santa") == 0 && theObject->mType == TYPE_GUPPY)
	{
		for (GameObjectSet::iterator it = mBoard->mGameObjectSet.begin(); it != mBoard->mGameObjectSet.end(); ++it)
		{
			GameObject* anObj = *it;
			if (anObj->mPreNamedTypeId == SANTA)
				return;
		}
		Fish* aFish = (Fish*)theObject;
		if (!aFish->mHasSpecialColors && !aFish->mRainbowFish && aFish->mPreNamedTypeId == -1)
		{
			aFish->mPreNamedTypeId = SANTA;
			aFish->mHasSpecialColors = true;
			aFish->mColors[0] = Color(0xffffff);
			aFish->mColors[1] = Color(0xff0000);
			aFish->mColors[2] = Color(0xffffff);
			aFish->mSinging = true;
			aFish->mFoodNeededToGrow = 1;
		}
	}
}

// Copies the game executable to screensaver.dat (when it is missing or its time stamp differs)
bool Sexy::WinFishApp::DoScrCopy(const SexyString& theExePath)
{
	SexyString anExePath = theExePath;
	SexyString aDatPath;
	if (CheckForVista())
		aDatPath = GetAppDataFolder() + "\\screensaver.dat";
	else
		aDatPath = GetFileDir(theExePath, false) + "\\screensaver.dat";

	FILETIME anExeTime;
	FILETIME aDatTime;
	bool anExeTimeOk = GetFileWriteTime(anExePath.c_str(), &anExeTime);
	bool aDatTimeOk = GetFileWriteTime(aDatPath.c_str(), &aDatTime);

	bool aNeedsCopy = !aDatTimeOk;
	if (anExeTimeOk && aDatTimeOk)
		aNeedsCopy = CompareFileTime(&anExeTime, &aDatTime) != 0;

	if (!aNeedsCopy)
		return true;

	if (!CopyFileA(anExePath.c_str(), aDatPath.c_str(), FALSE))
		return false;

	// "r+w" is not a valid mode for the CRT's fopen (as in the original)
	FILE* aFile = fopen(aDatPath.c_str(), "r+w");
	if (aFile == NULL)
		return false;

	fseek(aFile, 0, SEEK_SET);
	char aChar = 'N';
	fwrite(&aChar, 1, 1, aFile);
	fclose(aFile);

	SetFileWriteTime(aDatPath.c_str(), &anExeTime);
	return true;
}

void Sexy::WinFishApp::DoOptionsDialog(bool theFlag)
{
	CleanDialogs();
	OptionsDialog* aDia = new OptionsDialog(this, theFlag);
	int aPrefHght = aDia->GetPreferredHeight(348);
	aDia->Resize(143, 26, 348, aPrefHght);
	AddDialog(DIALOG_OPTIONS, aDia);
}

void Sexy::WinFishApp::SwitchToBoard(bool theFlag1, bool theFlag2)
{
	if (CheckTrialEnded())
	{
		SwitchToGameSelector();
		return;
	}
	RemoveBoard();
	if (mGameMode == GAMEMODE_VIRTUAL_TANK && !gFirstVirtualTankEnter)
	{
		gUnkBool05 = true;
		gFirstVirtualTankEnter = true;
		mWorkerThread->DoTask(LoadFishSongsTaskWrapper, 0);
	}
	if (theFlag1 && LoadBoardGame())
		return;

	for (int i = 0; i < 24; i++)
		mCurrentProfile->m0x5a[i] = false;

	if (theFlag2 && mGameMode == GAMEMODE_ADVENTURE && !mCurrentProfile->mFinishedGame && 
		mCurrentProfile->mTank == 1 && mCurrentProfile->mLevel <= 3)
	{
		SwitchToHelpScreen(true);
		return;
	}
	if (!IsBonusLevel() && mGameMode != GAMEMODE_VIRTUAL_TANK && 
		(mCurrentProfile->mNumOfUnlockedPets > 3 && (mGameMode != GAMEMODE_ADVENTURE || mCurrentProfile->mTank != 5)))
	{
		SwitchToPetsScreen();
		return;
	}
	StartGame();
}

void Sexy::WinFishApp::DoQuitDialog()
{
	Dialog* aDia = DoDialog(DIALOG_QUIT_GAME, true, "Quit", "Stop the insanity?", "", Dialog::BUTTONS_OK_CANCEL);
	aDia->mYesButton->mLabel = "Quit";
}

void Sexy::WinFishApp::SwitchToGameSelector()
{
	RemoveBoard();
	UpdatePlayData();
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
	}
	if (mCurrentProfile)
		mCurrentProfile->Unk01();

	mGameSelector = new GameSelector(this);
	mGameSelector->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mGameSelector);
	mWidgetManager->BringToBack(mGameSelector);
	mWidgetManager->SetFocus(mGameSelector);

	if (!mCurrentProfile)
	{
		DoNewUserDialog();
	}
	if (CheckTrialEnded())
		DoTrialVersionExpiredDialog();
}

void Sexy::WinFishApp::SwitchToHelpScreen(bool instructions)
{
	RemoveGameSelector();
	RemoveHelpScreen();
	mHelpScreen = new HelpScreen(this, instructions);
	mHelpScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mHelpScreen);
}

void Sexy::WinFishApp::RemoveBoard()
{
	CleanDialogs();
	if (mBoard)
	{
		mBoard->SaveCurrentGame();
		if (mBoard->Unk02() > 120)
			m0x884 = true;

		mWidgetManager->RemoveWidget(mBoard);
		SafeDeleteWidget(mBoard);
		mBoard = 0;
	}
}

void Sexy::WinFishApp::RestartLevel()
{
	KillDialog(DIALOG_CONTINUE_GAME);
	KillDialog(DIALOG_RESTART_GAME);
	RemoveBoard();
	SexyString aSavePath = UserProfile::GetSaveGameFilePath(mGameMode, mCurrentProfile->mId);
	EraseFile(aSavePath);
	StopMusic();
	PlayMusic(2, 0);
	if (mGameMode == GAMEMODE_ADVENTURE)
		SwitchToBoard(false, true);
	else
		SwitchToTankScreen();
}

void Sexy::WinFishApp::RemoveGameSelector()
{
	if (mGameSelector)
	{
		mWidgetManager->RemoveWidget(mGameSelector);
		SafeDeleteWidget(mGameSelector);
		mGameSelector = 0;
	}
}

void Sexy::WinFishApp::UpdatePlayData()
{
	if (m0x884)
	{
		mPlaytimeCounter2++;
		m0x884 = false;
	}
	if (mMaxTime > 0)
	{
		mTimesPlayed += (uint)(mFrameTime * mPlaytimeCounter) / 60000;
		mPlaytimeCounter2 = 0;
		mPlaytimeCounter = 0;
		return;
	}
	if (mMaxPlays > 0)
	{
		mTimesPlayed += mPlaytimeCounter2;
	}
	mPlaytimeCounter2 = 0;
	mPlaytimeCounter = 0;
}

bool Sexy::WinFishApp::CheckTrialEnded()
{
	if (mIsRegistered || IsScreenSaver())
		return false;
	UpdatePlayData();

	if (mMaxTime > 0)
	{
		int aTimeLeft = mMaxTime - mTimesPlayed;
		if (aTimeLeft <= 0)
			return true;
	}
	else if (mMaxPlays > 0)
	{
		int aPlaysLeft = mMaxPlays - mTimesPlayed;

		if (m0x884)
			aPlaysLeft--;

		if (aPlaysLeft <= 0)
			return true;
	}
	else if (mMaxExecutions > 0)
	{
		int anExecutionsLeft = mMaxExecutions - mTimesExecuted + 1;
		if (anExecutionsLeft <= 0)
			return true;
	}

	return false;
}

bool Sexy::WinFishApp::ApplyOptionsSettings()
{
	OptionsDialog* aDia = (OptionsDialog*)GetDialog(DIALOG_OPTIONS);
	if (!aDia)
		return false;
	// The fullscreen box is read before the 3D box
	bool aWindowed = !aDia->mFullscreenCB->IsChecked();
	bool anIs3D = aDia->m3DCB->IsChecked();
	SwitchScreenMode(aWindowed, anIs3D);
	EnableCustomCursors(aDia->mCustomCursorsCB->IsChecked());
	KillDialog(DIALOG_OPTIONS);
	ClearUpdateBacklog(true);
	if (mBoard)
		mBoard->ApplyShadowsIf3D();
	return true;
}

void Sexy::WinFishApp::DoTrialVersionExpiredDialog()
{
	Dialog* aDia = DoDialog(3, true, "PLEASE REGISTER!", "Your trial version of Insaniquarium has expired!\n\nYou must register your copy\nto continue playing.", "", Dialog::BUTTONS_OK_CANCEL);
	aDia->mYesButton->mLabel = "Register";
	aDia->mNoButton->mLabel = "Quit";
}

void Sexy::WinFishApp::DoUpdateDialog()
{
	UpdateCheckQueried();
	Dialog* aDia = DoDialog(DIALOG_UPDATE_ASK, true, "Updates", "Do you want to check for updates to Insaniquarium? New versions may offer new features and bug fixes.  This requires an active Internet connection.", "", Dialog::BUTTONS_YES_NO);
	int aPrefHeight = aDia->GetPreferredHeight(348);
	aDia->Resize(146, 50, 348, aPrefHeight);
}

void Sexy::WinFishApp::DoContinueDialog()
{
	ContinueDialog* aDia = new ContinueDialog(this);
	int aPrefHght = aDia->GetPreferredHeight(380);
	aDia->Resize((mWidth - 380) / 2, 70, 380, aPrefHght);
	AddDialog(DIALOG_CONTINUE_GAME, aDia);
}

void Sexy::WinFishApp::DoNewUserDialog()
{
	KillDialog(DIALOG_NEW_USER);
	NewUserDialog* aDia = new NewUserDialog(this, false);
	int aPrefHght = aDia->GetPreferredHeight(400);
	aDia->Resize((mWidth - 400) / 2, (mHeight - aPrefHght) / 2, 400, aPrefHght);
	AddDialog(DIALOG_NEW_USER, aDia);
}

void Sexy::WinFishApp::StartGame()
{
	mGameNotPlayed = false;
	if (mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		if (LoadBoardGame())
			return;
	}
	CreateBoard();
	mBoard->InitLevel();
	if (IsBonusLevel())
	{
		mBoard->InitBonusLevel();
		StartGameMusic();
		return;
	}

	if (mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		mBoard->StartVirtualTank();
		StartGameMusic();
		return;
	}

	mBoard->StartGame();
	StartGameMusic();
}

bool Sexy::WinFishApp::LoadBoardGame()
{
	CreateBoard();
	SexyString aSavePath = UserProfile::GetSaveGameFilePath(mGameMode, mCurrentProfile->mId);
	if (mBoard->LoadGame(aSavePath))
	{
		mGameNotPlayed = false;
		if (mGameMode != GAMEMODE_VIRTUAL_TANK)
			DoContinueDialog();
		return true;
	}

	RemoveBoard();
	return false;
}

void Sexy::WinFishApp::MakeNewUser(bool makeUser)
{
	NewUserDialog* aDia =(NewUserDialog*) GetDialog(DIALOG_NEW_USER);
	if (!aDia)
		return;

	SexyString aNewUserName = aDia->CleanString();

	if (makeUser && aNewUserName.size() == 0)
	{
		DoDialog(DIALOG_INFO_NEW_USER, true, "Enter Your Name", "Please enter your name to create a new user profile for storing high score data and game progress.", "OK", Dialog::BUTTONS_FOOTER);
		return;
	}

	if (mCurrentProfile == nullptr && (!makeUser || aNewUserName.size() == 0))
	{
		DoDialog(DIALOG_INFO_NEW_USER, true, "Enter Your Name",
			"Please enter your name to create a new user profile for storing high score data and game progress.",
			"OK", Dialog::BUTTONS_FOOTER);
		return;
	}

	if (!makeUser)
	{
		KillDialog(DIALOG_NEW_USER);
		return;
	}

	UserProfile* aProf = mProfileMgr->MakeNewUser(aNewUserName);
	if (!aProf)
	{
		DoDialog(DIALOG_INFO_NEW_USER, true, "Name Conflict",
			"The name you entered is already being used.  Please enter a unique player name.",
			"OK", Dialog::BUTTONS_FOOTER);
		return;
	}

	mProfileMgr->SaveUsersDat();
	mCurrentProfile = aProf;
	KillDialog(DIALOG_USER_DIALOG);
	KillDialog(DIALOG_NEW_USER);
	mWidgetManager->MarkAllDirty();
	if (mGameSelector != nullptr)
		mGameSelector->ProfileChanged();
}

void Sexy::WinFishApp::CreateBoard()
{
	RemoveBoard();
	m0x884 = false;
	mBoard = new Board(this);
	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);
	mWidgetManager->BringToBack(mBoard);
	mWidgetManager->SetFocus(mBoard);
	mCurrentProfile->m0x4c++;
}

bool Sexy::WinFishApp::IsBonusLevel()
{
	if (mGameMode == GAMEMODE_ADVENTURE && mCurrentProfile->mLevel == 6)
		return true;
	if (mBoard)
		return mBoard->mIsBonusRound;
	return false;
}

bool Sexy::WinFishApp::IsSongPlaying(int theSongId, int theSongOffset)
{
	return mCurrentSongId == theSongId && mCurrentSongOffset == theSongOffset;
}

void Sexy::WinFishApp::StopMusic()
{
	if (!mMusicInterface)
		return;

	mMusicInterface->StopMusic(0);
	mMusicInterface->StopMusic(1);
	mMusicInterface->StopMusic(2);
	mMusicInterface->StopMusic(3);
	mMusicInterface->StopMusic(4);
	mCurrentSongId = -1;
	mCurrentSongOffset = -1;
}

void Sexy::WinFishApp::StartGameMusic()
{
	StopMusic();
	if (mGameMode == GAMEMODE_VIRTUAL_TANK)
		return;

	if (IsBonusLevel())
		PlayMusic(0, 13);
	else if(GetCurrentTank() == 1)
		PlayMusic(0, 22);
	else if(GetCurrentTank() == 2)
		PlayMusic(0, 0);
	else if(GetCurrentTank() == 3)
		PlayMusic(2, 32);
	else if(GetCurrentTank() == 4)
		PlayMusic(2, 45);
}

void Sexy::WinFishApp::SomeMusicPlayFunc(bool flag)
{
	if (mGameMode == GAMEMODE_VIRTUAL_TANK)
	{
		StopMusic();
		return;
	}

	double aSpeed;
	if (flag)
	{
		FadeOutMusic(1, true, 0.008);
		aSpeed = 0.002;
	}
	else
	{
		StopMusic();
		aSpeed = 1.0;
	}

	if (GetCurrentTank() == 1)
		FadeInMusic(0, 22, aSpeed, false);
	else if(GetCurrentTank() == 2)
		FadeInMusic(0, 0, aSpeed, false);
	else if(GetCurrentTank() == 3)
		FadeInMusic(2, 32, aSpeed, false);
	else if(GetCurrentTank() == 4)
		FadeInMusic(2, 45, aSpeed, false);
}

bool Sexy::WinFishApp::ChangeDirHook(const char* theIntendedPath)
{
	if (!IsScreenSaver())
		return false;

	SexyString aDirectoryPath;

	if (!RegistryReadString(mScreenSaverRegKey + "Directory", &aDirectoryPath))
		return false;

	if (SetCurrentDirectoryA(aDirectoryPath.c_str()) != 0)
		return true;

	return false;
}

void Sexy::WinFishApp::LogScreenSaverError(const std::string& theError)
{
	if (!gScreenSaverStarted)
		StartScreenSaver();
	SexyApp::LogScreenSaverError(theError);
}

void Sexy::WinFishApp::UpdateFrames()
{
	if (gUnkBool01)
	{
		if (++gUpdateFramesCounter < 4)
			return;
		gUpdateFramesCounter = 0;
	}
	else if (gDoHundredUpdates)
	{
		for(int i = 0; i < 100; i++)
			SexyApp::UpdateFrames();
		return;
	}

	SexyApp::UpdateFrames();
}

void Sexy::WinFishApp::TitleScreenIsFinished()
{
	mWidgetManager->RemoveWidget(mTitleScreen);
	SafeDeleteWidget(mTitleScreen);
	mTitleScreen = NULL;

	mResourceManager->DeleteImage("IMAGE_TITLEPAGE");
	mResourceManager->DeleteImage("IMAGE_LOADERBAR");
	mResourceManager->DeleteImage("IMAGE_LOADERBAROVER");
	mResourceManager->DeleteImage("IMAGE_LOADERBAROVER2");
	mResourceManager->DeleteImage("IMAGE_LOADERPLAY");
	mResourceManager->DeleteImage("IMAGE_TITLEPAGEMASK");

	if (IsScreenSaver())
	{
		mGameMode = GAMEMODE_VIRTUAL_TANK;
		SwitchToBoard(true, true);
		ClearUpdateBacklog(true);
		return;
	}

	SwitchToGameSelector();
}

void Sexy::WinFishApp::Blank()
{
}

// The original overrides SexyApp::PreDisplayHook with an empty body (vtable slot 26 is the shared ret at 0x4E9E00)
void Sexy::WinFishApp::PreDisplayHook()
{
}

void Sexy::WinFishApp::WFAShowResourceError(bool doExit)
{
	if (!IsScreenSaver())
	{
		ShowResourceError(doExit);
		return;
	}

	// The error text is passed as a C string, so a second string is built from it
	LogScreenSaverError(mResourceManager->GetErrorText().c_str());
	mLoadingFailed = true;
}

void Sexy::WinFishApp::SwitchToHighScoreScreen()
{
	RemoveGameSelector();
	RemoveHighScoreScreen();
	mHighScoreScreen = new HighScoreScreen(this);
	mHighScoreScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mHighScoreScreen);
}

void Sexy::WinFishApp::SwitchToPetsScreen()
{
	CleanDialogs();
	RemovePetsScreen();
	if (mBoard)
		mBoard->PauseGame(true);
	mPetsScreen = new PetsScreen(this);
	mPetsScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mPetsScreen);
}

void Sexy::WinFishApp::SwitchToHatchScreen(int thePetId)
{
	CleanDialogs();
	m0x884 = true;
	if (mBoard)
		mBoard->mShouldSave = false;
	RemoveHatchScreen();
	mHatchScreen = new HatchScreen(this, thePetId);
	mHatchScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mHatchScreen);
	SaveCurrentUserData();
	RemoveBoard();
}

void Sexy::WinFishApp::SwitchToInterludeScreen()
{
	CleanDialogs();
	RemoveInterludeScreen();
	mInterludeScreen = new InterludeScreen(this, 0);
	mInterludeScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mInterludeScreen);
	mWidgetManager->SetFocus(mInterludeScreen);
	if (mBoard == nullptr)
		mWidgetManager->BringToBack(mInterludeScreen);
	else
		mWidgetManager->PutInfront(mInterludeScreen, mBoard);
}
void Sexy::WinFishApp::SwitchToSimSetupScreen()
{
	ReadSSFromRegistry();
	CleanDialogs();
	RemoveSimSetupScreen();
	if (mBoard)
		mBoard->PauseGame(true);

	mSimSetupScreen = new SimSetupScreen(this);
	mSimSetupScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mSimSetupScreen);
}
void Sexy::WinFishApp::SwitchToTankScreen()
{
	CleanDialogs();
	RemoveTankScreen();

	mTankScreen = new TankScreen(this);
	mTankScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mTankScreen);
	if (mBoard == nullptr)
		mWidgetManager->BringToBack(mTankScreen);
	else
		mWidgetManager->PutInfront(mTankScreen, mBoard);
}

void Sexy::WinFishApp::SwitchToStoryScreen(int unk)
{
	CleanDialogs();
	RemoveStoryScreen();

	int aGroupIdx = unk - 1;
	bool isAdventure = false;
	if (aGroupIdx < 0)
		aGroupIdx = 0;
	else if (aGroupIdx > 3)
		aGroupIdx = 3;
	else
		isAdventure = true;

	// A local table (the original stores it on the stack on every call)
	int aStoryTable[32] = {
		0, 1, 2, 3, 4, 20, 24, 25,
		5, 6, 7, 8, 9, 21, 26, 27,
		10, 11, 12, 13, 14, 22, 28, 29,
		15, 16, 17, 18, 19, 23, 30, 31
	};

	int aStoryId = -2;
	bool storyFound = false;
	if (isAdventure)
	{
		if (mCurrentProfile->m0x7c == -1)
		{
			if (mCurrentProfile->m0x80 == 0 && mCurrentProfile->mBonusItemId >= 6)
			{
				aStoryId = 32;
				mCurrentProfile->m0x80 = 1;
				SaveCurrentUserData();
				storyFound = true;
			}
			else
			{
				int aLimit = 33;
				if (mCurrentProfile->m0x80 != 1) aLimit = 32;
				aStoryId = Rand() % aLimit;
				if (aStoryId != -1)
					storyFound = true;
			}
		}

		int* aGroupStart = &aStoryTable[aGroupIdx * 8];
		if (!storyFound)
		{
			// The first story of the group not seen yet (a pet's story only once the pet is unlocked)
			int i;
			for (i = 0; i < 8; i++)
			{
				aStoryId = aGroupStart[i];
				if ((mCurrentProfile->m0x7c & (1 << aStoryId)) == 0 &&
					((uint)(aStoryId - 20) > 3 || mCurrentProfile->IsPetUnlocked(aStoryId)))
					break;
			}

			if (i != 8)
			{
				mCurrentProfile->m0x7c |= (1 << aStoryId);
				SaveCurrentUserData();
				if(aStoryId != -1)
					storyFound = true;
			}
		}

		if (!storyFound)
		{
			std::vector<int> aSeenStories;
			for (int i = 0; i < 8; i++)
			{
				int anId = aGroupStart[i];
				bool seen = (mCurrentProfile->m0x7c & (1 << anId)) != 0;
				if (seen)
				{
					aSeenStories.push_back(anId);
				}
			}

			if (aSeenStories.empty())
				aSeenStories.push_back(0);

			uint aNumSeen = aSeenStories.size(); // the size is read before the draw
			int aRandIdx = Rand() % aNumSeen;
			aStoryId = aSeenStories[aRandIdx];
		}
	}

	mStoryScreen = new StoryScreen(this, aStoryId);
	mStoryScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mStoryScreen);
	mWidgetManager->SetFocus(mStoryScreen);
}

void Sexy::WinFishApp::SwitchToStoreScreen()
{
	CleanDialogs();
	RemoveStoreScreen();
	if (mBoard)
		mBoard->PauseGame(true);

	if (mCurrentProfile)
		mCurrentProfile->Unk01();

	mStoreScreen = new StoreScreen(this);
	mStoreScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mStoreScreen);
	mWidgetManager->SetFocus(mStoreScreen);
}

void Sexy::WinFishApp::SwitchToSimFishScreen()
{
	CleanDialogs();
	RemoveSimFishScreen();
	if (mBoard)
		mBoard->PauseGame(true);

	mSimFishScreen = new SimFishScreen(this);
	mSimFishScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mSimFishScreen);
	mWidgetManager->SetFocus(mSimFishScreen);
}

void Sexy::WinFishApp::SwitchToBonusScreen()
{
	CleanDialogs();
	m0x884 = true;
	if (mBoard)
		mBoard->mShouldSave = false;
	RemoveBonusScreen();
	if (mCurrentProfile)
		mCurrentProfile->Unk01();

	mBonusScreen = new BonusScreen(this);
	mBonusScreen->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBonusScreen);
	SaveCurrentUserData();
	RemoveBoard();
	StopMusic();
	PlayMusic(2, 54);
}

void Sexy::WinFishApp::RemoveHighScoreScreen()
{
	if (mHighScoreScreen)
	{
		mWidgetManager->RemoveWidget(mHighScoreScreen);
		SafeDeleteWidget(mHighScoreScreen);
		mHighScoreScreen = nullptr;
		SwitchToGameSelector();
	}
}

void Sexy::WinFishApp::RemovePetsScreen()
{
	if (mPetsScreen)
	{
		mWidgetManager->RemoveWidget(mPetsScreen);
		SafeDeleteWidget(mPetsScreen);
		mPetsScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveHatchScreen()
{
	if (mHatchScreen)
	{
		mWidgetManager->RemoveWidget(mHatchScreen);
		SafeDeleteWidget(mHatchScreen);
		mHatchScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveSimSetupScreen()
{
	if (mSimSetupScreen)
	{
		mWidgetManager->RemoveWidget(mSimSetupScreen);
		SafeDeleteWidget(mSimSetupScreen);
		mSimSetupScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveTankScreen()
{
	if (mTankScreen)
	{
		mWidgetManager->RemoveWidget(mTankScreen);
		SafeDeleteWidget(mTankScreen);
		mTankScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveStoryScreen()
{
	if (mStoryScreen)
	{
		mWidgetManager->RemoveWidget(mStoryScreen);
		SafeDeleteWidget(mStoryScreen);
		mStoryScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveStoreScreen()
{
	if (mStoreScreen)
	{
		mWidgetManager->RemoveWidget(mStoreScreen);
		SafeDeleteWidget(mStoreScreen);
		mStoreScreen = nullptr;
		if (mBoard != nullptr)
			mWidgetManager->SetFocus(mBoard);
	}
}

void Sexy::WinFishApp::RemoveSimFishScreen()
{
	if (mSimFishScreen)
	{
		mWidgetManager->RemoveWidget(mSimFishScreen);
		SafeDeleteWidget(mSimFishScreen);
		mSimFishScreen = nullptr;
		if (mBoard != nullptr)
			mWidgetManager->SetFocus(mBoard);
	}
}

void Sexy::WinFishApp::RemoveBonusScreen()
{
	if (mBonusScreen)
	{
		mWidgetManager->RemoveWidget(mBonusScreen);
		SafeDeleteWidget(mBonusScreen);
		mBonusScreen = nullptr;
	}
}

void Sexy::WinFishApp::RemoveInterludeScreen()
{
	if (mInterludeScreen)
	{
		mWidgetManager->RemoveWidget(mInterludeScreen);
		SafeDeleteWidget(mInterludeScreen);
		mInterludeScreen = nullptr;
	}
}

void Sexy::WinFishApp::LeaveGameBoard()
{
	SaveCurrentUserData();
	ApplyOptionsSettings();
	RemoveBoard();
	SwitchToGameSelector();
}

void Sexy::WinFishApp::RemoveHelpScreen()
{
	if (mHelpScreen)
	{
		mWidgetManager->RemoveWidget(mHelpScreen);
		SafeDeleteWidget(mHelpScreen);
		mHelpScreen = 0;
	}
}

void Sexy::WinFishApp::DoUpdateCheckDialog()
{
	KillDialog(DIALOG_UPDATE_CHECK);
	KillDialog(DIALOG_UPDATE_ASK);
	UpdateCheckDialog* aDialog = new UpdateCheckDialog(IMAGE_DIALOG, IMAGE_DIALOGBUTTON, IMAGE_WAIT_BAR, DIALOG_UPDATE_CHECK);
	DefaultDialogSettings(aDialog);
	int aPrefHght = aDialog->GetPreferredHeight(348);
	aDialog->Resize(146, 50, 348, aPrefHght);
	AddDialog(DIALOG_UPDATE_CHECK, aDialog);
	mInternetManager->StartUpdateCheck("http://www.popcap.com/win32updatecheck.php?prod=" + mProdName +
		"&ver=" + mProductVersion
		+ "&referid=" + mReferId);
}

void Sexy::WinFishApp::DoWhoAreYouDialog()
{
	KillDialog(DIALOG_USER_DIALOG);
	UserDialog* aDia = new UserDialog(this, false);
	int aPrefHght = aDia->GetPreferredHeight(400);
	aDia->Resize(0, 17, 400, aPrefHght);
	AddDialog(DIALOG_USER_DIALOG, aDia);
}

void Sexy::WinFishApp::DoGiveDialog()
{
	KillDialog(DIALOG_GIVE_SHELLS);
	if (mCurrentProfile && (mCurrentProfile->mFinishedGame || mCurrentProfile->mTank != 1))
	{
		UserDialog* aDia = new UserDialog(this, true);
		int aPrefHght = aDia->GetPreferredHeight(400);
		aDia->Resize(0, 100, 400, aPrefHght);
		AddDialog(DIALOG_GIVE_SHELLS, aDia);
		return;
	}

	DoDialog(DIALOG_INFO, true, "Not Allowed", "You need to beat the first tank before you can transfer shells.", "OK", Dialog::BUTTONS_FOOTER);
}

void Sexy::WinFishApp::DoDeleteWarningDialog(SexyString& theName)
{
	KillDialog(DIALOG_ARE_YOU_SURE_DELETE);
	DoDialog(DIALOG_ARE_YOU_SURE_DELETE, true, "Are You Sure?", StrFormat("This will permanently remove \'%s\' from the player roster!", theName.c_str()), "", Dialog::BUTTONS_YES_NO);
}

void Sexy::WinFishApp::RenameUser(bool makeUser)
{
	if (!makeUser)
	{
		KillDialog(DIALOG_RENAME);
		return;
	}

	UserDialog* anUserDialog = (UserDialog*)GetDialog(DIALOG_USER_DIALOG);
	NewUserDialog* aRenameDialog = (NewUserDialog*)GetDialog(DIALOG_RENAME);

	if (!anUserDialog || !aRenameDialog)
		return;

	SexyString anOldName = anUserDialog->GetSelectedUserName();
	SexyString aNewName = aRenameDialog->CleanString();

	if (aNewName.size() > 0)
	{
		bool isCurrentProfile = mProfileMgr->GetUserProfile(anOldName) == mCurrentProfile;
		bool aSuccess = mProfileMgr->RenameUser(anOldName, aNewName);
		if (!aSuccess)
		{
			DoDialog(DIALOG_NAME_CONFLICT, true, "Name Conflict", "The name you entered is already being used.  Please enter a unique player name.", "OK", Dialog::BUTTONS_FOOTER);
			return;
		}

		mProfileMgr->SaveUsersDat();
		if (isCurrentProfile)
			mCurrentProfile = mProfileMgr->GetUserProfile(aNewName);

		anUserDialog->RenameSelectedUser(aNewName);

		mWidgetManager->MarkAllDirty();
		KillDialog(DIALOG_RENAME);
	}
}

void Sexy::WinFishApp::DeleteUser(bool deleteUser)
{
	KillDialog(DIALOG_ARE_YOU_SURE_DELETE);
	if (!deleteUser)
		return;
	UserDialog* aDia = (UserDialog*)GetDialog(DIALOG_USER_DIALOG);
	if (!aDia)
		return;

	SexyString aUserName = mCurrentProfile ? mCurrentProfile->mUserName : SexyString("");

	SexyString aSelUserName = aDia->GetSelectedUserName();

	if (aSelUserName == aUserName)
		mCurrentProfile = nullptr;
	mProfileMgr->DeleteUser(aSelUserName);
	aDia->RemoveSelectedUser();
	if (!mCurrentProfile)
	{
		mCurrentProfile = mProfileMgr->GetUserProfile(aDia->GetSelectedUserName());
		if (!mCurrentProfile)
			mCurrentProfile = mProfileMgr->GetFirstUserProfile();
	}
	mProfileMgr->SaveUsersDat();
	if (!mCurrentProfile)
		DoNewUserDialog();
	mWidgetManager->MarkAllDirty();
	if (mGameSelector != nullptr)
		mGameSelector->ProfileChanged();
}

void Sexy::WinFishApp::DoRenameDialog(const SexyString& theUserName)
{
	KillDialog(DIALOG_RENAME);
	NewUserDialog* aRenameDialog = new NewUserDialog(this, true);
	int aPrefHght = aRenameDialog->GetPreferredHeight(400);
	aRenameDialog->Resize((mWidth - 400) / 2, (mHeight - aPrefHght) / 2, 400, aPrefHght);
	aRenameDialog->SetName(theUserName);
	AddDialog(DIALOG_RENAME, aRenameDialog);
}

void Sexy::WinFishApp::UserDialogOkPressed(bool applyChanges)
{
	UserDialog* aDia =(UserDialog*) GetDialog(DIALOG_USER_DIALOG);
	if (!aDia)
		return;

	if (applyChanges)
	{
		UserProfile* aProf = mProfileMgr->GetUserProfile(aDia->GetSelectedUserName());
		if (aProf)
		{
			mCurrentProfile = aProf;
			aProf->ResetAfterFinalTank();
			mWidgetManager->MarkAllDirty();
			if (mGameSelector != nullptr)
				mGameSelector->ProfileChanged();
		}
	}

	KillDialog(DIALOG_USER_DIALOG);
}

int Sexy::WinFishApp::GetCurrentTank()
{
	if (mBoard)
		return mBoard->mTank;
	return mCurrentProfile->mTank;
}
