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

	DWORD access = ((openFlags&OF_Read) ? GENERIC_READ : NULL) | ((openFlags&OF_Write) ? GENERIC_WRITE : NULL);
	DBGASSERT(access, "Neither OF_Read nor OF_Write specified.");

	openFiles[fileID].file = CreateFile(pFilename, access, FILE_SHARE_READ, NULL, (openFlags&OF_Write) ? OPEN_ALWAYS : OPEN_EXISTING, NULL, NULL);

	if(openFiles[fileID].file == INVALID_HANDLE_VALUE)
	{
		openFiles[fileID].file = 0;
		return -1;
	}
	DBGASSERT(openFiles[fileID].file != 0, "Aparently CreateFile can return a valid handle of '0' ... THIS IS BAD! FIX ME!");

	openFiles[fileID].state = FS_Ready;
	openFiles[fileID].operation = FO_None;
	openFiles[fileID].createFlags = openFlags;
	openFiles[fileID].offset = 0;
	openFiles[fileID].len = -1;

#if defined(_DEBUG)
	strcpy(openFiles[fileID].filename, pFilename);
#endif

	return fileID;
}

void File_Close(uint32 fileHandle)
{
	CloseHandle(openFiles[fileHandle].file);
	openFiles[fileHandle].file = NULL;
}

uint32 File_Read(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesRead;

	ReadFile(openFiles[fileHandle].file, (void*)pBuffer, bytes, (DWORD*)&bytesRead, NULL);

	openFiles[fileHandle].offset += bytesRead;

	return bytesRead;
}

uint32 File_Write(void *pBuffer, uint32 bytes, uint32 fileHandle)
{
	uint32 bytesWritten;

	WriteFile(openFiles[fileHandle].file, pBuffer, bytes, (LPDWORD)&bytesWritten, NULL);

	openFiles[fileHandle].offset += bytesWritten;

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

uint32 File_Query(uint32 fileHandle)
{
	if(!openFiles[fileHandle].file) return FS_Unavailable;
	return openFiles[fileHandle].state;
}

uint32 File_Seek(FileSeek relativity, uint32 bytes, uint32 fileHandle)
{
	if(!openFiles[fileHandle].file) return -1;

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

	DWORD newPos = SetFilePointer(openFiles[fileHandle].file, bytes, NULL, method);

	openFiles[fileHandle].offset = newPos;

	return newPos;
}

uint32 File_GetSize(uint32 fileHandle)
{
	if(!openFiles[fileHandle].file) return -1;

	if(openFiles[fileHandle].len != -1) return openFiles[fileHandle].len;

	DWORD excess;
	DWORD fileSize = GetFileSize(openFiles[fileHandle].file, &excess);

	DBGASSERT(excess == 0, "Fuji does not support files larger than 4,294,967,295 bytes.");

	openFiles[fileHandle].len = fileSize;

	return fileSize;
}

uint32 File_GetSize(const char *pFilename)
{
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
	bool exists = false;

	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		exists = true;
	}

	return exists;
}
