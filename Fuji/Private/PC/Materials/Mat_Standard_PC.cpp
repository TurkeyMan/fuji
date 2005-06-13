#include "Common.h"
#include "Texture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "View_Internal.h"

#include "../../Source/Materials/Mat_Standard.h"

static MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

int Mat_Standard_RegisterMaterial(void *pPlatformData)
{
	CALLSTACK;

	return 0;
}

void Mat_Standard_UnregisterMaterial()
{
	CALLSTACK;

}

int Mat_Standard_Begin(MFMaterial *pMaterial)
{
	CALLSTACK;

	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			pd3dDevice->SetTexture(0, pData->pTextures[pData->diffuseMapIndex]->pTexture);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DXMATRIX*)&pData->textureMatrix);
		}
		else
		{
			pd3dDevice->SetTexture(0, NULL);
		}

		switch(pData->materialType&MF_BlendMask)
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

	return 0;
}

void Mat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	CALLSTACK;

	pMaterial->pInstanceData = Heap_Alloc(sizeof(Mat_Standard_Data));
	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	memset(pData, 0, sizeof(Mat_Standard_Data));

	pData->ambient = Vector4::one;
	pData->diffuse = Vector4::one;

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = Matrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;
}

void Mat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	CALLSTACK;

	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		Texture_Destroy(pData->pTextures[a]);
	}

	Heap_Free(pMaterial->pInstanceData);
}
