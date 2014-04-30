#include "Fuji_Internal.h"

#if MF_FILESYSTEM == MF_DRIVER_WIN32

#if defined(MF_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(MF_XBOX)
	#include <xtl.h>

	char *FixXBoxFilename(const char *pFilename);
#endif

#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"

MFInitStatus MFFileSystemNative_InitModulePlatformSpecific()
{
	return MFIS_Succeeded;
}

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

static void CreateDir(const char *pPath)
{
	if(!pPath || *pPath == 0)
		return;

	// strip trailing '/'
	((char*)pPath)[MFString_Length(pPath)-1] = 0;

	// the path is empty
	if(*pPath == 0)
		return;

	CreateDir(MFStr_GetFilePath(pPath));

	CreateDirectory(pPath, NULL);
}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	MFDebug_Assert(pOpenData->openFlags & (MFOF_Read|MFOF_Write), "Neither MFOF_Read nor MFOF_Write specified.");
	MFDebug_Assert((pNative->openFlags & (MFOF_Append|MFOF_Truncate)) != (MFOF_Append|MFOF_Truncate), "MFOF_Append and MFOF_Truncate are mutually exclusive.");
	MFDebug_Assert((pNative->openFlags & (MFOF_Text|MFOF_Binary)) != (MFOF_Text|MFOF_Binary), "MFOF_Text and MFOF_Binary are mutually exclusive.");

	DWORD access = ((pOpenData->openFlags&MFOF_Read) ? GENERIC_READ : NULL) | ((pOpenData->openFlags&MFOF_Write) ? GENERIC_WRITE : NULL);

	const char *pFilename = pNative->pFilename;
#if defined(MF_XBOX) || defined(MF_X360)
	pFilename = FixXBoxFilename(pFilename);
#endif

	if(pOpenData->openFlags & MFOF_CreateDirectory)
		CreateDir(MFStr_GetFilePath(pFilename));

	DWORD create = (pOpenData->openFlags & MFOF_Read) ? ((pOpenData->openFlags & MFOF_Write) ? OPEN_ALWAYS : OPEN_EXISTING) : CREATE_ALWAYS;

	pFile->pFilesysData = CreateFile(pFilename, access, FILE_SHARE_READ, NULL, create, NULL, NULL);

	if(pFile->pFilesysData == INVALID_HANDLE_VALUE)
	{
//		MFDebug_Warn(3, MFStr("Failed to open file '%s'.", pNative->pFilename));
		pFile->pFilesysData = 0;
		return -1;
	}

	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	DWORD excess;
	uint32 fileSize = GetFileSize(pFile->pFilesysData, &excess);

	MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

	pFile->length = fileSize;

	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	CloseHandle(fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	uint32 bytesRead;
	ReadFile(fileHandle->pFilesysData, pBuffer, (DWORD)bytes, (DWORD*)&bytesRead, NULL);
	fileHandle->offset += bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	uint32 bytesWritten;
	WriteFile(fileHandle->pFilesysData, pBuffer, (DWORD)bytes, (LPDWORD)&bytesWritten, NULL);
	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, fileHandle->length);

	return bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity)
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

	DWORD newPos = SetFilePointer(fileHandle->pFilesysData, (LONG)bytes, (LONG*)&bytes + 1, method);
	if(newPos == INVALID_SET_FILE_POINTER)
		return -1;

	bytes = (bytes & 0xFFFFFFFF00000000LL) | (int64)newPos;
	fileHandle->offset = bytes;
	return (int)bytes;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	int64 fileSize = 0;

#if defined(MF_XBOX) || defined(MF_X360)
	pFilename = FixXBoxFilename(pFilename);
#endif

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD excess;
		fileSize = GetFileSize(hFile, &excess);
		CloseHandle(hFile);
		fileSize |= (int64)excess << 32;

		MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");
	}

	return (uint32)fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	bool exists = false;

#if defined(MF_XBOX) || defined(MF_X360)
	pFilename = FixXBoxFilename(pFilename);
#endif

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		exists = true;
	}

	return exists;
}

const char* MFFileNative_MakeAbsolute(const char* pFilename)
{
	char path[256];
	GetFullPathName(pFilename, sizeof(path), path, NULL);
	return MFStr(path);
}

bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
	WIN32_FIND_DATA fd;

	HANDLE hFind = FindFirstFile(MFStr("%s%s", (char*)pFind->pMount->pFilesysData, pSearchPattern), &fd);

	if(hFind == INVALID_HANDLE_VALUE)
		return false;

	BOOL more = TRUE;
	while(!MFString_Compare(fd.cFileName, ".") || !MFString_Compare(fd.cFileName, "..") && more)
		more = FindNextFile(hFind, &fd);
	if(!more)
	{
		FindClose(hFind);
		return false;
	}

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
