#if !defined(_RENDERER_H)
#define _RENDERER_H

enum RenderSource
{
	RS_None = 0,

	RS_Model,
	RS_MFPrimitive,
	RS_Custom
};

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

void Renderer_InitModule();
void Renderer_DeinitModule();

void Renderer_SetRenderer(uint32 additionalRendererFlags, uint32 flags = NULL, uint32 renderSource = RS_Custom);

uint32 SetRenderStateOverride(uint32 renderState, uint32 value);
uint32 GetRenderStateOverride(uint32 renderState);

#endif
