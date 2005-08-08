#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemZipFile.h"

#include <zlib.h>
#include <zip.h>
#include <unzip.h>
#include <ioapi.h>

void MFFileSystemZipFile_InitModule()
{
	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = MFFileSystemZipFile_Register;
	fsCallbacks.UnregisterFS = MFFileSystemZipFile_Unregister;
	fsCallbacks.FSMount = MFFileSystemZipFile_Mount;
	fsCallbacks.FSDismount = MFFileSystemZipFile_Dismount;
	fsCallbacks.FSOpen = MFFileSystemZipFile_Open;
	fsCallbacks.Open = MFFileZipFile_Open;
	fsCallbacks.Close = MFFileZipFile_Close;
	fsCallbacks.Read = MFFileZipFile_Read;
	fsCallbacks.Write = MFFileZipFile_Write;
	fsCallbacks.Seek = MFFileZipFile_Seek;
	fsCallbacks.Tell = MFFileZipFile_Tell;
	fsCallbacks.Query = MFFileZipFile_Query;
	fsCallbacks.GetSize = MFFileZipFile_GetSize;

	hZipFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemZipFile_DeinitModule()
{
	MFFileSystem_UnregisterFileSystem(hZipFileSystem);
}

// filesystem callbacks
void MFFileSystemZipFile_Register()
{

}

void MFFileSystemZipFile_Unregister()
{

}

// zlib callbacks
voidpf zopen_file_func(voidpf opaque, const char* filename, int mode)
{
	return opaque; 
}

uLong zread_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	return MFFile_Read((MFFileHandle)stream, buf, size, false);
}

uLong zwrite_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	return MFFile_Write((MFFileHandle)stream, buf, size, false);
}

long ztell_file_func(voidpf opaque, voidpf stream)
{
	return MFFile_Tell((MFFileHandle)stream);
}

long zseek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	return MFFile_StdSeek((MFFileHandle)stream, offset, (MFFileSeek)origin);
}

int zclose_file_func(voidpf opaque, voidpf stream)
{
	return 0; 
}

int ztesterror_file_func(voidpf opaque, voidpf stream)
{
	return 0;
}


int MFFileSystemZipFile_GetNumEntries(unzFile zipFile, bool recursive, bool flatten, int *pStringLengths)
{
	// count files and calculate string cache length
	int numFiles = 0;

	int zipFileIndex = unzGoToFirstFile(zipFile);

	// search through files in zip archive
	while(zipFileIndex != UNZ_END_OF_LIST_OF_FILE)
	{
		DBGASSERT(zipFileIndex == UNZ_OK, "Error in .zip file.");

		char fileName[256];

		unz_file_info fileInfo;
		unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, 256, NULL, 0, NULL, 0);

		int nameLen = strlen(fileName);

		bool isDirectory = fileName[nameLen-1] == '/';

		if(isDirectory)
		{
			// register directory for individual files to reference
		}
		else
		{
			*pStringLengths += nameLen + 1;
			++numFiles;
		}

		zipFileIndex = unzGoToNextFile(zipFile);
	}

	return numFiles;
}

MFTOCEntry* MFFileSystemZipFile_BuildToc(unzFile zipFile, MFTOCEntry *pToc, MFTOCEntry *pParent, char* &pStringCache, bool recursive, bool flatten)
{
	int zipFileIndex = unzGoToFirstFile(zipFile);

	// search through files in zip archive
	while(zipFileIndex != UNZ_END_OF_LIST_OF_FILE)
	{
		DBGASSERT(zipFileIndex == UNZ_OK, "Error in .zip file.");

		char fileName[256];

		unz_file_info fileInfo;
		unzGetCurrentFileInfo(zipFile, &fileInfo, fileName, 256, NULL, 0, NULL, 0);

		int fileLen = strlen(fileName);
		bool isDirectory = fileName[fileLen-1] == '/';

		if(isDirectory)
		{
			// register directory for individual files to reference
		}
		else
		{
			strcpy(pStringCache, fileName);
			pToc->pFilesysData = pStringCache;

			for(int a=fileLen; --a;)
			{
				if(pStringCache[a] == '/')
					break;
			}

			if(a == 0)
			{
				pToc->pName = pStringCache;
				pToc->pFilesysData = NULL;
			}
			else
			{
				pStringCache[a] = 0;
				pToc->pName = &pStringCache[a+1];
			}

			pStringCache += fileLen+1;

			pToc->pParent = NULL;
			pToc->pChild = NULL;

			pToc->flags = 0;
			pToc->size = fileInfo.uncompressed_size;

			++pToc;
		}

		zipFileIndex = unzGoToNextFile(zipFile);
	}

	return pToc;
}

int MFFileSystemZipFile_Mount(MFMount *pMount, MFMountData *pMountData)
{
	DBGASSERT(pMountData->cbSize == sizeof(MFMountDataZipFile), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataZipFile *pMountZipFile = (MFMountDataZipFile*)pMountData;

	bool flatten = (pMountData->flags & MFMF_FlattenDirectoryStructure) != 0;
	bool recursive = (pMountData->flags & MFMF_Recursive) != 0;

	DBGASSERT(flatten, ".zip file currently only supports a flattened directory structure.");

	if(!recursive)
	{
		LOGD("Warning: Zip filesystem mounted WITHOUT 'Recursive' flag. Zip file will be mounted recursive anyway.");
	}

	// attempt to open zipfile..
	zlib_filefunc_def zipIOFunctions;

	zipIOFunctions.zopen_file = zopen_file_func;
    zipIOFunctions.zread_file = zread_file_func;
    zipIOFunctions.zwrite_file = zwrite_file_func;
    zipIOFunctions.ztell_file = ztell_file_func;
    zipIOFunctions.zseek_file = zseek_file_func;
    zipIOFunctions.zclose_file = zclose_file_func;
    zipIOFunctions.zerror_file = ztesterror_file_func;
    zipIOFunctions.opaque = pMountZipFile->zipArchiveHandle;

	unzFile zipFile = unzOpen2(NULL, &zipIOFunctions);
	pMount->pFilesysData = zipFile;

	if(!zipFile)
	{
		LOGD("FileSystem: Supplied file handle is not a valid .zip file.");
		return -1;
	}

	int stringCacheSize = 0;
	pMount->numFiles = MFFileSystemZipFile_GetNumEntries(zipFile, recursive, flatten, &stringCacheSize);

	int sizeOfToc = sizeof(MFTOCEntry)*pMount->numFiles;
	pMount->pEntries = (MFTOCEntry*)Heap_Alloc(sizeOfToc + stringCacheSize);

	char *pStringCache = ((char*)pMount->pEntries)+sizeOfToc;
	MFFileSystemZipFile_BuildToc(zipFile, pMount->pEntries, NULL, pStringCache, recursive, flatten);

	return 0;
}

int MFFileSystemZipFile_Dismount(MFMount *pMount)
{
	MFFileSystem_ReleaseToc(pMount->pEntries, pMount->numFiles);

	unzClose((unzFile)pMount->pFilesysData);

	return 0;
}

MFFile* MFFileSystemZipFile_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	MFFileHandle hFile = NULL;

	// recurse toc
	MFTOCEntry *pTOCEntry = MFFileSystem_GetTocEntry(pFilename, pMount->pEntries, pMount->numFiles);

	if(pTOCEntry)
	{
		MFOpenDataZipFile openData;

		openData.cbSize = sizeof(MFOpenDataZipFile);
		openData.openFlags = openFlags | OFZip_AlreadyMounted;
		openData.zipArchiveHandle = (MFFileHandle)pMount->pFilesysData;
		openData.pFilename = STR("%s%s", pTOCEntry->pFilesysData ? STR("%s/", (char*)pTOCEntry->pFilesysData) : "", pTOCEntry->pName);

		hFile = MFFile_Open(hZipFileSystem, &openData);
	}

	return hFile;
}

int MFFileZipFile_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	CALLSTACK;

	DBGASSERT(pOpenData->cbSize == sizeof(MFOpenDataZipFile), "Incorrect size for MFOpenDataZipFile structure. Invalid pOpenData.");
	MFOpenDataZipFile *pZipFile = (MFOpenDataZipFile*)pOpenData;

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	bool alreadyMounted = (pOpenData->openFlags&OFZip_AlreadyMounted) != 0;

	unzFile zipFile;

	if(!alreadyMounted)
	{
		zlib_filefunc_def zipIOFunctions;

		zipIOFunctions.zopen_file = zopen_file_func;
		zipIOFunctions.zread_file = zread_file_func;
		zipIOFunctions.zwrite_file = zwrite_file_func;
		zipIOFunctions.ztell_file = ztell_file_func;
		zipIOFunctions.zseek_file = zseek_file_func;
		zipIOFunctions.zclose_file = zclose_file_func;
		zipIOFunctions.zerror_file = ztesterror_file_func;
		zipIOFunctions.opaque = pZipFile->zipArchiveHandle;

		zipFile = unzOpen2(NULL, &zipIOFunctions);

	}
	else
	{
		zipFile = (unzFile)pZipFile->zipArchiveHandle;
	}

	if(!zipFile)
	{
		LOGD("FileSystem: Supplied file handle is not a valid .zip file.");
		return -1;
	}

	pFile->pFilesysData = zipFile;

	int result = unzLocateFile(zipFile, pZipFile->pFilename, 2);

	if(result != UNZ_OK)
	{
		DBGASSERT(false, STR("Couldnt locate file '%s' in .zip file", pZipFile->pFilename));
		return -1;
	}

	unzOpenCurrentFile(zipFile);

	// get length of deflated file..
	unz_file_info fileInfo;
	unzGetCurrentFileInfo(zipFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0);

	pFile->length = fileInfo.uncompressed_size;

#if defined(_DEBUG)
	strcpy(pFile->fileIdentifier, pZipFile->pFilename);
#endif

	return 0;
}

int MFFileZipFile_Close(MFFile* pFile)
{
	CALLSTACK;

	unzCloseCurrentFile((unzFile)pFile->pFilesysData);

	bool alreadyMounted = (pFile->createFlags&OFZip_AlreadyMounted) != 0;

	if(!alreadyMounted)
	{
		unzClose((unzFile)pFile->pFilesysData);
	}

	return unzClose((unzFile)pFile->pFilesysData);
}

int MFFileZipFile_Read(MFFile* pFile, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	DBGASSERT(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	bytesRead = unzReadCurrentFile((unzFile)pFile->pFilesysData, pBuffer, bytes);
	pFile->offset += bytesRead;

	return bytesRead;
}

int MFFileZipFile_Write(MFFile* pFile, const void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	DBGASSERT(async == false, "Asynchronous Filesystem not yet supported...");

	// write

	return 0;
}

int MFFileZipFile_Seek(MFFile* pFile, int bytes, MFFileSeek relativity)
{
	CALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = Min(bytes, pFile->length);
			break;
		case MFSeek_End:
			newPos = Max(0, pFile->length - bytes);
			break;
		case MFSeek_Current:
			newPos = Clamp(0, (int)pFile->offset + bytes, pFile->length);
			break;
		default:
			DBGASSERT(false, "Invalid 'relativity' for file seeking.");
	}

	unzSetOffset((unzFile)pFile->pFilesysData, (uint32)newPos);
	pFile->offset = (uint32)newPos;
	return newPos;
}

int MFFileZipFile_Tell(MFFile* pFile)
{
	CALLSTACK;
	return (int)pFile->offset;
}

MFFileState MFFileZipFile_Query(MFFile* pFile)
{
	CALLSTACK;
	return pFile->state;
}

int MFFileZipFile_GetSize(MFFile* pFile)
{
	CALLSTACK;
	return pFile->length;
}
