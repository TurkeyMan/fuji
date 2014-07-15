#include "Fuji/Fuji.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFView.h"
#include "Fuji/MFFont.h"
#include "Fuji/MFInput.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

/**** Functions ****/

void Game_InitFilesystem()
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
		pInitFujiFS();
}

void Game_Init()
{
	// create the renderer with a single layer that clears before rendering
	MFRenderLayerDescription layers[] = { "Scene" };
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

void Game_Update()
{
}

void Game_Draw()
{
	MFView_SetOrtho();

	const char *pText;
	float x, y;
	int a, b;

	x = 30.0f;
	for(a=0; a<MFInput_GetNumGamepads(); a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
		{
			MFFont_DrawText2(MFFont_GetDebugFont(), x-10.0f, 15.0f, 15.0f, MFVector::one, MFStr("%s (%d):", MFInput_GetDeviceName(IDD_Gamepad,a), a));

			if(MFInput_IsReady(IDD_Gamepad, a))
			{
				y = 30.0f;
				for(b=0; b<GamepadType_Max; b++)
				{
					float value = MFInput_Read(b, IDD_Gamepad, a);
//					int ival = (int)(value * 255.f);
//					MFFont_DrawText2(MFFont_GetDebugFont(), x, y, 15.0f, MFVector::one, MFStr("%s: %.3g - %d (0x%02X)", MFInput_EnumerateString(b, IDD_Gamepad, a), value, ival, ival));
					MFFont_DrawText2(MFFont_GetDebugFont(), x, y, 15.0f, MFVector::one, MFStr("%s: %.3g", MFInput_EnumerateString(b, IDD_Gamepad, a), value));
					y += 15.0f;
				}
			}
			else
			{
				MFFont_DrawText2(MFFont_GetDebugFont(), x, 30.0f, 15.0f, MakeVector(1.0f, 0.0f, 0.0f, 1.0f), "Disconnected...");
			}

			x += 170.0f;
		}
	}

	int numMouses = MFInput_GetNumPointers();

	x = 80.0f;
	for(a=0; a<numMouses; a++)
	{
		MFFont_DrawText2(MFFont_GetDebugFont(), x, 355.0f, 15.0f, MFVector::one, MFStr("Mouse Pos: %g, %g", MFInput_Read(Mouse_XPos, IDD_Mouse, a), MFInput_Read(Mouse_YPos, IDD_Mouse, a)));
		MFFont_DrawText2(MFFont_GetDebugFont(), x, 370.0f, 15.0f, MFVector::one, MFStr("Mouse Delta: %g, %g", MFInput_Read(Mouse_XDelta, IDD_Mouse, a), MFInput_Read(Mouse_YDelta, IDD_Mouse, a)));
		MFFont_DrawText2(MFFont_GetDebugFont(), x, 385.0f, 15.0f, MFVector::one, MFStr("Mouse Wheel: %g, %g", MFInput_Read(Mouse_Wheel, IDD_Mouse, a), MFInput_Read(Mouse_Wheel2, IDD_Mouse, a)));
		pText = "Mouse Buttons:";
		for(b=Mouse_MaxAxis; b<Mouse_Max; b++)
		{
			if(MFInput_Read(b, IDD_Mouse, a))
			{
				pText = MFStr("%s %s", pText, MFInput_EnumerateString(b, IDD_Mouse, a));
			}
		}
		MFFont_DrawText2(MFFont_GetDebugFont(), x, 400.0f, 15.0f, MFVector::one, pText);

		x += 170.0f;
	}

	pText = "Keys:";
	for(a=0; a<Key_Max; a++)
	{
		if(MFInput_Read(a, IDD_Keyboard, 0))
		{
			pText = MFStr("%s %s", pText, MFInput_EnumerateString(a, IDD_Keyboard, 0));
		}
	}
	MFFont_DrawText2(MFFont_GetDebugFont(), 80.0f, 430.0f, 15.0f, MFVector::one, pText);
}

void Game_Deinit()
{
	MFRenderer_Destroy(pRenderer);
}


int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)(MFSystem_ReadRTC() & 0xFFFFFFFF));

//	gDefaults.input.useXInput = false;

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
