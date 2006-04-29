#include "Fuji.h"
#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "../../Source/Materials/MFMat_Standard.h"

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
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
			glBindTexture(GL_TEXTURE_2D, pData->pTextures[pData->diffuseMapIndex]->textureID);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//			glActiveTexture(0);
		}
		else
		{
//			glActiveTexture(texUnit);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
//				MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//				MFRendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
//				MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
			case MF_AlphaBlend:
//				MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//				MFRendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//				MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
//				MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//				MFRendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//				MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
//				MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//				MFRendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
//				MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
		}

		switch(pData->materialType&MF_CullMode)
		{
			case 0<<6:
//				MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				break;
			case 1<<6:
//				MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
			case 2<<6:
//				MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
				break;
			case 3<<6:
				// 'default' ?
//				MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
		}

		// TODO: This is broken! You cant disable zwrites and still have zreads with this configuration...
		if(pData->materialType&MF_NoZRead)
			glDepthFunc(GL_ALWAYS);
		else
			glDepthFunc(GL_LEQUAL);

		if(pData->materialType&MF_NoZWrite)
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
	}

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	memset(pData, 0, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend | 1<<6 /* back face culling */;
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
