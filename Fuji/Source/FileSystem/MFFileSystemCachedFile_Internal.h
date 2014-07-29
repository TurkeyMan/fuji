#if !defined(_MFFILESYSTEM_CACHEDFILE_INTERNAL_H)
#define _MFFILESYSTEM_CACHEDFILE_INTERNAL_H

#include "FileSystem/MFFileSystemCachedFile.h"

// internal functions
MFInitStatus MFFileSystemCachedFile_InitModule(int moduleId, bool bPerformInitialisation);
void MFFileSystemCachedFile_DeinitModule();

// filesystem callbacks
int MFFileCachedFile_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileCachedFile_Close(MFFile* fileHandle);
size_t MFFileCachedFile_Read(MFFile* fileHandle, void *pBuffer, size_t bytes);
size_t MFFileCachedFile_Write(MFFile* fileHandle, const void *pBuffer, size_t bytes);
uint64 MFFileCachedFile_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);

bool MFFileCachedFile_Stat(const char *pPath, MFFileInfo *pFileInfo);
bool MFFileCachedFile_Delete(const char *pPath, bool bRecursive);

// this is just for convenience sake, and not part of the main filesystem interface
uint64 MFFileCachedFile_GetSize(const char* pFilename);
bool MFFileCachedFile_Exists(const char* pFilename);

#define MFFILESYSTEM_NUMCACHEDBUCKETS 4

// internal data for cached files
struct MFFileCachedBucket
{
	char *pData;
	size_t size;
	uint64 fileOffset;
	uint64 lastTouched;
};

struct MFFileCachedData
{
	char *pCache;
	size_t cacheSize;

	MFFileCachedBucket buckets[MFFILESYSTEM_NUMCACHEDBUCKETS];

	MFFile *pBaseFile;
};

#endif
