#if !defined(_RENDERER_INTERNAL_H)
#define _RENDERER_INTERNAL_H

// init/deinit
void Renderer_InitModule();
void Renderer_DeinitModule();

enum RenderState
{
	RS_MaterialOverride,
	RS_NoZRead,
	RS_NoZWrite,
	RS_Untextured,
	RS_PreLit,
	RS_PrimType,

	RS_ShowZBuffer,
	RS_ShowOverDraw,

	RS_Max
};

#include "Renderer.h"

#endif