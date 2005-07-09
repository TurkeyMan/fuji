#include "Common.h"
#include "System_Internal.h"

int main(int argc, char *argv[])
{
//	srand(GetTickCount());

	System_GameLoop();
}

uint64 RDTSC()
{
	return 0;
}

uint64 GetTSCFrequency()
{
	return 0;
}
