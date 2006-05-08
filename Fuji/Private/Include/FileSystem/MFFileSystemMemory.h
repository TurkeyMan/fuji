/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_MEMORY_H)
#define _MFFILESYSTEM_MEMORY_H

/**
 * Open memory file structure.
 * Information required to open a memory file.
 */
struct MFOpenDataMemory : public MFOpenData
{
	char *pMemoryPointer;	/**< Pointer to the file. */
	int fileSize;			/**< Size of memory file. Pass -1 for an unknown for unlimited file size (WARNING: this is dangerous) */
	uint32 allocated;		/**< Amount of space allocated to file. Pass 0 to specify no upper limit to the memory file (WARNING: this is dangerous!) */
	bool ownsMemory;		/**< This tells the filesystem to call /a MFHeap_Free() on the memory when the file is closed. */
};

/** @} */

#endif
