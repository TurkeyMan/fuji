#include "Common.h"
#include "System.h"
#include "Display.h"
#include "Input.h"
#include "Primitive.h"
#include "DebugMenu.h"

int quit;

float TIMEDELTA = 0.0f;
float fps;

void System_Init()
{
	DebugMenu_InitModule();
	Primitive_InitModule();
	Input_InitModule();
}

void System_Deinit()
{
	Input_DeinitModule();
	Primitive_DeinitModule();
	DebugMenu_DeinitModule();
}

void System_Update()
{
	Input_Update();
	DebugMenu_Update();
}

void System_PostUpdate()
{

}

void System_Draw()
{
	DebugMenu_Draw();
}

int System_GameLoop()
{
	quit = 0;

	System_Init();
	Game_Init();

	while(!quit)
	{
		System_UpdateTimeDelta();

		System_Update();
		Game_Update();
		System_PostUpdate();

		Display_BeginFrame();

		Game_Draw();
		System_Draw();

		Display_EndFrame();
	}

	Game_Deinit();
	System_Deinit();

	return quit;
}

void System_UpdateTimeDelta()
{
	static uint64 frameStart = ReadPerformanceCounter();
	static int updatefps = 0;

	uint64 time = ReadPerformanceCounter();

	TIMEDELTA = (float)((double)(time-frameStart)/(double)GetPerfprmanceFrequency());

	if(!updatefps)
	{
		fps = 1.0f/TIMEDELTA;
	}

	updatefps++;
	updatefps%=20;
	frameStart = time;
}

float GetFPS()
{
	return (float)fps;
}

