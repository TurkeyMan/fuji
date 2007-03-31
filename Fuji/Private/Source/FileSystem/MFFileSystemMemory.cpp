#include "Fuji.h"
#include "MFSystem.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemMemory_Internal.h"

MFPtrListDL<MFFileMemoryData> gMemoryFiles;

void MFFileSystemMemory_InitModule()
{
	gMemoryFiles.Init("Memory Files", gDefaults.filesys.maxOpenFiles);

	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = NULL;
	fsCallbacks.UnregisterFS = NULL;
	fsCallbacks.FSMount = MFFileSystemMemory_Mount;
	fsCallbacks.FSDismount = MFFileSystemMemory_Dismount;
	fsCallbacks.FSOpen = MFFileSystemMemory_Open;
	fsCallbacks.Open = MFFileMemory_Open;
	fsCallbacks.Close = MFFileMemory_Close;
	fsCallbacks.Read = MFFileMemory_Read;
	fsCallbacks.Write = MFFileMemory_Write;
	fsCallbacks.Seek = MFFileMemory_Seek;
	fsCallbacks.Tell = MFFileMemory_Tell;
	fsCallbacks.Query = MFFileMemory_Query;
	fsCallbacks.GetSize = MFFileMemory_GetSize;
	fsCallbacks.FindFirst = NULL;
	fsCallbacks.FindNext = NULL;
	fsCallbacks.FindClose = NULL;

	hMemoryFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemMemory_DeinitModule()
{
	MFFileSystem_UnregisterFileSystem(hMemoryFileSystem);

	gMemoryFiles.Deinit();
}

// filesystem callbacks
int MFFileSystemMemory_Mount(MFMount *pMount, MFMountData *pMountData)
{
	// cant mount a memory filesystem
	return -1;
}

int MFFileSystemMemory_Dismount(MFMount *pMount)
{
	// cant mount a memory filesystem
	return -1;
}

MFFile* MFFileSystemMemory_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	// no mounted memory filesystems
	return NULL;
}

int MFFileMemory_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataMemory), "Incorrect size for MFOpenDataMemory structure. Invalid pOpenData.");
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
	MFString_Copy(pFile->fileIdentifier, MFStr("Memory: 0x%08X", pMem->pMemoryPointer));
#endif

	return 0;
}

int MFFileMemory_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	if(pMem->ownsMemory)
		MFHeap_Free(pMem->pMemoryPointer);

	gMemoryFiles.Destroy(pMem);

	return 0;
}

int MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	uint32 bytesToCopy = fileHandle->length > -1 ? MFMin(bytes, (uint32)fileHandle->length - fileHandle->offset) : bytes;

	MFCopyMemory(pBuffer, &((char*)pMem->pMemoryPointer)[fileHandle->offset], bytesToCopy);
	fileHandle->offset += bytesToCopy;

	return bytesToCopy;
}

int MFFileMemory_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	uint32 bytesToCopy = pMem->allocated ? MFMin(bytes, pMem->allocated - fileHandle->offset) : bytes;

	MFCopyMemory(&((char*)pMem->pMemoryPointer)[fileHandle->offset], pBuffer, bytesToCopy);

	fileHandle->offset += bytesToCopy;
	fileHandle->length = MFMax((int)fileHandle->offset, fileHandle->length);

	return bytesToCopy;
}

int MFFileMemory_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, fileHandle->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0, fileHandle->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0, (int)fileHandle->offset + bytes, fileHandle->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	fileHandle->offset = (uint32)newPos;
	return newPos;
}

int MFFileMemory_Tell(MFFile* fileHandle)
{
	MFCALLSTACK;
	return (int)fileHandle->offset;
}

MFFileState MFFileMemory_Query(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->state;
}

int MFFileMemory_GetSize(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->length;
}
