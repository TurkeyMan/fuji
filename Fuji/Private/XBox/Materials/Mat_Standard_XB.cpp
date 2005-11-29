#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "View_Internal.h"
#include "Renderer_XB.h"
#include "../../Source/Materials/Mat_Standard.h"

static MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

int Mat_Standard_RegisterMaterial(void *pPlatformData)
{
	MFCALLSTACK;

	return 0;
}

void Mat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

}

int Mat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			RendererXB_SetTexture(0, pData->pTextures[pData->diffuseMapIndex]->pTexture);
			RendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			RendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			RendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

			RendererXB_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RendererXB_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			RendererXB_SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DXMATRIX*)&pData->textureMatrix);
		}
		else
		{
			RendererXB_SetTexture(0, NULL);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				RendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				RendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
			case MF_AlphaBlend:
				RendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
				RendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
				RendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				RendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
		}
	}

	return 0;
}

void Mat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(Mat_Standard_Data));
	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	memset(pData, 0, sizeof(Mat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;
}

void Mat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	Mat_Standard_Data *pData = (Mat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	Heap_Free(pMaterial->pInstanceData);
}
