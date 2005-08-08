#if !defined(_MFFILESYSTEM_ZIPFILE_H)
#define _MFFILESYSTEM_ZIPFILE_H

struct MFOpenDataZipFile : public MFOpenData
{
	MFFileHandle zipArchiveHandle;
	const char *pFilename;
};

// open file base data
struct MFMountDataZipFile : public MFMountData
{
	MFFileHandle zipArchiveHandle;
};

enum OpenZipFlags
{
	OFZip_AlreadyMounted = MFOF_User
};

// internal functions
void MFFileSystemZipFile_InitModule();
void MFFileSystemZipFile_DeinitModule();

// filesystem callbacks
void MFFileSystemZipFile_Register();
void MFFileSystemZipFile_Unregister();

int MFFileSystemZipFile_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemZipFile_Dismount(MFMount *pMount);

MFFile* MFFileSystemZipFile_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileZipFile_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileZipFile_Close(MFFile* fileHandle);
int MFFileZipFile_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async);
int MFFileZipFile_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async);
int MFFileZipFile_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity);
int MFFileZipFile_Tell(MFFile* fileHandle);

MFFileState MFFileZipFile_Query(MFFile* fileHandle);
int MFFileZipFile_GetSize(MFFile* fileHandle);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileZipFile_GetSize(const char* pFilename);
bool MFFileZipFile_Exists(const char* pFilename);

// handle to the ZipFile filesystem
extern FileSystemHandle hZipFileSystem;

#endif
