#include "Common.h"
#include "Filesystem.h"
#include "PtrList.h"

File openFiles[MAX_FILE_COUNT];

void FileSystem_InitModule()
{
	for(int a=0; a<MAX_FILE_COUNT; a++)
	{
		openFiles[a].file = NULL;
	}
}

void FileSystem_DeinitModule()
{
#if defined(_DEBUG)
	bool fileCloseRave = false;

	for(int a=0; a<MAX_FILE_COUNT; a++)
	{
		if(openFiles[a].file)
		{
			if(!fileCloseRave)
			{
				LOGD("FileSystem left open file's:");
				fileCloseRave = true;
			}

			LOGD(STR("  %s", openFiles[a].filename));
		}
	}
#endif
}

char* File_SystemPath(const char *filename)
{
#if defined(_XBOX)
	return STR("D:\\Data\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Data\\%s", filename);
#else
	return STR("%s", filename);
#endif
}

char* File_HomePath(const char *filename)
{
#if defined(_XBOX)
	return STR("E:\\Home\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Home\\%s", filename);
#else
	return STR("%s", filename);
#endif
}
