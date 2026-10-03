#include "WorkerThread.h"

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#error WorkerThread is unimplemented on non-win32 platforms.
#endif

using namespace Sexy;

WorkerThread::WorkerThread()
{
	mShutdown = false;
	mTaskProc = NULL;
	mUnk1 = CreateEventA(0, FALSE, FALSE, NULL);
	mUnk2 = CreateEventA(0, FALSE, FALSE, NULL);

	_beginthread((void(*)(void*)) WorkerThread::StaticThreadProc, 0, this);
}

WorkerThread::~WorkerThread()
{
	WaitForTask();

	mShutdown = true;
	SetEvent(mUnk1);
	WaitForSingleObject(mUnk2, 5000);

	CloseHandle(mUnk1);
	CloseHandle(mUnk2);
}

void WorkerThread::WaitForTask()
{
	while (mTaskProc)
	{
		WaitForSingleObject(mUnk2, 1000);
	}

	ResetEvent(mUnk2);
}

void WorkerThread::DoTask(void (*theTaskProc)(void*), void* theParam)
{
	WaitForTask();

	mTaskProc = theTaskProc;
	mParam = theParam;

	SetEvent(mUnk1);
}

// The thread's wait-and-run loop is a member function of its own in the original (0x502560), which
// StaticThreadProc (0x503CA0) enters with a jmp
void WorkerThread::ThreadProc()
{
	WaitForSingleObject(mUnk1, 1000);
	while (!mShutdown)
	{
		if (mTaskProc) {
			mTaskProc(mParam);
			mTaskProc = NULL;

			SetEvent(mUnk2);
		}

		WaitForSingleObject(mUnk1, 1000);
	}

	SetEvent(mUnk2);
}

void WorkerThread::StaticThreadProc(WorkerThread* thr)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	thr->ThreadProc();
}