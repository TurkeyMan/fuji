#define VERSION 100

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include "Common.h"
#include "F3D.h"

F3DFile model;

int main(int argc, char *argv[])
{
	FujiPlatforms platform = FP_Unknown;
	char source[1024] = "";
	char dest[1024] = "";
	int a, b;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!stricmp(&argv[a][1], gPlatformStrings[b]))
				{
					platform = (FujiPlatforms)b;
					break;
				}
			}

			if(!stricmp(&argv[a][1], "?") || !stricmp(&argv[a][1], "h") || !stricmp(&argv[a][1], "-help"))
			{
				// show help

				return 0;
			}
			else if(!stricmp(&argv[a][1], "v") || !stricmp(&argv[a][1], "version"))
			{
				printf("%.2f", (float)VERSION/100.0f);
				return 0;
			}
		}
		else
		{
			if(!source[0])
			{
				strcpy(source, argv[a]);
			}
			else if(!dest[0])
			{
				strcpy(dest, argv[a]);
			}
			else
			{
				printf("Too many paramaters...");
				return 1;
			}
		}
	}

	if(!source[0])
	{
		printf("No source file specified.\n");
		return 1;
	}

	// find source file extension
	for(a=(int)strlen(source); a>0 && source[a-1] != '.'; a--);

	// read source file
	if(!stricmp(&source[a], "f3d"))
	{
		a = model.ReadFromDisk(source);
		if(a) return a;
	}
	else if(!stricmp(&source[a], "ase"))
	{
		a = model.ReadASE(source);
		if(a) return a;

		model.ProcessSkeletonData();
		model.Optimise();
	}
	else if(!stricmp(&source[a], "dae"))
	{
		// read collada file...
/*
		a = model.ReadDAE(pSource);
		if(a) return a;

		model.ProcessSkeletonData();
		model.Optimise();
*/
	}
	else
	{
		printf("Unrecognised source file format.\n");
		return 1;
	}

	// generate output filename
	if(!dest[0])
	{
		strcpy(dest, source);

		dest[a] = 0;
		strcat(dest, "mdl");
	}

	// find output extension
	for(a=(int)strlen(dest); a>0 && dest[a-1] != '.'; a--);

	// write output file
	if(!stricmp(&dest[a], "f3d"))
	{
		model.WriteToDisk(dest);
	}
	else if(!stricmp(&dest[a], "mdl"))
	{
		if(platform == FP_Unknown)
		{
			printf("No system specified, use -pc|-xbox|etc.\n");
			return 1;
		}

		model.WriteMDL(dest, platform);
	}
	else
	{
		printf("Unrecognised output file format.\n");
		return 1;
	}

	return 0;
}
