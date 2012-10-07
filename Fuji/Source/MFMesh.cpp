#include "Fuji.h"
#include "MFMesh_Internal.h"

#include "MFVertex.h"

static MFVertexDataFormat formatMappings[MFMVDT_Max] =
{
	MFVDF_Float1,		//MFMVDT_Float1,
	MFVDF_Float2,		//MFMVDT_Float2,
	MFVDF_Float3,		//MFMVDT_Float3,
	MFVDF_Float4,		//MFMVDT_Float4,
	MFVDF_UByte4N_BGRA,	//MFMVDT_ColourBGRA,
	MFVDF_UByte4_RGBA,	//MFMVDT_UByte4,
	MFVDF_UByte4N_RGBA,	//MFMVDT_UByte4N,
	MFVDF_SShort2,		//MFMVDT_Short2,
	MFVDF_SShort4,		//MFMVDT_Short4,
	MFVDF_SShort2N,		//MFMVDT_Short2N,
	MFVDF_SShort4N,		//MFMVDT_Short4N,
	MFVDF_UShort2N,		//MFMVDT_UShort2N,
	MFVDF_UShort4N,		//MFMVDT_UShort4N,
	MFVDF_UByte4_RGBA,	//MFMVDT_UDec3,
	MFVDF_UByte4_RGBA,	//MFMVDT_Dec3N,
	MFVDF_Float16_2,	//MFMVDT_Float16_2,
	MFVDF_Float16_4,	//MFMVDT_Float16_4,
};

MFVertexDataFormat MFMesh_ConvertMeshVertexDataType(MFMeshVertexDataType type)
{
	return formatMappings[type];
}

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
