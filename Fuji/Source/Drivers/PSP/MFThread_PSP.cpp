#include "Fuji_Internal.h"

#if MF_THREAD == MF_DRIVER_PSP

#include "MFThread.h"

#include <pspthreadman.h>

// globals

static const int gMaxThreads = 16;
static const int gMaxMutexes = 16;

struct MFThreadInfoPSP
{
	SceUID hThread;

	MFThreadEntryPoint pEntryPoint;
	void *pUserData;
};

static MFThreadInfoPSP gThreads[gMaxThreads];


// functions

static MFThreadInfoPSP* MFThreadPC_GetNewThreadInfo()
{
	for(int a=0; a<gMaxThreads; a++)
	{
		if(!gThreads[a].hThread)
			return &gThreads[a];
	}

	return NULL;
}

int ThreadProc(SceSize args, void *argp)
{
	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)argp;
	return pThreadInfo->pEntryPoint(pThreadInfo->pUserData);
}


// interface functions

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo)
{
	fix me!
	MFThreadInfoPSP *pThreadInfo = MFThreadPC_GetNewThreadInfo();

	if(!pThreadInfo)
		return NULL;

	if(stackSize == 0)
		stackSize = 0x10000;

	pThreadInfo->pEntryPoint = pEntryPoint;
	pThreadInfo->pUserData = pUserData;
	pThreadInfo->hThread = sceKernelCreateThread(pName, ThreadProc, 0x18, stackSize, 0, NULL);

	sceKernelStartThread(pThreadInfo->hThread, sizeof(MFThreadInfoPSP), pThreadInfo);

	return pThreadInfo;
}

MF_API void MFThread_ExitThread(int exitCode)
{
	sceKernelExitThread(exitCode);
}

MF_API void MFThread_TerminateThread(MFThread thread)
{
	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)thread;

	sceKernelTerminateThread(pThreadInfo->hThread);
}

void MFThread_DestroyThreadPlatformSpecific(MFThread thread)
{
	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)thread;

	sceKernelDeleteThread(pThreadInfo->hThread);
	pThreadInfo->hThread = 0;
}


MF_API MFMutex MFThread_CreateMutex(const char *pName)
{
	return (MFMutex)sceKernelCreateSema(pName, 0, 1, 1, NULL);
}

MF_API void MFThread_DestroyMutex(MFMutex mutex)
{
	sceKernelDeleteSema((SceUID)mutex);
}

MF_API void MFThread_LockMutex(MFMutex mutex)
{
	sceKernelWaitSema((SceUID)mutex, 1, NULL);
}

MF_API void MFThread_ReleaseMutex(MFMutex mutex)
{
	sceKernelSignalSema((SceUID)mutex, 1);
}

MF_API MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return (MFSemaphore)sceKernelCreateSema(pName, 0, startCount, maxCount, NULL);
}

MF_API void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	sceKernelDeleteSema((SceUID)semaphore);
}

MF_API uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	return (uint32)sceKernelWaitSema((SceUID)semaphore, 1, NULL);
}

MF_API void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	sceKernelSignalSema((SceUID)semaphore, 1);
}

#endif
