#if !defined(_MFHEAP_INTERNAL_H)
#define _MFHEAP_INTERNAL_H

#include "MFHeap.h"

// heap module init/deinit
void MFHeap_InitModule();
void MFHeap_DeinitModule();

void MFHeap_InitModulePlatformSpecific();
void MFHeap_DeinitModulePlatformSpecific();

// system allocation/deallocation
void* MFHeap_SystemMalloc(uint32 bytes);
void* MFHeap_SystemRealloc(void *pBuffer, uint32 bytes);
void MFHeap_SystemFree(void *pBuffer);

// heap structure
struct MFHeap
{
	MFMemoryCallbacks *pCallbacks;
	void *pHeapData;
	MFHeap *pTempHeap;

	MFHeapType heapType;

#if !defined(_RETAIL)
	char heapName[32];
#endif
};

// allocation structure. stored immediately before any fuji allocation to identify the heap it was allocated with, and verify memory integrity.
struct MFAllocHeader
{
	MFHeap *pHeap;
	uint32 size;
	const char *pFile;
	uint16 line;
	uint16 alignment;
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
