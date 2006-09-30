#include "Fuji.h"
#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "MFRenderer_XB.h"
#include "../../Source/Materials/MFMat_Standard.h"

static MFMaterial *pSetMaterial = 0;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

extern IDirect3DDevice8 *pd3dDevice;

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	MFCALLSTACK;

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			MFRendererXB_SetTexture(0, pData->pTextures[pData->diffuseMapIndex]->pTexture);
			MFRendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			MFRendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			MFRendererXB_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

			MFRendererXB_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			MFRendererXB_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			MFRendererXB_SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
//			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&pData->textureMatrix);

			MFMatrix texMat = pData->textureMatrix;
			texMat.SetZAxis3(texMat.GetTrans());
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&texMat);
		}
		else
		{
			MFRendererXB_SetTexture(0, NULL);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				MFRendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				break;
			case MF_AlphaBlend:
				MFRendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				MFRendererXB_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				MFRendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				MFRendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
				MFRendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				MFRendererXB_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				MFRendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				MFRendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
				MFRendererXB_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				MFRendererXB_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
				MFRendererXB_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				MFRendererXB_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
		}

		switch(pData->materialType&MF_CullMode)
		{
			case 0<<6:
				MFRendererXB_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				break;
			case 1<<6:
				MFRendererXB_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
			case 2<<6:
				MFRendererXB_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
				break;
			case 3<<6:
				// 'default' ?
				MFRendererXB_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
		}

		MFRendererXB_SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		MFRendererXB_SetRenderState(D3DRS_ZWRITEENABLE, (pData->materialType&MF_NoZWrite) ? FALSE : TRUE);
		MFRendererXB_SetRenderState(D3DRS_ZFUNC, (pData->materialType&MF_NoZRead) ? D3DCMP_ALWAYS : D3DCMP_LESSEQUAL);
	}

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}
