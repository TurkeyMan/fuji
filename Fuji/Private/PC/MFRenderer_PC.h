#if !defined(_MFRENDERER_PC_H)
#define _MFRENDERER_PC_H

enum PCVF_Type
{
	PCVF_Float4,
	PCVF_Float3,
	PCVF_Float2,
	PCVF_Float1,
	PCVF_D3DColor,
	PCVF_UByte4,
	PCVF_Short2,
	PCVF_Short4,
	PCVF_UByte4N,
	PCVF_Short2N,
	PCVF_Short4N,
	PCVF_UShort2N,
	PCVF_UShort4N,
	PCVF_UDec3,
	PCVF_Dec3N,
	PCVF_Float16_2,
	PCVF_Float16_4,

	PCVF_Max,
	PCVF_ForceInt = 0x7FFFFFFF
};

void MFRendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture);
void MFRendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader);
void MFRendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride);
void MFRendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer);

void MFRendererPC_ApplyGPUStates();
void MFRendererPC_SetDefaultGPUStates();

void MFRendererPC_ApplyRenderStates();
void MFRendererPC_SetDefaultRenderStates();
void MFRendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value);
void MFRendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue);

void MFRendererPC_ApplyTextureStageStates();
void MFRendererPC_SetDefaultTextureStageStates();
void MFRendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value);
void MFRendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue);

void MFRendererPC_ApplySamplerStates();
void MFRendererPC_SetDefaultSamplerStates();
void MFRendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value);
void MFRendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue);

void MFRendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type);
void MFRendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type);

void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen);
void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix);
void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix);
void MFRendererPC_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd);
void MFRendererPC_SetNumWeights(int numWeights);

#endif // _MFRENDERER_PC_H
