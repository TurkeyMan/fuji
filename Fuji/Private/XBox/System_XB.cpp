#include "Common.h"
#include "System.h"

void main()
{
	System_GameLoop();
}

uint64 ReadPerformanceCounter()
{
	uint64 tickCount;
	QueryPerformanceCounter((LARGE_INTEGER*)&tickCount);
	return tickCount;
}

uint64 GetPerfprmanceFrequency()
{
	uint64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return freq;
}

