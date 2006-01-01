#include "Fuji.h"
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

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 stackSize)
{
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

void MFThread_ExitThread(int exitCode)
{
	sceKernelExitThread(exitCode);
}

void MFThread_TerminateThread(MFThread thread)
{
	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)thread;

	sceKernelTerminateThread(pThreadInfo->hThread);
}

int MFThread_GetExitCode(MFThread thread)
{
//	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)thread;
	return 0;
}

void MFThread_DestroyThread(MFThread thread)
{
	MFThreadInfoPSP *pThreadInfo = (MFThreadInfoPSP*)thread;

	sceKernelDeleteThread(pThreadInfo->hThread);
	pThreadInfo->hThread = 0;
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	return (MFMutex)MFThread_CreateSemaphore(pName, 1, 1);
}

void MFThread_DestroyMutex(MFMutex mutex)
{
	MFThread_DestroySemaphore((MFSemaphore)mutex);
}

void MFThread_LockMutex(MFMutex mutex)
{
	MFThread_WaitSemaphore((MFSemaphore)mutex);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	MFThread_SignalSemaphore((MFSemaphore)mutex);
}

MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return (MFSemaphore)sceKernelCreateSema(pName, 0, startCount, maxCount, NULL);
}

void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	sceKernelDeleteSema((SceUID)semaphore);
}

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	return (uint32)sceKernelWaitSema((SceUID)semaphore, 1, NULL);
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	sceKernelSignalSema((SceUID)semaphore, 1);
}
