#include "Common.h"
#include "Filesystem.h"
#include "PtrList.h"

PtrListDL<File> file;

void FileSystem_InitModule()
{
	file.Init("File Handles", MAX_FILE_COUNT);
}

void FileSystem_DeinitModule()
{
//	file.Deinit();
}

char* File_SystemPath(char *filename)
{
#if defined(_XBOX)
	return STR("D:\\Data\\%s", filename);
#elif defined(_WINDOWS)
	return STR("Data\\%s", filename);
#else
	return STR("%s", filename);
#endif
}
