#if !defined(_FILESYSTEM_H)
#define _FILESYSTEM_H

#if defined(_LINUX)
#include <stdio.h>
#endif

#define MAX_FILE_COUNT 14

enum OpenFlags
{
	OF_Read		= 1,
	OF_Write	= 2,
	OF_Text		= 4,
	OF_Binary	= 8,
	OF_Async	= 16
};

enum FileOp
{
	FO_None,
	FO_Read,
	FO_Write
};

enum FileState
{
	FS_Unavailable = -1,
	FS_Ready = 0,
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
#elif defined(_LINUX)
	int file;
#elif defined(_DC)
	int file;
#else
	int file;
#endif
#if defined(_DEBUG)
	char filename[256];
#endif
};

void FileSystem_InitModule();
void FileSystem_DeinitModule();


char* File_SystemPath(const char *filename);
char* File_HomePath(const char *filename);

int File_Open(const char *pFilename, uint32 openFlags = OF_Read|OF_Binary);
void File_Close(int fileHandle);

char* File_Load(const char *pFilename, uint32 *pBytesRead = NULL);

int File_Read(void *pBuffer, uint32 bytes, int fileHandle);
int File_Write(void *pBuffer, uint32 bytes, int fileHandle);

int File_ReadAsync(void *pBuffer, uint32 bytes, int fileHandle);
int File_WriteAsync(void *pBuffer, uint32 bytes, int fileHandle);

int File_Query(int fileHandle);

int File_Seek(FileSeek relativity, int32 bytes, int fileHandle);

uint32 File_GetSize(int fileHandle);
uint32 File_GetSize(const char *pFilename);

bool File_Exists(const char *pFilename);

#endif
