#include "Fuji.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"

#include <stdio.h>
#include <dirent.h>

void MFFileSystemNative_Register()
{
}

void MFFileSystemNative_Unregister()
{
}

int MFFileSystemNative_GetNumEntries(const char *pFindPattern, bool recursive, bool flatten, int *pStringLengths)
{
	int numFiles = 0;

	DIR *pDir;
	dirent *pD;

	*pStringLengths += MFString_Length(pFindPattern) + 1;

	pDir = opendir(pFindPattern);

	while((pD = readdir(pDir)))
	{
		if(MFString_Compare(pD->d_name, ".") && MFString_Compare(pD->d_name, "..") && MFString_Compare(pD->d_name, ".svn"))
		{
			// check if the entry is a directory..
			const char *pSubDir = MFStr("%s%s/", pFindPattern, pD->d_name);
			DIR *pIsDir = opendir(pSubDir);

			if(pIsDir)
			{
				closedir(pIsDir);

				if(recursive)
				{
					if(flatten)
					{
						numFiles += MFFileSystemNative_GetNumEntries(pSubDir, recursive, flatten, pStringLengths);
					}
					else
					{
						*pStringLengths += MFString_Length(pD->d_name) + 1;
						++numFiles;
					}
				}
			}
			else
			{
				*pStringLengths += MFString_Length(pD->d_name) + 1;
				++numFiles;
			}
		}
	}

	closedir(pDir);

	return numFiles;
}

MFTOCEntry* MFFileSystemNative_BuildToc(const char *pFindPattern, MFTOCEntry *pToc, MFTOCEntry *pParent, char* &pStringCache, bool recursive, bool flatten)
{
	DIR *pDir;
	dirent *pD;

	pDir = opendir(pFindPattern);

	char *pCurrentDir = pStringCache;
	MFString_Copy(pCurrentDir, pFindPattern);
	pStringCache += MFString_Length(pCurrentDir) + 1;

	while((pD = readdir(pDir)))
	{
		if(MFString_Compare(pD->d_name, ".") && MFString_Compare(pD->d_name, "..") && MFString_Compare(pD->d_name, ".svn"))
		{
			// check if the entry is a directory..
			const char *pSubDir = MFStr("%s%s/", pFindPattern, pD->d_name);
			DIR *pIsDir = opendir(pSubDir);

			if(pIsDir)
			{
				closedir(pIsDir);

				if(recursive)
				{
					if(flatten)
					{
						pToc = MFFileSystemNative_BuildToc(pSubDir, pToc, pParent, pStringCache, recursive, flatten);
					}
					else
					{
						const char *pNewPath = pSubDir;

						int stringCacheSize = 0;
						pToc->size = MFFileSystemNative_GetNumEntries(pNewPath, recursive, flatten, &stringCacheSize);

						if(pToc->size)
						{
							MFString_Copy(pStringCache, pD->d_name);
							pToc->pName = pStringCache;
							pStringCache += MFString_Length(pStringCache)+1;

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
				MFString_Copy(pStringCache, pD->d_name);
				pToc->pName = pStringCache;
				pStringCache += MFString_Length(pStringCache)+1;

				pToc->pFilesysData = pCurrentDir;

				pToc->pParent = pParent;
				pToc->pChild = NULL;

				pToc->flags = 0;
				pToc->size = 0;

				++pToc;
			}
		}
	}

	closedir(pDir);

	return pToc;
}

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataNative), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataNative *pMountNative = (MFMountDataNative*)pMountData;

	DIR *pDir;

	bool flatten = (pMountData->flags & MFMF_FlattenDirectoryStructure) != 0;
	bool recursive = (pMountData->flags & MFMF_Recursive) != 0;

	const char *pFindPattern = pMountNative->pPath;

	if(pFindPattern[MFString_Length(pFindPattern)-1] != '/')
		pFindPattern = MFStr("%s/", pFindPattern);

	pDir = opendir(pFindPattern);

	if(!pDir)
	{
		MFDebug_Warn(1, MFStr("FileSystem: Couldnt Mount Native FileSystem '%s'.", pMountNative->pPath));
		return -1;
	}

	closedir(pDir);

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
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	const char *pAccess = (pOpenData->openFlags&MFOF_Write) ? "wb" : ((pOpenData->openFlags&MFOF_Read) ? "rb" : NULL);
	MFDebug_Assert(pAccess, "Neither MFOF_Read nor MFOF_Write specified.");

	pFile->pFilesysData = fopen(pNative->pFilename, pAccess);

	if(!pFile->pFilesysData)
	{
		return -1;
	}

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	fseek((FILE*)pFile->pFilesysData, 0, SEEK_END);
	uint32 fileSize = ftell((FILE*)pFile->pFilesysData);
	fseek((FILE*)pFile->pFilesysData, 0, SEEK_SET);

	pFile->length = fileSize;

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	fclose((FILE*)fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	bytesRead = fread(pBuffer, 1, bytes, (FILE*)fileHandle->pFilesysData);
	fileHandle->offset += bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesWritten;
	bytesWritten = fwrite(pBuffer, 1, bytes, (FILE*)fileHandle->pFilesysData);
	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, (uint32)fileHandle->length);

	return bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int method = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			method = SEEK_SET;
			break;
		case MFSeek_End:
			method = SEEK_END;
			break;
		case MFSeek_Current:
			method = SEEK_CUR;
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	fseek((FILE*)fileHandle->pFilesysData, bytes, method);
	long newPos = ftell((FILE*)fileHandle->pFilesysData);
	if(newPos == -1)
		return -1;

	fileHandle->offset = (uint32)newPos;
	return (int)newPos;
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

	uint32 fileSize = 0;
	FILE *pFile = fopen(pFilename, "rb");

	if(pFile)
	{
		fileSize = fseek(pFile, 0, SEEK_END);
		fclose(pFile);
	}

	return fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	bool exists = false;

	FILE *pFile = fopen(pFilename, "r");

	if(pFile)
	{
		fclose(pFile);
		exists = true;
	}

	return exists;
}
