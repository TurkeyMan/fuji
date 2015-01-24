module fuji.c.MFHeap;

nothrow:
@nogc:

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
	Unknown = -1,		/**< Unknown heap type. */

	Active = 0,			/**< Main heap for general game usage. */
	ActiveTemporary,	/**< For small short term temporary allocation, allocates back from the end of the main static heap (If the main heap is full, temp alloc's will fail). */
	Debug,				/**< The defalt heap for debugging info (so debugging doesn't pollute the game heap). */
	External,			/**< Accesses the system heap directly. */
	Custom				/**< Use user supplied callbacks for allocating memory (User must have registered a custom heap prior to allocation). */
}

/**
 * Memory allocaiton callbacks.
 * Used to store a set of memoy allocation callbacks used when registering custom heaps.
 */
struct MFMemoryCallbacks
{
	extern (C) void* function(size_t bytes, void* pUserData) nothrow pMalloc;	/**< Pointer to function used to allocate memory. */
	extern (C) void  function(void* pMemory, void* pUserData) nothrow pFree;	/**< Pointer to function used to free memory. */
}

/**
* Frees a block of memory.
* Frees an allocated block of memory.
* @param pMem Pointer to memory to be freed.
* @return None.
* @see MFHeap_Alloc()
* @see MFHeap_Realloc()
*/
extern (C) void MFHeap_Free(void *pMem);


// *** document me!! ***
extern (C) size_t MFHeap_GetTotalAllocated(MFHeap* pHeap);
extern (C) size_t MFHeap_GetTotalWaste(MFHeap* pHeap);
extern (C) int MFHeap_GetNumAllocations(MFHeap* pHeap);

/**
 * Get the size of an allocation.
 * Get the size of a fuji allocation.
 * @param pMemory Pointer to an allocated block of memory.
 * @return Size, in bytes, of the block of memory that \a pMemory points to.
 * @see MFHeap_Alloc()
 * @see MFHeap_GetAllocHeap()
 */
extern (C) size_t MFHeap_GetAllocSize(const(void*) pMemory);

/**
 * Get the heap of an allocation.
 * Get the heap of a fuji allocation.
 * @param pMemory Pointer to an allocated block of memory.
 * @return Pointer to the MFHeap the memory was allocated in.
 * @see MFHeap_Alloc()
 * @see MFHeap_GetAllocSize()
 */
extern (C) MFHeap* MFHeap_GetAllocHeap(const(void*) pMemory);

/**
 * Get a Heap pointer.
 * Returns a pointer to an available heaps.
 * @param heap Type of heap to be retrieved.
 * @return Pointer to the desired heap.
 * @see MFHeap_GetTempHeap()
 * @see MFHeap_SetActiveHeap()
 */
extern (C) MFHeap* MFHeap_GetHeap(MFHeapType heap);

/**
 * Gets the temp heap associated with a heap.
 * Returns a pointer to a heap's associated 'Temp' heap. This heap can be used for making small, short term allocations.
 * Temp allocations are typically allocated in a different area in the heap so they dont interfere with, or fragment, regular allocations.
 * @param pHeap Pointer to the main heap for which the temp heap is desired.
 * @return Pointer to the heaps 'Temp' heap.
 * @see MFHeap_GetHeap()
 * @see MFHeap_SetActiveHeap()
 */
extern (C) MFHeap* MFHeap_GetTempHeap(MFHeap* pHeap);

/**
 * Gets the debug heap.
 * Returns a pointer to the debug heap.
 * @return Pointer to the debug heap.
 * @see MFHeap_GetHeap()
 * @see MFHeap_SetActiveHeap()
 */
extern (C) MFHeap* MFHeap_GetDebugHeap();

/**
 * Set active heap.
 * Changes the current 'Active' heap, that is, the heap allocations are made in by default.
 * passing Active to MFHeap_GetHeap() will retrieve the current active heap.
 * @param pHeap Pointer to a heap that will be come the active heap.
 * @return Pointer to the old active heap.
 * @see MFHeap_GetHeap()
 */
extern (C) MFHeap* MFHeap_SetActiveHeap(MFHeap* pHeap);

/**
 * Set allocation alignment.
 * Sets the allocation alignment in bytes.
 * @param bytes Allocation alignment in bytes.
 * @return Old allocation alignment in bytes.
 * @remarks The allocation alignment is the minimum byte boundary where memory can be allocated. The default allocation alignment is 32 bytes.
 * @see MFHeap_AllocInternal()
 */
extern (C) int MFHeap_SetAllocAlignment(int bytes);

/**
 * Push a heap marker.
 * Push a heap marker for static heap.
 * @param pHeap Heap to push mark on. Passing NULL refers to the current active heap.
 * @return None.
 * @see MFHeap_Release()
 */
extern (C) void MFHeap_Mark(MFHeap* pHeap = null);

/**
 * Pop a heap marker.
 * Pop a heap marker for static heap.
 * @param pHeap Heap to pop mark from. Passing NULL refers to the current active heap.
 * @return None.
 * @see MFHeap_Mark()
 */
extern (C) void MFHeap_Release(MFHeap* pHeap = null);

/**
 * Registers a custom heap.
 * Registers a set of callbacks and some user data to access a custom heap.
 * @param pCallbacks Pointer to an MFMemoryCallbacks structure containing a set of callbacks to be used for memory management.
 * @param pUserData Pointer to a block of user data. This is passed into the memory callbacks. This parameter may be NULL.
 * @return None.
 * @see MFHeap_GetHeap()
 */
extern (C) void MFHeap_RegisterCustomHeap(const(MFMemoryCallbacks*) pCallbacks, void* pUserData);

/**
 * Set over-ride heap.
 * Sets an override heap which forces all allocation operations to use this override heap.
 * @param pHeap Pointer to a heap that will be used for ALL following memory operations. Pass NULL to release the override heap.
 * @return None.
 */
extern (C) void MFHeap_SetHeapOverride(MFHeap* pHeap);

/**
 * Validate a block of memory.
 * Checks to see if an allocated block of memory has been corrupted. This will test for buffer overflows and underflows.
 * @param pMemory Pointer to a block of memory to validate.
 * @return TRUE is memory is in a valid, FALSE otherwise.
 */
extern (C) bool MFHeap_ValidateMemory(const(void*) pMemory);

// TODO...
extern (C) bool MFHeap_ValidateHeap();

/**
 * Get an uncached pointer to a block of memory.
 * Returns a pointer that provides uncached access to a specified block of memory.
 * @param pPointer Pointer to a block of memory.
 * @return A pointer to the same block of memory with uncached read and write access.
 * @remarks This function is not guaranteed to return a genuine uncached pointer. Some platforms dont have an uncached addressing mode. This should be used for performance optimisation only.
 */
extern (C) void* MFHeap_GetUncachedPointer(void* pPointer);

/**
 * Flushes the Data Cache.
 * Flushes the Data Cache.
 * @return None.
 */
extern (C) void MFHeap_FlushDCache();

/**
 * Push a memory allocation group.
 * Pushes a memory allocation group that is used for heap tracking and profiling.
 * @param pGroupName Pointer to a string containing the group name for following allocations.
 * @return None.
 */
extern (C) void MFHeap_PushGroupName(const(char*) pGroupName);

/**
 * Pop a memory allocation group.
 * Pops a memory allocation group that is used for heap tracking and profiling.
 * @return None.
 */
extern (C) void MFHeap_PopGroupName();

/+
/** @internal */
class MFHeapScope
{
public:
	MFHeapScope(MFHeapType type) { pOld = MFHeap_SetActiveHeap(MFHeap_GetHeap(type)); }
	~MFHeapScope() { MFHeap_SetActiveHeap(pOld); }
	MFHeap *pOld;
};

/**
 */
#define MFHEAP_SCOPE(type) MFHeapScope scopeHeap(type)
+/
