#if !defined(_FILESYSTEM_H)
#define _FILESYSTEM_H

#define MAX_FILE_COUNT 14

enum OpenFlags
{
	OF_Read = 0,
	OF_Text = 0,
	OF_Write = 1,
	OF_Binary = 2,
	OF_Async = 16
};

enum FileOp
{
	FO_None,
	FO_Read,
	FO_Write
};

enum FileState
{
	FS_Unavailable,
	FS_Ready,
	FS_Busy,
	FS_Waiting
};

enum FileSeek
{
	Seek_Begin,
	Seek_End,
	Seek_Current
};

struct File
{
	uint32 offset;
	uint32 len;

	uint32 operation;	// current operation
	uint32 state;		// current activity state

	uint32 createFlags;	// creat flags

#if defined(_XBOX) || defined(_WINDOWS)
	HANDLE file;
#endif
#if defined(_DEBUG)
	char filename[256];
#endif
};

void FileSystem_InitModule();
void FileSystem_DeinitModule();


char* File_SystemPath(const char *filename);

uint32 File_Open(const char *pFilename, uint32 openFlags = OF_Read|OF_Binary);
void File_Close(uint32 fileHandle);

uint32 File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle);
uint32 File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle);

uint32 File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle);
uint32 File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle);

uint32 File_Query();

uint32 File_Seek(FileSeek relativity, uint32 bytes, uint32 fileHandle);

#endif
