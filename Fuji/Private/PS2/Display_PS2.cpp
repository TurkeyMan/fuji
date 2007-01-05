#include "Fuji.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

#include <graph.h>
#include "PS2_Internal.h"

bool isortho = false;
float fieldOfView;

extern MFVector gClearColour;

void MFDisplay_DestroyWindow()
{
	MFCALLSTACK;
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	MFCALLSTACK;

	graph_initialize();
	graph_set_mode(GRAPH_MODE_PAL, GRAPH_PSM_32, GRAPH_PSM_32);
	graph_set_displaybuffer(0);
	graph_set_drawbuffer(0);
	graph_set_zbuffer(graph_get_size());

	return 0;
}

void MFDisplay_ResetDisplay()
{
	MFCALLSTACK;
}

void MFDisplay_DestroyDisplay()
{
	MFCALLSTACK;
}

void MFDisplay_BeginFrame()
{
	MFCALLSTACK;
}

void MFDisplay_EndFrame()
{
	MFCALLSTACK;

// Wait for the vertical blank	
#define CSR ((volatile u64 *)(0x12001000))
	*CSR = *CSR & 8;
	while(!(*CSR & 8));
}

void MFDisplay_SetClearColour(float r, float g, float b, float a)
{
	gClearColour.x = r;
	gClearColour.y = g;
	gClearColour.z = b;
	gClearColour.w = a;
}

void MFDisplay_ClearScreen(uint32 flags)
{
	MFCALLSTACK;
	graph_set_clearbuffer(gClearColour.x * 256, gClearColour.y*256, gClearColour.z*256);
}

void MFDisplay_SetViewport(float x, float y, float width, float height)
{
	MFCALLSTACK;
}

void MFDisplay_ResetViewport()
{
	MFCALLSTACK;
}

float MFDisplay_GetNativeAspectRatio()
{
	return MFAspect_4x3;
}

bool MFDisplay_IsWidescreen()
{
	return false;
}
