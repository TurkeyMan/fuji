#include "Fuji.h"
#include "MFThread.h"
#include "MFPtrList.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// globals

static const int gMaxThreads = 16;
static const int gMaxMutexes = 16;

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
static MFMutexPC gMutexes[gMaxMutexes];


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

static MFMutexPC* MFThreadPC_GetNewMutex()
{
	for(int a=0; a<gMaxMutexes; a++)
	{
		if(!gMutexes[a].name[0])
			return &gMutexes[a];
	}

	return NULL;
}


static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)lpParameter;
	return (DWORD)pThreadInfo->pEntryPoint(pThreadInfo->pUserData);
}


// interface functions

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData)
{
	MFThreadInfoPC *pThreadInfo = MFThreadPC_GetNewThreadInfo();

	if(!pThreadInfo)
		return NULL;

	pThreadInfo->pEntryPoint = pEntryPoint;
	pThreadInfo->pUserData = pUserData;
	pThreadInfo->hThread = CreateThread(NULL, 0, ThreadProc, pThreadInfo, 0, &pThreadInfo->threadID);

	return pThreadInfo;
}

void MFThread_ExitThread(uint32 exitCode)
{
	ExitThread((DWORD)exitCode);
}

void MFThread_TerminateThread(MFThread thread, uint32 exitCode)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;
	TerminateThread(pThreadInfo->hThread, (DWORD)exitCode);
}

uint32 MFThread_GetExitCode(MFThread thread)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;
	DWORD exitCode;

	GetExitCodeThread(pThreadInfo->hThread, &exitCode);

	return (uint32)exitCode;
}

void MFThread_CloseThread(MFThread thread)
{
	MFThreadInfoPC *pThreadInfo = (MFThreadInfoPC*)thread;

	CloseHandle(pThreadInfo->hThread);
	pThreadInfo->hThread = NULL;
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	MFMutexPC *pMutex = MFThreadPC_GetNewMutex();

	if(!pMutex)
		return NULL;

	MFDebug_Assert(pName[0], "No name specified.");
	MFDebug_Assert(strlen(pName) <= 31, "Name must be less than 31 characters");
	strcpy(pMutex->name, pName);

	InitializeCriticalSection(&pMutex->criticalSection);

	return pMutex;
}

void MFThread_DestroyMutex(MFMutex mutex)
{
	DeleteCriticalSection((CRITICAL_SECTION*)mutex);
}

void MFThread_LockMutex(MFMutex mutex)
{
	EnterCriticalSection((CRITICAL_SECTION*)mutex);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	LeaveCriticalSection((CRITICAL_SECTION*)mutex);
}

MFSemaphore MFThread_CreateSemaphore(const char *pName, int initialCount, int maximumCount)
{
	return CreateSemaphore(NULL, initialCount, maximumCount, pName);
}

void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	CloseHandle(semaphore);
}

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore, uint32 timeout)
{
	DWORD r = WaitForSingleObject(semaphore, timeout);
	return r == WAIT_OBJECT_0 ? 0 : 1;
}

void MFThread_ReleaseSemaphore(MFSemaphore semaphore)
{
	ReleaseSemaphore(semaphore, 1, NULL);
}
