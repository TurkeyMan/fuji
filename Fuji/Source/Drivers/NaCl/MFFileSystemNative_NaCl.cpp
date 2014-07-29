#include "Fuji_Internal.h"

#if MF_FILESYSTEM == MF_DRIVER_NACL

#include "MFSystem_NaCl.h"

#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"
#include "MFHeap.h"

static PPB_FileSystem *pFSInterface = NULL;

static PP_Resource gFilesystemExternal = 0;
static PP_Resource gFilesystemLocalPersistent = 0;
static PP_Resource gFilesystemLocalTemporary = 0;
static int gOpenComplete = 0;

#undef _USE_CRT_FOR_NULL_DRIVERS

static void CompletionCallback(void *user_data, int32_t result)
{
	if(result != PP_OK)
		MFDebug_Error("MFFileSystemNative_InitModulePlatformSpecific: Error mounting filesystem!");

	gOpenComplete |= 1 << (size_t)user_data;
}

MFInitStatus MFFileSystemNative_InitModulePlatformSpecific()
{
	if(gFilesystemExternal == 0)
	{
		FujiModule *pModule = (FujiModule*)pp::Module::Get();

		pFSInterface = (PPB_FileSystem*)pModule->GetBrowserInterface(PPB_FILESYSTEM_INTERFACE);

		Fuji *pInstance = pModule->GetInstance();
		if(!pInstance)
			return MFAIC_Failed;

		gFilesystemExternal = pFSInterface->Create(pInstance->pp_instance(), PP_FILESYSTEMTYPE_EXTERNAL);
		gFilesystemLocalPersistent = pFSInterface->Create(pInstance->pp_instance(), PP_FILESYSTEMTYPE_LOCALPERSISTENT);
		gFilesystemLocalTemporary = pFSInterface->Create(pInstance->pp_instance(), PP_FILESYSTEMTYPE_LOCALTEMPORARY);

		int32_t err = pFSInterface->Open(gFilesystemExternal, 0, PP_MakeOptionalCompletionCallback(CompletionCallback, (void*)0));
		if(err == PP_OK)
		{
			gOpenComplete |= 1;
		}
		else if(err != PP_OK_COMPLETIONPENDING)
		{
			MFDebug_Error("MFFileSystemNative_InitModulePlatformSpecific: Error opening FS interface!");
			return MFAIC_Failed;
		}

		err = pFSInterface->Open(gFilesystemLocalPersistent, 0, PP_MakeOptionalCompletionCallback(CompletionCallback, (void*)1));
		if(err == PP_OK)
		{
			gOpenComplete |= 2;
		}
		else if(err != PP_OK_COMPLETIONPENDING)
		{
			MFDebug_Error("MFFileSystemNative_InitModulePlatformSpecific: Error opening FS interface!");
			return MFAIC_Failed;
		}

		err = pFSInterface->Open(gFilesystemLocalTemporary, 0, PP_MakeOptionalCompletionCallback(CompletionCallback, (void*)2));
		if(err == PP_OK)
		{
			gOpenComplete |= 4;
		}
		else if(err != PP_OK_COMPLETIONPENDING)
		{
			MFDebug_Error("MFFileSystemNative_InitModulePlatformSpecific: Error opening FS interface!");
			return MFAIC_Failed;
		}
	}

	if(gOpenComplete == -1)
		return MFAIC_Failed;

	return gOpenComplete == 7 ? MFIS_Succeeded : MFAIC_Pending;
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

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	MFDebug_Assert(pOpenData->cbSize == sizeof(MFOpenDataNative), "Incorrect size for MFOpenDataNative structure. Invalid pOpenData.");
	MFOpenDataNative *pNative = (MFOpenDataNative*)pOpenData;

	int flags = 0;
	const char *pAccess = "";

	MFDebug_Assert(pOpenData->openFlags & (MFOF_Read|MFOF_Write), "Neither MFOF_Read nor MFOF_Write specified.");
	MFDebug_Assert((pNative->openFlags & (MFOF_Append|MFOF_Truncate)) != (MFOF_Append|MFOF_Truncate), "MFOF_Append and MFOF_Truncate are mutually exclusive.");
	MFDebug_Assert((pNative->openFlags & (MFOF_Text|MFOF_Binary)) != (MFOF_Text|MFOF_Binary), "MFOF_Text and MFOF_Binary are mutually exclusive.");

	if(pOpenData->openFlags & MFOF_Read)
	{
		if(pNative->openFlags & MFOF_Write)
		{
			pAccess = "w+b";
		}
		else
		{
			pAccess = "rb";
		}
	}
	else if(pOpenData->openFlags & MFOF_Write)
	{
		pAccess = "wb";
	}

	FILE *pF = fopen(pNative->pFilename, pAccess);
	if(pF == NULL)
	{
//		MFDebug_Warn(3, MFStr("Failed to open file '%s'.", pNative->pFilename));
		pFile->pFilesysData = 0;
		return -1;
	}

	pFile->pFilesysData = (void*)pF;
	pFile->createFlags = pOpenData->openFlags;
	pFile->offset = 0;

	fseek(pF, 0, SEEK_END);
	pFile->length = ftell(pF);
	fseek(pF, 0, SEEK_SET);

	MFString_Copy(pFile->fileIdentifier, pNative->pFilename);

	return 0;
#else
	return -1;
#endif
}

int MFFileNative_Close(MFFile* fileHandle)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	fclose((FILE*)fileHandle->pFilesysData);
	fileHandle->pFilesysData = 0;
#endif

	return 0;
}

int MFFileNative_Read(MFFile* fileHandle, void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	size_t bytesRead = 0;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	bytesRead = fread(pBuffer, 1, (size_t)bytes, (FILE*)fileHandle->pFilesysData);
	fileHandle->offset += bytesRead;
#endif

	return (int)bytesRead;
}

int MFFileNative_Write(MFFile* fileHandle, const void *pBuffer, int64 bytes)
{
	MFCALLSTACK;

	size_t bytesWritten = 0;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	bytesWritten = fwrite(pBuffer, 1, (size_t)bytes, (FILE*)fileHandle->pFilesysData);
	fileHandle->offset += bytesWritten;
	fileHandle->length = MFMax(fileHandle->offset, fileHandle->length);
#endif

	return (int)bytesWritten;
}

int MFFileNative_Seek(MFFile* fileHandle, int64 bytes, MFFileSeek relativity)
{
	MFCALLSTACK;

	if(!fileHandle->pFilesysData)
		return -1;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
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

	fseek((FILE*)fileHandle->pFilesysData, (long)bytes, whence);
	long newOffset = ftell((FILE*)fileHandle->pFilesysData);

	if(newOffset != -1)
	{
		fileHandle->offset = (uint32)newOffset;
	}
#endif

	return (int)fileHandle->offset;
}

uint32 MFFileNative_GetSize(const char* pFilename)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	FILE *pFile = fopen(pFilename, "rb");
	if(!pFile)
		return 0;

	fseek(pFile, 0, SEEK_END);
	long size = ftell(pFile);
	fclose(pFile);

	return (uint32)size;
#else
	return 0;
#endif
}

bool MFFileNative_Exists(const char* pFilename)
{
	MFCALLSTACK;

#if defined(_USE_CRT_FOR_NULL_DRIVERS)
	FILE *pFile = fopen(pFilename, "rb");

	if(!pFile)
		return false;

	fclose(pFile);
	return true;
#else
	return false;
#endif
}

bool MFFileNative_Stat(const char *pPath, MFFileInfo *pFileInfo)
{
	MFDebug_Assert(false, "TODO");
	return false;
}

bool MFFileNative_CreateDirectory(const char *pPath)
{
	MFDebug_Assert(false, "TODO");
	return false;
}

bool MFFileNative_Delete(const char *pPath, bool bRecursive)
{
	MFDebug_Assert(false, "TODO");
	return false;
}

const char* MFFileNative_MakeAbsolute(const char* pFilename)
{
	MFDebug_Assert(false, "!");
	return pPath;
}

bool MFFileNative_FindFirst(MFFind *pFind, const char *pSearchPattern, MFFindData *pFindData)
{
#if defined(_USE_CRT_FOR_NULL_DRIVERS) && defined(CRT_SUPPORTS_FIND)
	// open the directory
	_finddata_t fd;

	char *pPath = (char*)MFStr("%s%s", (char*)pFind->pMount->pFilesysData, pSearchPattern);
	intptr_t find = _findfirst(pPath, &fd);

	if(find == -1)
		return false;

	pFindData->attributes = ((fd.attrib & _A_SUBDIR) ? MFFA_Directory : 0) |
							((fd.attrib & _A_HIDDEN) ? MFFA_Hidden : 0) |
							((fd.attrib & _A_RDONLY) ? MFFA_ReadOnly : 0);
	pFindData->fileSize = fd.size;
	pFindData->writeTime.ticks = (uint64)fd.time_write;
	pFindData->accessTime.ticks = (uint64)fd.time_access;
	MFString_Copy((char*)pFindData->pFilename, fd.name);

	MFString_CopyCat(pFindData->pSystemPath, (char*)pFind->pMount->pFilesysData, pSearchPattern);
	char *pLast = MFString_RChr(pFindData->pSystemPath, '/');
	if(pLast)
		pLast[1] = 0;
	else
		pFindData->pSystemPath[0] = NULL;

	pFind->pFilesystemData = (void*)find;

	return true;
#else
	return false;
#endif
}

bool MFFileNative_FindNext(MFFind *pFind, MFFindData *pFindData)
{
#if defined(_USE_CRT_FOR_NULL_DRIVERS) && defined(CRT_SUPPORTS_FIND)
	_finddata_t fd;

	int more = _findnext((intptr_t)pFind->pFilesystemData, &fd);

	if(more == -1)
		return false;

	MFDebug_Assert(more == 0, "Something's not quite right here.. You have a bad CRT.");

	pFindData->attributes = ((fd.attrib & _A_SUBDIR) ? MFFA_Directory : 0) |
							((fd.attrib & _A_HIDDEN) ? MFFA_Hidden : 0) |
							((fd.attrib & _A_RDONLY) ? MFFA_ReadOnly : 0);
	pFindData->fileSize = fd.size;
	pFindData->writeTime.ticks = (uint64)fd.time_write;
	pFindData->accessTime.ticks = (uint64)fd.time_access;
	MFString_Copy((char*)pFindData->pFilename, fd.name);

	return true;
#else
	return false;
#endif
}

void MFFileNative_FindClose(MFFind *pFind)
{
#if defined(_USE_CRT_FOR_NULL_DRIVERS) && defined(CRT_SUPPORTS_FIND)
	_findclose((intptr_t)pFind->pFilesystemData);
#endif
}

#endif
