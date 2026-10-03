#include "SexyCache.h"
#include "AutoCrit.h"

using namespace Sexy;

SexyCache Sexy::gSexyCache;

SexyCache::SexyCache()
{
	mMessage = 0;
	mHeader.mHWnd = NULL;
	mHeader.mUnknown0 = 0;

	HANDLE aFileMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "SexyCacheData");
	if (aFileMapping != NULL)
	{
		SexyCacheHeader* aHeader = (SexyCacheHeader*) MapViewOfFile(aFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SexyCacheHeader));
		mHeader = *aHeader;
		UnmapViewOfFile(aHeader);
		CloseHandle(aFileMapping);

		mMessage = RegisterWindowMessageA("SexyCacheMessage");
	}
}

bool SexyCache::Connected()
{
	return mMessage != 0;
}

bool SexyCache::GetData(const std::string& theSrcFile, const std::string& theDataType, void** thePtr, int* theSize)
{
	if (mMessage == 0)
		return false;

	char aName[256];
	sprintf(aName, "SexyCacheReq#%d", GetCurrentThreadId());
	HANDLE aReqMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x304, aName);
	char* aReq = (char*) MapViewOfFile(aReqMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0x304);
	strcpy(aReq, theSrcFile.c_str());
	strcpy(aReq + 0x100, theDataType.c_str());
	*((int*) (aReq + 0x300)) = 0;

	bool aSuccess = false;
	int aDataId = SendMessageA(mHeader.mHWnd, mMessage, 0, GetCurrentThreadId());
	if (aDataId != 0)
	{
		char aDataName[260];
		sprintf(aDataName, "SexyCacheData#%d", aDataId);
		HANDLE aDataMapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, aDataName);
		if (aDataMapping != NULL)
		{
			void* aData = MapViewOfFile(aDataMapping, FILE_MAP_ALL_ACCESS, 0, 0, *((int*) (aReq + 0x300)));
			*thePtr = aData;

			AutoCrit anAutoCrit(mCritSect);
			mMappingMap.insert(MappingMap::value_type(aData, aDataMapping));
			aSuccess = true;
		}

		// Set even when the data mapping could not be opened
		*theSize = *((int*) (aReq + 0x300));
	}

	UnmapViewOfFile(aReq);
	CloseHandle(aReqMapping);
	return aSuccess;
}

void SexyCache::FreeGetData(void* thePtr)
{
	AutoCrit anAutoCrit(mCritSect);

	MappingMap::iterator anItr = mMappingMap.find(thePtr);
	if (anItr != mMappingMap.end())
	{
		UnmapViewOfFile(anItr->first);
		CloseHandle(anItr->second);
		mMappingMap.erase(anItr);
	}
}

void SexyCache::SetFileDeps(const std::string& theSrcFile, const std::string& theDataType, const std::string& theFileDeps)
{
	void* aData = AllocSetData(theSrcFile, theDataType, (int) theFileDeps.length() + 1);
	if (aData != NULL)
	{
		memcpy(aData, theFileDeps.c_str(), theFileDeps.length() + 1);
		SendMessageA(mHeader.mHWnd, mMessage, 2, GetCurrentThreadId());
		FreeSetData(aData);
	}
}

void* SexyCache::AllocSetData(const std::string& theSrcFile, const std::string& theDataType, int theSize)
{
	if (mMessage == 0)
		return NULL;

	char aName[260];
	sprintf(aName, "SexyCacheReq#%d", GetCurrentThreadId());
	HANDLE aReqMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, theSize + 0x304, aName);
	char* aReq = (char*) MapViewOfFile(aReqMapping, FILE_MAP_ALL_ACCESS, 0, 0, theSize + 0x304);
	strcpy(aReq, theSrcFile.c_str());
	strcpy(aReq + 0x100, theDataType.c_str());
	*((int*) (aReq + 0x300)) = theSize;

	AutoCrit anAutoCrit(mCritSect);
	void* aData = aReq + 0x304;
	mMappingMap.insert(MappingMap::value_type(aData, aReqMapping));
	return aData;
}

void SexyCache::FreeSetData(void* thePtr)
{
	AutoCrit anAutoCrit(mCritSect);

	MappingMap::iterator anItr = mMappingMap.find(thePtr);
	if (anItr != mMappingMap.end())
	{
		UnmapViewOfFile((char*) anItr->first - 0x304);
		CloseHandle(anItr->second);
		mMappingMap.erase(anItr);
	}
}

bool SexyCache::SetData(void* thePtr)
{
	{
		AutoCrit anAutoCrit(mCritSect);
		if (mMappingMap.find(thePtr) == mMappingMap.end())
			return false;
	}

	if (SendMessageA(mHeader.mHWnd, mMessage, 1, GetCurrentThreadId()) == -1)
	{
		// The server is gone
		mMessage = 0;
		return false;
	}

	return true;
}

void Sexy::SMemR(void*& theSrc, void* theDest, int theSize)
{
	memcpy(theDest, theSrc, theSize);
	theSrc = (char*) theSrc + theSize;
}

void Sexy::SMemRStr(void*& theSrc, std::string& theString)
{
	int aLength;
	SMemR(theSrc, &aLength, sizeof(int));
	theString.resize(aLength);
	SMemR(theSrc, (void*) theString.c_str(), aLength);
}

void Sexy::SMemW(void*& theDest, const void* theSrc, int theSize)
{
	memcpy(theDest, theSrc, theSize);
	theDest = (char*) theDest + theSize;
}

void Sexy::SMemWStr(void*& theDest, const std::string& theString)
{
	int aLength = (int) theString.length();
	SMemW(theDest, &aLength, sizeof(int));
	SMemW(theDest, theString.c_str(), aLength);
}
