#include "Fuji.h"
#include "MFSystem.h"
#include "MFHeap.h"
#include "MFPtrList.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemCachedFile_Internal.h"

MFPtrListDL<MFFileCachedData> gCachedFiles;

void MFFileSystemCachedFile_InitModule()
{
	MFCALLSTACK;

	gCachedFiles.Init("Memory Files", gDefaults.filesys.maxOpenFiles);

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
	fsCallbacks.Tell = MFFileCachedFile_Tell;
	fsCallbacks.Query = MFFileCachedFile_Query;
	fsCallbacks.GetSize = MFFileCachedFile_GetSize;
	fsCallbacks.FindFirst = NULL;
	fsCallbacks.FindNext = NULL;
	fsCallbacks.FindClose = NULL;

	hCachedFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemCachedFile_DeinitModule()
{
	MFCALLSTACK;

	MFFileSystem_UnregisterFileSystem(hCachedFileSystem);

	gCachedFiles.Deinit();
}

int MFFileCachedFile_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataCachedFile), "Incorrect size for MFOpenDataCachedFile structure. Invalid pOpenData.");
	MFOpenDataCachedFile *pCachedFile = (MFOpenDataCachedFile*)pOpenData;

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;
	pFile->length = pCachedFile->pBaseFile->length;

	pFile->pFilesysData = gCachedFiles.Create();
	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;
	MFZeroMemory(pCacheData, sizeof(MFFileCachedData));

	int baseFileSize = pFile->length;

	if(pCachedFile->maxCacheSize > 0)
		pCacheData->cacheSize = MFMin(baseFileSize, pCachedFile->maxCacheSize);
	else
		pCacheData->cacheSize = baseFileSize;

	pCacheData->pCache = (char*)MFHeap_Alloc(pCacheData->cacheSize);
	pCacheData->pBaseFile = pCachedFile->pBaseFile;

	// calculate bucket sizes
	if(baseFileSize == pCacheData->cacheSize)
	{
		pCacheData->buckets[0].pData = pCacheData->pCache;
		pCacheData->buckets[0].size = baseFileSize;
		pCacheData->buckets[0].fileOffset = 0xFFFFFFFF;
	}
	else
	{
		int bucketSize = pCacheData->cacheSize/MFFILESYSTEM_NUMCACHEDBUCKETS;

		for(int a=0; a<MFFILESYSTEM_NUMCACHEDBUCKETS; a++)
		{
			pCacheData->buckets[a].pData = pCacheData->pCache + a*bucketSize;
			pCacheData->buckets[a].size = bucketSize;
			pCacheData->buckets[a].fileOffset = 0xFFFFFFFF;
		}
	}

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pCachedFile->pBaseFile->fileIdentifier);
#endif

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

int MFFileCachedFile_Read(MFFile* pFile, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;

    int bytesRead = 0;

	while(bytes)
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
			if(pCacheData->pBaseFile->length <= pCacheData->cacheSize)
			{
				pBucket = &pCacheData->buckets[0];
				pBucket->fileOffset = 0;
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

				pBucket->fileOffset = MFMin(pFile->offset, (uint32)(pFile->length - pBucket->size));
			}

			MFFile_Seek(pCacheData->pBaseFile, pBucket->fileOffset, MFSeek_Begin);
			uint32 read = MFFile_Read(pCacheData->pBaseFile, pBucket->pData, pBucket->size);

			// if there are less bytes left in the file than we are trying to read, then we need to update the read count...
			bytes = MFMin(bytes, read);
		}

		pBucket->lastTouched = (uint32)MFSystem_ReadRTC();

		uint32 bucketOffset = pFile->offset - pBucket->fileOffset;
		uint32 bytesRemaining = pBucket->size - bucketOffset;
		int bytesToCopy = MFMin(bytes, bytesRemaining);

		MFCopyMemory(pBuffer, pBucket->pData + bucketOffset, bytesToCopy);

		bytes -= bytesToCopy;
		(char*&)pBuffer += bytesToCopy;
		pFile->offset += bytesToCopy;

		bytesRead += bytesToCopy;
	}

	return bytesRead;
}

int MFFileCachedFile_Write(MFFile* pFile, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	// write

	return 0;
}

int MFFileCachedFile_Seek(MFFile* pFile, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, pFile->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0, pFile->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0, (int)pFile->offset + bytes, pFile->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	pFile->offset = (uint32)newPos;

	return (int)pFile->offset;
}

int MFFileCachedFile_Tell(MFFile* pFile)
{
	MFCALLSTACK;
	return (int)pFile->offset;
}

MFFileState MFFileCachedFile_Query(MFFile* pFile)
{
	MFCALLSTACK;
	return pFile->state;
}

int MFFileCachedFile_GetSize(MFFile* pFile)
{
	MFCALLSTACK;
	return pFile->length;
}
