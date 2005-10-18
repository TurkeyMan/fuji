#include "Fuji.h"
#include "System_Internal.h"

int main()
{
	kos_init_all(ALL_ENABLE, ROMDISK_NONE);
//	kos_init_all(THD_ENABLE | IRQ_ENABLE | TA_ENABLE, ROMDISK_NONE);

	System_GameLoop();

	kos_shutdown_all();

	return 0;
}

uint64 RDTSC()
{
	return 0;//timer_ms_gettime64();
}

uint64 GetTSCFrequency()
{
	return 1000;
}

