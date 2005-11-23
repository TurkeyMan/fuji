#if !defined(_RENDERER_XB_H)
#define _RENDERER_XB_H

enum XBVF_Type
{
	XBVF_Float4,
	XBVF_Float3,
	XBVF_Float2,
	XBVF_Float1,
	XBVF_D3DColor,
	XBVF_Short2,
	XBVF_Short4,
	XBVF_Short1N,
	XBVF_Short2N,
	XBVF_Short3N,
	XBVF_Short4N,
	XBVF_NormPacked,
	XBVF_Short1,
	XBVF_Short3,
	XBVF_UByte1N,
	XBVF_UByte2N,
	XBVF_UByte3N,
	XBVF_UByte4N,
	XBVF_Float2H,

	XBVF_Max,
	XBVF_ForceInt = 0x7FFFFFFF
};

void RendererXB_SetTexture(int stage, IDirect3DTexture8 *pTexture);
void RendererXB_SetVertexShader(uint32 vsHandle);
void RendererXB_SetStreamSource(int stream, IDirect3DVertexBuffer8 *pVertexBuffer, int stride);
void RendererXB_SetIndices(IDirect3DIndexBuffer8 *pIndexBuffer, int baseIndex);

void RendererXB_ApplyGPUStates();
void RendererXB_SetDefaultGPUStates();

void RendererXB_ApplyRenderStates();
void RendererXB_SetDefaultRenderStates();
void RendererXB_SetRenderState(D3DRENDERSTATETYPE type, uint32 value);
void RendererXB_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue);

void RendererXB_ApplyTextureStageStates();
void RendererXB_SetDefaultTextureStageStates();
void RendererXB_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value);
void RendererXB_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue);

void RendererXB_ConvertFloatToXBVF(const float *pFloat, char *pData, XBVF_Type type);
void RendererXB_ConvertXBVFToFloat(const char *pData, float *pFloat, XBVF_Type type);

#endif
