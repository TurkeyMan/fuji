#include "Fuji.h"

#if MF_RENDERER == NULL

#include "MFMaterial_Internal.h"

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	pMaterial->pInstanceData = NULL;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
}

#endif
