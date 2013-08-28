#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PS2

#include "MFModel_Internal.h"
#include "MFView.h"
#include "MFRenderer.h"

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	MFFixUp(pMC->pMaterial, pBase, load);
	MFFixUp(pMC->pDMAList, pBase, load);
}

#endif
