#include "Fuji.h"

#if MF_HEAP == MF_DRIVER_PS2

#include "MFHeap.h"

#include <stdlib.h>

void MFHeap_InitModulePlatformSpecific()
{
}

void MFHeap_DeinitModulePlatformSpecific()
{
}

// use CRT memory functions
void* MFHeap_SystemMalloc(size_t bytes)
{
	return malloc(bytes);
}

void* MFHeap_SystemRealloc(void *buffer, size_t bytes)
{
	return realloc(buffer, bytes);
}

void MFHeap_SystemFree(void *buffer)
{
	return free(buffer);
}


void* MFHeap_GetUncachedPointer(void *pPointer)
{
	return (void*)((uintp)pPointer | 0x20000000); // enable uncached mode

	// use 0x30000000 for uncached accelerated..
}

void MFHeap_FlushDCache()
{
}

#endif
