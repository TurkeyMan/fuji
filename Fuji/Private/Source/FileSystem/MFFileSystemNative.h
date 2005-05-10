#if !defined(_MFFILESYSTEM_NATIVE_H)
#define _MFFILESYSTEM_NATIVE_H

struct MFOpenDataNative : public MFOpenData
{
	const char *pFilename;
};

// open file base data
struct MFMountDataNative : public MFMountData
{
	const char *pPath;
};

// internal functions
void MFFileSystemNative_InitModule();
void MFFileSystemNative_DeinitModule();

// filesystem callbacks
void MFFileSystemNative_Register();
void MFFileSystemNative_Unregister();

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemNative_Dismount(MFMount *pMount);

MFFile* MFFileSystemNative_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileNative_Close(MFFile* fileHandle);
int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileNative_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileNative_Tell(MFFile* fileHandle);

MFFileState MFFileNative_Query(MFFile* fileHandle);
int MFFileNative_GetSize(MFFile* fileHandle);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileNative_GetSize(const char* pFilename);
bool MFFileNative_Exists(const char* pFilename);

// handle to the native filesystem
extern FileSystemHandle hNativeFileSystem;

#endif