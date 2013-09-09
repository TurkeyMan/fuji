#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_D3D9
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_D3D9
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_D3D9
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_D3D9
#endif

#include "MFShader_Internal.h"
#include "MFFileSystem.h"

#include <d3dx9.h>

extern IDirect3DDevice9 *pd3dDevice;


void MFShader_InitModulePlatformSpecific()
{
}

void MFShader_DeinitModulePlatformSpecific()
{
}

bool MFShader_CreatePlatformSpecific(MFShader *pShader, MFShaderMacro *pMacros, const char *pFilename, const char *pSource)
{
	ID3DXBuffer *pProgram = NULL;
	ID3DXBuffer *pErrors = NULL;
	ID3DXConstantTable *pConstantTable = NULL;

	D3DXMACRO macros[256];
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

#if defined(MF_DEBUG)
	DWORD flags = D3DXSHADER_DEBUG | D3DXSHADER_OPTIMIZATION_LEVEL0;
#else
	DWORD flags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif
	if(pFilename)
	{
		HRESULT hr = D3DXCompileShaderFromFile(MFFile_SystemPath(pFilename), pMacros ? macros : NULL, NULL, "main", "vs_2_0", flags, &pProgram, &pErrors, &pConstantTable);
		if(hr != D3D_OK)
			return false;
	}
	else if(pSource)
	{
		HRESULT hr = D3DXCompileShader(pSource, MFString_Length(pSource), pMacros ? macros : NULL, NULL, "main", "vs_2_0", flags, &pProgram, &pErrors, &pConstantTable);
		if(hr != D3D_OK)
			return false;
	}

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
				IDirect3DVertexShader9 *pVS;
				HRESULT hr = pd3dDevice->CreateVertexShader((const DWORD*)pShader->pProgram, &pVS);
				if(hr == D3D_OK)
					pShader->pPlatformData = pVS;
				break;
			}
			case MFST_PixelShader:
			{
				IDirect3DPixelShader9 *pPS;
				HRESULT hr = pd3dDevice->CreatePixelShader((const DWORD*)pShader->pProgram, &pPS);
				if(hr == D3D_OK)
					pShader->pPlatformData = pPS;
				break;
			}
			case MFST_GeometryShader:
			case MFST_DomainShader:
			case MFST_HullShader:
			case MFST_ComputeShader:
				MFDebug_Assert(false, "Shader type not supported in D3D9!");
				break;
			default:
				MFUNREACHABLE;
		}
	}

	if(pErrors)
		pErrors->Release();
	if(pConstantTable)
		pConstantTable->Release();

	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	switch(pShader->shaderType)
	{
		case MFST_VertexShader:
		{
			IDirect3DVertexShader9 *pVS = (IDirect3DVertexShader9*)pShader->pPlatformData;
			pVS->Release();
			break;
		}
		case MFST_PixelShader:
		{
			IDirect3DPixelShader9 *pPS = (IDirect3DPixelShader9*)pShader->pPlatformData;
			pPS->Release();
			break;
		}
		case MFST_GeometryShader:
		case MFST_DomainShader:
		case MFST_HullShader:
		case MFST_ComputeShader:
			MFDebug_Assert(false, "Not supported in D3D9!");
			break;
		default:
			MFUNREACHABLE;
	}
}

#endif // MF_RENDERER
