#include "Fuji.h"

#if MF_THREAD == MF_DRIVER_WIN32

#include "MFThread.h"
#include "MFHeap.h"

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(MF_XBOX)
	#include <xtl.h>
#endif

// globals

static const int gMaxThreads = 16;

struct MFThreadInfoPC
{
	HANDLE hThread;
	DWORD threadID;

	MFThreadEntryPoint pEntryPoint;
	void *pUserData;
};

struct MFMutexPC
{
	CRITICAL_SECTION criticalSection;
	char name[32];
};

static MFThreadInfoPC gThreads[gMaxThreads];


// functions

static MFThreadInfoPC* MFThreadPC_GetNewThreadInfo()
{
	for(int a=0; a<gMaxThreads; a++)
	{
		if(!gThreads[a].hThread)
			return &gThreads[a];
	}

	return NULL;
}

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)lpParameter;
	return (DWORD)pThreadInfo->pEntryPoint(pThreadInfo->pUserData);
}


// interface functions

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 stackSize)
{
	MFThreadInfoPC *pThreadInfo = MFThreadPC_GetNewThreadInfo();

	if(!pThreadInfo)
		return NULL;

	pThreadInfo->pEntryPoint = pEntryPoint;
	pThreadInfo->pUserData = pUserData;
	pThreadInfo->hThread = CreateThread(NULL, stackSize, ThreadProc, pThreadInfo, 0, &pThreadInfo->threadID);

	return pThreadInfo;
}

void MFThread_ExitThread(int exitCode)
{
	ExitThread((DWORD)exitCode);
}

void MFThread_TerminateThread(MFThread thread)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;

#if defined(MF_XBOX)
	#pragma message("TODO: This needs to be fixed")
	MFDebug_Assert(false, "Cant terminate a thread on xbox.");
#else
	TerminateThread(pThreadInfo->hThread, (DWORD)-1);
#endif
}

int MFThread_GetExitCode(MFThread thread)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;
	DWORD exitCode;

	GetExitCodeThread(pThreadInfo->hThread, &exitCode);

	return (int)exitCode;
}

void MFThread_DestroyThread(MFThread thread)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;

	CloseHandle(pThreadInfo->hThread);
	pThreadInfo->hThread = NULL;
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	MFMutexPC *pMutex = (MFMutexPC*)MFHeap_Alloc(sizeof(MFMutexPC));

	if(!pMutex)
		return NULL;

	MFDebug_Assert(pName[0], "No name specified.");
	MFDebug_Assert(MFString_Length(pName) <= 31, "Name must be less than 31 characters");
	MFString_Copy(pMutex->name, pName);

	InitializeCriticalSection(&pMutex->criticalSection);

	return pMutex;
}

void MFThread_DestroyMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	DeleteCriticalSection(&pMutex->criticalSection);

	MFHeap_Free(pMutex);
}

void MFThread_LockMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	EnterCriticalSection(&pMutex->criticalSection);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	LeaveCriticalSection(&pMutex->criticalSection);
}

MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return CreateSemaphore(NULL, startCount, maxCount, pName);
}

void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	CloseHandle(semaphore);
}

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	DWORD r = WaitForSingleObject(semaphore, INFINITE);
	return r == WAIT_OBJECT_0 ? 0 : 1;
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	ReleaseSemaphore(semaphore, 1, NULL);
}

#endif // MF_THREAD
