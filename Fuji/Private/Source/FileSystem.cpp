#include "Common.h"
#include "Filesystem.h"

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
