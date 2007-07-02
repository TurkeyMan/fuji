#include "Fuji.h"

#if MF_HEAP == WIN32

#include "MFHeap.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HANDLE gMainHeap = NULL;

void MFHeap_InitModulePlatformSpecific()
{
	MFCALLSTACK;

}

void MFHeap_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;

	MFDebug_Assert(HeapValidate(gMainHeap, 0, NULL), "System Heap corruption detected!!");
}

// use CRT memory functions
void* MFHeap_SystemMalloc(uint32 bytes)
{
	MFCALLSTACK;

	if(!gMainHeap)
		gMainHeap = GetProcessHeap();

	return HeapAlloc(gMainHeap, 0, bytes);
}

void* MFHeap_SystemRealloc(void *buffer, uint32 bytes)
{
	MFCALLSTACK;

	return HeapReAlloc(gMainHeap, 0, buffer, bytes);
}

void MFHeap_SystemFree(void *buffer)
{
	MFCALLSTACK;

	MFDebug_Assert(HeapValidate(gMainHeap, 0, buffer), "System heap corruption detected!");
	HeapFree(gMainHeap, 0, buffer);
}

void* MFHeap_GetUncachedPointer(void *pPointer)
{
	return pPointer;
}

void MFHeap_FlushDCache()
{
}

#endif // MF_HEAP
