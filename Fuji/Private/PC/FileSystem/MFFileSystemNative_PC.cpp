#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Fuji.h"
#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

int MFFileSystemNative_GetNumEntries(const char *pFindPattern, bool recursive, bool flatten, int *pStringLengths)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind;

	int numFiles = 0;

	*pStringLengths += strlen(pFindPattern) + 1;

	hFind = FindFirstFile(MFStr("%s*", pFindPattern), &findData);

	while(hFind != INVALID_HANDLE_VALUE)
	{
		if(strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, "..") && strcmp(findData.cFileName, ".svn"))
		{
			if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if(recursive)
				{
					if(flatten)
					{
						numFiles += MFFileSystemNative_GetNumEntries(MFStr("%s%s/", pFindPattern, findData.cFileName), recursive, flatten, pStringLengths);
					}
					else
					{
						*pStringLengths += strlen(findData.cFileName) + 1;
						++numFiles;
					}
				}
			}
			else
			{
				*pStringLengths += strlen(findData.cFileName) + 1;
				++numFiles;
			}
		}

		if(!FindNextFile(hFind, &findData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return numFiles;
}

MFTOCEntry* MFFileSystemNative_BuildToc(const char *pFindPattern, MFTOCEntry *pToc, MFTOCEntry *pParent, char* &pStringCache, bool recursive, bool flatten)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind;

	hFind = FindFirstFile(MFStr("%s*", pFindPattern), &findData);

	char *pCurrentDir = pStringCache;
	strcpy(pCurrentDir, pFindPattern);
	pStringCache += strlen(pCurrentDir) + 1;

	while(hFind != INVALID_HANDLE_VALUE)
	{
		if(strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, "..") && strcmp(findData.cFileName, ".svn"))
		{
			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(recursive)
				{
					if(flatten)
					{
						pToc = MFFileSystemNative_BuildToc(MFStr("%s%s/", pFindPattern, findData.cFileName), pToc, pParent, pStringCache, recursive, flatten);
					}
					else
					{
						const char *pNewPath = MFStr("%s%s/", pFindPattern, findData.cFileName);

						int stringCacheSize = 0;
						pToc->size = MFFileSystemNative_GetNumEntries(pNewPath, recursive, flatten, &stringCacheSize);

						if(pToc->size)
						{
							strcpy(pStringCache, findData.cFileName);
							pToc->pName = pStringCache;
							pStringCache += strlen(pStringCache)+1;

							pToc->flags = MFTF_Directory;
							pToc->pFilesysData = pCurrentDir;
							pToc->pParent = pParent;

							int sizeOfToc = sizeof(MFTOCEntry)*pToc->size;
							pToc->pChild = (MFTOCEntry*)MFHeap_Alloc(sizeof(MFTOCEntry)*sizeOfToc + stringCacheSize);

							char *pNewStringCache = ((char*)pToc->pChild)+sizeOfToc;
							MFFileSystemNative_BuildToc(pNewPath, pToc->pChild, pToc, pNewStringCache, recursive, flatten);

							++pToc;
						}
					}
				}
			}
			else
			{
				strcpy(pStringCache, findData.cFileName);
				pToc->pName = pStringCache;
				pStringCache += strlen(pStringCache)+1;

				pToc->pFilesysData = pCurrentDir;

				pToc->pParent = pParent;
				pToc->pChild = NULL;

				pToc->flags = 0;
				pToc->size = 0;

				++pToc;
			}
		}

		if(!FindNextFile(hFind, &findData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}

	return pToc;
}

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataNative), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataNative *pMountNative = (MFMountDataNative*)pMountData;

	WIN32_FIND_DATA findData;
	HANDLE hFind;

	bool flatten = (pMountData->flags & MFMF_FlattenDirectoryStructure) != 0;
	bool recursive = (pMountData->flags & MFMF_Recursive) != 0;

	const char *pFindPattern = pMountNative->pPath;

	if(pFindPattern[strlen(pFindPattern)-1] != '/')
		pFindPattern = MFStr("%s/", pFindPattern);

	hFind = FindFirstFile(MFStr("%s*", pFindPattern), &findData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		MFDebug_Warn(1, MFStr("FileSystem: Couldnt Mount Native FileSystem '%s'.", pMountNative->pPath));
		return -1;
	}

	FindClose(hFind);

	int stringCacheSize = 0;
	pMount->numFiles = MFFileSystemNative_GetNumEntries(pFindPattern, recursive, flatten, &stringCacheSize);

	int sizeOfToc = sizeof(MFTOCEntry)*pMount->numFiles;
	pMount->pEntries = (MFTOCEntry*)MFHeap_Alloc(sizeOfToc + stringCacheSize);

	char *pStringCache = ((char*)pMount->pEntries)+sizeOfToc;
	MFFileSystemNative_BuildToc(pFindPattern, pMount->pEntries, NULL, pStringCache, recursive, flatten);

	return 0;
}

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	CALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	DWORD access = ((pOpenData->openFlags&MFOF_Read) ? GENERIC_READ : NULL) | ((pOpenData->openFlags&MFOF_Write) ? GENERIC_WRITE : NULL);
	MFDebug_Assert(access, "Neither MFOF_Read nor MFOF_Write specified.");

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

	MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

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

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	ReadFile(fileHandle->pFilesysData, pBuffer, bytes, (DWORD*)&bytesRead, NULL);
	fileHandle->offset += bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesWritten;
	WriteFile(fileHandle->pFilesysData, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);
	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, (uint32)fileHandle->length);

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

		MFDebug_Assert(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");
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
