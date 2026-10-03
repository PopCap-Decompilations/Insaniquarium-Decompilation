#include <SexyAppFramework/WidgetManager.h>
#include <SexyAppFramework/Font.h>
#include <SexyAppFramework/DialogButton.h>

#include "SimFishScreen.h"
#include "WinFishApp.h"
#include "WinFishCommon.h"
#include "Board.h"
#include "ProfileMgr.h"
#include "BubbleMgr.h"
#include "Fish.h"
#include "Food.h"
#include "FishButtonWidget.h"
#include "Res.h"

#include <time.h>

// The original's tables are arrays of char pointers (0x5E0430, 0x5E0414, 0x5DFEE8), read as 4-byte entries
const char* HOMETOWNS[] = {
	"Virtual Tank",
	"Abilene, Texas",
	"Adak, Alaska",
	"Canton, Ohio",
	"Alamogordo, N.M.",
	"Alamosa, Colo.",
	"Albany, N.Y.",
	"Albuquerque, N.M.",
	"Alexandria, La.",
	"Allentown, Pa.",
	"Alpena, Mich.",
	"Altus, Okla",
	"Amarillo, Texas",
	"Anchorage, Alaska",
	"Apalachicola, Fla.",
	"Asheville, N.C.",
	"Aspen, Colo.",
	"Astoria, Ore.",
	"Athens, Ga.",
	"Atlanta, Ga.",
	"Augusta, Ga.",
	"Austin, Texas",
	"Bakersfield, Calif.",
	"Baltimore, Md.",
	"Bar Harbor, Maine",
	"Barrow, Alaska",
	"Baton Rouge, La.",
	"Beckley, W.Va.",
	"Beeville, Texas",
	"Billings, Mont.",
	"Biloxi, Miss.",
	"Binghamton, N.Y.",
	"Birmingham, Ala.",
	"Bishop, Calif.",
	"Bismarck, N.D.",
	"Boston, Mass.",
	"Bridgeport, Conn.",
	"Brownsville, Texas",
	"Brunswick, Maine",
	"Buffalo, N.Y.",
	"Burlington, Vt.",
	"Cape Hatteras, N.C.",
	"Caribou, Maine",
	"Casper, Wyo.",
	"Charleston, S.C.",
	"Charleston, W.Va.",
	"Charlotte, N.C.",
	"Chattanooga, Tenn.",
	"Cheyenne, Wyo.",
	"Chicago, Ill.",
	"Cincinnati, Ohio",
	"Clayton, N.M.",
	"Cleveland, Ohio",
	"Clovis, N.M.",
	"Cocoa Beach, Fla.",
	"Colorado Springs, Colo.",
	"Columbia, Mo.",
	"Columbia, S.C.",
	"Columbus, Ga.",
	"Columbus, Miss.",
	"Columbus, Ohio",
	"Concord, N.H.",
	"Concordia, Kan.",
	"Corpus Christi, Texas",
	"Covington, Ky.",
	"Crested Butte, Colo.",
	"Daggett, Calif.",
	"Dallas, Texas",
	"Davenport, Iowa",
	"Dayton, Ohio",
	"Daytona Beach, Fla.",
	"Denver, Colo.",
	"Del Rio, Texas",
	"Des Moines, Iowa",
	"Detroit, Mich.",
	"Dodge City, Kan.",
	"Dover, Del.",
	"Dubuque, Iowa",
	"Duluth, Minn.",
	"Elkins, W.Va.",
	"Elko, Nev.",
	"El Paso, Texas",
	"Ely, Nev.",
	"Enid, Okla.",
	"Erie, Pa.",
	"Eugene, Ore.",
	"Eureka, Calif.",
	"Evansville, Ind.",
	"Fairbanks, Alaska",
	"Fallon, Nev.",
	"Fargo, N.D.",
	"Fayetteville, N.C.",
	"Flagstaff, Ariz.",
	"Flint, Mich.",
	"Fort Myers, Fla.",
	"Fort Smith, Ark.",
	"Fort Wayne, Ind.",
	"Fresno, Calif.",
	"Gainesville, Fla.",
	"Galveston, Texas",
	"Gatlinburg, Tenn.",
	"Gila Bend, Ariz.",
	"Glasgow, Mont.",
	"Glenview, Ill.",
	"Goldsboro, N.C.",
	"Goodland, Kan.",
	"Grand Forks, N.D.",
	"Grand Island, Neb.",
	"Grand Junction, Colo.",
	"Grand Rapids, Mich.",
	"Great Falls, Mont.",
	"Green Bay, Wis.",
	"Greensboro, N.C.",
	"Greenville, S.C.",
	"Gulfport, Miss.",
	"Harrisburg, Pa.",
	"Hartford, Conn.",
	"Havre, Mont.",
	"Helena, Mont.",
	"Hilo, Hawaii",
	"Hondo, Texas",
	"Honolulu, Hawaii",
	"Houghton Lake, Mich.",
	"Houston, Texas",
	"Huntington, W.Va.",
	"Huntsville, Ala.",
	"Huron, S.D.",
	"Indianapolis, Ind.",
	"Long Island, N.Y.",
	"Jackson, Ky.",
	"Jackson, Miss.",
	"Jacksonville, Fla.",
	"Jacksonville, N.C.",
	"Juneau, Alaska",
	"Kahului, Hawaii",
	"Kalispell, Mont.",
	"Kansas City, Kan.",
	"Kansas City, Mo.",
	"Key West, Fla.",
	"Killeen, Texas",
	"Kingsville, Texas",
	"Klamath Falls, Ore.",
	"Knoxville, Tenn.",
	"La Crosse, Wis.",
	"Lake Charles, La.",
	"Lake Havasu, Ariz.",
	"Lakehurst, N.J.",
	"Lake Tahoe",
	"Lander, Wyo.",
	"Lansing, Mich.",
	"Las Vegas, Nev.",
	"Lawton, Okla.",
	"Lemoore, Calif.",
	"Rehoboth Beach, Del.",
	"Lewiston, Idaho",
	"Lexington, Ky.",
	"Lexington Park, Md.",
	"Lihue, Hawaii",
	"Lincoln, Neb.",
	"Little Rock, Ark.",
	"Long Beach, Calif.",
	"Los Angeles, Calif.",
	"Louisville, Ky.",
	"Lubbock, Texas",
	"Lynchburg, Va.",
	"Macon, Ga.",
	"Madison, Wis.",
	"Mansfield, Ohio",
	"Marquette, Mich.",
	"Mascoutah, Ill.",
	"Medford, Ore.",
	"Melbourne, Fla.",
	"Memphis, Tenn.",
	"Meridian, Miss.",
	"Miami, Fla.",
	"Miles City, Mont.",
	"Milford, Utah",
	"Milwaukee, Wis.",
	"St. Paul, Minn.",
	"Minneapolis",
	"Minot, N.D.",
	"Missoula, Mont.",
	"Mobile, Ala.",
	"Moline, Ill.",
	"Monterey, Calif.",
	"Montgomery, Ala.",
	"Moses Lake, Wash.",
	"Muskegon, Mich.",
	"Myrtle Beach, S.C.",
	"Nantucket, Mass.",
	"Nashville, Tenn.",
	"New Bern, N.C.",
	"New Orleans, La.",
	"Newport, R.I.",
	"New York, N.Y.",
	"Nome, Alaska",
	"Norfolk, Neb.",
	"Norfolk, Va.",
	"North Platte, Neb.",
	"Oakland, Calif.",
	"Ocean City, Md.",
	"Oceanside, Calif.",
	"Ogden, Utah",
	"Oklahoma City, Okla.",
	"Orlando, Fla.",
	"Olympia, Wash.",
	"Omaha, Neb.",
	"Ontario, Calif.",
	"Oxnard, Calif.",
	"Paducah, Ky.",
	"Palm Springs, Calif.",
	"Panama City, Fla.",
	"Parkersburg, W.Va.",
	"Pendleton, Ore.",
	"Pensacola, Fla.",
	"Peoria, Ill.",
	"Peru, Ind.",
	"Philadelphia, Pa.",
	"Phoenix, Ariz.",
	"Pittsburgh, Pa.",
	"Pocatello, Idaho",
	"Port Angeles, Wash.",
	"Portland, Maine",
	"Portland, Ore.",
	"Portsmouth, N.H.",
	"Providence, R.I.",
	"Cape Cod, Mass.",
	"Pueblo, Colo.",
	"Quantico, Va.",
	"Quillayute, Wash.",
	"Raleigh, N.C.",
	"Rapid City, S.D.",
	"Red Bluff, Calif.",
	"Redding, Calif.",
	"Richmond, Va.",
	"Ridgecrest, Calif.",
	"Roanoke, Va.",
	"Rochester, Minn.",
	"Rochester, N.Y.",
	"Rockford, Ill.",
	"Utica, N.Y.",
	"Roswell, N.M.",
	"Sacramento, Calif.",
	"St. Cloud, Minn.",
	"St. George, Utah",
	"St. Joseph, Mo.",
	"St. Louis, Mo.",
	"St. Robert, Mo.",
	"Salt Lake City, Utah",
	"Salem, Ore.",
	"San Angelo, Texas",
	"San Antonio, Texas",
	"San Diego, Calif.",
	"San Francisco, Calif.",
	"Santa Barbara, Calif.",
	"Santa Maria, Calif.",
	"Savannah, Ga.",
	"Scottsbluff, Neb.",
	"Seattle, Wash.",
	"Tacoma, Wash.",
	"Sheridan, Wyo.",
	"Shreveport, La.",
	"Sioux City, Iowa",
	"Sioux Falls, S.D.",
	"South Bend, Ind.",
	"Spokane, Wash.",
	"Springfield, Ill.",
	"Springfield, Mass.",
	"Springfield, Mo.",
	"Sterling, Va.",
	"Stockbridge, Mass.",
	"Stockton, Calif.",
	"Sumter, S.C.",
	"Sun Valley, Idaho",
	"Syracuse, N.Y.",
	"Tallahassee, Fla.",
	"Tampa, Fla.",
	"St. Petersburg, Fla.",
	"Taos, N.M.",
	"Toledo, Ohio",
	"Topeka, Kan.",
	"Tucson, Ariz.",
	"Tupelo, Miss.",
	"Valdosta, Ga.",
	"Valentine, Neb.",
	"Vero Beach, Fla.",
	"Victoria, Texas",
	"Virginia Beach, Va.",
	"Walla Walla, Wash.",
	"Waco, Texas",
	"Washington, D.C.",
	"Waterloo, Iowa",
	"Watertown, N.Y.",
	"Wendover, Utah",
	"West Palm Beach, Fla.",
	"Whidbey Island, Wash.",
	"Wichita, Kan.",
	"Wichita Falls, Texas",
	"Williamsport, Pa.",
	"Williston, N.D.",
	"Scranton, Pa.",
	"Wilmington, Del.",
	"Wilmington, N.C.",
	"Winnemucca, Nev.",
	"Winslow, Ariz.",
	"Worcester, Mass.",
	"Yakima, Wash.",
	"Youngstown, Ohio",
	"Yuma, Ariz.",
	"Muncie, Ind",
	"McLean, Va.",
	"Issaquah, Wash.",
	"Pasadena, Calif.",
	"Walnut Creek, Calif.",
	"Moscow, Russia",
	"London, England",
	"Berlin, Germany",
	"Madrid, Spain",
	"Rome, Italy",
	"Kiev, Ukraine",
	"Paris, France",
	"Budapest, Hungary",
	"Hamburg, Germany",
	"Warsaw, Poland",
	"Wembley Park, England",
	"Kilburn, England",
	"Milan, Italy",
	"Barcelona, Spain",
	"Munich, Germany",
	"Naples, Italy",
	"Florence, Italy",
	"Riga, Latvia",
	"Leeds, England",
	"Staines, Westside",
	"Omsk, Russia",
	"Tooting, England",
	"North Pole, Alaska",
	"Why, Ariz.",
	"Lorida, Fla.",
	"Experiment, Ga.",
	"Normal, Ill.",
	"Dwarf, Ky.",
	"Hippo, Ky.",
	"Humptulips, Wash.",
	"Walla Walla, Wash.",
	"Bigfoot, Texas",
	"Happy, Texas",
	"Bird-in-Hand, Pa.",
	"Businessburg, Ohio",
	"Watford, England",
	"Loch Ness, Scotland",
	"Lake Titicaca, Peru",
	"Puddledock, England",
	"Little Snoring, U.K.",
	"Yelling, U.K.",
	"Crackpot, U.K.",
	"Great Snoring, U.K.",
	"High Ham, U.K.",
	"Giggleswick, U.K.",
	"Shiner, Texas",
	"Why, Ariz.",
	"Hasty, Colo.",
	"Protection, Kan.",
	"Moon, Ky.",
	"Trout, La.",
	"Weed, N.M.",
	"Hopton Wafers, U.K.",
	"Abu Dhabi",
	"Middle Wallop, U.K.",
	"Tokyo, Japan",
	"Easter Island",
	"Bermuda Triangle",
	"Mars",
	"The White House, D.C.",
	"Wapping, U.K.",
};

const char* SPECIALHOMETOWNS[6] = {
	"Philadelphia, Pa.",
	"Vienna, Austria",
	"Biloxi, Miss.",
	"Rensselaer, Ind.",
	"Los Angeles, Calif.",
	"North Pole"
};

const char* LIKES[] = {
	"Knitting",
	"Painting",
	"Woodworking",
	"Antiques",
	"Comic books",
	"Dumpster Diving",
	"Records",
	"Collecting Stamps",
	"Trading cards",
	"Smooth Jazz",
	"Board Games",
	"Card Games",
	"Historical Reenactment",
	"Rafting",
	"Photography",
	"Baseball",
	"Bowling",
	"Curling",
	"Live Action Role-playing",
	"Mountain Biking",
	"Hang Gliding",
	"Flying Ultralites",
	"Digeridooing",
	"Lottery Scratch-offs",
	"Samurai Swords",
	"Muscle Cars",
	"Mushroom Hunting",
	"Flossing",
	"Whiffleball",
	"Stargazing",
	"Tie-dying",
	"Kitchen Remodeling",
	"Mastiff Breeding",
	"Ornithology",
	"Mancala",
	"Backgammon",
	"Techno",
	"Corporate Law",
	"The Occult",
	"Videogames",
	"Gangsta Rap",
	"Poetry",
	"Wine Collecting",
	"Drag Racing",
	"Euchre",
	"Hoe-downs",
	"Jogging",
	"Breakdancing",
	"Fashion",
	"Swimming",
	"Line Dancing",
	"Mountaineering",
	"Tennis",
	"Staring",
	"Skiing",
	"Algebra",
	"Golf",
	"Power Walking",
	"High Explosives",
	"Blogging",
	"Pina Coladas",
	"WWII Memorabilia",
	"Online Chat",
	"Fundraising",
	"Yoga",
	"Coffee",
	"Fresh Fruit",
	"Salsa Dancing",
	"Improv Comedy",
	"Musicals",
	"Wine Tasting",
	"Weekend Getaways",
	"Bejeweled",
	"Zuma",
	"Bookworm",
	"Particle Physics",
	"Journalism",
	"Tearjerkers",
	"Foreign Cinema",
	"Spaceships",
	"Nanotechnology",
	"Leather Pants",
	"Country Music",
	"Grunge",
	"Styling Hair",
	"Ham Radio",
	"Fire Eating",
	"Window Tinting",
	"Bobsledding",
	"Glass Etching",
	"Poppies",
	"Glass Blowing",
	"Pottery",
	"Church Choir",
	"Spooning",
	"Gorgonzola",
	"Jam Bands",
	"Aggressive Skating",
	"Fire Walking",
	"20 Inch Rims",
	"Tricked Out Rides",
	"Poll Volunteering",
	"Indie Films",
	"Hair Bands",
	"Digital Cable",
	"HDTV",
	"Spelling Bees",
	"Combines",
	"Food Fights",
	"Beans",
	"Extreme Sports",
	"Riddles",
	"Knock-Knock Jokes",
	"Trampolines",
	"Low Carbin'",
	"Broadway Show Tunes",
	"The Lambada",
	"Car Repair",
	"Garage Sales",
	"Rubber Bands",
	"Number 2 Pencils",
	"Skim Milk",
	"Avoiding Sharks",
	"Collecting Tea Bags",
	"Tap Dancing",
	"Vaudeville",
	"Denver Omelets",
	"Basket Weaving",
	"Kung Fu Movies",
	"Debating",
	"Bargain Hunting",
	"Avoiding Hooks",
	"Train spotting",
	"Hot-tubbing",
	"Hide and Seek",
	"Local Politics",
	"General Insanity",
	"Antique Weapon Collecting",
	"Finger Painting",
	"Underwater Cartography",
	"Seismology",
	"Antique Car Restoration",
	"Blenders",
	"Skydiving",
	"Paintball",
	"Long Walks in the Park",
	"Sushi",
	"Company Picnics",
	"Filling Out Forms",
	"Meteorology",
	"Baking Cookies",
	"Billiards",
	"Soy Sauce",
	"Bubbles",
	"Trampolines",
	"Pirates",
	"Ninjas",
	"Robots",
	"Candy",
	"Camping",
	"Foreign Languages",
	"Clipping Coupons",
	"Swashbuckling",
	"Lumberjacks",
	"Entomology",
	"Weed Whacking",
	"Power Washers",
	"Working Out",
	"The Pacific Northwest",
	"Thrift Stores",
	"Monorails",
	"Recycling",
	"Beta Testing",
	"Fondue Parties",
	"Ladies Nights",
	"Infomercials",
	"Hippies",
	"Practical Jokes",
	"College Parties",
	"Soccer Hooligans",
	"Yakisoba",
	"Welding",
	"Ice Sculpting",
	"Vibraphones",
	"Anime",
	"MP3 Players",
	"Fingerless Gloves",
	"Fireside Chats",
	"Horseback Riding",
	"Working with Animals",
	"Whittling",
	"Oil Painting",
	"Macrame",
	"Sun Tanning",
	"Wakeboarding",
	"World Music",
	"Travel",
	"Horse Racing",
	"Sports Books",
	"Hockey",
	"Violin",
	"Crochet",
	"Sewing",
	"Spelunking",
	"Lizards",
	"Street Fighting",
	"Jigsaws",
	"Jujitsu",
	"Lard",
	"Pumping Iron",
	"Body Massage",
	"Stuffed Animals",
	"Stickers",
	"Glitter Glue",
	"Hot Dogs",
	"Pancakes",
	"Pork Chops",
	"Muffins",
	"Fencing",
	"Dental Visits",
	"Fireworks",
	"Skipping",
	"Glubbing",
	"Beekeeping",
	"Haggis",
	"Cheese Sculpture",
	"Shampoo",
	"Cubicles",
	"Bits of String",
	"Wood",
	"Telling Jokes",
	"Making Toast",
	"Counting to 10",
	"Pillow Fights",
	"Chili Cheese Omelets",
	"Kung Fu",
	"Karate",
	"Boxing",
	"Phone Phreaking",
	"Hacking",
	"Chemistry",
	"Karaoke",
	"Heavy Metal",
	"Ballet",
	"Casual Fridays",
	"Bejeweled 2",
	"The Stock Market",
	"Telecommuting",
	"Buddhism",
	"Vegan Cuisine",
	"TV Dinners",
	"Romantic Comedies",
	"Hearts",
	"Spades",
	"Yogic Flying",
	"Beach Workouts",
	"Collecting Shells",
	"Quantum Physics",
	"Pickling Vegetables",
	"Double Dipping",
	"Ichthyology",
	"Doomsday Cults",
	"80s Arcade Games",
	"True Crime Novels",
	"New Wave Music",
	"Bingo",
	"Breeding Pugs",
	"Dog Shows",
	"Flossing",
	"Moonlighting",
	"Asian Culture",
	"The GOP",
	"Libertarians",
	"Tai Chi",
	"Lip Balm",
	"Smoking Cigars",
	"Reality TV",
	"Haikus",
	"Lumbar Support",
	"Community Theater",
	"Organ Music",
	"Online Auctions",
	"Chili Cook-offs",
	"MMORPGs",
	"Fighting Terrorism",
	"Reggae",
	"Hog Roasts",
	"Tooth Necklaces",
	"Making Omelets",
	"Greasy Spoon Fare",
	"Port Hopping",
	"Raking Gravel",
	"Making Jerky",
	"Smoking Bacon",
	"Grilling Veggies",
	"Bar Crawling",
	"Wine Tasting",
	"Picketing",
	"Renaissance Faires",
	"Yachting",
	"Shopping for shoes",
	"Norse Mythology",
	"Strategy Gaming",
	"Glockenspiels",
	"Stock Car Racing",
	"Old Game Shows",
	"Tetherball",
	"Four Square",
	"Sleight of Hand",
	"Mandarin Cooking",
	"Hospital Corners",
	"Truffle Hunting",
	"Paintball",
	"Daytime TV",
	"Old 45s",
	"Haunted Houses",
	"Fixing Flats",
	"Public Transit",
	"GPS Games",
	"Hiking Rainier",
	"Cancer Research",
	"Woodburning Stoves",
	"Chimney Sweeping",
	"Telemarketers",
	"Moonlit Walks",
	"Value Menus",
	"Combo Meals",
	"Keeping it Real",
	"Clicking Like Crazy",
	"Bling Bling",
	"Romance Novels",
};

// The table sizes are const ints the original reads from .rdata at each use (0x59BDC8 = 375, 0x59BDC4 = 331)
const int NUM_HOMETOWNS = sizeof(HOMETOWNS) / sizeof(HOMETOWNS[0]);
const int NUM_LIKES = sizeof(LIKES) / sizeof(LIKES[0]);

Sexy::SimFishScreen::SimFishScreen(WinFishApp* theApp)
{
	mApp = theApp;
	mBubbleMgr = new BubbleMgr();
	mApp->StopMusic();
	mApp->PlayMusic(2, 0);
	mX = 0;
	mY = 0;
	m0x10c = 25;
	m0x110 = 94;
	mWidth = mApp->mWidth;
	mHeight = mApp->mHeight;

	mHideAllButton = MakeDialogButton2(105, this, "Hide All", IMAGE_MAINBUTTON);
	mHideAllButton->Resize(500, 4, 100, mHideAllButton->mHeight);

	mShowAllButton = MakeDialogButton2(106, this, "Show All", IMAGE_MAINBUTTON);
	mShowAllButton->Resize(500, 4, 100, mShowAllButton->mHeight);
	mShowAllButton->mVisible = false;

	mReturnButton = MakeDialogButton(100, this, "Return to Tank", FONT_JUNGLEFEVER10OUTLINE);
	mReturnButton->Resize(218, 428, 206, mReturnButton->mHeight);

	mMenuButton = MakeDialogButton2(101, this, "Menu", IMAGE_MAINBUTTON);
	mMenuButton->Resize(525, 4, 80, mMenuButton->mHeight);

	mHideShowButton = MakeDialogButton2(102, this, "", IMAGE_LEFTBUTTON);
	mHideShowButton->Resize(225, 390, 58, mHideShowButton->mHeight);
	mHideShowButton->mTextOffsetX = 0;

	mSellButton = MakeDialogButton2(103, this, "Sell", IMAGE_RIGHTBUTTON);
	mSellButton->Layout(0x4403, mHideShowButton, 72);
	mSellButton->mTextOffsetX = 0;

	mRenameButton = MakeDialogButton2(104, this, "Rename", IMAGE_CENTERBUTTON);
	mRenameButton->Layout(0x4402, mHideShowButton, -2);
	mRenameButton->Layout(0x20000, mSellButton, 0,0,3);

	mHideShowButton->SetColor(0, Color(0xffffff));
	mSellButton->SetColor(0, Color(0xffffff));
	mRenameButton->SetColor(0, Color(0xffffff));
	mReturnButton->SetColor(0, Color(255,240,0));

	mBackgroundImage = new MemoryImage(theApp);
	mBackgroundImage->Create(640, 480);
	mBackgroundImage->SetImageMode(true, true);
	Graphics gImg(mBackgroundImage);
	gImg.DrawImageBox(Rect(-5,-5,650,490), IMAGE_SCREENBACK);
	gImg.DrawImageBox(Rect(20,0,600, IMAGE_SCREENTITLE->mHeight), IMAGE_SCREENTITLE);
	gImg.DrawImageBox(Rect(mHideAllButton->mX - 1, mHideAllButton->mY - 1, mHideAllButton->mWidth + 2, IMAGE_SCREENTITLEHOLE->mHeight), IMAGE_SCREENTITLEHOLE);
	gImg.DrawImageBox(Rect(216, 48, IMAGE_FISHBOX->mWidth, 350), IMAGE_FISHBOX);
	gImg.DrawImage(IMAGE_FISHBOXBUTTON, 221, 385);

	mBubbleMgr->SetBubbleBounds(Rect(236, 48, IMAGE_FISHBOX->mWidth - 40, 335));
	mBubbleMgr->SetBubbleConfig(10, 3);
	mBubbleMgr->UpdateALot();

	memset(mObjectButtons, 0, sizeof(mObjectButtons));

	std::multimap<time_t, GameObject*> aFishMap;

	for (GameObjectSet::iterator it = mApp->mBoard->mGameObjectSet.begin(); it != mApp->mBoard->mGameObjectSet.end(); ++it)
	{
		GameObject* anObj = *it;
		// The original tests the upper bound unsigned (0x52F9D8: cmp 19; ja)
		if (anObj->mVirtualTankId >= 0 && (uint)anObj->mVirtualTankId <= 19)
			aFishMap.insert(std::pair<time_t, GameObject*>(anObj->mTimeBought, anObj));
	}

	// As the original: the whole map is walked, and only the first 20 get a button
	int aFishBtnId = 0;
	for (std::multimap<time_t, GameObject*>::iterator it = aFishMap.begin(); it != aFishMap.end(); ++it, aFishBtnId++)
	{
		if (aFishBtnId < 20)
			CreateFishButton(aFishBtnId, it->second);
	}

	for (int i = 0; i < 20;i++)
	{
		if (mObjectButtons[i] == nullptr)
			CreateFishButton(i, nullptr);
	}

	mOverlay = new SimFishScreenOverlay(this);

	mSelectedFishButton = nullptr;
	m0x114 = 0;
	m0x118 = false;
	m0x119 = false;
	m0x11a = false;
	DetermineShowHideForButtons();
}

Sexy::SimFishScreen::~SimFishScreen()
{
	if (mBubbleMgr)
		delete mBubbleMgr;
	for (int i = 0;i < 20;i++)
		delete mObjectButtons[i];
	if (mHideAllButton)
		delete mHideAllButton;
	if (mShowAllButton)
		delete mShowAllButton;
	if (mReturnButton)
		delete mReturnButton;
	if (mMenuButton)
		delete mMenuButton;
	if (mBackgroundImage)
		delete mBackgroundImage;
	if (mOverlay)
		delete mOverlay;
	if (mHideShowButton)
		delete mHideShowButton;
	if (mSellButton)
		delete mSellButton;
	if (mRenameButton)
		delete mRenameButton;
}

void Sexy::SimFishScreen::AddedToManager(WidgetManager* theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);
	for (int i = 0; i < 20;i++)
		theWidgetManager->AddWidget(mObjectButtons[i]);
	theWidgetManager->AddWidget(mOverlay);
	theWidgetManager->AddWidget(mReturnButton);
	theWidgetManager->AddWidget(mHideAllButton);
	theWidgetManager->AddWidget(mShowAllButton);
	theWidgetManager->AddWidget(mSellButton);
	theWidgetManager->AddWidget(mHideShowButton);
	theWidgetManager->AddWidget(mRenameButton);
}

void Sexy::SimFishScreen::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	for (int i = 0; i < 20;i++)
		theWidgetManager->RemoveWidget(mObjectButtons[i]);
	theWidgetManager->RemoveWidget(mOverlay);
	theWidgetManager->RemoveWidget(mReturnButton);
	theWidgetManager->RemoveWidget(mHideAllButton);
	theWidgetManager->RemoveWidget(mShowAllButton);
	theWidgetManager->RemoveWidget(mSellButton);
	theWidgetManager->RemoveWidget(mHideShowButton);
	theWidgetManager->RemoveWidget(mRenameButton);
}

void Sexy::SimFishScreen::Update()
{
	MarkDirty();
	mBubbleMgr->Update();
	if (m0x114 > 0)
	{
		if (m0x118)
			m0x114--;
		else
		{
			m0x114++;
			if (m0x114 >= 8)
				m0x114 = 0;
		}
	}
}

void Sexy::SimFishScreen::OrderInManagerChanged()
{
	for (int i = 0; i < 20;i++)
		mWidgetManager->BringToFront(mObjectButtons[i]);
	mWidgetManager->BringToFront(mOverlay);
	mWidgetManager->BringToFront(mReturnButton);
	mWidgetManager->BringToFront(mHideAllButton);
	mWidgetManager->BringToFront(mShowAllButton);
	mWidgetManager->BringToFront(mSellButton);
	mWidgetManager->BringToFront(mHideShowButton);
	mWidgetManager->BringToFront(mRenameButton);
}

void Sexy::SimFishScreen::DrawOverlay(Graphics* g)
{
	g->DrawImage(mBackgroundImage, 0, 0);
	mBubbleMgr->Draw(g);
	g->SetFont(FONT_JUNGLEFEVER17OUTLINE);
	g->SetColor(Color(255, 200, 0, 255));
	WriteCenteredLine(g, 25, "Fish Setup");

	for (int i = 0;i < 20;i++)
	{
		FishButtonWidget* aButton = mObjectButtons[i];
		g->DrawImage(IMAGE_PETBUTTONRING, aButton->mX, aButton->mY);
		if (!aButton->mDisabled)
		{
			g->DrawImage(IMAGE_PETBUTTONREFLECT, aButton->mX, aButton->mY);
			if (aButton->mObject && !aButton->mObject->mShown)
			{
				g->SetFont(FONT_JUNGLEFEVER12OUTLINE);
				g->SetColor(Color(0xff6060));
				g->DrawString("HIDDEN", aButton->mX + 13, aButton->mY + 50);
			}
		}
	}

	if (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
	{
		GameObject* anObj = mSelectedFishButton->mObject;
		g->Translate(320, 90);
		anObj->DrawStoreAnimation(g, 4);
		g->Translate(-320, -90);
		g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
		g->SetColor(Color(255, 200, 0, 255));
		// The original compares the name's last four characters in place (an inlined strcmp)
		if (g->GetFont()->StringWidth(anObj->mName) > 170 && anObj->mName.length() > 4 &&
			strcmp(anObj->mName.c_str() + anObj->mName.length() - 4, " JR.") == 0)
		{
			// A long name ending in " JR." is split over two lines
			SexyString aBaseName = anObj->mName.substr(0, anObj->mName.length() - 4);
			WriteCenteredLine(g, 80, aBaseName);
			WriteCenteredLine(g, 100, "JR.");
		}
		else
		{
			WriteCenteredLine(g, 80, anObj->mName);
		}

		if (!anObj->mShown)
		{
			g->SetColor(Color(0xff6060));
		}

		// One pair of colours serves both blocks below, as in the original
		Color aCol1(255, 200, 0, 255);
		Color aCol3(255, 255, 255);
		g->SetFont(FONT_JUNGLEFEVER10OUTLINE);
		int aFntHght = g->GetFont()->GetHeight();
		int aHeightSeparator = aFntHght * 1.75;
		if (!m0x118 || m0x114 > 0)
		{
			int aCurHght = 180;
			if (m0x114 > 0)
			{
				aCol3.mAlpha = m0x114 * 255 / 8;
				aCol1.mAlpha = aCol3.mAlpha;
			}
			g->SetColor(aCol1);
			if (anObj->mType == TYPE_GUPPY && ((Fish*)anObj)->mVirtualFish)
				WriteCenteredLine(g, aCurHght, "Date of Birth");
			else
				WriteCenteredLine(g, aCurHght, "Purchase Date");

			time_t aTime = anObj->mTimeBought;
			if (anObj->mTimeBought < 0)
				aTime = 0;
			tm* aTM = localtime(&aTime);
			char aTimeString[1024];
			if (aTM != nullptr)
				strftime(aTimeString, 1000, "%b %d %Y", aTM);
			else
				sprintf(aTimeString, "Unknown");

			g->SetColor(aCol3);
			aCurHght += aFntHght;
			WriteCenteredLine(g, aCurHght, aTimeString);

			g->SetColor(aCol1);
			aCurHght += aHeightSeparator;
			WriteCenteredLine(g, aCurHght, "Hometown");

			g->SetColor(aCol3);
			aCurHght += aFntHght;
			if (gUnkInt09 >= 0 && gUnkInt09 < NUM_HOMETOWNS)
				WriteCenteredLine(g, aCurHght, StrFormat("%s", HOMETOWNS[gUnkInt09]));
			else if (anObj->mPreNamedTypeId >= 0 && anObj->mPreNamedTypeId < 6)
				WriteCenteredLine(g, aCurHght, StrFormat("%s", SPECIALHOMETOWNS[anObj->mPreNamedTypeId]));
			else if (anObj->mHometownIdx >= 0 && anObj->mHometownIdx < NUM_HOMETOWNS)
				WriteCenteredLine(g, aCurHght, StrFormat("%s", HOMETOWNS[anObj->mHometownIdx]));
			else
				WriteCenteredLine(g, aCurHght, "Unknown");

			g->SetColor(aCol1);
			aCurHght += aHeightSeparator;
			WriteCenteredLine(g, aCurHght, "Mental State");

			g->SetColor(aCol3);
			aCurHght += aFntHght;
			WriteCenteredLine(g, aCurHght, anObj->GetMentalStateString());

			g->SetColor(aCol1);
			aCurHght += aHeightSeparator;
			WriteCenteredLine(g, aCurHght, "Additional Notes");

			g->SetColor(aCol3);
			aCurHght += 5;
			Rect aRect(234, aCurHght, 172, 100);
			SexyString aLikesStr;
			if (gUnkInt10 >= 0 && gUnkInt10 < NUM_LIKES)
				aLikesStr = StrFormat("Likes %s, %s, and %s.", LIKES[gUnkInt10], LIKES[gUnkInt10], LIKES[gUnkInt10]);
			else
				aLikesStr = GetAdditionalNotes(anObj);

			WriteWordWrapped(g, aRect, aLikesStr, -1, 0);
		}

		if (m0x118 || m0x114 > 0)
		{
			if (m0x114 > 0)
			{
				aCol3.mAlpha = 255 - ((m0x114 * 255) / 8);
				aCol1.mAlpha = aCol3.mAlpha;
			}
			g->SetColor(aCol1);
			int aCurHght = 190;
			WriteCenteredLine(g, aCurHght, "Purchase Price");

			g->SetColor(aCol3);
			aCurHght += aFntHght;
			if (anObj->mType == TYPE_GUPPY && ((Fish*)anObj)->mVirtualFish)
				WriteCenteredLine(g, aCurHght, "N/A");
			else
				WriteCenteredLine(g, aCurHght, StrFormat("%d Shells", anObj->mShellPrice));

			int aRefundValue = anObj->GetShellPrice();
			SexyString aRefundStr;
			if (aRefundValue == -1)
				aRefundStr = "Full Refund";
			else
				aRefundStr = StrFormat("%d Shells", aRefundValue);
			g->SetColor(aCol1);
			aCurHght += aHeightSeparator;
			WriteCenteredLine(g, aCurHght, "Resale Value");
			g->SetColor(aCol3);
			aCurHght += aFntHght;
			WriteCenteredLine(g, aCurHght, aRefundStr);
		}
	}
	else
	{
		Rect aRect(230, 180, 180, 100);
		g->SetFont(FONT_JUNGLEFEVER15OUTLINE);
		g->SetColor(Color(255, 200, 0, 255));
		WriteWordWrapped(g, aRect, "Select\na\nFish", -1, 0);
	}
}

void Sexy::SimFishScreen::ButtonPress(int theId, int theClickCount)
{
	mApp->PlaySample(SOUND_BUTTONCLICK);
	if (theClickCount == 2 && (uint)theId <= 19)
		ButtonDepress(102);
}

void Sexy::SimFishScreen::ButtonDepress(int theId)
{
	if ((uint)theId <= 19)
	{
		FishButtonWidget* aBtn = mObjectButtons[theId];
		if (aBtn != nullptr)
		{
			if (mSelectedFishButton != nullptr)
			{
				if (mSelectedFishButton->mId == theId)
					return;

				mSelectedFishButton->SwitchDrawRects(false);
			}
			mSelectedFishButton = aBtn;
			aBtn->SwitchDrawRects(true);
			DetermineShowHideForButtons();
		}
	}
	else if (theId == 100)
	{
		mApp->StopMusic();
		mApp->RemoveSimFishScreen();
		if (m0x11a)
			mApp->SaveVirtualTankAndUserData();
		if (mApp->mBoard)
		{
			mApp->mBoard->StartMusic();
			mApp->mBoard->PauseGame(false);
		}
	}
	else if (theId == 103)
	{
		if (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
		{
			m0x119 = true;
			m0x118 = true;
			m0x114 = 0;
			SexyString aStr = "Are you sure you want to sell your fish?";
			if (mSelectedFishButton->mObject->mType == TYPE_BREEDER)
			{
				GameObject* anObj = mApp->mBoard->GetGameObjectByVirtualId(mSelectedFishButton->mObject->mVirtualTankId + 100);
				if (anObj)
					aStr += "\n\nNote that selling the mama fish will not sell her baby fish.";
			}

			mApp->DoAreYouSureSellDialog(aStr);
		}
	}
	else if (theId == 102)
	{
		if (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
		{
			m0x11a = true;
			mApp->mBoard->HideObject(mSelectedFishButton->mObject, !mSelectedFishButton->mObject->mShown);
			DetermineShowHideForButtons();
			mWidgetManager->BringToFront(this);
		}
	}
	else if (theId == 104)
	{
		if (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
		{
			m0x11a = true;
			GameObject* anObj = mSelectedFishButton->mObject;
			mApp->DoFishNamingDialog("Please choose a name for your fish",
				anObj->mType == TYPE_BREEDER,
				anObj->mName,
				false);
		}
	}
	else if (theId == 105 || theId == 106)
	{
		m0x11a = true;
		for (int i = 0; i < 20; i++)
		{
			if (mObjectButtons[i] != nullptr && mObjectButtons[i]->mObject != nullptr)
				mApp->mBoard->HideObject(mObjectButtons[i]->mObject, theId == 106);
			mWidgetManager->BringToFront(this);
			DetermineShowHideForButtons();
		}
	}
}

void Sexy::SimFishScreen::ButtonMouseEnter(int theId)
{
	if (theId == 103)
	{
		m0x118 = true;
		m0x114 = 8;
	}
}

void Sexy::SimFishScreen::ButtonMouseLeave(int theId)
{
	if (theId == 103 && !m0x119)
	{
		m0x118 = false;
		m0x114 = 1;
	}
}

SexyString Sexy::GetAdditionalNotes(GameObject* theObject)
{
	switch (theObject->mPreNamedTypeId)
	{
	case ROCKY:
		return "By doctor\'s orders, is on a special low-carb high-Ultravore diet.";
	case LUDWIG:
		return "Likes monster truck rallies, Thai kick-boxing, and Beethoven.";
	case COOKIE:
		return "A fishy philanthropist who likes to feed food to famished fish? That\'s Fish-tastic!";
	case JOHNNYV:
		return "Has been trying to stop eating fast food and pizza.";
	case KILGORE:
		return "Likes to intimidate other fish by playing the music of Wagner.";
	case SANTA:
		return "Likes giving lots of toys to all the good girls and boys.";
	default:
	{
		const char* aLastLike = GetLastSpecialLike(theObject);
		if (aLastLike == NULL)
			aLastLike = LIKES[theObject->mLikes[2]];
		return StrFormat("Likes %s, %s, and %s.",
			LIKES[theObject->mLikes[0]],
			LIKES[theObject->mLikes[1]],
			aLastLike
		);
	}
	}
}

// The like that follows from the fish's special attribute, or NULL when it has none
const char* Sexy::GetLastSpecialLike(GameObject* theObject)
{
	int anAttrib = GameObject::GetAttribute(theObject);
	if (anAttrib == 9)
		return NULL;

	switch (anAttrib)
	{
	case 0:
		return "stealth";
	case 1:
		return "eating";
	case 2:
		return "quickness";
	case 3:
		return "singing";
	case 4:
		return "swimming backwards";
	case 5:
		switch (theObject->mExoticDietFoodType)
		{
		case EXO_FOOD_PIZZA:
			return "pizza";
		case EXO_FOOD_ICE_CREAM:
			return "ice cream";
		case EXO_FOOD_CHICKEN:
			return "chicken";
		default:
			return NULL;
		}
	case 6:
		switch (theObject->mExoticDietFoodType)
		{
		case EXO_FOOD_GUPPY:
			return "eating guppies";
		case EXO_FOOD_OSCAR:
			return "eating carnivores";
		case EXO_FOOD_ULTRA:
			return "eating ultravores";
		default:
			return NULL;
		}
	default:
		return "being different";
	}
}

void Sexy::SimFishScreen::CreateFishButton(int theButtonId, GameObject* theObject)
{
	if (theButtonId < 20)
	{
		mObjectButtons[theButtonId] = new FishButtonWidget(theObject, theButtonId, this);

		// Each range test starts with its lower bound, as in the original
		if (theButtonId >= 0 && theButtonId < 5)
			mObjectButtons[theButtonId]->Resize(m0x10c, theButtonId * 83 + 41, 90, 83);
		else if (theButtonId >= 5 && theButtonId < 10)
			mObjectButtons[theButtonId]->Resize(m0x110 + m0x10c, theButtonId * 83 - 374, 90, 83);
		else if (theButtonId >= 10 && theButtonId < 15)
			mObjectButtons[theButtonId]->Resize(m0x110*4 + m0x10c + 24, theButtonId * 83 - 789, 90, 83);
		else if (theButtonId >= 15)
			mObjectButtons[theButtonId]->Resize(m0x110*5 + m0x10c + 24, theButtonId * 83 -1204, 90, 83);

		ButtonHoleHelper(mBackgroundImage, (MemoryImage*)IMAGE_PETBUTTONHOLE, mObjectButtons[theButtonId]->mX, mObjectButtons[theButtonId]->mY);
	}
}

void Sexy::SimFishScreen::DetermineShowHideForButtons()
{
	bool anyHidden = false;
	for (int i = 0; i < 20; i++)
	{
		FishButtonWidget* aBtn = mObjectButtons[i];
		if (aBtn != nullptr && aBtn->mObject != nullptr && aBtn->mObject->mShown)
		{
			anyHidden = true;
			break;
		}
	}

	if (anyHidden)
	{
		mHideAllButton->mLabel = "Hide All";
		mHideAllButton->mId = 105;
	}
	else
	{
		mHideAllButton->mLabel = "Show All";
		mHideAllButton->mId = 106;
	}

	// One assignment of the chosen text, as in the original
	mHideShowButton->mLabel = (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr && !mSelectedFishButton->mObject->mShown) ? "Show" : "Hide";
}

void Sexy::SimFishScreen::RenameSelectedFish(const SexyString& theName)
{
	if (mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
		mSelectedFishButton->mObject->mName = theName;
}

void Sexy::SimFishScreen::SellSelectedObject(bool sell)
{
	m0x119 = false;
	m0x118 = false;
	m0x114 = 1;
	if (sell && mSelectedFishButton != nullptr && mSelectedFishButton->mObject != nullptr)
	{
		m0x11a = true;
		// The original keeps the sold object in a register across the calls below
		GameObject* aSoldObj = mSelectedFishButton->mObject;
		int aVal = aSoldObj->GetShellPrice();
		if (aVal <= 0)
			aVal = aSoldObj->mShellPrice;
		mApp->mCurrentProfile->AddShells(aVal);
		aSoldObj->RemoveHelper02(false);
		GameObject* anObj = nullptr;
		if (aSoldObj->mType == TYPE_BREEDER)
		{
			anObj = mApp->mBoard->GetGameObjectByVirtualId(aSoldObj->mVirtualTankId + 100);
			if (anObj)
				anObj->mVirtualTankId = aSoldObj->mVirtualTankId;
		}

		mApp->SafeDeleteWidget(aSoldObj);
		mSelectedFishButton->mObject = anObj;
		mSelectedFishButton->SetDisabled(anObj == nullptr);
		DetermineShowHideForButtons();
	}
}

void SimFishScreenOverlay::Draw(Sexy::Graphics* g)
{
	mScreen->DrawOverlay(g);
}
