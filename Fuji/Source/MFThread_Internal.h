#if !defined(_MFTHREAD_INTERNAL_H)
#define _MFTHREAD_INTERNAL_H

#include "MFThread.h"

struct MFThreadInfo
{
	char name[32];

	int priority;
	int exitCode;
	uint32 flags;
	size_t stackSize;

	MFThreadEntryPoint pEntryPoint;
	void *pUserData;

	void **ppTLSPointers;

	MFSemaphore joinSemaphore;

	char platformSpecific[16];
};

MFInitStatus MFThread_InitModule(int moduleId, bool bPerformInitialisation);
void MFThread_DeinitModule();

int MFThread_ThreadProc(MFThreadInfo *pThreadInfo);

void MFThread_CreatePlatformSpecific(MFThreadInfo *pThreadInfo);
void MFThread_DestroyThreadPlatformSpecific(MFThread thread);

size_t MFThread_GetMutexSizePlatformSpecific();
void MFThread_InitMutexPlatformSpecific(MFMutex mutex, const char *pName);

#endif
