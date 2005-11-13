#include "Fuji.h"
#include "System_Internal.h"

void main()
{
	SYSTEMTIME s;
	GetSystemTime(&s);
	srand(s.wMilliseconds|(s.wSecond<<16));

	System_GameLoop();
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

void System_InitModulePlatformSpecific()
{
}

void System_DeinitModulePlatformSpecific()
{
}

void System_UpdatePlatformSpecific()
{
}

void System_DrawPlatformSpecific()
{
}

uint64 RDTSC()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

uint64 GetTSCFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}
