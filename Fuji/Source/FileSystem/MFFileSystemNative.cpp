#include "Fuji_Internal.h"
#include "MFHeap_Internal.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative_Internal.h"

int gFileSystemNativeId = -1;

MFInitStatus MFFileSystemNative_InitModule(int moduleId, bool bPerformInitialisation)
{
	gFileSystemNativeId = moduleId;

	if(!bPerformInitialisation)
		return MFIS_Succeeded;

	ALLOC_MODULE_DATA(MFFileSystemGlobalState);

	MFInitStatus status = MFFileSystemNative_InitModulePlatformSpecific();

	if(status == MFIS_Succeeded)
	{
		MFFileSystemCallbacks fsCallbacks;

		fsCallbacks.RegisterFS = MFFileSystemNative_Register;
		fsCallbacks.UnregisterFS = MFFileSystemNative_Unregister;
		fsCallbacks.FSMount = MFFileSystemNative_Mount;
		fsCallbacks.FSDismount = MFFileSystemNative_Dismount;
		fsCallbacks.FSOpen = MFFileSystemNative_Open;
		fsCallbacks.Open = MFFileNative_Open;
		fsCallbacks.Close = MFFileNative_Close;
		fsCallbacks.Read = MFFileNative_Read;
		fsCallbacks.Write = MFFileNative_Write;
		fsCallbacks.Seek = MFFileNative_Seek;
		fsCallbacks.FindFirst = MFFileNative_FindFirst;
		fsCallbacks.FindNext = MFFileNative_FindNext;
		fsCallbacks.FindClose = MFFileNative_FindClose;
		fsCallbacks.Stat = MFFileNative_Stat;
		fsCallbacks.CreateDir = MFFileNative_CreateDirectory;
		fsCallbacks.Delete = MFFileNative_Delete;

		pModuleData->hFileSystemHandle = MFFileSystem_RegisterFileSystem("Native Filesystem", &fsCallbacks);
	}

	return status;
}

void MFFileSystemNative_DeinitModule()
{
	GET_MODULE_DATA_ID(MFFileSystemGlobalState, gFileSystemNativeId);

	MFFileSystem_UnregisterFileSystem(pModuleData->hFileSystemHandle);
}

int MFFileSystemNative_Mount(MFMount *pMount, MFMountData *pMountData)
{
	MFDebug_Assert(pMountData->cbSize == sizeof(MFMountDataNative), "Incorrect size for MFMountDataNative structure. Invalid pMountData.");

	MFMountDataNative *pMountNative = (MFMountDataNative*)pMountData;

	size_t pathLen = MFString_Length(pMountNative->pPath);

	// we need to append a slash if its not already there...
	int addSlash = 0;
	if(pMountNative->pPath[pathLen-1] != '/')
		addSlash = 1;

	pMount->pFilesysData = (char*)MFHeap_Alloc(pathLen + addSlash + 1);
	MFString_CopyCat((char*)pMount->pFilesysData, pMountNative->pPath, addSlash ? "/" : "");

	return 0;
}

int MFFileSystemNative_Dismount(MFMount *pMount)
{
	if (pMount->pEntries)
		MFFileSystem_ReleaseToc(pMount->pEntries, pMount->numFiles);
	MFHeap_Free(pMount->pFilesysData);
	return 0;
}

MFFile* MFFileSystemNative_Open(MFMount *pMount, const char *pFilename, uint32 openFlags)
{
	GET_MODULE_DATA_ID(MFFileSystemGlobalState, gFileSystemNativeId);

	MFFile *hFile = NULL;

	// recurse toc
	if(pMount->pEntries)
	{
		MFTOCEntry *pTOCEntry = MFFileSystem_GetTocEntry(pFilename, pMount->pEntries, pMount->numFiles);

		if(pTOCEntry)
		{
			MFOpenDataNative openData;

			openData.cbSize = sizeof(MFOpenDataNative);
			openData.openFlags = openFlags;
			openData.pFilename = MFStr("%s%s", (char*)pTOCEntry->pFilesysData, pTOCEntry->pName);

			hFile = MFFile_Open(pModuleData->hFileSystemHandle, &openData);
		}
	}
	else
	{
		MFOpenDataNative openData;

		openData.cbSize = sizeof(MFOpenDataNative);
		openData.openFlags = openFlags;
		openData.pFilename = MFStr("%s%s", (char*)pMount->pFilesysData, pFilename);

		hFile = MFFile_Open(pModuleData->hFileSystemHandle, &openData);
	}

	return hFile;
}
