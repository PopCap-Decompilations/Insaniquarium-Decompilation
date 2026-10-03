#include <SexyAppFramework\SEHCatcher.h>
#include <SexyAppFramework\Debug.h>
#include <SexyAppFramework\BigInt.h>
#include <ImageLib\ImageLib.h>

#include "SexyApp.h"
#include "InternetManager.h"
#include "BetaSupport.h"

#include <time.h>
#include <fstream>
#include <direct.h>

using namespace Sexy;

SexyApp* Sexy::gSexyApp = NULL;

// 0x5BDBF8: folds accented characters of a registration name to plain letters (93 pairs and a terminator)
static const char gRegKeyTranslationTable[][2] =
{
	{ '\x80', 'C' }, { '\x81', 'u' }, { '\x82', 'e' }, { '\x83', 'a' }, { '\x84', 'a' }, { '\x85', 'a' },
	{ '\x86', 'a' }, { '\x87', 'c' }, { '\x88', 'e' }, { '\x89', 'e' }, { '\x8A', 'e' }, { '\x8B', 'i' },
	{ '\x8C', 'i' }, { '\x8D', 'i' }, { '\x8E', 'A' }, { '\x8F', 'A' }, { '\x90', 'E' }, { '\x91', 'a' },
	{ '\x92', 'A' }, { '\x93', 'o' }, { '\x94', 'o' }, { '\x95', 'o' }, { '\x96', 'u' }, { '\x97', 'u' },
	{ '\x98', 'y' }, { '\x99', 'O' }, { '\x9A', 'U' }, { '\xA0', 'a' }, { '\xA1', 'i' }, { '\xA2', 'o' },
	{ '\xA3', 'u' }, { '\xA4', 'n' }, { '\xA5', 'N' }, { '\xC0', 'A' }, { '\xC1', 'A' }, { '\xC2', 'A' },
	{ '\xC3', 'A' }, { '\xC4', 'A' }, { '\xC5', 'A' }, { '\xC6', 'A' }, { '\xC7', 'C' }, { '\xC8', 'E' },
	{ '\xC9', 'E' }, { '\xCA', 'E' }, { '\xCB', 'E' }, { '\xCC', 'I' }, { '\xCD', 'I' }, { '\xCE', 'I' },
	{ '\xCF', 'I' }, { '\xD0', 'D' }, { '\xD1', 'N' }, { '\xD2', 'O' }, { '\xD3', 'O' }, { '\xD4', 'O' },
	{ '\xD5', 'O' }, { '\xD6', 'O' }, { '\xD8', 'O' }, { '\xD9', 'U' }, { '\xDA', 'U' }, { '\xDB', 'U' },
	{ '\xDC', 'U' }, { '\xDD', 'Y' }, { '\xDE', 'b' }, { '\xDF', 'B' }, { '\xE0', 'a' }, { '\xE1', 'a' },
	{ '\xE2', 'a' }, { '\xE3', 'a' }, { '\xE4', 'a' }, { '\xE5', 'a' }, { '\xE6', 'a' }, { '\xE7', 'c' },
	{ '\xE8', 'e' }, { '\xE9', 'e' }, { '\xEA', 'e' }, { '\xEB', 'e' }, { '\xEC', 'i' }, { '\xED', 'i' },
	{ '\xEE', 'i' }, { '\xEF', 'i' }, { '\xF0', 'o' }, { '\xF1', 'n' }, { '\xF2', 'o' }, { '\xF3', 'o' },
	{ '\xF4', 'o' }, { '\xF5', 'o' }, { '\xF6', 'o' }, { '\xF8', 'o' }, { '\xF9', 'u' }, { '\xFA', 'u' },
	{ '\xFB', 'u' }, { '\xFC', 'u' }, { '\xFD', 'y' },
	{ 0, 0 }
};

// 0x5BDCB8: build-info block (80) + signature-code block (80) + the signature code "86824943", zero-filled to 400
const char DYNAMIC_DATA_BLOCK[400] =
"DYN00000PACPOPPOPCAPPACPOPPOPCAPBUILDINFOMARKERPACPOPPOPCAPPACPOPPOPCAPXXXXXXXXX"
"00000000PACPOPPOPCAPPACPOPPOPCAPBUILDINFOMARKERPACPOPPOPCAPPACPOPPOPCAPXXXXXXXXX"
"86824943";

const char* BUILD_INFO_MARKER = DYNAMIC_DATA_BLOCK + 80;
const char* SIGNATURE_CODE_MARKER = DYNAMIC_DATA_BLOCK + 80 * 2;
const char* BETA_ID_MARKER = DYNAMIC_DATA_BLOCK + 80 * 3;

SexyApp::SexyApp()
{
	gSexyApp = this;

	mTimesPlayed = 0;
	mTimesExecuted = 0;
	mTimedOut = false;

	mIsRegistered = false;
	mBuildUnlocked = false;
	mDownloadId = 0;
	mRegSource = "ingame";
	mSkipAd = false;
	mDontUpdate = false;
	mLastVerCheckQueryTime = 0;

	mDemoPrefix = "popcap";
	mDemoFileName = mDemoPrefix + ".dmo";
	mCompanyName = "PopCap";
	mFullCompanyName = "PopCap Games";

	mInternetManager = new InternetManager();
	mBetaSupport = nullptr;
	mBetaValidate = false;

	char aStr[9] = { 0 };
	strncpy(aStr, BUILD_INFO_MARKER, 8);
	mBuildNum = atol(aStr);

	if (mBuildNum != 0)
		mBuildDate = BUILD_INFO_MARKER + 8;
}

SexyApp::~SexyApp()
{
    if (mBetaSupport)
        delete mBetaSupport;
    if (mInternetManager)
        delete mInternetManager;
}

bool SexyApp::Validate(const std::string& theUserName, const std::string& theRegCode)
{
	// toupper gets unsigned char values: the original's VS2005 C-locale toupper left bytes above
	// 0x7F unchanged, while the debug UCRT asserts on negative arguments
	BigInt n("42BF94023BBA6D040C8B81D9");
	BigInt e("11");

	ulong i;
	std::string aDataString;
	bool space = false;
	for (i = 0; i < theUserName.size(); i++)
	{
		if (theUserName[i] == ' ')
		{
			if (aDataString.length() > 0)
				space = true;
		}
		else
		{
			if (space)
			{
				aDataString += " ";
				space = false;
			}

			char aChar = theUserName[i];
			for (int j = 0; gRegKeyTranslationTable[j][0] != 0; j++)
				if (gRegKeyTranslationTable[j][0] == aChar)
					aChar = gRegKeyTranslationTable[j][1];

			aDataString += toupper((unsigned char)aChar);
		}
	}

	std::string aProduct;
	aProduct = mProdName;
	for (i = 0; i < aProduct.length(); i++)
		aProduct[i] = toupper((unsigned char)aProduct[i]);

	aDataString += "\n";
	aDataString += aProduct;
	BigInt aHash = HashString(aDataString, 94);

	BigInt aSignature = KeyToInt(theRegCode);
	BigInt aHashTest = aSignature.ModPow(e, n);

	return aHashTest == aHash;
}

// 0x4910F0: takes both strings by value and does nothing with them; this build stores no registration
static void SetRegistrationData(std::string theRegName, std::string theRegCode)
{
}

// 0x491130: the registration shared by PopCap games: popcreg.dat in the Windows folder (the folder
// above the app-data folder on Vista and later), then HKLM\SOFTWARE\<mRegKey>. The original passes
// &theRegCode in ECX, &theTimesPlayed in EDX and the rest on the stack.
static void GetRegistrationData(SexyAppBase* theApp, std::string* theRegName, std::string* theRegCode, int* theTimesPlayed, int* theTimesExecuted)
{
	char aPath[MAX_PATH];
	if (CheckForVista())
		strcpy(aPath, GetPathFrom("..", GetAppDataFolder()).c_str());
	else
		GetWindowsDirectoryA(aPath, MAX_PATH);

	if (aPath[strlen(aPath) - 1] != '\\')
		strcat(aPath, "\\");
	strcat(aPath, "popcreg.dat");

	FILE* aFP = fopen(aPath, "rb");
	if (aFP != NULL)
	{
		// Each record: ushort-length product name, reg name and reg code, then short TimesPlayed and TimesExecuted
		char aProdName[65536];
		char aRegName[65536];
		char aRegCode[65536];
		unsigned short aLen;
		short aTimesPlayed;
		short aTimesExecuted;

		while (fread(&aLen, 2, 1, aFP) == 1)
		{
			aProdName[aLen] = 0;
			fread(aProdName, 1, aLen, aFP);
			if (fread(&aLen, 2, 1, aFP) != 1)
				break;
			aRegName[aLen] = 0;
			fread(aRegName, 1, aLen, aFP);
			if (fread(&aLen, 2, 1, aFP) != 1)
				break;
			aRegCode[aLen] = 0;
			fread(aRegCode, 1, aLen, aFP);
			if ((fread(&aTimesPlayed, 2, 1, aFP) != 1) || (fread(&aTimesExecuted, 2, 1, aFP) != 1))
				break;

			if (theApp->mProdName == aProdName)
			{
				*theRegName = aRegName;
				*theRegCode = aRegCode;
				*theTimesPlayed = aTimesPlayed;
				*theTimesExecuted = aTimesExecuted;
				fclose(aFP);
				return;
			}
		}
		fclose(aFP);
	}

	std::string aKeyName = RemoveTrailingSlash("SOFTWARE\\" + theApp->mRegKey);
	HKEY aGameKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, aKeyName.c_str(), 0, KEY_WRITE, &aGameKey) == ERROR_SUCCESS)
	{
		DWORD aType;
		DWORD aLen;
		// The original's buffers are 256 bytes and it writes [aLen] with aLen up to 256; the spare
		// byte keeps that write inside the array
		BYTE aRegName[256 + 1];
		BYTE aRegCode[256 + 1];
		DWORD aValue;

		aType = REG_SZ;
		aLen = 256;
		if (RegQueryValueExA(aGameKey, "RegName", 0, &aType, aRegName, &aLen) == ERROR_SUCCESS)
		{
			aRegName[aLen] = 0;
			*theRegName = (char*)aRegName;
		}

		aType = REG_SZ;
		aLen = 256;
		if (RegQueryValueExA(aGameKey, "RegCode", 0, &aType, aRegCode, &aLen) == ERROR_SUCCESS)
		{
			aRegCode[aLen] = 0;
			*theRegCode = (char*)aRegCode;
		}

		aType = REG_DWORD;
		aLen = 4;
		if (RegQueryValueExA(aGameKey, "TimesPlayed", 0, &aType, (BYTE*)&aValue, &aLen) == ERROR_SUCCESS)
			*theTimesPlayed = aValue;

		aType = REG_DWORD;
		aLen = 4;
		if (RegQueryValueExA(aGameKey, "TimesExecuted", 0, &aType, (BYTE*)&aValue, &aLen) == ERROR_SUCCESS)
			*theTimesExecuted = aValue;

		RegCloseKey(aGameKey);
	}
}

void SexyApp::ReadFromRegistry()
{
	SexyAppBase::ReadFromRegistry();

	char aStr[1024];
	ulong aType;
	ulong aLen;
	int anInt;

	aLen = 1023;
	if (RegistryReadKey("ReferId", &aType, (uchar*)aStr, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_SZ))
	{
		aStr[aLen] = 0;
		mSexyAppReferId = aStr;
	}
	mSexyAppReferId = SexyStringToString(GetString("ReferId", StringToSexyString(mSexyAppReferId)));

	aLen = 1023;
	if (RegistryReadKey("RegisterLink", &aType, (uchar*)aStr, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_SZ))
	{
		aStr[aLen] = 0;
		mRegisterLink = aStr;
	}
	else
	{
		mRegisterLink = "http://www.popcap.com/register.php?theGame=" + mProdName + "&referid=" + mSexyAppReferId;
	}

	aLen = 1023;
	if (RegistryReadKey("Variation", &aType, (uchar*)aStr, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_SZ))
	{
		aStr[aLen] = 0;
		mVariation = aStr;
	}

	aLen = 1023;
	if (RegistryReadKey("PartnerName", &aType, (uchar*)aStr, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_SZ))
	{
		aStr[aLen] = 0;
		mFullCompanyName = aStr;
	}

	aLen = 4;
	if (RegistryReadKey("DontUpdate", &aType, (uchar*)&anInt, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_DWORD))
		mDontUpdate = anInt != 0;

	aLen = 4;
	if (RegistryReadKey("DownloadId", &aType, (uchar*)&anInt, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_DWORD))
		mDownloadId = anInt;

	if (RegistryReadInteger("LastVerCheckQueryTime", &anInt))
	{
		mLastVerCheckQueryTime = anInt;
	}
	// sic: aLen is not reset to 4 before this read
	else if (RegistryReadKey("LastVerCheckQueryTime", &aType, (uchar*)&anInt, &aLen, HKEY_LOCAL_MACHINE) && (aType == REG_DWORD))
	{
		mLastVerCheckQueryTime = anInt;
	}
	else
	{
		time_t aTimeNow;
		time(&aTimeNow);

		mLastVerCheckQueryTime = (ulong)aTimeNow;
	}

	std::string aRegName;
	std::string aRegCode;
	int aTimesPlayed = 0;
	int aTimesExecuted = 0;
	GetRegistrationData(this, &aRegName, &aRegCode, &aTimesPlayed, &aTimesExecuted);
	mTimesPlayed = aTimesPlayed;
	mTimesExecuted = aTimesExecuted;
	mUserName = aRegName;
	mRegCode = aRegCode;

	// mRegUserName is never loaded here, as in the original
	mIsRegistered |= Validate(mRegUserName, mRegCode);

	// Override registry values with partner.xml values
	mRegisterLink = SexyStringToString(GetString("RegisterLink", StringToSexyString(mRegisterLink)));
	mDontUpdate = GetBoolean("DontUpdate", mDontUpdate);
	mFullCompanyName = SexyStringToString(GetString("PartnerName", StringToSexyString(mFullCompanyName)));
}

void SexyApp::WriteToRegistry()
{
	SexyAppBase::WriteToRegistry();

	RegistryWriteInteger("LastVerCheckQueryTime", mLastVerCheckQueryTime);

	// This is for "compatibility"
	if ((mRegUserName.length() == 0) &&
		(mUserName.length() > 0) &&
		(mRegCode.length() > 0))
		mRegUserName = mUserName;

	SetRegistrationData(mRegUserName, mRegCode);
}

bool SexyApp::OpenHTMLTemplate(const std::string& theTemplateFile, const DefinesMap& theDefinesMap)
{
	std::fstream anInStream(theTemplateFile.c_str(), std::ios::in);

	if (!anInStream.is_open())
		return false;

	WIN32_FIND_DATA aFindData;
	HANDLE aHandle = FindFirstFile("temp\\tpl*.html", &aFindData);
	if (aHandle != NULL)
	{
		do
		{
			std::string aFilePath = std::string("temp\\") + aFindData.cFileName;
			DeleteFile(aFilePath.c_str());
		} while (FindNextFile(aHandle, &aFindData));

		FindClose(aHandle);
	}

	mkdir("temp");

	std::string anOutFilename = StrFormat("temp\\tpl%04d.html", rand() % 10000);

	//TODO: A better failover case?
	std::fstream anOutStream(anOutFilename.c_str(), std::ios::out);
	if (!anOutStream.is_open())
		return false;

	char aStr[4096];
	while (!anInStream.eof())
	{
		anInStream.getline(aStr, 4096);

		std::string aNewString = Evaluate(aStr, theDefinesMap);

		anOutStream << aNewString.c_str() << std::endl;
	}

	return OpenURL(GetFullPath(anOutFilename));
}

bool SexyApp::OpenRegisterPage(DefinesMap theStatsMap)
{
#ifdef ZYLOM
	ZylomGS_StandAlone_ShowBuyPage();
	return true;
#endif

	// Insert standard defines 
	DefinesMap aDefinesMap;

	aDefinesMap.insert(DefinesMap::value_type("Src", mRegSource));
	aDefinesMap.insert(DefinesMap::value_type("ProdName", mProdName));
	aDefinesMap.insert(DefinesMap::value_type("Version", mProductVersion));
	aDefinesMap.insert(DefinesMap::value_type("Variation", mVariation));
	aDefinesMap.insert(DefinesMap::value_type("ReferId", mSexyAppReferId));
	aDefinesMap.insert(DefinesMap::value_type("DownloadId", StrFormat("%d", mDownloadId)));
	aDefinesMap.insert(DefinesMap::value_type("TimesPlayed", StrFormat("%d", mTimesPlayed)));
	aDefinesMap.insert(DefinesMap::value_type("TimesExecuted", StrFormat("%d", mTimesExecuted)));
	aDefinesMap.insert(DefinesMap::value_type("TimedOut", mTimedOut ? "Y" : "N"));

	// Insert game specific stats 
	std::string aStatsString;
	DefinesMap::iterator anItr = theStatsMap.begin();
	while (anItr != theStatsMap.end())
	{
		std::string aKeyString = anItr->first;
		std::string aValueString = anItr->second;

		aStatsString +=
			StrFormat("%04X", aKeyString.length()) + aKeyString +
			"S" +
			StrFormat("%04X", aValueString.length()) + aValueString;

		++anItr;
	}

	aDefinesMap.insert(DefinesMap::value_type("Stats", aStatsString));

	if (FileExists("register.tpl"))
	{
		return OpenHTMLTemplate("register.tpl", aDefinesMap);
	}
	else
	{
		return OpenURL(mRegisterLink);
	}
}

bool SexyApp::ShouldCheckForUpdate()
{
	if (mDontUpdate)
		return false;

#ifdef ZYLOM
	return ZylomUpdateCheckNeeded();
#else
	time_t aTimeNow;
	time(&aTimeNow);

	// It is set to 0 if we crash, otherwise ask every week
	return ((mLastVerCheckQueryTime == 0) ||
		(!mLastShutdownWasGraceful) ||
		((mLastVerCheckQueryTime != 0) &&
			(aTimeNow - mLastVerCheckQueryTime > 7 * 24 * 60 * 60)));
#endif
}

void SexyApp::UpdateCheckQueried()
{
	time_t aTimeNow;
	time(&aTimeNow);

	mLastVerCheckQueryTime = aTimeNow;
}

void SexyApp::URLOpenSucceeded(const std::string& theURL)
{
	SexyAppBase::URLOpenSucceeded(theURL);

	if (mShutdownOnURLOpen)
		mSkipAd = true;
}

bool SexyApp::OpenRegisterPage()
{
	DefinesMap aStatsMap;
	return OpenRegisterPage(aStatsMap);
}

bool SexyApp::CheckSignature(const Buffer& theBuffer, const std::string& theFileName)
{
	if (mSkipSignatureChecks)
		return true;

	char aSigStr[25];

	FILE* aFP = fopen((theFileName + ".sig").c_str(), "rb");
	if (aFP == NULL)
		return false;

	fread(aSigStr, 1, 24, aFP);
	aSigStr[24] = 0;

	fclose(aFP);

	char* aFileData = new char[theBuffer.GetDataLen()+4];
	int aFileDataPos = 0;

	char aStr[9] = {0};
	strncpy(aStr, SIGNATURE_CODE_MARKER, 8);
	int aSignatureCode = atol(aStr);

	aFileData[aFileDataPos++] = (aSignatureCode & 0xFF);
	aFileData[aFileDataPos++] = ((aSignatureCode >> 8) & 0xFF);
	aFileData[aFileDataPos++] = ((aSignatureCode >> 16) & 0xFF);
	aFileData[aFileDataPos++] = ((aSignatureCode >> 24) & 0xFF);

	theBuffer.SeekFront();
	while (!theBuffer.AtEnd())
	{
		unsigned char c = theBuffer.ReadByte();
		// The original also calls fread(&c, 1, 1, aFP) here on the already closed aFP. The VS2005 CRT
		// reads nothing and leaves c unchanged; on the UCRT that call is undefined, so it is left out.
		if (!::isspace(c))
			aFileData[aFileDataPos++] = c;
	}

	// Public RSA stuff
	BigInt n("D99BC76AB7B2578738E606F7");
	BigInt e("11");

	BigInt aHash = HashData(aFileData, aFileDataPos, 94);
	delete [] aFileData;

	BigInt aSignature(aSigStr);
	BigInt aHashTest = aSignature.ModPow(e, n);

	return aHashTest == aHash;
}

void SexyApp::PreTerminate()
{
#ifdef ZYLOM
	ZylomShowAd();
#else
	if ((!mSkipAd) &&
		((((!mIsRegistered) || (mInternetManager->HasNewAds())) && ((Rand()%2) == 0))))
	{
		mInternetManager->TryShowAd();
	}
#endif
}

void SexyApp::OpenUpdateURL()
{
#ifdef ZYLOM
	ZylomGS_StandAlone_ShowUpdatePage();
#else
	OpenURL(mInternetManager->GetUpdateURL(), true);	
#endif
	Shutdown();
}

void SexyApp::HandleCmdLineParam(const std::string& theParamName, const std::string& theParamValue)
{
	if (theParamName == "-version")
	{
		// Just print version info and then quit

		std::string aVersionString =
			"Product: " + mProdName + "\r\n" +
			"Version: " + mProductVersion + "\r\n" +
			"Build Num: " + SexyStringToString(StrFormat(_S("%d"), mBuildNum)) + "\r\n" +
			"Build Date: " + mBuildDate;

		MessageBox(NULL, aVersionString.c_str(), "Version Info", MB_ICONINFORMATION | MB_OK);
		DoExit(0);
	}
	else
		SexyAppBase::HandleCmdLineParam(theParamName, theParamValue);
}

std::string SexyApp::GetGameSEHInfo()
{
	char aGamesPlayedStr[16];
	sprintf(aGamesPlayedStr, "%d", mTimesPlayed);

	std::string anInfoString = SexyAppBase::GetGameSEHInfo() +
		"Times Played: " + std::string(aGamesPlayedStr) + "\r\n" +
		"Build Num: " + StrFormat("%d", mBuildNum) + "\r\n" +
		"Build Date: " + mBuildDate + "\r\n";

	if (mSexyAppReferId.length() != 0)
	{
		anInfoString +=
			"ReferId: " + mSexyAppReferId + "\r\n";
	}

	return anInfoString;
}

void SexyApp::GetSEHWebParams(DefinesMap* theDefinesMap)
{
	theDefinesMap->insert(DefinesMap::value_type("username", mUserName));
	theDefinesMap->insert(DefinesMap::value_type("buildnum", StrFormat("%d", mBuildNum)));
	theDefinesMap->insert(DefinesMap::value_type("builddate", mBuildDate));
	theDefinesMap->insert(DefinesMap::value_type("referid", mSexyAppReferId));
}

void SexyApp::PreDisplayHook()
{
	if (mBetaValidate && !mBetaSupport->Validate())
	{
		Shutdown();
		DoExit(0);
		return;
	}
}

void SexyApp::InitPropertiesHook()
{
	// Load properties if we need to
	bool checkSig = !IsScreenSaver();
	LoadProperties("properties\\partner.xml", false, checkSig);

	// Check to see if this build is unlocked.
	if (GetBoolean("NoReg", false))
	{
		mIsRegistered = true;
		mBuildUnlocked = true;
	}

	mProdName = SexyStringToString(GetString("ProdName", StringToSexyString(mProdName)));
	mIsWindowed = GetBoolean("DefaultWindowed", mIsWindowed);

	SexyString aNewTitle = GetString("Title", _S(""));
	if (aNewTitle.length() > 0)
		mTitle = aNewTitle + _S(" ") + StringToSexyString(mProductVersion);

	mInternetManager->ReadAdList();

	mBetaSupport = new BetaSupport(this);

#ifdef ZYLOM
	LoadProperties();
	ZylomGS_StandAlone_Init(mZylomGameId, (char*)GetString("BUG_REPORT_TITLE").c_str(), (char*)GetString("BUG_REPORT_BODY").c_str());
#endif
}

void SexyApp::Init()
{
	SEHCatcher::mCrashMessage = SexyStringToWString(GetString("UNEXPECTED_ERROR",
		"An unexpected error has occured!  Pressing 'Send Report' "
		"will send us helpful debugging information that may help "
		"us resolve this issue in the future.\r\n\r\n"
		"You can also contact us directly at feedback@popcap.com."));

	SEHCatcher::mSubmitMessage = SexyStringToWString(GetString("PLEASE_HELP",
		"Please help us out by providing as much information as "
		"you can about this crash. Is this the first time it happened? "
		"Have you used other PopCap Deluxe games successfully before? "
		"Have you upgraded your drivers or any software recently that "
		"may be interfering with this program?"));

	SEHCatcher::mSubmitErrorMessage = SexyStringToWString(GetString("FAILED_CONNECT_POPCAP",
		"Failed to connect to PopCap servers.  Please check your Internet connection.\n"
		"If you are on a dial-up connection, you may have to manually connect to your ISP."));

	SEHCatcher::mSubmitHost = "www.popcap.com";

	OutputDebug("Product: %s\r\n", mProdName.c_str());
	OutputDebug("BuildNum: %d\r\n", mBuildNum);
	OutputDebug("BuildDate: %s\r\n", mBuildDate.c_str());

	ImageLib::SetJ2KCodecKey("LU5OkT7!L06fiD9nMbCE9ZoJuXxgx4zcgVDd2!a1tb2uHfhRrLG5wnNySQswXqAt2h");

	SexyAppBase::Init();

	if (IsScreenSaver())
		mSkipAd = true;

	mTimesExecuted++;
}

void SexyApp::UpdateFrames()
{
	SexyAppBase::UpdateFrames();

	mInternetManager->Update();
}

#ifdef ZYLOM

bool SexyApp::ZylomUpdateCheckNeeded()
{
	return ZylomGS_StandAlone_UpdateCheckNeeded();
}

void SexyApp::ZylomShowAd()
{
	ZylomGS_StandAlone_ShowAd(mIsRegistered);
}

#endif
