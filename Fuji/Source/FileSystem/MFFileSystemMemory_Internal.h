#if !defined(_MFFILESYSTEM_MEMORY_INTERNAL_H)
#define _MFFILESYSTEM_MEMORY_INTERNAL_H

#include "FileSystem/MFFileSystemMemory.h"

struct MFFileMemoryData
{
	void *pMemoryPointer;
	size_t allocated;
	bool ownsMemory;	// this calls Heap_Free() on the memory pointer when the file is closed.
};

// internal functions
MFInitStatus MFFileSystemMemory_InitModule(int moduleId, bool bPerformInitialisation);
void MFFileSystemMemory_DeinitModule();

// filesystem callbacks
int MFFileSystemMemory_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemMemory_Dismount(MFMount *pMount);

MFFile* MFFileSystemMemory_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileMemory_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileMemory_Close(MFFile* fileHandle);
size_t MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, size_t bytes);
size_t MFFileMemory_Write(MFFile* fileHandle, const void *pBuffer, size_t bytes);
uint64 MFFileMemory_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);

bool MFFileMemory_Delete(const char *pPath, bool bRecursive);

#endif
