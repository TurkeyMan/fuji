#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PSP

#include "Display_Internal.h"
#include "DebugMenu_Internal.h"
#include "MFRenderer_Internal.h"

#include <pspdisplay.h>
#include <pspgu.h>

#include <pspdebug.h>

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH * SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

// 256k display buffer...
static unsigned int __attribute__((aligned(16))) displayList[256 * 1024];

MFRect gCurrentViewport;

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{
	gDisplay.width = SCR_WIDTH;
	gDisplay.height = SCR_HEIGHT;
	gDisplay.wide = true;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = SCR_WIDTH;
	gCurrentViewport.height = SCR_HEIGHT;

	sceGuInit();

	sceGuStart(GU_DIRECT, displayList);
	sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)FRAME_SIZE, BUF_WIDTH);
	sceGuDepthBuffer((void*)(FRAME_SIZE*2), BUF_WIDTH);
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

void MFRenderer_DestroyDisplay()
{
	sceGuTerm();
}

void MFRenderer_ResetDisplay()
{
}

extern uint32 currentVert;

void MFRenderer_BeginFramePlatformSpecific()
{
	MFCALLSTACK;

	currentVert = 0;

	sceGuStart(0, displayList);
}

void MFRenderer_EndFramePlatformSpecific()
{
	MFCALLSTACK;

	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

MF_API void MFRenderer_ClearScreen(MFRenderClearFlags flags, MFVector colour, float z, int stencil)
{
	MFCALLSTACK;

	clearColour = ((uint32)(a*255.0f) << 24) |
				  ((uint32)(b*255.0f) << 16) |
				  ((uint32)(g*255.0f) << 8) |
				   (uint32)(r*255.0f);

	sceGuClearColor(clearColour);
	sceGuClearDepth(0xFFFF);

	sceGuClear( ((flags&CS_Colour) ? GU_COLOR_BUFFER_BIT : NULL) | ((flags&CS_ZBuffer) ? GU_DEPTH_BUFFER_BIT : NULL) );
}

void MFRenderer_SetViewport(MFRect *pRect)
{
	MFCALLSTACK;

	gCurrentViewport = *pRect;

	sceGuViewport(2048 + (int)pRect->x, 2048 + (int)pRect->y, (int)pRect->width, (int)pRect->height);
}

void MFRenderer_ResetViewport()
{
	MFCALLSTACK;

	gCurrentViewport.x = 0.0f;
	gCurrentViewport.y = 0.0f;
	gCurrentViewport.width = (float)SCR_WIDTH;
	gCurrentViewport.height = (float)SCR_HEIGHT;

	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
}

#endif
