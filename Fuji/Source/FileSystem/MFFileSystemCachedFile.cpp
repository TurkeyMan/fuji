#include "Fuji_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"

struct MFFileSystemCachedFileState : public MFFileSystemGlobalState
{
	MFPtrListDL<MFFileCachedData> gCachedFiles;
};

int gFileSystemCachedFileId = -1;

MFInitStatus MFFileSystemCachedFile_InitModule(int moduleId, bool bPerformInitialisation)
{
	gFileSystemCachedFileId = moduleId;

	if(!bPerformInitialisation)
		return MFIS_Succeeded;

	ALLOC_MODULE_DATA(MFFileSystemCachedFileState);

	pModuleData->gCachedFiles.Init("Cached Files", gDefaults.filesys.maxOpenFiles);

	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = NULL;
	fsCallbacks.UnregisterFS = NULL;
	fsCallbacks.FSMount = NULL;
	fsCallbacks.FSDismount = NULL;
	fsCallbacks.FSOpen = NULL;
	fsCallbacks.Open = MFFileCachedFile_Open;
	fsCallbacks.Close = MFFileCachedFile_Close;
	fsCallbacks.Read = MFFileCachedFile_Read;
	fsCallbacks.Write = MFFileCachedFile_Write;
	fsCallbacks.Seek = MFFileCachedFile_Seek;
	fsCallbacks.FindFirst = NULL;
	fsCallbacks.FindNext = NULL;
	fsCallbacks.FindClose = NULL;
	fsCallbacks.Stat = MFFileCachedFile_Stat;
	fsCallbacks.Delete = MFFileCachedFile_Delete;

	pModuleData->hFileSystemHandle = MFFileSystem_RegisterFileSystem("Cached Filesystem", &fsCallbacks);

	return MFIS_Succeeded;
}

void MFFileSystemCachedFile_DeinitModule()
{
	GET_MODULE_DATA_ID(MFFileSystemCachedFileState, gFileSystemCachedFileId);

	MFFileSystem_UnregisterFileSystem(pModuleData->hFileSystemHandle);

	pModuleData->gCachedFiles.Deinit();
}

int MFFileCachedFile_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	GET_MODULE_DATA_ID(MFFileSystemCachedFileState, gFileSystemCachedFileId);

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataCachedFile), "Incorrect size for MFOpenDataCachedFile structure. Invalid pOpenData.");
	MFOpenDataCachedFile *pCachedFile = (MFOpenDataCachedFile*)pOpenData;

	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;
	pFile->length = pCachedFile->pBaseFile->length;

	pFile->pFilesysData = pModuleData->gCachedFiles.Create();
	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;
	MFZeroMemory(pCacheData, sizeof(MFFileCachedData));

	int64 baseFileSize = pFile->length;

	if(pCachedFile->maxCacheSize > 0)
		pCacheData->cacheSize = (size_t)MFMin(baseFileSize, (int64)pCachedFile->maxCacheSize);
	else
	{
		MFDebug_Assert(baseFileSize < (1LL<<sizeof(size_t))-1, "File is too big for cache!");
		pCacheData->cacheSize = (size_t)baseFileSize;
	}

	pCacheData->pCache = (char*)MFHeap_Alloc(pCacheData->cacheSize);
	pCacheData->pBaseFile = pCachedFile->pBaseFile;

	// calculate bucket sizes
	if(baseFileSize == (int64)pCacheData->cacheSize)
	{
		pCacheData->buckets[0].pData = pCacheData->pCache;
		pCacheData->buckets[0].size = (size_t)baseFileSize;
		pCacheData->buckets[0].fileOffset = 0x7FFFFFFFFFFFFFFFLL;
	}
	else
	{
		size_t bucketSize = pCacheData->cacheSize/MFFILESYSTEM_NUMCACHEDBUCKETS;

		for(int a=0; a<MFFILESYSTEM_NUMCACHEDBUCKETS; a++)
		{
			pCacheData->buckets[a].pData = pCacheData->pCache + a*bucketSize;
			pCacheData->buckets[a].size = bucketSize;
			pCacheData->buckets[a].fileOffset = 0x7FFFFFFFFFFFFFFFLL;
		}
	}

	MFString_Copy(pFile->fileIdentifier, pCachedFile->pBaseFile->fileIdentifier);

	return 0;
}

int MFFileCachedFile_Close(MFFile* pFile)
{
	MFCALLSTACK;

	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;

	if(pFile->createFlags & MFOF_Cached_CleanupBaseFile)
		MFFile_Close(pCacheData->pBaseFile);

	return 0;
}

size_t MFFileCachedFile_Read(MFFile* pFile, void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;

    size_t bytesRead = 0;

	while(bytes && pFile->offset < pFile->length)
	{
		// find bucket
		MFFileCachedBucket *pBucket = NULL;

		for(int a=0; a<MFFILESYSTEM_NUMCACHEDBUCKETS; ++a)
		{
			if(pFile->offset >= pCacheData->buckets[a].fileOffset && pFile->offset < pCacheData->buckets[a].fileOffset + pCacheData->buckets[a].size)
			{
				pBucket = &pCacheData->buckets[a];
				break;
			}
		}

		if(!pBucket)
		{
			// no bucket was found
			if(pFile->length <= pCacheData->cacheSize)
			{
				pBucket = &pCacheData->buckets[0];
				pBucket->fileOffset = 0;
				pBucket->size = (size_t)pFile->length;
			}
			else
			{
				// find the least recently used bucket
				pBucket = &pCacheData->buckets[0];
				for(int a=1; a<MFFILESYSTEM_NUMCACHEDBUCKETS; ++a)
				{
					if(pCacheData->buckets[a].lastTouched < pBucket->lastTouched)
						pBucket = &pCacheData->buckets[a];
				}

				pBucket->fileOffset = MFMin(pFile->offset, pFile->length - pBucket->size);
			}

			MFFile_Seek(pCacheData->pBaseFile, pBucket->fileOffset, MFSeek_Begin);
			size_t read = MFFile_Read(pCacheData->pBaseFile, pBucket->pData, pBucket->size);
			MFDebug_Assert(read == pBucket->size, "Error reading base file...");
		}

		pBucket->lastTouched = MFSystem_ReadRTC();

		uint64 bucketOffset = pFile->offset - pBucket->fileOffset;
		size_t bytesRemaining = pBucket->size - (size_t)bucketOffset;
		size_t bytesToCopy = MFMin(bytes, bytesRemaining);

		MFCopyMemory(pBuffer, pBucket->pData + bucketOffset, bytesToCopy);

		bytes -= bytesToCopy;
		(char*&)pBuffer += bytesToCopy;
		pFile->offset += bytesToCopy;

		bytesRead += bytesToCopy;
	}

	return bytesRead;
}

size_t MFFileCachedFile_Write(MFFile* pFile, const void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "TODO!");

	return 0;
}

uint64 MFFileCachedFile_Seek(MFFile* pFile, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	uint64 newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin((uint64)bytes, pFile->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0ULL, pFile->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0ULL, pFile->offset + bytes, pFile->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	pFile->offset = newPos;
	return pFile->offset;
}

bool MFFileCachedFile_Stat(const char *pPath, MFFileInfo *pFileInfo)
{
	MFDebug_Assert(false, "Stat underlying file");
	return false;
}

bool MFFileCachedFile_Delete(const char *pPath, bool bRecursive)
{
	MFDebug_Assert(false, "Delete underlying file");
	return false;
}
