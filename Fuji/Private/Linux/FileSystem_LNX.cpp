#include "Common.h"
#include "FileSystem.h"
#include "PtrList.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern File openFiles[MAX_FILE_COUNT];

int32 File_Open(const char *pFilename, uint32 openFlags)
{
	int32 fileID;

	for(fileID = 0; fileID < MAX_FILE_COUNT; fileID++) {
		if(!openFiles[fileID].file) break;
	}

	if(fileID == MAX_FILE_COUNT) {
		return(-1);
	}

	int flags;
	
	if(openFlags & OF_Read) {
		if(openFlags & OF_Write) {
			flags = O_RDWR | O_CREAT;
		} else {
			flags = O_RDONLY;
		}
	} else if(openFlags & OF_Write) {
		flags = O_WRONLY | O_CREAT;
	} else {
		DBGASSERT(0, "Neither OF_Read nor OF_Write specified.");
	}

	openFiles[fileID].file = open(pFilename, flags);

	if(openFiles[fileID].file == -1) {
		openFiles[fileID].file = 0;
		return(-1);
	}
	

	openFiles[fileID].state = FS_Ready;
	openFiles[fileID].operation = FO_None;
	openFiles[fileID].createFlags = openFlags;
	openFiles[fileID].offset = 0;
	openFiles[fileID].len = 0;

#if defined(_DEBUG)
	strcpy(openFiles[fileID].filename, pFilename);
#endif

	return fileID;
}

void File_Close(uint32 fileHandle)
{
	close(openFiles[fileHandle].file);
	openFiles[fileHandle].file = 0;
}

uint32 File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	int bytesRead;

	bytesRead = read(openFiles[fileHandle].file, (void *)pBuffer, bytes);
	if(bytesRead < 0)
		bytesRead = 0;

	openFiles[fileHandle].offset += bytesRead;

	return(bytesRead);
}

// FIXME
uint32 File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesWritten;

//	bytesWritten = write();
}

uint32 File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	return(0);
}

uint32 File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	return(0);
}

uint32 File_Query(uint32 fileHandle)
{
	if(!openFiles[fileHandle].file) {
		return((uint32)FS_Unavailable);
	}

	return(openFiles[fileHandle].state);
}

uint32 File_Seek(FileSeek relativity, uint32 bytes, uint32 fileHandle)
{
}

uint32 File_GetSize(uint32 fileHandle)
{
}

uint32 File_GetSize(const char *pFilename)
{
}

bool File_Exists(const char *pFilename)
{
}
