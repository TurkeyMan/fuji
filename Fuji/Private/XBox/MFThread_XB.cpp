#include "Fuji.h"
#include "MFThread.h"

#include <xtl.h>

// globals

static const int gMaxThreads = 16;
static const int gMaxMutexes = 16;

struct MFThreadInfoXB
{
	HANDLE hThread;
	DWORD threadID;

	MFThreadEntryPoint pEntryPoint;
	void *pUserData;
};

struct MFMutexXB
{
	CRITICAL_SECTION criticalSection;
	char name[32];
};

static MFThreadInfoXB gThreads[gMaxThreads];
static MFMutexXB gMutexes[gMaxMutexes];


// functions

static MFThreadInfoXB* MFThreadXB_GetNewThreadInfo()
{
	for(int a=0; a<gMaxThreads; a++)
	{
		if(!gThreads[a].hThread)
			return &gThreads[a];
	}

	return NULL;
}

static MFMutexXB* MFThreadXB_GetNewMutex()
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
	MFThreadInfoXB *pThreadInfo = (MFThreadInfoXB*)lpParameter;
	return (DWORD)pThreadInfo->pEntryPoint(pThreadInfo->pUserData);
}


// interface functions

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 stackSize)
{
	MFThreadInfoXB *pThreadInfo = MFThreadXB_GetNewThreadInfo();

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
	MFThreadInfoXB *pThreadInfo = (MFThreadInfoXB*)thread;

#pragma message("TODO: This needs to be fixed")
	MFDebug_Assert(false, "Cant terminate a thread on xbox.");
//	TerminateThread(pThreadInfo->hThread, (DWORD)-1);
}

int MFThread_GetExitCode(MFThread thread)
{
	MFThreadInfoXB *pThreadInfo = (MFThreadInfoXB*)thread;
	DWORD exitCode;

	GetExitCodeThread(pThreadInfo->hThread, &exitCode);

	return (int)exitCode;
}

void MFThread_DestroyThread(MFThread thread)
{
	MFThreadInfoXB *pThreadInfo = (MFThreadInfoXB*)thread;

	CloseHandle(pThreadInfo->hThread);
	pThreadInfo->hThread = NULL;
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	MFMutexXB *pMutex = MFThreadXB_GetNewMutex();

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
	MFMutexXB *pMutex = (MFMutexXB*)mutex;

	DeleteCriticalSection(&pMutex->criticalSection);

	pMutex->name[0] = 0;
}

void MFThread_LockMutex(MFMutex mutex)
{
	MFMutexXB *pMutex = (MFMutexXB*)mutex;

	EnterCriticalSection(&pMutex->criticalSection);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	MFMutexXB *pMutex = (MFMutexXB*)mutex;

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

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore, uint32 timeout)
{
	DWORD r = WaitForSingleObject(semaphore, timeout);
	return r == WAIT_OBJECT_0 ? 0 : 1;
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	ReleaseSemaphore(semaphore, 1, NULL);
}
