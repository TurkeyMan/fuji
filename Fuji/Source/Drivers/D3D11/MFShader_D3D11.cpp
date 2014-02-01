#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_D3D11
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_D3D11
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_D3D11
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_D3D11
#endif

#include "MFShader_Internal.h"
#include "MFFileSystem.h"

#include <d3d11.h>


extern ID3D11Device *g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;


void MFShader_InitModulePlatformSpecific()
{
}

void MFShader_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader)
{
	MFShaderTemplate *pTemplate = pShader->pTemplate;
	if(pTemplate->pProgram)
	{
		switch(pTemplate->shaderType)
		{
			case MFST_VertexShader:
			{
				ID3D11VertexShader *pVS;
				HRESULT hr = g_pd3dDevice->CreateVertexShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pVS);
				if(hr == S_OK)
					pShader->pPlatformData = pVS;
				break;
			}
			case MFST_PixelShader:
			{
				ID3D11PixelShader *pPS;
				HRESULT hr = g_pd3dDevice->CreatePixelShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pPS);
				if(hr == S_OK)
					pShader->pPlatformData = pPS;
				break;
			}
			case MFST_GeometryShader:
			{
				ID3D11GeometryShader *pGS;
				HRESULT hr = g_pd3dDevice->CreateGeometryShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pGS);
				if(hr == S_OK)
					pShader->pPlatformData = pGS;
				break;
			}
			case MFST_DomainShader:
			{
				ID3D11DomainShader *pDS;
				HRESULT hr = g_pd3dDevice->CreateDomainShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pDS);
				if(hr == S_OK)
					pShader->pPlatformData = pDS;
				break;
			}
			case MFST_HullShader:
			{
				ID3D11HullShader *pHS;
				HRESULT hr = g_pd3dDevice->CreateHullShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pHS);
				if(hr == S_OK)
					pShader->pPlatformData = pHS;
				break;
			}
			case MFST_ComputeShader:
			{
				ID3D11ComputeShader *pCS;
				HRESULT hr = g_pd3dDevice->CreateComputeShader(pTemplate->pProgram, pTemplate->bytes, NULL, &pCS);
				if(hr == S_OK)
					pShader->pPlatformData = pCS;
				break;
			}
			default:
				MFUNREACHABLE;
		}
	}

	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	switch(pShader->pTemplate->shaderType)
	{
		case MFST_VertexShader:
		{
			ID3D11VertexShader *pVS = (ID3D11VertexShader*)pShader->pPlatformData;
			pVS->Release();
			break;
		}
		case MFST_PixelShader:
		{
			ID3D11PixelShader *pPS = (ID3D11PixelShader*)pShader->pPlatformData;
			pPS->Release();
			break;
		}
		case MFST_GeometryShader:
		{
			ID3D11GeometryShader *pGS = (ID3D11GeometryShader*)pShader->pPlatformData;
			pGS->Release();
			break;
		}
		case MFST_DomainShader:
		{
			ID3D11DomainShader *pDS = (ID3D11DomainShader*)pShader->pPlatformData;
			pDS->Release();
			break;
		}
		case MFST_HullShader:
		{
			ID3D11HullShader *pHS = (ID3D11HullShader*)pShader->pPlatformData;
			pHS->Release();
			break;
		}
		case MFST_ComputeShader:
		{
			ID3D11ComputeShader *pCS = (ID3D11ComputeShader*)pShader->pPlatformData;
			pCS->Release();
			break;
		}
		default:
			MFUNREACHABLE;
	}
}

#endif // MF_RENDERER
