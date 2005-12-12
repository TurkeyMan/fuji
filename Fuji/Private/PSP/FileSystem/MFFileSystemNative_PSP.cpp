#include "Fuji.h"
#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"

#include <pspkernel.h>
#include <stdio.h>

void MFFileSystemNative_Register()
{

}

void MFFileSystemNative_Unregister()
{

}

int MFFileSystemNative_GetNumEntries(const char *pFindPattern, bool recursive, bool flatten, int *pStringLengths)
{
	SceIoDirent findData;
	int findStatus;
	int numFiles = 0;

	memset(&findData, 0, sizeof(SceIoDirent));

	*pStringLengths += strlen(pFindPattern) + 1;

	SceUID hFind = sceIoDopen(pFindPattern);

	findStatus = sceIoDread(hFind, &findData);

	while(findStatus > 0)
	{
		if(strcmp(findData.d_name, ".") && strcmp(findData.d_name, "..") && strcmp(findData.d_name, ".svn"))
		{
			if(FIO_SO_ISDIR(findData.d_stat.st_attr))
			{
				if(recursive)
				{
					if(flatten)
					{
						numFiles += MFFileSystemNative_GetNumEntries(MFStr("%s%s/", pFindPattern, findData.d_name), recursive, flatten, pStringLengths);
					}
					else
					{
						*pStringLengths += strlen(findData.d_name) + 1;
						++numFiles;
					}
				}
			}
			else
			{
				*pStringLengths += strlen(findData.d_name) + 1;
				++numFiles;
			}
		}

		findStatus = sceIoDread(hFind, &findData);
	}

	MFDebug_Assert(findStatus == 0, "Error scanning directory...");

	sceIoDclose(hFind);

	return numFiles;
}

MFTOCEntry* MFFileSystemNative_BuildToc(const char *pFindPattern, MFTOCEntry *pToc, MFTOCEntry *pParent, char* &pStringCache, bool recursive, bool flatten)
{
	SceIoDirent findData;
	int findStatus;

	memset(&findData, 0, sizeof(SceIoDirent));

	SceUID hFind = sceIoDopen(pFindPattern);

	findStatus = sceIoDread(hFind, &findData);

	char *pCurrentDir = pStringCache;
	strcpy(pCurrentDir, pFindPattern);
	pStringCache += strlen(pCurrentDir) + 1;

	while(findStatus > 0)
	{
		if(strcmp(findData.d_name, ".") && strcmp(findData.d_name, "..") && strcmp(findData.d_name, ".svn"))
		{
			if(FIO_SO_ISDIR(findData.d_stat.st_attr))
			{
				if(recursive)
				{
					if(flatten)
					{
						pToc = MFFileSystemNative_BuildToc(MFStr("%s%s/", pFindPattern, findData.d_name), pToc, pParent, pStringCache, recursive, flatten);
					}
					else
					{
						const char *pNewPath = MFStr("%s%s/", pFindPattern, findData.d_name);

						int stringCacheSize = 0;
						pToc->size = MFFileSystemNative_GetNumEntries(pNewPath, recursive, flatten, &stringCacheSize);

						if(pToc->size)
						{
							strcpy(pStringCache, findData.d_name);
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
				strcpy(pStringCache, findData.d_name);
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

		findStatus = sceIoDread(hFind, &findData);
	}

	MFDebug_Assert(findStatus == 0, "Error scanning directory...");

	sceIoDclose(hFind);

	return pToc;
}

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataNative), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataNative *pMountNative = (MFMountDataNative*)pMountData;

	bool flatten = (pMountData->flags & MFMF_FlattenDirectoryStructure) != 0;
	bool recursive = (pMountData->flags & MFMF_Recursive) != 0;

	const char *pFindPattern = pMountNative->pPath;

	if(pFindPattern[strlen(pFindPattern)-1] != '/')
		pFindPattern = MFStr("%s/", pFindPattern);

	SceUID hFind = sceIoDopen(pFindPattern);

	if(hFind < 0)
	{
		MFDebug_Warn(1, MFStr("FileSystem: Couldnt Mount Native FileSystem '%s'.", pMountNative->pPath));
		return -1;
	}

	sceIoDclose(hFind);

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

	int access = ((pOpenData->openFlags&MFOF_Read) ? PSP_O_RDONLY : NULL) | ((pOpenData->openFlags&MFOF_Write) ? PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC : NULL);
	MFDebug_Assert(access, "Neither MFOF_Read nor MFOF_Write specified.");

	SceUID hFile = sceIoOpen(pNative->pFilename, access, 0777);

	if(hFile < 0)
	{
		MFDebug_Assert(hFile >= 0, MFStr("File does not exist: '%s'", pNative->pFilename));
		return -1;
	}

	pFile->pFilesysData = (void*)hFile;
	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	// find file length
	SceOff fileSize = sceIoLseek(hFile, 0, SEEK_END);
	MFDebug_Assert(fileSize < 2147483648LL, "Fuji does not support files larger than 2,147,483,647 bytes.");
	pFile->length = (int)fileSize;

	// return to start of file
	sceIoLseek32(hFile, 0, SEEK_SET);

#if defined(_DEBUG)
	strcpy(pFile->fileIdentifier, pNative->pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	sceIoClose((SceUID)fileHandle->pFilesysData);

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	int bytesRead;
	bytesRead = sceIoRead((SceUID)fileHandle->pFilesysData, pBuffer, bytes);
	fileHandle->offset += (uint32)bytesRead;

	return bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	int bytesWritten;
	bytesWritten = sceIoWrite((SceUID)fileHandle->pFilesysData, pBuffer, bytes);
	fileHandle->offset += (uint32)bytesWritten;
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

	SceOff newPos = sceIoLseek((SceUID)fileHandle->pFilesysData, bytes, method);
	fileHandle->offset = (uint32)newPos;
	return newPos;
}

int MFFileNative_Tell(MFFile* fileHandle)
{
	return fileHandle->offset;
}

MFFileState MFFileNative_Query(MFFile* fileHandle)
{
	return fileHandle->state;
}

int MFFileNative_GetSize(MFFile* fileHandle)
{
	return fileHandle->length;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	uint32 fileSize = 0;

	SceUID hFile = sceIoOpen(pFilename, PSP_O_RDONLY, 0777);

	if(hFile > 0)
	{
		SceOff fileSize = sceIoLseek(hFile, 0, SEEK_END);
		MFDebug_Assert(fileSize < (SceOff)4294967296ULL, "Fuji does not support files larger than 4,294,967,295 bytes.");
		fileSize = (uint32)fileSize;

		sceIoClose(hFile);
	}

	return fileSize;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	bool exists = false;

	SceUID hFile = sceIoOpen(pFilename, PSP_O_RDONLY, 0777);

	if(hFile > 0)
	{
		sceIoClose(hFile);
		exists = true;
	}

	return exists;
}
