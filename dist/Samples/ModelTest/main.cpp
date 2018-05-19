#include "Fuji/Fuji.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFRenderState.h"
#include "Fuji/MFModel.h"
#include "Fuji/MFAnimation.h"
#include "Fuji/MFView.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

#include "Fuji/MFEffect.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

MFModel *pModel;


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

	// load model
	pModel = MFModel_CreateWithAnimation("astro");
	MFDebug_Assert(pModel, "Couldn't load mesh!");
}

void Game_Update(void *pUserData)
{
	// calculate a spinning world matrix
	MFMatrix world;
	world.SetTranslation(MakeVector(0, -5, 50));

	static float rotation = 0.0f;
	rotation += MFSystem_GetTimeDelta();
	world.RotateY(rotation);

	// set world matrix to the model
	MFModel_SetWorldMatrix(pModel, world);

	// advance the animation
	MFAnimation *pAnim = MFModel_GetAnimation(pModel);
	if(pAnim)
	{
		float start, end;
		MFAnimation_GetFrameRange(pAnim, &start, &end);

		static float time = 0.f;
		time += MFSystem_GetTimeDelta();
		while(time >= end)
			time -= end;
		MFAnimation_SetFrame(pAnim, time);
	}
}

void Game_Draw(void *pUserData)
{
	// set projection
	MFView_SetAspectRatio(MFDisplay_GetAspectRatio());
	MFView_SetProjection();

	// render the mesh
	MFRenderer_AddModel(pModel, NULL, MFView_GetViewState());
}

void Game_Deinit(void *pUserData)
{
	MFModel_Release(pModel);

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
