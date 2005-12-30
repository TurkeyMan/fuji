#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "MFHeap.h"

MFPlatform gCurrentPlatform = FP_XBox;

void main()
{
	MFCALLSTACK;

	MFSystem_GameLoop();
}

char* FixXBoxFilename(const char *pFilename)
{
	if(!pFilename) return NULL;

	int len = strlen(pFilename);

	char *pXFilename = MFStr("%s", pFilename);

	for(int a=0; a<len; a++)
	{
		if(pXFilename[a] == '/') pXFilename[a] = '\\';
	}

	return pXFilename;
}

void MFSystem_InitModulePlatformSpecific()
{
}

void MFSystem_DeinitModulePlatformSpecific()
{
}

void MFSystem_HandleEventsPlatformSpecific()
{
}

void MFSystem_UpdatePlatformSpecific()
{
}

void MFSystem_DrawPlatformSpecific()
{
}

uint64 MFSystem_ReadRTC()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

uint64 MFSystem_GetRTCFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}

const char * MFSystem_GetSystemName()
{
	return "xbox";
}
