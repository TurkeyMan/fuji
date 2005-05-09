#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "FileSystem/MFFileSystemNative.h"
#include "FileSystem/MFFileSystemMemory.h"
#include "Ptrlist.h"

PtrListDL<MFFile> gOpenFiles;

PtrListDL<MFMount> gMounts;
MFMount **ppMountList;
int gMountCount = 0;

PtrListDL<MFFileSystemCallbacks> pFileSystemCallbacks;
MFFileSystemCallbacks **ppFileSystemList;

// internal filesystems
FileSystemHandle hNativeFileSystem = -1;
FileSystemHandle hMemoryFileSystem = -1;

void MFFileSystem_InitModule()
{
	gOpenFiles.Init("Open Files", gDefaults.filesys.maxOpenFiles);

	gMounts.Init("FileSystem Mounts", gDefaults.filesys.maxFileSystemMounts);
	ppMountList = (MFMount**)Heap_Alloc(sizeof(MFMount*) * gDefaults.filesys.maxFileSystemMounts);
	memset(ppMountList, 0, sizeof(MFMount*) * gDefaults.filesys.maxFileSystemMounts);

	pFileSystemCallbacks.Init("File System Callbacls", gDefaults.filesys.maxFileSystems);
	ppFileSystemList = (MFFileSystemCallbacks**)Heap_Alloc(sizeof(MFFileSystemCallbacks*) * gDefaults.filesys.maxFileSystems);
	memset(ppFileSystemList, 0, sizeof(MFFileSystemCallbacks*) * gDefaults.filesys.maxFileSystems);

	// mount filesystems
	MFFileSystemNative_InitModule();
	MFFileSystemMemory_InitModule();

	MFMountDataNative mountData;
	mountData.cbSize = sizeof(MFMountDataNative);
	mountData.flags = MFMF_Recursive|MFMF_FlattenDirectoryStructure;
	mountData.pPath = MFFile_SystemPath();

	MFFileSystem_Mount(hNativeFileSystem, &mountData);
}

void MFFileSystem_DeinitModule()
{
	// dismount filesystems
	MFFileSystemMemory_DeinitModule();
	MFFileSystemNative_DeinitModule();

	Heap_Free(ppFileSystemList);

	pFileSystemCallbacks.Deinit();
	gMounts.Deinit();
	gOpenFiles.Deinit();
}

FileSystemHandle MFFileSystem_RegisterFileSystem(MFFileSystemCallbacks *pCallbacks)
{
	for(uint32 a=0; a<gDefaults.filesys.maxFileSystems; a++)
	{
		if(ppFileSystemList[a] == NULL)
		{
			ppFileSystemList[a] = pFileSystemCallbacks.Create();
			memcpy(ppFileSystemList[a], pCallbacks, sizeof(MFFileSystemCallbacks));

			ppFileSystemList[a]->RegisterFS();

			return a;
		}
	}

	DBGASSERT(false, STR("Exceeded maximum of %d Filesystems. Modify 'gDefaults.filesys.maxFileSystems'.", gDefaults.filesys.maxFileSystems));

	return -1;
}

void MFFileSystem_UnregisterFileSystem(FileSystemHandle filesystemHandle)
{
	DBGASSERT(ppFileSystemList[filesystemHandle], "Filesystem not mounted");

	ppFileSystemList[filesystemHandle]->UnregisterFS();
	pFileSystemCallbacks.Destroy(ppFileSystemList[filesystemHandle]);
	ppFileSystemList[filesystemHandle] = NULL;
}

// interface functions
char* MFFile_SystemPath(const char *filename)
{
	CALLSTACK;

#if defined(_XBOX)
	return STR("D:\\Data\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Data/%s", filename);
#elif defined(_LINUX)
	return STR("Data/%s", filename);
#else
	return STR("%s", filename);
#endif
}

char* MFFile_HomePath(const char *filename)
{
	CALLSTACK;

#if defined(_XBOX)
	return STR("E:\\Home\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Home/%s", filename);
#else
	return STR("%s", filename);
#endif
}

///////////////////////////
// file access functions
MFFile* MFFile_Open(FileSystemHandle fileSystem, MFOpenData *pOpenData)
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

int MFFile_Write(MFFile* fileHandle, void *pBuffer, uint32 bytes, bool async)
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
	return MFFile_Seek((MFFileHandle)fileHandle, (int)bytes, (MFFileSeek)relativity);
}

long MFFile_StdTell(void* fileHandle)
{
	return (int)MFFile_Tell((MFFileHandle)fileHandle);
}

//////////////////////////////
// mounted filesystem access

// mount a filesystem
int MFFileSystem_Mount(FileSystemHandle fileSystem, MFMountData *pMountData)
{
	CALLSTACK;

	MFMount *pMount = gMounts.Create();
	memset(pMount, 0, sizeof(MFMount));

	pMount->mountFlags = pMountData->flags;
	pMount->fileSystem = fileSystem;

	int result = ppFileSystemList[fileSystem]->FSMount(pMount, pMountData);

	if(result < 0)
	{
		gMounts.Destroy(pMount);
		return -1;
	}

	ppMountList[gMountCount] = pMount;
	++gMountCount;

	return gMountCount;
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
			pSearchString = STRn(pFilename, a);
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

// open a file from the mounted filesystem stack
MFFile* MFFileSystem_Open(const char *pFilename, uint32 openFlags)
{
	// find file in filesystem stack
	int mountID = gMountCount-1;

	MFMount *pMount = NULL;
	MFTOCEntry *pEntry = NULL;

	// search from the top of the stack downwards...
	while(mountID >= 0)
	{
		pMount = ppMountList[mountID];

		// recurse toc
		pEntry = MFFileSystem_GetTocEntry(pFilename, pMount->pEntries, pMount->numFiles);

		if(pEntry)
		{
			// open the file from a mount
			return ppFileSystemList[mountID]->FSOpen(pMount, pEntry, openFlags);
		}

		--mountID;
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
			pBuffer = (char*)Heap_Alloc(size);

			*pBytesRead = MFFile_Read(hFile, pBuffer, size);
		}

		MFFile_Close(hFile);
	}

	return pBuffer;
}

void MFFileSystem_Save(const char *pFilename, char *pBuffer, uint32 size)
{
	DBGASSERT(false, "Not Written....");
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
