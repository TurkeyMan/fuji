#include "Fuji/Fuji.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFRenderState.h"
#include "Fuji/MFView.h"
#include "Fuji/MFFont.h"
#include "Fuji/MFInput.h"
#include "Fuji/Timer.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

/**** Functions ****/

void Game_InitFilesystem(void *pUserData)
{
	// mount the sample assets directory
	MFFileSystemHandle hNative = MFFileSystem_GetInternalFileSystemHandle(MFFSH_NativeFileSystem);
	MFMountDataNative mountData;
	mountData.cbSize = sizeof(MFMountDataNative);
	mountData.priority = MFMP_Normal;
	mountData.flags = MFMF_FlattenDirectoryStructure | MFMF_Recursive;
	mountData.pMountpoint = "game";
#if defined(MF_IPHONE)
	mountData.pPath = MFFile_SystemPath();
#else
	mountData.pPath = MFFile_SystemPath("../Sample_Data/");
#endif
	MFFileSystem_Mount(hNative, &mountData);

	if(pInitFujiFS)
		pInitFujiFS(pUserData);
}

void Game_Init(void *pUserData)
{
	// create the renderer with a single layer that clears before rendering
	MFRenderLayerDescription layers[] = { { "Scene" } };
	pRenderer = MFRenderer_Create(layers, 1, NULL, NULL);
	MFRenderer_SetCurrent(pRenderer);

	pDefaultStates = MFStateBlock_CreateDefault();
	MFRenderer_SetGlobalStateBlock(pRenderer, pDefaultStates);

	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, 0);
	MFRenderLayer_SetClear(pLayer, MFRCF_All, MakeVector(0.f, 0.f, 0.2f, 1.f));

	MFRenderLayerSet layerSet;
	MFZeroMemory(&layerSet, sizeof(layerSet));
	layerSet.pSolidLayer = pLayer;
	MFRenderer_SetRenderLayerSet(pRenderer, &layerSet);
}

#define CHK_BUTTON(x) \
     if (MFInput_Read(x, IDD_Gamepad)!=0.0f)  MFDebug_Message(#x);

void Game_Update(void *pUserData)
{
	MFDebug_Message(MFStr("Time is %f- %f FPS\t", gSystemTimer.GetSecondsF(), gSystemTimer.GetFPS()));
	MFDebug_Message(MFStr("Left (%1.4f, %1.4f) ", MFInput_Read(Axis_LX, IDD_Gamepad), MFInput_Read(Axis_LY, IDD_Gamepad)));
	MFDebug_Message(MFStr("Right (%1.4f, %1.4f) ", MFInput_Read(Axis_RX, IDD_Gamepad), MFInput_Read(Axis_RY, IDD_Gamepad)));

	CHK_BUTTON(Button_P2_Cross);
	CHK_BUTTON(Button_P2_Circle);
	CHK_BUTTON(Button_P2_Box);
	CHK_BUTTON(Button_P2_Triangle);

	CHK_BUTTON(Button_P2_L1);
	CHK_BUTTON(Button_P2_R1);
	CHK_BUTTON(Button_P2_L2);
	CHK_BUTTON(Button_P2_R2);

	CHK_BUTTON(Button_P2_Start);
	CHK_BUTTON(Button_P2_Select);

	CHK_BUTTON(Button_P2_L3);
	CHK_BUTTON(Button_P2_R3);

	CHK_BUTTON(Button_DUp);
	CHK_BUTTON(Button_DDown);
	CHK_BUTTON(Button_DLeft);
	CHK_BUTTON(Button_DRight);
}

void Game_Draw(void *pUserData)
{
	// set orthographic projection
	MFView_SetOrtho();

	// render some text
	MFFont_DrawText2(MFFont_GetDebugFont(), 200.f, 200.f, 50.f, MFVector::one, "Hello World!");
}

void Game_Deinit(void *pUserData)
{
	MFRenderer_Destroy(pRenderer);
}


int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)(MFSystem_ReadRTC() & 0xFFFFFFFF));

	Fuji_CreateEngineInstance();

	MFSystem_RegisterSystemCallback(MFCB_InitDone, Game_Init);
	MFSystem_RegisterSystemCallback(MFCB_Update, Game_Update);
	MFSystem_RegisterSystemCallback(MFCB_Draw, Game_Draw);
	MFSystem_RegisterSystemCallback(MFCB_Deinit, Game_Deinit);

	pInitFujiFS = MFSystem_RegisterSystemCallback(MFCB_FileSystemInit, Game_InitFilesystem);

	int r = MFMain(pInitParams);

	Fuji_DestroyEngineInstance();

	return r;
}

#if defined(MF_WINDOWS)
#include <windows.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	MFInitParams initParams;
	initParams.hInstance = hInstance;
	initParams.pCommandLine = lpCmdLine;

	return GameMain(&initParams);
}

#elif defined(MF_PSP)
#include <pspkernel.h>

int main(int argc, const char *argv[])
{
	MFInitParams initParams;
	initParams.argc = argc;
	initParams.argv = argv;

	int r = GameMain(&initParams);

	sceKernelExitGame();
	return r;
}

#else

int main(int argc, const char *argv[])
{
	MFInitParams initParams;
	initParams.argc = argc;
	initParams.argv = argv;

	return GameMain(&initParams);
}

#endif
