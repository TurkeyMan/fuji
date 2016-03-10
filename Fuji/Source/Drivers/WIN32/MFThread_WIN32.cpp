#include "Fuji_Internal.h"

#if MF_THREAD == MF_DRIVER_WIN32

#include "MFThread_Internal.h"
#include "MFHeap.h"

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(MF_XBOX)
	#include <xtl.h>
#endif

// globals

struct MFThreadInfoPC
{
	HANDLE hThread;
	DWORD threadID;
};

struct MFMutexPC
{
	CRITICAL_SECTION criticalSection;
	char name[32];
};

// functions

static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	return (DWORD)MFThread_ThreadProc((MFThreadInfo*)lpParameter);
}

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo)
{
	MFDebug_Assert(sizeof(MFThreadInfoPC) <= sizeof(pThreadInfo->platformSpecific), "Thread info too large!");
	MFDebug_Assert(pThreadInfo->pEntryPoint != NULL, "No entry point specified!");

	MFThreadInfoPC *pThreadInfoPC = (MFThreadInfoPC*)pThreadInfo->platformSpecific;
	pThreadInfoPC->hThread = CreateThread(NULL, pThreadInfo->priority, ThreadProc, pThreadInfo, 0, &pThreadInfoPC->threadID);
}

MF_API void MFThread_ExitThread(int exitCode)
{
	// TODO: get pThreadInfo from TLS...
//	pThreadInfo->exitCode = exitCode;

	// TODO: signal join semaphore

	ExitThread((DWORD)exitCode);
}

MF_API void MFThread_TerminateThread(MFThread pThreadInfo)
{
	MFThreadInfoPC *pThreadInfoPC = (MFThreadInfoPC*)pThreadInfo->platformSpecific;

	pThreadInfo->exitCode = -1;

#if defined(MF_XBOX)
	#pragma message("TODO: This needs to be fixed")
	MFDebug_Assert(false, "Cant terminate a thread on xbox.");
#else
	TerminateThread(pThreadInfoPC->hThread, (DWORD)pThreadInfo->exitCode);
#endif
}

void MFThread_DestroyThreadPlatformSpecific(MFThread thread)
{
	MFThreadInfoPC *pThreadInfoPC = (MFThreadInfoPC*)thread->platformSpecific;

	CloseHandle(pThreadInfoPC->hThread);
	pThreadInfoPC->hThread = NULL;
}

MF_API void MFThread_Sleep(uint32 milliseconds)
{
	Sleep(milliseconds);
}

size_t MFThread_GetMutexSizePlatformSpecific()
{
	return sizeof(MFMutexPC);
}

void MFThread_InitMutexPlatformSpecific(MFMutex mutex, const char *pName)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	MFDebug_Assert(pName[0], "No name specified.");
	MFDebug_Assert(MFString_Length(pName) <= 31, "Name must be less than 31 characters");
	MFString_Copy(pMutex->name, pName);

	InitializeCriticalSection(&pMutex->criticalSection);
}

MF_API void MFThread_DestroyMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	DeleteCriticalSection(&pMutex->criticalSection);

	MFHeap_Free(pMutex);
}

MF_API void MFThread_LockMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	EnterCriticalSection(&pMutex->criticalSection);
}

MF_API void MFThread_ReleaseMutex(MFMutex mutex)
{
	MFMutexPC *pMutex = (MFMutexPC*)mutex;

	LeaveCriticalSection(&pMutex->criticalSection);
}

MF_API MFSemaphore MFThread_CreateSemaphore(const char *pName, int maxCount, int startCount)
{
	return CreateSemaphore(NULL, startCount, maxCount, MFString_UFT8AsWChar(pName));
}

MF_API void MFThread_DestroySemaphore(MFSemaphore semaphore)
{
	CloseHandle(semaphore);
}

MF_API uint32 MFThread_WaitSemaphore(MFSemaphore semaphore)
{
	DWORD r = WaitForSingleObject(semaphore, INFINITE);
	return r == WAIT_OBJECT_0 ? 0 : 1;
}

MF_API void MFThread_SignalSemaphore(MFSemaphore semaphore)
{
	ReleaseSemaphore(semaphore, 1, NULL);
}

#endif // MF_THREAD
