module fuji.heap;

public import fuji.c.MFHeap;

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
void[] MFHeap_Alloc(int Line = __LINE__, string File = __FILE__)(size_t bytes, MFHeap* pHeap = null) nothrow
{
	debug MFHeap_SetLineAndFile(Line, File.ptr);
	return MFHeap_AllocInternal(bytes, pHeap)[0..bytes];
}

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
void[] MFHeap_AllocAndZero(int Line = __LINE__, string File = __FILE__)(size_t bytes, MFHeap* pHeap = null) nothrow
{
	debug MFHeap_SetLineAndFile(Line, File.ptr);
	return MFHeap_AllocAndZeroInternal(bytes, pHeap)[0..bytes];
}

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
void[] MFHeap_Realloc(int Line = __LINE__, string File = __FILE__)(void* pMem, size_t bytes) nothrow
{
	debug MFHeap_SetLineAndFile(Line, File.ptr);
	return MFHeap_ReallocInternal(pMem, bytes)[0..bytes];
}

alias MFHeap_Free = fuji.c.MFHeap.MFHeap_Free;
void MFHeap_Free(void[] mem) nothrow
{
	debug assert(MFHeap_GetAllocSize(mem.ptr) == mem.length, "Buffer being freed has different size than the allocation!");
	MFHeap_Free(mem.ptr);
}

void[] MFHeap_TAlloc(int Line = __LINE__, string File = __FILE__)(size_t bytes, MFHeap* pHeap = null) nothrow
{
	debug MFHeap_SetLineAndFile(Line, File.ptr);
	return MFHeap_AllocInternal(bytes, MFHeap_GetHeap(MFHeapType.ActiveTemporary))[0..bytes];
}
