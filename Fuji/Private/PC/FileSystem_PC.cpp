#include "Common.h"
#include "Filesystem.h"
#include "PtrList.h"

extern File *gOpenFiles;

int File_Open(const char *pFilename, uint32 openFlags)
{
	CALLSTACK;

	int fileID;

	for(fileID=0; fileID<MAX_FILE_COUNT; fileID++)
	{
		if(!gOpenFiles[fileID].file) break;
	}

	if(fileID == MAX_FILE_COUNT) return -1;

	DWORD access = ((openFlags&OF_Read) ? GENERIC_READ : NULL) | ((openFlags&OF_Write) ? GENERIC_WRITE : NULL);
	DBGASSERT(access, "Neither OF_Read nor OF_Write specified.");

	gOpenFiles[fileID].file = CreateFile(pFilename, access, FILE_SHARE_READ, NULL, (openFlags&OF_Write) ? OPEN_ALWAYS : OPEN_EXISTING, NULL, NULL);

	if(gOpenFiles[fileID].file == INVALID_HANDLE_VALUE)
	{
		gOpenFiles[fileID].file = 0;
		return -1;
	}
	DBGASSERT(gOpenFiles[fileID].file != 0, "Aparently CreateFile can return a valid handle of '0' ... THIS IS BAD! FIX ME!");

	gOpenFiles[fileID].state = FS_Ready;
	gOpenFiles[fileID].operation = FO_None;
	gOpenFiles[fileID].createFlags = openFlags;
	gOpenFiles[fileID].offset = 0;

	DWORD excess;
	DWORD fileSize = GetFileSize(gOpenFiles[fileID].file, &excess);

	DBGASSERT(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

	gOpenFiles[fileID].len = fileSize;

#if defined(_DEBUG)
	strcpy(gOpenFiles[fileID].filename, pFilename);
#endif

	return fileID;
}

void File_Close(uint32 fileHandle)
{
	CALLSTACK;

	CloseHandle(gOpenFiles[fileHandle].file);
	gOpenFiles[fileHandle].file = NULL;
}

int File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	CALLSTACK;

	uint32 bytesRead;

	ReadFile(gOpenFiles[fileHandle].file, (void*)pBuffer, bytes, (DWORD*)&bytesRead, NULL);

	gOpenFiles[fileHandle].offset += bytesRead;

	return bytesRead;
}

int File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	CALLSTACK;

	uint32 bytesWritten;

	WriteFile(gOpenFiles[fileHandle].file, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);

	gOpenFiles[fileHandle].offset += bytesWritten;

	return bytesWritten;
}

int File_ReadAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	CALLSTACK;

	return 0;
}

int File_WriteAsync(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	CALLSTACK;

	return 0;
}

int File_Query(uint32 fileHandle)
{
	CALLSTACK;

	if(!gOpenFiles[fileHandle].file) return FS_Unavailable;
	return gOpenFiles[fileHandle].state;
}

int File_Seek(FileSeek relativity, int32 bytes, uint32 fileHandle)
{
	CALLSTACK;

	if(!gOpenFiles[fileHandle].file) return -1;

	DWORD method;

	switch(relativity)
	{
		case Seek_Begin:
			method = FILE_BEGIN;
		case Seek_End:
			method = FILE_END;
		case Seek_Current:
			method = FILE_CURRENT;
	}

	DWORD newPos = SetFilePointer(gOpenFiles[fileHandle].file, bytes, NULL, method);

	gOpenFiles[fileHandle].offset = newPos;

	return newPos;
}

uint32 File_GetSize(uint32 fileHandle)
{
	CALLSTACK;

	if(!gOpenFiles[fileHandle].file) return 0;

	return gOpenFiles[fileHandle].len;
}

uint32 File_GetSize(const char *pFilename)
{
	CALLSTACK;

	DWORD fileSize = 0;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD excess;
		fileSize = GetFileSize(hFile, &excess);
		CloseHandle(hFile);

		DBGASSERT(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");
	}

	return fileSize;
}

bool File_Exists(const char *pFilename)
{
	CALLSTACK;

	bool exists = false;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		exists = true;
	}

	return exists;
}
