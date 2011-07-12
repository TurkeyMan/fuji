#include "Fuji.h"
#include "MFHeap_Internal.h"

//#define _USE_TRACKING_HASH_TABLE
#define _USE_ALLOC_TRACKER

/*** Structures ***/

#if defined(_USE_TRACKING_HASH_TABLE)
	#include "MFObjectPool.h"

	struct MFHeap_AllocItem
	{
		MFAllocHeader header;
		void *pMemory;
		MFHeap_AllocItem *pNext;
	};

	static const int MFHeap_AllocTableLength = 1024;
	static MFHeap_AllocItem *gpAllocTable[MFHeap_AllocTableLength];

	static MFObjectPool gAllocHeaderPool;
#endif
#if defined(_USE_ALLOC_TRACKER)
	#include "MFObjectPool.h"

	static MFObjectPool gAllocList;
	static bool gPoolInitialised = false;
#endif

/*** Globals ***/

static int heapAlignment = 32;

static size_t totalAllocated = 0;
static size_t totalWaste = 0;
static int allocCount = 0;

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
#if !defined(_RETAIL)
	"External Heap"
#endif
};

// heap pointers
static MFHeap *pActiveHeap = &gExternalHeap;
static MFHeap *pOverrideHeap = NULL;

static MFHeap *pDebugHeap = &gExternalHeap;

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
#if !defined(_RETAIL)
	"Custom Heap"
#endif
};

#if !defined(_RETAIL)
	#define MFHeap_MungwallBytes 8
	static const char gMungwall[9] = "mungwall";
#else
	#define MFHeap_MungwallBytes 0
#endif

/*** heap tracker ***/

static int gMFHeap_TrackerLine = 0;
static const char *gpMFHeap_TrackerFile = "Unknown in RETAIL build";

void MFHeap_SetLineAndFile(int line, const char *pFile)
{
	gMFHeap_TrackerLine = line;
	gpMFHeap_TrackerFile = pFile;
}


/*** functions ***/

void MFHeap_InitModule()
{
	MFCALLSTACK;

#if defined(_USE_TRACKING_HASH_TABLE)
	// set up the memory tracking hash table
	// note: this is slightly complicated due to the chicken and egg nature
	// of making the first allocation for the pool its self ;)
	static const int numAllocHeaders = 1024;
	uint32 bytes = (sizeof(MFHeap_AllocItem) + sizeof(void**)) * numAllocHeaders;
	void *pPoolMemory = MFHeap_SystemMalloc(bytes);
	gAllocHeaderPool.Init(sizeof(MFHeap_AllocItem), numAllocHeaders, 1024, pPoolMemory, bytes);

	MFZeroMemory(gpAllocTable, sizeof(gpAllocTable));

	// artificially add an entry for the pool its self...
	MFHeap_AllocItem *pHeader = (MFHeap_AllocItem*)gAllocHeaderPool.Alloc();
	pHeader->header.pHeap = &gExternalHeap;
	pHeader->header.size = bytes;
	pHeader->header.pFile = __FILE__;
	pHeader->header.line = (uint16)__LINE__;
	pHeader->header.alignment = 4;
	pHeader->pMemory = pPoolMemory;
	pHeader->pNext = NULL;
	gpAllocTable[MFUtil_HashPointer(pPoolMemory) % MFHeap_AllocTableLength] = pHeader;
#endif
#if defined(_USE_ALLOC_TRACKER)
	gAllocList.Init(sizeof(void*), 1024, 1024);
	gPoolInitialised = true;
#endif

	// init the heap
	MFHeap_InitModulePlatformSpecific();
}

void MFHeap_DeinitModule()
{
	MFCALLSTACK;

	MFHeap_DeinitModulePlatformSpecific();

#if defined(_USE_ALLOC_TRACKER)
	gPoolInitialised = false;
	gAllocList.Deinit();
#endif
#if defined(_USE_TRACKING_HASH_TABLE)
	// todo: list all unfreed allocations?
	//...

	gAllocHeaderPool.Deinit();
	MFZeroMemory(gpAllocTable, sizeof(gpAllocTable));
#endif
}

void *MFHeap_AllocInternal(size_t bytes, MFHeap *pHeap)
{
	MFCALLSTACK;

	MFHeap *pAllocHeap = pOverrideHeap ? pOverrideHeap : (pHeap ? pHeap : pActiveHeap);

	int pad = 0;
	while(pad < (int)sizeof(MFAllocHeader))
		pad += heapAlignment;

	size_t allocExtra = pad + sizeof(MFAllocHeader) + MFHeap_MungwallBytes;
	size_t allocBytes = bytes + allocExtra;

	char *pMemory = (char*)pAllocHeap->pCallbacks->pMalloc(allocBytes, pAllocHeap->pHeapData);
	MFDebug_Assert(pMemory, "Failed to allocate memory!");
	if(!pMemory)
		return NULL;

	int alignment = (int)(MFALIGN(pMemory + sizeof(MFAllocHeader), heapAlignment) - (uintp)pMemory);

	pMemory += alignment;

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMemory)[-1];

	pHeader->alignment = (uint16)alignment;
	pHeader->pHeap = pAllocHeap;
	pHeader->size = (uint32)bytes;
	pHeader->pFile = gpMFHeap_TrackerFile;
	pHeader->line = (uint16)gMFHeap_TrackerLine;

#if defined(_USE_TRACKING_HASH_TABLE)
	MFHeap_AllocItem *pAlloc = (MFHeap_AllocItem*)gAllocHeaderPool.Alloc();
	pAlloc->header.alignment = (uint16)alignment;
	pAlloc->header.pHeap = pAllocHeap;
	pAlloc->header.size = (uint32)bytes;
	pAlloc->header.pFile = gpMFHeap_TrackerFile;
	pAlloc->header.line = (uint16)gMFHeap_TrackerLine;
	pAlloc->pMemory = pMemory;

	int hash = MFUtil_HashPointer(pMemory) % MFHeap_AllocTableLength;
	pAlloc->pNext = gpAllocTable[hash];
	gpAllocTable[hash] = pAlloc;
#endif
#if defined(_USE_ALLOC_TRACKER)
	if(gPoolInitialised)
	{
		*(void**)gAllocList.Alloc() = pMemory;
	}
#endif

#if !defined(_RETAIL)
	MFCopyMemory(pMemory + bytes, gMungwall, MFHeap_MungwallBytes);

	totalAllocated += allocBytes;
	totalWaste += allocExtra;
	++allocCount;

	MFDebug_Log(2, MFStr("Alloc: %08X(%08X), %d bytes - %s:(%d)", pMemory, pMemory - pHeader->alignment, bytes, gpMFHeap_TrackerFile, gMFHeap_TrackerLine));
#endif

	return (void*)pMemory;
}

void* MFHeap_AllocAndZeroInternal(size_t bytes, MFHeap *pHeap)
{
	void *pMem = MFHeap_AllocInternal(bytes, pHeap);
	if(pMem)
		MFZeroMemory(pMem, bytes);
	return pMem;
}

void *MFHeap_ReallocInternal(void *pMem, size_t bytes)
{
	MFCALLSTACK;

	if(pMem)
	{
		MFAllocHeader *pHeader = &((MFAllocHeader*)pMem)[-1];
		MFDebug_Assert(MFHeap_ValidateMemory(pMem), MFStr("Memory corruption detected!!\n%s(%d)", pHeader->pFile, pHeader->line));

		void *pNew = MFHeap_AllocInternal(bytes, pHeader->pHeap);
		MFDebug_Assert(pNew, "Failed to allocate memory!");
		if(!pNew)
			return NULL;

		MFCopyMemory(pNew, pMem, MFMin(bytes, (size_t)pHeader->size));
		MFHeap_Free(pMem);
		return pNew;
	}
	else
	{
		return MFHeap_AllocInternal(bytes, NULL);
	}
}

void MFHeap_Free(void *pMem)
{
	MFCALLSTACK;

	if(!pMem)
	{
		MFDebug_Warn(3, "Attemptd to Free 'NULL' pointer.");
		return;
	}

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMem)[-1];
	MFDebug_Assert(MFHeap_ValidateMemory(pMem), MFStr("Memory corruption detected!!\n%s(%d)", pHeader->pFile, pHeader->line));

#if defined(_USE_TRACKING_HASH_TABLE)
	int hash = MFUtil_HashPointer(pMem) % MFHeap_AllocTableLength;
	MFHeap_AllocItem *pT = gpAllocTable[hash];
	if(pT)
	{
		if(pT->pMemory == pMem)
		{
			gpAllocTable[hash] = pT->pNext;
			gAllocHeaderPool.Free(pT);
		}
		else
		{
			while(pT->pNext && pT->pNext->pMemory != pMem)
				pT = pT->pNext;
			if(pT->pNext)
			{
				MFHeap_AllocItem *pTN = pT->pNext;
				pT->pNext = pTN->pNext;
				gAllocHeaderPool.Free(pTN);
			}
		}
	}
#endif
#if defined(_USE_ALLOC_TRACKER)
	if(gPoolInitialised)
	{
		int numAllocs = gAllocList.GetNumAllocated();

		for(int a=0; a<numAllocs; ++a)
		{
			void **ppAlloc = (void**)gAllocList.GetItem(a);
			if(*ppAlloc == pMem)
			{
				gAllocList.Free(ppAlloc);
				break;
			}
		}
	}
#endif

#if !defined(_RETAIL)
	int pad = 0;
	while(pad < (int)sizeof(MFAllocHeader))
		pad += heapAlignment;
	size_t extra = pad + sizeof(MFAllocHeader) + MFHeap_MungwallBytes;

	totalAllocated -= pHeader->size + extra;
	totalWaste -= extra;
	--allocCount;

	MFDebug_Log(2, MFStr("Free: %d bytes - %s:(%d)", pHeader->size, pHeader->pFile, (int)pHeader->line));
#endif

	MFHeap *pAllocHeap = pHeader->pHeap;

	pAllocHeap->pCallbacks->pFree((char*)pMem - pHeader->alignment, pAllocHeap->pHeapData);
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

size_t MFHeap_GetTotalAllocated()
{
	return totalAllocated;
}

size_t MFHeap_GetTotalWaste()
{
	return totalWaste;
}

int MFHeap_GetNumAllocations()
{
	return allocCount;
}

// get the size of an allocation
uint32 MFHeap_GetAllocSize(const void *pMemory)
{
	MFCALLSTACK;

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMemory)[-1];
	return pHeader->size;
}

// get a heap pointer
MFHeap* MFHeap_GetHeap(MFHeapType heap)
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
MFHeap* MFHeap_GetTempHeap(MFHeap *pHeap)
{
	MFCALLSTACK;
	MFDebug_Assert(pHeap, "Invalid heap");

	return pHeap->pTempHeap ? pHeap->pTempHeap : pHeap;
}

// set active heap, return the old active heap
MFHeap* MFHeap_SetActiveHeap(MFHeap *pHeap)
{
	MFHeap *pOld = pActiveHeap;
	pActiveHeap = pHeap ? pHeap : &gExternalHeap;
	return pOld;
}

// set allocation alignment, return old alignment
int MFHeap_SetAllocAlignment(int bytes)
{
	int oldAlignment = heapAlignment;
	heapAlignment = bytes < 4 ? 4 : bytes;
	return oldAlignment;
}

// push/pop a heap marker for static heaps
void MFHeap_Mark(MFHeap *pHeap)
{

}

void MFHeap_Release(MFHeap *pHeap)
{

}

// register a custom heap that can be used by the game
void MFHeap_RegisterCustomHeap(MFMemoryCallbacks *pCallbacks, void *pUserData)
{
	MFCALLSTACK;

	*gCustomHeap.pCallbacks = *pCallbacks;
	gCustomHeap.pHeapData = pUserData;
}

// set override heap. any allocation operations are forced to use this override heap.
void MFHeap_SetHeapOverride(MFHeap *pHeap)
{
	pOverrideHeap = pHeap;
}

// validate a block of memory. returns false if memory had been corrupted.
bool MFHeap_ValidateMemory(const void *pMemory)
{
	MFCALLSTACK;

	if(!pMemory)
		return true;

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMemory)[-1];
	return MFMemCompare((char*&)pMemory + pHeader->size, gMungwall, MFHeap_MungwallBytes) == 0;
}

bool MFHeap_ValidateHeap()
{
#if defined(_USE_ALLOC_TRACKER)
	if(gPoolInitialised)
	{
		int numAllocated = gAllocList.GetNumAllocated();
		for(int a=0; a<numAllocated; ++a)
		{
			void *pMem = *(void**)gAllocList.GetItem(a);
			if(!MFHeap_ValidateMemory(pMem))
			{
				MFDebug_Assert(false, "Corrupt memory allocation!");
				return false;
			}
		}
	}
#endif
	return true;
}

// memory allocation groups for profiling
void MFHeap_PushGroupName(const char *pGroupName)
{

}

void MFHeap_PopGroupName()
{

}
