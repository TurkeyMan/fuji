#include "Common.h"
#include "System.h"
#include "FileSystem.h"
#include "Display.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "Font.h"
#include "IniFile.h"

int gQuit = 0;
int gRestart = 1;
uint32 gFrameCount = 0;

MenuItemStatic quitOption;
MenuItemStatic restartOption;

Timer gSystemTimer;

void QuitCallback(MenuObject *pMenu, void *pData)
{
	gQuit = 1;
}

void RestartCallback(MenuObject *pMenu, void *pData)
{
	gQuit = 1;
	gRestart = 1;
}

void System_Init()
{
	CALLSTACK;

	Heap_InitModule();

	DebugMenu_InitModule();
	Callstack_InitModule();
	Timer_InitModule();
	gSystemTimer.Init();

	FileSystem_InitModule();

	Display_InitModule();
	Input_InitModule();
	Texture_InitModule();
	Primitive_InitModule();
	Font_InitModule();
	
	DebugMenu_AddItem("Restart", "Fuji Options", &restartOption, RestartCallback, NULL);
	DebugMenu_AddItem("Quit", "Fuji Options", &quitOption, QuitCallback, NULL);
}

void System_Deinit()
{
	CALLSTACK;

	Font_DeinitModule();
	Primitive_DeinitModule();
	Texture_DeinitModule();
	Input_DeinitModule();
	Display_DeinitModule();

	FileSystem_DeinitModule();

	Timer_DeinitModule();
	Callstack_DeinitModule();
	DebugMenu_DeinitModule();

	Heap_DeinitModule();
}

void System_Update()
{
	CALLSTACKc;

	Input_Update();

#if defined(_XBOX)
	if(Input_ReadGamepad(0, Button_Start) && Input_ReadGamepad(0, Button_White) && Input_ReadGamepad(0, Button_LTrig) && Input_ReadGamepad(0, Button_RTrig))
		RestartCallback(NULL, NULL);
#elif defined(_WINDOWS)
	if(Input_ReadGamepad(0, Button_Start) && Input_ReadGamepad(0, Button_Select) && Input_ReadGamepad(0, Button_L1) && Input_ReadGamepad(0, Button_R1) && Input_ReadGamepad(0, Button_L2) && Input_ReadGamepad(0, Button_R2))
		RestartCallback(NULL, NULL);
#endif

#if !defined(_RETAIL)
	DebugMenu_Update();
#endif
}

void System_PostUpdate()
{
	CALLSTACK;

}

void System_Draw()
{
	CALLSTACKc;

#if !defined(_RETAIL)
	bool o = SetOrtho(true);

	Callstack_DrawProfile();

	//FPS Display
	debugFont.DrawTextf(500.0f, 30.0f, 0, 20.0f, 0xFFFFFF00, "FPS: %.2f", GetFPS());
	float rate = (float)gSystemTimer.GetRate();
	if(rate != 1.0f)
		debugFont.DrawTextf(50.0f, 420.0f, 0, 20.0f, 0xFFFF0000, "Rate: %s", STR(rate == 0.0f ? "Paused" : "%.2f", rate));

	DebugMenu_Draw();

	SetOrtho(o);
#endif
}

int System_GameLoop()
{
	CALLSTACK;

	System_Init();

	while(gRestart)
	{
		gRestart = 0;
		gQuit = 0;

		Game_Init();

		while(!gQuit)
		{
	#if defined(_WINDOWS)
			DoMessageLoop();
	#endif
			Callstack_BeginFrame();
			System_UpdateTimeDelta();
			gFrameCount++;

			System_Update();
			if(!DebugMenu_IsEnabled())
				Game_Update();
			System_PostUpdate();

			Display_BeginFrame();

			Game_Draw();
			System_Draw();

			Callstack_EndFrame();
			Display_EndFrame();
		}

		Game_Deinit();
	}

	System_Deinit();

	return gQuit;
}

void System_UpdateTimeDelta()
{
	CALLSTACK;

	gSystemTimer.Update();
}

float GetFPS()
{
	CALLSTACK;

	return gSystemTimer.GetFPS();
}

