#include "FS.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <direent.h>
#include <unistd.h>
#endif

void FreeDirectoryEntries(std::vector<std::string> &entries)
{
	entries.clear();
}


#if defined(_WIN32) // *** WINDOWS CODE ***

bool GetCurrentDir(char *dir, int maxlen)
{
	// This is valid on Windows & Linux but not on some POSIXes, best enforce it
	if(maxlen == 0)
		return false;

	return (_getcwd(dir, maxlen) != NULL);
}

int GetDirectoryEntries(const char *directory, std::vector<std::string> &entries)
{
	int numEntries = 0;
	char tempDir[256];

	FreeDirectoryEntries(entries);

	strcpy(tempDir, directory);
	strcat(tempDir, "*");

	WIN32_FIND_DATAA findData;
	HANDLE dirHandle = FindFirstFile(tempDir, &findData);

	if(dirHandle == INVALID_HANDLE_VALUE)
		return 0;

	BOOL more = true;

	while(more)
	{
		if(strcmp(findData.cFileName, ".") && strcmp(findData.cFileName, "..") && strcmp(findData.cFileName, ".svn"))
		{
			// check if it matches any exclude patterns...

			// if not
			entries.push_back(findData.cFileName);
			++numEntries;
		}

		more = FindNextFile(dirHandle, &findData);
	}

	return numEntries;
}

bool IsDirectory(const char *entry)
{
	DWORD attr = GetFileAttributes(entry);
	if(attr == INVALID_FILE_ATTRIBUTES)
		return false;

	return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

#else // *** POSIX CODE ***

bool GetCurrentDirectory(char *dir, int maxlen)
{
	// This is valid on Windows & Linux but not on some POSIXes, best enforce it
	if(maxlen == 0)
		return false;

	return getcwd(dir, maxlen) != NULL;
}

int GetDirectoryEntries(const char *directory, std::vector<std::string> &entries)
{
	int numEntries = 0;
	struct dirent *entry;

	FreeDirectoryEntries(entries);

	char *tempDir = malloc(strlen(directory) + 4);
	strcpy(tempDir, directory);
	strcat(tempDir, "*");

	DIR *dirHandle = opendir(tempDir);
	free(tempDir);
	if(dirHandle == NULL)
		return 0;

	vectorOut.push_back(std::string(findData.cFileName));

	while(readdir(dirHandle))
	{
		if((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
		{
			tempDir = new char[strlen(entry->d_name) + 1];
			strcpy(tempDir, entry->d_name);
			entries.push_back(tempDir);

			++numEntries;
		}
	}

	closedir(dirHandle);

	return numEntries;
}

bool IsDirectory(const char *entry)
{
	struct stat statbuf;

	if(stat(entry, &statbuf) < 0)
		return false;

	return S_ISDIR(statbuf.st_mode);
}

#endif
