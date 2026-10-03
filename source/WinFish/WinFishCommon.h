#ifndef __WINFISHAPPCOMMON_H__
#define __WINFISHAPPCOMMON_H__

#include "SexyAppFramework/Dialog.h"
#include "SexyAppFramework/EditWidget.h"
#include "SexyAppFramework/Checkbox.h"
#include "SexyAppFramework/HyperlinkWidget.h"
#include "SexyAppFramework/MemoryImage.h"
#include "FishSongMgr.h"
#include "GameObject.h"

namespace Sexy
{
	// The number generator GetSongData hands to the song shuffle (std::random_shuffle's generator);
	// its operator() is inlined into 0x5047C0
	class SongShuffleRand
	{
	public:
		int				operator()(int theRange) { return Rand() % theRange; }
	};

	DialogButton*		MakeDialogButton(int theId, ButtonListener* theListener, const SexyString& theText, Font* theFont);
	DialogButton*		MakeDialogButton2(int theId, ButtonListener* theListener, const SexyString& theText, Image* theComponentImage);
	void				DefaultDialogSettings(Dialog* theDialog);
	void				DefaultDialogButtonSettings(DialogButton* theDialogButton);
	EditWidget*			MakeEditWidget(int theId, EditListener* theEditListener);
	Checkbox*			MakeCheckbox(int theId, CheckboxListener* theCheckboxListener, bool theChecked);
	HyperlinkWidget*	MakeHyperlinkWidget(int theId, ButtonListener* theButtonListener, const char* theLabel);

	void				DrawStringWithOutline(Graphics* g, const SexyString& theLine, int theX, int theY, Font* theFont, ulong theOutlineColor);
	void				DrawWaves(Graphics* g, int theY, int theUpdateCnt);
	void				DrawHorzCrease(Graphics* g, Image* theImage,int theX, int theY, int theWidth);
	void				DrawVertCrease(Graphics* g, Image* theImage,int theX, int theY, int theHeight);
	void				DrawCheckboxString(Graphics* g, const SexyString& theLine, Checkbox* theCB, const char* theExtensionString);

	int					InterpolateInt(int startValue, int endValue, int currentProgress, int totalDuration, bool swap);

	void				DrawAlienAttractorMisc(Graphics* g, int theX, int theY, int theTime, int unk);
	void				DrawTankWaves(Graphics* g, int theY, int theTime);
	void				RemoveWidgetHelper(Widget* theWidget);
	const char*			GetOrdinalSuffix(int theNum);
	const char*			GetCyraxEndGameString(int theUnk01);
	SexyString			GetPlayTimeString(int theTimeInSec);

	void				KnuthShuffleFishSongs(std::vector<FishSongData*>::iterator theFirst, std::vector<FishSongData*>::iterator theLast, SongShuffleRand& theRand);
	FishSongData*		GetSongData(int theSpecId);

	long long			GetTodayStartSeconds();

	void				ButtonHoleHelper(MemoryImage* theImage, MemoryImage* theHoleImage, int theX, int theY);
	Image*				LoadMaskImage(Image* theImage, Image* theImageMask, int theX, int theY);
	void				DrawEditWidgetBox(Graphics* g, EditWidget* theWidget);
	bool				CanAlienChaseAnyFish();
	const char*			GetPetName(int thePetId);

	int					GetLevelIndex(int theTank, int theLevel);
}

#endif