#if !defined(_MFHEAP_INTERNAL_H)
#define _MFHEAP_INTERNAL_H

#include "MFHeap.h"

// heap module init/deinit
MFInitStatus MFHeap_InitModule(int moduleId, bool bPerformInitialisation);
void MFHeap_DeinitModule();

void MFHeap_InitModulePlatformSpecific();
void MFHeap_DeinitModulePlatformSpecific();

// system allocation/deallocation
void* MFHeap_SystemMalloc(size_t bytes);
void* MFHeap_SystemRealloc(void *pBuffer, size_t bytes);
void MFHeap_SystemFree(void *pBuffer);

// heap structure
struct MFHeap
{
	MFMemoryCallbacks *pCallbacks;
	void *pHeapData;
	MFHeap *pTempHeap;

	MFHeapType heapType;

	size_t totalAllocated;
	size_t totalWaste;
	int allocCount;

#if !defined(_RETAIL)
	char heapName[32];
#endif
};

// static (mark/release) heap
struct MFStaticHeapData
{
	void *pHeapPointer;
	uint32 heapSize;

	char **markStack;
	uint32 markCount;
};

#endif
