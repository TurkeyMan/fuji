#include "Common.h"
#include "System.h"
#include "FileSystem.h"
#include "Display.h"
#include "View.h"
#include "Material.h"
#include "Input.h"
#include "Font.h"
#include "Primitive.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "Font.h"
#include "IniFile.h"

FujiDefaults gDefaults = 
{
	// HeapDefaults
	{
		4*1024*1024,	// dynamicHeapSize
		4*1024*1024,	// staticHeapSize
		0,				// dynamicHeapCount
		0,				// staticHeapCount
		256,			// maxResources
		2048			// maxStaticMarkers
	},

	// DisplayDefaults
	{
		640,			// otrhoWidth
		480				// otrhoHeight
	},

	// TextureDefaults
	{
		1024			// maxTextures
	},

	// MaterialDefaults
	{
		254				// maxMaterials
	}
};

bool gDrawSystemInfo = true;
int gQuit = 0;
int gRestart = 1;
uint32 gFrameCount = 0;
float gSystemTimeDelta;

MenuItemStatic quitOption;
MenuItemStatic restartOption;

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
	gSystemTimer.Init(NULL);
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();

	FileSystem_InitModule();

	Display_InitModule();
	Input_InitModule();
	Texture_InitModule();
	Material_InitModule();
	Primitive_InitModule();
	Font_InitModule();

	DebugMenu_AddItem("Restart", "Fuji Options", &restartOption, RestartCallback, NULL);
	DebugMenu_AddItem("Quit", "Fuji Options", &quitOption, QuitCallback, NULL);

	Heap_MarkHeap();
}

void System_Deinit()
{
	CALLSTACK;

	Heap_ReleaseMark();

	Font_DeinitModule();
	Primitive_DeinitModule();
	Material_DeinitModule();
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
	if(Input_ReadGamepad(0, Button_XB_Start) && Input_ReadGamepad(0, Button_XB_White) && Input_ReadGamepad(0, Button_XB_LTrig) && Input_ReadGamepad(0, Button_XB_RTrig))
		RestartCallback(NULL, NULL);
#else//if defined(_WINDOWS)
	if(Input_ReadGamepad(0, Button_P2_Start) && Input_ReadGamepad(0, Button_P2_Select) && Input_ReadGamepad(0, Button_P2_L1) && Input_ReadGamepad(0, Button_P2_R1) && Input_ReadGamepad(0, Button_P2_L2) && Input_ReadGamepad(0, Button_P2_R2))
		RestartCallback(NULL, NULL);
#endif

	if(Input_ReadGamepad(0, Button_P2_L1) && Input_ReadGamepad(0, Button_P2_L2) && Input_WasPressed(0, Button_P2_LThumb))
		gDrawSystemInfo = !gDrawSystemInfo;

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
	View::UseDefault();
	bool o = View::GetCurrent()->SetOrtho(true);

	Callstack_DrawProfile();

	if(gDrawSystemInfo)
	{
		//FPS Display
		debugFont.DrawTextf(500.0f, 30.0f, 0, 20.0f, 0xFFFFFF00, "FPS: %.2f", GetFPS());
		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			debugFont.DrawTextf(50.0f, 420.0f, 0, 20.0f, 0xFFFF0000, "Rate: %s", STR(rate == 0.0f ? "Paused" : "%.2f", rate));
	}

	DebugMenu_Draw();

	View::GetCurrent()->SetOrtho(o);
#endif
}

int System_GameLoop()
{
	CALLSTACK;

	// allow's game to set defaults and what not
	// before the system begins initialisation
	Game_InitSystem();

	// initialise the system and create displays etc..
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
	#elif defined(_LINUX)
			CheckEvents();
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
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();
}

