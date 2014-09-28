#include "Fuji_Internal.h"
#include "MFCallstack_Internal.h"
#include "MFHeap_Internal.h"
#include "MFSystem_Internal.h"
#include "MFDisplay_Internal.h"
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
#include "MFMidi_Internal.h"
#include "MFSockets_Internal.h"
#include "MFNetwork_Internal.h"
#include "MFScript_Internal.h"
#include "MFCollision_Internal.h"
#include "MFAnimScript_Internal.h"
#include "MFVertex_Internal.h"
#include "MFThread_Internal.h"
#include "MFCompute_Internal.h"

#if defined(MF_WINDOWS)
	// NOTE: REMOVE ME!!!
	#include <windows.h>
#endif

// externs
void MFSystem_HandleEventsPlatformSpecific();

// local variables
extern "C" {
MFDefaults gDefaults =
{
	// HeapDefaults
	{
		4096,			// maxAllocations
		2048			// maxStaticMarkers
	},

	// SystemDefaults
	{
		MFPriority_Normal,	// threadPriority
		16,					// maxThreads
		16					// maxTlsSlots
	},

	// DisplayDefaults
	{
		"Fuji Window",	// pWindowTitle
		NULL,			// pIcon
		false			// hideMouseCursor
	},

	// RenderDefaults
	{
		256*1024		// renderHeapSize
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
		16,				// maxOpenFiles
		16,				// maxFinds
		16,				// maxFileSystems
		16,				// maxFileSystemStackSize
		8,				// maxHTTPFiles
		512				// maxHTTPFileCache
	},

	// SoundDefaults
	{
		256,			// maxSounds
		32,				// maxVoices
		16,				// maxMusicTracks
		true			// useGlobalFocus
	},

	// InputDefaults
	{
		true,			// allowMultipleMice
		true,			// mouseZeroIsSystemMouse
		true,			// systemMouseUseWindowsCursor
		true,			// useDirectInputKeyboard
		true			// useXInput
	},

	// MidiDefaults
	{
		false,			// useMidi
	},

	// MiscellaneousDefaults
	{
		true			// enableUSBOnStartup
	},

	// PluginSelectionDefaults
	{
		0,	// renderPlugin
		0,	// soundPlugin
		0	// inputPlugin
	}
};
}

struct MFSystemState
{
#if !defined(_FUJI_UTIL)
	MenuItemStatic quitOption;
	MenuItemStatic restartOption;
#endif
};

static int gModuleId = -1;

#if !defined(_FUJI_UTIL)

void QuitCallback(MenuObject *pMenu, void *pData)
{
	gpEngineInstance->bQuit = 1;
}

void RestartCallback(MenuObject *pMenu, void *pData)
{
	gpEngineInstance->bQuit = 1;
	gpEngineInstance->bRestart = 1;
}

MF_API void MFSystem_Quit()
{
	gpEngineInstance->bQuit = 1;
}

MFInitStatus MFSystem_InitModule(int moduleId, bool bPerformInitialisation)
{
	gModuleId = moduleId;

	if(!bPerformInitialisation)
		return MFIS_Succeeded;

	ALLOC_MODULE_DATA(MFSystemState);

	void MFFileSystem_RegisterDefaultArchives();
	gpEngineInstance->pSystemCallbacks[MFCB_FileSystemInit] = MFFileSystem_RegisterDefaultArchives;

	gpEngineInstance->bDrawSystemInfo = true;
	gpEngineInstance->bQuit = 0;
	gpEngineInstance->bRestart = 0;
	gpEngineInstance->frameCount = 0;

	MFSystem_InitModulePlatformSpecific();

	DebugMenu_AddItem("Restart", "Fuji Options", &pModuleData->restartOption, RestartCallback, NULL);
	DebugMenu_AddItem("Quit", "Fuji Options", &pModuleData->quitOption, QuitCallback, NULL);

	return MFIS_Succeeded;
}

void MFSystem_DeinitModule()
{
	MFSystem_DeinitModulePlatformSpecific();
}

void MFSystem_Update()
{
	MFCALLSTACKc;

	MFSystem_UpdatePlatformSpecific();

	MFInput_Update();

#if defined(MF_XBOX)
	if(MFInput_Read(Button_XB_Start, IDD_Gamepad) && MFInput_Read(Button_XB_White, IDD_Gamepad) && MFInput_Read(Button_XB_LTrig, IDD_Gamepad) && MFInput_Read(Button_XB_RTrig, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#elif defined(_PSP)
	if(MFInput_Read(Button_DLeft, IDD_Gamepad) && MFInput_Read(Button_PP_Circle, IDD_Gamepad) && MFInput_Read(Button_PP_L, IDD_Gamepad) && MFInput_Read(Button_PP_R, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#else//if defined(MF_WINDOWS)
	if(MFInput_Read(Button_P2_Start, IDD_Gamepad) && MFInput_Read(Button_P2_Select, IDD_Gamepad) && MFInput_Read(Button_P2_L1, IDD_Gamepad) && MFInput_Read(Button_P2_R1, IDD_Gamepad) && MFInput_Read(Button_P2_L2, IDD_Gamepad) && MFInput_Read(Button_P2_R2, IDD_Gamepad))
		RestartCallback(NULL, NULL);
#endif

#if defined(_PSP)
	if(MFInput_Read(Button_DLeft, IDD_Gamepad) && MFInput_Read(Button_PP_L, IDD_Gamepad) && MFInput_WasPressed(Button_PP_Start, IDD_Gamepad))
		gpEngineInstance->bDrawSystemInfo = !gpEngineInstance->bDrawSystemInfo;
#else
	if(MFInput_Read(Button_P2_L1, IDD_Gamepad) && MFInput_Read(Button_P2_L2, IDD_Gamepad) && MFInput_WasPressed(Button_P2_L3, IDD_Gamepad))
		gpEngineInstance->bDrawSystemInfo = !gpEngineInstance->bDrawSystemInfo;
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
	MFRenderer *pRenderer = MFRenderer_GetCurrent();

	MFRenderLayerSet set;
	MFZeroMemory(&set, sizeof(set));
	set.pSolidLayer = MFRenderer_GetDebugLayer(pRenderer);
	MFRenderer_SetRenderLayerSet(pRenderer, &set);

	MFView_SetDefault();

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);
	rect.width = rect.height * MFDisplay_GetAspectRatio();

	MFView_SetOrtho(&rect);

	// should be the first thing rendered so we only display game vertices
	MFPrimitive_DrawStats();

	MFSound_Draw();

	MFCallstack_Draw();

	if(gpEngineInstance->bDrawSystemInfo)
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

		float xaspect = 25 * MFDisplay_GetAspectRatio();

		MFFont_DrawText2f(MFFont_GetDebugFont(), x, y, 20.0f, MakeVector(1,1,0,1), "FPS: %.2f", MFSystem_GetFPS());

		float rate = (float)gSystemTimer.GetRate();
		if(rate != 1.0f)
			MFFont_DrawText2f(MFFont_GetDebugFont(), xaspect + 60, rect.height-50.0f, 20.0f, MakeVector(1,0,0,1), rate == 0.0f ? "Rate: Paused" : MFStr("Rate: %.2f", rate));

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
	rect.width = rect.height * MFDisplay_GetAspectRatio();
	MFView_SetOrtho(&rect);

	DebugMenu_Draw();
#endif
}

int MFSystem_GameLoop()
{
	MFCALLSTACK;

	// initialise the system and create displays etc..
	MFModule_RegisterEngineModules();

	while(gpEngineInstance->bRestart)
	{
		gpEngineInstance->bRestart = 0;
		gpEngineInstance->bQuit = 0;

		while(!gpEngineInstance->bQuit)
			MFSystem_RunFrame();

		if(gpEngineInstance->pSystemCallbacks[MFCB_Deinit])
			gpEngineInstance->pSystemCallbacks[MFCB_Deinit]();

		// deinit modules
		MFModule_DeinitModules();
	}

	return 0;
}

void MFSystem_RunFrame()
{
	MFCALLSTACKc;

	// allow fuji to complete initialisation
	if(!gpEngineInstance->bIsInitialised)
	{
		if(!MFModule_InitModules())
			return;
	}

	// run 1 frame
	MFCallstack_BeginFrame();

	if(gpEngineInstance->pSystemCallbacks[MFCB_HandleSystemMessages])
		gpEngineInstance->pSystemCallbacks[MFCB_HandleSystemMessages]();
	else
		MFSystem_HandleEventsPlatformSpecific();

	MFSystem_UpdateTimeDelta();
	gpEngineInstance->frameCount++;

	MFSystem_Update();
	if(!DebugMenu_IsEnabled())
	{
		MFCALLSTACKcs("MFSystem_RunFrame : Update");

		if(gpEngineInstance->pSystemCallbacks[MFCB_Update])
			gpEngineInstance->pSystemCallbacks[MFCB_Update]();
	}
	MFSystem_PostUpdate();

	if(MFRenderer_BeginFrame())
	{
		MFCALLSTACKcs("MFSystem_RunFrame : Render");

		MFRenderer *pRenderer = MFRenderer_GetCurrent();

		MFView_SetDefault();
		if(gpEngineInstance->pSystemCallbacks[MFCB_Draw])
			gpEngineInstance->pSystemCallbacks[MFCB_Draw]();
		MFSystem_Draw();

		// build and kick the GPU command buffers
		MFRenderer_BuildCommandBuffers(pRenderer);
		MFRenderer_Kick(pRenderer);
	}

	MFRenderer_EndFrame();
	MFCallstack_EndFrame();
}

void MFSystem_UpdateTimeDelta()
{
	MFCALLSTACK;

#if defined(MF_WINDOWS)
	static uint64 clock = MFSystem_ReadRTC();
	static uint64 freq = MFSystem_GetRTCFrequency();
	uint64 curClock = MFSystem_ReadRTC();

	while(curClock - clock < freq/85)
	{
		Sleep(1);
		curClock = MFSystem_ReadRTC();
	}
	clock = curClock;
#endif

	gSystemTimer.Update();
	gpEngineInstance->timeDelta = gSystemTimer.TimeDeltaF();
}

MF_API float MFSystem_GetFPS()
{
	return gSystemTimer.GetFPS();
}
#endif // !defined(_FUJI_UTIL)

MF_API MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction)
{
	MFDebug_Assert(callback >= 0 && callback < MFCB_Max, "Unknown system callback.");

	MFSystemCallbackFunction pOldCallback = gpEngineInstance->pSystemCallbacks[callback];
	gpEngineInstance->pSystemCallbacks[callback] = pCallbackFunction;
	return pOldCallback;
}

MF_API MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback)
{
	MFDebug_Assert(callback >= 0 && callback < MFCB_Max, "Unknown system callback.");

	return gpEngineInstance->pSystemCallbacks[callback];
}

MF_API MFDefaults* MFSystem_GetDefaults()
{
	return &gDefaults;
}

MF_API const char * MFSystem_GetSettingString(int tabDepth)
{
	const char *pSettings = "";

//	pSettings = MFStr("\ttest 0\r\n", tabDepth, "");

	return pSettings;
}

MF_API void MFSystem_InitFromSettings(const MFIniLine *pSettings)
{

}

MF_API MFPlatform MFSystem_GetCurrentPlatform()
{
	return gpEngineInstance->currentPlatform;
}

MF_API float MFSystem_GetTimeDelta()
{
	return gpEngineInstance->timeDelta;
}

MF_API uint32 MFSystem_GetFrameCounter()
{
	return gpEngineInstance->frameCount;
}
