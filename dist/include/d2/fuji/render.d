module fuji.render;

public import fuji.c.MFRenderer;

import fuji.renderstate;

alias MFRenderer_Create = fuji.c.MFRenderer.MFRenderer_Create;
MFRenderer* MFRenderer_Create(MFRenderLayerDescription[] layers, MFStateBlock* pGlobal, MFStateBlock* pOverride) nothrow
{
	return MFRenderer_Create(layers.ptr, cast(int)layers.length, pGlobal, pOverride);
}
