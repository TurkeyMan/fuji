#include "Fuji.h"
#include "MFSystem.h"
#include "MFDisplay.h"
#include "MFRenderer.h"
#include "MFView.h"
#include "MFFont.h"
#include "MFInput.h"
#include "Timer.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemNative.h"
ř
/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

/**** Functions ****/

void Game_InitFilesystem()
{
	// mount the game directory
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
		pInitFujiFS();
}

void Game_Init()
{
	MFCALLSTACK;
}

#define CHK_BUTTON(x) \
     if (MFInput_Read(x, IDD_Gamepad)!=0.0f)  MFDebug_Message(#x);

void Game_Update()
{
	MFCALLSTACK;

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

void Game_Draw()
{
	MFCALLSTACK;

	// set clear colour and clear the screen
	MFRenderer_SetClearColour(0.f, 0.f, 0.2f, 1.f);
	MFRenderer_ClearScreen();

	// push current view onto the stack
	MFView_Push();

	// set orthographic projection
	MFView_SetOrtho();

	// render some text
	MFFont_DrawText(MFFont_GetDebugFont(), 200.f, 200.f, 50.f, MFVector::one, "Hello World!");

	// pop the current view
	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;
}


int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)MFSystem_ReadRTC());

	MFSystem_RegisterSystemCallback(MFCB_InitDone, Game_Init);
	MFSystem_RegisterSystemCallback(MFCB_Update, Game_Update);
	MFSystem_RegisterSystemCallback(MFCB_Draw, Game_Draw);
	MFSystem_RegisterSystemCallback(MFCB_Deinit, Game_Deinit);

	pInitFujiFS = MFSystem_RegisterSystemCallback(MFCB_FileSystemInit, Game_InitFilesystem);

	return MFMain(pInitParams);
}

#if defined(MF_WINDOWS) || defined(_WINDOWS)
#include <windows.h>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.hInstance = hInstance;
	initParams.pCommandLine = lpCmdLine;

	return GameMain(&initParams);
}

#elif defined(MF_PSP) || defined(_PSP)
#include <pspkernel.h>

int main(int argc, const char *argv[])
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
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
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.argc = argc;
	initParams.argv = argv;

	return GameMain(&initParams);
}

#endif
