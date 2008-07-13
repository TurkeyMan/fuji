#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9

#include "MFVector.h"
#include "MFHeap.h"
#include "MFVertex_Internal.h"
#include "MFRenderer.h"

#include <d3d9.h>

extern IDirect3DDevice9 *pd3dDevice;

extern int gVertexDataStride[MFVDF_Max];

static const BYTE gUsageSemantic[MFVE_Max] =
{
	D3DDECLUSAGE_POSITION, // MFVE_Position,
	D3DDECLUSAGE_NORMAL, // MFVE_Normal
	D3DDECLUSAGE_COLOR, // MFVE_Colour
	D3DDECLUSAGE_TEXCOORD, // MFVE_TexCoord
	D3DDECLUSAGE_BINORMAL, // MFVE_Binormal
	D3DDECLUSAGE_TANGENT, // MFVE_Tangent
	D3DDECLUSAGE_BLENDINDICES, // MFVE_Indices
	D3DDECLUSAGE_BLENDWEIGHT // MFVE_Weights
};

static const BYTE gDataType[MFVDF_Max] =
{
	D3DDECLTYPE_FLOAT4, // MFVDF_Float4
	D3DDECLTYPE_FLOAT3, // MFVDF_Float3
	D3DDECLTYPE_FLOAT2, // MFVDF_Float2
	D3DDECLTYPE_FLOAT1, // MFVDF_Float1
	D3DDECLTYPE_UBYTE4, // MFVDF_UByte4_RGBA
	D3DDECLTYPE_UBYTE4N, // MFVDF_UByte4N_RGBA
	D3DDECLTYPE_D3DCOLOR, // MFVDF_UByte4N_BGRA
	D3DDECLTYPE_SHORT4, // MFVDF_SShort4
	D3DDECLTYPE_SHORT2, // MFVDF_SShort2
	D3DDECLTYPE_SHORT4N, // MFVDF_SShort4N
	D3DDECLTYPE_SHORT2N, // MFVDF_SShort2N
	-1, // MFVDF_UShort4
	-1, // MFVDF_UShort2
	D3DDECLTYPE_USHORT4N, // MFVDF_UShort4N
	D3DDECLTYPE_USHORT2N, // MFVDF_UShort2N
	D3DDECLTYPE_FLOAT16_4, // MFVDF_Float16_4
	D3DDECLTYPE_FLOAT16_2 // MFVDF_Float16_2
	// D3DDECLTYPE_UDEC3
	// D3DDECLTYPE_DEC3N
};

static const D3DPRIMITIVETYPE gPrimTypes[] =
{
	D3DPT_POINTLIST, // MFVPT_Points
	D3DPT_LINELIST, // MFVPT_LineList
	D3DPT_LINESTRIP, // MFVPT_LineStrip
	D3DPT_TRIANGLELIST, // MFVPT_TriangleList
	D3DPT_TRIANGLESTRIP, // MFVPT_TriangleStrip
	D3DPT_TRIANGLEFAN, // MFVPT_TriangleFan
};

MFVertexDataFormat MFVertexD3D9_ChoooseDataType(MFVertexElementType elementType, int components)
{
	const MFVertexDataFormat floatComponents[5] = { MFVDF_Unknown, MFVDF_Float1, MFVDF_Float2, MFVDF_Float3, MFVDF_Float4 };
	switch(elementType)
	{
		case MFVE_Colour:
		case MFVE_Weights:
			return MFVDF_UByte4N_BGRA;	// we'd love to use the non-swizzled ones, but we want maximum backwards compatibility
		case MFVE_Indices:
			return MFVDF_UByte4_RGBA;
		default:
			break;
	}
	// everything else is a float for now...
	return floatComponents[components];
}

MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{
	MFVertexDeclaration *pDecl = (MFVertexDeclaration*)MFHeap_Alloc(sizeof(MFVertexDeclaration) + (sizeof(MFVertexElement)+sizeof(MFVertexElementData))*elementCount);
	pDecl->numElements = elementCount;
	pDecl->pElements = (MFVertexElement*)&pDecl[1];
	pDecl->pElementData = (MFVertexElementData*)&pDecl->pElements[elementCount];
	pDecl->pPlatformData = NULL;

	MFCopyMemory(pDecl->pElements, pElementArray, sizeof(MFVertexElement)*elementCount);
	MFZeroMemory(pDecl->pElementData, sizeof(MFVertexElementData)*elementCount);

	int streamOffsets[16];
	MFZeroMemory(streamOffsets, sizeof(streamOffsets));

	D3DVERTEXELEMENT9 elements[32];
	D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
	for(int a=0; a<elementCount; ++a)
	{
		MFVertexDataFormat dataFormat = MFVertexD3D9_ChoooseDataType(pElementArray[a].elementType, pElementArray[a].componentCount);
		elements[a].Stream = pElementArray[a].stream;
		elements[a].Offset = streamOffsets[pElementArray[a].stream];
		elements[a].Type = gDataType[dataFormat];
		elements[a].Method = D3DDECLMETHOD_DEFAULT;
		elements[a].Usage = gUsageSemantic[pElementArray[a].elementType];
		elements[a].UsageIndex = pElementArray[a].elementIndex;

		pDecl->pElementData[a].format = dataFormat;
		pDecl->pElementData[a].offset = streamOffsets[pElementArray[a].stream];
		pDecl->pElementData[a].stride = 0;
		pDecl->pElementData[a].pData = NULL;

		streamOffsets[pElementArray[a].stream] += gVertexDataStride[dataFormat];
	}
	elements[elementCount] = endMacro;

	// set the strides for each component
	for(int a=0; a<elementCount; ++a)
		pDecl->pElementData[a].stride = streamOffsets[pElementArray[a].stream];

	IDirect3DVertexDeclaration9 *pVertexDecl;
	HRESULT hr = pd3dDevice->CreateVertexDeclaration(elements, &pVertexDecl);
	if(FAILED(hr))
	{
		MFHeap_Free(pDecl);
		return NULL;
	}

	pDecl->pPlatformData = pVertexDecl;

	return pDecl;
}

void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{
	IDirect3DVertexDeclaration9 *pDecl = (IDirect3DVertexDeclaration9*)pDeclaration->pPlatformData;
	pDecl->Release();

	MFHeap_Free(pDeclaration);
}

MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory)
{
	DWORD usage = type == MFVBType_Static ? D3DUSAGE_WRITEONLY : D3DUSAGE_DYNAMIC;
	D3DPOOL pool = usage == D3DUSAGE_DYNAMIC ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;

	IDirect3DVertexBuffer9 *pVertBuffer;
	HRESULT hr = pd3dDevice->CreateVertexBuffer(pVertexFormat->pElementData[0].stride*numVerts, usage, 0, pool, &pVertBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer");
	if(FAILED(hr))
		return NULL;

	MFVertexBuffer *pVB = (MFVertexBuffer*)MFHeap_Alloc(sizeof(MFVertexBuffer));
	pVB->pVertexDeclatation = pVertexFormat;
	pVB->bufferType = type;
	pVB->numVerts = numVerts;
	pVB->bLocked = false;
	pVB->pPlatformData = pVertBuffer;

	if(pVertexBufferMemory)
	{
		void *pData;
		pVertBuffer->Lock(0, 0, &pData, D3DLOCK_DISCARD);
		MFCopyMemory(pData, pVertexBufferMemory, pVertexFormat->pElementData[0].stride*numVerts);
		pVertBuffer->Unlock();
	}

	return pVB;
}

void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	IDirect3DVertexBuffer9 *pVB = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	pVB->Release();

	MFHeap_Free(pVertexBuffer);
}

void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(!pVertexBuffer->bLocked, "Vertex buffer already locked!");

	IDirect3DVertexBuffer9 *pVB = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;

	void *pData;
	HRESULT hr = pVB->Lock(0, 0, &pData, pVertexBuffer->bufferType == MFVBType_Static ? 0 : D3DLOCK_DISCARD);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vertex buffer");

	for(int a=0; a<pVertexBuffer->pVertexDeclatation->numElements; ++a)
	{
		if(pVertexBuffer->pVertexDeclatation->pElements[a].stream == 0)
			pVertexBuffer->pVertexDeclatation->pElementData[a].pData = (char*)pData + pVertexBuffer->pVertexDeclatation->pElementData[a].offset;
		else
			pVertexBuffer->pVertexDeclatation->pElementData[a].pData = NULL;
	}

	pVertexBuffer->bLocked = true;
}

void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(pVertexBuffer->bLocked, "Vertex buffer already locked!");

	IDirect3DVertexBuffer9 *pVertBuffer = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;

	pVertBuffer->Unlock();

	pVertexBuffer->bLocked = false;
}

MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory)
{
	IDirect3DIndexBuffer9 *pIndexBuffer;
	HRESULT hr = pd3dDevice->CreateIndexBuffer(sizeof(uint16)*numIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndexBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer");
	if(FAILED(hr))
		return NULL;

	MFIndexBuffer *pIB = (MFIndexBuffer*)MFHeap_Alloc(sizeof(MFIndexBuffer) + (pIndexBufferMemory ? 0 : sizeof(uint16)*numIndices));
	pIB->pIndices = pIndexBufferMemory ? pIndexBufferMemory : (uint16*)&pIB[1];
	pIB->numIndices = numIndices;
	pIB->bLocked = false;
	pIB->pPlatformData = pIndexBuffer;

	if(pIndexBufferMemory)
	{
		void *pData;
		pIndexBuffer->Lock(0, 0, &pData, D3DLOCK_DISCARD);
		MFCopyMemory(pData, pIndexBufferMemory, sizeof(uint16)*numIndices);
		pIndexBuffer->Unlock();
	}

	return NULL;
}

void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;
	pIB->Release();

	MFHeap_Free(pIndexBuffer);
}

void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
	MFDebug_Assert(!pIndexBuffer->bLocked, "Index buffer already locked!");

	*ppIndices = pIndexBuffer->pIndices;

	pIndexBuffer->bLocked = true;
}

void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	MFDebug_Assert(pIndexBuffer->bLocked, "Index buffer already locked!");

	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;

	void *pData;
	pIB->Lock(0, 0, &pData, 0);
	MFCopyMemory(pData, pIndexBuffer->pIndices, sizeof(uint16)*pIndexBuffer->numIndices);
	pIB->Unlock();

	pIndexBuffer->bLocked = false;
}

void MFVertex_SetVertexDeclaration(MFVertexDeclaration *pVertexDeclaration)
{
	IDirect3DVertexDeclaration9 *pVertexDecl = (IDirect3DVertexDeclaration9*)pVertexDeclaration->pPlatformData;
	pd3dDevice->SetVertexDeclaration(pVertexDecl);
}

void MFVertex_SetVertexStreamSource(int stream, MFVertexBuffer *pVertexBuffer)
{
	IDirect3DVertexBuffer9 *pVertBuffer = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	pd3dDevice->SetStreamSource(stream, pVertBuffer, 0, pVertexBuffer->pVertexDeclatation->pElementData[0].stride);

	if(stream == 0)
		MFVertex_SetVertexDeclaration(pVertexBuffer->pVertexDeclatation);
}

static int MFVertex_GetNumPrims(MFVertexPrimType primType, int numVertices)
{
	switch(primType)
	{
		case MFVPT_Points:			return numVertices;
		case MFVPT_LineList:		return numVertices / 2;
		case MFVPT_LineStrip:		return numVertices - 1;
		case MFVPT_TriangleList:	return numVertices / 3;
		case MFVPT_TriangleStrip:	return numVertices - 2;
		case MFVPT_TriangleFan:		return numVertices - 2;
		default:
			MFDebug_Assert(false, "Unknown primitive type!");
	}
	return 0;
}

void MFVertex_RenderVertices(MFVertexPrimType primType, int firstVertex, int numVertices)
{
	pd3dDevice->DrawPrimitive(gPrimTypes[primType], firstVertex, MFVertex_GetNumPrims(primType, numVertices));
}

void MFVertex_RenderIndexedVertices(MFVertexPrimType primType, int numVertices, int numIndices, MFIndexBuffer *pIndexBuffer)
{
	pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData);
	pd3dDevice->DrawIndexedPrimitive(gPrimTypes[primType], 0, 0, numVertices, 0, MFVertex_GetNumPrims(primType, numVertices));
}

#endif // MF_RENDERER
