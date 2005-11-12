#include "Fuji.h"
#include "MFHeap.h"

#include <stdlib.h>

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
	return pPointer;
}

void MFHeap_FlushDCache()
{
}
