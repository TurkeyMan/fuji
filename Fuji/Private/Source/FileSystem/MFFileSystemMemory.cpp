#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemMemory.h"

PtrListDL<MFFileMemoryData> gMemoryFiles;

void MFFileSystemMemory_InitModule()
{
	gMemoryFiles.Init("Memory Files", gDefaults.filesys.maxOpenFiles);

	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = MFFileSystemMemory_Register;
	fsCallbacks.UnregisterFS = MFFileSystemMemory_Unregister;
	fsCallbacks.FSMount = MFFileSystemMemory_Mount;
	fsCallbacks.FSOpen = MFFileSystemMemory_Open;
	fsCallbacks.Open = MFFileMemory_Open;
	fsCallbacks.Close = MFFileMemory_Close;
	fsCallbacks.Read = MFFileMemory_Read;
	fsCallbacks.Write = MFFileMemory_Write;
	fsCallbacks.Seek = MFFileMemory_Seek;
	fsCallbacks.Tell = MFFileMemory_Tell;
	fsCallbacks.Query = MFFileMemory_Query;
	fsCallbacks.GetSize = MFFileMemory_GetSize;

	hMemoryFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemMemory_DeinitModule()
{
	MFFileSystem_UnregisterFileSystem(hMemoryFileSystem);

	gMemoryFiles.Deinit();
}

// filesystem callbacks
void MFFileSystemMemory_Register()
{

}

void MFFileSystemMemory_Unregister()
{

}

int MFFileSystemMemory_Mount(MFMount *pMount, MFMountData *pMountData)
{
	// cant mount a memory filesystem
	return -1;
}

MFFile* MFFileSystemMemory_Open(MFMount *pMount, MFTOCEntry *pTOCEntry, uint32 openFlags)
{
	// no mounted memory filesystems
	return NULL;
}

int MFFileMemory_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	CALLSTACK;

	DBGASSERT(pOpenData->cbSize == sizeof(MFOpenDataMemory), "Incorrect size for MFOpenDataMemory structure. Invalid pOpenData.");
	MFOpenDataMemory *pMemory = (MFOpenDataMemory*)pOpenData;

	pFile->pFilesysData = gMemoryFiles.Create();
	MFFileMemoryData *pMem = (MFFileMemoryData*)pFile->pFilesysData;

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;
	pFile->length = pMemory->fileSize;

	pMem->pMemoryPointer = pMemory->pMemoryPointer;
	pMem->allocated = pMemory->allocated;
	pMem->ownsMemory = pMemory->ownsMemory;

#if defined(_DEBUG)
	strcpy(pFile->fileIdentifier, STR("Memory: 0x%08X", pMem->pMemoryPointer));
#endif

	return 0;
}

int MFFileMemory_Close(MFFile* fileHandle)
{
	CALLSTACK;

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	if(pMem->ownsMemory)
		Heap_Free(pMem->pMemoryPointer);

	gMemoryFiles.Destroy(pMem);

	return 0;
}

int MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	DBGASSERT(async == false, "Asynchronous Filesystem not yet supported...");

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	uint32 bytesToCopy = fileHandle->length > -1 ? Min(bytes, (uint32)fileHandle->length - fileHandle->offset) : bytes;

	memcpy(pBuffer, &pMem->pMemoryPointer[fileHandle->offset], bytesToCopy);
	fileHandle->offset += bytesToCopy;

	return bytesToCopy;
}

int MFFileMemory_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	DBGASSERT(async == false, "Asynchronous Filesystem not yet supported...");

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	uint32 bytesToCopy = pMem->allocated ? Min(bytes, pMem->allocated - fileHandle->offset) : bytes;

	memcpy(&pMem->pMemoryPointer[fileHandle->offset], pBuffer, bytesToCopy);

	fileHandle->offset += bytesToCopy;
	fileHandle->length = Max((int)fileHandle->offset, fileHandle->length);

	return bytesToCopy;
}

int MFFileMemory_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	CALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = Min(bytes, fileHandle->length);
			break;
		case MFSeek_End:
			newPos = Max(0, fileHandle->length - bytes);
			break;
		case MFSeek_Current:
			newPos = Clamp(0, (int)fileHandle->offset + bytes, fileHandle->length);
			break;
		default:
			DBGASSERT(false, "Invalid 'relativity' for file seeking.");
	}

	fileHandle->offset = (uint32)newPos;
	return newPos;
}

int MFFileMemory_Tell(MFFile* fileHandle)
{
	CALLSTACK;
	return (int)fileHandle->offset;
}

MFFileState MFFileMemory_Query(MFFile* fileHandle)
{
	CALLSTACK;
	return fileHandle->state;
}

int MFFileMemory_GetSize(MFFile* fileHandle)
{
	CALLSTACK;
	return fileHandle->length;
}
