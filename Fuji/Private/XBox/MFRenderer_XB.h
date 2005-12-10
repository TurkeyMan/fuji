#if !defined(_MFRENDERER_XB_H)
#define _MFRENDERER_XB_H

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

void MFRendererXB_SetTexture(int stage, IDirect3DTexture8 *pTexture);
void MFRendererXB_SetVertexShader(uint32 vsHandle);
void MFRendererXB_SetStreamSource(int stream, IDirect3DVertexBuffer8 *pVertexBuffer, int stride);
void MFRendererXB_SetIndices(IDirect3DIndexBuffer8 *pIndexBuffer, int baseIndex);

void MFRendererXB_ApplyGPUStates();
void MFRendererXB_SetDefaultGPUStates();

void MFRendererXB_ApplyRenderStates();
void MFRendererXB_SetDefaultRenderStates();
void MFRendererXB_SetRenderState(D3DRENDERSTATETYPE type, uint32 value);
void MFRendererXB_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue);

void MFRendererXB_ApplyTextureStageStates();
void MFRendererXB_SetDefaultTextureStageStates();
void MFRendererXB_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value);
void MFRendererXB_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue);

void MFRendererXB_ConvertFloatToXBVF(const float *pFloat, char *pData, XBVF_Type type);
void MFRendererXB_ConvertXBVFToFloat(const char *pData, float *pFloat, XBVF_Type type);

#endif
