#if !defined(_MFFILESYSTEM_CACHEDFILE_INTERNAL_H)
#define _MFFILESYSTEM_CACHEDFILE_INTERNAL_H

#include "FileSystem/MFFileSystemCachedFile.h"

// internal functions
MFInitStatus MFFileSystemCachedFile_InitModule(int moduleId, bool bPerformInitialisation);
void MFFileSystemCachedFile_DeinitModule();

// filesystem callbacks
int MFFileCachedFile_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileCachedFile_Close(MFFile* fileHandle);
int MFFileCachedFile_Read(MFFile* fileHandle, void *pBuffer, int64 bytes);
int MFFileCachedFile_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes);
int MFFileCachedFile_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileCachedFile_GetSize(const char* pFilename);
bool MFFileCachedFile_Exists(const char* pFilename);

#define MFFILESYSTEM_NUMCACHEDBUCKETS 4

// internal data for cached files
struct MFFileCachedBucket
{
	char *pData;
	int size;
	int64 fileOffset;
	int64 lastTouched;
};

struct MFFileCachedData
{
	char *pCache;
	int cacheSize;

	MFFileCachedBucket buckets[MFFILESYSTEM_NUMCACHEDBUCKETS];

	MFFile *pBaseFile;
};

#endif
