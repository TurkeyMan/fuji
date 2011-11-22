#include "Fuji.h"

#if MF_THREAD == MF_DRIVER_PS2

#include "MFThread.h"

// globals

// interface functions

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo)
{
	return NULL;
}

void MFThread_ExitThread(int exitCode)
{
}

void MFThread_TerminateThread(MFThread thread)
{
}

void MFThread_DestroyThreadPlatformSpecific(MFThread thread)
{
}

MFMutex MFThread_CreateMutex(const char *pName)
{
	return NULL;
}

void MFThread_DestroyMutex(MFMutex mutex)
{
}

void MFThread_LockMutex(MFMutex mutex)
{
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
}

MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return NULL;
}

void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
}

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	return 0;
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
}

#endif
