#include "Common.h"
#include "System.h"

int main(int argc, char **argv)
{
	// Might as well do this now instead of initialising each component individually in the modules
	if(SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Unable to initialise SDL\n");
		exit(1);
	}

	System_GameLoop();

	SDL_Quit();
}
