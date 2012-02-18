#if !defined(_MFVERTEX_INTERNAL_H)
#define _MFVERTEX_INTERNAL_H

#include "MFVertex.h"

MFInitStatus MFVertex_InitModule();
void MFVertex_DeinitModule();

void MFVertex_InitModulePlatformSpecific();
void MFVertex_DeinitModulePlatformSpecific();

struct MFVertexElementData
{
	void *pData;
	int offset;
	int stride;
	MFVertexDataFormat format;
};

struct MFVertexDeclaration
{
	MFVertexElement *pElements;
	MFVertexElementData *pElementData;
	int numElements;
	void *pPlatformData;
};

struct MFVertexBuffer
{
	MFVertexDeclaration *pVertexDeclatation;
	MFVertexBufferType bufferType;
	int numVerts;
	void *pPlatformData;

	bool bLocked;
};

struct MFIndexBuffer
{
	uint16 *pIndices;
	int numIndices;
	void *pPlatformData;

	bool bLocked;
};

inline void MFVertex_PackVertexData(MFVector &source, void *pTarget, MFVertexDataFormat format)
{
	const float scaleTable[2] = { 32768.f, 32767.f };
	switch(format)
	{
		case MFVDF_Float4:
			((float*)pTarget)[3] = source.w;
		case MFVDF_Float3:
			((float*)pTarget)[2] = source.z;
		case MFVDF_Float2:
			((float*)pTarget)[1] = source.y;
		case MFVDF_Float1:
			((float*)pTarget)[0] = source.x;
			break;
		case MFVDF_UByte4_RGBA:
		{
			struct rgba { uint8 r, g, b, a; } *pColour = (rgba*)pTarget;
			pColour->r = (uint8)source.x;
			pColour->g = (uint8)source.y;
			pColour->b = (uint8)source.z;
			pColour->a = (uint8)source.w;
			break;
		}
		case MFVDF_UByte4N_RGBA:
		{
			struct rgba { uint8 r, g, b, a; } *pColour = (rgba*)pTarget;
			pColour->r = (uint8)(source.x * 255.f);
			pColour->g = (uint8)(source.y * 255.f);
			pColour->b = (uint8)(source.z * 255.f);
			pColour->a = (uint8)(source.w * 255.f);
			break;
		}
		case MFVDF_UByte4N_BGRA:
		{
			struct gbra { uint8 b, g, r, a; } *pColour = (gbra*)pTarget;
			pColour->r = (uint8)(source.x * 255.f);
			pColour->g = (uint8)(source.y * 255.f);
			pColour->b = (uint8)(source.z * 255.f);
			pColour->a = (uint8)(source.w * 255.f);
			break;
		}
		case MFVDF_SShort4:
			((int16*)pTarget)[2] = (int16)source.z;
			((int16*)pTarget)[3] = (int16)source.w;
		case MFVDF_SShort2:
			((int16*)pTarget)[0] = (int16)source.x;
			((int16*)pTarget)[1] = (int16)source.y;
			break;
		case MFVDF_SShort4N:
			((int16*)pTarget)[2] = (int16)(source.z * scaleTable[source.z < 0.f]);
			((int16*)pTarget)[3] = (int16)(source.w * scaleTable[source.w < 0.f]);
		case MFVDF_SShort2N:
			((int16*)pTarget)[0] = (int16)(source.x * scaleTable[source.x < 0.f]);
			((int16*)pTarget)[1] = (int16)(source.y * scaleTable[source.y < 0.f]);
			break;
		case MFVDF_UShort4:
			((uint16*)pTarget)[2] = (uint16)source.z;
			((uint16*)pTarget)[3] = (uint16)source.w;
		case MFVDF_UShort2:
			((uint16*)pTarget)[0] = (uint16)source.x;
			((uint16*)pTarget)[1] = (uint16)source.y;
			break;
		case MFVDF_UShort4N:
			((uint16*)pTarget)[2] = (uint16)(source.z * 65535.f);
			((uint16*)pTarget)[3] = (uint16)(source.w * 65535.f);
		case MFVDF_UShort2N:
			((uint16*)pTarget)[0] = (uint16)(source.x * 65535.f);
			((uint16*)pTarget)[1] = (uint16)(source.y * 65535.f);
			break;
		case MFVDF_Float16_4:
		case MFVDF_Float16_2:
		default:
			MFDebug_Assert(false, "Unsupported format conversion!");
	}
}

inline MFVector MFVertex_UnpackVertexData(void *pData, MFVertexDataFormat format)
{
	const float scaleTable[2] = { 1.f/32768.f, 1.f/32767.f };
	MFVector output = MFVector::identity;

	switch(format)
	{
		case MFVDF_Float4:
			output.w = ((float*)pData)[3];
		case MFVDF_Float3:
			output.z = ((float*)pData)[2];
		case MFVDF_Float2:
			output.y = ((float*)pData)[1];
		case MFVDF_Float1:
			output.x = ((float*)pData)[0];
			break;
		case MFVDF_UByte4_RGBA:
		{
			struct rgba { uint8 r, g, b, a; } *pColour = (rgba*)pData;
			output.x = (float)pColour->r;
			output.y = (float)pColour->g;
			output.z = (float)pColour->b;
			output.w = (float)pColour->a;
			break;
		}
		case MFVDF_UByte4N_RGBA:
		{
			struct rgba { uint8 r, g, b, a; } *pColour = (rgba*)pData;
			output.x = (float)pColour->r * (1.f/255.f);
			output.y = (float)pColour->g * (1.f/255.f);
			output.z = (float)pColour->b * (1.f/255.f);
			output.w = (float)pColour->a * (1.f/255.f);
			break;
		}
		case MFVDF_UByte4N_BGRA:
		{
			struct bgra { uint8 b, g, r, a; } *pColour = (bgra*)pData;
			output.x = (float)pColour->r * (1.f/255.f);
			output.y = (float)pColour->g * (1.f/255.f);
			output.z = (float)pColour->b * (1.f/255.f);
			output.w = (float)pColour->a * (1.f/255.f);
			break;
		}
		case MFVDF_SShort4:
			output.z = (float)((int16*)pData)[2];
			output.w = (float)((int16*)pData)[3];
		case MFVDF_SShort2:
			output.x = (float)((int16*)pData)[0];
			output.y = (float)((int16*)pData)[1];
			break;
		case MFVDF_SShort4N:
			output.z = (float)((int16*)pData)[2] * scaleTable[((int16*)pData)[2] < 0];
			output.w = (float)((int16*)pData)[3] * scaleTable[((int16*)pData)[3] < 0];
		case MFVDF_SShort2N:
			output.x = (float)((int16*)pData)[0] * scaleTable[((int16*)pData)[0] < 0];
			output.y = (float)((int16*)pData)[1] * scaleTable[((int16*)pData)[1] < 0];
			break;
		case MFVDF_UShort4:
			output.z = (float)((uint16*)pData)[2];
			output.w = (float)((uint16*)pData)[3];
		case MFVDF_UShort2:
			output.x = (float)((uint16*)pData)[0];
			output.y = (float)((uint16*)pData)[1];
			break;
		case MFVDF_UShort4N:
			output.z = (float)((uint16*)pData)[2] * (1.f/65535.f);
			output.w = (float)((uint16*)pData)[3] * (1.f/65535.f);
		case MFVDF_UShort2N:
			output.x = (float)((uint16*)pData)[0] * (1.f/65535.f);
			output.y = (float)((uint16*)pData)[1] * (1.f/65535.f);
			break;
		case MFVDF_Float16_4:
		case MFVDF_Float16_2:
		default:
			MFDebug_Assert(false, "Unsupported format conversion!");
	}

	return output;
}

#endif
