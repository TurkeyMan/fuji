#include "Fuji.h"
#include "MFHeap_Internal.h"
#include "MFThread_Internal.h"

#define _USE_TRACKING_HASH_TABLE

/*** Globals ***/

static int heapAlignment = 32;

// external heap
static void* ExternalMalloc(size_t bytes, void *pUserData)
{
	return MFHeap_SystemMalloc(bytes);
}

static void ExternalFree(void *pMemory, void *pUserData)
{
	MFHeap_SystemFree(pMemory);
}

static MFMemoryCallbacks gExternalCallbacks =
{
	ExternalMalloc,
	ExternalFree
};

static MFHeap gExternalHeap =
{
	&gExternalCallbacks,
	NULL,
	NULL,
	MFHT_External,
	0, 0, 0,
#if !defined(_RETAIL)
	"External Heap"
#endif
};

static MFHeap gDebugHeap =
{
	&gExternalCallbacks,
	NULL,
	NULL,
	MFHT_Debug,
	0, 0, 0,
#if !defined(_RETAIL)
	"Debug Heap"
#endif
};

// heap pointers
static MFHeap *pActiveHeap = &gExternalHeap;
static MFHeap *pOverrideHeap = NULL;

static MFHeap *pDebugHeap = &gDebugHeap;

static char gMutexBuffer[72];
static MFMutex gAllocMutex = NULL;

// custom heap
static MFMemoryCallbacks gCustomCallbacks =
{
	NULL,
	NULL
};

static MFHeap gCustomHeap =
{
	&gCustomCallbacks,
	NULL,
	NULL,
	MFHT_Custom,
	0, 0, 0,
#if !defined(_RETAIL)
	"Custom Heap"
#endif
};

#if !defined(_RETAIL)
	static const int MFHeap_MungwallBytes = 8;
	static const char *gMungwall = "mungwall";

	#define USE_PRE_MUNGWALL
	static const char *gLlawgnum = "llawgnum";
#else
	static const int MFHeap_MungwallBytes 0
#endif

// allocation structure. stored immediately before any fuji allocation to identify the heap it was allocated with, and verify memory integrity.
struct MFAllocHeader
{
	MFHeap *pHeap;
	size_t size;
	const char *pFile;
	size_t alignment	: 16;
	size_t line			: sizeof(size_t)*8 - 16;
#if defined(USE_PRE_MUNGWALL) && !defined(_USE_TRACKING_HASH_TABLE)
	char llawgnum[MFHeap_MungwallBytes];
#endif
};

#if defined(_USE_TRACKING_HASH_TABLE)
	static void AllocateAllocTable();
	static void ExpandAllocTable();
	static void FreeAllocTable();
	static MFAllocHeader *AddAlloc(void *pMemory, size_t bytes, const char *pDesc);
	static void FreeAlloc(void *pMemory);
#endif

/*** heap tracker ***/

static int gMFHeap_TrackerLine = 0;
static const char *gpMFHeap_TrackerFile = "Unknown in RETAIL build";

MF_API void MFHeap_SetLineAndFile(int line, const char *pFile)
{
	gMFHeap_TrackerLine = line;
	gpMFHeap_TrackerFile = pFile;
}


/*** functions ***/

MFInitStatus MFHeap_InitModule()
{
	MFCALLSTACK;

	MFDebug_Assert(MFThread_GetMutexSizePlatformSpecific() <= sizeof(gMutexBuffer), "Mutex buffer too small!");
	MFThread_InitMutexPlatformSpecific((MFMutex)gMutexBuffer, "MFHeap alloc mutex");
	gAllocMutex = (MFMutex)gMutexBuffer;

	// any heap tracking allocations should be made in the debug space
	MFHeap *pOld = MFHeap_SetActiveHeap(MFHeap_GetDebugHeap());

#if defined(_USE_TRACKING_HASH_TABLE)
	AllocateAllocTable();
#endif

	// restore the active heap
	MFHeap_SetActiveHeap(pOld);

	// init the heap
	MFHeap_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFHeap_DeinitModule()
{
	MFCALLSTACK;

	MFHeap_DeinitModulePlatformSpecific();

#if defined(_USE_TRACKING_HASH_TABLE)
	// todo: list all unfreed allocations?
	//...

	FreeAllocTable();
#endif

	// TODO: gAllocMutex is not allocated by MFHeap... separate the 'deinit' from the 'destroy'
//	MFThread_DestroyMutex(gAllocMutex);
}

#if defined(_USE_TRACKING_HASH_TABLE)
static MFAllocHeader *GetAllocHeader(const void *pMem);
#else
__forceinline MFAllocHeader *GetAllocHeader(const void *pMem)
{
	return &((MFAllocHeader*)pMem)[-1];
}
#endif

__forceinline size_t GetAllocSize(size_t bytes, size_t &extra)
{
#if defined(_USE_TRACKING_HASH_TABLE)
	#if defined(USE_PRE_MUNGWALL)
		size_t preAllocBytes = MFHeap_MungwallBytes;
	#else
		size_t preAllocBytes = 0;
	#endif
#else
	size_t preAllocBytes = sizeof(MFAllocHeader);
#endif

	size_t pad = 0;
	while(pad < preAllocBytes)
		pad += heapAlignment;

#if defined(_USE_TRACKING_HASH_TABLE)
	#if defined(USE_PRE_MUNGWALL)
		size_t allocExtra = pad + MFHeap_MungwallBytes*2; // the pre-mungwall will not be included in the header
	#else
		size_t allocExtra = pad + MFHeap_MungwallBytes;
	#endif
#else
	size_t allocExtra = pad + sizeof(MFAllocHeader) + MFHeap_MungwallBytes;
#endif
	extra = allocExtra;
	return bytes + allocExtra;
}

MF_API void *MFHeap_AllocInternal(size_t bytes, MFHeap *pHeap)
{
	MFCALLSTACK;

	MFHeap *pAllocHeap = pOverrideHeap ? pOverrideHeap : (pHeap ? pHeap : pActiveHeap);

	size_t extra;
	size_t allocBytes = GetAllocSize(bytes, extra);

	MFThread_LockMutex(gAllocMutex);

	char *pMemory = (char*)pAllocHeap->pCallbacks->pMalloc(allocBytes, pAllocHeap->pHeapData);
	if(pMemory)
	{
#if defined(_USE_TRACKING_HASH_TABLE)
	#if defined(USE_PRE_MUNGWALL)
		int alignment = (int)(MFALIGN(pMemory + MFHeap_MungwallBytes, heapAlignment) - (uintp)pMemory);
	#else
		int alignment = (int)(MFALIGN(pMemory, heapAlignment) - (uintp)pMemory);
	#endif
#else
		int alignment = (int)(MFALIGN(pMemory + sizeof(MFAllocHeader), heapAlignment) - (uintp)pMemory);
#endif

		pMemory += alignment;

#if defined(_USE_TRACKING_HASH_TABLE)
		MFAllocHeader *pHeader = AddAlloc(pMemory, bytes, NULL);
#else
		MFAllocHeader *pHeader = GetAllocHeader(pMemory);
#endif
		pHeader->alignment = alignment;
		pHeader->pHeap = pAllocHeap;
		pHeader->size = bytes;
		pHeader->pFile = gpMFHeap_TrackerFile;
		pHeader->line = gMFHeap_TrackerLine;

#if defined(USE_PRE_MUNGWALL)
		MFCopyMemory(pMemory - MFHeap_MungwallBytes, gLlawgnum, MFHeap_MungwallBytes);
#endif
#if !defined(_RETAIL)
		MFCopyMemory(pMemory + bytes, gMungwall, MFHeap_MungwallBytes);

		pAllocHeap->totalAllocated += allocBytes;
		pAllocHeap->totalWaste += extra;
		++pAllocHeap->allocCount;

//		MFDebug_Log(2, MFStr("Alloc: %08X(%08X), %d bytes - %s:(%d)", pMemory, pMemory - pHeader->alignment, bytes, gpMFHeap_TrackerFile, gMFHeap_TrackerLine));
#endif
	}

	MFThread_ReleaseMutex(gAllocMutex);

	return (void*)pMemory;
}

MF_API void* MFHeap_AllocAndZeroInternal(size_t bytes, MFHeap *pHeap)
{
	void *pMem = MFHeap_AllocInternal(bytes, pHeap);
	if(pMem)
		MFZeroMemory(pMem, bytes);
	return pMem;
}

MF_API void *MFHeap_ReallocInternal(void *pMem, size_t bytes)
{
	MFCALLSTACK;

	if(pMem)
	{
		MFAllocHeader *pHeader = GetAllocHeader(pMem);
		MFDebug_Assert(MFHeap_ValidateMemory(pMem), MFStr("Memory corruption detected!!\n%s(%d)", pHeader->pFile, pHeader->line));

		void *pNew = MFHeap_AllocInternal(bytes, pHeader->pHeap);
		MFDebug_Assert(pNew, "Failed to allocate memory!");
		if(!pNew)
			return NULL;

		MFCopyMemory(pNew, pMem, MFMin(bytes, pHeader->size));
		MFHeap_Free(pMem);
		return pNew;
	}
	else
	{
		return MFHeap_AllocInternal(bytes, NULL);
	}
}

MF_API void MFHeap_Free(void *pMem)
{
	MFCALLSTACK;

	if(!pMem)
	{
		MFDebug_Warn(3, "Attemptd to Free 'NULL' pointer.");
		return;
	}

	MFThread_LockMutex(gAllocMutex);

	MFAllocHeader *pHeader = GetAllocHeader(pMem);
	MFDebug_Assert(MFHeap_ValidateMemory(pMem), MFStr("Memory corruption detected!!\n%s(%d)", pHeader->pFile, pHeader->line));

	MFHeap *pHeap = pHeader->pHeap;
#if !defined(_RETAIL)
	size_t extra;
	size_t allocBytes = GetAllocSize(pHeader->size, extra);

	pHeap->totalAllocated -= allocBytes;
	pHeap->totalWaste -= extra;
	--pHeap->allocCount;

//	MFDebug_Log(2, MFStr("Free: %08X, %d bytes - %s:(%d)", pMem, pHeader->size, pHeader->pFile, (int)pHeader->line));
#endif

	MFCopyMemory((char*)pMem + pHeader->size, "freefreefreefree", MFHeap_MungwallBytes);
#if defined(USE_PRE_MUNGWALL)
	MFCopyMemory((char*)pMem - MFHeap_MungwallBytes, "eerfeerfeerfeerf", MFHeap_MungwallBytes);
#endif
	MFMemSet(pMem, 0xFE, pHeader->size);

	pHeap->pCallbacks->pFree((char*)pMem - pHeader->alignment, pHeap->pHeapData);

#if defined(_USE_TRACKING_HASH_TABLE)
	FreeAlloc(pMem);
#endif

	MFThread_ReleaseMutex(gAllocMutex);
}

// #if !(defined(_FUJI_UTIL) && defined(_LINUX)) && !defined(MF_GC)
// // new/delete operators
// void* operator new(size_t size)
// {
// //	MFDebug_Message(MFStr("new %d bytes", size));
//
// 	return MFHeap_AllocInternal(size);
// }
//
// void* operator new[](size_t size)
// {
// //	MFDebug_Message(MFStr("new %d bytes", size));
//
// 	return MFHeap_AllocInternal(size);
// }
//
// void operator delete(void *pMemory)
// {
// 	MFHeap_Free(pMemory);
// }
//
// void operator delete[](void *pMemory)
// {
// 	MFHeap_Free(pMemory);
// }
// #endif

#if !(defined(MF_WINDOWS) || defined(MF_XBOX)) && !defined(_FUJI_UTIL)
void* operator new(size_t size, void *pMem)
{
	return pMem;
}

void* operator new[](size_t size, void *pMem)
{
	return pMem;
}

void operator delete(void *pMemory, void *pMem)
{
}

void operator delete[](void *pMemory, void *pMem)
{
}
#endif

MF_API size_t MFHeap_GetTotalAllocated(MFHeap *pHeap)
{
	return pHeap->totalAllocated;
}

MF_API size_t MFHeap_GetTotalWaste(MFHeap *pHeap)
{
	return pHeap->totalWaste;
}

MF_API int MFHeap_GetNumAllocations(MFHeap *pHeap)
{
	return pHeap->allocCount;
}

// get the size of an allocation
MF_API size_t MFHeap_GetAllocSize(const void *pMemory)
{
	MFCALLSTACK;

	MFAllocHeader *pHeader = GetAllocHeader(pMemory);
	return pHeader->size;
}

// get the size of an allocation
MF_API MFHeap *MFHeap_GetAllocHeap(const void *pMemory)
{
	MFCALLSTACK;

	MFAllocHeader *pHeader = GetAllocHeader(pMemory);
	return pHeader->pHeap;
}

// get a heap pointer
MF_API MFHeap* MFHeap_GetHeap(MFHeapType heap)
{
	MFCALLSTACK;

	switch(heap)
	{
		case MFHT_Active:
			return pActiveHeap;
		case MFHT_ActiveTemporary:
			return pActiveHeap ? MFHeap_GetTempHeap(pActiveHeap) : NULL;
		case MFHT_Debug:
			return pDebugHeap;
		case MFHT_External:
			return &gExternalHeap;
		case MFHT_Custom:
			MFDebug_Assert(gCustomHeap.pCallbacks->pMalloc, "No custom heap registered!");
			return &gCustomHeap;
		default:
			break;
	}

	return NULL;
}

// gets the temp heap associated with a heap
MF_API MFHeap* MFHeap_GetTempHeap(MFHeap *pHeap)
{
	MFCALLSTACK;
	MFDebug_Assert(pHeap, "Invalid heap");

	return pHeap->pTempHeap ? pHeap->pTempHeap : pHeap;
}

MF_API MFHeap* MFHeap_GetDebugHeap()
{
	return pDebugHeap;
}

// set active heap, return the old active heap
MF_API MFHeap* MFHeap_SetActiveHeap(MFHeap *pHeap)
{
	MFHeap *pOld = pActiveHeap;
	pActiveHeap = pHeap ? pHeap : &gExternalHeap;
	return pOld;
}

// set allocation alignment, return old alignment
MF_API int MFHeap_SetAllocAlignment(int bytes)
{
	int oldAlignment = heapAlignment;
	heapAlignment = bytes < 4 ? 4 : bytes;
	return oldAlignment;
}

// push/pop a heap marker for static heaps
MF_API void MFHeap_Mark(MFHeap *pHeap)
{

}

MF_API void MFHeap_Release(MFHeap *pHeap)
{

}

// register a custom heap that can be used by the game
MF_API void MFHeap_RegisterCustomHeap(const MFMemoryCallbacks *pCallbacks, void *pUserData)
{
	MFCALLSTACK;

	*gCustomHeap.pCallbacks = *pCallbacks;
	gCustomHeap.pHeapData = pUserData;
}

// set override heap. any allocation operations are forced to use this override heap.
MF_API void MFHeap_SetHeapOverride(MFHeap *pHeap)
{
	pOverrideHeap = pHeap;
}

// validate a block of memory. returns false if memory had been corrupted.
MF_API bool MFHeap_ValidateMemory(const void *pMemory)
{
	MFCALLSTACK;

	if(!pMemory)
		return true;

	MFAllocHeader *pHeader = GetAllocHeader(pMemory);
	if(!pHeader)
	{
		MFDebug_Warn(0, MFStr("Missing allocation header for allocation 0x%08X.", pMemory));
		return false;
	}

#if defined(USE_PRE_MUNGWALL)
	if(MFMemCompare((const char*)pMemory - MFHeap_MungwallBytes, gLlawgnum, MFHeap_MungwallBytes) == 0)
#endif
	if(MFMemCompare((const char*)pMemory + pHeader->size, gMungwall, MFHeap_MungwallBytes) == 0)
		return true;

	MFDebug_Log(0, MFStr("%s(%d) : Corrupted mungwall detected in allocation 0x%08X.", pHeader->pFile, pHeader->line, pMemory));
	return false;
}

// memory allocation groups for profiling
MF_API void MFHeap_PushGroupName(const char *pGroupName)
{

}

MF_API void MFHeap_PopGroupName()
{

}

// implement the allocation hash table
#if defined(_USE_TRACKING_HASH_TABLE)
struct MFHeap_AllocItem
{
	MFAllocHeader header;
	void *pMemory;
	MFHeap_AllocItem *pNext;
};

static const int MFHeap_AllocTableLength = 1024;
static MFHeap_AllocItem *gpAllocTable[MFHeap_AllocTableLength];

size_t poolSize = 0;
size_t numFree = 0;
MFHeap_AllocItem **gppFreeList = NULL;
MFHeap_AllocItem *gpItemPool = NULL;

static void AllocateAllocTable()
{
	numFree = poolSize = 1024;

	gpItemPool = (MFHeap_AllocItem*)MFHeap_SystemMalloc(sizeof(MFHeap_AllocItem)*poolSize + sizeof(MFHeap_AllocItem*)*poolSize);
	gppFreeList = (MFHeap_AllocItem**)&gpItemPool[poolSize];

	for(size_t i = 0; i<poolSize; ++i)
		gppFreeList[i] = &gpItemPool[i];

	MFZeroMemory(gpAllocTable, sizeof(gpAllocTable));
}

static void ExpandAllocTable()
{
	size_t oldSize = poolSize;
	size_t numToExpand = poolSize;
	poolSize += numToExpand;

	// resize the pool
	MFHeap_AllocItem *pNewItemPool = (MFHeap_AllocItem*)MFHeap_SystemRealloc(gpItemPool, sizeof(MFHeap_AllocItem)*poolSize + sizeof(MFHeap_AllocItem*)*poolSize);
	MFHeap_AllocItem **ppNewFreeList = (MFHeap_AllocItem**)&pNewItemPool[poolSize];

	// calculate the pointer diff between the old and new pool
	ptrdiff_t diff = (char*)pNewItemPool - (char*)gpItemPool;

	// update the free list with all the new items
	for(size_t i=0; i<numFree; ++i)
		ppNewFreeList[i] = (MFHeap_AllocItem*)((char*)gppFreeList[i] + diff);
	for(size_t i = 0; i<numToExpand; ++i)
		ppNewFreeList[numFree + i] = &pNewItemPool[oldSize + i];
	numFree += numToExpand;

	// update all the pointers in the hashtable
	for(int i=0; i<MFHeap_AllocTableLength; ++i)
	{
		if(gpAllocTable[i])
		{
			gpAllocTable[i] = (MFHeap_AllocItem*)((char*)gpAllocTable[i] + diff);

			MFHeap_AllocItem *pI = gpAllocTable[i];
			while(pI->pNext)
			{
				pI->pNext = (MFHeap_AllocItem*)((char*)pI->pNext + diff);
				pI = pI->pNext;
			}
		}
	}

	// we're done!
	gpItemPool = pNewItemPool;
	gppFreeList = ppNewFreeList;
}

static void FreeAllocTable()
{
	MFHeap_SystemFree(gpItemPool);
}

static MFAllocHeader *AddAlloc(void *pMemory, size_t bytes, const char *pDesc)
{
	if(numFree == 0)
		ExpandAllocTable();

	MFHeap_AllocItem *pAlloc = gppFreeList[--numFree];
	pAlloc->pMemory = pMemory;

	int hash = MFUtil_HashPointer(pMemory) % MFHeap_AllocTableLength;
	pAlloc->pNext = gpAllocTable[hash];
	gpAllocTable[hash] = pAlloc;

	return &pAlloc->header;
}

static void FreeAlloc(void *pMemory)
{
	int hash = MFUtil_HashPointer(pMemory) % MFHeap_AllocTableLength;
	MFDebug_Assert(gpAllocTable[hash], "No allocation record! O_O");

	if(gpAllocTable[hash]->pMemory == pMemory)
	{
		gppFreeList[numFree++] = gpAllocTable[hash];
		gpAllocTable[hash] = gpAllocTable[hash]->pNext;
	}
	else
	{
		MFHeap_AllocItem *pI = gpAllocTable[hash];
		while(pI->pNext && pI->pNext->pMemory != pMemory)
			pI = pI->pNext;
		MFDebug_Assert(pI->pNext, "No allocation record! O_O");
		gppFreeList[numFree++] = pI->pNext;
		pI->pNext = pI->pNext->pNext;
	}
}

static MFAllocHeader *GetAllocHeader(const void *pMemory)
{
	int hash = MFUtil_HashPointer(pMemory) % MFHeap_AllocTableLength;

	MFHeap_AllocItem *pI = gpAllocTable[hash];
	while(pI && pI->pMemory != pMemory)
		pI = pI->pNext;
	if(!pI)
		return NULL;
	return &pI->header;
}
#endif

MF_API bool MFHeap_ValidateHeap()
{
#if defined(_USE_TRACKING_HASH_TABLE)
	MFThread_LockMutex(gAllocMutex);

	for(int i=0; i<MFHeap_AllocTableLength; ++i)
	{
		MFHeap_AllocItem *pI = gpAllocTable[i];
		while(pI)
		{
			if(!MFHeap_ValidateMemory(pI->pMemory))
				MFDebug_Assert(false, "Corrupt memory allocation!");

			pI = pI->pNext;
		}
	}

	MFThread_ReleaseMutex(gAllocMutex);
#endif
	return true;
}
