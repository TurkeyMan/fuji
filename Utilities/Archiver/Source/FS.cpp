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

void FreeDirectoryEntries(std::vector<char *> &entries)
{
	for(unsigned int i = 0; i < entries.size(); i++) {
		delete[] entries[i];
	}
	
	entries.clear();
}


#if defined(_WIN32) // *** WINDOWS CODE ***

bool GetCurrentDir(char *dir, int maxlen)
{
	if(maxlen == 0) { // This is valid on Windows & Linux but not on some POSIXes, best enforce it
		return(false);
	}
	
	return((_getcwd(dir, maxlen) != NULL));
}

int GetDirectoryEntries(const char *directory, std::vector<char *> &entries)
{
	int numEntries = 0;

	FreeDirectoryEntries(entries);
	
	char *tempDir = new char[strlen(directory) + 4];
	strcpy(tempDir, directory);
	strcat(tempDir, "\\*");

	WIN32_FIND_DATAA findData;
	HANDLE dirHandle = FindFirstFileA(tempDir, &findData);
	delete[] tempDir;
	if(dirHandle == INVALID_HANDLE_VALUE) {
		return(0);
	}

	if(strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
		tempDir = new char[strlen(findData.cFileName) + 1];	
		strcpy(tempDir, findData.cFileName);
		entries.push_back(tempDir);
		++numEntries;
	}

	while(FindNextFileA(dirHandle, &findData)) {
		if(strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
			tempDir = new char[strlen(findData.cFileName) + 1];
			strcpy(tempDir, findData.cFileName);
			entries.push_back(tempDir);
			++numEntries;
		}
	}

	return(numEntries);
}

bool IsDirectory(const char *entry)
{
	DWORD attr = GetFileAttributes(entry);
	if(attr == INVALID_FILE_ATTRIBUTES) {
		return(false);
	}
	
	return(((attr & FILE_ATTRIBUTE_DIRECTORY) != 0));
}

#else // *** POSIX CODE ***

bool GetCurrentDirectory(char *dir, int maxlen)
{
	if(maxlen == 0) { // This is valid on Windows & Linux but not on some POSIXes, best enforce it
		return(false);
	}
	
	return((getcwd(dir, maxlen) != NULL));
}

int GetDirectoryEntries(const char *directory, std::vector<char *> &entries)
{
	int numEntries = 0;
	struct dirent *entry;

	FreeDirectoryEntries(entries);
	
	char *tempDir = new char[strlen(directory) + 4];
	strcpy(tempDir, directory);
	strcat(tempDir, "\\*");

	DIR *dirHandle = opendir(tempDir);
	delete[] tempDir;
	if(dirHandle == NULL) {
		return(0);
	}

	vectorOut.push_back(std::string(findData.cFileName));

	while(readdir(dirHandle)) {
		if((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
			tempDir = new char[strlen(entry->d_name) + 1];
			strcpy(tempDir, entry->d_name);
			entries.push_back(tempDir);
			
			++numEntries;
		}
	}

	closedir(dirHandle);

	return(numEntries);
}

bool IsDirectory(const char *entry)
{
	struct stat statbuf;
	
	if(stat(entry, &statbuf) < 0) {
		return(false);
	}

	return(S_ISDIR(statbuf.st_mode));
}

#endif