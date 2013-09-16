/**
 * @addtogroup MFFileSystem
 * @{
 */

#if !defined(_MFFILESYSTEM_MEMORY_H)
#define _MFFILESYSTEM_MEMORY_H

#include "Fuji/MFFileSystem.h"

/**
 * Open memory file structure.
 * Information required to open a memory file.
 */
struct MFOpenDataMemory : public MFOpenData
{
	void *pMemoryPointer;	/**< Pointer to the file. */
	size_t allocated;		/**< Amount of space allocated to file. Pass 0 to specify no upper limit to the memory file (WARNING: this is dangerous!) */
	int64 fileSize;			/**< Size of memory file. Pass -1 for an unknown for unlimited file size (WARNING: this is dangerous) */
	bool ownsMemory;		/**< This tells the filesystem to call /a MFHeap_Free() on the memory when the file is closed. */
};

/** @} */

#endif
