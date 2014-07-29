#if !defined(_MFFILESYSTEM_HTTP_INTERNAL_H)
#define _MFFILESYSTEM_HTTP_INTERNAL_H

#include "FileSystem/MFFileSystemHTTP.h"

struct MFFileHTTPData
{
	MFSocketAddressInet address;
	char *pServer;
	char *pPath;
	int port;

	char *pArgString;

	char *pCache;
	size_t cacheSize;
	size_t cacheStart;
};

// internal functions
MFInitStatus MFFileSystemHTTP_InitModule(int moduleId, bool bPerformInitialisation);
void MFFileSystemHTTP_DeinitModule();

// filesystem callbacks
int MFFileSystemHTTP_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemHTTP_Dismount(MFMount *pMount);

MFFile* MFFileSystemHTTP_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileHTTP_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileHTTP_Close(MFFile* fileHandle);
size_t MFFileHTTP_Read(MFFile* fileHandle, void *pBuffer, size_t bytes);
size_t MFFileHTTP_Write(MFFile* fileHandle, const void *pBuffer, size_t bytes);
uint64 MFFileHTTP_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);

bool MFFileHTTP_Stat(const char *pPath, MFFileInfo *pFileInfo);

// this is just for convenience sake, and not part of the main filesystem interface
uint64 MFFileHTTP_GetSize(const char* pFilename);
bool MFFileHTTP_Exists(const char* pFilename);

#endif
