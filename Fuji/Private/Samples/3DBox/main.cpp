#include "Fuji.h"
#include "Display.h"
#include "MFView.h"
#include "MFMaterial.h"
#include "MFPrimitive.h"
#include "MFSystem.h"

/**** Globals ****/

MFMaterial *pMat;

/**** Functions ****/

void Game_InitSystem()
{
	MFCALLSTACK;
}

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

	MFDisplay_SetClearColour(0.f, 0.f, 0.2f, 1.f);
	MFDisplay_ClearScreen();

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
