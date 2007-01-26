#include "Fuji.h"
#include "MFThread.h"
#include "MFHeap.h"

#include <pthread.h>

// interface functions

MFThread MFThread_CreateThread(const char *pName, MFThreadEntryPoint pEntryPoint, void *pUserData, int priority, uint32 stackSize)
{
	pthread_t thread;

	// set the attributes
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	attr.stacksize = MFMax(stackSize, PTHREAD_STACK_MIN);
//	attr.schedparam.sched_priority = priority; // we can set the priority here, we'll just use the default for the moment...

	pthread_create(&thread, NULL, (void*(*)(void*))pEntryPoint, pUserData);

	return (MFThread&)thread;
}

void MFThread_ExitThread(int exitCode)
{
	pthread_exit((void*&)exitCode);
}

void MFThread_TerminateThread(MFThread thread)
{
	MFDebug_Assert(false, "Not written...");
}

int MFThread_GetExitCode(MFThread thread)
{
	MFDebug_Assert(false, "Not written...");
	return 0;
}

void MFThread_DestroyThread(MFThread thread)
{
}


MFMutex MFThread_CreateMutex(const char *pName)
{
	pthread_mutex_t *pMutex = (pthread_mutex_t*)MFHeap_Alloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pMutex, NULL);

	return pMutex;
}

void MFThread_DestroyMutex(MFMutex mutex)
{
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
	MFHeap_Free(mutex);
}

void MFThread_LockMutex(MFMutex mutex)
{
	pthread_mutex_lock((pthread_mutex_t*)mutex);
}

void MFThread_ReleaseMutex(MFMutex mutex)
{
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
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

uint32 MFThread_WaitSemaphore(MFSemaphore semaphore, uint32 timeout)
{
	MFDebug_Assert(false, "Not written...");
	return 0;
}

void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	MFDebug_Assert(false, "Not written...");
}
