#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_PSP

#include "MFMesh_Internal.h"
#include "MFModel_Internal.h"
#include "MFView.h"
#include "MFRenderer.h"

#include <pspgu.h>

void MFModel_InitModulePlatformSpecific()
{
}

void MFModel_DeinitModulePlatformSpecific()
{
}

MF_API void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	sceGuSetMatrix(GU_MODEL, (ScePspFMatrix4*)&pModel->worldMatrix);
	sceGuSetMatrix(GU_PROJECTION, (ScePspFMatrix4*)&MFView_GetViewToScreenMatrix());

	if(MFView_IsOrtho())
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFMatrix::identity);
	else
		sceGuSetMatrix(GU_VIEW, (ScePspFMatrix4*)&MFView_GetWorldToViewMatrix());

	MFMaterial *pMatOverride = (MFMaterial*)MFRenderer_GetRenderStateOverride(MFRS_MaterialOverride);

	if(pMatOverride)
		MFMaterial_SetMaterial(pMatOverride);

	MFModelDataChunk *pChunk =	MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		MFModelSubObject *pSubobjects = (MFModelSubObject*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)MFModel_GetMeshChunkInternal(pModel->pTemplate, a, b);

				if(!pMatOverride)
					MFMaterial_SetMaterial(pMC->pMaterial);

				MFRenderer_Begin();

				sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_NORMAL_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, pMC->numVertices, 0, pMC->pVertexData);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, void *pBase, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_PSP *pMC = (MFMeshChunk_PSP*)pMeshChunk;

	MFFixUp(pMC->pMaterial, pBase, load);
	MFFixUp(pMC->pVertexData, pBase, load);
}


#endif
