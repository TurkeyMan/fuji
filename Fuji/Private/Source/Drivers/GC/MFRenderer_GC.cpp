#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_GC

#include "MFRenderer_Internal.h"
#include "Display_Internal.h"

MFVector gClearColour = MakeVector(0.f,0.f,0.22f,1.f);
MFRect gCurrentViewport;

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{

	return 0;
}

void MFRenderer_DestroyDisplay()
{
}

void MFRenderer_ResetDisplay()
{
	MFRenderer_ResetViewport();
}

void MFRenderer_BeginFrame()
{
}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;

}

void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFRenderer_ClearScreen(uint32 flags)
{
	MFCALLSTACK;
}

void MFRenderer_GetViewport(MFRect *pRect)
{
	*pRect = gCurrentViewport;
}

void MFRenderer_SetViewport(MFRect *pRect)
{
	MFCALLSTACK;

	gCurrentViewport = *pRect;
	// set viewport
}

void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)gDisplay.width;
	gCurrentViewport.height = (float)gDisplay.height;

	// set viewport
}

#endif
