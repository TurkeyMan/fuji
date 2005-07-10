#include "Common.h"
#include "System.h"

int main()
{
//	kos_init_all(ALL_ENABLE, ROMDISK_NONE);
	kos_init_all(THD_ENABLE | IRQ_ENABLE | TA_ENABLE, romdisk);

	System_GameLoop();

//	kos_shutdown_all();

	return 0;
}

void _start()
{

}

uint64 RDTSC()
{
	return timer_ms_gettime64();
}

uint64 GetTSCFrequency()
{
	return 1000;
}

