#include "Common.h"

void *Heap_Alloc(uint32 bytes)
{
	return malloc(bytes);
}

void *Heap_Realloc(uint32 bytes)
{
	return malloc(bytes);
}

void Heap_Free(void *pMem)
{

}

void Heap_New()
{

}

void Heap_Delete(void *pMem)
{

}

void *Heap_NewArray(int arraySize)
{
	return NULL;
}

void *Heap_RenewArray(int arraySize)
{
	return NULL;
}

void Heap_DeleteArray(void *pMem)
{

}
