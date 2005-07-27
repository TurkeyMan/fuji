#include "Common.h"
#include "Display_Internal.h"
#include "DebugMenu_Internal.h"

#include <pspdisplay.h>
#include <pspgu.h>

#include <pspdebug.h>

bool isortho = false;
float fieldOfView;

uint32 clearColour = 0xFF400000;

static unsigned int __attribute__((aligned(16))) displayList[262144];

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

int Display_CreateDisplay(int width, int height, int bpp, int rate, bool vsync, bool triplebuffer, bool wide, bool progressive)
{
	CALLSTACK;

	sceGuInit();

	sceGuStart(GU_DIRECT, displayList);
	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)0x88000, BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000, BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2), 2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuDepthRange(0x0, 0xFFFF);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_LEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW); // NOTE: not CCW
	sceGuShadeModel(GU_SMOOTH);
	sceGuDisable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	return 0;
}

void Display_DestroyWindow()
{
}

void Display_ResetDisplay()
{
}

void Display_DestroyDisplay()
{
	CALLSTACK;

	sceGuTerm();
}

extern uint32 currentVert;

void Display_BeginFrame()
{
	CALLSTACK;

	currentVert = 0;

	sceGuStart(0, displayList);
}

void Display_EndFrame()
{
	CALLSTACK;

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void Display_SetClearColour(float r, float g, float b, float a)
{
	clearColour = ((uint32)(a*255.0f) << 24) |
				  ((uint32)(b*255.0f) << 16) |
				  ((uint32)(g*255.0f) << 8) |
				   (uint32)(r*255.0f);

	sceGuClearColor(clearColour);
	sceGuClearDepth(0xFFFF);
}

void Display_ClearScreen(uint32 flags)
{
	CALLSTACK;

	sceGuClear( ((flags&CS_Colour) ? GU_COLOR_BUFFER_BIT : NULL) | ((flags&CS_ZBuffer) ? GU_DEPTH_BUFFER_BIT : NULL) );
}

void SetViewport(float x, float y, float width, float height)
{
	CALLSTACK;

	sceGuViewport(2048 + (int)x, 2048 + (int)y, (int)width, (int)height);
}

void ResetViewport()
{
	CALLSTACK;

	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
}
