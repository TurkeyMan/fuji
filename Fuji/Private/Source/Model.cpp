#include "Common.h"
#include "Display.h"
#include "Model.h"
#include "Animation.h"
#include "Material.h"
#include "Renderer.h"
#include "FileSystem.h"

Model* Model::Create(char *pFilename)
{
	CALLSTACK;

	char *pModelData = File_Load(pFilename);
	if(!pModelData) return NULL;

	Model *pModel = new Model;

	pModel->pModelData = (ModelData*)pModelData;
	pModel->pModelData->FixUpPointers();

	for(int a=0; a<pModel->pModelData->materialCount; a++)
	{
		pModel->pModelData->pMaterials[a].pMaterial = Material::Create(pModel->pModelData->pMaterials[a].pName);
	}

#if defined(_WINDOWS)
	char *pData;

	pd3dDevice->CreateVertexBuffer(sizeof(FileVertex)*pModel->pModelData->vertexCount, 0, FileVertex::FVF, D3DPOOL_MANAGED, &pModel->pVertexBuffer, NULL);
	pModel->pVertexBuffer->Lock(0, 0, (void**)&pData, 0);
	memcpy(pData, pModel->pModelData->pVertexData, sizeof(FileVertex)*pModel->pModelData->vertexCount);
	pModel->pVertexBuffer->Unlock();

	pd3dDevice->CreateIndexBuffer(sizeof(int)*pModel->pModelData->indexCount, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &pModel->pIndexBuffer, NULL);
	pModel->pIndexBuffer->Lock(0, 0, (void**)&pData, 0);
	memcpy(pData, pModel->pModelData->pIndexData, sizeof(int)*pModel->pModelData->indexCount);
	pModel->pIndexBuffer->Unlock();

#endif

	pModel->worldMatrix.SetIdentity();

	return pModel;
}

void Model::Draw()
{
	CALLSTACKc;
/*
	pd3dDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(FileVertex));
	pd3dDevice->SetFVF(FileVertex::FVF);
	pd3dDevice->SetIndices(pIndexBuffer);

	for(int a=0; a<pModelData->subobjectCount; a++)
	{
		pModelData->pMaterials[pModelData->pSubobjects[a].materialIndex].pMaterial->Use();
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, pModelData->pSubobjects[a].indexOffset, 0, pModelData->pSubobjects[a].indexCount, 0, pModelData->pSubobjects[a].indexCount/3);
	}
*/
}

void ModelData::FixUpPointers()
{
	CALLSTACK;

	uint32 base = (uint32)((char*)this);

	pName += base;
	*((char**)&pMaterials) += base;
	*((char**)&pSubobjects) += base;
	*((char**)&pCustomData) += base;
	pVertexData += base;
	pIndexData += base;

	for(int a=0; a<materialCount; a++)
	{
		pMaterials[a].pName += base;
		pMaterials[a].pMaterialDescription += base;
	}
}

void ModelData::CollapsePointers()
{
	CALLSTACK;

	uint32 base = (uint32)((char*)this);

	for(int a=0; a<materialCount; a++)
	{
		pMaterials[a].pName -= base;
		pMaterials[a].pMaterialDescription -= base;
	}

	pName -= base;
	*((char**)&pMaterials) -= base;
	*((char**)&pSubobjects) -= base;
	*((char**)&pCustomData) -= base;
	pVertexData -= base;
	pIndexData -= base;
}

