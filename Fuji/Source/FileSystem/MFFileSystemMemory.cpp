#include "Fuji_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemMemory_Internal.h"

struct MFFileSystemMemoryState : public MFFileSystemGlobalState
{
	MFPtrListDL<MFFileMemoryData> gMemoryFiles;
};

int gFileSystemMemoryId = -1;

MFInitStatus MFFileSystemMemory_InitModule(int moduleId, bool bPerformInitialisation)
{
	gFileSystemMemoryId = moduleId;

	if(!bPerformInitialisation)
		return MFIS_Succeeded;

	ALLOC_MODULE_DATA(MFFileSystemMemoryState);

	pModuleData->gMemoryFiles.Init("Memory Files", gDefaults.filesys.maxOpenFiles);

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
	fsCallbacks.FindFirst = NULL;
	fsCallbacks.FindNext = NULL;
	fsCallbacks.FindClose = NULL;
	fsCallbacks.Stat = NULL;
	fsCallbacks.CreateDir = NULL;
	fsCallbacks.Delete = MFFileMemory_Delete;

	pModuleData->hFileSystemHandle = MFFileSystem_RegisterFileSystem("Memory Filesystem", &fsCallbacks);

	return MFIS_Succeeded;
}

void MFFileSystemMemory_DeinitModule()
{
	GET_MODULE_DATA_ID(MFFileSystemMemoryState, gFileSystemMemoryId);

	MFFileSystem_UnregisterFileSystem(pModuleData->hFileSystemHandle);

	pModuleData->gMemoryFiles.Deinit();
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
	GET_MODULE_DATA_ID(MFFileSystemMemoryState, gFileSystemMemoryId);

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataMemory), "Incorrect size for MFOpenDataMemory structure. Invalid pOpenData.");
	MFOpenDataMemory *pMemory = (MFOpenDataMemory*)pOpenData;

	pFile->pFilesysData = pModuleData->gMemoryFiles.Create();
	MFFileMemoryData *pMem = (MFFileMemoryData*)pFile->pFilesysData;

	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;
	pFile->length = pMemory->fileSize;

	pMem->pMemoryPointer = pMemory->pMemoryPointer;
	pMem->allocated = pMemory->allocated;
	pMem->ownsMemory = pMemory->ownsMemory;

	MFString_Copy(pFile->fileIdentifier, MFStr("Memory: 0x%p", pMem->pMemoryPointer));

	return 0;
}

int MFFileMemory_Close(MFFile* fileHandle)
{
	GET_MODULE_DATA_ID(MFFileSystemMemoryState, gFileSystemMemoryId);

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	if(pMem->ownsMemory)
		MFHeap_Free(pMem->pMemoryPointer);

	pModuleData->gMemoryFiles.Destroy(pMem);

	return 0;
}

size_t MFFileMemory_Read(MFFile* fileHandle, void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	size_t bytesToCopy = fileHandle->length > -1 ? MFMin(bytes, (size_t)(fileHandle->length - fileHandle->offset)) : bytes;

	MFCopyMemory(pBuffer, (char*)pMem->pMemoryPointer + fileHandle->offset, bytesToCopy);
	fileHandle->offset += bytesToCopy;

	return bytesToCopy;
}

size_t MFFileMemory_Write(MFFile* fileHandle, const void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	MFFileMemoryData *pMem = (MFFileMemoryData*)fileHandle->pFilesysData;

	size_t bytesToCopy = pMem->allocated ? MFMin(bytes, pMem->allocated - (size_t)fileHandle->offset) : bytes;

	MFCopyMemory((char*)pMem->pMemoryPointer + fileHandle->offset, pBuffer, bytesToCopy);

	fileHandle->offset += bytesToCopy;
	fileHandle->length = MFMax(fileHandle->offset, fileHandle->length);

	return bytesToCopy;
}

uint64 MFFileMemory_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	uint64 newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin((uint64)bytes, fileHandle->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0ULL, fileHandle->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0ULL, fileHandle->offset + bytes, fileHandle->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	fileHandle->offset = newPos;
	return newPos;
}

bool MFFileMemory_Delete(const char *pPath, bool bRecursive)
{
	MFDebug_Assert(false, "Free file");
	return false;
}
