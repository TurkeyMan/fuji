#include "Common.h"
#include "System.h"

void main()
{
	SYSTEMTIME s;
	GetSystemTime(&s);
	srand(s.wMilliseconds|(s.wSecond<<16));

	System_GameLoop();
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

void FixXBoxFilename(const char *pFilename)
{
	if(!pFilename) return;

	int len = strlen(pFilename);

	char *pName = const_cast<char*>(pFilename);

	for(int a=0; a<len; a++)
	{
		if(pName[a] == '/') pName[a] = '\\';
	}
}
