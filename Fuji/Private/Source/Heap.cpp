#include "Common.h"
#include "Heap.h"

#define MAX_HEAP_COUNT 16

Heap *gpHeapList[MAX_HEAP_COUNT];
Heap *pCurrentHeap = NULL;

PtrListDL<Resource> gResourceList;

void *malloc_aligned(size_t bytes)
{
	char *new_buffer;

	new_buffer = (char*)malloc(bytes+16);

	// make allocation 16 byte alligned
	char offset = 16 - ((uint32)new_buffer & 0xF);
	new_buffer += offset;
	new_buffer[-1] = offset;

	return new_buffer;
}

void *realloc_aligned(void *buffer, size_t bytes)
{
	void *new_buffer = malloc_aligned(ALIGN16(bytes));

	// ummmmm... yeah need to keep record of what is allocated where... 
	memcpy(new_buffer, buffer, Min(bytes, bytes));

	free_aligned(buffer);

	return new_buffer;
}

void free_aligned(void *buffer)
{
	// used to ensure 16 byte allignment
	(char*&)buffer -= ((char*)buffer)[-1];

	free(buffer);
}


void Heap_InitModule()
{
	CALLSTACK;

	memset(gpHeapList, 0, sizeof(Heap*) * MAX_HEAP_COUNT);

	gResourceList.Init("Heap Resource List", gDefaults.heap.maxResources);

	// probably want to create the default heap here...
	// but we'll let the system manage the memory for the time being
	// all currently supports systems have memory managers
}

void Heap_DeinitModule()
{
	CALLSTACK;

	for(int a=0; a<MAX_HEAP_COUNT; a++)
	{
		if(gpHeapList[a])
			gpHeapList[a]->Release();
	}
}

Heap* Heap_CreateHeap(uint32 size, HeapType type, char *name)
{
	CALLSTACK;

	// find an empty heap pointer
	int heapIndex;
	for(heapIndex=0; heapIndex<MAX_HEAP_COUNT && gpHeapList[heapIndex]; heapIndex++);
	DBGASSERT(heapIndex < MAX_HEAP_COUNT, "Exceeded MAX_HEAP_COUNT heap's");

	// only 16 byte alligned blocks can be allocated
	size = ALIGN16(size);

	// calculate the offset of the start of the heap memory
	uint32 heapStart;

	switch(type)
	{
	case HEAP_Static:
		heapStart = ALIGN16(sizeof(StaticHeap)) + ALIGN16(sizeof(char*) * gDefaults.heap.maxStaticMarkers);

	default:
		DBGASSERT(0, "Unknown Heap Type");
	}

	// FIXME: i think you need to call new on a class with a vf-table?
	// will need a placement new or something.... :/
	Heap *pHeap = (Heap*)malloc_aligned(size + heapStart);

	// fill heap structure
	pHeap->pHeap			= (char*)pHeap + heapStart;
	pHeap->pAllocPointer	= (char*)pHeap + heapStart;
	pHeap->heapSize			= size;
	pHeap->heapType			= type;

#if !defined(_RETAIL)
	strcpy(pHeap->heapName, name);
#endif

	// fill heap type specific data
	switch(type)
	{
	case HEAP_Static:
		StaticHeap *pStaticHeap = (StaticHeap*)pHeap;

		pStaticHeap->markStack = (char**)pHeap + ALIGN16(sizeof(StaticHeap));
		pStaticHeap->markCount = 0;
	}

	// add to list
	gpHeapList[heapIndex] = pHeap;

	// return heap
	return (Heap*)pHeap;
}

Resource* Heap_CreateResource(uint32 size)
{
	CALLSTACK;
/*
	DBGASSERT(pCurrentHeap->heapType == HEAP_Defrag, "Can't allocate resource");

	size = ALIGN16(size);

	int a;
	for(a=0; a<MAX_RESOURCES; a++) if(!resourceList[a].pData) break;

	DBGASSERT(a<MAX_RESOURCES, "Exceeded MAX_RESOURCES resources's");

	Resource *pRes = &resourceList[a];

	pRes->pData = Heap_Alloc(size);
	pRes->bytes = size;
	pRes->resourceType = type;

	return pRes;
*/
	return NULL;
}

void Heap_ReleaseResource(Resource *pResource)
{
	CALLSTACK;
/*
	Heap_Free(pResource->pData);
	pResource->pData = NULL;
*/
}

void Heap_MarkHeap()
{
	CALLSTACK;
/*
	pHeap->markStack[pHeap->markCount] = pHeap->pAllocPointer;
#if !defined(_RETAIL)
	pHeap->markAlloc[pHeap->markCount] = pHeap->allocCount;
#endif
	pHeap->markCount++;
*/
}

void Heap_ReleaseMark()
{
	CALLSTACK;

	// list unfree'd alloc's
/*
	pHeap->markCount--;
#if !defined(_RETAIL)
	pHeap->allocCount = pHeap->markAlloc[pHeap->markCount];
#endif
	pHeap->pAllocPointer = pHeap->markStack[pHeap->markCount];
*/
}

Heap *Heap_SetCurrentHeap(Heap *pHeap)
{
	Heap *pTemp = pCurrentHeap;
	pCurrentHeap = pHeap;
	return pTemp;
}

Heap *Heap_GetCurrentHeap()
{
	return pCurrentHeap;
}

void Heap_PushGroupName(const char *pGroupName)
{
	// TODO: push group name into group stack
	// new allocation will reference this group name for identification
}

void Heap_PopGroupName()
{

}

#if !defined(_RETAIL)
void *Heap_Alloc(uint32 bytes, char *pFile, uint32 line)
#else
void *Heap_Alloc(uint32 bytes)
#endif
{
	CALLSTACK;

	char *pMem;

	if(pCurrentHeap)
	{
		pMem = (char*)pCurrentHeap->Alloc(bytes);
	}
	else
	{
        pMem = (char*)malloc_aligned(bytes);
	}
/*
#if !defined(_RETAIL)
	DBGASSERT(pCurrentHeap->allocCount < MAX_ALLOC_COUNT, "Exceeded alloc count!");
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pAddress = pMem;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].bytes = ALIGN16(bytes);
	pCurrentHeap->allocList[pCurrentHeap->allocCount].pFilename = pFile;
	pCurrentHeap->allocList[pCurrentHeap->allocCount].lineNumber = line;
	pCurrentHeap->allocCount++;
#endif
*/
	return pMem;
}

#if !defined(_RETAIL)
void *Heap_Realloc(void *pMem, uint32 bytes, char *pFile, uint32 line)
#else
void *Heap_Realloc(void *pMem, uint32 bytes)
#endif
{
	CALLSTACK;

	if(pCurrentHeap)
	{
		pMem = (char*)pCurrentHeap->Realloc(pMem, bytes);
	}
	else
	{
        pMem = (char*)realloc_aligned(pMem, bytes);
	}

	return pMem;
}

void Heap_Free(void *pMem)
{
	CALLSTACK;
/*
#if !defined(_RETAIL)
	uint32 a;
	for(a=pCurrentHeap->allocCount-1; a>=0; a--)
	{
		if(pCurrentHeap->allocList[a].pAddress == pMem)
		{
			pCurrentHeap->allocCount--;
			pCurrentHeap->allocList[a] = pCurrentHeap->allocList[pCurrentHeap->allocCount];
		}
	}

	DBGASSERT(a >= 0, STR("Memory not allocated at address: 0x%08X.", pMem));
#endif
*/
	if(pCurrentHeap)
	{
		pCurrentHeap->Free(pMem);
	}
	else
	{
        free_aligned(pMem);
	}
}

void *Heap_TAlloc(uint32 bytes)
{
	return malloc_aligned(bytes);
}

void Heap_TFree(void *pMem)
{
	free_aligned(pMem);
}


/*** Static Heap ***/

void *StaticHeap::Alloc(uint32 bytes)
{
	// TODO: static heap alloc
	return NULL;
}

void *StaticHeap::Realloc(void *pBuffer, uint32 bytes)
{
	// TODO: static heap realloc
	return NULL;
}

void StaticHeap::Free(void *pBuffer)
{
	// TODO: static heap free
}

void StaticHeap::Release()
{
	CALLSTACK;

#if !defined(_RETAIL)
	// list unfreed allocations
/*
	if(pHeap->allocCount)
	{
		uint32 total = 0;

		LOGD(STR("\StaticHeap::Release(): %d allocations were not freed in heap '%s':\n-----------------------------------------\n", allocCount, heapName));

		for(uint32 a=0; a<pHeap->allocCount; a++)
		{
			LOGD(STR("%s(%d) : Address: 0x%08X, %d bytes.", pHeap->allocList[a].pFilename, pHeap->allocList[a].lineNumber, pHeap->allocList[a].pAddress, pHeap->allocList[a].bytes));
			total += pHeap->allocList[a].bytes;
		}

		LOGD(STR("\nTotal: %d bytes unfreed\n", total));
	}
*/
#endif

	// find and delete heap pointer
	int heapIndex;
	for(heapIndex=0; heapIndex<MAX_HEAP_COUNT && gpHeapList[heapIndex] != (Heap*)this; heapIndex++);

	DBGASSERT(heapIndex < MAX_HEAP_COUNT, "Heap not found");

	gpHeapList[heapIndex] = NULL;

	// FIXME: i think i need to call delete on a class with a vf-table?
	free_aligned(this);
}
