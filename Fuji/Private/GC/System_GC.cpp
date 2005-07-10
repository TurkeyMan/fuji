#include "Common.h"
#include "System.h"

int main()
{

	return 0;
}

uint64 RDTSC()
{
	return timer_ms_gettime64();
}

uint64 GetTSCFrequency()
{
	return 1000;
}

