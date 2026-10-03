#include "WinFishApp.h"

using namespace Sexy;

// 0x5E8F28, used only by WinMain
WinFishApp* gWinFishApp = NULL;

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#else
int main()
{
#endif
	gWinFishApp = new WinFishApp();

	gWinFishApp->Init();
	gWinFishApp->Start();
	gWinFishApp->Shutdown();

	delete gWinFishApp;
	return 0;
}