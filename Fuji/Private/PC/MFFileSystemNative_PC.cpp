#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

void* MFFileSystemNative_Mount(void *pMountData, uint32 flags)
{

	return NULL;
}

MFFile* MFFileSystemNative_Open(const char *pFilename, uint32 openFlags)
{
	MFOpenDataNative openData;

	openData.cbSize = sizeof(MFOpenDataNative);
	openData.openFlags = openFlags;
	openData.pFilename = pFilename;

	return MFFile_Open(hNativeFileSystem, &openData);
}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	CALLSTACK;

	DBGASSERT(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	DWORD access = ((pOpenData->openFlags&MFOF_Read) ? GENERIC_READ : NULL) | ((pOpenData->openFlags&MFOF_Write) ? GENERIC_WRITE : NULL);
	DBGASSERT(access, "Neither MFOF_Read nor MFOF_Write specified.");

	pFile->pFilesysData = CreateFile(pNative->pFilename, access, FILE_SHARE_READ, NULL, (pOpenData->openFlags&MFOF_Write) ? OPEN_ALWAYS : OPEN_EXISTING, NULL, NULL);

	if(pFile->pFilesysData == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	DWORD excess;
	uint32 fileSize = GetFileSize(pFile->pFilesysData, &excess);

	DBGASSERT(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

	pFile->length = fileSize;

#if defined(_DEBUG)
	strcpy(pFile->fileIdentifier, pNative->pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	CALLSTACK;

	CloseHandle(fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	DBGASSERT(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	ReadFile(fileHandle->pFilesysData, pBuffer, bytes, (DWORD*)&bytesRead, NULL);
	fileHandle->offset += bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	uint32 bytesWritten;
	WriteFile(fileHandle->pFilesysData, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);
	fileHandle->offset += bytesWritten;
	fileHandle->length = Max(fileHandle->offset, (uint32)fileHandle->length);

	return bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	CALLSTACK;

	DWORD method = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			method = FILE_BEGIN;
			break;
		case MFSeek_End:
			method = FILE_END;
			break;
		case MFSeek_Current:
			method = FILE_CURRENT;
			break;
		default:
			DBGASSERT(false, "Invalid 'relativity' for file seeking.");
	}

	uint32 newPos = SetFilePointer(fileHandle->pFilesysData, bytes, NULL, method);
	fileHandle->offset = newPos;
	return newPos;
}

int MFFileNative_Tell(MFFile* fileHandle)
{
	CALLSTACK;
	return fileHandle->offset;
}

MFFileState MFFileNative_Query(MFFile* fileHandle)
{
	CALLSTACK;
	return fileHandle->state;
}

int MFFileNative_GetSize(MFFile* fileHandle)
{
	CALLSTACK;
	return fileHandle->length;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	CALLSTACK;

	DWORD fileSize = 0;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD excess;
		fileSize = GetFileSize(hFile, &excess);
		CloseHandle(hFile);

		DBGASSERT(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");
	}

	return fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	CALLSTACK;

	bool exists = false;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		exists = true;
	}

	return exists;
}
