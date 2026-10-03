#ifndef __SEXYCACHE_H__
#define __SEXYCACHE_H__

// Client for PopCap's out-of-process asset cache. The original has no RTTI or symbols for it:
// the class, member and helper names are reconstructed (from the "SexyCache..." strings); the
// behaviour follows the disassembly at 0x41C380-0x41CBF5. The global object is at 0x5EC2EC.
//
// The cache server publishes a "SexyCacheData" file mapping and a window that answers the
// "SexyCacheMessage" message. Every request goes through a per-thread "SexyCacheReq#<id>"
// mapping laid out as: +0x000 source file (char[256]), +0x100 data type (char[512]),
// +0x300 int size, +0x304 payload. When the server is not running the message id stays 0,
// so every call returns at once.

#include "Common.h"
#include "CritSect.h"

namespace Sexy
{

// The first 16 bytes of the server's "SexyCacheData" mapping; only the window is used
struct SexyCacheHeader
{
	DWORD					mUnknown0;
	HWND					mHWnd;
	DWORD					mUnknown2;
	DWORD					mUnknown3;
};

class SexyCache
{
public:
	typedef std::map<void*, HANDLE> MappingMap;

	CritSect				mCritSect;			// +0x00
	SexyCacheHeader			mHeader;			// +0x18
	UINT					mMessage;			// +0x28 0 when no cache server is running
	MappingMap				mMappingMap;		// +0x2C mapped view -> file mapping handle

public:
	SexyCache();																	// 0x41C3E0 (the destructor is implicit: 0x41C380)

	bool					Connected();											// 0x41C4E0
	bool					GetData(const std::string& theSrcFile, const std::string& theDataType, void** thePtr, int* theSize); // 0x41C4F0
	void					FreeGetData(void* thePtr);								// 0x41C700
	void					SetFileDeps(const std::string& theSrcFile, const std::string& theDataType, const std::string& theFileDeps); // 0x41C7E0
	void*					AllocSetData(const std::string& theSrcFile, const std::string& theDataType, int theSize); // 0x41C850
	void					FreeSetData(void* thePtr);								// 0x41C9B0
	bool					SetData(void* thePtr);									// 0x41CAA0
};

extern SexyCache gSexyCache;

// Serialisation helpers for cache blocks: copy, then advance the cursor
void						SMemR(void*& theSrc, void* theDest, int theSize);		// 0x41CB90
void						SMemRStr(void*& theSrc, std::string& theString);		// 0x41CBA0 (int length, then the characters)
void						SMemW(void*& theDest, const void* theSrc, int theSize);	// 0x41CB40
void						SMemWStr(void*& theDest, const std::string& theString);	// 0x41CB50

}

#endif //__SEXYCACHE_H__
