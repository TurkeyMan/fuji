#include "Fuji.h"
#include "MFMaterial_Internal.h"

int Mat_Standard_RegisterMaterial(void *pPlatformData)
{
	return 0;
}

void Mat_Standard_UnregisterMaterial()
{
}

int Mat_Standard_Begin(MFMaterial *pMaterial)
{
	return 0;
}

void Mat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	pMaterial->pInstanceData = NULL;
}

void Mat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
}
