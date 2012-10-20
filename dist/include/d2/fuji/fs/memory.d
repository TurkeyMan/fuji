module fuji.fs.memory;

public import fuji.filesystem;

/**
 * Open memory file structure.
 * Information required to open a memory file.
 */
struct MFOpenDataMemory
{
	MFOpenData base = MFOpenData(typeof(this).sizeof, MFOpenData.init.openFlags);
	alias base this;

	void* pMemoryPointer;	/**< Pointer to the file. */
	size_t allocated;		/**< Amount of space allocated to file. Pass 0 to specify no upper limit to the memory file (WARNING: this is dangerous!) */
	long fileSize;			/**< Size of memory file. Pass -1 for an unknown for unlimited file size (WARNING: this is dangerous) */
	bool ownsMemory;		/**< This tells the filesystem to call /a MFHeap_Free() on the memory when the file is closed. */
}

