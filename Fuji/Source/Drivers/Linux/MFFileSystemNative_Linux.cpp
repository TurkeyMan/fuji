#include "Fuji_Internal.h"

#if MF_FILESYSTEM == MF_DRIVER_LINUX

#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"
#include "MFHeap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

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

int MFFileNative_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	MFDebug_Assert(pOpenData->openFlags & (MFOF_Read|MFOF_Write), "Neither MFOF_Read nor MFOF_Write specified.");
//	MFDebug_Assert((pNative->openFlags & (MFOF_Append|MFOF_Truncate)) != (MFOF_Append|MFOF_Truncate), "MFOF_Append and MFOF_Truncate are mutually exclusive.");
	MFDebug_Assert((pNative->openFlags & (MFOF_Text|MFOF_Binary)) != (MFOF_Text|MFOF_Binary), "MFOF_Text and MFOF_Binary are mutually exclusive.");

	const char *pFilename = pNative->pFilename;

	if(pOpenData->openFlags & MFOF_CreateDirectory)
		MFFileNative_CreateDirectory(MFStr_GetFilePath(pFilename));

	int flags = 0;

	if(pOpenData->openFlags & MFOF_Read)
	{
		if(pNative->openFlags & MFOF_Write)
			flags = O_RDWR | O_CREAT;
		else
			flags = O_RDONLY;
	}
	else if(pOpenData->openFlags & MFOF_Write)
	{
		flags = O_WRONLY | O_CREAT;
	}

	if(pOpenData->openFlags & MFOF_Write)
	{
		if(pNative->openFlags & MFOF_Append)
			flags |= O_APPEND;
		if(pNative->openFlags & MFOF_Truncate)
			flags |= O_TRUNC;
	}

	int file = open(pNative->pFilename, flags, S_IRWXU|S_IRWXG|S_IRWXO);
	if(file == -1)
	{
		pFile->pFilesysData = 0;
		return -1;
	}

	off_t size = 0;
	if(pNative->openFlags & MFOF_Append)
	{
		size = lseek(file, 0, SEEK_CUR);
	}
	else
	{
		struct stat fileStats;
		if(fstat(file, &fileStats) == -1)
		{
			MFDebug_Warn(2, MFStr("Call: MFFileNative_Open() - Error: fstat() failed, couldn't get size for file '%s'.", pNative->pFilename));
			close(file);
			pFile->pFilesysData = 0;
			return -1;
		}
		size = fileStats.st_size;
	}

	pFile->pFilesysData = (void*)(size_t)file;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = (pNative->openFlags & MFOF_Append) ? size : 0;
	pFile->length = size;

	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);

	return 0;
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

	close((size_t)fileHandle->pFilesysData);
	fileHandle->pFilesysData = 0;

	return 0;
}

size_t MFFileNative_Read(MFFile *fileHandle, void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	ssize_t bytesRead = 0;

	bytesRead = read((size_t)fileHandle->pFilesysData, pBuffer, bytes);
	if(bytesRead == -1) // read() returns -1 on error
		return 0;

	fileHandle->offset += bytesRead;

	return bytesRead;
}

size_t MFFileNative_Write(MFFile *fileHandle, const void *pBuffer, size_t bytes)
{
	MFCALLSTACK;

	ssize_t bytesWritten = 0;

	bytesWritten = write((size_t)fileHandle->pFilesysData, pBuffer, bytes);
	if(bytesWritten == -1) // write() returns -1 on error
		return 0;

	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, fileHandle->length);

	return bytesWritten;
}

uint64 MFFileNative_Seek(MFFile *fileHandle, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	MFDebug_Assert(fileHandle->pFilesysData, "MFFileNative_Seek() - Invalid file!");

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
			MFDebug_Assert(false, "MFFileNative_Seek() - Invalid 'relativity'.");
			break;
	}

	off_t newOffset = lseek((size_t)fileHandle->pFilesysData, bytes, whence);
	if(newOffset == -1)
		MFDebug_Warn(1, "MFFileNative_Seek() - lseek() failed!");
	else
		fileHandle->offset = newOffset;

	return fileHandle->offset;
}

uint64 MFFileNative_GetSize(const char *pFilename)
{
	MFCALLSTACK;

	struct stat fileStats;
	if(stat(pFilename, &fileStats) == -1)
		return 0;

	return fileStats.st_size;
}

bool MFFileNative_Exists(const char *pFilename)
{
	MFCALLSTACK;

	struct stat fileStats;

	if(stat(pFilename, &fileStats) < 0)
		return false;

	return true;
}

bool MFFileNative_Stat(const char *pPath, MFFileInfo *pFileInfo)
{
	MFDebug_Assert(false, "TODO");
	return false;
}

bool MFFileNative_CreateDirectory(const char *pPath)
{
	if(!pPath || *pPath == 0)
		return false;

	// strip trailing '/'
	((char*)pPath)[MFString_Length(pPath)-1] = 0;

	// the path is empty
	if(*pPath == 0)
		return false;

	MFFileNative_CreateDirectory(MFStr_GetFilePath(pPath));

	int r = mkdir(pPath, 0777);
	return r == 0;
}

bool MFFileNative_Delete(const char *pPath, bool bRecursive)
{
	MFDebug_Assert(false, "TODO");
	return false;
}

const char* MFFileNative_MakeAbsolute(const char *pFilename)
{
	char path[PATH_MAX];
	if(realpath(pFilename, path))
		return MFStr(path);
	return NULL;
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
		pPath = (char*)".";
	}

	// open the directory
	DIR *pDir = opendir(pPath);

	if(!pDir)
	{
		MFDebug_Warn(2, MFStr("Couldnt open directory '%s' with search pattern '%s'", pPath, pPattern));
		return false;
	}

	pFind->pFilesystemData = (void*)pDir;

	bool bFound = MFFileNative_FindNext(pFind, pFindData);
	if(!bFound)
		MFFileNative_FindClose(pFind);
	return bFound;
}

bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData)
{
	dirent *pDir = readdir((DIR*)pFind->pFilesystemData);

	while(pDir && (!MFString_Compare(pDir->d_name, ".") || !MFString_Compare(pDir->d_name, "..")))
		pDir = readdir((DIR*)pFind->pFilesystemData);
	if(!pDir)
		return false;

	MFString_CopyCat(pFindData->pSystemPath, (char*)pFind->pMount->pFilesysData, pFind->searchPattern);
	char *pLast = MFString_RChr(pFindData->pSystemPath, '/');
	if(pLast)
		pLast[1] = 0;
	else
		pFindData->pSystemPath[0] = 0;

	const char *pFilePath = MFStr("%s%s", pFindData->pSystemPath, pDir->d_name);

	struct stat statbuf;
	if(stat(pFilePath, &statbuf) < 0)
		return false;

	pFindData->attributes = (S_ISDIR(statbuf.st_mode) ? MFFA_Directory : 0) |
							(S_ISLNK(statbuf.st_mode) ? MFFA_SymLink : 0) |
							(pDir->d_name[0] == '.' ? MFFA_Hidden : 0);
							// TODO: Set MFFA_ReadOnly from write privileges
	pFindData->fileSize = statbuf.st_size;
	pFindData->writeTime.ticks = (uint64)statbuf.st_mtime;
	pFindData->accessTime.ticks = (uint64)statbuf.st_atime;
	MFString_Copy((char*)pFindData->pFilename, pDir->d_name);

	return true;
}

void MFFileNative_FindClose(MFFind *pFind)
{
	closedir((DIR*)pFind->pFilesystemData);
}

#endif // MF_DRIVER_LINUX
