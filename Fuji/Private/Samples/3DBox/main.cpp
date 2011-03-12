#include "Fuji.h"
#include "MFDisplay.h"
#include "MFRenderer.h"
#include "MFView.h"
#include "MFMaterial.h"
#include "MFPrimitive.h"
#include "MFSystem.h"

/**** Globals ****/

MFMaterial *pMat;

/**** Functions ****/

void Game_Init()
{
	MFCALLSTACK;

	pMat = MFMaterial_Create("samnmax");
}

void Game_Update()
{
	MFCALLSTACK;
}

void Game_Draw()
{
	MFCALLSTACK;

	MFRenderer_SetClearColour(0.f, 0.f, 0.2f, 1.f);
	MFRenderer_ClearScreen();

	// Set identity camera (no camera)
	MFView_Push();
	MFView_SetAspectRatio(MFDisplay_GetNativeAspectRatio());
	MFView_SetProjection();

	MFMaterial_SetMaterial(pMat);

	// set the world matrix to identity
	MFMatrix world = MFMatrix::identity;

	// move the box into the scene (along the z axis)
	world.Translate(MakeVector(0, 0, 5));

	// increment rotation
	static float rotation = 0.0f;
	rotation += MFSystem_TimeDelta();

	// rotate the box
	world.RotateYPR(rotation, rotation * 2.0f, rotation * 0.5f);

	// begin rendering the box
	MFPrimitive(PT_TriList);
	MFSetMatrix(world);

	// begin rendering 12 triangles (12 * 3 vertices)
	MFBegin(3 * 12);

	// draw a bunch of triangles
	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1, -1);
	MFSetTexCoord1(1,0);
	MFSetPosition(-1, 1, -1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1, -1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1, -1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1, -1);
	MFSetTexCoord1(0,1);
	MFSetPosition( 1,-1, -1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,0);
	MFSetPosition( 1,-1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,1);
	MFSetTexCoord1(0,1);
	MFSetPosition(-1, 1,1);

	MFSetTexCoord1(0,0);
	MFSetPosition( 1,-1,1);
	MFSetTexCoord1(1,0);
	MFSetPosition( 1,-1,-1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition( 1,-1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,-1);
	MFSetTexCoord1(0,1);
	MFSetPosition( 1, 1,1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,0);
	MFSetPosition(-1, 1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition(-1, 1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition(-1, 1,-1);
	MFSetTexCoord1(0,1);
	MFSetPosition(-1,-1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1, 1,1);
	MFSetTexCoord1(1,0);
	MFSetPosition( 1, 1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1, 1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1, 1,-1);
	MFSetTexCoord1(0,1);
	MFSetPosition(-1, 1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,0);
	MFSetPosition(-1,-1,-1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1,-1,-1);

	MFSetTexCoord1(0,0);
	MFSetPosition(-1,-1,1);
	MFSetTexCoord1(1,1);
	MFSetPosition( 1,-1,-1);
	MFSetTexCoord1(0,1);
	MFSetPosition( 1,-1,1);

	MFEnd();

	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;

	MFMaterial_Destroy(pMat);
}


int GameMain(MFInitParams *pInitParams)
{
	MFRand_Seed((uint32)MFSystem_ReadRTC());

	MFSystem_RegisterSystemCallback(MFCB_InitDone, Game_Init);
	MFSystem_RegisterSystemCallback(MFCB_Update, Game_Update);
	MFSystem_RegisterSystemCallback(MFCB_Draw, Game_Draw);
	MFSystem_RegisterSystemCallback(MFCB_Deinit, Game_Deinit);

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
