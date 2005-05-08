#include "Common.h"
#include "MFFileSystem_Internal.h"
#include "PtrList.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern File gOpenFiles[MAX_FILE_COUNT];

int File_Open(const char *pFilename, uint32 openFlags)
{
	int32 fileID;

	for(fileID = 0; fileID < MAX_FILE_COUNT; fileID++) {
		if(!gOpenFiles[fileID].file) break;
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

	gOpenFiles[fileID].file = open(pFilename, flags);

	if(gOpenFiles[fileID].file == -1) {
		gOpenFiles[fileID].file = 0;
		return(-1);
	}
	

	gOpenFiles[fileID].state = FS_Ready;
	gOpenFiles[fileID].operation = FO_None;
	gOpenFiles[fileID].createFlags = openFlags;
	gOpenFiles[fileID].offset = 0;
	gOpenFiles[fileID].len = 0;

#if defined(_DEBUG)
	strcpy(gOpenFiles[fileID].filename, pFilename);
#endif

	return fileID;
}

void File_Close(uint32 fileHandle)
{
	close(gOpenFiles[fileHandle].file);
	gOpenFiles[fileHandle].file = 0;
}

int File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	ssize_t bytesRead;

	bytesRead = read(gOpenFiles[fileHandle].file, (void *)pBuffer, bytes);
	if(bytesRead < 0) // read() returns -1 on error
		bytesRead = 0;

	gOpenFiles[fileHandle].offset += bytesRead;

	return(bytesRead);
}

// FIXME
int File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	ssize_t bytesWritten;

	bytesWritten = write(gOpenFiles[fileHandle].file, pBuffer, (size_t)bytes);
	if(bytesWritten < 0) // write() returns -1 on error
		bytesWritten = 0;

	gOpenFiles[fileHandle].offset += bytesWritten;

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
	if(!gOpenFiles[fileHandle].file) {
		return((uint32)FS_Unavailable);
	}

	return(gOpenFiles[fileHandle].state);
}

int File_Seek(FileSeek relativity, int32 bytes, uint32 fileHandle)
{
	off_t newOffset;
	int whence;
	
	if(!gOpenFiles[fileHandle].file) return -1;

	switch(relativity)
	{
		case Seek_Begin:
			whence = SEEK_SET;
		case Seek_End:
			whence = SEEK_END;
		case Seek_Current:
			whence = SEEK_CUR;
	}

	
	newOffset = lseek(gOpenFiles[fileHandle].file, bytes, whence);
	if(newOffset != -1) {
		gOpenFiles[fileHandle].offset = newOffset;
	}

	return(gOpenFiles[fileHandle].offset);
}

uint32 File_GetSize(uint32 fileHandle)
{
	struct stat fileStats;

	if(fstat(gOpenFiles[fileHandle].file, &fileStats) == -1) {
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
