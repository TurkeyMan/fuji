#include "Fuji.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"
#include "FileSystem/MFFileSystemMemory.h"
#include "FileSystem/MFFileSystemZipFile.h"
#include "MFPtrList.h"

MFPtrListDL<MFFile> gOpenFiles;

MFMount *pMountList = NULL;
MFMount *pMountListEnd = NULL;

MFPtrListDL<MFFileSystemCallbacks> pFileSystemCallbacks;
MFFileSystemCallbacks **ppFileSystemList;

// internal filesystems
MFFileSystemHandle hNativeFileSystem = -1;
MFFileSystemHandle hMemoryFileSystem = -1;
MFFileSystemHandle hZipFileSystem = -1;

MFFileHandle hDataArchive = NULL;

void MFFileSystem_InitModule()
{
	gOpenFiles.Init("Open Files", gDefaults.filesys.maxOpenFiles);

	pFileSystemCallbacks.Init("File System Callbacls", gDefaults.filesys.maxFileSystems);
	ppFileSystemList = (MFFileSystemCallbacks**)MFHeap_Alloc(sizeof(MFFileSystemCallbacks*) * gDefaults.filesys.maxFileSystems);
	memset(ppFileSystemList, 0, sizeof(MFFileSystemCallbacks*) * gDefaults.filesys.maxFileSystems);

	// mount filesystems
	MFFileSystemNative_InitModule();
	MFFileSystemMemory_InitModule();
	MFFileSystemZipFile_InitModule();

	MFOpenDataNative dataArchive;
	dataArchive.cbSize = sizeof(MFOpenDataNative);
	dataArchive.openFlags = MFOF_Read|MFOF_Binary;
	dataArchive.pFilename =  MFFile_SystemPath(MFStr("Data_%s.zip", MFSystem_GetPlatformName(MFSystem_GetCurrentPlatform())));
	hDataArchive = MFFile_Open(hNativeFileSystem, &dataArchive);

	MFMountDataNative mountData;
	mountData.cbSize = sizeof(MFMountDataNative);
	mountData.priority = MFMP_Normal;

	if(hDataArchive)
	{
		MFMountDataZipFile zipMountData;
		zipMountData.cbSize = sizeof(MFMountDataZipFile);
		zipMountData.flags = MFMF_Recursive|MFMF_FlattenDirectoryStructure;
		zipMountData.priority = MFMP_Normal;
		zipMountData.pMountpoint = "data";
		zipMountData.zipArchiveHandle = hDataArchive;
		MFFileSystem_Mount(hZipFileSystem, &zipMountData);
	}
	else
	{
		mountData.flags = MFMF_Recursive|MFMF_FlattenDirectoryStructure;
		mountData.pMountpoint = "data";
		mountData.pPath = MFStr("%sData_%s/", MFFile_SystemPath(), MFSystem_GetPlatformName(MFSystem_GetCurrentPlatform()));
		MFFileSystem_Mount(hNativeFileSystem, &mountData);
	}

	mountData.flags = MFMF_Recursive;
	mountData.pMountpoint = "home";
	mountData.pPath = MFFile_HomePath();
	MFFileSystem_Mount(hNativeFileSystem, &mountData);
}

void MFFileSystem_DeinitModule()
{
	if(hDataArchive)
	{
		MFFile_Close(hDataArchive);
	}

	// dismount filesystems
	MFFileSystemZipFile_DeinitModule();
	MFFileSystemMemory_DeinitModule();
	MFFileSystemNative_DeinitModule();

	MFHeap_Free(ppFileSystemList);

	pFileSystemCallbacks.Deinit();
	gOpenFiles.Deinit();
}

MFFileSystemHandle MFFileSystem_RegisterFileSystem(MFFileSystemCallbacks *pCallbacks)
{
	for(uint32 a=0; a<gDefaults.filesys.maxFileSystems; a++)
	{
		if(ppFileSystemList[a] == NULL)
		{
			MFDebug_Assert(pCallbacks->RegisterFS, "No RegisterFS function supplied.");
			MFDebug_Assert(pCallbacks->UnregisterFS, "No UnregisterFS function supplied.");
			MFDebug_Assert(pCallbacks->FSMount, "No FSMount function supplied.");
			MFDebug_Assert(pCallbacks->FSDismount, "No FSDismount function supplied.");
			MFDebug_Assert(pCallbacks->FSOpen, "No FSOpen function supplied.");
			MFDebug_Assert(pCallbacks->Open, "No Open function supplied.");
			MFDebug_Assert(pCallbacks->Close, "No Close function supplied.");
			MFDebug_Assert(pCallbacks->Read, "No Read function supplied.");
			MFDebug_Assert(pCallbacks->Write, "No Write function supplied.");
			MFDebug_Assert(pCallbacks->Seek, "No Seek function supplied.");
			MFDebug_Assert(pCallbacks->Tell, "No Tell function supplied.");
			MFDebug_Assert(pCallbacks->Query, "No Query function supplied.");
			MFDebug_Assert(pCallbacks->GetSize, "No GetSize function supplied.");

			ppFileSystemList[a] = pFileSystemCallbacks.Create();
			memcpy(ppFileSystemList[a], pCallbacks, sizeof(MFFileSystemCallbacks));

			ppFileSystemList[a]->RegisterFS();

			return a;
		}
	}

	MFDebug_Assert(false, MFStr("Exceeded maximum of %d Filesystems. Modify 'gDefaults.filesys.maxFileSystems'.", gDefaults.filesys.maxFileSystems));

	return -1;
}

void MFFileSystem_UnregisterFileSystem(MFFileSystemHandle filesystemHandle)
{
	MFDebug_Assert(ppFileSystemList[filesystemHandle], "Filesystem not mounted");

	ppFileSystemList[filesystemHandle]->UnregisterFS();
	pFileSystemCallbacks.Destroy(ppFileSystemList[filesystemHandle]);
	ppFileSystemList[filesystemHandle] = NULL;
}

// interface functions
char* MFFile_SystemPath(const char *filename)
{
	CALLSTACK;

	filename ? filename : "";

#if defined(_XBOX)
	return MFStr("D:\\Data\\%s", filename);
#elif defined(_WINDOWS)
	return MFStr("Data/%s", filename);
#elif defined(_LINUX)
	return MFStr("Data/%s", filename);
#elif defined(_PSP)
	return MFStr("ms0:/PSP/GAME/FUJI/Data/%s", filename);
#else
	return MFStr("%s", filename);
#endif
}

char* MFFile_HomePath(const char *filename)
{
	CALLSTACK;

	filename ? filename : "";

#if defined(_XBOX)
	return MFStr("E:\\Home\\%s", filename);
#elif defined(_WINDOWS)
	return MFStr("Home/%s", filename);
#elif defined(_PSP)
	return MFStr("ms0:/PSP/GAME/FUJI/Home/%s", filename);
#else
	return MFStr("%s", filename);
#endif
}

///////////////////////////
// file access functions
MFFile* MFFile_Open(MFFileSystemHandle fileSystem, MFOpenData *pOpenData)
{
	CALLSTACK;

	MFFile *pFile = gOpenFiles.Create();

	memset(pFile, 0, sizeof(MFFile));
	pFile->filesystem = fileSystem;

	int result = ppFileSystemList[fileSystem]->Open(pFile, pOpenData);

	if(result < 0)
	{
		gOpenFiles.Destroy(pFile);
		return NULL;
	}

	return pFile;
}

int MFFile_Close(MFFile* fileHandle)
{
	CALLSTACK;

	int fileID = fileHandle->filesystem;

	ppFileSystemList[fileID]->Close(fileHandle);
	gOpenFiles.Destroy(fileHandle);

	return 0;
}

// read/write functions
int MFFile_Read(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->Read(fileHandle, pBuffer, bytes, async);
}

int MFFile_Write(MFFile* fileHandle, const void *pBuffer, uint32 bytes, bool async)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->Write(fileHandle, pBuffer, bytes, async);
}

// offset management (these are stdio function signature compliant)
int MFFile_Seek(MFFile* fileHandle, int bytes, MFFileSeek relativity)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->Seek(fileHandle, bytes, relativity);
}

int MFFile_Tell(MFFile* fileHandle)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->Tell(fileHandle);
}

// return the state of a file
MFFileState MFFile_Query(MFFile* fileHandle)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->Query(fileHandle);
}

// get file stream length (retuurs -1 for an undefined stream length)
int MFFile_GetSize(MFFile* fileHandle)
{
	CALLSTACK;

	return ppFileSystemList[fileHandle->filesystem]->GetSize(fileHandle);
}

// stdio signiture functions (these can be used as a callback to many libs and API's)
uint32 MFFile_StdRead(void *pBuffer, uint32 size, uint32 count, void* fileHandle)
{
	return (uint32)MFFile_Read((MFFileHandle)fileHandle, pBuffer, size*count, false);
}

uint32 MFFile_StdWrite(void *pBuffer, uint32 size, uint32 count, void* fileHandle)
{
	return (uint32)MFFile_Write((MFFileHandle)fileHandle, pBuffer, size*count, false);
}
int MFFile_StdClose(void* fileHandle)
{
	return MFFile_Close((MFFileHandle)fileHandle);
}

int MFFile_StdSeek(void* fileHandle, long bytes, int relativity)
{
	int seek = MFFile_Seek((MFFileHandle)fileHandle, (int)bytes, (MFFileSeek)relativity);
	return seek < 0 ? seek : 0;
}

long MFFile_StdTell(void* fileHandle)
{
	return (long)MFFile_Tell((MFFileHandle)fileHandle);
}

//////////////////////////////
// mounted filesystem access

// return a handle to a specific filesystem
MFFileSystemHandle MFFileSystem_GetInternalFileSystemHandle(MFFileSystemHandles fileSystemHandle)
{
	switch(fileSystemHandle)
	{
		case MFFSH_NativeFileSystem:
			MFDebug_Assert(hNativeFileSystem > -1, "Native filesystem is not available...");
			if(hNativeFileSystem < 0)
				MFDebug_Error("Native filesystem is not available...");
			return hNativeFileSystem;
		case MFFSH_MemoryFileSystem:
			MFDebug_Assert(hMemoryFileSystem > -1, "Memory file filesystem is not available...");
			if(hMemoryFileSystem < 0)
				MFDebug_Error("Memory file filesystem is not available...");
			return hMemoryFileSystem;
		case MFFSH_ZipFileSystem:
			MFDebug_Assert(hZipFileSystem > -1, "Zip file filesystem is not available...");
			if(hZipFileSystem < 0)
				MFDebug_Error("Zip file filesystem is not available...");
			return hZipFileSystem;
		default:
			MFDebug_Error(MFStr("Invalid filesystem handle: %d", fileSystemHandle));
			return -1;
	}
}

// mount a filesystem
int MFFileSystem_Mount(MFFileSystemHandle fileSystem, MFMountData *pMountData)
{
	CALLSTACK;

	MFMount *pMount;
	pMount = (MFMount*)MFHeap_Alloc(sizeof(MFMount) + strlen(pMountData->pMountpoint) + 1);
	memset(pMount, 0, sizeof(MFMount));

	pMount->mountFlags = pMountData->flags;
	pMount->fileSystem = fileSystem;
	pMount->priority = pMountData->priority;
	pMount->pMountpoint = (const char*)&pMount[1];
	strcpy((char*)&pMount[1], pMountData->pMountpoint);

	int result = ppFileSystemList[fileSystem]->FSMount(pMount, pMountData);

	if(result < 0)
	{
		MFHeap_Free(pMount);
		return -1;
	}

	// hook it up..
	if(!pMountList)
	{
		pMountList = pMountListEnd = pMount;
		pMount->pPrev = pMount->pNext = NULL;
	}
	else
	{
		MFMount *pT = pMountList;

		while(pT && pT->priority < pMount->priority)
			pT = pT->pNext;

		if(pT)
		{
			if(pT == pMountList)
				pMountList = pMount;

			pMount->pPrev = pT->pPrev;
			pMount->pNext = pT;
			pT->pPrev = pMount;
		}
		else
		{
			pMount->pPrev = pMountListEnd;
			pMountListEnd->pNext = pMount;
			pMountListEnd = pMount;
		}
	}

	return 0;
}

int MFFileSystem_Dismount(const char *pMountpoint)
{
	MFMount *pT = pMountList;

	while(pT && stricmp(pMountpoint, pT->pMountpoint))
		pT = pT->pNext;

	if(pT)
	{
		// dismount
		ppFileSystemList[pT->fileSystem]->FSDismount(pT);

		// remove mount
		if(pMountList == pT)
			pMountList = pT->pNext;
		if(pMountListEnd == pT)
			pMountListEnd = pT->pPrev;

		if(pT->pNext)
			pT->pNext->pPrev = pT->pPrev;
		if(pT->pPrev)
			pT->pPrev->pNext = pT->pNext;

		MFHeap_Free(pT);

		return 0;
	}

	// coundnt find mount..
	return -1;
}

MFTOCEntry *MFFileSystem_GetTocEntry(const char *pFilename, MFTOCEntry *pEntry, int numEntries)
{
	const char *pSearchString = pFilename;
	int nameLen = strlen(pFilename);
	int a;

	bool isDirectory = false;
	for(a=0; a<nameLen; a++)
	{
		if(pFilename[a] == '/')
		{
			isDirectory = true;
			pSearchString = MFStrN(pFilename, a);
			pFilename += a+1;
			break;
		}
	}

	for(a=0; a<numEntries; a++)
	{
		if(!stricmp(pSearchString, pEntry[a].pName))
		{
			if(isDirectory)
			{
				if(pEntry[a].flags & MFTF_Directory)
				{
					return MFFileSystem_GetTocEntry(pFilename, pEntry[a].pChild, pEntry[a].size);
				}
			}
			else
			{
				if(!(pEntry[a].flags & MFTF_Directory))
				{
					return &pEntry[a];
				}
			}
		}
	}

	return NULL;
}

void MFFileSystem_ReleaseToc(MFTOCEntry *pEntry, int numEntries)
{
	for(int a=0; a<numEntries; a++)
	{
		if(pEntry[a].flags & MFTF_Directory)
		{
			MFFileSystem_ReleaseToc(pEntry[a].pChild, pEntry[a].size);
		}
	}

	MFHeap_Free(pEntry);
}

// open a file from the mounted filesystem stack
MFFile* MFFileSystem_Open(const char *pFilename, uint32 openFlags)
{
	MFMount *pMount = pMountList;
	const char *pMountpoint = NULL;

	// search for a mountpoint
	int len = strlen(pFilename);
	for(int a=0; a<len; a++)
	{
		if(pFilename[a] == ':')
		{
			pMountpoint = MFStrN(pFilename, a);
			pFilename += a+1;
			break;
		}

		if(pFilename[a] == '.')
		{
			// if we have found a dot, this cant be a mountpoint
			// (mountpoints may only be alphanumeric)			
			break;
		}
	}

	// search for file through the mount list...
	while(pMount)
	{
		int onlyexclusive = pMount->mountFlags & MFMF_OnlyAllowExclusiveAccess;

		if((!pMountpoint && !onlyexclusive) || (pMountpoint && !stricmp(pMountpoint, pMount->pMountpoint)))
		{
			// open the file from a mount
			MFFileHandle hFile = ppFileSystemList[pMount->fileSystem]->FSOpen(pMount, pFilename, openFlags);

			if(hFile)
				return hFile;
		}

		pMount = pMount->pNext;
	}

	return NULL;
}

// read/write a file to a filesystem
char* MFFileSystem_Load(const char *pFilename, uint32 *pBytesRead)
{
	char *pBuffer = NULL;

	MFFileHandle hFile = MFFileSystem_Open(pFilename, MFOF_Read|MFOF_Binary);

	if(hFile)
	{
		int size = MFFile_GetSize(hFile);

		if(size > 0)
		{
			pBuffer = (char*)MFHeap_Alloc(size);

			int bytesRead = MFFile_Read(hFile, pBuffer, size);

			if(pBytesRead)
				*pBytesRead = bytesRead;
		}

		MFFile_Close(hFile);
	}

	return pBuffer;
}

int MFFileSystem_Save(const char *pFilename, char *pBuffer, uint32 size)
{
	MFDebug_Assert(false, "Not Written....");

	return -1;
}

// if file does not exist, GetSize returns 0, however, a zero length file can also return 0 use 'Exists' to confirm
int MFFileSystem_GetSize(const char *pFilename)
{
	int size = 0;

	MFFileHandle hFile = MFFileSystem_Open(pFilename, MFOF_Read|MFOF_Binary);

	if(hFile)
	{
		size = MFFile_GetSize(hFile);
		MFFile_Close(hFile);
	}

	return size;
}

// returns true if the file can be found within the mounted filesystem stack
bool MFFileSystem_Exists(const char *pFilename)
{
	bool exists = false;

	MFFileHandle hFile = MFFileSystem_Open(pFilename, MFOF_Read|MFOF_Binary);

	if(hFile)
	{
		exists = true;
		MFFile_Close(hFile);
	}

	return exists;
}
