#include "Fuji.h"
#include "MFHeap.h"

#include <stdlib.h>
#include <psputils.h>

void MFHeap_InitModulePlatformSpecific()
{
}

void MFHeap_DeinitModulePlatformSpecific()
{
}

// use CRT memory functions
void* MFHeap_SystemMalloc(uint32 bytes)
{
	return malloc(bytes);
}

void* MFHeap_SystemRealloc(void *buffer, uint32 bytes)
{
	return realloc(buffer, bytes);
}

void MFHeap_SystemFree(void *buffer)
{
	return free(buffer);
}


void* MFHeap_GetUncachedPointer(void *pPointer)
{
	return (void*)((uint32)pPointer | 0x40000000); // enable uncached mode
}

void MFHeap_FlushDCache()
{
	sceKernelDcacheWritebackAll();
}
