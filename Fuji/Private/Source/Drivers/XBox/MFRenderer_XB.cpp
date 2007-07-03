#include "Fuji.h"

#if MF_RENDERER == XBOX

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_XB.h"

extern IDirect3DDevice8 *pd3dDevice;

// direct3d management fucntions
void MFRendererXB_SetTexture(int stage, IDirect3DTexture8 *pTexture)
{
	pd3dDevice->SetTexture(stage, pTexture);
}

void MFRendererXB_SetVertexShader(uint32 vsHandle)
{
	pd3dDevice->SetVertexShader(vsHandle);
}

void MFRendererXB_SetStreamSource(int stream, IDirect3DVertexBuffer8 *pVertexBuffer, int stride)
{
	pd3dDevice->SetStreamSource((uint32)stream, pVertexBuffer, stride);
}

void MFRendererXB_SetIndices(IDirect3DIndexBuffer8 *pIndexBuffer, int baseIndex)
{
	pd3dDevice->SetIndices(pIndexBuffer, baseIndex);
}

void MFRendererXB_SetTextureMatrix(const MFMatrix &textureMatrix)
{
	MFMatrix texMat = textureMatrix;
	texMat.SetZAxis3(texMat.GetTrans());
	pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&texMat);
}

void MFRendererXB_ApplyGPUStates()
{

}

void MFRendererXB_SetDefaultGPUStates()
{

}

void MFRendererXB_ApplyRenderStates()
{

}

void MFRendererXB_SetDefaultRenderStates()
{

}

void MFRendererXB_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetRenderState(type, value);
}

void MFRendererXB_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetRenderState(type, (DWORD*)pValue);
}

void MFRendererXB_ApplyTextureStageStates()
{

}

void MFRendererXB_SetDefaultTextureStageStates()
{

}

void MFRendererXB_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
{
	pd3dDevice->SetTextureStageState(stage, type, value);
}

void MFRendererXB_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetTextureStageState(stage, type, (DWORD*)pValue);
}

void MFRendererXB_ConvertFloatToXBVF(const float *pFloat, char *pData, XBVF_Type type, int *pNumBytesWritten)
{
	int numBytes = 0;

	switch(type)
	{
		case XBVF_Float4:
			((float*)pData)[3] = pFloat[3];
			numBytes = 4;
		case XBVF_Float3:
			((float*)pData)[2] = pFloat[2];
			numBytes += 4;
		case XBVF_Float2:
			((float*)pData)[1] = pFloat[1];
			numBytes += 4;
		case XBVF_Float1:
			((float*)pData)[0] = pFloat[0];
			numBytes += 4;
			break;

		case XBVF_D3DColor:
			*(uint32*)pData = (((uint32)(pFloat[3] * 255.0f) & 0xFF) << 24) |
							  (((uint32)(pFloat[0] * 255.0f) & 0xFF) << 16) |
							  (((uint32)(pFloat[1] * 255.0f) & 0xFF) << 8) |
							   ((uint32)(pFloat[2] * 255.0f) & 0xFF);
			numBytes = 4;
			break;

		case XBVF_Short4:
			((int16*)pData)[2] = (int16)pFloat[2];
			((int16*)pData)[3] = (int16)pFloat[3];
			numBytes = 4;
		case XBVF_Short2:
			((int16*)pData)[0] = (int16)pFloat[0];
			((int16*)pData)[1] = (int16)pFloat[1];
			numBytes += 4;
			break;

		case XBVF_Short1N:
			break;

		case XBVF_Short2N:
			break;

		case XBVF_Short3N:
			break;

		case XBVF_Short4N:
			break;

		case XBVF_NormPacked:
			break;

		case XBVF_Short1:
			break;

		case XBVF_Short3:
			break;

		case XBVF_UByte1N:
			break;

		case XBVF_UByte2N:
			break;

		case XBVF_UByte3N:
			break;

		case XBVF_UByte4N:
			break;

		case XBVF_Float2H:
			break;

		default:
			MFDebug_Assert(false, "Invalid XBVF_Type");
	}

	if(pNumBytesWritten)
		*pNumBytesWritten = numBytes;
}

void MFRendererXB_ConvertXBVFToFloat(const char *pData, float *pFloat, XBVF_Type type, int *pNumComponentsWritten)
{
	int numComponents = 0;

	switch(type)
	{
		case XBVF_Float4:
			pFloat[3] = ((float*)pData)[3];
			++numComponents;
		case XBVF_Float3:
			pFloat[2] = ((float*)pData)[2];
			++numComponents;
		case XBVF_Float2:
			pFloat[1] = ((float*)pData)[1];
			++numComponents;
		case XBVF_Float1:
			pFloat[0] = ((float*)pData)[0];
			++numComponents;
			break;

		case XBVF_D3DColor:
			break;

		case XBVF_Short2:
			break;

		case XBVF_Short4:
			break;

		case XBVF_Short1N:
			break;

		case XBVF_Short2N:
			break;

		case XBVF_Short3N:
			break;

		case XBVF_Short4N:
			break;

		case XBVF_NormPacked:
			break;

		case XBVF_Short1:
			break;

		case XBVF_Short3:
			break;

		case XBVF_UByte1N:
			break;

		case XBVF_UByte2N:
			break;

		case XBVF_UByte3N:
			break;

		case XBVF_UByte4N:
			break;

		case XBVF_Float2H:
			break;

		default:
			MFDebug_Assert(false, "Invalid XBVF_Type");
	}

	if(pNumComponentsWritten)
		*pNumComponentsWritten = numComponents;
}

#endif
