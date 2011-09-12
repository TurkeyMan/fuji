#include "Fuji.h"
#include "MFDisplay.h"
#include "MFRenderer.h"
#include "MFView.h"
#include "MFMaterial.h"
#include "MFPrimitive.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemNative.h"

#include "UI/HKUI.h"
#include "UI/HKInputSource.h"

#include "UI/Widgets/HKWidgetButton.h"
#include "UI/Widgets/HKWidgetLabel.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"

/**** Globals ****/

MFSystemCallbackFunction pInitFujiFS = NULL;

HKUserInterface *pUI;

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

	HKUserInterface::Init();

	pUI = new HKUserInterface();
	HKUserInterface::SetActiveUI(pUI);

	// hard code a test UI...
	HKWidgetLayoutFrame *pFrame = (HKWidgetLayoutFrame*)HKUserInterface::CreateWidget("HKWidgetLayoutFrame");
	pFrame->SetSize(MakeVector(200, 200));
	pFrame->SetMargin(MakeVector(2, 5, 10, 20));
	pFrame->SetPosition(MakeVector(100, 100));

	HKWidgetButton *pButtonBig = (HKWidgetButton *)HKUserInterface::CreateWidget("HKWidgetButton");
	pButtonBig->SetLabel("Wide Load!");
	int i = pFrame->AddChild(pButtonBig);
	pFrame->SetChildJustification(i, HKWidgetLayout::BottomFill);

	HKWidgetLabel *pLabel = (HKWidgetLabel *)HKUserInterface::CreateWidget("HKWidgetLabel");
	pLabel->SetLabel("Whee!");
	i = pFrame->AddChild(pLabel);
	pFrame->SetChildJustification(i, HKWidgetLayout::TopRight);

	HKWidgetLayoutLinear *pLinear = (HKWidgetLayoutLinear*)HKUserInterface::CreateWidget("HKWidgetLayoutLinear");

	HKWidgetButton *pButton1 = (HKWidgetButton *)HKUserInterface::CreateWidget("HKWidgetButton");
	pButton1->SetLabel("Button");
	HKWidgetButton *pButton2 = (HKWidgetButton *)HKUserInterface::CreateWidget("HKWidgetButton");
	pButton2->SetLabel("Button2");

	pLinear->AddChild(pButton1);
	pLinear->AddChild(pButton2);

	i = pFrame->AddChild(pLinear);
	pFrame->SetChildJustification(i, HKWidgetLayout::Center);

	pUI->SetRootWidget(pFrame);
}

void Game_Update()
{
	MFCALLSTACK;

	pUI->Update();
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

	MFMaterial_SetMaterial(MFMaterial_GetStockMaterial(MFMat_White));

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

	MFView_Pop();
}

void Game_Deinit()
{
	MFCALLSTACK;

	HKUserInterface::Deinit();
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
