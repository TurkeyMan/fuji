#include "Fuji.h"
#include "MFHeap.h"

#include <stdlib.h>

void MFHeap_InitModulePlatformSpecific()
{
#if defined(XB_XGTEXTURES)
	MFHeap_SetAllocAlignment(4096);
#endif
}

void MFHeap_DeinitModulePlatformSpecific()
{
}

// use CRT memory functions
void* MFHeap_SystemMalloc(uint32 bytes)
{
	MFCALLSTACK;

#if defined(XB_XGTEXTURES)
	return XPhysicalAlloc(bytes, MAXULONG_PTR, 16, PAGE_READWRITE);
#else
	return malloc(bytes);
#endif
}

void* MFHeap_SystemRealloc(void *buffer, uint32 bytes)
{
	MFCALLSTACK;

#if defined(XB_XGTEXTURES)
	return NULL;
#else
	return realloc(buffer, bytes);
#endif
}

void MFHeap_SystemFree(void *buffer)
{
	MFCALLSTACK;

#if defined(XB_XGTEXTURES)
	XPhysicalFree(buffer);
#else
	free(buffer);
#endif
}


void* MFHeap_GetUncachedPointer(void *pPointer)
{
	return pPointer;
}

void MFHeap_FlushDCache()
{
}
