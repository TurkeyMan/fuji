#define VERSION 100

#include "Fuji.h"
#include "F3D.h"
#include "MFSystem.h"

F3DFile *pModel;

int main(int argc, char *argv[])
{
	MFPlatform platform = FP_Unknown;
	char source[1024] = "";
	char dest[1024] = "";
	int a;

	pModel = new F3DFile;

	// process command line
	for(a=1; a<argc; a++)
	{
		if(argv[a][0] == '-' || argv[a][0] == '/')
		{
			for(int b=0; b<FP_Max; b++)
			{
				if(!MFString_CaseCmp(&argv[a][1], MFSystem_GetPlatformString(b)))
				{
					platform = (MFPlatform)b;
					break;
				}
			}

			if(!MFString_CaseCmp(&argv[a][1], "?") || !MFString_CaseCmp(&argv[a][1], "h") || !MFString_CaseCmp(&argv[a][1], "-help"))
			{
				// show help

				return 0;
			}
			else if(!MFString_CaseCmp(&argv[a][1], "v") || !MFString_CaseCmp(&argv[a][1], "version"))
			{
				printf("%.2f", (float)VERSION/100.0f);
				return VERSION;
			}
		}
		else
		{
			if(!source[0])
			{
				MFString_Copy(source, argv[a]);
			}
			else if(!dest[0])
			{
				MFString_Copy(dest, argv[a]);
			}
			else
			{
				printf("Too many parameters...");
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
	for(a=MFString_Length(source); a>0 && source[a-1] != '.'; a--);

	// initialise the strings
	MFString_Copy(pModel->name, "");
	MFString_Copy(pModel->author, "");
	MFString_Copy(pModel->authoringTool, "");
	MFString_Copy(pModel->copyrightString, "");

	// read source file
	if(!MFString_CaseCmp(&source[a], "f3d"))
	{
		// read .f3d file
		a = pModel->ReadF3D(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "ase"))
	{
		// read .ase file
		a = pModel->ReadASE(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "dae"))
	{
		// read collada file...
		a = pModel->ReadDAE(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "obj"))
	{
		// read .obj file
		a = pModel->ReadOBJ(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "md2"))
	{
		// read MD2 file... (Quake2)
		a = pModel->ReadMD2(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "pk3"))
	{
		// read MD3 file... (Quake3)
		a = pModel->ReadMD3(source);
		if(a) return a;
	}
	else if(!MFString_CaseCmp(&source[a], "me2"))
	{
		// read MEMD2 file... (Manu Engine)
		a = pModel->ReadMEMD2(source);
		if(a) return a;
	}
	else
	{
		printf("Unrecognised source file format.\n");
		return 1;
	}

	// process model
	pModel->ProcessSkeletonData();
	pModel->Optimise();
	pModel->ProcessCollisionData();
	pModel->StripModel();

	// generate output filename
	if(!dest[0])
	{
		MFString_Copy(dest, source);

		dest[a] = 0;
		strcat(dest, "mdl");
	}

	// find output extension
	for(a=MFString_Length(dest); a>0 && dest[a-1] != '.'; a--);

	// write output file
	if(!MFString_CaseCmp(&dest[a], "f3d"))
	{
		pModel->WriteF3D(dest);
	}
	else if(!MFString_CaseCmp(&dest[a], "mdl"))
	{
		if(platform == FP_Unknown)
		{
			printf("No system specified, use -pc|-xbox|etc.\n");
			return 1;
		}

		pModel->WriteMDL(dest, platform);
	}
	else
	{
		printf("Unrecognised output file format.\n");
		return 1;
	}

	printf("> %s\n", dest);

	delete pModel;

	return 0;
}
