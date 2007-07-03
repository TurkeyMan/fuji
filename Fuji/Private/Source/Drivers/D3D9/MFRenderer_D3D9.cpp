#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9

#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"

#include "MFRenderer_Internal.h"
#include "MFRenderer_D3D9.h"

#include "Shaders/Registers.h"

#include <d3d9.h>

extern IDirect3DDevice9 *pd3dDevice;

int gNumWeights = 0;

// direct3d management fucntions
void MFRendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture)
{
	pd3dDevice->SetTexture(stage, pTexture);
}

void MFRendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader)
{
	pd3dDevice->SetVertexShader(pVertexShader);
}

void MFRendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride)
{
	pd3dDevice->SetStreamSource(stream, pVertexBuffer, offset, stride);
}

void MFRendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer)
{
	pd3dDevice->SetIndices(pIndexBuffer);
}

void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
{
	MFMatrix mat = animationMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_animMats + boneID*3, (float*)&mat, 3);
}

void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
{
	MFMatrix mat = worldToScreen;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_wvp, (float*)&mat, 4);
}

void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix)
{
	MFMatrix mat = textureMatrix;
	mat.Transpose();
	pd3dDevice->SetVertexShaderConstantF(r_tex, (float*)&mat, 2);
}

void MFRendererPC_SetModelColour(const MFVector &colour)
{
	pd3dDevice->SetVertexShaderConstantF(r_modelColour, colour, 1);
}

void MFRendererPC_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd)
{
	float mask[4] = { colourModulate, colourAdd, alphaModulate, alphaAdd };
	pd3dDevice->SetVertexShaderConstantF(r_colourMask, mask, 1);
}

void MFRendererPC_SetNumWeights(int numWeights)
{
	gNumWeights = numWeights;

//	int i[4] = { numWeights, numWeights, numWeights, numWeights };
//	pd3dDevice->SetVertexShaderConstantI(r_numWeights, i, 1);
	pd3dDevice->SetVertexShaderConstantF(r_animating, numWeights ? MFVector::identity : MFVector::one, 1);
	pd3dDevice->SetVertexShaderConstantF(r_animating + 1, numWeights ? MFVector::one : MFVector::identity, 1);
}

int MFRendererPC_GetNumWeights()
{
	return gNumWeights;
}

void MFRendererPC_ApplyGPUStates()
{

}

void MFRendererPC_SetDefaultGPUStates()
{

}

void MFRendererPC_ApplyRenderStates()
{

}

void MFRendererPC_SetDefaultRenderStates()
{

}

void MFRendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetRenderState(type, value);
}

void MFRendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetRenderState(type, (DWORD*)pValue);
}

void MFRendererPC_ApplyTextureStageStates()
{

}

void MFRendererPC_SetDefaultTextureStageStates()
{

}

void MFRendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
{
	pd3dDevice->SetTextureStageState(stage, type, value);
}

void MFRendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetTextureStageState(stage, type, (DWORD*)pValue);
}

void MFRendererPC_ApplySamplerStates()
{

}

void MFRendererPC_SetDefaultSamplerStates()
{

}

void MFRendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value)
{
	pd3dDevice->SetSamplerState(sampler, type, value);
}

void MFRendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue)
{
	pd3dDevice->GetSamplerState(sampler, type, (DWORD*)pValue);
}

void MFRendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type, int *pNumBytesWritten)
{
	int numBytes = 0;

	switch(type)
	{
		case PCVF_Float4:
			((float*)pData)[3] = pFloat[3];
			numBytes = 4;
		case PCVF_Float3:
			((float*)pData)[2] = pFloat[2];
			numBytes += 4;
		case PCVF_Float2:
			((float*)pData)[1] = pFloat[1];
			numBytes += 4;
		case PCVF_Float1:
			((float*)pData)[0] = pFloat[0];
			numBytes += 4;
			break;

		case PCVF_D3DColor:
			*(uint32*)pData = (((uint32)(pFloat[3] * 255.0f) & 0xFF) << 24) |
							  (((uint32)(pFloat[0] * 255.0f) & 0xFF) << 16) |
							  (((uint32)(pFloat[1] * 255.0f) & 0xFF) << 8) |
							   ((uint32)(pFloat[2] * 255.0f) & 0xFF);
			numBytes = 4;
			break;

		case PCVF_UByte4:
			pData[0] = (uint8)pFloat[0];
			pData[1] = (uint8)pFloat[1];
			pData[2] = (uint8)pFloat[2];
			pData[3] = (uint8)pFloat[3];
			numBytes = 4;
			break;

		case PCVF_Short4:
			((int16*)pData)[2] = (int16)pFloat[2];
			((int16*)pData)[3] = (int16)pFloat[3];
			numBytes = 4;
		case PCVF_Short2:
			((int16*)pData)[0] = (int16)pFloat[0];
			((int16*)pData)[1] = (int16)pFloat[1];
			numBytes += 4;
			break;

		case PCVF_UByte4N:
			break;

		case PCVF_Short2N:
			break;

		case PCVF_Short4N:
			break;

		case PCVF_UShort2N:
			break;

		case PCVF_UShort4N:
			break;

		case PCVF_UDec3:
			break;

		case PCVF_Dec3N:
			break;

		case PCVF_Float16_2:
			break;

		case PCVF_Float16_4:
			break;

		default:
			MFDebug_Assert(false, "Invalid PCVF_Type");
	}

	if(pNumBytesWritten)
		*pNumBytesWritten = numBytes;
}

void MFRendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type, int *pNumComponentsWritten)
{
	int numComponents = 0;

	switch(type)
	{
		case PCVF_Float4:
			pFloat[3] = ((float*)pData)[3];
			++numComponents;
		case PCVF_Float3:
			pFloat[2] = ((float*)pData)[2];
			++numComponents;
		case PCVF_Float2:
			pFloat[1] = ((float*)pData)[1];
			++numComponents;
		case PCVF_Float1:
			pFloat[0] = ((float*)pData)[0];
			++numComponents;
			break;

		case PCVF_D3DColor:
			break;

		case PCVF_UByte4:
			break;

		case PCVF_Short2:
			break;

		case PCVF_Short4:
			break;

		case PCVF_UByte4N:
			break;

		case PCVF_Short2N:
			break;

		case PCVF_Short4N:
			break;

		case PCVF_UShort2N:
			break;

		case PCVF_UShort4N:
			break;

		case PCVF_UDec3:
			break;

		case PCVF_Dec3N:
			break;

		case PCVF_Float16_2:
			break;

		case PCVF_Float16_4:
			break;

		default:
			MFDebug_Assert(false, "Invalid PCVF_Type");
	}

	if(pNumComponentsWritten)
		*pNumComponentsWritten = numComponents;
}

#endif // MF_RENDERER
