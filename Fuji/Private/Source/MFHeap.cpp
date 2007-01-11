#include "Fuji.h"
#include "MFHeap_Internal.h"
#include "MFPtrList.h"

/*** globals ***/

static int heapAlignment = 32;

// external heap
static void* ExternalMalloc(uint32 bytes, void *pUserData)
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

#define MFHeap_MungwallBytes 8
static const char gMungwall[9] = "mungwall";

/*** heap tracker ***/

static int gMFHeap_TrackerLine = 0;
static const char *gpMFHeap_TrackerFile = "Unknown in RETAIL build";

void MFHeap_SetLineAndFile(int line, char *pFile)
{
	gMFHeap_TrackerLine = line;
	gpMFHeap_TrackerFile = pFile;
}


/*** functions ***/

void MFHeap_InitModule()
{
	MFCALLSTACK;

	MFHeap_InitModulePlatformSpecific();
}

void MFHeap_DeinitModule()
{
	MFCALLSTACK;

	MFHeap_DeinitModulePlatformSpecific();
}

void *MFHeap_AllocInternal(uint32 bytes, MFHeap *pHeap)
{
	MFCALLSTACK;

	MFHeap *pAllocHeap = pOverrideHeap ? pOverrideHeap : (pHeap ? pHeap : pActiveHeap);

	int pad = 0;
	while(pad < (int)sizeof(MFAllocHeader))
		pad += heapAlignment;

	char *pMemory = (char*)pAllocHeap->pCallbacks->pMalloc(bytes + pad + sizeof(MFAllocHeader) + MFHeap_MungwallBytes, pAllocHeap->pHeapData);

	int alignment = (int)((uint32)MFALIGN(pMemory + sizeof(MFAllocHeader), heapAlignment) - (uint32&)pMemory);

	pMemory += alignment;

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMemory)[-1];

	pHeader->alignment = (uint16)alignment;
	pHeader->pHeap = pAllocHeap;
	pHeader->size = bytes;
	pHeader->pFile = gpMFHeap_TrackerFile;
	pHeader->line = (uint16)gMFHeap_TrackerLine;

#if !defined(_RETAIL)
	MFCopyMemory(pMemory + bytes, gMungwall, MFHeap_MungwallBytes);
#endif

	return (void*)pMemory;
}

void *MFHeap_ReallocInternal(void *pMem, uint32 bytes)
{
	MFCALLSTACK;

	MFAllocHeader *pHeader = &((MFAllocHeader*)pMem)[-1];
	MFDebug_Assert(MFHeap_ValidateMemory(pMem), MFStr("Memory corruption detected!!\n%s(%d)", pHeader->pFile, pHeader->line));

	MFHeap *pAllocHeap = pHeader->pHeap;

	void *pNew = MFHeap_AllocInternal(bytes, pAllocHeap);

	MFCopyMemory(pNew, pMem, MFMin(bytes, pHeader->size));

	MFHeap_Free(pMem);

	return pNew;
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

	MFHeap *pAllocHeap = pHeader->pHeap;

	pAllocHeap->pCallbacks->pFree((char*)pMem - pHeader->alignment, pAllocHeap->pHeapData);
}

#if !(defined(_FUJI_UTIL) && defined(_LINUX))
// new/delete operators
void* operator new(size_t size)
{
//	MFDebug_Message(MFStr("new %d bytes", size));

	return MFHeap_AllocInternal((uint32)size);
}

void* operator new[](size_t size)
{
//	MFDebug_Message(MFStr("new %d bytes", size));

	return MFHeap_AllocInternal((uint32)size);
}

void operator delete(void *pMemory)
{
	MFHeap_Free(pMemory);
}

void operator delete[](void *pMemory)
{
	MFHeap_Free(pMemory);
}
#endif

#if !defined(_WINDOWS) && !defined(_FUJI_UTIL) 
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

// memory allocation groups for profiling
void MFHeap_PushGroupName(const char *pGroupName)
{

}

void MFHeap_PopGroupName()
{

}
