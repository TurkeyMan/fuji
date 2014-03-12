#include "Fuji_Internal.h"

#if MF_THREAD == MF_DRIVER_PS2

#include "MFThread.h"

// globals

// interface functions

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo)
{
	return NULL;
}

MF_API void MFThread_ExitThread(int exitCode)
{
}

MF_API void MFThread_TerminateThread(MFThread thread)
{
}

void MFThread_DestroyThreadPlatformSpecific(MFThread thread)
{
}

MF_API MFMutex MFThread_CreateMutex(const char *pName)
{
	return NULL;
}

MF_API void MFThread_DestroyMutex(MFMutex mutex)
{
}

MF_API void MFThread_LockMutex(MFMutex mutex)
{
}

MF_API void MFThread_ReleaseMutex(MFMutex mutex)
{
}

MF_API MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return NULL;
}

MF_API void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
}

MF_API uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	return 0;
}

MF_API void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
}

#endif
