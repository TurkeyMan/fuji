#include "Common.h"
#include "Filesystem.h"
#include "PtrList.h"

extern File openFiles[MAX_FILE_COUNT];

uint32 File_Open(const char *pFilename, uint32 openFlags)
{
	int fileID;

	for(fileID=0; fileID<MAX_FILE_COUNT; fileID++)
	{
		if(!openFiles[fileID].file) break;
	}

	if(fileID == MAX_FILE_COUNT) return -1;

	openFiles[fileID].file = CreateFile(pFilename, (openFlags&OF_Write) ? GENERIC_WRITE : GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(!openFiles[fileID].file) return -1;

	openFiles[fileID].state = FS_Ready;
	openFiles[fileID].operation = FO_None;
	openFiles[fileID].createFlags = openFlags;
	openFiles[fileID].offset = 0;
	openFiles[fileID].len = -1;

	return fileID;
}

void File_Close(uint32 fileHandle)
{
	CloseHandle(openFiles[fileHandle].file);
}

uint32 File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesRead;

	ReadFile(openFiles[fileHandle].file, (void*)pBuffer, bytes, (DWORD*)&bytesRead, NULL);

	return bytesRead;
}

uint32 File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesWritten;

	WriteFile(openFiles[fileHandle].file, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);

	return bytesWritten;
}

uint32 File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{

	return 0;
}

uint32 File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{

	return 0;
}

uint32 File_Query()
{

	return 0;
}

uint32 File_Seek(FileSeek relativity, uint32 bytes, uint32 fileHandle)
{

	return 0;
}
