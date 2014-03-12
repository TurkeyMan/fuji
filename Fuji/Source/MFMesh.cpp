#include "Fuji_Internal.h"
#include "MFMesh_Internal.h"
#include "MFVertex.h"
#include "Util.h"

void MFMesh_FixUpMeshChunkGeneric(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFCALLSTACK;

	MFDebug_Assert(pMeshChunk->type == MFMCT_Generic, "Not a Generic type meshchunk!!");

	MFMeshChunk_Generic *pMC = (MFMeshChunk_Generic*)pMeshChunk;

	MFFixUp(pMC->pMaterial, pBase, load);
	MFFixUp(pMC->pIndexData, pBase, load);
	MFFixUp(pMC->pBatchIndices, pBase, load);

	if(load)
	{
		MFFixUp(pMC->pElements, pBase, 1);
		MFFixUp(pMC->ppVertexStreams, pBase, 1);
	}

	for(int b=0; b<pMC->numVertexStreams; ++b)
		MFFixUp(pMC->ppVertexStreams[b], pBase, load);

	if(!load)
	{
		MFFixUp(pMC->pElements, pBase, 0);
		MFFixUp(pMC->ppVertexStreams, pBase, 0);
	}
}
