#include "Fuji_Internal.h"

#include "MFThread_Internal.h"
#include "MFSystem_Internal.h"
#include "MFHeap.h"
#include "MFObjectPool.h"

// globals

static MFTls gThreadTls;

static size_t gMutexSize = MFThread_GetMutexSizePlatformSpecific();
static MFObjectPool gThreadInfo;

// functions

MFInitStatus MFThread_InitModule(int moduleId, bool bPerformInitialisation)
{

	if(!bPerformInitialisation)
		return MFIS_Succeeded;

	gMutexSize = MFThread_GetMutexSizePlatformSpecific();

	gThreadInfo.Init(sizeof(MFThreadInfo), gDefaults.system.maxThreads, gDefaults.system.maxThreads);
//	gThreadTls = MFThread_TlsAlloc();

	return MFIS_Succeeded;
}

void MFThread_DeinitModule()
{
//	MFThread_TlsFree(gThreadTls);
	gThreadInfo.Deinit();
}

static MFThreadInfo* MFThread_GetNewThreadInfo()
{
	return (MFThreadInfo*)gThreadInfo.Alloc();
}

int MFThread_ThreadProc(MFThreadInfo *pThreadInfo)
{
	// setup stuff...
	// tls? str buffer? etc...

	pThreadInfo->exitCode = pThreadInfo->pEntryPoint(pThreadInfo->pUserData);

	if(pThreadInfo->joinSemaphore)
		MFThread_SignalSemaphore(pThreadInfo->joinSemaphore);

	return pThreadInfo->exitCode;
}


// interface functions

MF_API MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 flags, uint32 stackSize)
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

MF_API int MFThread_GetExitCode(MFThread pThreadInfo)
{
	return pThreadInfo->exitCode;
}

MF_API void MFThread_DestroyThread(MFThread pThreadInfo)
{
	MFDebug_Assert(pThreadInfo != NULL, "Invalid thread.");

	MFThread_DestroyThreadPlatformSpecific(pThreadInfo);

	if(pThreadInfo->joinSemaphore)
	{
		MFThread_DestroySemaphore(pThreadInfo->joinSemaphore);
		pThreadInfo->joinSemaphore = NULL;
	}

	gThreadInfo.Free(pThreadInfo);
}

MF_API void MFThread_Join(MFThread pThreadInfo)
{
	MFDebug_Assert(pThreadInfo->flags & MFTF_Joinable, "Thread is not joinable.");

	MFThread_WaitSemaphore(pThreadInfo->joinSemaphore);
}


MF_API MFMutex MFThread_CreateMutex(const char *pName)
{
	MFMutex mutex = MFHeap_Alloc(gMutexSize);
	if(!mutex)
		return NULL;

	MFThread_InitMutexPlatformSpecific(mutex, pName);

	return mutex;
}
