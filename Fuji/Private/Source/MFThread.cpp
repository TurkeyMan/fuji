#include "Fuji.h"

#include "MFThread_Internal.h"
#include "MFSystem_Internal.h"
#include "MFHeap.h"

// globals

static MFThreadInfo *gpThreads;

static MFTls gThreadTls;

static int gMutexSize = MFThread_GetMutexSizePlatformSpecific();

// functions

void MFThread_InitModule()
{
	gMutexSize = MFThread_GetMutexSizePlatformSpecific();

	gpThreads = (MFThreadInfo*)MFHeap_AllocAndZero(sizeof(MFThreadInfo)*gDefaults.thread.maxThreads);
//	gThreadTls = MFThread_TlsAlloc();
}

void MFThread_DeinitModule()
{
//	MFThread_TlsFree(gThreadTls);
	MFHeap_Free(gpThreads);
}

static MFThreadInfo* MFThread_GetNewThreadInfo()
{
	// TODO: ALLOC FROM LOCK-FREE POOL
	for(uint32 a=0; a<gDefaults.thread.maxThreads; a++)
	{
		if(!gpThreads[a].pEntryPoint)
			return &gpThreads[a];
	}

	return NULL;
}

int MFThread_ThreadProc(MFThreadInfo *pThreadInfo)
{
	// setup stuff...
	// tls? str buffer? etc...

	pThreadInfo->exitCode = pThreadInfo->pEntryPoint(pThreadInfo->pUserData);

	return pThreadInfo->exitCode;
}


// interface functions

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 flags, uint32 stackSize)
{
	MFThreadInfo *pThreadInfo = MFThread_GetNewThreadInfo();

	if(!pThreadInfo)
		return NULL;

	pThreadInfo->pEntryPoint = pEntryPoint;
	pThreadInfo->pUserData = pUserData;
	pThreadInfo->priority = priority;
	pThreadInfo->flags = flags;
	pThreadInfo->stackSize = stackSize;
	pThreadInfo->exitCode = 0;

	if(pThreadInfo->flags & MFTF_Joinable)
		pThreadInfo->joinSemaphore = MFThread_CreateSemaphore("Join Semaphore", 1, 0);
	else
		pThreadInfo->joinSemaphore = 0;

	MFThread_CreatePlatformSpecific(pThreadInfo);

	return pThreadInfo;
}

int MFThread_GetExitCode(MFThread pThreadInfo)
{
	return pThreadInfo->exitCode;
}

void MFThread_DestroyThread(MFThread pThreadInfo)
{
	MFDebug_Assert(pThreadInfo != NULL, "Invalid thread.");

	MFThread_DestroyThreadPlatformSpecific(pThreadInfo);

	if(pThreadInfo->joinSemaphore)
	{
		MFThread_DestroySemaphore(pThreadInfo->joinSemaphore);
		pThreadInfo->joinSemaphore = NULL;
	}

	pThreadInfo->pEntryPoint = NULL;
}

void MFThread_Join(MFThread pThreadInfo)
{
	MFDebug_Assert(pThreadInfo->flags & MFTF_Joinable, "Thread is not joinable.");

	MFThread_WaitSemaphore(pThreadInfo->joinSemaphore);
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	MFMutex mutex = MFHeap_Alloc(gMutexSize);
	if(!mutex)
		return NULL;

	MFThread_InitMutexPlatformSpecific(mutex, pName);

	return mutex;
}
