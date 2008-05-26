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

	gCachedFiles.Init("Cached Files", gDefaults.filesys.maxOpenFiles);

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

	hCachedFileSystem = MFFileSystem_RegisterFileSystem("Cached Filesystem", &fsCallbacks);
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

	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;
	pFile->length = pCachedFile->pBaseFile->length;

	pFile->pFilesysData = gCachedFiles.Create();
	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;
	MFZeroMemory(pCacheData, sizeof(MFFileCachedData));

	int64 baseFileSize = pFile->length;

	if(pCachedFile->maxCacheSize > 0)
		pCacheData->cacheSize = (int)MFMin(baseFileSize, (int64)pCachedFile->maxCacheSize);
	else
		pCacheData->cacheSize = (int)baseFileSize;

	pCacheData->pCache = (char*)MFHeap_Alloc(pCacheData->cacheSize);
	pCacheData->pBaseFile = pCachedFile->pBaseFile;

	// calculate bucket sizes
	if(baseFileSize == pCacheData->cacheSize)
	{
		pCacheData->buckets[0].pData = pCacheData->pCache;
		pCacheData->buckets[0].size = (int)baseFileSize;
		pCacheData->buckets[0].fileOffset = 0x7FFFFFFFFFFFFFFFLL;
	}
	else
	{
		int bucketSize = pCacheData->cacheSize/MFFILESYSTEM_NUMCACHEDBUCKETS;

		for(int a=0; a<MFFILESYSTEM_NUMCACHEDBUCKETS; a++)
		{
			pCacheData->buckets[a].pData = pCacheData->pCache + a*bucketSize;
			pCacheData->buckets[a].size = bucketSize;
			pCacheData->buckets[a].fileOffset = 0x7FFFFFFFFFFFFFFFLL;
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

int MFFileCachedFile_Read(MFFile* pFile, void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	MFFileCachedData *pCacheData = (MFFileCachedData*)pFile->pFilesysData;

    int64 bytesRead = 0;

	while(bytes && (int)pFile->offset < pFile->length)
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
				pBucket->size = (int)pFile->length;
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

			MFFile_Seek(pCacheData->pBaseFile, (int)pBucket->fileOffset, MFSeek_Begin);
			uint32 read = MFFile_Read(pCacheData->pBaseFile, pBucket->pData, pBucket->size);
			MFDebug_Assert(read == pBucket->size, "Error reading base file...");
		}

		pBucket->lastTouched = (uint32)MFSystem_ReadRTC();

		int64 bucketOffset = pFile->offset - pBucket->fileOffset;
		int64 bytesRemaining = pBucket->size - bucketOffset;
		int64 bytesToCopy = MFMin(bytes, bytesRemaining);

		MFCopyMemory(pBuffer, pBucket->pData + bucketOffset, (uint32)bytesToCopy);

		bytes -= bytesToCopy;
		(char*&)pBuffer += bytesToCopy;
		pFile->offset += bytesToCopy;

		bytesRead += bytesToCopy;
	}

	return (int)bytesRead;
}

int MFFileCachedFile_Write(MFFile* pFile, const void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	// write

	return 0;
}

int MFFileCachedFile_Seek(MFFile* pFile, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int64 newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, pFile->length);
			break;
		case MFSeek_End:
			newPos = MFMax((int64)0, pFile->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp((int64)0, pFile->offset + bytes, pFile->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	pFile->offset = newPos;

	return (int)pFile->offset;
}
