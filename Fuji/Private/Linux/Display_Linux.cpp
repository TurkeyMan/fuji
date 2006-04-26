#include "Fuji.h"
#include "Display_Internal.h"

void MFDisplay_DestroyWindow()
{
	// need to destroy the window here
}

int MFDisplay_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	// need to create a window and the opngl context here

	return 0;
}

void MFDisplay_ResetDisplay()
{
	// probably dont need to do anything here
}

void MFDisplay_DestroyDisplay()
{
	// need to destroy the opengl contxt here
}

void MFDisplay_BeginFrame()
{
	// begin the frame
}

void MFDisplay_EndFrame()
{
	// end the frame and swap buffers
}

void MFDisplay_SetClearColour(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void MFDisplay_ClearScreen(uint32 flags)
{
	int mask = ((flags & CS_Colour) ? GL_COLOR_BUFFER_BIT : 0) | ((flags & CS_ZBuffer) ? GL_DEPTH_BUFFER_BIT : 0);
	glClear(mask);
}

void MFDisplay_SetViewport(float x, float y, float width, float height)
{
}

void MFDisplay_ResetViewport()
{
}

bool MFDisplay_IsWidescreen()
{
	return false;
}
