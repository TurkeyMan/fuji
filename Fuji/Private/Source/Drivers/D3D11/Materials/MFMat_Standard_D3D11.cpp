#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D11
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D11
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D11
	#define MFMat_Standard_CreateInstance MFMat_Standard_CreateInstance_D3D11
	#define MFMat_Standard_DestroyInstance MFMat_Standard_DestroyInstance_D3D11
#endif

#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Standard.h"
#include "MFHeap.h"


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

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));
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

#endif // MF_RENDERER
