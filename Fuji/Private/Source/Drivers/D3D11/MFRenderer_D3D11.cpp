#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFRenderer_InitModulePlatformSpecific MFRenderer_InitModulePlatformSpecific_D3D11
	#define MFRenderer_DeinitModulePlatformSpecific MFRenderer_DeinitModulePlatformSpecific_D3D11
	#define MFRenderer_CreateDisplay MFRenderer_CreateDisplay_D3D11
	#define MFRenderer_DestroyDisplay MFRenderer_DestroyDisplay_D3D11
	#define MFRenderer_ResetDisplay MFRenderer_ResetDisplay_D3D11
	#define MFRenderer_BeginFrame MFRenderer_BeginFrame_D3D11
	#define MFRenderer_EndFrame MFRenderer_EndFrame_D3D11
	#define MFRenderer_SetClearColour MFRenderer_SetClearColour_D3D11
	#define MFRenderer_ClearScreen MFRenderer_ClearScreen_D3D11
	#define MFRenderer_GetViewport MFRenderer_GetViewport_D3D11
	#define MFRenderer_SetViewport MFRenderer_SetViewport_D3D11
	#define MFRenderer_ResetViewport MFRenderer_ResetViewport_D3D11
	#define MFRenderer_SetRenderTarget MFRenderer_SetRenderTarget_D3D11
	#define MFRenderer_SetDeviceRenderTarget MFRenderer_SetDeviceRenderTarget_D3D11
	#define MFRenderer_GetTexelCenterOffset MFRenderer_GetTexelCenterOffset_D3D11
#endif

#include "MFTexture_Internal.h"

void MFRenderer_InitModulePlatformSpecific()
{
}

void MFRenderer_DeinitModulePlatformSpecific()
{
}

int MFRenderer_CreateDisplay()
{
	return 0;
}

void MFRenderer_DestroyDisplay()
{
}

void MFRenderer_ResetDisplay()
{
}

void MFRenderer_BeginFrame()
{
	MFCALLSTACK;
}

void MFRenderer_EndFrame()
{
	MFCALLSTACK;
}

void MFRenderer_SetClearColour(float r, float g, float b, float a)
{
}

void MFRenderer_ClearScreen(uint32 flags)
{
	MFCALLSTACKc;

}

void MFRenderer_GetViewport(MFRect *pRect)
{
}

void MFRenderer_SetViewport(MFRect *pRect)
{

}

void MFRenderer_ResetViewport()
{

}

void MFRenderer_SetRenderTarget(MFTexture *pRenderTarget, MFTexture *pZTarget)
{

}

void MFRenderer_SetDeviceRenderTarget()
{
}

float MFRenderer_GetTexelCenterOffset()
{
	return 0.5f;
}
//
//// direct3d management fucntions
//void MFRendererPC_SetTexture(int stage, IDirect3DTexture9 *pTexture)
//{
//}
//
//void MFRendererPC_SetVertexShader(IDirect3DVertexShader9 *pVertexShader)
//{
//}
//
//void MFRendererPC_SetStreamSource(int stream, IDirect3DVertexBuffer9 *pVertexBuffer, int offset, int stride)
//{
//}
//
//void MFRendererPC_SetIndices(IDirect3DIndexBuffer9 *pIndexBuffer)
//{
//}
//
//void MFRendererPC_SetAnimationMatrix(int boneID, const MFMatrix &animationMatrix)
//{
//}
//
//void MFRendererPC_SetWorldToScreenMatrix(const MFMatrix &worldToScreen)
//{
//}
//
//void MFRendererPC_SetTextureMatrix(const MFMatrix &textureMatrix)
//{
//}
//
//void MFRendererPC_SetModelColour(const MFVector &colour)
//{
//}
//
//void MFRendererPC_SetColourMask(float colourModulate, float colourAdd, float alphaModulate, float alphaAdd)
//{
//}
//
//void MFRendererPC_SetNumWeights(int numWeights)
//{
//}
//
//int MFRendererPC_GetNumWeights()
//{
//	return 0;
//}
//
//void MFRendererPC_ApplyGPUStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultGPUStates()
//{
//
//}
//
//void MFRendererPC_ApplyRenderStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultRenderStates()
//{
//
//}
//
//void MFRendererPC_SetRenderState(D3DRENDERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetRenderState(D3DRENDERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ApplyTextureStageStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultTextureStageStates()
//{
//
//}
//
//void MFRendererPC_SetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ApplySamplerStates()
//{
//
//}
//
//void MFRendererPC_SetDefaultSamplerStates()
//{
//
//}
//
//void MFRendererPC_SetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 value)
//{
//}
//
//void MFRendererPC_GetSamplerState(int sampler, D3DSAMPLERSTATETYPE type, uint32 *pValue)
//{
//}
//
//void MFRendererPC_ConvertFloatToPCVF(const float *pFloat, char *pData, PCVF_Type type, int *pNumBytesWritten)
//{
//}
//
//void MFRendererPC_ConvertPCVFToFloat(const char *pData, float *pFloat, PCVF_Type type, int *pNumComponentsWritten)
//{
//}


#endif // MF_RENDERER
