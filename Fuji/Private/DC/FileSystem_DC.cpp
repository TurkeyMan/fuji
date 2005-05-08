#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "PtrList.h"

extern File *gOpenFiles;

int File_Open(const char *pFilename, uint32 openFlags)
{
	int fileID;

	for(fileID=0; fileID<MAX_FILE_COUNT; fileID++)
	{
		if(!gOpenFiles[fileID].file) break;
	}

	if(fileID == MAX_FILE_COUNT) return -1;

	return -1;

	return fileID;
}

void File_Close(uint32 fileHandle)
{

	gOpenFiles[fileHandle].file = NULL;
}

int File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesRead = 0;



	gOpenFiles[fileHandle].offset += bytesRead;

	return bytesRead;
}

int File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesWritten = 0;



	gOpenFiles[fileHandle].offset += bytesWritten;

	return bytesWritten;
}

int File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{

	return 0;
}

int File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{

	return 0;
}

int File_Query(uint32 fileHandle)
{
	if(!gOpenFiles[fileHandle].file) return FS_Unavailable;
	return gOpenFiles[fileHandle].state;
}

int File_Seek(FileSeek relativity, int32 bytes, uint32 fileHandle)
{
	if(!gOpenFiles[fileHandle].file) return -1;


	return gOpenFiles[fileHandle].offset;
}

uint32 File_GetSize(uint32 fileHandle)
{
	if(!gOpenFiles[fileHandle].file) return 0;

	return gOpenFiles[fileHandle].len;
}

uint32 File_GetSize(const char *pFilename)
{
	uint32 fileSize = 0;

	return fileSize;
}

bool File_Exists(const char *pFilename)
{
	bool exists = false;


	return exists;
}
