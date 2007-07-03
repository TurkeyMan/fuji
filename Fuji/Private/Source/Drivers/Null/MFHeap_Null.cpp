#include "Fuji.h"

#if MF_HEAP == NULL

#include "MFHeap.h"

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	#include <stdlib.h>
#endif

void MFHeap_InitModulePlatformSpecific()
{
}

void MFHeap_DeinitModulePlatformSpecific()
{
}

// use CRT memory functions
void* MFHeap_SystemMalloc(uint32 bytes)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	return malloc(bytes);
#else
	return NULL;
#endif
}

void* MFHeap_SystemRealloc(void *buffer, uint32 bytes)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	return realloc(buffer, bytes);
#else
	return NULL;
#endif
}

void MFHeap_SystemFree(void *buffer)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
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

#endif
