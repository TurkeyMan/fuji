#include "Fuji.h"
#include "MFModel_Internal.h"
#include "MFView.h"
#include "MFRenderer.h"

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;
}

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

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	MFFixUp(pMC->pMaterial, (void*)base, load);
	MFFixUp(pMC->pDMAList, (void*)base, load);
}

MFMeshChunk* MFModel_GetMeshChunkInternal(MFModelTemplate *pModelTemplate, int subobjectIndex, int meshChunkIndex)
{
	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModelTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFDebug_Assert(subobjectIndex < pChunk->count, "Subobject index out of bounds.");
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		MFDebug_Assert(meshChunkIndex < pSubobjects->numMeshChunks, "Mesh chunk index out of bounds.");
		MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pSubobjects[subobjectIndex].pMeshChunks;
		return &pMC[meshChunkIndex];
	}

	return NULL;
}
