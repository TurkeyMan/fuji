#include "Fuji.h"
#include "MFVector.h"
#include "MFVertex_Internal.h"

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

MFInitStatus MFVertex_InitModule()
{
	MFVertex_InitModulePlatformSpecific();

	return MFAIC_Succeeded;
}

void MFVertex_DeinitModule()
{
	MFVertex_DeinitModulePlatformSpecific();
}

MFVertexElement *MFVertex_FindVertexElement(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, MFVertexElementData **ppElementData)
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

void MFVertex_CopyVertexData(MFVertexBuffer *pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const void *pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices)
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

void MFVertex_SetVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, const MFVector &data)
{
	
}

void MFVertex_ReadVertexData4v(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector *pData)
{

}

void MFVertex_SetVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 data)
{

}

void MFVertex_ReadVertexData4ub(MFVertexBuffer *pVertexBuffer, MFVertexElementType element, int elementIndex, uint32 *pData)
{

}
