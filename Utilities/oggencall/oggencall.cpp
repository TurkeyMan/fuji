#include <windows.h>
#include <stdio.h>

void ConvertWav(const char *pSearch);

#include <fstream>

int main(int argc, char *argv[])
{
	if(argc < 1)
	{
		printf("Not enough parameters\n");
		return 1;
	}

	char poo[1024];

//	strcpy(poo, argv[1]);
	strcpy(poo, "G:\\Music");

	ConvertWav(poo);

	return 0;
}

char *GetExt(char *pFilename)
{
	for(size_t a=strlen(pFilename); pFilename[a] != '.' && a>0; a--);
	return &pFilename[a+1];
}

void ConvertWav(const char *pSearch)
{
	WIN32_FIND_DATA fd;

	char poo[1024];
	strcpy(poo, pSearch);
	strcat(poo, "\\*");

	HANDLE find = FindFirstFile(poo, &fd);

	if(find == INVALID_HANDLE_VALUE)
	{
		printf("Something went very wrong :(\n");
		return;
	}

	BOOL found = true;

	while(found)
	{
		if(strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
		{
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(poo, pSearch);
				strcat(poo, "\\");
				strcat(poo, fd.cFileName);
				ConvertWav(poo);
			}
			else if(!stricmp(GetExt(fd.cFileName),"wav"))
			{
				strcpy(poo, "C:\\oggenc -q4 \"");
				strcat(poo, pSearch);
				strcat(poo, "\\");
				strcat(poo, fd.cFileName);
				strcat(poo, "\"");

				system(poo);

				strcpy(poo, "del \"");
				strcat(poo, pSearch);
				strcat(poo, "\\");
				strcat(poo, fd.cFileName);
				strcat(poo, "\"");

				system(poo);
			}
		}

		found = FindNextFile(find, &fd);
	}

	FindClose(find);
}