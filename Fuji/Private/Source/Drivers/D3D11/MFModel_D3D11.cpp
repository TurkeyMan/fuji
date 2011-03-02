#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFModel_InitModulePlatformSpecific MFModel_InitModulePlatformSpecific_D3D11
	#define MFModel_DeinitModulePlatformSpecific MFModel_DeinitModulePlatformSpecific_D3D11
	#define MFModel_Draw MFModel_Draw_D3D11
	#define MFModel_CreateMeshChunk MFModel_CreateMeshChunk_D3D11
	#define MFModel_DestroyMeshChunk MFModel_DestroyMeshChunk_D3D11
	#define MFModel_FixUpMeshChunk MFModel_FixUpMeshChunk_D3D11
#endif

#include "MFModel_Internal.h"
#include "MFMesh_Internal.h"

void MFModel_InitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFModel_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}

#endif // MF_RENDERER