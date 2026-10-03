#ifndef __INTERNETMGR_H__
#define __INTERNETMGR_H__

namespace Sexy
{
	class HTTPTransfer;
	#define MAX_HTTP_TRANSFERS 4

	enum
	{
		AD_TYPE_NONE	= 0,
		AD_TYPE_POPUP	= 1,	// "popup <width> <height> | <files...> | <isNew> <moreActive>"
		AD_TYPE_MORE	= 2		// "more <arg1> <group> | <files...> | <isNew> <moreActive>"
	};

	// One ad line of adlist.txt or of the update server's reply (0x54 bytes). Its copy
	// constructor (0x42E530) and destructor (0x40BC20) are compiler-generated.
	struct AdInfo
	{
		int						mType;				// AD_TYPE_*
		int						mWidth;				// popup client size; the constructor leaves both unset
		int						mHeight;
		std::list<SexyString>	mFiles;				// front() is the page TryShowAd opens
		bool					mDownloaded;		// every file in mFiles exists (CheckFiles)
		bool					mIsNew;				// popup not shown yet (saved)
		bool					mMoreActive;		// "more" ad picked by UpdateMoreAds (saved)
		bool					mNewThisSession;	// first listed by this session's update check (not saved)
		SexyString				mMoreArg1;
		SexyString				mMoreGroup;			// at most one active "more" ad per group (case-insensitive)

		AdInfo()
		{
			mType = AD_TYPE_NONE;
			mMoreActive = false;
			mNewThisSession = false;
			mDownloaded = false;
			mIsNew = false;
		}

		void					GetLine(SexyString& theLine);
		void					CheckFiles();
	};

	typedef std::list<AdInfo> AdInfoList;

	class InternetManager
	{
	public:
		int						mMaxMoreAds;
		HTTPTransfer			mUpdateTransfer;
		HTTPTransfer			mHTTPTransfers[MAX_HTTP_TRANSFERS];
		bool					mUpdateChecked;		// never set to true, so every CheckForUpdates call re-parses the reply
		bool					mUpToDate;
		bool					mAdsLoading;
		SexyString				mUpdateURL;
		AdInfoList				mAdList;
		AdInfoList				mOldAdList;
		std::list<SexyString>	mAdUrlList;			// ad files still to download

	public:
		InternetManager();
		virtual void			ParseAdLine(const SexyString& theLine, bool fromUpdateCheck);
		virtual ~InternetManager();

		void					Update();
		bool					CheckForUpdates();
		bool					IsUpToDate();
		void					StartAdLoading();
		void					StartUpdateCheck(const SexyString& theURL);

		SexyString				GetUpdateURL();
		int						GetUpdateResultCode();

		void					ReadAdList();
		bool					LoadAdList();
		bool					SaveAdList();
		void					UpdateMoreAds();
		static AdInfo*			FindAdInfo(AdInfoList& theList, AdInfo& theAdInfo);
		void					CheckAdFiles();
		bool					HasNewAds();
		void					TryShowAd();
	};
}

#endif