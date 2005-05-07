#include "Common.h"
#include "Texture_Internal.h"
#include "Material_Internal.h"
#include "View_Internal.h"
#include "Display_Internal.h"

#include "Renderer_XB.h"

extern Material *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

void Renderer_SetRenderer(uint32 rendererFlags, uint32 flags, uint32 newRenderSource)
{
	Material *pMat = Material_GetCurrent();

	if(pSetMaterial != pMat || (currentRenderFlags&RT_Untextured) != (rendererFlags&RT_Untextured))
	{
		// set some render states
		if(pMat->pTextures[pMat->diffuseMapIndex] && !(rendererFlags & RT_Untextured))
		{
			pd3dDevice->SetTexture(0, pMat->pTextures[pMat->diffuseMapIndex]->pTexture);
			pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);

			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DXMATRIX*)&pMat->textureMatrix);
		}
		else
		{
			pd3dDevice->SetTexture(0, NULL);
		}
	}

	if(pSetMaterial != pMat)
	{
		switch(pMat->materialType&MF_BlendMask)
		{
			case 0:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
			case MF_AlphaBlend:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
		}
	}

	pSetMaterial = pMat;
	renderSource = newRenderSource;
	currentRenderFlags = rendererFlags;
}


