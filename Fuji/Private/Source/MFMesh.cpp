#include "Fuji.h"
#include "MFMesh_Internal.h"

#include "MFVertex.h"

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
		MFFixUp(pMC->pVertexFormat, pBase, 1);
		MFFixUp(pMC->pVertexFormat->pStreams, pBase, 1);
		MFFixUp(pMC->ppVertexStreams, pBase, 1);
	}

	for(int b=0; b<pMC->pVertexFormat->numVertexStreams; ++b)
	{
		if(pMC->pVertexFormat->pStreams[b].pStreamName)
			MFFixUp(pMC->pVertexFormat->pStreams[b].pStreamName, pBase, load);
		MFFixUp(pMC->pVertexFormat->pStreams[b].pElements, pBase, load);
		MFFixUp(pMC->ppVertexStreams[b], pBase, load);
	}

	if(!load)
	{
		MFFixUp(pMC->pVertexFormat->pStreams, pBase, 0);
		MFFixUp(pMC->pVertexFormat, pBase, 0);
		MFFixUp(pMC->ppVertexStreams, pBase, 0);
	}
}
