#if !defined(_MFFILESYSTEM_CACHEDFILE_INTERNAL_H)
#define _MFFILESYSTEM_CACHEDFILE_INTERNAL_H

#include "FileSystem/MFFileSystemCachedFile.h"

// internal functions
void MFFileSystemCachedFile_InitModule();
void MFFileSystemCachedFile_DeinitModule();

// filesystem callbacks
int MFFileCachedFile_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileCachedFile_Close(MFFile* fileHandle);
int MFFileCachedFile_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileCachedFile_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async);
int MFFileCachedFile_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileCachedFile_Tell(MFFile* fileHandle);

MFFileState MFFileCachedFile_Query(MFFile* fileHandle);
int MFFileCachedFile_GetSize(MFFile* fileHandle);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileCachedFile_GetSize(const char* pFilename);
bool MFFileCachedFile_Exists(const char* pFilename);

#define MFFILESYSTEM_NUMCACHEDBUCKETS 4

// internal data for cached files
struct MFFileCachedBucket
{
	char *pData;
	int size;
	uint32 fileOffset;
	uint32 lastTouched;
};

struct MFFileCachedData
{
	char *pCache;
	int cacheSize;

	MFFileCachedBucket buckets[MFFILESYSTEM_NUMCACHEDBUCKETS];

	MFFile *pBaseFile;
};

// handle to the CachedFile filesystem
extern MFFileSystemHandle hCachedFileSystem;

#endif
