#if !defined(_MFHEAP_H)
#define _MFHEAP_H

struct MFHeap;

// enum's for the default heaps
enum MFHeapType
{
	MFHT_Unknown = -1,

	MFHT_Active = 0,		// main heap for general game usage.
	MFHT_ActiveTemporary,	// for small short term temporary allocation, allocates back from the end of the main static heap. If the main heap is full, temp alloc's will fail.
	MFHT_Debug,				// the defalt heap for debugging info (so debugging doesn't pollute the game heap).
	MFHT_External,			// just call 'malloc' directly.
	MFHT_Custom,			// use user supplied callbacks for allocating memory. user must have registered a custom heap prior to allocation.

	MFHT_Max,
	MFHT_ForceInt = 0x7FFFFFFF
};

// custom heap related
struct MFMemoryCallbacks
{
	void* (*pMalloc)(uint32 bytes, void *pUserData);
	void  (*pFree)(void *pMemory, void *pUserData);
};

// functions to allocate/free memory
void *MFHeap_AllocInternal(uint32 bytes, MFHeap *pHeap = NULL);
void *MFHeap_ReallocInternal(void *pMem, uint32 bytes);
void  MFHeap_Free(void *pMem);

// user alloc macros
#if !defined(_RETAIL)
	// to trace memory allocations
	void MFHeap_SetLineAndFile(int line, char *pFile);

	static class MFHeapDebug
	{
		public:
		inline MFHeapDebug& Tracker(int line, char *pFile) { MFHeap_SetLineAndFile(line, pFile); return *this; }
		inline static void *Alloc(uint32 bytes, MFHeap *pHeap = NULL) { return MFHeap_AllocInternal(bytes, pHeap); }
	} MFHeap_Debug;

	#define MFHeap_Alloc MFHeap_Debug.Tracker(__LINE__, __FILE__).Alloc
	#define MFHeap_Realloc(pMem, bytes) (MFHeap_SetLineAndFile(__LINE__, __FILE__), MFHeap_ReallocInternal(pMem, bytes))
#else
	#define MFHeap_Alloc MFHeap_AllocInternal
	#define MFHeap_Realloc MFHeap_ReallocInternal
#endif

// get a heap pointer
MFHeap* MFHeap_GetHeap(MFHeapType heap);

// gets the temp heap associated with a heap
MFHeap* MFHeap_GetTempHeap(MFHeap *pHeap);

// set active heap, return the old active heap
MFHeap* MFHeap_SetActiveHeap(MFHeap *pHeap);

// set allocation alignment, return old alignment
int MFHeap_SetAllocAlignment(int bytes);

// push/pop a heap marker for static heaps
void MFHeap_Mark(MFHeap *pHeap = NULL);
void MFHeap_Release(MFHeap *pHeap = NULL);

// register a custom heap
void MFHeap_RegisterCustomHeap(const MFMemoryCallbacks *pCallbacks, void *pUserData);

// set override heap. any allocation operations are forced to use this override heap.
void MFHeap_SetHeapOverride(MFHeap *pHeap);

// validate a block of memory. returns false if memory had been corrupted.
bool MFHeap_ValidateMemory(void *pMemory);

// memory related functions
void* MFHeap_GetUncachedPointer(void *pPointer);
void MFHeap_FlushDCache();

// memory allocation groups for profiling
void MFHeap_PushGroupName(const char *pGroupName);
void MFHeap_PopGroupName();

#endif // _MFHEAP_H
