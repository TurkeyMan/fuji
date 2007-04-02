#include "Fuji.h"
#include "MFHeap.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemZipFile_Internal.h"

#include <zlib/zlib.h>
#include <zlib/minizip/zip.h>
#include <zlib/minizip/unzip.h>
#include <zlib/minizip/ioapi.h>

void MFFileSystemZipFile_InitModule()
{
	MFCALLSTACK;

	MFFileSystemCallbacks fsCallbacks;

	fsCallbacks.RegisterFS = NULL;
	fsCallbacks.UnregisterFS = NULL;
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
	fsCallbacks.FindFirst = MFFileZipFile_FindFirst;
	fsCallbacks.FindNext = MFFileZipFile_FindNext;
	fsCallbacks.FindClose = MFFileZipFile_FindClose;

	hZipFileSystem = MFFileSystem_RegisterFileSystem(&fsCallbacks);
}

void MFFileSystemZipFile_DeinitModule()
{
	MFCALLSTACK;

	MFFileSystem_UnregisterFileSystem(hZipFileSystem);
}

// filesystem callbacks
voidpf zopen_file_func(voidpf opaque, const char* filename, int mode)
{
	return opaque; 
}

uLong zread_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	return MFFile_Read((MFFile*)stream, buf, size, false);
}

uLong zwrite_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	return MFFile_Write((MFFile*)stream, buf, size, false);
}

long ztell_file_func(voidpf opaque, voidpf stream)
{
	return MFFile_Tell((MFFile*)stream);
}

long zseek_file_func(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	return MFFile_StdSeek((MFFile*)stream, offset, (MFFileSeek)origin);
}

int zclose_file_func(voidpf opaque, voidpf stream)
{
	return 0; 
}

int ztesterror_file_func(voidpf opaque, voidpf stream)
{
	return 0;
}

int MFFileSystemZipFile_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFCALLSTACK;

	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataZipFile), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataZipFile *pMountZipFile = (MFMountDataZipFile*)pMountData;

	bool flatten = (pMount->volumeInfo.flags & MFMF_FlattenDirectoryStructure) != 0;
	bool recursive = (pMount->volumeInfo.flags & MFMF_Recursive) != 0;

	MFDebug_Assert(flatten, ".zip file currently only supports a flattened directory structure.");

	if(!recursive)
		MFDebug_Warn(3, "Mounting Zip filesystems without the 'Recursive' flag is invalid. Zip file will be mounted recursive anyway.");

	// attempt to open zipfile..
	zlib_filefunc_def zipIOFunctions;

	zipIOFunctions.zopen_file = zopen_file_func;
    zipIOFunctions.zread_file = zread_file_func;
    zipIOFunctions.zwrite_file = zwrite_file_func;
    zipIOFunctions.ztell_file = ztell_file_func;
    zipIOFunctions.zseek_file = zseek_file_func;
    zipIOFunctions.zclose_file = zclose_file_func;
    zipIOFunctions.zerror_file = ztesterror_file_func;
    zipIOFunctions.opaque = pMountZipFile->pZipArchive;

	unzFile zipFile = unzOpen2(NULL, &zipIOFunctions);

	if(!zipFile)
	{
		MFDebug_Warn(1, "FileSystem: Supplied file handle is not a valid .zip file.");
		return -1;
	}

	pMount->pFilesysData = zipFile;

	// make sure the toc is being cached...
	if(pMount->volumeInfo.flags & MFMF_DontCacheTOC)
	{
		MFDebug_Warn(2, "Zip files MUST cache the toc");
		pMount->volumeInfo.flags &= ~MFMF_DontCacheTOC;
	}

	return 0;
}

int MFFileSystemZipFile_Dismount(MFMount *pMount)
{
	MFCALLSTACK;

	MFFileSystem_ReleaseToc(pMount->pEntries, pMount->numFiles);

	unzClose((unzFile)pMount->pFilesysData);

	return 0;
}

MFFile* MFFileSystemZipFile_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	MFCALLSTACK;

	MFFile *hFile = NULL;

	// recurse toc
	MFTOCEntry *pTOCEntry = MFFileSystem_GetTocEntry(pFilename, pMount->pEntries, pMount->numFiles);

	if(pTOCEntry)
	{
		MFOpenDataZipFile openData;

		openData.cbSize = sizeof(MFOpenDataZipFile);
		openData.openFlags = openFlags | MFOF_Zip_AlreadyMounted;
		openData.pZipArchive = (MFFile*)pMount->pFilesysData;
		openData.pFilename = MFStr("%s%s", pTOCEntry->pFilesysData ? (char*)pTOCEntry->pFilesysData : "", pTOCEntry->pName);

		hFile = MFFile_Open(hZipFileSystem, &openData);
	}

	return hFile;
}

int MFFileZipFile_Open(MFFile *pFile, MFOpenData *pOpenData)
{
	MFCALLSTACK;

	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataZipFile), "Incorrect size for MFOpenDataZipFile structure. Invalid pOpenData.");
	MFOpenDataZipFile *pZipFile = (MFOpenDataZipFile*)pOpenData;

	pFile->state = MFFS_Ready;
	pFile->operation = MFFO_None;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	bool alreadyMounted = (pOpenData->openFlags&MFOF_Zip_AlreadyMounted) != 0;

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
		zipIOFunctions.opaque = pZipFile->pZipArchive;

		zipFile = unzOpen2(NULL, &zipIOFunctions);
	}
	else
	{
		zipFile = (unzFile)pZipFile->pZipArchive;
	}

	if(!zipFile)
	{
		MFDebug_Warn(1, "FileSystem: Supplied file handle is not a valid .zip file.");
		return -1;
	}


	pFile->pFilesysData = zipFile;

	int result = unzLocateFile(zipFile, pZipFile->pFilename, 2);

	if(result != UNZ_OK)
	{
		MFDebug_Assert(false, MFStr("Couldnt locate file '%s' in .zip file", pZipFile->pFilename));
		return -1;
	}

	unzOpenCurrentFile(zipFile);

	// get length of deflated file..
	unz_file_info fileInfo;
	unzGetCurrentFileInfo(zipFile, &fileInfo, NULL, 0, NULL, 0, NULL, 0);

	pFile->length = fileInfo.uncompressed_size;

#if defined(_DEBUG)
	MFString_Copy(pFile->fileIdentifier, pZipFile->pFilename);
#endif

	return 0;
}

int MFFileZipFile_Close(MFFile* pFile)
{
	MFCALLSTACK;

	unzCloseCurrentFile((unzFile)pFile->pFilesysData);

	bool alreadyMounted = (pFile->createFlags&MFOF_Zip_AlreadyMounted) != 0;

	if(!alreadyMounted)
	{
		unzClose((unzFile)pFile->pFilesysData);
	}

	return 0;//unzClose((unzFile)pFile->pFilesysData);
}

int MFFileZipFile_Read(MFFile* pFile, void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	uint32 bytesRead;
	bytesRead = unzReadCurrentFile((unzFile)pFile->pFilesysData, pBuffer, bytes);
	pFile->offset += bytesRead;

	return bytesRead;
}

int MFFileZipFile_Write(MFFile* pFile, const void *pBuffer, uint32 bytes, bool async)
{
	MFCALLSTACK;

	MFDebug_Assert(async == false, "Asynchronous Filesystem not yet supported...");

	// write

	return 0;
}

int MFFileZipFile_Seek(MFFile* pFile, int bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	int newPos = 0;

	switch(relativity)
	{
		case MFSeek_Begin:
			newPos = MFMin(bytes, pFile->length);
			break;
		case MFSeek_End:
			newPos = MFMax(0, pFile->length - bytes);
			break;
		case MFSeek_Current:
			newPos = MFClamp(0, (int)pFile->offset + bytes, pFile->length);
			break;
		default:
			MFDebug_Assert(false, "Invalid 'relativity' for file seeking.");
	}

	unzFile f = (unzFile)pFile->pFilesysData;

	unzCloseCurrentFile(f);
	unzOpenCurrentFile(f);

	pFile->offset = (uint32)newPos;

	// read to the desired position.
	char buffer[256];

	while(newPos)
	{
		int bytes = newPos < 256 ? newPos : 256;
		unzReadCurrentFile(f, buffer, bytes);
		newPos -= bytes;
	}

	return (int)pFile->offset;
}

int MFFileZipFile_Tell(MFFile* pFile)
{
	MFCALLSTACK;
	return (int)pFile->offset;
}

MFFileState MFFileZipFile_Query(MFFile* pFile)
{
	MFCALLSTACK;
	return pFile->state;
}

int MFFileZipFile_GetSize(MFFile* pFile)
{
	MFCALLSTACK;
	return pFile->length;
}

bool MFFileZipFile_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
	unzFile zipFile = (unzFile)pFind->pMount->pFilesysData;

	int err = unzGoToFirstFile(zipFile);

	if(err != UNZ_OK)
		return false;

	unz_file_info fileInfo;
	unzGetCurrentFileInfo(zipFile, &fileInfo, pFindData->pFilename, sizeof(pFindData->pFilename), NULL, 0, NULL, 0);
	pFindData->attributes = 0;
	pFindData->fileSize = fileInfo.uncompressed_size;
	pFindData->pSystemPath[0] = 0;

	return true;
}

bool MFFileZipFile_FindNext(MFFind *pFind, MFFindData *pFindData)
{
	unzFile zipFile = (unzFile)pFind->pMount->pFilesysData;

	int err = unzGoToNextFile(zipFile);

	if(err != UNZ_OK)
		return false;

	unz_file_info fileInfo;
	unzGetCurrentFileInfo(zipFile, &fileInfo, pFindData->pFilename, sizeof(pFindData->pFilename), NULL, 0, NULL, 0);
	pFindData->attributes = 0;
	pFindData->fileSize = fileInfo.uncompressed_size;

	return true;
}

void MFFileZipFile_FindClose(MFFind *pFind)
{
}
