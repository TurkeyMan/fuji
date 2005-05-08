#if !defined(_MFFILESYSTEM_INTERNAL_H)
#define _MFFILESYSTEM_INTERNAL_H

#include "MFFileSystem.h"

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

// open file structure
struct MFFile
{
	uint32 offset;
	int length;						// yes we are limiting our files to only 2gb here, but we can define if its an endless stream

	MFFileOp operation;				// current operation
	MFFileState state;				// current activity state

	uint32 createFlags;				// creat flags

	FileSystemHandle filesystem;	// filesystem that created the file
	void *pFilesysData;				// extra data related to the file

#if !defined(_RETAIL)
	char fileIdentifier[256];
#endif
};

// mounted filesystem management
struct TOCEntry
{
	uint32 flags;
	const char *pName;
	const char *pData;
	uint32 size;
};

struct MFMount
{
	TOCEntry *pFirstEntry;
};


/////////////////////////////////////
// implementing a custom filesystem

struct MFFileSystemCallbacks
{
	void (*RegisterFS)();
	void (*UnregisterFS)();

	void* (*FSMount)(void *, uint32);
	MFFile* (*FSOpen)(const char*, uint32);

	int (*Open)(MFFile*, MFOpenData*);
	int (*Close)(MFFile*);
	int (*Read)(MFFile*, void*, uint32, bool);
	int (*Write)(MFFile*, void*, uint32, bool);
	int (*Seek)(MFFile*, int, MFFileSeek);
	int (*Tell)(MFFile*);
	MFFileState (*Query)(MFFile*);
	int (*GetSize)(MFFile*);
};

FileSystemHandle MFFileSystem_RegisterFileSystem(MFFileSystemCallbacks *pCallbacks);
void MFFileSystem_UnregisterFileSystem(FileSystemHandle filesystemHandle);

#endif
