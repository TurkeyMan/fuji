#include "Common.h"
#include "Heap.h"

Heap heapList[MAX_HEAP_COUNT];
Resource resourceList[MAX_RESOURCES];

Heap *pCurrentHeap = NULL;

void Heap_InitModule()
{
	CALLSTACK;

	memset(heapList, 0, sizeof(Heap) * MAX_HEAP_COUNT);
	memset(resourceList, 0, sizeof(Resource) * MAX_RESOURCES);

	Heap *pHeap = CreateHeap(8*1024*1024, "GlobalHeap");
	pCurrentHeap = pHeap;
}

void Heap_DeinitModule()
{
	CALLSTACK;

	for(int a=0; a<MAX_HEAP_COUNT; a++)
	{
		FreeHeap(&heapList[a]);
	}
}

Heap* CreateHeap(uint32 size, char *name)
{
	CALLSTACK;

	for(int a=0; a<MAX_HEAP_COUNT; a++) if(!heapList[a].pHeap) break;

	DBGASSERT(a<MAX_HEAP_COUNT, "Exceeded MAX_HEAP_COUNT heap's");

	Heap *pHeap = &heapList[a];

	pHeap->pAllocPointer = pHeap->pHeap = (char*)malloc(ALIGN16(size));
	pHeap->heapSize = size;

	pHeap->markCount = 0;

#if !defined(_RETAIL)
	strcpy(pHeap->heapName, name);

	pHeap->allocCount = 0;
#endif

	return pHeap;
}

void FreeHeap(Heap *pHeap)
{
	CALLSTACK;

#if !defined(_RETAIL)
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

	free(pHeap->pHeap);
	pHeap->pHeap = NULL;
}

Resource* CreateResource(uint32 size, uint32 type = RES_Unknown)
{
	CALLSTACK;

	size = ALIGN16(size);

	for(int a=0; a<MAX_RESOURCES; a++) if(!resourceList[a].pData) break;

	DBGASSERT(a<MAX_RESOURCES, "Exceeded MAX_RESOURCES resources's");

	Resource *pRes = &resourceList[a];

	pRes->pData = Heap_Alloc(size);
	pRes->bytes = size;
	pRes->resourceType = type;

	return pRes;
}

void ReleaseResource(Resource *pResource)
{
	CALLSTACK;

	Heap_Free(pResource->pData);
	pResource->pData = NULL;
}

void MarkHeap(Heap *pHeap)
{
	CALLSTACK;

	pHeap->markStack[pHeap->markCount] = pHeap->pAllocPointer;
#if !defined(_RETAIL)
	pHeap->markAlloc[pHeap->markCount] = pHeap->allocCount;
#endif
	pHeap->markCount++;
}

void ReleaseMark(Heap *pHeap)
{
	CALLSTACK;

	// list unfree'd alloc's

	pHeap->markCount--;
#if !defined(_RETAIL)
	pHeap->allocCount = pHeap->markAlloc[pHeap->markCount];
#endif
	pHeap->pAllocPointer = pHeap->markStack[pHeap->markCount];
}

void SetCurrentHeap(Heap *pHeap)
{
	pCurrentHeap = pHeap;
}

#if !defined(_RETAIL)
void *Heap_Alloc(uint32 bytes, char *pFile, uint32 line)
#else
void *Heap_Alloc(uint32 bytes)
#endif
{
	CALLSTACK;

	char *pMem = pCurrentHeap->pAllocPointer;
	pCurrentHeap->pAllocPointer += ALIGN16(bytes);

	pMem = (char*)malloc(bytes+16);

	char offset = 16 - ((uint32)pMem & 0xF);
	pMem += offset;
	pMem[-1] = offset;

#if !defined(_RETAIL)
	DBGASSERT(pCurrentHeap->allocCount < MAX_ALLOC_COUNT, "Exceeded alloc count!");
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pAddress = pMem;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].bytes = ALIGN16(bytes);
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pFilename = pFile;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].lineNumber = line;
	pCurrentHeap->allocCount++;
#endif

	ASSERT_ALLIGN16(pMem);

	return pMem;
}

#if !defined(_RETAIL)
void *Heap_Realloc(void *pMem, uint32 bytes, char *pFile, uint32 line)
#else
void *Heap_Realloc(void *pMem, uint32 bytes)
#endif
{
	CALLSTACK;

#if !defined(_RETAIL)
	void *pNew = Heap_Alloc(ALIGN16(bytes), pFile, line);
#else
	void *pNew = Heap_Alloc(ALIGN16(bytes));
#endif

	// ummmmm... yeah need to keep record of what is allocated where... 
	memcpy(pNew, pMem, min(bytes, bytes));

	return pNew;
}

void Heap_Free(void *pMem)
{
	CALLSTACK;

#if !defined(_RETAIL)
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

	(char*&)pMem -= ((char*)pMem)[-1];

	free(pMem);
}

#if !defined(_RETAIL)
template<class T>
T* Managed_New(T *pT, char *pFile, uint32 line)
{
#else
template<class T>
T* Unmanaged_New(T *pT)
{
#endif
#if !defined(_RETAIL)
	DBGASSERT(pCurrentHeap->allocCount < MAX_ALLOC_COUNT, "Exceeded alloc count!");
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pAddress = pT;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].bytes = sizeof(T);
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
#if !defined(_RETAIL)
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
