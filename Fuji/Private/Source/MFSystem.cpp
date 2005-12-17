#include "Fuji.h"
#include "MFCallstack_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem_Internal.h"
#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFModel_Internal.h"
#include "MFInput_Internal.h"
#include "MFView_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive_Internal.h"
#include "DebugMenu.h"
#include "Timer.h"
#include "MFFont.h"
#include "MFRenderer_Internal.h"
#include "MFSound_Internal.h"
#include "MFSockets_Internal.h"
#include "MFNetwork_Internal.h"

// externs
void MFSystem_HandleEventsPlatformSpecific();

// extern to platform
extern MFPlatform gCurrentPlatform;

// local variables
MFDefaults gDefaults = 
{
	// HeapDefaults
	{
		4096,			// maxAllocations
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
		8,				// maxHTTPFiles
		512				// maxHTTPFileCache
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

void MFSystem_Init()
{
	MFCALLSTACK;

	CrcInit();

	MFHeap_InitModule();

	DebugMenu_InitModule();
	MFCallstack_InitModule();

	MFSystem_InitModulePlatformSpecific();

	Timer_InitModule();
	gSystemTimer.Init(NULL);
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();

	MFSockets_InitModule();

	MFFileSystem_InitModule();

	MFView_InitModule();
	MFDisplay_InitModule();
	MFInput_InitModule();

	MFSound_InitModule();

	MFRenderer_InitModule();
	MFTexture_InitModule();
	MFMaterial_InitModule();

	MFModel_InitModule();

	MFPrimitive_InitModule();
	MFFont_InitModule();

	MFNetwork_InitModule();

	DebugMenu_AddItem("Restart", "Fuji Options", &restartOption, RestartCallback, NULL);
	DebugMenu_AddItem("Quit", "Fuji Options", &quitOption, QuitCallback, NULL);

	MFHeap_Mark();
}

void MFSystem_Deinit()
{
	MFCALLSTACK;

	MFHeap_Release();

	MFNetwork_DeinitModule();

	MFFont_DeinitModule();
	MFPrimitive_DeinitModule();

	MFModel_DeinitModule();

	MFMaterial_DeinitModule();
	MFTexture_DeinitModule();
	MFRenderer_DeinitModule();

	MFSound_DeinitModule();

	MFInput_DeinitModule();
	MFDisplay_DeinitModule();
	MFView_DeinitModule();

	MFFileSystem_DeinitModule();

	MFSockets_DeinitModule();

	Timer_DeinitModule();

	MFSystem_DeinitModulePlatformSpecific();

	MFCallstack_DeinitModule();
	DebugMenu_DeinitModule();

	MFHeap_DeinitModule();
}

void MFSystem_Update()
{
	MFCALLSTACKc;

	MFSystem_UpdatePlatformSpecific();

	MFInput_Update();

#if defined(_XBOX)
	if(MFInput_Read(Button_XB_Start, IDD_Gamepad) && MFInput_Read(Button_XB_White, IDD_Gamepad) && MFInput_Read(Button_XB_LTrig, IDD_Gamepad) && MFInput_Read(Button_XB_RTrig, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#elif defined(_PSP)
	if(MFInput_Read(Button_DLeft, IDD_Gamepad) && MFInput_Read(Button_PP_Circle, IDD_Gamepad) && MFInput_Read(Button_PP_L, IDD_Gamepad) && MFInput_Read(Button_PP_R, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#else//if defined(_WINDOWS)
	if(MFInput_Read(Button_P2_Start, IDD_Gamepad) && MFInput_Read(Button_P2_Select, IDD_Gamepad) && MFInput_Read(Button_P2_L1, IDD_Gamepad) && MFInput_Read(Button_P2_R1, IDD_Gamepad) && MFInput_Read(Button_P2_L2, IDD_Gamepad) && MFInput_Read(Button_P2_R2, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#endif

#if defined(_PSP)
	if(MFInput_Read(Button_DLeft, IDD_Gamepad) && MFInput_Read(Button_PP_L, IDD_Gamepad) && MFInput_WasPressed(Button_PP_Start, IDD_Gamepad))
		gDrawSystemInfo = !gDrawSystemInfo;
#else
	if(MFInput_Read(Button_P2_L1, IDD_Gamepad) && MFInput_Read(Button_P2_L2, IDD_Gamepad) && MFInput_WasPressed(Button_P2_L3, IDD_Gamepad))
		gDrawSystemInfo = !gDrawSystemInfo;
#endif

#if !defined(_RETAIL)
	DebugMenu_Update();
#endif

	MFMaterial_Update();

	MFSound_Update();

	MFNetwork_Update();
}

void MFSystem_PostUpdate()
{
	MFCALLSTACK;

}

void MFSystem_Draw()
{
	MFCALLSTACKc;

#if !defined(_RETAIL)
	MFView_Push();
	MFView_SetDefault();

	MFRect rect;

	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.width = (float)gDefaults.display.displayWidth;
	rect.height = (float)gDefaults.display.displayHeight;

	MFView_SetOrtho(&rect);

	// should be the first thing rendered so we only display game vertices
	MFPrimitive_DrawStats();

	MFSound_Draw();

	MFCallstack_Draw();

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

		MFFont_DrawTextf(MFFont_GetDebugFont(), x, y, 20.0f, MakeVector(1,1,0,1), "FPS: %.2f", MFSystem_GetFPS());

		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			MFFont_DrawTextf(MFFont_GetDebugFont(), 80.0f, gDefaults.display.displayHeight-50.0f, 20.0f, MakeVector(1,0,0,1), "Rate: %s", MFStr(rate == 0.0f ? "Paused" : "%.2f", rate));

		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_SysLogoSmall));
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

		MFSystem_DrawPlatformSpecific();
	}

	DebugMenu_Draw();

	MFView_Pop();
#endif
}

int MFSystem_GameLoop()
{
	MFCALLSTACK;

	// allow's game to set defaults and what not
	// before the system begins initialisation
	Game_InitSystem();

	// initialise the system and create displays etc..
	MFSystem_Init();

	while(gRestart)
	{
		gRestart = 0;
		gQuit = 0;

		Game_Init();

		while(!gQuit)
		{
			MFSystem_HandleEventsPlatformSpecific();

			MFCallstack_BeginFrame();
			MFSystem_UpdateTimeDelta();
			gFrameCount++;

			MFSystem_Update();
			if(!DebugMenu_IsEnabled())
				Game_Update();
			MFSystem_PostUpdate();

			MFDisplay_BeginFrame();

			Game_Draw();
			MFSystem_Draw();

			MFCallstack_EndFrame();
			MFDisplay_EndFrame();
		}

		Game_Deinit();
	}

	MFSystem_Deinit();

	return gQuit;
}

void MFSystem_UpdateTimeDelta()
{
	MFCALLSTACK;

	gSystemTimer.Update();
	gSystemTimeDelta = gSystemTimer.TimeDeltaF();
}

float MFSystem_GetFPS()
{
	return gSystemTimer.GetFPS();
}

MFPlatform MFSystem_GetCurrentPlatform()
{
	return gCurrentPlatform;
}
