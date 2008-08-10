#include "Fuji.h"

#if MF_SYSTEM == MF_DRIVER_XBOX

#include "MFSystem_Internal.h"
#include "MFHeap.h"

MFPlatform gCurrentPlatform = FP_XBox;

char *gpCommandLineBuffer = NULL;

char* FixXBoxFilename(const char *pFilename)
{
	if(!pFilename) return NULL;

	char *pXFilename;

	int len = MFString_Length(pFilename);

	if(len > 1 && pFilename[1] != ':')
	{
		pXFilename = (char*)MFStr("D:\\%s", pFilename);
		len += 3;
	}
	else
		pXFilename = (char*)MFStr("%s", pFilename);

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
	return "XBox";
}

#endif // MF_SYSTEM
