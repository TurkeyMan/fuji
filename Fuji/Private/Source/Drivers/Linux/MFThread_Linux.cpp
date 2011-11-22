#include "Fuji.h"

#if MF_THREAD == MF_DRIVER_LINUX

#include "MFThread.h"
#include "MFHeap.h"

#include <pthread.h>

struct MFThreadInfoLinux
{
	pthread_t thread;
};

struct MFMutexLinux
{
	pthread_mutex_t mutex;
	char name[32];
};

// interface functions

static void *ThreadProc(void *pUserData)
{
	MFThread_ThreadProc((MFThreadInfo*)pUserData);
	return NULL;
}

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo)
{
	MFDebug_Assert(sizeof(MFThreadInfoPC) <= sizeof(pThreadInfo->platformSpecific), "Thread info too large!");
	MFThreadInfoLinux *pThreadInfoLinux = (MFThreadInfoLinux*)pThreadInfo->platformSpecific;

	// set the attributes
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	attr.stacksize = MFMax(stackSize, PTHREAD_STACK_MIN);
//	attr.schedparam.sched_priority = priority; // we can set the priority here, we'll just use the default for the moment...

	pthread_create(&pThreadInfoLinux->thread, NULL, ThreadProc, pThreadInfo);
}

void MFThread_ExitThread(int exitCode)
{
	pthread_exit((void*&)exitCode);
}

void MFThread_TerminateThread(MFThread thread)
{
	MFDebug_Assert(false, "Not written...");
}

void MFThread_DestroyThreadPlatformSpecific(MFThread thread)
{
}


int MFThread_GetMutexSizePlatformSpecific()
{
	return sizeof(MFMutexLinux);
}

void MFThread_InitMutexPlatformSpecific(MFMutex mutex, const char *pName)
{
	MFMutexLinux *pMutex = (MFMutexLinux*)mutex;

	MFDebug_Assert(pName[0], "No name specified.");
	MFDebug_Assert(MFString_Length(pName) <= 31, "Name must be less than 31 characters");
	MFString_Copy(pMutex->name, pName);

	pthread_mutex_init(&pMutex->mutex, NULL);
}

void MFThread_DestroyMutex(MFMutex mutex)
{
	MFMutexLinux *pMutex = (MFMutexLinux*)mutex;
	pthread_mutex_destroy(&pMutex->mutex);
}

void MFThread_LockMutex(MFMutex mutex)
{
	MFMutexLinux *pMutex = (MFMutexLinux*)mutex;
	pthread_mutex_lock(&pMutex->mutex);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	MFMutexLinux *pMutex = (MFMutexLinux*)mutex;
	pthread_mutex_unlock(&pMutex->mutex);
}

MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	MFDebug_Assert(false, "Not written...");
	return NULL;
}

void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	MFDebug_Assert(false, "Not written...");
}

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	MFDebug_Assert(false, "Not written...");
	return 0;
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	MFDebug_Assert(false, "Not written...");
}

#endif
