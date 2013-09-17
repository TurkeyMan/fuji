/**
 * @file MFHeap.h
 * @brief Declares structures and functions used when accessing the Fuji Heap.
 * @author Manu Evans
 * @defgroup MFHeap Heap Management
 * @{
 */

#if !defined(_MFHEAP_H)
#define _MFHEAP_H

/**
 * @struct MFHeap
 * Represents a Fuji Heap.
 */
struct MFHeap;

/**
 * Heap type enums.
 * Various heaps available at runtime.
 */
enum MFHeapType
{
	MFHT_Unknown = -1,		/**< Unknown heap type. */

	MFHT_Active = 0,		/**< Main heap for general game usage. */
	MFHT_ActiveTemporary,	/**< For small short term temporary allocation, allocates back from the end of the main static heap (If the main heap is full, temp alloc's will fail). */
	MFHT_Debug,				/**< The defalt heap for debugging info (so debugging doesn't pollute the game heap). */
	MFHT_External,			/**< Accesses the system heap directly. */
	MFHT_Custom,			/**< Use user supplied callbacks for allocating memory (User must have registered a custom heap prior to allocation). */

	MFHT_Max,				/**< Max heap. */
	MFHT_ForceInt = 0x7FFFFFFF	/**< Force enumeration to an int type */
};

/**
 * Memory allocaiton callbacks.
 * Used to store a set of memoy allocation callbacks used when registering custom heaps.
 */
struct MFMemoryCallbacks
{
	void* (*pMalloc)(size_t bytes, void *pUserData);	/**< Pointer to function used to allocate memory. */
	void  (*pFree)(void *pMemory, void *pUserData);		/**< Pointer to function used to free memory. */
};

/**
 * @fn void* MFHeap_Alloc(size_t bytes, MFHeap *pHeap)
 * Allocates a block of memory.
 * Allocates a new memory block of the specified size.
 * @param bytes Number of bytes to allocate for the new buffer.
 * @param pHeap Optional pointer to an MFHeap where the memory will be allocated. If pHeap is set to NULL, the current 'Active' heap will be used.
 * @return A pointer to a new area in memory ready for use.
 * @see MFHeap_AllocAndZero()
 * @see MFHeap_Realloc()
 * @see MFHeap_Free()
 */
MF_API void* MFHeap_AllocInternal(size_t bytes, MFHeap *pHeap = NULL);

/**
 * @fn void* MFHeap_AllocAndZero(size_t bytes, MFHeap *pHeap)
 * Allocates a block of memory and zero's the contents.
 * Allocates a new memory block of the specified size and zero's the contents.
 * @param bytes Number of bytes to allocate for the new buffer.
 * @param pHeap Optional pointer to an MFHeap where the memory will be allocated. If pHeap is set to NULL, the current 'Active' heap will be used.
 * @return A pointer to a new area in memory ready for use.
 * @see MFHeap_Alloc()
 * @see MFHeap_Realloc()
 * @see MFHeap_Free()
 */
MF_API void* MFHeap_AllocAndZeroInternal(size_t bytes, MFHeap *pHeap = NULL);

/**
 * @fn void* MFHeap_Realloc(void *pMem, size_t bytes)
 * Re-allocates an allocated block of memory.
 * Allocates a new memory block of the specified size and copies the contents of the previous buffer.
 * The old buffer is safely released.
 * @param pMem Pointer to block of memory being re-allocated.
 * @param bytes Numer of bytes to allocate for the new buffer.
 * @return A pointer to the new allocated memory.
 * @remarks If the new allocation is smaller than the previous allocation, only the number of bytes that fit into the new allocation will be copied. The remaining memory will be lost. Memory will be allocated using the same heap manager as the initial allocation.
 * @see MFHeap_Alloc()
 * @see MFHeap_Free()
 */
MF_API void* MFHeap_ReallocInternal(void *pMem, size_t bytes);

// these are here to satisfy doxygen
#if 0
void* MFHeap_AllocAndZero(size_t bytes, MFHeap *pHeap = NULL);
void* MFHeap_Alloc(size_t bytes, MFHeap *pHeap = NULL);
void* MFHeap_Realloc(void *pMem, size_t bytes);
void* MFHeap_TAlloc(size_t bytes);
#endif

/**
 * Frees a block of memory.
 * Frees an allocated block of memory.
 * @param pMem Pointer to memory to be freed.
 * @return None.
 * @see MFHeap_Alloc()
 * @see MFHeap_Realloc()
 */
MF_API void MFHeap_Free(void *pMem);

#include <new>

#if defined(MF_COMPILER_VISUALC) && !defined(_FUJI_UTIL)
//	#define MF_NEW_OPERATORS
#endif
#if !defined(_FUJI_UTIL)
//	#define MF_PLACEMENT_NEW_OPERATORS
#endif

// FIXME
#if defined(MF_NEW_OPERATORS)
inline void* operator new(size_t size) { return MFHeap_AllocInternal(size); }
inline void* operator new[](size_t size) { return MFHeap_AllocInternal(size); }
inline void operator delete(void *pMemory) { MFHerap_Free(pMemory); }
inline void operator delete[](void *pMemory) { MFHerap_Free(pMemory); }
#endif

// placement new operators
#if defined(MF_PLACEMENT_NEW_OPERATORS)
inline void* operator new(size_t size, void *pMem) { return pMem; }
inline void* operator new[](size_t size, void *pMem) { return pMem; }
inline void operator delete(void *pMemory, void *pMem) {}
inline void operator delete[](void *pMemory, void *pMem) {}
#endif

// *** document me!! ***
MF_API size_t MFHeap_GetTotalAllocated(MFHeap *pHeap);
MF_API size_t MFHeap_GetTotalWaste(MFHeap *pHeap);
MF_API int MFHeap_GetNumAllocations(MFHeap *pHeap);

/**
 * Get the size of an allocation.
 * Get the size of a fuji allocation.
 * @param pMemory Pointer to an allocated block of memory.
 * @return Size, in bytes, of the block of memory that \a pMemory points to.
 * @see MFHeap_Alloc()
 * @see MFHeap_GetAllocHeap()
 */
MF_API size_t MFHeap_GetAllocSize(const void *pMemory);

/**
 * Get the heap of an allocation.
 * Get the heap of a fuji allocation.
 * @param pMemory Pointer to an allocated block of memory.
 * @return Pointer to the MFHeap the memory was allocated in.
 * @see MFHeap_Alloc()
 * @see MFHeap_GetAllocSize()
 */
MF_API MFHeap *MFHeap_GetAllocHeap(const void *pMemory);

/**
 * Get a Heap pointer.
 * Returns a pointer to an available heaps.
 * @param heap Type of heap to be retrieved.
 * @return Pointer to the desired heap.
 * @see MFHeap_GetTempHeap()
 * @see MFHeap_SetActiveHeap()
 */
MF_API MFHeap* MFHeap_GetHeap(MFHeapType heap);

/**
 * Gets the temp heap associated with a heap.
 * Returns a pointer to a heap's associated 'Temp' heap. This heap can be used for making small, short term allocations.
 * Temp allocations are typically allocated in a different area in the heap so they dont interfere with, or fragment, regular allocations.
 * @param pHeap Pointer to the main heap for which the temp heap is desired.
 * @return Pointer to the heaps 'Temp' heap.
 * @see MFHeap_GetHeap()
 * @see MFHeap_SetActiveHeap()
 */
MF_API MFHeap* MFHeap_GetTempHeap(MFHeap *pHeap);

/**
 * Gets the debug heap.
 * Returns a pointer to the debug heap.
 * @return Pointer to the debug heap.
 * @see MFHeap_GetHeap()
 * @see MFHeap_SetActiveHeap()
 */
MF_API MFHeap* MFHeap_GetDebugHeap();

/**
 * Set active heap.
 * Changes the current 'Active' heap, that is, the heap allocations are made in by default.
 * passing MFHT_Active to MFHeap_GetHeap() will retrieve the current active heap.
 * @param pHeap Pointer to a heap that will be come the active heap.
 * @return Pointer to the old active heap.
 * @see MFHeap_GetHeap()
 */
MF_API MFHeap* MFHeap_SetActiveHeap(MFHeap *pHeap);

/**
 * Set allocation alignment.
 * Sets the allocation alignment in bytes.
 * @param bytes Allocation alignment in bytes.
 * @return Old allocation alignment in bytes.
 * @remarks The allocation alignment is the minimum byte boundary where memory can be allocated. The default allocation alignment is 32 bytes.
 * @see MFHeap_AllocInternal()
 */
MF_API int MFHeap_SetAllocAlignment(int bytes);

/**
 * Push a heap marker.
 * Push a heap marker for static heap.
 * @param pHeap Heap to push mark on. Passing NULL refers to the current active heap.
 * @return None.
 * @see MFHeap_Release()
 */
MF_API void MFHeap_Mark(MFHeap *pHeap = NULL);

/**
 * Pop a heap marker.
 * Pop a heap marker for static heap.
 * @param pHeap Heap to pop mark from. Passing NULL refers to the current active heap.
 * @return None.
 * @see MFHeap_Mark()
 */
MF_API void MFHeap_Release(MFHeap *pHeap = NULL);

/**
 * Registers a custom heap.
 * Registers a set of callbacks and some user data to access a custom heap.
 * @param pCallbacks Pointer to an MFMemoryCallbacks structure containing a set of callbacks to be used for memory management.
 * @param pUserData Pointer to a block of user data. This is passed into the memory callbacks. This parameter may be NULL.
 * @return None.
 * @see MFHeap_GetHeap()
 */
MF_API void MFHeap_RegisterCustomHeap(const MFMemoryCallbacks *pCallbacks, void *pUserData);

/**
 * Set over-ride heap.
 * Sets an override heap which forces all allocation operations to use this override heap.
 * @param pHeap Pointer to a heap that will be used for ALL following memory operations. Pass NULL to release the override heap.
 * @return None.
 */
MF_API void MFHeap_SetHeapOverride(MFHeap *pHeap);

/**
 * Validate a block of memory.
 * Checks to see if an allocated block of memory has been corrupted. This will test for buffer overflows and underflows.
 * @param pMemory Pointer to a block of memory to validate.
 * @return \a true if memory is in a valid, \a false otherwise.
 */
MF_API bool MFHeap_ValidateMemory(const void *pMemory);

/**
 * Validate the heap.
 * Checks to see if any allocations have been corrupted. This will test for buffer overflows and underflows.
 * @return \a true is memory is in a valid, \a false otherwise.
 */
MF_API bool MFHeap_ValidateHeap();

/**
 * Get an uncached pointer to a block of memory.
 * Returns a pointer that provides uncached access to a specified block of memory.
 * @param pPointer Pointer to a block of memory.
 * @return Pointer to the same block of memory with uncached read and write access.
 * @remarks This function is not guaranteed to return a genuine uncached pointer. Some platforms dont have an uncached addressing mode. This should be used for performance optimisation only.
 */
MF_API void* MFHeap_GetUncachedPointer(void *pPointer);

/**
 * Flushes the Data Cache.
 * Flushes the Data Cache.
 * @return None.
 */
MF_API void MFHeap_FlushDCache();

/**
 * Push a memory allocation group.
 * Pushes a memory allocation group that is used for heap tracking and profiling.
 * @param pGroupName Pointer to a string containing the group name for following allocations.
 * @return None.
 */
MF_API void MFHeap_PushGroupName(const char *pGroupName);

/**
 * Pop a memory allocation group.
 * Pops a memory allocation group that is used for heap tracking and profiling.
 * @return None.
 */
MF_API void MFHeap_PopGroupName();


/** @internal */
class MFHeapScope
{
public:
	MFHeapScope(MFHeapType type) { pOld = MFHeap_SetActiveHeap(MFHeap_GetHeap(type)); }
	~MFHeapScope() { MFHeap_SetActiveHeap(pOld); }
	MFHeap *pOld;
};

#define MFHEAP_SCOPE(type) MFHeapScope scopeHeap(type)

/*** internal ***/

// user alloc macros
#if !defined(_RETAIL)
	// to trace memory allocations
	MF_API void MFHeap_SetLineAndFile(int line, const char *pFile);

	/** @internal */
	static class MFHeapDebug
	{
		public:
		inline MFHeapDebug& Tracker(int line, const char *pFile) { MFHeap_SetLineAndFile(line, pFile); return *this; }
		inline static void *Alloc(size_t bytes, MFHeap *pHeap = NULL) { return MFHeap_AllocInternal(bytes, pHeap); }
		inline static void *AllocAndZero(size_t bytes, MFHeap *pHeap = NULL) { return MFHeap_AllocAndZeroInternal(bytes, pHeap); }
	} MFHeap_Debug;

	// these macros wrap the debug heap trackers functionality
	#define MFHeap_Alloc MFHeap_Debug.Tracker(__LINE__, __FILE__).Alloc
	#define MFHeap_AllocAndZero MFHeap_Debug.Tracker(__LINE__, __FILE__).AllocAndZero
	#define MFHeap_Realloc(pMem, bytes) (MFHeap_SetLineAndFile(__LINE__, __FILE__), MFHeap_ReallocInternal((pMem), (bytes)))
	#define MFHeap_TAlloc(bytes) MFHeap_Debug.Tracker(__LINE__, __FILE__).Alloc((bytes), MFHeap_GetHeap(MFHT_ActiveTemporary))
#else
	// compiled out in retail
	#define MFHeap_Alloc MFHeap_AllocInternal
	#define MFHeap_AllocAndZero MFHeap_AllocAndZeroInternal
	#define MFHeap_Realloc MFHeap_ReallocInternal
	#define MFHeap_TAlloc(bytes) MFHeap_AllocInternal((bytes), MFHeap_GetHeap(MFHT_ActiveTemporary))
#endif

#endif // _MFHEAP_H

/** @} */
