#if !defined(_MFFILESYSTEM_H)
#define _MFFILESYSTEM_H

struct MFFile;
typedef MFFile* MFFileHandle;
typedef int FileSystemHandle;
typedef void (*AsyncOperationCompletedCallback)(MFFile *);

// flags when opening a file
enum MFOpenFlags
{
	MFOF_Read	= 1,
	MFOF_Write	= 2,
	MFOF_Text	= 4,
	MFOF_Binary	= 8,
	MFOF_Async	= 16
};

// seek offset
enum MFFileSeek
{
	MFSeek_Begin,
	MFSeek_Current,
	MFSeek_End
};

// MFFile_Query() returns one of these
enum MFFileState
{
	MFFS_Unavailable = -1,
	MFFS_Ready = 0,
	MFFS_Busy,
	MFFS_Waiting
};

// handle to the internal filesystems
extern FileSystemHandle hNativeFileSystem;
extern FileSystemHandle hMemoryFileSystem;


// interface functions
char* MFFile_SystemPath(const char *filename = "");
char* MFFile_HomePath(const char *filename = "");


///////////////////////////
// file access functions

// open file base data
struct MFOpenData
{
	int cbSize;
	uint32 openFlags;
};

// open/close a file
MFFileHandle MFFile_Open(FileSystemHandle fileSystem, MFOpenData *pOpenData);
int MFFile_Close(MFFileHandle fileHandle);

// read/write functions
int MFFile_Read(MFFileHandle fileHandle, void *pBuffer, uint32 bytes, bool async = false);
int MFFile_Write(MFFileHandle fileHandle, void *pBuffer, uint32 bytes, bool async = false);

// offset management (these are stdio function signature compliant)
int MFFile_Seek(MFFileHandle fileHandle, int bytes, MFFileSeek relativity);
int MFFile_Tell(MFFileHandle fileHandle);

// return the state of a file
MFFileState MFFile_Query(MFFileHandle fileHandle);

// get file stream length (retuurs -1 for an undefined stream length)
int MFFile_GetSize(MFFileHandle fileHandle);

// stdio signiture functions (these can be used as callbacks to many libs and API's)
int MFFile_StdClose(void* stream);
uint32 MFFile_StdRead(void *buffer, uint32 size, uint32 count, void* stream);
uint32 MFFile_StdWrite(void *buffer, uint32 size, uint32 count, void* stream);
int MFFile_StdSeek(void* stream, long offset, int whence);
long MFFile_StdTell(void* stream);


//////////////////////////////
// mounted filesystem access

// mount flags
enum MFMountFlags
{
	MFMF_FlattenDirectoryStructure	// flattens the directory heirarchy
};

// mount a filesystem
int MFFileSystem_Mount(FileSystemHandle fileSystem, void *pMountData, uint32 flags);

// open a file from the mounted filesystem stack
MFFileHandle MFFileSystem_Open(const char *pFilename, uint32 openFlags = MFOF_Read|MFOF_Binary);

// read/write a file to a filesystem
char* MFFileSystem_Load(const char *pFilename, uint32 *pBytesRead = NULL);
void MFFileSystem_Save(const char *pFilename, char *pBuffer, uint32 size);

// if file does not exist, GetSize returns 0, however, a zero length file can also return 0 use 'Exists' to confirm
uint32 MFFileSystem_GetSize(const char *pFilename);

// returns true if the file can be found within the mounted filesystem stack
bool MFFileSystem_Exists(const char *pFilename);


/////////////////////////////////////
// implementing a custom filesystem

// this can be exposed to the game later on when the design is more settled

#endif
