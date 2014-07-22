#include "Fuji/Fuji.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFView.h"
#include "Fuji/MFMaterial.h"
#include "Fuji/MFPrimitive.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

#include "Haku/UI/HKUI.h"
#include "Haku/UI/HKWidgetLoader-XML.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

HKUserInterface *pUI;

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

	// init HKUserInterface
	HKUserInterface::Init();

	pUI = new HKUserInterface();
	HKUserInterface::SetActiveUI(pUI);

	// load a test UI
	HKWidget *pTestLayout = HKWidget_CreateFromXML("ui-test.xml");
	MFDebug_Assert(pTestLayout != NULL, "Couldn't load UI!");

	pUI->AddTopLevelWidget(pTestLayout, true);
}

void Game_Update()
{
	pUI->Update();
}

void Game_Draw()
{
	// Set identity camera (no camera)
	MFView_SetAspectRatio(MFDisplay_GetAspectRatio());
	MFView_SetProjection();

	MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

	// set the world matrix to identity
	MFMatrix world = MFMatrix::identity;

	// move the box into the scene (along the z axis)
	world.Translate(MakeVector(0, 0, 5));

	// increment rotation
	static float rotation = 0.0f;
	rotation += MFTimeDelta();

	// rotate the box
	world.RotateYPR(rotation, rotation * 2.0f, rotation * 0.5f);

	// begin rendering the box
	MFPrimitive(PT_TriList);
	MFSetMatrix(world);

	// begin rendering 12 triangles (12 * 3 vertices)
	MFBegin(3 * 12);

	// draw a bunch of triangles
	MFSetColour(1,0,0,1);
	MFSetPosition(-1,-1, -1);
	MFSetPosition(-1, 1, -1);
	MFSetPosition( 1, 1, -1);
	MFSetPosition(-1,-1, -1);
	MFSetPosition( 1, 1, -1);
	MFSetPosition( 1,-1, -1);

	MFSetColour(0,1,0,1);
	MFSetPosition(-1,-1,1);
	MFSetPosition( 1,-1,1);
	MFSetPosition( 1, 1,1);
	MFSetPosition(-1,-1,1);
	MFSetPosition( 1, 1,1);
	MFSetPosition(-1, 1,1);

	MFSetColour(0,0,1,1);
	MFSetPosition( 1,-1,1);
	MFSetPosition( 1,-1,-1);
	MFSetPosition( 1, 1,-1);
	MFSetPosition( 1,-1,1);
	MFSetPosition( 1, 1,-1);
	MFSetPosition( 1, 1,1);

	MFSetColour(1,0,1,1);
	MFSetPosition(-1,-1,1);
	MFSetPosition(-1, 1,1);
	MFSetPosition(-1, 1,-1);
	MFSetPosition(-1,-1,1);
	MFSetPosition(-1, 1,-1);
	MFSetPosition(-1,-1,-1);

	MFSetColour(1,1,0,1);
	MFSetPosition(-1, 1,1);
	MFSetPosition( 1, 1,1);
	MFSetPosition( 1, 1,-1);
	MFSetPosition(-1, 1,1);
	MFSetPosition( 1, 1,-1);
	MFSetPosition(-1, 1,-1);

	MFSetColour(0,1,1,1);
	MFSetPosition(-1,-1,1);
	MFSetPosition(-1,-1,-1);
	MFSetPosition( 1,-1,-1);
	MFSetPosition(-1,-1,1);
	MFSetPosition( 1,-1,-1);
	MFSetPosition( 1,-1,1);

	MFEnd();

	MFRect disp;
	MFDisplay_GetDisplayRect(&disp);
	MFView_SetOrtho(&disp);
	pUI->Draw();
}

void Game_Deinit()
{
	HKUserInterface::Deinit();

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
