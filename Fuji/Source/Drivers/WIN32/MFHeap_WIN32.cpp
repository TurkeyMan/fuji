#include "Fuji.h"

#if MF_HEAP == MF_DRIVER_WIN32

#include "MFHeap.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void MFHeap_InitModulePlatformSpecific()
{
	MFCALLSTACK;

}

void MFHeap_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	MFDebug_Assert(HeapValidate(GetProcessHeap(), 0, NULL), "System Heap corruption detected!!");
}

// use CRT memory functions
void* MFHeap_SystemMalloc(size_t bytes)
{
	MFCALLSTACK;

	return HeapAlloc(GetProcessHeap(), 0, bytes);
}

void* MFHeap_SystemRealloc(void *buffer, size_t bytes)
{
	MFCALLSTACK;

	return HeapReAlloc(GetProcessHeap(), 0, buffer, bytes);
}

void MFHeap_SystemFree(void *buffer)
{
	MFCALLSTACK;

	MFDebug_Assert(HeapValidate(GetProcessHeap(), 0, buffer), "System heap corruption detected!");
	HeapFree(GetProcessHeap(), 0, buffer);
}

void* MFHeap_GetUncachedPointer(void *pPointer)
{
	return pPointer;
}

void MFHeap_FlushDCache()
{
}

bool MFHeap_Validate()
{
	return !!HeapValidate(GetProcessHeap(), 0, NULL);
}

#endif // MF_HEAP
