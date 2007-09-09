#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_NULL

#include "MFModel_Internal.h"

void MFModel_InitModulePlatformSpecific()
{
}

void MFModel_DeinitModulePlatformSpecific()
{
}

void MFModel_Draw(MFModel *pModel)
{
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
}

#endif
