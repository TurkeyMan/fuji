#include "Common.h"
#include "System.h"

int main(int argc, char **argv)
{
	// Might as well do this now instead of initialising each component individually in the modules
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) == -1) {
		fprintf(stderr, "Unable to initialise SDL\n");
		exit(1);
	}

	System_GameLoop();

	SDL_Quit();
}


// Spy uses these, but I guess it should really be using the timer class. So this is temporary.

uint64 RDTSC()
{
	static Uint32 lastTime = 0;
	static uint64 bigCounter = 0;
	Uint32 thisTime;

	thisTime = SDL_GetTicks();


	Uint32 difference;

	if(thisTime < lastTime) {
		difference = (0xFFFFFFFF - lastTime) + (thisTime + 1);
	} else {
		difference = thisTime - lastTime;
	}

	lastTime = thisTime;
	bigCounter += difference;
	
	return(bigCounter);
}

uint64 GetTSCFrequency()
{
	return(1000);
}

