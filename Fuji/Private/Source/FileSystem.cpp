#include "Common.h"
#include "Filesystem.h"
#include "PtrList.h"

File openFiles[MAX_FILE_COUNT];

void FileSystem_InitModule()
{

}

void FileSystem_DeinitModule()
{

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
