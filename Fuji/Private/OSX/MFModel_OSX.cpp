#include "Fuji.h"
#include "MFModel_Internal.h"

void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;

	MFMatrix localToView;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)&MFView_GetViewToScreenMatrix());

	glMatrixMode(GL_MODELVIEW);
	if(MFView_IsOrtho())
		glLoadMatrixf((GLfloat*)&pModel->worldMatrix);
	else
		glLoadMatrixf((GLfloat*)&MFView_GetLocalToView(pModel->worldMatrix, &localToView));

	MFModelDataChunk *pChunk = MFModel_GetDataChunk(pModel->pTemplate, MFChunkType_SubObjects);

	if(pChunk)
	{
		SubObjectChunk *pSubobjects = (SubObjectChunk*)pChunk->pData;

		for(int a=0; a<pChunk->count; a++)
		{
			for(int b=0; b<pSubobjects[a].numMeshChunks; b++)
			{
				MFMeshChunk_Linux *pMC = (MFMeshChunk_Linux*)&pSubobjects[a].pMeshChunks[b];

				MFMaterial_SetMaterial(pMC->pMaterial);
				MFRenderer_Begin();

				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);

				glVertexPointer(3, GL_FLOAT, 0, pMC->pVertexData);
				glNormalPointer(GL_FLOAT, 0, pMC->pNormalData);
				glColorPointer(4, GL_UNSIGNED_BYTE, 0, pMC->pColorData);
				glTexCoordPointer(2, GL_FLOAT, 0, pMC->pUVData)

//				glLockArraysEXT(0, numberOfVertices);
				glDrawElements(GL_TRIANGLES, pMC->numIndices, GL_UNSIGNED_SHORT, pMC->pIndexData);
//				glUnlockArraysEXT();

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
}

void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Linux *pMC = (MFMeshChunk_Linux*)pMeshChunk;

	pMC->pMaterial = MFMaterial_Create((char*)pMC->pMaterial);
}

void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;

	MFMeshChunk_Linux *pMC = (MFMeshChunk_Linux*)pMeshChunk;

	MFMaterial_Destroy(pMC->pMaterial);
}

void MFModel_FixUpMeshChunk(MFMeshChunk *pMeshChunk, uint32 base, bool load)
{
	MFCALLSTACK;

	MFMeshChunk_Linux *pMC = (MFMeshChunk_Linux*)pMeshChunk;

	if(load)
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial + base);
		pMC->pVertexData += base;
		pMC->pNormalData += base;
		pMC->pColourData += base;
		pMC->pUVData += base;
		pMC->pIndexData += base;
		(char*&)pMC->pVertexElements += base;
	}
	else
	{
		pMC->pMaterial = (MFMaterial*)((char*)pMC->pMaterial - base);
		pMC->pVertexData -= base;
		pMC->pNormalData -= base;
		pMC->pColourData -= base;
		pMC->pUVData -= base;
		pMC->pIndexData -= base;
		(char*&)pMC->pVertexElements -= base;
	}
}
