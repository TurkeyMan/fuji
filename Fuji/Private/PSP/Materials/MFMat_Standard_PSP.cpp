#include "Fuji.h"
#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "../../Source/Materials/MFMat_Standard.h"

#include <pspdisplay.h>
#include <pspgu.h>

static MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

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

	sceGuAmbientColor(0xFFFFFFFF);

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			MFTexture *pTexture = pData->pTextures[pData->diffuseMapIndex];

			sceGuSetStatus(GU_TEXTURE_2D, GU_TRUE);

			int width = pTexture->pTemplateData->pSurfaces[0].width;
			int height = pTexture->pTemplateData->pSurfaces[0].height;
			char *pImageData = pTexture->pTemplateData->pSurfaces[0].pImageData;

			sceGuTexMode(pTexture->pTemplateData->platformFormat, 0, 0, pTexture->pTemplateData->swizzled);
			sceGuTexImage(0, width, height, width, pImageData);
			sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
			sceGuTexFilter(GU_LINEAR, GU_LINEAR);
			sceGuTexScale(pData->textureMatrix.GetXAxis().Magnitude3(), pData->textureMatrix.GetYAxis().Magnitude3());
			sceGuTexOffset(pData->textureMatrix.GetTrans().x, pData->textureMatrix.GetTrans().y);
//			sceGuSetMatrix(GU_TEXTURE, (ScePspFMatrix4*)&pData->textureMatrix);
		}
		else
		{
			sceGuSetStatus(GU_TEXTURE_2D, GU_FALSE);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				sceGuSetStatus(GU_BLEND, GU_FALSE);
				break;
			case MF_AlphaBlend:
				sceGuSetStatus(GU_BLEND, GU_TRUE);
				sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
				break;
			case MF_Additive:
				sceGuSetStatus(GU_BLEND, GU_TRUE);
				sceGuBlendFunc(GU_ADD, GU_SRC_COLOR, GU_DST_COLOR, 0, 0);
				break;
			case MF_Subtractive:
				sceGuSetStatus(GU_BLEND, GU_TRUE);
				sceGuBlendFunc(GU_SUBTRACT, GU_SRC_COLOR, GU_DST_COLOR, 0, 0);
				break;
		}
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

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFDebug_Assert(pMaterial, "NULL Material!");
	MFDebug_Assert(pMaterial->pInstanceData, MFStr("Material '%s' already destroyed!", pMaterial->pName));

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	MFHeap_Free(pMaterial->pInstanceData);
	pMaterial->pInstanceData = NULL;
}
