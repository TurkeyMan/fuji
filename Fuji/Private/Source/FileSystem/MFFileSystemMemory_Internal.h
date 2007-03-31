#if !defined(_MFFILESYSTEM_MEMORY_INTERNAL_H)
#define _MFFILESYSTEM_MEMORY_INTERNAL_H

#include "FileSystem/MFFileSystemMemory.h"

struct MFFileMemoryData
{
	void *pMemoryPointer;
	uint32 allocated;
	bool ownsMemory;	// this calls Heap_Free() on the memory pointer when the file is closed.
};

// internal functions
void MFFileSystemMemory_InitModule();
void MFFileSystemMemory_DeinitModule();

// filesystem callbacks
int MFFileSystemMemory_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemMemory_Dismount(MFMount *pMount);

MFFile* MFFileSystemMemory_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileMemory_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileMemory_Close(MFFile* fileHandle);
int MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileMemory_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async);
int MFFileMemory_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileMemory_Tell(MFFile* fileHandle);

MFFileState MFFileMemory_Query(MFFile* fileHandle);
int MFFileMemory_GetSize(MFFile* fileHandle);

// handle to the Memory filesystem
extern MFFileSystemHandle hMemoryFileSystem;

#endif
