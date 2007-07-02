#include "Fuji.h"

#if MF_FILESYSTEM == WIN32

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(MF_XBOX)
	#include <xtl.h>
#endif

#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	DWORD access = ((pOpenData->openFlags&MFOF_Read) ? GENERIC_READ : NULL) | ((pOpenData->openFlags&MFOF_Write) ? GENERIC_WRITE : NULL);
	MFDebug_Assert(access, "Neither MFOF_Read nor MFOF_Write specified.");

	DWORD create = (pOpenData->openFlags&MFOF_Read) ? ((pOpenData->openFlags&MFOF_Write) ? OPEN_ALWAYS : OPEN_EXISTING) : CREATE_ALWAYS;

	pFile->pFilesysData = CreateFile(pNative->pFilename, access, FILE_SHARE_READ, NULL, create, NULL, NULL);

	if(pFile->pFilesysData == INVALID_HANDLE_VALUE)
	{
		MFDebug_Warn(3, MFStr("Failed to open file '%s'.", pNative->pFilename));
		return -1;
	}

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	DWORD excess;
	uint32 fileSize = GetFileSize(pFile->pFilesysData, &excess);

	MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

	pFile->length = fileSize;

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	CloseHandle(fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	ReadFile(fileHandle->pFilesysData, pBuffer, bytes, (DWORD*)&bytesRead, NULL);
	fileHandle->offset += bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesWritten;
	WriteFile(fileHandle->pFilesysData, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);
	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, (uint32)fileHandle->length);

	return bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

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
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	uint32 newPos = SetFilePointer(fileHandle->pFilesysData, bytes, NULL, method);
	if(newPos == INVALID_SET_FILE_POINTER)
		return -1;

	fileHandle->offset = newPos;
	return newPos;
}

int MFFileNative_Tell(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->offset;
}

MFFileState MFFileNative_Query(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->state;
}

int MFFileNative_GetSize(MFFile* fileHandle)
{
	MFCALLSTACK;
	return fileHandle->length;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	DWORD fileSize = 0;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD excess;
		fileSize = GetFileSize(hFile, &excess);
		CloseHandle(hFile);

		MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");
	}

	return fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	bool exists = false;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		exists = true;
	}

	return exists;
}

bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
	WIN32_FIND_DATA fd;

	HANDLE hFind = FindFirstFile(MFStr("%s%s", (char*)pFind->pMount->pFilesysData, pSearchPattern), &fd);

	if(hFind == INVALID_HANDLE_VALUE)
		return false;

	pFindData->attributes = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? MFFA_Directory : 0;
	pFindData->fileSize = (uint64)fd.nFileSizeLow | (((uint64)fd.nFileSizeHigh) << 32);
	MFString_Copy((char*)pFindData->pFilename, fd.cFileName);

	MFString_CopyCat(pFindData->pSystemPath, (char*)pFind->pMount->pFilesysData, pSearchPattern);
	char *pLast = MFString_RChr(pFindData->pSystemPath, '/');
	if(pLast)
		pLast[1] = 0;
	else
		pFindData->pSystemPath[0] = NULL;

	pFind->pFilesystemData = (void*)hFind;

	return true;
}

bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData)
{
	WIN32_FIND_DATA fd;

	BOOL more = FindNextFile((HANDLE)pFind->pFilesystemData, &fd);

	if(!more)
		return false;

	pFindData->attributes = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? MFFA_Directory : 0;
	pFindData->fileSize = (uint64)fd.nFileSizeLow | (((uint64)fd.nFileSizeHigh) << 32);
	MFString_Copy((char*)pFindData->pFilename, fd.cFileName);

	return true;
}

void MFFileNative_FindClose(MFFind *pFind)
{
	FindClose((HANDLE)pFind->pFilesystemData);
}

#endif // MF_FILESYSTEM
