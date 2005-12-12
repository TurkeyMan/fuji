#include "Common.h"
#include "Texture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "View_Internal.h"

#include "../../Source/Materials/MFMat_Standard.h"

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
