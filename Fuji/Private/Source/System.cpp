#include "Common.h"
#include "System.h"
#include "Display.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "Font.h"

int quit;
uint32 gFrameCount = 0;

MenuItemStatic quitOption;
MenuItemFloat mf(0, 10);
MenuItemInt mi;
MenuItemBool mb;
MenuItemColour col;

char *strings[] = { "Hello", "World", "Tang!", NULL };
MenuItemIntString mis(strings);

Timer gSystemTimer;

void QuitCallback(MenuObject *pMenu, void *pData)
{
	quit = 1;
}

void System_Init()
{
	CALLSTACK("System_Init");

	DebugMenu_InitModule();

	DebugMenu_AddMenu("Fuji Options", &rootMenu);

	Callstack_InitModule();
	Timer_InitModule();

	gSystemTimer.Init();

	Display_InitModule();
	Input_InitModule();
	Texture_InitModule();
	Primitive_InitModule();
	Font_InitModule();

	DebugMenu_AddItem("Test Float", "Fuji Options", &mf);
	DebugMenu_AddItem("Test Int", "Fuji Options", &mi);
	DebugMenu_AddItem("Test Bool", "Fuji Options", &mb);
	DebugMenu_AddItem("Test IntString", "Fuji Options", &mis);
	DebugMenu_AddItem("Test Colour", "Fuji Options", &col);
	DebugMenu_AddItem("Quit", "Fuji Options", &quitOption, QuitCallback, NULL);
}

void System_Deinit()
{
	CALLSTACK("System_Deinit");

	Font_DeinitModule();
	Primitive_DeinitModule();
	Texture_DeinitModule();
	Input_DeinitModule();
	Display_DeinitModule();
	Timer_DeinitModule();
	Callstack_DeinitModule();
	DebugMenu_DeinitModule();
}

void System_Update()
{
	CALLSTACKc("System_Update");

	Input_Update();
	DebugMenu_Update();
}

void System_PostUpdate()
{
	CALLSTACK("System_PostUpdate");

}

void System_Draw()
{
	CALLSTACKc("System_Draw");

	bool o = SetOrtho(true);

	Callstack_DrawProfile();

	//FPS Display
	debugFont.DrawTextf(500.0f, 30.0f, 0, 20.0f, 0xFFFFFF00, "FPS: %.2f", GetFPS());
	float rate = (float)gSystemTimer.GetRate();
	if(rate != 1.0f)
		debugFont.DrawTextf(50.0f, 420.0f, 0, 20.0f, 0xFFFF0000, "Rate: %.2f", rate);

	DebugMenu_Draw();

	SetOrtho(o);
}

int System_GameLoop()
{
	CALLSTACK("System_GameLoop");
	quit = 0;

	System_Init();
	Game_Init();

	while(!quit)
	{
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
	System_Deinit();

	return quit;
}

void System_UpdateTimeDelta()
{
	CALLSTACK("System_UpdateTimeDelta");

	gSystemTimer.Update();
}

float GetFPS()
{
	CALLSTACK("GetFPS");

	return gSystemTimer.GetFPS();
}

