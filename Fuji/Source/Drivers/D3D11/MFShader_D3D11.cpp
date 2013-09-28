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

#define USE_D3DX11
#if defined(USE_D3DX11)
	#include <D3DX11async.h>
#else
	#include <D3Dcompiler.h>
#endif

extern ID3D11Device *g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;


void MFShader_InitModulePlatformSpecific()
{
}

void MFShader_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader, MFShaderMacro *pMacros, const char *pFilename, const char *pSource)
{
#if defined(USE_D3DX11)
	D3D10_SHADER_MACRO macros[256];
#else
	D3D_SHADER_MACRO macros[256];
#endif
	if(pMacros)
	{
		int a = 0;
		do
		{
			macros[a].Name = pMacros[a].pDefine;
			macros[a].Definition = pMacros[a].pValue;
		}
		while(pMacros[a++].pDefine != NULL);
	}

#if defined(USE_D3DX11)
	ID3D10Blob *pProgram = NULL;
	ID3D10Blob *pErrors = NULL;

#if defined(MF_DEBUG)
	UINT flags = D3D10_SHADER_DEBUG | D3D10_SHADER_OPTIMIZATION_LEVEL0;
#else
	UINT flags = D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif
	if(pSource)
	{
		HRESULT hr = D3DX11CompileFromMemory(pSource, MFString_Length(pSource), pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, NULL, &pProgram, &pErrors, NULL);
		if(hr != S_OK)
			return false;
	}
	else if(pFilename)
	{
		HRESULT hr = D3DX11CompileFromFile(pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, NULL, &pProgram, &pErrors, NULL);
		if(hr != S_OK)
			return false;
	}
#else
	ID3DBlob *pProgram = NULL;
	ID3DBlob *pErrors = NULL;

#if defined(MF_DEBUG)
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
	UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	if(pSource)
	{
		HRESULT hr = D3DCompile(pSource, MFString_Length(pSource), pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, &pProgram, &pErrors);
//		HRESULT hr = D3DCompile2(pSource, MFString_Length(pSource), pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, &pProgram, &pErrors, NULL);
		if(hr != S_OK)
			return false;
	}
	else if(pFilename)
	{
//		HRESULT hr = D3DCompileFromFile(pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, &pProgram, &pErrors);
//		if(hr != S_OK)
			return false;
	}
#endif

	if(pProgram)
	{
		pShader->bytes = pProgram->GetBufferSize();
		pShader->pProgram = MFHeap_Alloc(pShader->bytes);
		MFCopyMemory(pShader->pProgram, pProgram->GetBufferPointer(), pShader->bytes);
		pProgram->Release();
	}

	if(pShader->pProgram)
	{
		switch(pShader->shaderType)
		{
			case MFST_VertexShader:
			{
				ID3D11VertexShader *pVS;
				HRESULT hr = g_pd3dDevice->CreateVertexShader(pShader->pProgram, pShader->bytes, NULL, &pVS);
				if(hr == S_OK)
					pShader->pPlatformData = pVS;
				break;
			}
			case MFST_PixelShader:
			{
				ID3D11PixelShader *pPS;
				HRESULT hr = g_pd3dDevice->CreatePixelShader(pShader->pProgram, pShader->bytes, NULL, &pPS);
				if(hr == S_OK)
					pShader->pPlatformData = pPS;
				break;
			}
			case MFST_GeometryShader:
			{
				ID3D11GeometryShader *pGS;
				HRESULT hr = g_pd3dDevice->CreateGeometryShader(pShader->pProgram, pShader->bytes, NULL, &pGS);
				if(hr == S_OK)
					pShader->pPlatformData = pGS;
				break;
			}
			case MFST_DomainShader:
			{
				ID3D11DomainShader *pDS;
				HRESULT hr = g_pd3dDevice->CreateDomainShader(pShader->pProgram, pShader->bytes, NULL, &pDS);
				if(hr == S_OK)
					pShader->pPlatformData = pDS;
				break;
			}
			case MFST_HullShader:
			{
				ID3D11HullShader *pHS;
				HRESULT hr = g_pd3dDevice->CreateHullShader(pShader->pProgram, pShader->bytes, NULL, &pHS);
				if(hr == S_OK)
					pShader->pPlatformData = pHS;
				break;
			}
			case MFST_ComputeShader:
			{
				ID3D11ComputeShader *pCS;
				HRESULT hr = g_pd3dDevice->CreateComputeShader(pShader->pProgram, pShader->bytes, NULL, &pCS);
				if(hr == S_OK)
					pShader->pPlatformData = pCS;
				break;
			}
			default:
				MFUNREACHABLE;
		}
	}

	if(pErrors)
		pErrors->Release();

	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	switch(pShader->shaderType)
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
