#if !defined(_MFFILESYSTEM_INTERNAL_H)
#define _MFFILESYSTEM_INTERNAL_H

#include "MFFileSystem.h"
#include "MFThread.h"
#include "MFPtrList.h"

struct MFTOCEntry;
struct MFMount;

// internal functions
MFInitStatus MFFileSystem_InitModule();
void MFFileSystem_DeinitModule();

MFMount *MFFileSystem_FindVolume(const char *pVolumeName);

MFTOCEntry *MFFileSystem_GetTocEntry(const char *pFilename, MFTOCEntry *pEntry, int numEntries);
void MFFileSystem_ReleaseToc(MFTOCEntry *pEntry, int numEntries);

// open file structure
struct MFFile
{
	int64 offset;
	int64 length;

	uint32 createFlags;				// creat flags

	MFFileSystemHandle filesystem;	// filesystem that created the file
	void *pFilesysData;				// extra data related to the file

#if !defined(_RETAIL)
	char fileIdentifier[256];
#endif
};

enum MFFileOp
{
	MFFJ_Exit = 0,
	MFFJ_Open,
	MFFJ_Close,
	MFFJ_Read,
	MFFJ_Write,
	MFFJ_Seek,
	MFFJ_Stat,
	MFFJ_Tell,
	MFFJ_GetSize,
	MFFJ_Load,
	MFFJ_Save,
	MFFJ_Max,
	MFFJ_ForceInt = 0x7FFFFFFF
};

struct MFJob
{
	MFFileOp job;
	MFFile *pFile;
	int64 result;
	union
	{
		struct Open
		{
			MFOpenData openData;
			char buffer[64 - sizeof(MFOpenData)];
		} open;
		struct Read
		{
			void *pBuffer;
			uint32 bytes;
		} read;
		struct Write
		{
			int64 bytes;
			const void *pBuffer;
		} write;
		struct Seek
		{
			int64 bytes;
			MFFileSeek whence;
		} seek;
		struct Stat
		{
		} stat;
		struct Load
		{
			const char *pFilename;
			void *pBuffer;
		} load;
		struct Save
		{
			const char *pFilename;
			void *pBuffer;
			int64 size;
		} save;
	} data;
	volatile MFJobState status;
	int completion;
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
	char searchPattern[256];
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
	int (*Read)(MFFile*, void*, int64);
	int (*Write)(MFFile*, const void*, int64);
	int (*Seek)(MFFile*, int64, MFFileSeek);

	bool (*FindFirst)(MFFind*, const char*, MFFindData*);
	bool (*FindNext)(MFFind*, MFFindData*);
	void (*FindClose)(MFFind*);
};

struct MFFileSystem
{
	char name[64];
	MFFileSystemCallbacks callbacks;
	MFThread thread;
	MFSemaphore semaphore;
	MFJob ** ppJobQueue;
	volatile int readJob;
	volatile int writeJob;
	int numJobs;
	MFPtrListDL<MFJob> jobs;
};

MFFileSystemHandle MFFileSystem_RegisterFileSystem(const char *pFilesystemName, MFFileSystemCallbacks *pCallbacks);
void MFFileSystem_UnregisterFileSystem(MFFileSystemHandle filesystemHandle);

#endif
