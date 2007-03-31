#if !defined(_MFFILESYSTEM_INTERNAL_H)
#define _MFFILESYSTEM_INTERNAL_H

#include "MFFileSystem.h"

struct MFTOCEntry;

// asynchronous file operations
enum MFFileOp
{
	MFFO_None,
	MFFO_Read,
	MFFO_Write
};

// internal functions
void MFFileSystem_InitModule();
void MFFileSystem_DeinitModule();

MFTOCEntry *MFFileSystem_GetTocEntry(const char *pFilename, MFTOCEntry *pEntry, int numEntries);
void MFFileSystem_ReleaseToc(MFTOCEntry *pEntry, int numEntries);

// open file structure
struct MFFile
{
	uint32 offset;
	int length;						// yes we are limiting our files to only 2gb here, but we can define if its an endless stream

	MFFileOp operation;				// current operation
	MFFileState state;				// current activity state

	uint32 createFlags;				// creat flags

	MFFileSystemHandle filesystem;	// filesystem that created the file
	void *pFilesysData;				// extra data related to the file

	MFAsyncOperationCompletedCallback pAsyncCallback;

#if !defined(_RETAIL)
	char fileIdentifier[256];
#endif
};

// mounted filesystem management

enum MFTOCFlags
{
	MFTF_Directory = 1,
	MFTF_SymbolicLink = 2,
	MFTF_Hidden = 4
};

struct MFTOCEntry
{
	MFTOCEntry *pChild;
	MFTOCEntry *pParent;

	char *pName;
	uint32 flags;

	void *pFilesysData;
	uint32 size;
};

struct MFMount
{
	MFVolumeInfo volumeInfo;

	MFMount *pPrev;
	MFMount *pNext;

	MFTOCEntry *pEntries;
	uint32 numFiles;

	void *pFilesysData;
};

struct MFFind
{
	MFMount *pMount;
	void *pFilesystemData;
};

/////////////////////////////////////
// implementing a custom filesystem

struct MFFileSystemCallbacks
{
	void (*RegisterFS)();
	void (*UnregisterFS)();

	int (*FSMount)(MFMount*, MFMountData*);
	int (*FSDismount)(MFMount*);

	MFFile* (*FSOpen)(MFMount*, const char *, uint32);

	int (*Open)(MFFile*, MFOpenData*);
	int (*Close)(MFFile*);
	int (*Read)(MFFile*, void*, uint32, bool);
	int (*Write)(MFFile*, const void*, uint32, bool);
	int (*Seek)(MFFile*, int, MFFileSeek);
	int (*Tell)(MFFile*);

	bool (*FindFirst)(MFFind*, const char*, MFFindData*);
	bool (*FindNext)(MFFind*, MFFindData*);
	void (*FindClose)(MFFind*);

	MFFileState (*Query)(MFFile*);
	int (*GetSize)(MFFile*);
};

MFFileSystemHandle MFFileSystem_RegisterFileSystem(MFFileSystemCallbacks *pCallbacks);
void MFFileSystem_UnregisterFileSystem(MFFileSystemHandle filesystemHandle);

#endif
