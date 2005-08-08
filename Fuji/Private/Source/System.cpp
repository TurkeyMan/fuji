#include "Common.h"
#include "System_Internal.h"
#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFModel_Internal.h"
#include "Input_Internal.h"
#include "View_Internal.h"
#include "MFFileSystem_Internal.h"
#include "Font.h"
#include "Primitive.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "Font.h"
#include "Renderer_Internal.h"
#include "Sound.h"

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
#if !defined(_PSP)
		640,			// displayWidth
		480				// displayHeight
#else
		480,			// displayWidth
		272				// displayHeight
#endif
	},

	// ViewDefaults
	{
		16,				// maxViewsOnStack
		60.0f,			// defaultFOV;
		4.0f/3.0f,		// defaultAspect;
		0.1f,			// defaultNearPlane;
		1000.0f,		// defaultFarPlane;
		0.0f,			// orthoMinX
		0.0f,			// orthoMinY
		640.0f,			// orthoMaxX
		480.0f			// orthoMaxY
	},

	// TextureDefaults
	{
		256				// maxTextures
	},

	// MaterialDefaults
	{
		32,				// maxMaterialTypes
		32,				// maxMaterialDefs
		2048			// maxMaterials
	},

	// ModelDefaults
	{
		256				// maxModels
	},

	// FileSystemDefaults
	{
		128,			// maxOpenFiles
		16,				// maxFileSystems
		16,				// maxFileSystemStackSize
	},

	// SoundDefaults
	{
		4				// maxMusicTracks
	},

	// InputDefaults
	{
		true,			// allowMultipleMice
		true,			// mouseZeroIsSystemMouse
		true			// systemMouseUseWindowsCursor
	},

	// MiscellaneousDefaults
	{
		true			// enableUSBOnStartup
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

	CrcInit();

	Heap_InitModule();

	DebugMenu_InitModule();
	Callstack_InitModule();

	System_InitModulePlatformSpecific();

	Timer_InitModule();
	gSystemTimer.Init(NULL);
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();

	MFFileSystem_InitModule();

	View_InitModule();
	Display_InitModule();
	Input_InitModule();

	Sound_InitModule();

	Renderer_InitModule();
	MFTexture_InitModule();
	MFMaterial_InitModule();

	MFModel_InitModule();

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

	MFModel_DeinitModule();

	MFMaterial_DeinitModule();
	MFTexture_DeinitModule();
	Renderer_DeinitModule();

	Sound_DeinitModule();

	Input_DeinitModule();
	Display_DeinitModule();
	View_DeinitModule();

	MFFileSystem_DeinitModule();

	Timer_DeinitModule();

	System_DeinitModulePlatformSpecific();

	Callstack_DeinitModule();
	DebugMenu_DeinitModule();

	Heap_DeinitModule();
}

void System_Update()
{
	CALLSTACKc;

	System_UpdatePlatformSpecific();

	Input_Update();

#if defined(_XBOX)
	if(Input_Read(IDD_Gamepad, 0, Button_XB_Start) && Input_Read(IDD_Gamepad, 0, Button_XB_White) && Input_Read(IDD_Gamepad, 0, Button_XB_LTrig) && Input_Read(IDD_Gamepad, 0, Button_XB_RTrig))
		RestartCallback(NULL, NULL);
#elif defined(_PSP)
	if(Input_Read(IDD_Gamepad, 0, Button_DLeft) && Input_Read(IDD_Gamepad, 0, Button_PP_Circle) && Input_Read(IDD_Gamepad, 0, Button_PP_L) && Input_Read(IDD_Gamepad, 0, Button_PP_R))
		RestartCallback(NULL, NULL);
#else//if defined(_WINDOWS)
	if(Input_Read(IDD_Gamepad, 0, Button_P2_Start) && Input_Read(IDD_Gamepad, 0, Button_P2_Select) && Input_Read(IDD_Gamepad, 0, Button_P2_L1) && Input_Read(IDD_Gamepad, 0, Button_P2_R1) && Input_Read(IDD_Gamepad, 0, Button_P2_L2) && Input_Read(IDD_Gamepad, 0, Button_P2_R2))
		RestartCallback(NULL, NULL);
#endif

#if defined(_PSP)
	if(Input_Read(IDD_Gamepad, 0, Button_DLeft) && Input_Read(IDD_Gamepad, 0, Button_PP_L) && Input_WasPressed(IDD_Gamepad, 0, Button_PP_Start))
		gDrawSystemInfo = !gDrawSystemInfo;
#else
	if(Input_Read(IDD_Gamepad, 0, Button_P2_L1) && Input_Read(IDD_Gamepad, 0, Button_P2_L2) && Input_WasPressed(IDD_Gamepad, 0, Button_P2_LThumb))
		gDrawSystemInfo = !gDrawSystemInfo;
#endif

#if !defined(_RETAIL)
	DebugMenu_Update();
#endif

	MFMaterial_Update();

	Sound_Update();
}

void System_PostUpdate()
{
	CALLSTACK;

}

void System_Draw()
{
	CALLSTACKc;

#if !defined(_RETAIL)
	View_Push();
	View_SetDefault();

	MFRect rect;

	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.width = (float)gDefaults.display.displayWidth;
	rect.height = (float)gDefaults.display.displayHeight;

	View_SetOrtho(&rect);

	// should be the first thing rendered so we only display game vertices
	DrawMFPrimitiveStats();

	Sound_Draw();

	Callstack_DrawProfile();

	if(gDrawSystemInfo)
	{
		//FPS Display
#if defined(_PSP)
		float x = gDefaults.display.displayWidth-100.0f;
		float y = 10.0f;
#else
		float x = gDefaults.display.displayWidth-140.0f;
		float y = 30.0f;
#endif

		Font_DrawTextf(gpDebugFont, x, y, 0, 20.0f, Vector(1,1,0,1), "FPS: %.2f", GetFPS());

		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			Font_DrawTextf(gpDebugFont, 80.0f, gDefaults.display.displayHeight-50.0f, 0, 20.0f, Vector(1,0,0,1), "Rate: %s", STR(rate == 0.0f ? "Paused" : "%.2f", rate));

		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(Mat_SysLogoSmall));
		const float iconSize = 55.0f;

		float yOffset = gDefaults.display.displayHeight-70.0f;

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(1,1,1,0.5f);
		MFSetTexCoord1(0,0);
		MFSetPosition(15, yOffset, 0);
		MFSetTexCoord1(1,0);
		MFSetPosition(15+iconSize, yOffset, 0);
		MFSetTexCoord1(0,1);
		MFSetPosition(15, yOffset+iconSize, 0);
		MFSetTexCoord1(1,1);
		MFSetPosition(15+iconSize, yOffset+iconSize, 0);
		MFEnd();

		System_DrawPlatformSpecific();
	}

	DebugMenu_Draw();

	View_Pop();
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

