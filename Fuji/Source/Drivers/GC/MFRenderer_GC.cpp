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

bool MFRenderer_BeginFrame()
{
	return true;
}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;

}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, const MFVector &colour, float z, int stencil)
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
