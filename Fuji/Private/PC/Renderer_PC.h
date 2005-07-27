#if !defined(_RENDERER_PC_H)
#define _RENDERER_PC_H

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

void RendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture);
void RendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader);
void RendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride);
void RendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer);

void RendererPC_ApplyGPUStates();
void RendererPC_SetDefaultGPUStates();

void RendererPC_ApplyRenderStates();
void RendererPC_SetDefaultRenderStates();
void RendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value);
void RendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue);

void RendererPC_ApplyTextureStageStates();
void RendererPC_SetDefaultTextureStageStates();
void RendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value);
void RendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue);

void RendererPC_ApplySamplerStates();
void RendererPC_SetDefaultSamplerStates();
void RendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value);
void RendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue);

void RendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type);
void RendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type);

#endif
