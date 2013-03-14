#include "Fuji.h"
#include "MFVertex_Internal.h"
#include "MFHeap.h"
#include "MFVector.h"

int gVertexDataStride[MFVDF_Max] =
{
	16,	// MFVDF_Float4
	12,	// MFVDF_Float3
	8,	// MFVDF_Float2
	4,	// MFVDF_Float1
	4,	// MFVDF_UByte4_RGBA
	4,	// MFVDF_UByte4N_RGBA
	4,	// MFVDF_UByte4N_BGRA
	8,	// MFVDF_SShort4
	4,	// MFVDF_SShort2
	8,	// MFVDF_SShort4N
	4,	// MFVDF_SShort2N
	8,	// MFVDF_UShort4
	4,	// MFVDF_UShort2
	8,	// MFVDF_UShort4N
	4,	// MFVDF_UShort2N
	8,	// MFVDF_Float16_4
	4	// MFVDF_Float16_2
};

static MFVertexBuffer *gpScratchBufferList = NULL;

MFInitStatus MFVertex_InitModule()
{
	MFVertex_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFVertex_DeinitModule()
{
	MFVertex_DeinitModulePlatformSpecific();
}

MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(MFVertexElement *pElementArray, int elementCount)
{
	uint32 hash = MFUtil_HashBuffer(pElementArray, sizeof(MFVertexElement)*elementCount);

	MFVertexDeclaration *pDecl = (MFVertexDeclaration*)MFResource_FindResource(hash);
	if(!pDecl)
	{
		pDecl = (MFVertexDeclaration*)MFHeap_Alloc(sizeof(MFVertexDeclaration) + (sizeof(MFVertexElement)+sizeof(MFVertexElementData))*elementCount);
		pDecl->type = MFRT_VertexDecl;
		pDecl->hash = hash;
		pDecl->refCount = 0;

		pDecl->numElements = elementCount;
		pDecl->pElements = (MFVertexElement*)&pDecl[1];
		pDecl->pElementData = (MFVertexElementData*)&pDecl->pElements[elementCount];
		pDecl->pPlatformData = NULL;

		MFCopyMemory(pDecl->pElements, pElementArray, sizeof(MFVertexElement)*elementCount);
		MFZeroMemory(pDecl->pElementData, sizeof(MFVertexElementData)*elementCount);

		if(!MFVertex_CreateVertexDeclarationPlatformSpecific(pDecl))
		{
			MFHeap_Free(pDecl);
			return NULL;
		}

		MFResource_AddResource(pDecl);
	}

	++pDecl->refCount;

	return pDecl;
}

MF_API void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{
	if(--pDeclaration->refCount > 0)
		return;

	MFResource_RemoveResource(pDeclaration);

	MFVertex_DestroyVertexDeclarationPlatformSpecific(pDeclaration);
	MFHeap_Free(pDeclaration);
}

MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory)
{
	MFVertexBuffer *pVB = (MFVertexBuffer*)MFHeap_Alloc(sizeof(MFVertexBuffer));
	pVB->type = MFRT_VertexBuffer;
	pVB->hash = (uint32)pVB;
	pVB->refCount = 1;

	pVB->pVertexDeclatation = pVertexFormat;
	pVB->bufferType = type;
	pVB->numVerts = numVerts;
	pVB->bLocked = false;
	pVB->pPlatformData = NULL;
	pVB->pNextScratchBuffer = NULL;

	if(!MFVertex_CreateVertexBufferPlatformSpecific(pVB, pVertexBufferMemory))
	{
		MFHeap_Free(pVB);
		return NULL;
	}

	if(type == MFVBType_Scratch)
	{
		// add to a scratch list that will be cleaned up later...
		pVB->pNextScratchBuffer = gpScratchBufferList;
		gpScratchBufferList = pVB;
	}
	else
		MFResource_AddResource(pVB);

	return pVB;
}

MF_API void MFVertex_DestroyVertexBuffer(MFVertexBuffer *pVertexBuffer)
{
	if(--pVertexBuffer->refCount > 0)
		return;

	MFDebug_Assert(pVertexBuffer->type != MFVBType_Scratch, "Scratch buffers should not be freed!");
	if(pVertexBuffer->type == MFVBType_Scratch)
		return;

	MFResource_RemoveResource(pVertexBuffer);

	MFVertex_DestroyVertexBufferPlatformSpecific(pVertexBuffer);
	MFHeap_Free(pVertexBuffer);
}

static MFVertexElement *MFVertex_FindVertexElement(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, MFVertexElementData **ppElementData)
{
	MFVertexDeclaration *pDecl = pVertexBuffer->pVertexDeclatation;

	for(int a=0; a<pDecl->numElements; ++a)
	{
		if(pDecl->pElements[a].elementType == targetElement && pDecl->pElements[a].elementIndex == targetElementIndex)
		{
			if(ppElementData)
				*ppElementData = pVertexBuffer->pVertexDeclatation->pElementData + a;
			return pDecl->pElements + a;
		}
	}

	return NULL;
}

MF_API void MFVertex_CopyVertexData(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const void *pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices)
{
	MFVertexElementData *pElementData;
	if(!pVertexBuffer->bLocked || !MFVertex_FindVertexElement(pVertexBuffer, targetElement, targetElementIndex, &pElementData))
		return;

	MFDebug_Assert(numVertices <= pVertexBuffer->numVerts, "Too manu vertices for vertex buffer!");

	if(pElementData->format == sourceDataFormat && pElementData->stride == sourceDataStride)
	{
		// direct stream copy
		MFCopyMemory(pElementData->pData, pSourceData, numVertices * sourceDataStride);
	}
	else if(pElementData->format == sourceDataFormat)
	{
		// interleaved stream copy
		char *pSource = (char*)pSourceData;
		char *pTarget = (char*)pElementData->pData;
		int dataSize = gVertexDataStride[sourceDataFormat];
		for(int a=0; a<numVertices; ++a)
		{
			MFCopyMemory(pTarget, pSource, dataSize);
			pSource += sourceDataStride;
			pTarget += pElementData->stride;
		}
	}
	else
	{
		// transcode formats....
		char *pSource = (char*)pSourceData;
		char *pTarget = (char*)pElementData->pData;
		if(sourceDataFormat == MFVDF_Float4)
		{
			// this is a really common case (filling a vertex buffer with generated vertex data, or blanket setting to zero/one)
			for(int a=0; a<numVertices; ++a)
			{
				MFVertex_PackVertexData(*(MFVector*)pSource, pTarget, pElementData->format);
				pSource += sourceDataStride;
				pTarget += pElementData->stride;
			}
		}
		else
		{
			for(int a=0; a<numVertices; ++a)
			{
				MFVector v = MFVertex_UnpackVertexData(pSource, sourceDataFormat);
				MFVertex_PackVertexData(v, pTarget, pElementData->format);
				pSource += sourceDataStride;
				pTarget += pElementData->stride;
			}
		}
	}
}

MF_API void MFVertex_SetVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, const MFVector &data)
{
	
}

MF_API void MFVertex_ReadVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector *pData)
{

}

MF_API void MFVertex_SetVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 data)
{

}

MF_API void MFVertex_ReadVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 *pData)
{

}

MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory)
{
	MFIndexBuffer *pIB = (MFIndexBuffer*)MFHeap_Alloc(sizeof(MFIndexBuffer) + (pIndexBufferMemory ? 0 : sizeof(uint16)*numIndices));
	pIB->type = MFRT_IndexBuffer;
	pIB->hash = (uint32)pIB;
	pIB->refCount = 1;

	pIB->pIndices = pIndexBufferMemory ? pIndexBufferMemory : (uint16*)&pIB[1];
	pIB->numIndices = numIndices;
	pIB->bLocked = false;
	pIB->pPlatformData = NULL;

	if(!MFVertex_CreateIndexBufferPlatformSpecific(pIB, pIndexBufferMemory))
	{
		MFHeap_Free(pIB);
		return NULL;
	}

	MFResource_AddResource(pIB);

	return pIB;
}

MF_API void MFVertex_DestroyIndexBuffer(MFIndexBuffer *pIndexBuffer)
{
	if(--pIndexBuffer->refCount > 0)
		return;

	MFResource_RemoveResource(pIndexBuffer);

	MFVertex_DestroyIndexBufferPlatformSpecific(pIndexBuffer);
	MFHeap_Free(pIndexBuffer);
}
