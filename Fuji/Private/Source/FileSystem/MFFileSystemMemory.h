#if !defined(_MFFILESYSTEM_MEMORY_H)
#define _MFFILESYSTEM_MEMORY_H

struct MFOpenDataMemory : public MFOpenData
{
	char *pMemoryPointer;
	int fileSize;		// pass -1 for an unknown or unlimited file size (WARNING: this is dangerous)
	uint32 allocated;	// pass 0 to specify no upper limit to the memory file (WARNING: this is dangerous!)
	bool ownsMemory;	// this calls Heap_Free() on the memory pointer when the file is closed.
};

struct MFFileMemoryData
{
	char *pMemoryPointer;
	uint32 allocated;
	bool ownsMemory;	// this calls Heap_Free() on the memory pointer when the file is closed.
};

// internal functions
void MFFileSystemMemory_InitModule();
void MFFileSystemMemory_DeinitModule();

// filesystem callbacks
void MFFileSystemMemory_Register();
void MFFileSystemMemory_Unregister();

int MFFileSystemMemory_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemMemory_Dismount(MFMount *pMount);

MFFile* MFFileSystemMemory_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileMemory_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileMemory_Close(MFFile* fileHandle);
int MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileMemory_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileMemory_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileMemory_Tell(MFFile* fileHandle);

MFFileState MFFileMemory_Query(MFFile* fileHandle);
int MFFileMemory_GetSize(MFFile* fileHandle);

// handle to the Memory filesystem
extern FileSystemHandle hMemoryFileSystem;

#endif