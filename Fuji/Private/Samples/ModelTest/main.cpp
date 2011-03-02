#include "Fuji.h"
#include "MFDisplay.h"
#include "MFRenderer.h"
#include "MFView.h"
#include "MFModel.h"
#include "MFSystem.h"

/**** Globals ****/

MFModel *pModel;

/**** Functions ****/

void Game_Init()
{
	MFCALLSTACK;

	pModel = MFModel_Create("astro");
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

	// set the world matrix to identity
	MFMatrix world = MFMatrix::identity;

	// move the model into the scene (along the z axis), and down a little bit.
	world.Translate(MakeVector(0, -5, 50));

	// increment rotation
	static float rotation = 0.0f;
	rotation += MFSystem_TimeDelta();

	// rotate the box
	world.RotateY(rotation);

	// draw the model
	MFModel_SetWorldMatrix(pModel, world);
	MFModel_Draw(pModel);

	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;

	MFModel_Destroy(pModel);
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

int main(int argc, char *argv[])
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

int main(int argc, char *argv[])
{
	MFInitParams initParams;
	MFZeroMemory(&initParams, sizeof(MFInitParams));
	initParams.argc = argc;
	initParams.argv = argv;

	return GameMain(&initParams);
}

#endif
