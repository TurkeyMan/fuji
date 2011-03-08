#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D11
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D11
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D11
	#define MFMat_Standard_CreateInstance MFMat_Standard_CreateInstance_D3D11
	#define MFMat_Standard_DestroyInstance MFMat_Standard_DestroyInstance_D3D11
#endif

#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Standard.h"
#include "MFHeap.h"

#include <D3D11.h>

#include "../Shaders/MatStandard_s.h"

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

const uint8 *g_pVertexShaderData = g_vs_main_s;
unsigned int g_vertexShaderSize  = sizeof(g_vs_main_s);

//const uint8 *g_pPixelShaderData = g_ps_main_s;
//unsigned int g_pixelShaderSize  = sizeof(g_ps_main_s);

ID3D11VertexShader *pVertexShader = NULL;
ID3D11PixelShader *pPixelShader = NULL;


int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	MFCALLSTACK;
	
	g_pd3dDevice->CreateVertexShader(g_pVertexShaderData, g_vertexShaderSize, NULL, &pVertexShader);
	//g_pd3dDevice->CreatePixelShader(g_pPixelShaderData, g_pixelShaderSize, NULL, &pPixelShader);

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;
	
	if (pPixelShader) pPixelShader->Release();
	if (pVertexShader) pVertexShader->Release();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;
	
	g_pImmediateContext->VSSetShader(pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(pPixelShader, NULL, 0);

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}

#endif // MF_RENDERER
