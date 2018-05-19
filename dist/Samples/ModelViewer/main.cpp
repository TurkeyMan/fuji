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
#include "Fuji/MFInput.h"
#include "Fuji/MFFont.h"

#include "Fuji/MFEffect.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

MFModel *pModel = NULL;

MFArray<MFString> models;
int menuIndex = 0;
bool bShowModel = false;

float yaw = 0.f;
float pitch = 0.f;
float zoom = 5.f;

const char* ppFormats[] =
{
	".f3d",
	".dae",
	".x",
	".ase",
	".obj",
	".md2",
	".md3",
	".memd2",

	// assimp formats
	".fbx",
	".blend",
	".3ds",
	".dxf",
	".lwo",
	".lws",
	".ms3d",
	".mdl",
	".pk3",
	".mdc",
	".md5",
	".smd",
	".vta",
	".m3",
	".3d"
};


/**** Functions ****/

void Game_InitFilesystem(void *pUserData)
{
	// mount the sample assets directory
	MFFileSystemHandle hNative = MFFileSystem_GetInternalFileSystemHandle(MFFSH_NativeFileSystem);
	MFMountDataNative mountData;
	mountData.cbSize = sizeof(MFMountDataNative);
	mountData.priority = MFMP_Normal;
	mountData.flags = MFMF_FlattenDirectoryStructure | MFMF_Recursive;
	mountData.pMountpoint = "sample";
#if defined(MF_IPHONE)
	mountData.pPath = MFFile_SystemPath();
#else
	mountData.pPath = MFFile_SystemPath("../Sample_Data/");
#endif
	MFFileSystem_Mount(hNative, &mountData);

	mountData.pMountpoint = "game";
	mountData.pPath = MFFile_SystemPath();
	MFFileSystem_Mount(hNative, &mountData);

	mountData.flags = MFMF_DontCacheTOC;
	mountData.pMountpoint = "data";
	MFFileSystem_Mount(hNative, &mountData);

	if(pInitFujiFS)
		pInitFujiFS(pUserData);
}

void Scan(MFString path)
{
	MFFindData fd;
	MFFind *pFind = MFFileSystem_FindFirst(MFStr("%s*", path.CStr()), &fd);
	if(pFind)
	{
		do
		{
			if(fd.info.attributes & MFFA_Directory)
			{
				Scan(MFString::Format("%s%s/", path.CStr(), fd.pFilename).CStr());
			}
			else
			{
				MFString ext = MFString(fd.pFilename).GetExtension();
				if(ext.Enumerate(ppFormats, sizeof(ppFormats) / sizeof(ppFormats[0])) > -1)
					models.push(MFString::Format("%s%s", path.CStr(), fd.pFilename));
			}
		}
		while(MFFileSystem_FindNext(pFind, &fd));

		MFFileSystem_FindClose(pFind);
	}
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

	Scan("data:");
}

void Game_Update(void *pUserData)
{
	if(!bShowModel)
	{
		if(MFInput_WasPressed(Key_Up, IDD_Keyboard) && menuIndex > 0)
			--menuIndex;
		else if(MFInput_WasPressed(Key_Down, IDD_Keyboard) && menuIndex < (int)models.size()-1)
			++menuIndex;
		else if(MFInput_WasPressed(Key_Return, IDD_Keyboard) && models.size() > 0)
		{
			bShowModel = true;

			// load model
			pModel = MFModel_CreateWithAnimation(models[menuIndex].CStr());
		}
	}
	else
	{
		if(MFInput_WasPressed(Key_Escape, IDD_Keyboard))
		{
			if(pModel)
			{
				MFModel_Release(pModel);
				pModel = NULL;
			}

			models.clear();
			Scan("data:");

			if(models.size() <= (size_t)menuIndex)
				menuIndex = models.size() ? (int)models.size() - 1 : 0;
			bShowModel = false;
			return;
		}

		if(pModel)
		{
			if(MFInput_Read(Mouse_LeftButton, IDD_Mouse) > 0.f)
			{
				yaw += -MFInput_Read(Mouse_XDelta, IDD_Mouse) * 0.02f;
				pitch += -MFInput_Read(Mouse_YDelta, IDD_Mouse) * 0.015f;
			}
			if(MFInput_Read(Mouse_MiddleButton, IDD_Mouse) > 0.f)
			{
				zoom *= 1.f + -MFInput_Read(Mouse_YDelta, IDD_Mouse) * 0.02f;
			}

			// calculate a spinning world matrix
			MFMatrix world;
			world.SetTranslation(MakeVector(0, -0.25f, 1) * zoom);
			world.RotateY(yaw);
			world.RotateX(pitch);

			// set world matrix to the model
			MFModel_SetWorldMatrix(pModel, world);

			// advance the animation
			MFAnimation *pAnim = MFModel_GetAnimation(pModel);
			if(pAnim)
			{
				float start, end;
				MFAnimation_GetFrameRange(pAnim, &start, &end);

				static float time = 0.f;
				time += MFSystem_GetTimeDelta();// * 500;
				while(time >= end)
					time -= end;
				MFAnimation_SetFrame(pAnim, time);
			}
		}
	}
}

void Game_Draw(void *pUserData)
{
	if(!bShowModel)
	{
		MFView_SetOrtho();

		// draw model list
		if(models.size() > 0)
		{
			for(int a=0; a<(int)models.size(); ++a)
			{
				MFFont_DrawText2(MFFont_GetDebugFont(), 100.f, 100.f + (-menuIndex*20 + a*20), 20.f, a == menuIndex ? MFVector::yellow : MFVector::white, models[a].CStr());
			}
		}
		else
		{
			MFFont_DrawText2(MFFont_GetDebugFont(), 100.f, 100.f, 20.f, MFVector::red, "No models found!");
		}
	}
	else
	{
		if(pModel)
		{
			// set projection
			MFView_ConfigureProjection(MFDEGREES(60.f), 0.1f, 10000.f);
			MFView_SetAspectRatio(MFDisplay_GetAspectRatio());
			MFView_SetProjection();

			// render the mesh
			MFRenderer_AddModel(pModel, NULL, MFView_GetViewState());
		}
		else
		{
			MFView_SetOrtho();
			MFFont_DrawText2(MFFont_GetDebugFont(), 100.f, 100.f, 20.f, MFVector::red, "Failed to load model!");
		}
	}
}

void Game_Deinit(void *pUserData)
{
	if(pModel)
		MFModel_Release(pModel);

	MFStateBlock_Destroy(pDefaultStates);

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
