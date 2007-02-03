#include "Fuji.h"
#include "MFCallstack_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem_Internal.h"
#include "Display_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFModel_Internal.h"
#include "MFAnimation_Internal.h"
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
#include "MFScript_Internal.h"
#include "MFCollision_Internal.h"
#include "MFAnimScript_Internal.h"

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

	// AnimationDefaults
	{
		256				// maxAnimations
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

MFSystemCallbackFunction pFilesystemInit = NULL;
MFSystemCallbackFunction pInit = NULL;
MFSystemCallbackFunction pDeinit = NULL;
MFSystemCallbackFunction pUpdate = NULL;
MFSystemCallbackFunction pDraw = NULL;
MFSystemCallbackFunction pHandleSysMessages = NULL;
MFSystemCallbackFunction pDisplayLost = NULL;
MFSystemCallbackFunction pDisplayReset = NULL;
MFSystemCallbackFunction pVerticalBlank = NULL;

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

	MFUtil_CrcInit();

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
	MFAnimation_InitModule();
	MFAnimScript_InitModule();

	MFPrimitive_InitModule();
	MFFont_InitModule();

	MFCollision_InitModule();

	MFNetwork_InitModule();

#if defined(_ENABLE_SCRIPTING)
	MFScript_InitModule();
#endif

	DebugMenu_AddItem("Restart", "Fuji Options", &restartOption, RestartCallback, NULL);
	DebugMenu_AddItem("Quit", "Fuji Options", &quitOption, QuitCallback, NULL);

	MFHeap_Mark();
}

void MFSystem_Deinit()
{
	MFCALLSTACK;

	MFHeap_Release();

#if defined(_ENABLE_SCRIPTING)
	MFScript_DeinitModule();
#endif

	MFNetwork_DeinitModule();

	MFCollision_DeinitModule();

	MFFont_DeinitModule();
	MFPrimitive_DeinitModule();

	MFAnimScript_DeinitModule();
	MFAnimation_DeinitModule();
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

#if defined(_MFXBOX)
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
	MFRect display;

	MFDisplay_GetDisplayRect(&display);

	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.height = display.height;
	rect.width = rect.height * MFDisplay_GetNativeAspectRatio();

	MFView_SetOrtho(&rect);

	// should be the first thing rendered so we only display game vertices
	MFPrimitive_DrawStats();

	MFSound_Draw();

	MFCallstack_Draw();

	if(gDrawSystemInfo)
	{
		//FPS Display
#if defined(_PSP)
		float x = display.width-100.0f;
		float y = 10.0f;

		MFFont_DrawTextf(MFFont_GetDebugFont(), x, y, 20.0f, MakeVector(1,1,0,1), "FPS: %.2f", MFSystem_GetFPS());

		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			MFFont_DrawTextf(MFFont_GetDebugFont(), 80.0f, display.height-50.0f, 20.0f, MakeVector(1,0,0,1), "Rate: %s", MFStr(rate == 0.0f ? "Paused" : "%.2f", rate));

		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_SysLogoSmall));

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(1,1,1,0.5f);

		const float iconSize = 20.0f;

		float yOffset = rect.height-25.0f;
		yOffset -= iconSize;

		MFSetTexCoord1(0,0.3f);
		MFSetPosition(15, yOffset, 0);
		MFSetTexCoord1(1,0.3f);
		MFSetPosition(15+iconSize*3, yOffset, 0);
		MFSetTexCoord1(0,0.65f);
		MFSetPosition(15, yOffset+iconSize, 0);
		MFSetTexCoord1(1,0.65f);
		MFSetPosition(15+iconSize*3, yOffset+iconSize, 0);

		MFEnd();
#else
		float x = rect.width-140.0f;
		float y = 30.0f;

		float xaspect = 25 * MFDisplay_GetNativeAspectRatio();

		MFFont_DrawTextf(MFFont_GetDebugFont(), x, y, 20.0f, MakeVector(1,1,0,1), "FPS: %.2f", MFSystem_GetFPS());

		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			MFFont_DrawTextf(MFFont_GetDebugFont(), xaspect + 60, rect.height-50.0f, 20.0f, MakeVector(1,0,0,1), "Rate: %s", MFStr(rate == 0.0f ? "Paused" : "%.2f", rate));

		MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_SysLogoSmall));

		MFPrimitive(PT_TriStrip);
		MFBegin(4);
		MFSetColour(1,1,1,0.5f);

		const float iconSize = 55.0f;
		float yOffset = rect.height-30.0f;
		yOffset -= iconSize;

		MFSetTexCoord1(0,0);
		MFSetPosition(xaspect, yOffset, 0);
		MFSetTexCoord1(1,0);
		MFSetPosition(xaspect+iconSize, yOffset, 0);
		MFSetTexCoord1(0,1);
		MFSetPosition(xaspect, yOffset+iconSize, 0);
		MFSetTexCoord1(1,1);
		MFSetPosition(xaspect+iconSize, yOffset+iconSize, 0);

		MFEnd();
#endif

		MFSystem_DrawPlatformSpecific();
	}

	rect.height = 480;
	rect.width = rect.height * MFDisplay_GetNativeAspectRatio();
	MFView_SetOrtho(&rect);

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

		if(pInit) pInit();
		Game_Init();

		// init the timedelta
		MFSystem_UpdateTimeDelta();

		while(!gQuit)
		{
			MFSystem_HandleEventsPlatformSpecific();

			MFCallstack_BeginFrame();
			MFSystem_UpdateTimeDelta();
			gFrameCount++;

			MFSystem_Update();
			if(!DebugMenu_IsEnabled())
			{
				if(pUpdate) pUpdate();
				Game_Update();
			}
			MFSystem_PostUpdate();

			MFDisplay_BeginFrame();

			if(pDraw) pDraw();
			Game_Draw();
			MFSystem_Draw();

			MFCallstack_EndFrame();
			MFDisplay_EndFrame();
		}

		if(pDeinit) pDeinit();
		Game_Deinit();
	}

	MFSystem_Deinit();

	return gQuit;
}

MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction)
{
	MFSystemCallbackFunction pOldCallback = NULL;

	switch(callback)
	{
		case MFCB_FileSystemInit:
			pOldCallback = pFilesystemInit;
			pFilesystemInit = pCallbackFunction;
			break;
		case MFCB_PostInit:
			pOldCallback = pInit;
			pInit = pCallbackFunction;
			break;
		case MFCB_Deinit:
			pOldCallback = pDeinit;
			pDeinit = pCallbackFunction;
			break;
		case MFCB_Update:
			pOldCallback = pUpdate;
			pUpdate = pCallbackFunction;
			break;
		case MFCB_Draw:
			pOldCallback = pDraw;
			pDraw = pCallbackFunction;
			break;
		case MFCB_HandleSystemMessages:
			pOldCallback = pHandleSysMessages;
			pHandleSysMessages = pCallbackFunction;
			break;
		case MFCB_DisplayLost:
			pOldCallback = pDisplayLost;
			pDisplayLost = pCallbackFunction;
			break;
		case MFCB_DisplayReset:
			pOldCallback = pDisplayReset;
			pDisplayReset = pCallbackFunction;
			break;
		case MFCB_VerticalBlank:
			pOldCallback = pVerticalBlank;
			pVerticalBlank = pCallbackFunction;
			break;
		default:
			MFDebug_Assert(false, "Unknown system callback.");
	}

	return pOldCallback;
}

MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback)
{
	switch(callback)
	{
		case MFCB_FileSystemInit:
			return pFilesystemInit;
		case MFCB_PostInit:
			return pInit;
		case MFCB_Deinit:
			return pDeinit;
		case MFCB_Update:
			return pUpdate;
		case MFCB_Draw:
			return pDraw;
		case MFCB_HandleSystemMessages:
			return pHandleSysMessages;
		case MFCB_DisplayLost:
			return pDisplayLost;
		case MFCB_DisplayReset:
			return pDisplayReset;
		case MFCB_VerticalBlank:
			return pVerticalBlank;
		default:
			MFDebug_Assert(false, "Unknown system callback.");
	}

	return NULL;
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
