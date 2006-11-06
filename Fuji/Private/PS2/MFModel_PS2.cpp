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
/*
	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
*/
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;
/*
	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
*/
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load)
{
	MFCALLSTACK;
/*
	MFMeshChunk_PS2 *pMC = (MFMeshChunk_PS2*)pMeshChunk;

	if(load)
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial + base);
		pMC->pVertexData += base;
	}
	else
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial - base);
		pMC->pVertexData -= base;
	}
*/
}
