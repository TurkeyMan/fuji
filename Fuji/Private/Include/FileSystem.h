#if !defined(_FILESYSTEM_H)
#define _FILESYSTEM_H

#define MAX_FILE_COUNT 14

struct File
{
	uint32 offset;
	uint32 len;

	uint32 operation;
	uint32 state;

#if defined(_XBOX)
	HANDLE file;
#elif defined(_WINDOWS)
	FILE *file;
#endif
};

enum FileSeek
{
	FS_Begin,
	FS_End,
	FS_Current
};

void FileSystem_InitModule();
void FileSystem_DeinitModule();


char* File_SystemPath(char *filename);

uint32 File_Open(const char *pFilename, uint32 openFlags);
void File_Close(uint32 fileHandle);

uint32 File_Read(const char *pBuffer, uint32 bytes, uint32 fileHandle);
uint32 File_Write(const char *pBuffer, uint32 bytes, uint32 fileHandle);

uint32 File_ReadAsync(const char *pBuffer, uint32 bytes, uint32 fileHandle);
uint32 File_WriteAsync(const char *pBuffer, uint32 bytes, uint32 fileHandle);

uint32 File_Query();

uint32 File_Seek(FileSeek relativity, uint32 bytes, uint32 fileHandle);

#endif
