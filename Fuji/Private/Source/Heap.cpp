#include "Common.h"

Heap *pCurrentHeap = NULL;

Heap gDefaultHeap;

void Heap_InitModule()
{
	CreateHeap(&gDefaultHeap, 48*1024*1024, "GlobalHeap");

	pCurrentHeap = &gDefaultHeap;
}

void Heap_DeinitModule()
{
	FreeHeap(&gDefaultHeap);
}

void CreateHeap(Heap *pHeap, uint32 size, char *name)
{
//	pHeap->pAllocPointer = pHeap->pHeap = malloc(size);
	pHeap->heapSize = size;

	pHeap->markCount = 0;

#if defined(_DEBUG)
	strcpy(pHeap->heapName, name);

	pHeap->allocCount = 0;
#endif
}

void FreeHeap(Heap *pHeap)
{
#if defined(_DEBUG)
	if(pHeap->allocCount)
	{
		uint32 total = 0;

		LOGD(STR("\nFreeHeap(): %d allocations were not freed:\n-----------------------------------------\n", pHeap->allocCount));

		for(uint32 a=0; a<pHeap->allocCount; a++)
		{
			LOGD(STR("%s(%d) : Address: 0x%08X, %d bytes.", pHeap->allocList[a].pFilename, pHeap->allocList[a].lineNumber, pHeap->allocList[a].pAddress, pHeap->allocList[a].bytes));
			total += pHeap->allocList[a].bytes;
		}

		LOGD(STR("\nTotal: %d bytes unfreed\n", total));
	}
#endif
}

void MarkHeap(Heap *pHeap)
{
	pHeap->markStack[pHeap->markCount] = pHeap->pAllocPointer;
#if defined(_DEBUG)
	pHeap->markAlloc[pHeap->markCount] = pHeap->allocCount;
#endif
	pHeap->markCount++;
}

void ReleaseMark(Heap *pHeap)
{

}

void SetCurrentHeap(Heap *pHeap)
{
	pCurrentHeap = pHeap;
}

#if defined(_DEBUG)
void *Managed_Alloc(uint32 bytes, char *pFile, uint32 line)
#else
void *Unmanaged_Alloc(uint32 bytes)
#endif
{
	CALLSTACKs("Heap_Alloc");

	void *pMem = malloc(bytes);

#if defined(_DEBUG)
	DBGASSERT(pCurrentHeap->allocCount < MAX_ALLOC_COUNT, "Exceeded alloc count!");
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pAddress = pMem;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].bytes = bytes;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pFilename = pFile;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].lineNumber = line;
	pCurrentHeap->allocCount++;
#endif

	return pMem;
}

/*
#if defined(_DEBUG)
void *Heap_Realloc(void *pMem, uint32 bytes, char *pFile, uint32 line)
#else
void *Heap_Realloc(uint32 bytes)
#endif
{
	void *pNew = malloc(bytes);

	memcpy(pNew, pMem, 
	return malloc(bytes);
}
*/

void Heap_Free(void *pMem)
{
	CALLSTACK;

#if defined(_DEBUG)
	for(uint32 a=pCurrentHeap->allocCount-1; a>=0; a--)
	{
		if(pCurrentHeap->allocList[a].pAddress == pMem)
		{
			pCurrentHeap->allocCount--;
			pCurrentHeap->allocList[a] = pCurrentHeap->allocList[pCurrentHeap->allocCount];
		}
	}

	DBGASSERT(a >= 0, STR("Memory not allocated at address: 0x%08X.", pMem));
#endif

	free(pMem);
}

#if defined(_DEBUG)
void* Managed_New(void *pT, char *pFile, uint32 line)
#else
void* Unmanaged_New(void *pT)
#endif
{
#if defined(_DEBUG)
	DBGASSERT(pCurrentHeap->allocCount < MAX_ALLOC_COUNT, "Exceeded alloc count!");
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pAddress = pMem;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].bytes = bytes;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pFilename = pFile;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].lineNumber = line;
	pCurrentHeap->allocCount++;
#endif

	return pT;
}

template<class T>
void Heap_Delete(T *pObject)
{
	delete pObject;
}

/*
template<class T>
#if defined(_DEBUG)
T* Heap_NewArray(int arraySize, char *pFile, uint32 line)
#else
T* Heap_NewArray(int arraySize)
#endif
{
	return new T[arraySize];
}

template<class T>
void Heap_DeleteArray(T *pArray)
{
	delete[] pArray;
}
*/