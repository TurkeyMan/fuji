#include "Fuji/Fuji.h"
#include "Fuji/MFDisplay.h"
#include "Fuji/MFRenderer.h"
#include "Fuji/MFRenderState.h"
#include "Fuji/MFTexture.h"
#include "Fuji/MFMaterial.h"
#include "Fuji/MFVertex.h"
#include "Fuji/MFView.h"
#include "Fuji/MFSystem.h"
#include "Fuji/MFFileSystem.h"
#include "Fuji/FileSystem/MFFileSystemNative.h"

void BuildVertexBuffers();


/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

MFRenderer *pRenderer = NULL;

MFVertexDeclaration *pVertexDecl = NULL;

MFVertexBuffer *pPrismVertexBuffer = NULL;
MFStateBlock *pPrismMeshStateBlock = NULL;
MFStateBlock *pPrismStateBlock = NULL;
MFMaterial *pPrismRenderTarget = NULL;

MFVertexBuffer *pBoxVertexBuffer = NULL;
MFStateBlock *pBoxMeshStateBlock = NULL;
MFStateBlock *pBoxStateBlock = NULL;


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
	MFRenderLayerDescription layers[] = { { "Prism" }, { "Box" } };
	pRenderer = MFRenderer_Create(layers, 2, NULL, NULL);
	MFRenderer_SetCurrent(pRenderer);

	// configure prism layer
	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, 0);
	MFRenderLayer_SetClear(pLayer, MFRCF_All, MakeVector(0.f, 0.f, 0.0f, 1.f));

	// create a render target for the layer
	MFTexture *pRenderTarget = MFTexture_CreateRenderTarget("Prism", 256, 256);
	pPrismRenderTarget = MFMaterial_Create("Prism");
	MFRenderLayer_SetLayerRenderTarget(pLayer, 0, pRenderTarget);
	MFTexture_Destroy(pRenderTarget); // release our local reference; materials holds the reference now

	// configure box layer
	pLayer = MFRenderer_GetLayer(pRenderer, 1);
	MFRenderLayer_SetClear(pLayer, MFRCF_All, MakeVector(0.f, 0.f, 0.2f, 1.f));

	// configure the box layer as the default render layer
	MFRenderLayerSet layerSet;
	MFZeroMemory(&layerSet, sizeof(layerSet));
	layerSet.pSolidLayer = pLayer;
	MFRenderer_SetRenderLayerSet(pRenderer, &layerSet);

	// build a vertex buffer
	BuildVertexBuffers();

	// create some state blocks
	pPrismStateBlock = MFStateBlock_Create(128);
	pBoxStateBlock = MFStateBlock_Create(128);
}

void Game_Update()
{
	static float rotation = 0.0f;
	rotation += MFSystem_TimeDelta();

	// spin the prism
	MFMatrix world;
	world.SetTranslation(MakeVector(0, 0.3f, 3));
	world.RotateY(rotation * 2.3f);
	MFStateBlock_SetMatrix(pPrismStateBlock, MFSCM_World, world);

	// spin the box
	world.SetTranslation(MakeVector(0, 0, 5));
	world.RotateYPR(rotation, rotation * 2.0f, rotation * 0.5f);
	MFStateBlock_SetMatrix(pBoxStateBlock, MFSCM_World, world);
}

void Game_Draw()
{
	// render the prism
	MFView_SetAspectRatio(1.f);
	MFView_SetProjection();

	MFRenderLayer *pLayer = MFRenderer_GetLayer(pRenderer, 0);
	MFRenderLayer_AddVertices(pLayer, pPrismMeshStateBlock, 0, 3*12, MFPT_TriangleList, MFMaterial_GetStockMaterial(MFMat_White), pPrismStateBlock, NULL, MFView_GetViewState());

	// render the box
	MFView_SetAspectRatio(MFDisplay_GetNativeAspectRatio());
	MFView_SetProjection();

	pLayer = MFRenderer_GetLayer(pRenderer, 1);
	MFRenderLayer_AddVertices(pLayer, pBoxMeshStateBlock, 0, 3*12, MFPT_TriangleList, pPrismRenderTarget, pBoxStateBlock, NULL, MFView_GetViewState());
}

void Game_Deinit()
{
	MFStateBlock_Destroy(pPrismStateBlock);
	MFStateBlock_Destroy(pPrismMeshStateBlock);
	MFVertex_DestroyVertexBuffer(pPrismVertexBuffer);

	MFStateBlock_Destroy(pBoxStateBlock);
	MFStateBlock_Destroy(pBoxMeshStateBlock);
	MFVertex_DestroyVertexBuffer(pBoxVertexBuffer);

	MFVertex_DestroyVertexDeclaration(pVertexDecl);

	MFMaterial_Destroy(pPrismRenderTarget);

	MFRenderer_Destroy(pRenderer);
}

void BuildVertexBuffers()
{
	struct Vertex
	{
		float pos[3];
		float uv[2];
		uint32 col;
	};

	// create vertex format declaration
	MFVertexElement elements[] =
	{
		{ 0, MFVET_Position, 0, 3 },
		{ 0, MFVET_TexCoord, 0, 2 },
		{ 0, MFVET_Colour, 0, 4 }
	};

	pVertexDecl = MFVertex_CreateVertexDeclaration(elements, sizeof(elements)/sizeof(elements[0]));

	// create box vertex buffer
	Vertex prism[4 * 3] = // 4 triangles, 1 per face
	{
		{ {  1, -1, -1 }, { 0, 0 }, 0xFFFF0000 },
		{ { -1, -1, -1 }, { 0, 0 }, 0xFFFF0000 },
		{ {  0,  1,  0 }, { 0, 0 }, 0xFFFF0000 },

		{ { -1, -1, -1 }, { 0, 0 }, 0xFF00FF00 },
		{ { -1, -1,  1 }, { 0, 0 }, 0xFF00FF00 },
		{ {  0,  1,  0 }, { 0, 0 }, 0xFF00FF00 },

		{ { -1, -1,  1 }, { 0, 0 }, 0xFF0000FF },
		{ {  1, -1,  1 }, { 0, 0 }, 0xFF0000FF },
		{ {  0,  1,  0 }, { 0, 0 }, 0xFF0000FF },

		{ {  1, -1,  1 }, { 0, 0 }, 0xFFFFFF00 },
		{ {  1, -1, -1 }, { 0, 0 }, 0xFFFFFF00 },
		{ {  0,  1,  0 }, { 0, 0 }, 0xFFFFFF00 }
	};

	pPrismVertexBuffer = MFVertex_CreateVertexBuffer(pVertexDecl, 4*3, MFVBType_Static, prism);

	pPrismMeshStateBlock = MFStateBlock_Create(64);
	MFStateBlock_SetRenderState(pPrismMeshStateBlock, MFSCRS_VertexDeclaration, pVertexDecl);
	MFStateBlock_SetRenderState(pPrismMeshStateBlock, MFSCRS_VertexBuffer(0), pPrismVertexBuffer);

	// create box vertex buffer
	Vertex box[12 * 3] = // 12 triangles, 2 per 6 faces of a cube
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

	pBoxVertexBuffer = MFVertex_CreateVertexBuffer(pVertexDecl, 12*3, MFVBType_Static, box);

	pBoxMeshStateBlock = MFStateBlock_Create(64);
	MFStateBlock_SetRenderState(pBoxMeshStateBlock, MFSCRS_VertexDeclaration, pVertexDecl);
	MFStateBlock_SetRenderState(pBoxMeshStateBlock, MFSCRS_VertexBuffer(0), pBoxVertexBuffer);
}

int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)(MFSystem_ReadRTC() & 0xFFFFFFFF));

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
