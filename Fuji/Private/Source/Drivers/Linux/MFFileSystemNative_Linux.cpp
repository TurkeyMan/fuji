#include "Fuji.h"

#if MF_FILESYSTEM == MF_DRIVER_LINUX

#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"
#include "MFHeap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

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

	int flags = 0;

	if(pOpenData->openFlags & MFOF_Read)
	{
		if(pNative->openFlags & MFOF_Write)
		{
			flags = O_RDWR | O_CREAT;
		}
		else
		{
			flags = O_RDONLY;
		}
	}
	else if(pOpenData->openFlags & MFOF_Write)
	{
		flags = O_WRONLY | O_CREAT;
	}
	else
	{
		MFDebug_Assert(0, "Neither MFOF_Read nor MFOF_Write specified.");
	}

	int file = open(pNative->pFilename, flags);
	if(file == -1)
	{
		MFDebug_Warn(3, MFStr("Failed to open file '%s'.", pNative->pFilename));
		pFile->pFilesysData = 0;
		return -1;
	}

	pFile->pFilesysData = (void*)file;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	struct stat fileStats;
	if(fstat(file, &fileStats) == -1)
	{
		close(file);
		pFile->pFilesysData = 0;
		return -1;
	}

	pFile->length = fileStats.st_size;

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);
#endif

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	close((int)fileHandle->pFilesysData);
	fileHandle->pFilesysData = 0;

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, uint64 bytes)
{
	MFCALLSTACK;

	int bytesRead = 0;

	bytesRead = read((int)fileHandle->pFilesysData, pBuffer, (unsigned int)bytes);
	if(bytesRead < 0) // read() returns -1 on error
		bytesRead = 0;

	fileHandle->offset += bytesRead;

	return (int)bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, uint64 bytes)
{
	MFCALLSTACK;

	int bytesWritten = 0;

	bytesWritten = write((int)fileHandle->pFilesysData, pBuffer, (unsigned int)bytes);
	if(bytesWritten < 0) // write() returns -1 on error
		bytesWritten = 0;

	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, fileHandle->length);

	return (int)bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	if(!fileHandle->pFilesysData)
		return -1;

	int whence = 0;
	switch(relativity)
	{
		case MFSeek_Begin:
			whence = SEEK_SET;
			break;
		case MFSeek_End:
			whence = SEEK_END;
			break;
		case MFSeek_Current:
			whence = SEEK_CUR;
			break;
		default:
			MFDebug_Assert(false, "Invalid 'whence'.");
			break;
	}

	off_t newOffset = lseek((int)fileHandle->pFilesysData, (long)bytes, whence);
//	lseek((int)fileHandle->pFilesysData, bytes, whence);
//	newOffset = tell((int)fileHandle->pFilesysData);

	if(newOffset != -1)
	{
		fileHandle->offset = newOffset;
	}

	return (int)fileHandle->offset;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

	struct stat fileStats;
	if(stat(pFilename, &fileStats) == -1)
		return 0;

	return fileStats.st_size;
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

	struct stat fileStats;

	if(stat(pFilename, &fileStats) < 0)
		return false;

	return true;
}

bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
	// separate path and search pattern..
	char *pPath = (char*)MFStr("%s%s", (char*)pFind->pMount->pFilesysData, pSearchPattern);
	const char *pPattern = pPath;

	char *pLast = MFString_RChr(pPath, '/');
	if(pLast)
	{
		*pLast = 0;
		pPattern = pLast + 1;
	}
	else
	{
		// find pattern refers to current directory..
		pPath = ".";
	}

	// open the directory
	DIR *hFind = opendir(pPath);

	if(!hFind)
	{
		MFDebug_Warn(2, MFStr("Couldnt open directory '%s' with search pattern '%s'", pPath, pPattern));
		return false;
	}

	dirent *pFD = readdir(hFind);

	if(!pFD)
		return false;

	struct stat statbuf;
	if(stat(pFD->d_name, &statbuf) < 0)
		return false;

	pFindData->attributes = (S_ISDIR(statbuf.st_mode) ? MFFA_Directory : 0) |
							(S_ISLNK(statbuf.st_mode) ? MFFA_SymLink : 0);
	pFindData->fileSize = (uint64)statbuf.st_size;
	MFString_Copy((char*)pFindData->pFilename, pFD->d_name);

	MFString_CopyCat(pFindData->pSystemPath, (char*)pFind->pMount->pFilesysData, pSearchPattern);
	pLast = MFString_RChr(pFindData->pSystemPath, '/');
	if(pLast)
		pLast[1] = 0;
	else
		pFindData->pSystemPath[0] = 0;

	pFind->pFilesystemData = (void*)hFind;

	return true;
}

bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData)
{
	dirent *pFD = readdir((DIR*)pFind->pFilesystemData);

	if(!pFD)
		return false;

	struct stat statbuf;
	if(stat(pFD->d_name, &statbuf) < 0)
		return false;

	pFindData->attributes = (S_ISDIR(statbuf.st_mode) ? MFFA_Directory : 0) |
							(S_ISLNK(statbuf.st_mode) ? MFFA_SymLink : 0);
	pFindData->fileSize = (uint64)statbuf.st_size;
	MFString_Copy((char*)pFindData->pFilename, pFD->d_name);

	return true;
}

void MFFileNative_FindClose(MFFind *pFind)
{
	closedir((DIR*)pFind->pFilesystemData);
}

#endif // MF_DRIVER_LINUX
