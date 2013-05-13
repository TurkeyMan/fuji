#include "Fuji.h"
#include "MFVertex_Internal.h"
#include "MFRenderer.h"
#include "MFHeap.h"
#include "MFVector.h"

int gVertexDataStride[MFVDF_Max] =
{
	0,	// MFVDF_Auto
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
	4,	// MFVDF_Float16_2
	4,	// MFVDF_UDec3
	4	// MFVDF_Dec3N
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

void MFVertex_EndFrame()
{
	while(gpScratchBufferList)
	{
		// allow the platform to release any hardware resources...
		MFVertex_DestroyVertexBufferPlatformSpecific(gpScratchBufferList);

		gpScratchBufferList = gpScratchBufferList->pNextScratchBuffer;
	}
}

MF_API MFVertexDeclaration *MFVertex_CreateVertexDeclaration(const MFVertexElement *pElementArray, int elementCount)
{
	// assign the auto format components before calculating the hash
	MFVertexElement elements[16];
	MFCopyMemory(elements, pElementArray, sizeof(MFVertexElement)*elementCount);
	for(int e=0; e<elementCount; ++e)
	{
		if(pElementArray[e].format == MFVDF_Auto)
			elements[e].format = MFVertex_ChoooseVertexDataTypePlatformSpecific(pElementArray[e].type, pElementArray[e].componentCount);
	}

	uint32 hash = MFUtil_HashBuffer(elements, sizeof(MFVertexElement)*elementCount);

	MFVertexDeclaration *pDecl = (MFVertexDeclaration*)MFResource_FindResource(hash);
	if(!pDecl)
	{
		pDecl = (MFVertexDeclaration*)MFHeap_AllocAndZero(sizeof(MFVertexDeclaration) + (sizeof(MFVertexElement) + sizeof(MFVertexElementData))*elementCount);
		pDecl->type = MFRT_VertexDecl;
		pDecl->hash = hash;

		pDecl->numElements = elementCount;
		pDecl->pElements = (MFVertexElement*)&pDecl[1];
		pDecl->pElementData = (MFVertexElementData*)&pDecl->pElements[elementCount];

		MFCopyMemory(pDecl->pElements, elements, sizeof(MFVertexElement)*elementCount);

		int streamOffsets[16];
		MFZeroMemory(streamOffsets, sizeof(streamOffsets));

		// set the element data and calculate the strides
		for(int e=0; e<elementCount; ++e)
		{
			pDecl->pElementData[e].offset = streamOffsets[elements[e].stream];
			pDecl->pElementData[e].stride = 0;

			streamOffsets[elements[e].stream] += gVertexDataStride[elements[e].format];
			pDecl->streamsUsed |= MFBIT(elements[e].stream);
		}

		// set the strides for each component
		for(int e=0; e<elementCount; ++e)
			pDecl->pElementData[e].stride = streamOffsets[elements[e].stream];

		if(!MFVertex_CreateVertexDeclarationPlatformSpecific(pDecl))
		{
			MFHeap_Free(pDecl);
			return NULL;
		}

		MFResource_AddResource(pDecl);

		if(pDecl->streamsUsed != 1)
		{
			// create the stream declarations...
			MFVertexElement elements[64];
			for(int s=0; s<16; ++s)
			{
				if(!(pDecl->streamsUsed & (1 << s)))
					continue;

				int streamElements = 0;
				for(int e=0; e<elementCount; ++e)
				{
					if(elements[e].stream == s)
					{
						elements[streamElements] = elements[e];
						elements[streamElements].stream = 0;
						++streamElements;
					}
				}

				if(streamElements)
					pDecl->pStreamDecl[s] = MFVertex_CreateVertexDeclaration(elements, streamElements);
			}
		}
	}

	++pDecl->refCount;

	return pDecl;
}

MF_API void MFVertex_DestroyVertexDeclaration(MFVertexDeclaration *pDeclaration)
{
	if(--pDeclaration->refCount > 0)
		return;

	if(pDeclaration->streamsUsed != 1)
	{
		for(int a=0; a<8; ++a)
			if(pDeclaration->pStreamDecl[a])
				MFVertex_DestroyVertexDeclaration(pDeclaration->pStreamDecl[a]);
	}

	MFResource_RemoveResource(pDeclaration);

	MFVertex_DestroyVertexDeclarationPlatformSpecific(pDeclaration);
	MFHeap_Free(pDeclaration);
}

MF_API const MFVertexDeclaration *MFVertex_GetStreamDeclaration(const MFVertexDeclaration *pDeclaration, int stream)
{
	if(pDeclaration->streamsUsed == 1)
		return pDeclaration;
	return pDeclaration->pStreamDecl[stream];
}

MF_API MFVertexBuffer *MFVertex_CreateVertexBuffer(const MFVertexDeclaration *pVertexFormat, int numVerts, MFVertexBufferType type, void *pVertexBufferMemory, const char *pName)
{
	int nameLen = pName ? MFString_Length(pName) + 1 : 0;
	MFVertexBuffer *pVB;
	if(type == MFVBType_Scratch)
		pVB = (MFVertexBuffer*)MFRenderer_AllocateScratchMemory(sizeof(MFVertexBuffer) + nameLen);
	else
		pVB = (MFVertexBuffer*)MFHeap_Alloc(sizeof(MFVertexBuffer) + nameLen);
	MFZeroMemory(pVB, sizeof(MFVertexBuffer));

	pVB->type = MFRT_VertexBuffer;
	pVB->hash = (uint32)pVB;
	pVB->refCount = 1;
	if(pName)
	{
		pVB->pName = (const char*)pVB + sizeof(MFVertexBuffer);
		MFString_Copy((char*)pVB->pName, pName);
	}

	pVB->pVertexDeclatation = pVertexFormat;
	pVB->bufferType = type;
	pVB->numVerts = numVerts;

	if(!MFVertex_CreateVertexBufferPlatformSpecific(pVB, pVertexBufferMemory))
	{
		if(type != MFVBType_Scratch)
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

	MFDebug_Assert(pVertexBuffer->bufferType != MFVBType_Scratch, "Scratch buffers should not be freed!");
	if(pVertexBuffer->bufferType == MFVBType_Scratch)
		return;

	MFResource_RemoveResource(pVertexBuffer);

	MFVertex_DestroyVertexBufferPlatformSpecific(pVertexBuffer);
	MFHeap_Free(pVertexBuffer);
}

static int MFVertex_FindVertexElement(const MFVertexDeclaration *pDecl, MFVertexElementType targetElement, int targetElementIndex)
{
	for(int a=0; a<pDecl->numElements; ++a)
	{
		if(pDecl->pElements[a].type == targetElement && pDecl->pElements[a].index == targetElementIndex)
			return a;
	}
	return -1;
}

MF_API void MFVertex_CopyVertexData(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const void *pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices)
{
	const MFVertexDeclaration *pDecl = pVertexBuffer->pVertexDeclatation;
	int element = MFVertex_FindVertexElement(pDecl, targetElement, targetElementIndex);

	if(!pVertexBuffer->bLocked || element == -1)
		return;

	MFDebug_Assert(numVertices <= pVertexBuffer->numVerts, "Too manu vertices for vertex buffer!");

	MFVertexDataFormat format = pDecl->pElements[element].format;
	int stride = pDecl->pElementData[element].stride;
	int offset = pDecl->pElementData[element].offset;
	void *pElementData = (char*)pVertexBuffer->pLocked + offset;

	if(format == sourceDataFormat && stride == sourceDataStride)
	{
		// The stream has only a single element, we can memcopy
		MFCopyMemory(pElementData, pSourceData, numVertices * sourceDataStride);
	}
	else if(format == sourceDataFormat)
	{
		// interleaved stream copy
		char *pSource = (char*)pSourceData;
		char *pTarget = (char*)pElementData;
		int dataSize = gVertexDataStride[sourceDataFormat];
		for(int a=0; a<numVertices; ++a)
		{
			MFCopyMemory(pTarget, pSource, dataSize);
			pSource += sourceDataStride;
			pTarget += stride;
		}
	}
	else
	{
		// transcode formats....
		char *pSource = (char*)pSourceData;
		char *pTarget = (char*)pElementData;
		if(sourceDataFormat == MFVDF_Float4)
		{
			// this is a really common case (filling a vertex buffer with generated vertex data, or blanket setting to zero/one)
			for(int a=0; a<numVertices; ++a)
			{
				MFVertex_PackVertexData(*(MFVector*)pSource, pTarget, format);
				pSource += sourceDataStride;
				pTarget += stride;
			}
		}
		else
		{
			for(int a=0; a<numVertices; ++a)
			{
				MFVector v = MFVertex_UnpackVertexData(pSource, sourceDataFormat);
				MFVertex_PackVertexData(v, pTarget, format);
				pSource += sourceDataStride;
				pTarget += stride;
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

MF_API MFIndexBuffer *MFVertex_CreateIndexBuffer(int numIndices, uint16 *pIndexBufferMemory, const char *pName)
{
	int nameLen = pName ? MFString_Length(pName) + 1 : 0;
	MFIndexBuffer *pIB = (MFIndexBuffer*)MFHeap_AllocAndZero(sizeof(MFIndexBuffer) + nameLen);
	pIB->type = MFRT_IndexBuffer;
	pIB->hash = (uint32)pIB;
	pIB->refCount = 1;
	if(pName)
	{
		pIB->pName = (const char*)pIB + sizeof(MFIndexBuffer);
		MFString_Copy((char*)pIB->pName, pName);
	}

	pIB->numIndices = numIndices;

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
