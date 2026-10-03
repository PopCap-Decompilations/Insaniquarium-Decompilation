#include <SexyAppFramework/HTTPTransfer.h>

#include "InternetManager.h"
#include "WinFishApp.h"

#include <direct.h>
#include <ole2.h>
#include <exdisp.h>

using namespace Sexy;

Sexy::InternetManager::InternetManager()
{
	mUpdateChecked = false;
	mUpToDate = true;
	mAdsLoading = false;
	mMaxMoreAds = 4;
}

Sexy::InternetManager::~InternetManager()
{
	SaveAdList();
}

void Sexy::InternetManager::Update()
{
	if (!mAdsLoading)
		return;

	for (int i = 0;i < MAX_HTTP_TRANSFERS; i++)
	{
		HTTPTransfer* aCurTransf = &mHTTPTransfers[i];

		int aResCode = aCurTransf->GetResultCode();
		if (aResCode == HTTPTransfer::RESULT_DONE)
		{
			SexyString aRelPath = aCurTransf->mSpecifiedRelURL;

			size_t aSlashPos = aRelPath.find('/');
			while (aSlashPos != std::string::npos)
			{
				_mkdir(aRelPath.substr(0, aSlashPos).c_str());
				aSlashPos = aRelPath.find('/', aSlashPos + 1);
			}

			OutputDebugStringA(StrFormat("AdFile: %s\r\n", aRelPath.c_str()).c_str());

			FILE* aFile = fopen(aRelPath.c_str(), "wb");
			if (aFile != nullptr)
			{
				SexyString aContent = aCurTransf->GetContent();
				fwrite(aContent.c_str(), 1, aContent.length(), aFile);
				fclose(aFile);
			}

			aCurTransf->Reset();
		}
		else if (aResCode == HTTPTransfer::RESULT_NOT_COMPLETED)
		{
			continue;
		}
		else if (aResCode != HTTPTransfer::RESULT_NOT_STARTED)
		{
			OutputDebugStringA(("Error on AdFile: " + aCurTransf->mSpecifiedRelURL + "\r\n").c_str());

			aCurTransf->Reset();
		}

		// A finished, failed or idle transfer takes the next ad file
		if (mAdUrlList.size() > 0)
		{
			SexyString aNextUrl = mAdUrlList.front();
			mAdUrlList.pop_front();

			// Ad files are relative to the update-check URL
			aCurTransf->Get(mUpdateTransfer.mURL, aNextUrl);
		}
	}
}

bool Sexy::InternetManager::CheckForUpdates()
{
	if (!mUpdateChecked)
	{
		if (mUpdateTransfer.GetResultCode() != HTTPTransfer::RESULT_DONE)
			return false;

		mAdsLoading = true;
		mOldAdList = mAdList;
		mAdList.clear();
		mAdUrlList.clear();

		// Reply: line 0 is the up-to-date flag, line 1 the update URL, the rest are ad lines
		int aLineNum = 0;
		SexyString aContent = mUpdateTransfer.GetContent();
		while (aContent.length() > 0)
		{
			int anEndPos = aContent.find('\n');
			if (anEndPos == -1)
				anEndPos = aContent.length();

			SexyString aLine = aContent.substr(0, anEndPos);
			while ((aLine.length() > 0) &&
				((aLine[aLine.length() - 1] == '\r') || (aLine[aLine.length() - 1] == ' ')))
				aLine.resize(aLine.length() - 1);

			if (aLine.length() > 0)
			{
				switch (aLineNum)
				{
				case 0:
					mUpToDate = atol(aLine.c_str()) != 0;
					break;
				case 1:
					mUpdateURL = aLine;
					break;
				default:
					ParseAdLine(aLine, true);
					break;
				}
				aLineNum++;
			}

			if (anEndPos + 1 >= (int)aContent.length())
				break;
			aContent = aContent.substr(anEndPos + 1);
		}

		UpdateMoreAds();
	}

	return true;
}

void Sexy::InternetManager::ParseAdLine(const SexyString& theLine, bool fromUpdateCheck)
{
	// isdigit gets unsigned char values: the debug UCRT asserts on negative arguments
	if (theLine.length() == 0)
		return;

	SexyString aLine = theLine;
	AdInfo anAdInfo;
	int aFieldNum = 0;
	int aSectionNum = 0;
	SexyString aToken;

	int aStartPos = aLine.find_first_not_of(' ');
	int aSpacePos = aLine.find(' ', aStartPos + 1);
	if ((aSpacePos == -1) || (aStartPos == -1))
		return;

	aToken = StringToLower(aLine.substr(aStartPos, aSpacePos - aStartPos));
	if ((aToken == "popup") || (isdigit((unsigned char)aToken[0])))
		anAdInfo.mType = AD_TYPE_POPUP;
	else if (aToken == "more")
		anAdInfo.mType = AD_TYPE_MORE;
	else
		return;

	// A line that starts with a number is a popup line without the "popup" keyword
	if (!isdigit((unsigned char)aToken[0]))
		aLine = aLine.substr(aSpacePos + 1);

	// Sections are separated by "|": the type's two fields, the ad's files, then the saved flags
	while (aLine.length() > 0)
	{
		int anEndPos = aLine.find(' ');
		if (anEndPos == -1)
			anEndPos = aLine.length();
		aToken = aLine.substr(0, anEndPos);

		if (aToken == "|")
		{
			aSectionNum++;
			aFieldNum = 0;
		}
		else
		{
			switch (aSectionNum)
			{
			case 0:
				if (anAdInfo.mType == AD_TYPE_POPUP)
				{
					switch (aFieldNum)
					{
					case 0:
						anAdInfo.mWidth = atol(aToken.c_str());
						break;
					case 1:
						anAdInfo.mHeight = atol(aToken.c_str());
						break;
					}
				}
				else if (anAdInfo.mType == AD_TYPE_MORE)
				{
					switch (aFieldNum)
					{
					case 0:
						anAdInfo.mMoreArg1 = aToken;
						break;
					case 1:
						anAdInfo.mMoreGroup = aToken;
						break;
					}
				}
				break;
			case 1:
				anAdInfo.mFiles.push_back(aToken);
				if (std::find(mAdUrlList.begin(), mAdUrlList.end(), aToken) == mAdUrlList.end())
					mAdUrlList.push_back(aToken);

				if (anAdInfo.mFiles.size() == 1)
				{
					anAdInfo.mIsNew = true;
					AdInfo* anOldAdInfo = FindAdInfo(mOldAdList, anAdInfo);
					if (anOldAdInfo != NULL)
					{
						anAdInfo.mIsNew = anOldAdInfo->mIsNew;
						anAdInfo.mMoreActive = anOldAdInfo->mMoreActive;
						anAdInfo.mNewThisSession = anOldAdInfo->mNewThisSession;
					}
					else
					{
						anAdInfo.mNewThisSession = fromUpdateCheck;
					}
				}
				break;
			case 2:
				switch (aFieldNum)
				{
				case 0:
					anAdInfo.mIsNew = atol(aToken.c_str()) != 0;
					break;
				case 1:
					anAdInfo.mMoreActive = atol(aToken.c_str()) != 0;
					break;
				}
				break;
			}

			aFieldNum++;
		}

		if (anEndPos + 1 >= (int)aLine.length())
			break;
		aLine = aLine.substr(anEndPos + 1);
	}

	mAdList.push_back(anAdInfo);
}

// Finds the ad in theList whose first file matches theAdInfo's
AdInfo* Sexy::InternetManager::FindAdInfo(AdInfoList& theList, AdInfo& theAdInfo)
{
	if (theAdInfo.mFiles.size() > 0)
	{
		AdInfoList::iterator anItr = theList.begin();
		while (anItr != theList.end())
		{
			// Dereferenced once: the original has no iterator check between the tests and the return
			AdInfo* anAdInfo = &(*anItr);
			if ((anAdInfo->mFiles.size() > 0) && (anAdInfo->mFiles.front() == theAdInfo.mFiles.front()))
				return anAdInfo;
			++anItr;
		}
	}

	return NULL;
}

// Keeps up to mMaxMoreAds downloaded "more" ads active, at most one per group
void Sexy::InternetManager::UpdateMoreAds()
{
	AdInfoList aCandidateList;
	std::set<SexyString, StringLessNoCase> aGroupSet;

	// Each loop dereferences its iterator once per ad, as in the original

	// "more" ads the server dropped whose files are still on disk stay candidates
	AdInfoList::iterator anItr = mOldAdList.begin();
	while (anItr != mOldAdList.end())
	{
		AdInfo* anAdInfo = &(*anItr);
		if (anAdInfo->mType == AD_TYPE_MORE)
		{
			if (FindAdInfo(mAdList, *anAdInfo) == NULL)
			{
				anAdInfo->CheckFiles();
				if (anAdInfo->mDownloaded)
					aCandidateList.push_back(*anAdInfo);
			}
		}
		++anItr;
	}

	int aNumMoreAds = 0;
	anItr = mAdList.begin();
	while (anItr != mAdList.end())
	{
		AdInfo* anAdInfo = &(*anItr);
		if (anAdInfo->mType == AD_TYPE_MORE)
		{
			anAdInfo->CheckFiles();
			if (anAdInfo->mDownloaded)
			{
				if (anAdInfo->mMoreActive)
				{
					aCandidateList.push_back(*anAdInfo);
				}
				else if (!anAdInfo->mNewThisSession)
				{
					aGroupSet.insert(anAdInfo->mMoreGroup);
					aNumMoreAds++;
				}
			}
		}

		if (anAdInfo->mMoreActive)
			anItr = mAdList.erase(anItr);
		else
			++anItr;
	}

	anItr = aCandidateList.begin();
	while ((anItr != aCandidateList.end()) && (aNumMoreAds < mMaxMoreAds))
	{
		AdInfo* anAdInfo = &(*anItr);
		if (aGroupSet.find(anAdInfo->mMoreGroup) == aGroupSet.end())
		{
			aGroupSet.insert(anAdInfo->mMoreGroup);
			anAdInfo->mMoreActive = true;
			mAdList.push_back(*anAdInfo);
			aNumMoreAds++;
			anItr = aCandidateList.erase(anItr);
		}
		else
		{
			++anItr;
		}
	}

	anItr = aCandidateList.begin();
	while ((anItr != aCandidateList.end()) && (aNumMoreAds < mMaxMoreAds))
	{
		AdInfo* anAdInfo = &(*anItr);
		anAdInfo->mMoreActive = true;
		mAdList.push_back(*anAdInfo);
		aNumMoreAds++;
		++anItr;
	}
}

bool Sexy::InternetManager::LoadAdList()
{
	Buffer aBuffer;
	if (!gSexyAppBase->ReadBufferFromFile("adlist.txt", &aBuffer))
		return false;

	while (!aBuffer.AtEnd())
	{
		SexyString aLine = aBuffer.ReadLine();
		ParseAdLine(aLine, false);
	}

	UpdateMoreAds();
	return true;
}

bool Sexy::InternetManager::SaveAdList()
{
	if (mAdList.size() == 0)
		return true;

	Buffer aBuffer;
	UpdateMoreAds();

	AdInfoList::iterator anItr = mAdList.begin();
	while (anItr != mAdList.end())
	{
		SexyString aLine;
		anItr->GetLine(aLine);
		aBuffer.WriteLine(aLine);
		++anItr;
	}

	return gSexyAppBase->WriteBufferToFile("adlist.txt", &aBuffer);
}

void Sexy::AdInfo::GetLine(SexyString& theLine)
{
	if (mType == AD_TYPE_POPUP)
		theLine = StrFormat("popup %d %d |", mWidth, mHeight);
	else if (mType == AD_TYPE_MORE)
		theLine = StrFormat("more %s %s |", mMoreArg1.c_str(), mMoreGroup.c_str());

	std::list<SexyString>::iterator anItr = mFiles.begin();
	while (anItr != mFiles.end())
	{
		theLine += " ";
		theLine += *anItr;
		++anItr;
	}

	theLine += " | ";
	theLine += mIsNew ? "1 " : "0 ";
	theLine += mMoreActive ? "1 " : "0 ";
}

void Sexy::AdInfo::CheckFiles()
{
	mDownloaded = true;

	std::list<SexyString>::iterator anItr = mFiles.begin();
	while (anItr != mFiles.end())
	{
		if (!gSexyAppBase->FileExists(*anItr))
			mDownloaded = false;
		++anItr;
	}

	if (mFiles.size() == 0)
		mDownloaded = false;
}

void Sexy::InternetManager::CheckAdFiles()
{
	AdInfoList::iterator anItr = mAdList.begin();
	while (anItr != mAdList.end())
	{
		anItr->CheckFiles();
		++anItr;
	}
}

bool Sexy::InternetManager::HasNewAds()
{
	CheckAdFiles();

	AdInfoList::iterator anItr = mAdList.begin();
	while (anItr != mAdList.end())
	{
		AdInfo* anAdInfo = &(*anItr);
		if ((anAdInfo->mType == AD_TYPE_POPUP) && (anAdInfo->mDownloaded) && (anAdInfo->mIsNew))
			return true;
		++anItr;
	}

	return false;
}

// Opens a downloaded popup ad (the first new one, else a random one) in an Internet Explorer window
void Sexy::InternetManager::TryShowAd()
{
	AdInfo* anAdInfo = NULL;
	int aNumAds = 0;

	CheckAdFiles();

	AdInfoList::iterator anItr = mAdList.begin();
	while (anItr != mAdList.end())
	{
		AdInfo* aCurAdInfo = &(*anItr);
		if ((aCurAdInfo->mDownloaded) && (aCurAdInfo->mType == AD_TYPE_POPUP))
		{
			if ((aCurAdInfo->mIsNew) && (anAdInfo == NULL))
				anAdInfo = aCurAdInfo;
			aNumAds++;
		}
		++anItr;
	}

	if (anAdInfo == NULL)
	{
		if (aNumAds <= 0)
			return;

		int anAdNum = Rand() % aNumAds;
		anItr = mAdList.begin();
		while (anItr != mAdList.end())
		{
			anAdInfo = &(*anItr);
			if ((anAdInfo->mDownloaded) && (anAdInfo->mType == AD_TYPE_POPUP))
			{
				if (anAdNum-- == 0)
					break;
			}
			++anItr;
		}

		if (anAdInfo == NULL)
			return;
	}

	if (gSexyAppBase->mPlayingDemoBuffer)
		return;

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		IWebBrowser2* aBrowser = NULL;
		if (SUCCEEDED(CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (void**)&aBrowser)))
		{
			RECT aRect;
			aRect.left = 0;
			aRect.top = 0;
			aRect.right = anAdInfo->mWidth;
			aRect.bottom = anAdInfo->mHeight;
			AdjustWindowRect(&aRect, WS_POPUP | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

			aBrowser->put_AddressBar(VARIANT_FALSE);
			aBrowser->put_MenuBar(VARIANT_FALSE);
			aBrowser->put_StatusBar(VARIANT_FALSE);
			aBrowser->put_ToolBar(FALSE);
			aBrowser->put_Resizable(VARIANT_FALSE);
			aBrowser->put_Width(aRect.right - aRect.left);
			aBrowser->put_Height(aRect.bottom - aRect.top);

			char aURL[4096];
			strcpy(aURL, "file://");
			_getcwd(aURL + strlen(aURL), 4096);
			strcat(aURL, "\\");
			strcat(aURL, anAdInfo->mFiles.front().c_str());

			VARIANT aVariant = {0};

			WCHAR aWideURL[4096];
			mbstowcs(aWideURL, aURL, 4096);
			BSTR aBStr = SysAllocString(aWideURL);
			aBrowser->Navigate(aBStr, &aVariant, &aVariant, &aVariant, &aVariant);
			SysFreeString(aBStr);

			aBrowser->put_Visible(VARIANT_TRUE);
			aBrowser->Release();
		}
	}

	anAdInfo->mIsNew = false;
}

void Sexy::InternetManager::StartAdLoading()
{
	CheckForUpdates();

	// Files already on disk are not downloaded again
	std::list<SexyString>::iterator anItr = mAdUrlList.begin();
	while (anItr != mAdUrlList.end())
	{
		SexyString aFileName = *anItr;
		if (gSexyAppBase->FileExists(aFileName))
			anItr = mAdUrlList.erase(anItr);
		else
			++anItr;
	}
}

bool Sexy::InternetManager::IsUpToDate()
{
	if (!CheckForUpdates())
		return true;
	return mUpToDate;
}

void Sexy::InternetManager::StartUpdateCheck(const SexyString& theURL)
{
	mUpdateChecked = false;
	mUpdateTransfer.Get(theURL);
}

SexyString Sexy::InternetManager::GetUpdateURL()
{
	if (!CheckForUpdates())
		return "";
	return mUpdateURL;
}

int Sexy::InternetManager::GetUpdateResultCode()
{
	switch (mUpdateTransfer.GetResultCode())
	{
	case HTTPTransfer::RESULT_DONE:
		return HTTPTransfer::RESULT_NOT_COMPLETED;
	case HTTPTransfer::RESULT_NOT_STARTED:
		return HTTPTransfer::RESULT_DONE;
	case HTTPTransfer::RESULT_NOT_COMPLETED:
		return HTTPTransfer::RESULT_NOT_STARTED;
	default:
		return HTTPTransfer::RESULT_NOT_FOUND;
	}
}

void Sexy::InternetManager::ReadAdList()
{
	LoadAdList();
}

