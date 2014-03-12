#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFShader_InitModulePlatformSpecific MFShader_InitModulePlatformSpecific_D3D9
	#define MFShader_DeinitModulePlatformSpecific MFShader_DeinitModulePlatformSpecific_D3D9
	#define MFShader_CreatePlatformSpecific MFShader_CreatePlatformSpecific_D3D9
	#define MFShader_DestroyPlatformSpecific MFShader_DestroyPlatformSpecific_D3D9
#endif

#include "MFShader_Internal.h"
#include "MFFileSystem.h"
#include "MFRenderer_D3D9.h"


extern IDirect3DDevice9 *pd3dDevice;


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
				IDirect3DVertexShader9 *pVS;
				HRESULT hr = pd3dDevice->CreateVertexShader((const DWORD*)pTemplate->pProgram, &pVS);
				if(hr == D3D_OK)
					pShader->pPlatformData = pVS;
				break;
			}
			case MFST_PixelShader:
			{
				IDirect3DPixelShader9 *pPS;
				HRESULT hr = pd3dDevice->CreatePixelShader((const DWORD*)pTemplate->pProgram, &pPS);
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

	return true;
}

void MFShader_DestroyPlatformSpecific(MFShader *pShader)
{
	switch(pShader->pTemplate->shaderType)
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
