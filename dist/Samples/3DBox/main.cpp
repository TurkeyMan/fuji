#include "Fuji/Fuji.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFRenderState.h"
#include "Fuji/MFMaterial.h"
#include "Fuji/MFVertex.h"
#include "Fuji/MFView.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

void BuildVertexBuffer();


/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;
MFStateBlock *pDefaultStates = NULL;

MFMaterial *pMaterial = NULL;

MFVertexDeclaration *pVertexDecl = NULL;
MFVertexBuffer *pVertexBuffer = NULL;
MFStateBlock *pMeshStateBlock = NULL;

MFStateBlock *pEntityStateBlock = NULL;


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

	// create a materual
	pMaterial = MFMaterial_Create("samnmax");

	// build a vertex buffer
	BuildVertexBuffer();

	// create a stateblock for the entity
	pEntityStateBlock = MFStateBlock_Create(128);
}

void Game_Update(void *pUserData)
{
	// calculate a spinning world matrix
	MFMatrix world;
	world.SetTranslation(MakeVector(0, 0, 5));

	static float rotation = 0.0f;
	rotation += MFSystem_GetTimeDelta();
	world.RotateYPR(rotation, rotation * 2.0f, rotation * 0.5f);

	// set world matrix to the entity stateblock
	MFStateBlock_SetMatrix(pEntityStateBlock, MFSCM_World, world);
}

void Game_Draw(void *pUserData)
{
	// set projection
	MFView_SetAspectRatio(MFDisplay_GetAspectRatio());
	MFView_SetProjection();

	// render the mesh
	MFRenderer_AddVertices(pMeshStateBlock, 0, 3*12, MFPT_TriangleList, pMaterial, pEntityStateBlock, NULL, MFView_GetViewState());
}

void Game_Deinit(void *pUserData)
{
	MFStateBlock_Destroy(pEntityStateBlock);

	MFStateBlock_Destroy(pMeshStateBlock);
	MFVertex_ReleaseVertexBuffer(pVertexBuffer);
	MFVertex_ReleaseVertexDeclaration(pVertexDecl);

	MFMaterial_Release(pMaterial);

	MFRenderer_Destroy(pRenderer);
}

void BuildVertexBuffer()
{
	// create vertex format declaration
	MFVertexElement elements[] =
	{
		{ 0, MFVET_Position, 0, 3 },
		{ 0, MFVET_TexCoord, 0, 2 },
		{ 0, MFVET_Colour, 0, 4 }
	};

	pVertexDecl = MFVertex_CreateVertexDeclaration(elements, sizeof(elements)/sizeof(elements[0]));

	// create vertex buffer
	struct Vertex
	{
		float pos[3];
		float uv[2];
		uint32 col;
	} verts[12 * 3] = // 12 triangles, 2 per 6 faces of a cube
	{
		{ { -1, -1, -1 }, { 0, 0 }, 0xFFFFFFFF },
		{ { -1,  1, -1 }, { 1, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },

		{ { -1, -1, -1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },
		{ {  1, -1, -1 }, { 0, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1, -1,  1 }, { 1, 0 }, 0xFFFFFFFF },
		{ {  1,  1,  1 }, { 1, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1,  1,  1 }, { 1, 1 }, 0xFFFFFFFF },
		{ { -1,  1,  1 }, { 0, 1 }, 0xFFFFFFFF },

		{ {  1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1, -1, -1 }, { 1, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },

		{ {  1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },
		{ {  1,  1,  1 }, { 0, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ { -1,  1,  1 }, { 1, 0 }, 0xFFFFFFFF },
		{ { -1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ { -1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },
		{ { -1, -1, -1 }, { 0, 1 }, 0xFFFFFFFF },

		{ { -1,  1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1,  1,  1 }, { 1, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },

		{ { -1,  1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1,  1, -1 }, { 1, 1 }, 0xFFFFFFFF },
		{ { -1,  1, -1 }, { 0, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ { -1, -1, -1 }, { 1, 0 }, 0xFFFFFFFF },
		{ {  1, -1, -1 }, { 1, 1 }, 0xFFFFFFFF },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFFFFFFFF },
		{ {  1, -1, -1 }, { 1, 1 }, 0xFFFFFFFF },
		{ {  1, -1,  1 }, { 0, 1 }, 0xFFFFFFFF }
	};

	pVertexBuffer = MFVertex_CreateVertexBuffer(pVertexDecl, 12*3, MFVBType_Static, verts);

	// create state block containing geometry data
	pMeshStateBlock = MFStateBlock_Create(64);
	MFStateBlock_SetRenderState(pMeshStateBlock, MFSCRS_VertexDeclaration, pVertexDecl);
	MFStateBlock_SetRenderState(pMeshStateBlock, MFSCRS_VertexBuffer(0), pVertexBuffer);
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
