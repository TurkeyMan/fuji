#include "Common.h"
#include "FileSystem.h"
#include "PtrList.h"

File *gOpenFiles;

void FileSystem_InitModule()
{
	gOpenFiles = (File*)Heap_Alloc(sizeof(File) * gDefaults.filesys.maxOpenFiles);
	memset(gOpenFiles, 0, sizeof(File) * gDefaults.filesys.maxOpenFiles);

	for(int a=0; a<MAX_FILE_COUNT; a++)
	{
		gOpenFiles[a].file = NULL;
	}
}

void FileSystem_DeinitModule()
{
#if defined(_DEBUG)
	bool fileCloseRave = false;

	for(int a=0; a<MAX_FILE_COUNT; a++)
	{
		if(gOpenFiles[a].file)
		{
			if(!fileCloseRave)
			{
				LOGD("FileSystem left open file's:");
				fileCloseRave = true;
			}

			LOGD(STR("  %s", gOpenFiles[a].filename));
		}
	}
#endif
}

char* File_SystemPath(const char *filename)
{
#if defined(_XBOX)
	return STR("D:\\Data\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Data/%s", filename);
#elif defined(_LINUX)
	return STR("Data/%s", filename);
#else
	return STR("%s", filename);
#endif
}

char* File_HomePath(const char *filename)
{
#if defined(_XBOX)
	return STR("E:\\Home\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Home/%s", filename);
#else
	return STR("%s", filename);
#endif
}

char* File_Load(const char *pFilename, uint32 *pBytesRead)
{
	char *pBuffer = NULL;

	if(pBytesRead) *pBytesRead = 0;

	int handle = File_Open(pFilename);
	if(handle < 0) return NULL;

	uint32 filesize = File_GetSize(handle);

	if(filesize)
	{
		pBuffer = (char*)Heap_Alloc(filesize);
		File_Read(pBuffer, filesize, handle);
	}

	File_Close(handle);

	if(pBytesRead) *pBytesRead = filesize;

	return pBuffer;
}
