#include "Common.h"
#include "Renderer_PC.h"
#include "Material.h"
#include "Model.h"
#include "Animation.h"
#include "View.h"

extern Material *pCurrentMaterial;
extern uint32 renderSource;
extern View *pCurrentView;

void Renderer_SetRenderer(uint32 rendererFlags, uint32 flags, uint32 newRenderSource)
{
	Material *pMat = Material::GetCurrent();

//	if(pCurrentMaterial != pMat)
	{
		// set some render states
		if(pMat->pTextures[pMat->diffuseMapIndex] && !(rendererFlags & RT_Untextured))
		{
			pMat->pTextures[pMat->diffuseMapIndex]->SetTexture(0);

			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DXMATRIX*)&pMat->textureMatrix);
		}
		else
		{
			Texture::UseNone(0);
		}
	}

	pCurrentMaterial = pMat;
	renderSource = newRenderSource;
}


