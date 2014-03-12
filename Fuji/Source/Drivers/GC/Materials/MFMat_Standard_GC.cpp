#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_GC

#include "MFMaterial_Internal.h"

int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
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
