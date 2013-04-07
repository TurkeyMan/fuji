#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_D3D11
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_D3D11
	#define MFVertex_ChoooseVertexDataTypePlatformSpecific MFVertex_ChoooseVertexDataTypePlatformSpecific_D3D11
	#define MFVertex_CreateVertexDeclarationPlatformSpecific MFVertex_CreateVertexDeclarationPlatformSpecific_D3D11
	#define MFVertex_DestroyVertexDeclarationPlatformSpecific MFVertex_DestroyVertexDeclarationPlatformSpecific_D3D11
	#define MFVertex_CreateVertexBufferPlatformSpecific MFVertex_CreateVertexBufferPlatformSpecific_D3D11
	#define MFVertex_DestroyVertexBufferPlatformSpecific MFVertex_DestroyVertexBufferPlatformSpecific_D3D11
	#define MFVertex_LockVertexBuffer MFVertex_LockVertexBuffer_D3D11
	#define MFVertex_UnlockVertexBuffer MFVertex_UnlockVertexBuffer_D3D11
	#define MFVertex_CreateIndexBufferPlatformSpecific MFVertex_CreateIndexBufferPlatformSpecific_D3D11
	#define MFVertex_DestroyIndexBufferPlatformSpecific MFVertex_DestroyIndexBufferPlatformSpecific_D3D11
	#define MFVertex_LockIndexBuffer MFVertex_LockIndexBuffer_D3D11
	#define MFVertex_UnlockIndexBuffer MFVertex_UnlockIndexBuffer_D3D11
	#define MFVertex_SetVertexDeclaration MFVertex_SetVertexDeclaration_D3D11
	#define MFVertex_SetVertexStreamSource MFVertex_SetVertexStreamSource_D3D11
	#define MFVertex_SetIndexBuffer MFVertex_SetIndexBuffer_D3D11
	#define MFVertex_RenderVertices MFVertex_RenderVertices_D3D11
	#define MFVertex_RenderIndexedVertices MFVertex_RenderIndexedVertices_D3D11
#endif

#include "MFVector.h"
#include "MFHeap.h"
#include "MFVertex_Internal.h"
#include "MFDebug.h"
#include "MFMesh_Internal.h"
#include "MFRenderer_D3D11.h"


extern int gVertexDataStride[MFVDF_Max];

extern const uint8 *g_pVertexShaderData;
extern uint32 g_vertexShaderSize;

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

static const char* s_SemanticName[MFVET_Max] =
{
	"POSITION",		// MFVE_Position,
	"NORMAL",		// MFVE_Normal
	"COLOR",		// MFVE_Colour
	"TEXCOORD",		// MFVE_TexCoord
	"BINORMAL",		// MFVE_Binormal
	"TANGENT",		// MFVE_Tangent
	"BLENDINDICES", // MFVE_Indices
	"BLENDWEIGHT",	// MFVE_Weights
};

static const DXGI_FORMAT s_MFVDF_To_DXGI[MFVDF_Max] =
{
	(DXGI_FORMAT)-1,				// MFVDF_Auto
	DXGI_FORMAT_R32G32B32A32_FLOAT, // MFVDF_Float4
	DXGI_FORMAT_R32G32B32_FLOAT,	// MFVDF_Float3
	DXGI_FORMAT_R32G32_FLOAT,		// MFVDF_Float2
	DXGI_FORMAT_R32_FLOAT,			// MFVDF_Float1
	DXGI_FORMAT_R8G8B8A8_UINT,		// MFVDF_UByte4_RGBA
	DXGI_FORMAT_R8G8B8A8_UNORM,		// MFVDF_UByte4N_RGBA
	DXGI_FORMAT_B8G8R8A8_UNORM,		// MFVDF_UByte4N_BGRA
	DXGI_FORMAT_R16G16B16A16_SINT,	// MFVDF_SShort4
	DXGI_FORMAT_R16G16_SINT,		// MFVDF_SShort2
	DXGI_FORMAT_R16G16B16A16_SNORM, // MFVDF_SShort4N
	DXGI_FORMAT_R16G16_SNORM,		// MFVDF_SShort2N
	DXGI_FORMAT_R16G16B16A16_UINT,	// MFVDF_UShort4
	DXGI_FORMAT_R32G32_UINT,		// MFVDF_UShort2
	DXGI_FORMAT_R16G16B16A16_UNORM, // MFVDF_UShort4N
	DXGI_FORMAT_R16G16_UNORM,		// MFVDF_UShort2N
	DXGI_FORMAT_R16G16B16A16_FLOAT,	// MFVDF_Float16_4
	DXGI_FORMAT_R16G16_FLOAT,		// MFVDF_Float16_2
	(DXGI_FORMAT)-1,				// MFVDF_UDec3
	(DXGI_FORMAT)-1					// MFVDF_Dec3N
};

static const D3D11_PRIMITIVE_TOPOLOGY gPrimTopology[MFPT_Max] =
{
	D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,		// MFVPT_Points
	D3D11_PRIMITIVE_TOPOLOGY_LINELIST,		// MFVPT_LineList
	D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,		// MFVPT_LineStrip
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,	// MFVPT_TriangleList
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,	// MFVPT_TriangleStrip
	D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,		// MFVPT_TriangleFan
};

static const D3D11_PRIMITIVE gPrimType[MFPT_Max] =
{
	D3D11_PRIMITIVE_POINT,		// MFVPT_Points
	D3D11_PRIMITIVE_LINE,		// MFVPT_LineList
	D3D11_PRIMITIVE_LINE,		// MFVPT_LineStrip
	D3D11_PRIMITIVE_TRIANGLE,	// MFVPT_TriangleList
	D3D11_PRIMITIVE_TRIANGLE,	// MFVPT_TriangleStrip
	D3D11_PRIMITIVE_UNDEFINED,	// MFVPT_TriangleFan
};

MFVertexDataFormat MFVertexD3D11_ChoooseDataType(MFVertexElementType elementType, int components)
{
	MFDebug_Assert((components >= 0) && (components <= 4), "Invalid number of components");

	const MFVertexDataFormat floatComponents[5] = { MFVDF_Unknown, MFVDF_Float1, MFVDF_Float2, MFVDF_Float3, MFVDF_Float4 };
	switch(elementType)
	{
		case MFVET_Colour:
		case MFVET_Weights:
			return MFVDF_UByte4N_RGBA;
		case MFVET_Indices:
			return MFVDF_UByte4_RGBA;
		default:
			break;
	}
	// everything else is a float for now...
	return floatComponents[components];
}

void MFVertex_InitModulePlatformSpecific()
{
}

void MFVertex_DeinitModulePlatformSpecific()
{
}

MFVertexDataFormat MFVertex_ChoooseVertexDataTypePlatformSpecific(MFVertexElementType elementType, int components)
{
	const MFVertexDataFormat floatComponents[5] = { MFVDF_Unknown, MFVDF_Float1, MFVDF_Float2, MFVDF_Float3, MFVDF_Float4 };
	switch(elementType)
	{
		case MFVET_Colour:
		case MFVET_Weights:
			return MFVDF_UByte4N_RGBA;
		case MFVET_Indices:
			return MFVDF_UByte4_RGBA;
		default:
			break;
	}
	// everything else is a float for now...
	return floatComponents[components];
}

bool MFVertex_CreateVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{
	MFVertexElement *pElements = pDeclaration->pElements;
	MFVertexElementData *pElementData = pDeclaration->pElementData;

	D3D11_INPUT_ELEMENT_DESC elements[32];
	for(int a=0; a<pDeclaration->numElements; ++a)
	{
		MFDebug_Assert(s_MFVDF_To_DXGI[pElements[a].format] != (DXGI_FORMAT)-1, "Invalid vertex data format!");

		elements[a].SemanticName = s_SemanticName[pElements[a].type];
		elements[a].SemanticIndex = (UINT)pElements[a].index;
		elements[a].Format = s_MFVDF_To_DXGI[pElements[a].format];
		elements[a].InputSlot = (UINT)pElements[a].stream;
		elements[a].AlignedByteOffset = (UINT)pElementData[a].offset;
		elements[a].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elements[a].InstanceDataStepRate = 0;
	}
	
	// this needs the vertex shader
	ID3D11InputLayout* pVertexLayout = NULL;
	HRESULT hr = g_pd3dDevice->CreateInputLayout(elements, pDeclaration->numElements, g_pVertexShaderData, g_vertexShaderSize, &pVertexLayout);
	if (FAILED(hr))
		return false;

	pDeclaration->pPlatformData = pVertexLayout;

	return true;
}

void MFVertex_DestroyVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{
	ID3D11InputLayout *pVertexLayout = (ID3D11InputLayout*)pDeclaration->pPlatformData;
	pVertexLayout->Release();
}

bool MFVertex_CreateVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer, void *pVertexBufferMemory)
{
	if(pVertexBuffer->bufferType == MFVBType_Dynamic || pVertexBufferMemory)
	{
		D3D11_BUFFER_DESC bd;
		MFZeroMemory(&bd, sizeof(bd));
		bd.Usage = (pVertexBuffer->bufferType == MFVBType_Dynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = pVertexBuffer->pVertexDeclatation->pElementData[0].stride * pVertexBuffer->numVerts;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = (pVertexBuffer->bufferType == MFVBType_Dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;

		D3D11_SUBRESOURCE_DATA InitData;
		MFZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pVertexBufferMemory;
	
		ID3D11Buffer* pVB = NULL;
		HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, pVertexBufferMemory ? &InitData : NULL, &pVB);
		if(FAILED(hr))
			return false;
	
		pVertexBuffer->pPlatformData = pVB;
	}

	return true;
}

void MFVertex_DestroyVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer)
{
	ID3D11Buffer *pVB = (ID3D11Buffer*)pVertexBuffer->pPlatformData;
	if(pVB)
		pVB->Release();
}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer, void **ppVertices)
{
	MFDebug_Assert(pVertexBuffer, "Null vertex buffer");
	MFDebug_Assert(!pVertexBuffer->bLocked, "Vertex buffer already locked!");

	if(pVertexBuffer->bufferType == MFVBType_Dynamic)
	{
		ID3D11Buffer *pVB = (ID3D11Buffer*)pVertexBuffer->pPlatformData;

		D3D11_MAPPED_SUBRESOURCE subresource;
		D3D11_MAP map = (pVertexBuffer->bufferType == MFVBType_Dynamic) ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE;
		HRESULT hr = g_pImmediateContext->Map(pVB, 0, map, D3D11_MAP_FLAG_DO_NOT_WAIT, &subresource);

		if(hr == DXGI_ERROR_WAS_STILL_DRAWING)
		{
			MFDebug_Message("waiting on vertex buffer lock");
			hr = g_pImmediateContext->Map(pVB, 0, map, 0, &subresource);
		}

		MFDebug_Assert(SUCCEEDED(hr), "Failed to map vertex buffer");

		pVertexBuffer->pLocked = subresource.pData;
	}
	else
	{
		pVertexBuffer->pLocked = MFHeap_Alloc(pVertexBuffer->numVerts*pVertexBuffer->pVertexDeclatation->pElementData[0].stride, MFHeap_GetHeap(MFHT_ActiveTemporary));
	}

	if(ppVertices)
		*ppVertices = pVertexBuffer->pLocked;

	pVertexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(pVertexBuffer, "Null vertex buffer");

	ID3D11Buffer *pVB = (ID3D11Buffer*)pVertexBuffer->pPlatformData;
	if(pVertexBuffer->bufferType == MFVBType_Dynamic)
	{
		g_pImmediateContext->Unmap(pVB, 0);
	}
	else
	{
		if(pVB)
			pVB->Release();

		D3D11_BUFFER_DESC bd;
		MFZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = pVertexBuffer->pVertexDeclatation->pElementData[0].stride * pVertexBuffer->numVerts;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData;
		MFZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = pVertexBuffer->pLocked;
	
		HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &pVB);
		MFDebug_Assert(SUCCEEDED(hr), "Couldn't create vertex buffer!");
		if(FAILED(hr))
		{
			pVertexBuffer->pPlatformData = NULL;
			return;
		}

		pVertexBuffer->pPlatformData = pVB;

		MFHeap_Free(pVertexBuffer->pLocked);
	}

	pVertexBuffer->pLocked = NULL;
	pVertexBuffer->bLocked = false;
}

bool MFVertex_CreateIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer, uint16 *pIndexBufferMemory)
{
	if(pIndexBufferMemory)
	{
		D3D11_BUFFER_DESC bd;
		MFZeroMemory(&bd, sizeof(bd));
	
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(WORD) * pIndexBuffer->numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
		D3D11_SUBRESOURCE_DATA initData;
		MFZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = pIndexBufferMemory;

		ID3D11Buffer *pIB = NULL;
		HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &pIB);
		if (FAILED(hr))
			return false;

		pIndexBuffer->pPlatformData = pIB;
	}

	return true;
}

void MFVertex_DestroyIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer)
{
	ID3D11Buffer *pIB = (ID3D11Buffer*)pIndexBuffer->pPlatformData;
	if(pIB)
		pIB->Release();
}

MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
	MFDebug_Assert(pIndexBuffer, "NULL index buffer");
	MFDebug_Assert(!pIndexBuffer->bLocked, "Index buffer already locked!");

	pIndexBuffer->pLocked = MFHeap_Alloc(sizeof(uint16)*pIndexBuffer->numIndices, MFHeap_GetHeap(MFHT_ActiveTemporary));

	if(ppIndices)
		*ppIndices = (uint16*)pIndexBuffer->pLocked;

	pIndexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	MFDebug_Assert(pIndexBuffer, "NULL index buffer");
	MFDebug_Assert(pIndexBuffer->bLocked, "Index buffer not locked!");

	ID3D11Buffer *pIB = (ID3D11Buffer*)pIndexBuffer->pPlatformData;
	if(pIB)
		pIB->Release();

	D3D11_BUFFER_DESC bd;
	MFZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(WORD) * pIndexBuffer->numIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA initData;
	MFZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = pIndexBuffer->pLocked;

	HRESULT hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &pIB);
	MFDebug_Assert(SUCCEEDED(hr), "Couldn't create index buffer!");
	if (FAILED(hr))
		return;

	pIndexBuffer->pPlatformData = pIB;

	MFHeap_Free(pIndexBuffer->pLocked);

	pIndexBuffer->pLocked = NULL;
	pIndexBuffer->bLocked = false;
}

MF_API void MFVertex_SetVertexDeclaration(const MFVertexDeclaration *pVertexDeclaration)
{
	ID3D11InputLayout *pVertexLayout = pVertexDeclaration ? (ID3D11InputLayout*)pVertexDeclaration->pPlatformData : NULL;
    g_pImmediateContext->IASetInputLayout(pVertexLayout);
}

MF_API void MFVertex_SetVertexStreamSource(int stream, const MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(pVertexBuffer, "Null vertex buffer");

	ID3D11Buffer *pVB = (ID3D11Buffer*)pVertexBuffer->pPlatformData;
	UINT stride = pVertexBuffer->pVertexDeclatation->pElementData[0].stride;
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(stream, 1, &pVB, &stride, &offset);
}

MF_API void MFVertex_SetIndexBuffer(const MFIndexBuffer *pIndexBuffer)
{
	ID3D11Buffer *pIB = (ID3D11Buffer*)pIndexBuffer->pPlatformData;
	g_pImmediateContext->IASetIndexBuffer(pIB, DXGI_FORMAT_R16_UINT, 0);
}

MF_API void MFVertex_RenderVertices(MFPrimType primType, int firstVertex, int numVertices)
{
    g_pImmediateContext->IASetPrimitiveTopology(gPrimTopology[primType]);
	g_pImmediateContext->Draw(numVertices, firstVertex);
}

MF_API void MFVertex_RenderIndexedVertices(MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices)
{
    g_pImmediateContext->IASetPrimitiveTopology(gPrimTopology[primType]);
    g_pImmediateContext->DrawIndexed(numIndices, indexOffset, vertexOffset);
}

#endif // MF_RENDERER
