#if !defined(_MFFILESYSTEM_NATIVE_INTERNAL_H)
#define _MFFILESYSTEM_NATIVE_INTERNAL_H

#include "FileSystem/MFFileSystemNative.h"

// internal functions
MFInitStatus MFFileSystemNative_InitModule();
void MFFileSystemNative_DeinitModule();

MFInitStatus MFFileSystemNative_InitModulePlatformSpecific();

// filesystem callbacks
void MFFileSystemNative_Register();
void MFFileSystemNative_Unregister();

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData);
int MFFileSystemNative_Dismount(MFMount *pMount);

MFFile* MFFileSystemNative_Open(MFMount *pMount, const char *pFilename, uint32 openFlags);

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData);
int MFFileNative_Close(MFFile* fileHandle);
int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, int64 bytes);
int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes);
int MFFileNative_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity);
bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData);
bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData);
void MFFileNative_FindClose(MFFind *pFind);

// this is just for convenience sake, and not part of the main filesystem interface
uint32 MFFileNative_GetSize(const char* pFilename);
bool MFFileNative_Exists(const char* pFilename);
const char* MFFileNative_MakeAbsolute(const char* pFilename);

// handle to the native filesystem
extern MFFileSystemHandle hNativeFileSystem;

#endif
