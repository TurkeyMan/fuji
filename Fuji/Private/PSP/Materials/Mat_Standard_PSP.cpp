#include "Common.h"
#include "Texture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "View_Internal.h"

#include "../../Source/Materials/Mat_Standard.h"

#include <pspdisplay.h>
#include <pspgu.h>

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

	sceGuAmbientColor(0xFFFFFFFF);

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			Texture *pTexture = pData->pTextures[pData->diffuseMapIndex];

			sceGuSetStatus(GU_TEXTURE_2D, GU_TRUE);

			sceGuTexMode(pTexture->format, 0, 0, 0);
			sceGuTexImage(0, pTexture->width, pTexture->height, pTexture->width, pTexture->pImageData);
			sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
			sceGuTexFilter(GU_LINEAR, GU_LINEAR);
			sceGuTexScale(pData->textureMatrix.GetXAxis3().Magnitude(), pData->textureMatrix.GetYAxis3().Magnitude());
			sceGuTexOffset(pData->textureMatrix.GetTrans3().x, pData->textureMatrix.GetTrans3().y);
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
