#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFVertex_InitModulePlatformSpecific MFVertex_InitModulePlatformSpecific_D3D9
	#define MFVertex_DeinitModulePlatformSpecific MFVertex_DeinitModulePlatformSpecific_D3D9
	#define MFVertex_ChoooseVertexDataTypePlatformSpecific MFVertex_ChoooseVertexDataTypePlatformSpecific_D3D9
	#define MFVertex_CreateVertexDeclarationPlatformSpecific MFVertex_CreateVertexDeclarationPlatformSpecific_D3D9
	#define MFVertex_DestroyVertexDeclarationPlatformSpecific MFVertex_DestroyVertexDeclarationPlatformSpecific_D3D9
	#define MFVertex_CreateVertexBufferPlatformSpecific MFVertex_CreateVertexBufferPlatformSpecific_D3D9
	#define MFVertex_DestroyVertexBufferPlatformSpecific MFVertex_DestroyVertexBufferPlatformSpecific_D3D9
	#define MFVertex_LockVertexBuffer MFVertex_LockVertexBuffer_D3D9
	#define MFVertex_UnlockVertexBuffer MFVertex_UnlockVertexBuffer_D3D9
	#define MFVertex_CreateIndexBufferPlatformSpecific MFVertex_CreateIndexBufferPlatformSpecific_D3D9
	#define MFVertex_DestroyIndexBufferPlatformSpecific MFVertex_DestroyIndexBufferPlatformSpecific_D3D9
	#define MFVertex_LockIndexBuffer MFVertex_LockIndexBuffer_D3D9
	#define MFVertex_UnlockIndexBuffer MFVertex_UnlockIndexBuffer_D3D9
	#define MFVertex_SetVertexDeclaration MFVertex_SetVertexDeclaration_D3D9
	#define MFVertex_SetVertexStreamSource MFVertex_SetVertexStreamSource_D3D9
	#define MFVertex_SetIndexBuffer MFVertex_SetIndexBuffer_D3D9
	#define MFVertex_RenderVertices MFVertex_RenderVertices_D3D9
	#define MFVertex_RenderIndexedVertices MFVertex_RenderIndexedVertices_D3D9
#endif

#include "MFVector.h"
#include "MFHeap.h"
#include "MFVertex_Internal.h"
#include "MFRenderer.h"
#include "MFRenderer_D3D9.h"

#include <d3d9.h>

extern IDirect3DDevice9 *pd3dDevice;

extern int gVertexDataStride[MFVDF_Max];

static const BYTE gUsageSemantic[MFVET_Max] =
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
	(BYTE)-1, // MFVDF_Auto
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
	(BYTE)-1, // MFVDF_UShort4
	(BYTE)-1, // MFVDF_UShort2
	D3DDECLTYPE_USHORT4N, // MFVDF_UShort4N
	D3DDECLTYPE_USHORT2N, // MFVDF_UShort2N
	D3DDECLTYPE_FLOAT16_4, // MFVDF_Float16_4
	D3DDECLTYPE_FLOAT16_2, // MFVDF_Float16_2
	D3DDECLTYPE_UDEC3, // MFVDF_UDec3
	D3DDECLTYPE_DEC3N // MFVDF_Dec3N
};

static const D3DPRIMITIVETYPE gPrimTypes[MFPT_Max] =
{
	D3DPT_POINTLIST, // MFVPT_Points
	D3DPT_LINELIST, // MFVPT_LineList
	D3DPT_LINESTRIP, // MFVPT_LineStrip
	D3DPT_TRIANGLELIST, // MFVPT_TriangleList
	D3DPT_TRIANGLESTRIP, // MFVPT_TriangleStrip
	D3DPT_TRIANGLEFAN, // MFVPT_TriangleFan
};

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
			return MFVDF_UByte4N_BGRA;	// we'd love to use the non-swizzled ones, but we want maximum backwards compatibility
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

	D3DVERTEXELEMENT9 elements[32];
	D3DVERTEXELEMENT9 endMacro = D3DDECL_END();
	for(int a=0; a<pDeclaration->numElements; ++a)
	{
		MFDebug_Assert(gDataType[pElements[a].format] != (BYTE)-1, "Invalid vertex data format!");

		elements[a].Stream = (uint16)pElements[a].stream;
		elements[a].Offset = (uint16)pElementData[a].offset;
		elements[a].Type = gDataType[pElements[a].format];
		elements[a].Method = D3DDECLMETHOD_DEFAULT;
		elements[a].Usage = gUsageSemantic[pElements[a].type];
		elements[a].UsageIndex = (uint8)pElements[a].index;
	}
	elements[pDeclaration->numElements] = endMacro;

	IDirect3DVertexDeclaration9 *pVertexDecl;
	HRESULT hr = pd3dDevice->CreateVertexDeclaration(elements, &pVertexDecl);
	if(FAILED(hr))
		return false;

	pDeclaration->pPlatformData = pVertexDecl;

	return true;
}

void MFVertex_DestroyVertexDeclarationPlatformSpecific(MFVertexDeclaration *pDeclaration)
{
	IDirect3DVertexDeclaration9 *pDecl = (IDirect3DVertexDeclaration9*)pDeclaration->pPlatformData;
	pDecl->Release();
}

bool MFVertex_CreateVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer, void *pVertexBufferMemory)
{
	DWORD usage;
	D3DPOOL pool;
	DWORD lockFlags = 0;
	switch(pVertexBuffer->bufferType)
	{
		case MFVBType_Static:
			pool = D3DPOOL_MANAGED;
			usage = 0;
			break;
		case MFVBType_Dynamic:
			pool = D3DPOOL_DEFAULT;
			usage = D3DUSAGE_WRITEONLY;
			usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
			lockFlags = D3DLOCK_DISCARD;
			break;
		case MFVBType_Scratch:
			// Maybe it's better to use UP draw calls for scratch buffers?
			pool = D3DPOOL_DEFAULT;
			usage = D3DUSAGE_WRITEONLY;
//			usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY; // DYNAMIC is supposedly only for frequent locking... we don't intend to lock more than once 
			break;
		default:
			MFDebug_Assert(false, "Invalid vertex buffer type!");
			return false;
	}

	int stride = pVertexBuffer->pVertexDeclatation->pElementData[0].stride;

	IDirect3DVertexBuffer9 *pVertBuffer;
	HRESULT hr = pd3dDevice->CreateVertexBuffer(stride*pVertexBuffer->numVerts, usage, 0, pool, &pVertBuffer, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create vertex buffer");
	if(FAILED(hr))
		return false;

	if(pVertexBuffer->pName)
		MFRenderer_D3D9_SetDebugName(pVertBuffer, pVertexBuffer->pName);
	pVertexBuffer->pPlatformData = pVertBuffer;

	if(pVertexBufferMemory)
	{
		void *pData;
		pVertBuffer->Lock(0, 0, &pData, lockFlags);
		MFCopyMemory(pData, pVertexBufferMemory, stride*pVertexBuffer->numVerts);
		pVertBuffer->Unlock();
	}

	return true;
}

void MFVertex_DestroyVertexBufferPlatformSpecific(MFVertexBuffer *pVertexBuffer)
{
	IDirect3DVertexBuffer9 *pVB = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	pVB->Release();
}

MF_API void MFVertex_LockVertexBuffer(MFVertexBuffer *pVertexBuffer, void **ppVertices)
{
	MFDebug_Assert(!pVertexBuffer->bLocked, "Vertex buffer already locked!");

	IDirect3DVertexBuffer9 *pVB = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	HRESULT hr = pVB->Lock(0, 0, &pVertexBuffer->pLocked, pVertexBuffer->bufferType == MFVBType_Dynamic ? D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE : 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock vertex buffer");

	if(ppVertices)
		*ppVertices = pVertexBuffer->pLocked;

	pVertexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	MFDebug_Assert(pVertexBuffer->bLocked, "Vertex buffer not locked!");

	IDirect3DVertexBuffer9 *pVertBuffer = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	pVertBuffer->Unlock();

	pVertexBuffer->pLocked = NULL;
	pVertexBuffer->bLocked = false;
}

bool MFVertex_CreateIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer, uint16 *pIndexBufferMemory)
{
	IDirect3DIndexBuffer9 *pIB;
	HRESULT hr = pd3dDevice->CreateIndexBuffer(sizeof(uint16)*pIndexBuffer->numIndices, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to create index buffer");
	if(FAILED(hr))
		return false;

	if(pIndexBuffer->pName)
		MFRenderer_D3D9_SetDebugName(pIB, pIndexBuffer->pName);
	pIndexBuffer->pPlatformData = pIB;

	if(pIndexBufferMemory)
	{
		void *pData;
		pIB->Lock(0, 0, &pData, 0);
		MFCopyMemory(pData, pIndexBufferMemory, sizeof(uint16)*pIndexBuffer->numIndices);
		pIB->Unlock();
	}

	return true;
}

void MFVertex_DestroyIndexBufferPlatformSpecific(MFIndexBuffer *pIndexBuffer)
{
	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;
	pIB->Release();
}

MF_API void MFVertex_LockIndexBuffer(MFIndexBuffer *pIndexBuffer, uint16 **ppIndices)
{
	MFDebug_Assert(!pIndexBuffer->bLocked, "Index buffer already locked!");

	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;
	HRESULT hr = pIB->Lock(0, sizeof(uint16)*pIndexBuffer->numIndices, &pIndexBuffer->pLocked, 0);
	MFDebug_Assert(SUCCEEDED(hr), "Failed to lock index buffer");

	if(ppIndices)
		*ppIndices = (uint16*)pIndexBuffer->pLocked;

	pIndexBuffer->bLocked = true;
}

MF_API void MFVertex_UnlockIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	MFDebug_Assert(pIndexBuffer->bLocked, "Index buffer not locked!");

	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;
	pIB->Unlock();

	pIndexBuffer->pLocked = NULL;
	pIndexBuffer->bLocked = false;
/*
	// why on earth did I defer the index locking to a post process???
	IDirect3DIndexBuffer9 *pIB = (IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData;

	void *pData;
	pIB->Lock(0, 0, &pData, 0);
	MFCopyMemory(pData, pIndexBuffer->pIndices, sizeof(uint16)*pIndexBuffer->numIndices);
	pIB->Unlock();

	pIndexBuffer->bLocked = false;
*/
}

MF_API void MFVertex_SetVertexDeclaration(const MFVertexDeclaration *pVertexDeclaration)
{
	IDirect3DVertexDeclaration9 *pVertexDecl = (IDirect3DVertexDeclaration9*)pVertexDeclaration->pPlatformData;
	pd3dDevice->SetVertexDeclaration(pVertexDecl);
}

MF_API void MFVertex_SetVertexStreamSource(int stream, const MFVertexBuffer *pVertexBuffer)
{
	IDirect3DVertexBuffer9 *pVertBuffer = (IDirect3DVertexBuffer9*)pVertexBuffer->pPlatformData;
	pd3dDevice->SetStreamSource(stream, pVertBuffer, 0, pVertexBuffer->pVertexDeclatation->pElementData[0].stride);
}

MF_API void MFVertex_SetIndexBuffer(const MFIndexBuffer *pIndexBuffer)
{
	pd3dDevice->SetIndices((IDirect3DIndexBuffer9*)pIndexBuffer->pPlatformData);
}

static int MFVertex_GetNumPrims(MFPrimType primType, int numVertices)
{
	switch(primType)
	{
		case MFPT_Points:			return numVertices;
		case MFPT_LineList:			return numVertices / 2;
		case MFPT_LineStrip:		return numVertices - 1;
		case MFPT_TriangleList:		return numVertices / 3;
		case MFPT_TriangleStrip:	return numVertices - 2;
		case MFPT_TriangleFan:		return numVertices - 2;
		default:
			MFDebug_Assert(false, "Unknown primitive type!");
	}
	return 0;
}

MF_API void MFVertex_RenderVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int firstVertex, int numVertices)
{
	pd3dDevice->DrawPrimitive(gPrimTypes[primType], firstVertex, MFVertex_GetNumPrims(primType, numVertices));
}

MF_API void MFVertex_RenderIndexedVertices(MFEffectTechnique *pTechnique, MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices)
{
	pd3dDevice->DrawIndexedPrimitive(gPrimTypes[primType], 0, 0, numVertices, 0, MFVertex_GetNumPrims(primType, numIndices));
}

#endif // MF_RENDERER
