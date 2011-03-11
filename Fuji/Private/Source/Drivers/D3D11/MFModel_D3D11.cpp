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

#include "MFVertex.h"

#include "MFRenderer_D3D11.h"


//---------------------------------------------------------------------------------------------------------------------
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFVertexDataFormat format);
DXGI_FORMAT MFRenderer_D3D11_GetFormat(MFMeshVertexDataType format);
const char* MFRenderer_D3D11_GetSemanticName(MFVertexElementType type);
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
struct MeshChunkRuntimeDataD3D11
{
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *animBuffer;
	ID3D11Buffer *indexBuffer;
	ID3D11Buffer *vertexDecl;
};
//---------------------------------------------------------------------------------------------------------------------
void MFModel_InitModulePlatformSpecific()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_DeinitModulePlatformSpecific()
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_Draw(MFModel *pModel)
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
HRESULT MFModel_D3D1_CreateVertexDeclaration(MFMeshVertexFormat *pMVF, ID3D11InputLayout** ppDecl)
{
	D3D11_INPUT_ELEMENT_DESC elements[32];
	int element = 0;

	for (int a = 0; a < pMVF->numVertexStreams; ++a)
	{
		for (int b = 0; b < pMVF->pStreams[a].numVertexElements; ++b)
		{
			MFMeshVertexElement &rElement = pMVF->pStreams[a].pElements[b];

			elements[element].SemanticName = MFRenderer_D3D11_GetSemanticName(rElement.usage);
			elements[element].SemanticIndex = rElement.usageIndex;
			elements[element].Format = MFRenderer_D3D11_GetFormat(rElement.type);
			elements[element].InputSlot = a;
			elements[element].AlignedByteOffset = pMVF->pStreams[a].pElements[b].offset;
			elements[element].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elements[element].InstanceDataStepRate = 0;
			++element;

			MFDebug_Assert(element < 32, "whoops");
		}
	}

	return NULL;

	//return pd3dDevice->CreateVertexDeclaration(elements, ppDecl);
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_CreateMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_DestroyMeshChunk(MFMeshChunk *pMeshChunk)
{
	MFCALLSTACK;
}
//---------------------------------------------------------------------------------------------------------------------
void MFModel_FixUpMeshChunk(MFMeshChunk *pMC, void *pBase, bool load)
{
	MFMesh_FixUpMeshChunkGeneric(pMC, pBase, load);
}
//---------------------------------------------------------------------------------------------------------------------
#endif // MF_RENDERER