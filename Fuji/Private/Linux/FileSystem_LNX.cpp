#include "Common.h"
#include "FileSystem.h"
#include "PtrList.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern File openFiles[MAX_FILE_COUNT];

int File_Open(const char *pFilename, uint32 openFlags)
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

int File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	ssize_t bytesRead;

	bytesRead = read(openFiles[fileHandle].file, (void *)pBuffer, bytes);
	if(bytesRead < 0) // read() returns -1 on error
		bytesRead = 0;

	openFiles[fileHandle].offset += bytesRead;

	return(bytesRead);
}

// FIXME
int File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	ssize_t bytesWritten;

	bytesWritten = write(openFiles[fileHandle].file, pBuffer, (size_t)bytes);
	if(bytesWritten < 0) // write() returns -1 on error
		bytesWritten = 0;

	openFiles[fileHandle].offset += bytesWritten;

	return(bytesWritten);
}

int File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	return(0);
}

int File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	return(0);
}

int File_Query(uint32 fileHandle)
{
	if(!openFiles[fileHandle].file) {
		return((uint32)FS_Unavailable);
	}

	return(openFiles[fileHandle].state);
}

int File_Seek(FileSeek relativity, int32 bytes, uint32 fileHandle)
{
	off_t newOffset;
	int whence;
	
	if(!openFiles[fileHandle].file) return -1;

	switch(relativity)
	{
		case Seek_Begin:
			whence = SEEK_SET;
		case Seek_End:
			whence = SEEK_END;
		case Seek_Current:
			whence = SEEK_CUR;
	}

	
	newOffset = lseek(openFiles[fileHandle].file, bytes, whence);
	if(newOffset != -1) {
		openFiles[fileHandle].offset = newOffset;
	}

	return(openFiles[fileHandle].offset);
}

uint32 File_GetSize(uint32 fileHandle)
{
	struct stat fileStats;

	if(fstat(openFiles[fileHandle].file, &fileStats) == -1) {
		return(0);
	}

	return(fileStats.st_size);
}

uint32 File_GetSize(const char *pFilename)
{
	struct stat fileStats;

	if(stat(pFilename, &fileStats) == -1) {
		return(0);
	}

	return(fileStats.st_size);
}

bool File_Exists(const char *pFilename)
{
	struct stat fileStats;
	int rv;

	rv = stat(pFilename, &fileStats);
	
	if(rv == -1) {
		return(false);
	} else {
		return(true);
	}
}
