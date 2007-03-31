#if !defined(_MFFILESYSTEM_HTTP_INTERNAL_H)
#define _MFFILESYSTEM_HTTP_INTERNAL_H

#include "FileSystem/MFFileSystemHTTP.h"

struct MFFileHTTPData
{
	char *pServer;
	char *pPath;
	int port;

	char *pCache;
	uint32 cacheSize;
	uint32 cacheStart;
};

// internal functions
void MFFileSystemHTTP_InitModule();
void MFFileSystemHTTP_DeinitModule();

// filesystem callbacks
int MFFileSystemHTTP_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemHTTP_Dismount(MFMount *pMount);

MFFile* MFFileSystemHTTP_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileHTTP_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileHTTP_Close(MFFile* fileHandle);
int MFFileHTTP_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileHTTP_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async);
int MFFileHTTP_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileHTTP_Tell(MFFile* fileHandle);

MFFileState MFFileHTTP_Query(MFFile* fileHandle);
int MFFileHTTP_GetSize(MFFile* fileHandle);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileHTTP_GetSize(const char* pFilename);
bool MFFileHTTP_Exists(const char* pFilename);

// handle to the HTTP filesystem
extern MFFileSystemHandle hHTTPFileSystem;

#endif
