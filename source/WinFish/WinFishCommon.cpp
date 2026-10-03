#include <SexyAppFramework/DialogButton.h>
#include <SexyAppFramework/MemoryImage.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/WidgetManager.h>

#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "Res.h"

#include <time.h>

using namespace Sexy;

DialogButton* Sexy::MakeDialogButton(int theId, ButtonListener* theListener, const SexyString& theText, Font* theFont)
{
	DialogButton* aDiaBtn = new DialogButton(IMAGE_DIALOGBUTTON, theId, theListener);
	if (!theFont)
		theFont = FONT_JUNGLEFEVER10OUTLINE;
	aDiaBtn->SetFont(theFont);
	aDiaBtn->mLabel = theText;
	aDiaBtn->mHeight = aDiaBtn->mComponentImage->mHeight;
	DefaultDialogButtonSettings(aDiaBtn);
	return aDiaBtn;
}

DialogButton* Sexy::MakeDialogButton2(int theId, ButtonListener* theListener, const SexyString& theText, Image* theComponentImage)
{
	DialogButton* aDiaBtn = new DialogButton(theComponentImage, theId, theListener);
	aDiaBtn->SetFont(FONT_JUNGLEFEVER10OUTLINE);
	aDiaBtn->mLabel = theText;
	aDiaBtn->SetColor(0, Color(0xff, 0xf0, 0));
	aDiaBtn->SetColor(1, Color(0xff, 0xff, 0xff));
	aDiaBtn->mDoFinger = true;
	aDiaBtn->mHeight = theComponentImage->mHeight;
	DefaultDialogButtonSettings(aDiaBtn);
	return aDiaBtn;
}

void Sexy::DefaultDialogSettings(Dialog* theDialog)
{
	theDialog->SetButtonFont(FONT_JUNGLEFEVER12OUTLINE);
	theDialog->SetHeaderFont(FONT_JUNGLEFEVER15OUTLINE);
	theDialog->SetLinesFont(FONT_JUNGLEFEVER10OUTLINE);
	theDialog->mContentInsets = Insets(0x24, 0xf, 0x24, 0x24);
	theDialog->SetColor(Dialog::COLOR_HEADER, Color(0xff, 200, 0));
	theDialog->SetColor(Dialog::COLOR_LINES, Color(0xff, 0xff, 0xff));
	theDialog->SetColor(Dialog::COLOR_BUTTON_TEXT, Color(0xff, 0xff, 100));
	theDialog->mSpaceAfterHeader = 15;
	if (theDialog->mYesButton)
		DefaultDialogButtonSettings(theDialog->mYesButton);
	if (theDialog->mNoButton)
		DefaultDialogButtonSettings(theDialog->mNoButton);
}

void Sexy::DefaultDialogButtonSettings(DialogButton* theDialogButton)
{
	theDialogButton->mTranslateX = 1;
	theDialogButton->mTranslateY = 1;
	theDialogButton->mHasTransparencies = true;
	theDialogButton->mHasAlpha = true;
	int anImgWdth = theDialogButton->mComponentImage->mWidth / 3;
	int anImgHght = theDialogButton->mComponentImage->mHeight;
	theDialogButton->mNormalRect.mX = 0;
	theDialogButton->mNormalRect.mY = 0;
	theDialogButton->mOverRect.mX = anImgWdth;
	theDialogButton->mOverRect.mY = 0;
	theDialogButton->mDownRect.mX = anImgWdth*2;
	theDialogButton->mDownRect.mY = 0;
	theDialogButton->mNormalRect.mWidth = anImgWdth;
	theDialogButton->mOverRect.mWidth = anImgWdth;
	theDialogButton->mDownRect.mWidth = anImgWdth;
	theDialogButton->mNormalRect.mHeight = anImgHght;
	theDialogButton->mOverRect.mHeight = anImgHght;
	theDialogButton->mDownRect.mHeight = anImgHght;
	theDialogButton->mTextOffsetX = -2;
	theDialogButton->mTextOffsetY = 0;
}

EditWidget* Sexy::MakeEditWidget(int theId, EditListener* theEditListener)
{
	EditWidget* anEditWidget = new EditWidget(theId, theEditListener);
	anEditWidget->SetFont(FONT_LIDDIE12, 0);
	// Static data in the original (0x5DFE68), not built on the stack
	static int anEditWidgetColors[][4] =
	{ {0, 0, 0, 0},
	{0, 0, 0, 0},
	{240, 240, 255, 255},
	{255, 255, 255, 255},
	{0, 0, 0, 255} };
	anEditWidget->SetColors(anEditWidgetColors, EditWidget::NUM_COLORS);
	anEditWidget->mBlinkDelay = 14;
	return anEditWidget;
}

Checkbox* Sexy::MakeCheckbox(int theId, CheckboxListener* theCheckboxListener, bool theChecked)
{
	Checkbox* aCB = new Checkbox(IMAGE_UNCHECKED, IMAGE_CHECKED, theId, theCheckboxListener);
	aCB->mChecked = theChecked;
	aCB->mHasTransparencies = true;
	aCB->mHasAlpha = true;
	return aCB;
}

HyperlinkWidget* Sexy::MakeHyperlinkWidget(int theId, ButtonListener* theButtonListener, const char* theLabel)
{
    HyperlinkWidget* aWidget = new HyperlinkWidget(theId, theButtonListener);
    aWidget->mLabel = theLabel;
    aWidget->SetFont(FONT_JUNGLEFEVER10OUTLINE);
    aWidget->mColor = Color(0x808080);
    aWidget->mOverColor = Color(0xffffff);
    aWidget->mUnderlineSize = 0;
    int aStrWdth = aWidget->mFont->StringWidth(aWidget->mLabel);
    aWidget->mWidth = aStrWdth + 20;
    int aStrHght = aWidget->mFont->GetHeight();
    aWidget->mHeight = aStrHght;
    return aWidget;
}

void Sexy::DrawStringWithOutline(Graphics* g, const SexyString& theLine, int theX, int theY, Font* theFont, ulong theOutlineColor)
{
	Font* aPrevFont = g->GetFont();
	Color aPrevColor = g->GetColor();
	g->SetFont(theFont);
	g->SetColor(Color(theOutlineColor));
	g->DrawString(theLine, theX, theY);
	g->SetFont(aPrevFont);
	g->SetColor(aPrevColor);
	g->DrawString(theLine, theX, theY);
}

void Sexy::DrawWaves(Graphics* g, int theY, int theUpdateCnt)
{
    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
    int aCel = IMAGE_WAVE->GetAnimCel(theUpdateCnt);
    g->DrawImageCel(IMAGE_WAVE, 0, theY, aCel);
    g->DrawImageCel(IMAGE_WAVE, 160, theY, aCel);
    g->DrawImageCel(IMAGE_WAVE, 320, theY, aCel);
    g->DrawImageCel(IMAGE_WAVE, 480, theY, aCel);
    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
}

void Sexy::DrawHorzCrease(Graphics* g, Image* theImage, int theX, int theY, int theWidth)
{
    for (int i = 0; i < theWidth; i += theImage->mWidth)
    {
        int aSrcWdth = theWidth - i;
        if (theImage->mWidth <= aSrcWdth)
            aSrcWdth = theImage->mWidth;
        g->DrawImage(theImage, theX, theY, Rect(0, 0, aSrcWdth, theImage->mHeight));
        theX += aSrcWdth;
    }
}

void Sexy::DrawVertCrease(Graphics* g, Image* theImage, int theX, int theY, int theHeight)
{
    for (int i = 0; i < theHeight; i += theImage->mHeight)
    {
        int aSrcHeight = theHeight - i;
        if (theImage->mHeight <= aSrcHeight)
            aSrcHeight = theImage->mHeight;
        g->DrawImage(theImage, theX, theY, Rect(0, 0, theImage->mWidth, aSrcHeight));
        theY += aSrcHeight;
    }
}

void Sexy::DrawCheckboxString(Graphics* g, const SexyString& theLine, Checkbox* theCB, const char* theExtensionString)
{
    // The offsets are doubles in the original
    if (theExtensionString != nullptr)
    {
        g->DrawString(theLine, (int)(theCB->mX - g->mTransX + 43.0), (int)(theCB->mY - g->mTransY + 15.0));

        SexyString aStr(theExtensionString);
        g->DrawString(aStr, (int)(theCB->mX - g->mTransX + 43.0), (int)(theCB->mY - g->mTransY + 30.0));
    }
    else
    {
        g->DrawString(theLine, (int)(theCB->mX - g->mTransX + 43.0), (int)(theCB->mY - g->mTransY + 24.0));
    }
}

int Sexy::InterpolateInt(int startValue, int endValue, int currentProgress, int totalDuration, bool swap)
{
    int start;
    int end;

    if (swap)
    {
        start = endValue;
        end = startValue;
    }
    else
    {
        start = startValue;
        end = endValue;
    }

    if (currentProgress <= 0)
        return start;

    if (currentProgress >= totalDuration)
        return end;

    // 32-bit arithmetic, like the original
    return (currentProgress * end + start * (totalDuration - currentProgress)) / totalDuration;
}

// A cdecl free function in the original (0x500940): the alien attractor's base, top and glow
void Sexy::DrawAlienAttractorMisc(Graphics* g, int theX, int theY, int theTime, int unk)
{
    if (unk != 0)
        theTime = 0;
    g->DrawImage(IMAGE_AA_BOTTOM, theX, theY);

    int aNewX = theX + 25;
    int aNewY = theY - 26;
    int aNewX2 = theX - 11;
    int aNewY2 = theY - 50;
    if (theTime >= 300)
    {
        g->DrawImage(IMAGE_AA_TOP1, aNewX, aNewY);
    }
    else if (theTime >= 40)
    {
        // The pulsing top is drawn over the plain one
        g->DrawImage(IMAGE_AA_TOP1, aNewX, aNewY);
        int anAlphaMod = (300 - theTime) % 40;
        if (anAlphaMod > 20)
            anAlphaMod = 40 - anAlphaMod;
        g->SetColorizeImages(true);
        g->SetColor(Color(255, 255, 255, (anAlphaMod * 255) / 20));
        g->DrawImage(IMAGE_AA_TOP2, aNewX, aNewY);
        g->SetColorizeImages(false);
    }
    else if (theTime >= 30)
    {
        g->DrawImage(IMAGE_AA_TOP2, aNewX, aNewY);
        g->SetColorizeImages(true);
        g->SetColor(Color(255, 255, 255, ((30 - theTime) * 255) / 20 + 255));
        g->DrawImage(IMAGE_AA_GLOW, aNewX2, aNewY2);
        g->SetColorizeImages(false);
    }
    else
    {
        g->DrawImage(IMAGE_AA_GLOW, aNewX2, aNewY2);
    }
}

// A cdecl free function in the original (0x500C90): the surface waves along the top of the tank
void Sexy::DrawTankWaves(Graphics* g, int theY, int theTime)
{
    g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
    int aCel = IMAGE_WAVE->GetAnimCel(theTime);
    Rect aRect = IMAGE_WAVESIDE->GetCelRect(aCel);
    g->DrawImageMirror(IMAGE_WAVESIDE, 0, theY, aRect, false);
    g->DrawImageCel(IMAGE_WAVE, 160, theY, aCel);
    g->DrawImageCel(IMAGE_WAVE, 320, theY, aCel);
    g->DrawImageMirror(IMAGE_WAVESIDE, 480, theY, aRect, true);
    g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
}

// A cdecl free function in the original (0x500FB0), used by Board to take its widgets down
void Sexy::RemoveWidgetHelper(Widget* theWidget)
{
    if (theWidget)
    {
        gSexyApp->mWidgetManager->RemoveWidget(theWidget);
        gSexyApp->SafeDeleteWidget(theWidget);
    }
}

// A separate function in the original (0x500E10)
const char* Sexy::GetOrdinalSuffix(int theNum)
{
    if (theNum / 10 % 10 == 1)
        return "th";

    switch (theNum % 10)
    {
    case 1:
        return "st";
    case 2:
        return "nd";
    case 3:
        return "rd";
    default:
        return "th";
    }
}

// The original returns a pointer into a static string, as here
const char* Sexy::GetCyraxEndGameString(int theCyraxCnt)
{
    static SexyString aString;
    if (theCyraxCnt <= 1)
        return "FINAL BOSS";

    aString = StrFormat("%d%s FINAL BOSS", theCyraxCnt, GetOrdinalSuffix(theCyraxCnt));
    return aString.c_str();
}

SexyString Sexy::GetPlayTimeString(int theTimeInSec)
{
    int aMins = theTimeInSec / 60;
    int anHours = aMins / 60;
    if (anHours > 0)
        return StrFormat("%2d:%02d:%02d", anHours, aMins % 60, theTimeInSec % 60);
    return StrFormat("%d:%02d", aMins, theTimeInSec % 60);
}

// Original 0x5047C0: std::random_shuffle's loop (std::_Random_shuffle) for the song lists, with
// SongShuffleRand::operator() inlined. Each element after the first is swapped with a random one
// at or before it.
static void RandomShuffleFishSongs(FishSongData** theFirst, FishSongData** theLast, SongShuffleRand& theRand)
{
    int anIndex = 2;
    for (FishSongData** aNext = theFirst; ++aNext != theLast; ++anIndex)
        std::iter_swap(aNext, theFirst + theRand(anIndex) % anIndex);
}

// Original 0x5049A0: the std::random_shuffle(first, last, rand) instantiation GetSongData calls
// (its checked-iterator wrapper)
void Sexy::KnuthShuffleFishSongs(std::vector<FishSongData*>::iterator theFirst, std::vector<FishSongData*>::iterator theLast, SongShuffleRand& theRand)
{
    if (theFirst != theLast)
        RandomShuffleFishSongs(&*theFirst, &*theFirst + (theLast - theFirst), theRand);
}

FishSongData* Sexy::GetSongData(int theSpecId)
{
    if (gUnkBool05)
        return nullptr;

    if (theSpecId == 4)
        return gKilgoreSongDataPtr;

    if (gTestSongDataPtr != nullptr)
        return gTestSongDataPtr;

    std::vector<FishSongData*>* aSongList;
    int* aListId;

    if (theSpecId == 1)
    {
        aSongList = &gLudwigSongs;
        aListId = &gLudwigSongId;
    }
    else if (theSpecId == 5)
    {
        aSongList = &gSantaSongs;
        aListId = &gSantaSongId;
    }
    else
    {
        aSongList = &gSongsVector2;
        aListId = &gSongs2Id;
    }

    if (aSongList->empty())
        return nullptr;

    if (*aListId >= (int)aSongList->size())
        *aListId = 0;

    if (*aListId == 0)
    {
        SongShuffleRand aRand;
        KnuthShuffleFishSongs(aSongList->begin(), aSongList->end(), aRand);
    }

    FishSongData* aChosenSong = (*aSongList)[*aListId];

    (*aListId)++;

    SexyString aPropVal;
    if (gUnkInt04 < 5)
        gUnkInt04++;

    if (aChosenSong->GetProperty("short", &aPropVal))
    {
        bool unkbool = true;
        if (!aChosenSong->GetProperty("beethoven", nullptr) || theSpecId == 1)
            unkbool = false;

        if (!unkbool && gUnkInt04 == 5)
        {
            std::map<SexyString, FishSongData*, Sexy::StringLessNoCase>::iterator it = gLongSongsMap.find(aPropVal);
            if (it != gLongSongsMap.end())
            {
                gUnkInt04 = 0;
                return it->second;
            }
        }
    }

    return aChosenSong;
}

long long Sexy::GetTodayStartSeconds() // Returns the start of todays day in sec from 1970 to today
{
    __time64_t currentTime_t = _time64(nullptr);

    if (currentTime_t < 0)
        currentTime_t = 0;

    tm* localTimeInfo = _localtime64(&currentTime_t);

    // Each part of the time of day is taken off the 64-bit time on its own
    currentTime_t -= localTimeInfo->tm_hour * 3600;
    currentTime_t -= localTimeInfo->tm_min * 60;

    return (currentTime_t - localTimeInfo->tm_sec) / 86400 + gUnkInt01;
}

void Sexy::ButtonHoleHelper(MemoryImage* theImage, MemoryImage* theHoleImage, int theX, int theY)
{
    Rect anIntersectRect = Rect(0, 0, theImage->mWidth, theImage->mHeight)
        .Intersection(Rect(theX, theY, theHoleImage->mWidth, theHoleImage->mHeight));

    if (anIntersectRect.mWidth <= 0 || anIntersectRect.mHeight <= 0)
        return;

    ulong* aHoleImgBits = theHoleImage->GetBits();
    ulong* aHolePos = aHoleImgBits + (anIntersectRect.mY - theY) * theHoleImage->mWidth - theX + anIntersectRect.mX;

    ulong* anImgBits = theImage->GetBits();
    ulong* anImgPos = anImgBits + theImage->mWidth * anIntersectRect.mY + anIntersectRect.mX;

    for (int y = 0; y < anIntersectRect.mHeight; y++)
    {
        ulong* aTemp1 = anImgPos;
        ulong* aTemp2 = aHolePos;

        for (int x = 0; x < anIntersectRect.mWidth; x++)
        {
            ulong aBitVal = *aTemp2;
            aTemp2++;
            if ((aBitVal & 0xff000000) != 0)
                *aTemp1 = 0;
            aTemp1++;
        }

        anImgPos += theImage->mWidth;
        aHolePos += theHoleImage->mWidth;
    }
}

// Copies theImage (clipped to the mask placed at theX, theY) and takes its alpha from the mask,
// reading the mask bits linearly over the clipped size
Image* Sexy::LoadMaskImage(Image* theImage, Image* theImageMask, int theX, int theY)
{
    Rect aMaskRect(theX, theY, theImageMask->GetWidth(), theImageMask->GetHeight());
    Rect anImageRect(0, 0, theImage->GetWidth(), theImage->GetHeight());
    Rect aClipRect = aMaskRect.Intersection(anImageRect);

    if (aClipRect.mWidth <= 0 || aClipRect.mHeight <= 0)
        return NULL;

    MemoryImage* aNewImage = new MemoryImage(gSexyApp);
    aNewImage->Create(aClipRect.mWidth, aClipRect.mHeight);

    Graphics g(aNewImage);
    g.DrawImage(theImage, -aClipRect.mX, -aClipRect.mY);

    int aCount = aClipRect.mWidth * aClipRect.mHeight;
    ulong* aMaskBits = ((MemoryImage*)theImageMask)->GetBits();
    ulong* aNewBits = aNewImage->GetBits();
    for (int i = 0; i < aCount; i++)
    {
        *aNewBits = (*aMaskBits++ & 0xFF000000) | (*aNewBits & 0x00FFFFFF);
        aNewBits++;
    }

    return aNewImage;
}

// The edit box frame drawn around a dialog's EditWidget (in the dialog's coordinates)
void Sexy::DrawEditWidgetBox(Graphics* g, EditWidget* theWidget)
{
    g->DrawImageBox(Rect((int)((double)(theWidget->mX - 8) - g->mTransX), (int)((double)(theWidget->mY - 4) - g->mTransY),
        theWidget->mWidth + 16, theWidget->mHeight + 8), IMAGE_EDITBOX);
}

bool Sexy::CanAlienChaseAnyFish()
{
    WinFishApp* anApp = (WinFishApp*)gSexyApp;
    if (anApp->mRelaxMode)
    {
        if (anApp->mUpdateCount - gUnkInt03 >= 360)
        {
            Board* aBoard = anApp->mBoard;
            int aFishCnt = aBoard->mFishList->size();
            int anOscarCnt = aBoard->mOscarList->size();
            int anUltraCnt = aBoard->mUltraList->size();
            int aBreederCnt = aBoard->mBreederList->size();
            int aPentaCnt = aBoard->mPentaList->size();
            int aGrubberCnt = aBoard->mGrubberList->size();
            int aGekkoCnt = aBoard->mGekkoList->size();

            int allCnt = aFishCnt + anOscarCnt + anUltraCnt + aBreederCnt + aPentaCnt + aGrubberCnt + aGekkoCnt;
            if (allCnt > 1)
                return true;

            gUnkInt03 = anApp->mUpdateCount - 500;
        }
        return false;
    }
    return true;
}

const char* Sexy::GetPetName(int thePetId)
{
    switch (thePetId)
    {
    case PET_STINKY:
        return "Stinky";
    case PET_NIKO:
        return "Niko";
    case PET_ITCHY:
        return "Itchy";
    case PET_PREGO:
        return "Prego";
    case PET_ZORF:
        return "Zorf";
    case PET_CLYDE:
        return "Clyde";
    case PET_VERT:
        return "Vert";
    case PET_RUFUS:
        return "Rufus";
    case PET_MERYL:
        return "Meryl";
    case PET_WADSWORTH:
        return "Wadsworth";
    case PET_SEYMOUR:
        return "Seymour";
    case PET_SHRAPNEL:
        return "Shrapnel";
    case PET_GUMBO:
        return "Gumbo";
    case PET_BLIP:
        return "Blip";
    case PET_RHUBARB:
        return "Rhubarb";
    case PET_NIMBUS:
        return "Nimbus";
    case PET_AMP:
        return "Amp";
    case PET_GASH:
        return "Gash";
    case PET_ANGIE:
        return "Angie";
    case PET_PRESTO:
        return "Presto";
    case PET_BRINKLEY:
        return "Brinkley";
    case PET_NOSTRADAMUS:
        return "Nostradamus";
    case PET_STANLEY:
        return "Stanley";
    case PET_WALTER:
        return "Walter";
    default:
        return "";
    }
}

int Sexy::GetLevelIndex(int theTank, int theLevel)
{
    if ((uint)(theTank - 1) > 4 || (uint)(theLevel - 1) > 4 || (theTank == 5 && theLevel != 1))
        return -1;
    return theTank * 5 + theLevel - 6;
}
