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

	const char *pShaderModel = pShader->shaderType == MFST_VertexShader ? "vs_3_0" : "ps_3_0";

#if defined(MF_DEBUG)
	DWORD flags = D3DXSHADER_DEBUG | D3DXSHADER_OPTIMIZATION_LEVEL0;
#else
	DWORD flags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif
	if(pFilename)
	{
		HRESULT hr = D3DXCompileShaderFromFile(MFFileSystem_ResolveSystemPath(pFilename), pMacros ? macros : NULL, NULL, "main", pShaderModel, flags, &pProgram, &pErrors, &pConstantTable);

		if(pErrors)
		{
			MFDebug_Warn(1, (char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		if(hr != D3D_OK)
			return false;
	}
	else if(pSource)
	{
		HRESULT hr = D3DXCompileShader(pSource, MFString_Length(pSource), pMacros ? macros : NULL, NULL, "main", pShaderModel, flags, &pProgram, &pErrors, &pConstantTable);

		if(pErrors)
		{
			MFDebug_Warn(1, (char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

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

	if(pConstantTable)
	{
		D3DXCONSTANTTABLE_DESC desc;
		pConstantTable->GetDesc(&desc);

		MFShaderInput inputs[256];

		size_t nameBytes = 0;
		for(UINT a=0; a<desc.Constants; ++a)
		{
			D3DXCONSTANT_DESC constant[4];
			UINT count = 4;

			D3DXHANDLE hConstant = pConstantTable->GetConstant(NULL, a);
			pConstantTable->GetConstantDesc(hConstant, constant, &count);
			MFDebug_Assert(count == 1, "??");

			if(count)
			{
				inputs[a].pName = constant[0].Name;
				nameBytes += MFString_Length(constant[0].Name) + 1;

				inputs[a].type = MFShader_IT_Unknown;
				inputs[a].constantRegister = constant[0].RegisterIndex;
				inputs[a].numRegisters = constant[0].RegisterCount;

				inputs[a].numRows = 1;
				inputs[a].columnMajor = 0;
				inputs[a].numElements = constant[0].Elements;

				switch(constant[0].Class)
				{
					case D3DXPC_SCALAR:
						switch(constant[0].Type)
						{
							case D3DXPT_FLOAT:
								inputs[a].type = MFShader_IT_Float;
								break;
							case D3DXPT_INT:
								inputs[a].type = MFShader_IT_Int;
								break;
							case D3DXPT_BOOL:
								inputs[a].type = MFShader_IT_Bool;
								break;
							default:
								MFDebug_Assert(false, "??");
						}
						break;
					case D3DXPC_VECTOR:
						MFDebug_Assert(constant[0].Type == D3DXPT_FLOAT, "!!");
						inputs[a].type = MFShader_IT_Vector;
						break;
					case D3DXPC_MATRIX_COLUMNS:
						inputs[a].columnMajor = 1;
					case D3DXPC_MATRIX_ROWS:
						MFDebug_Assert(constant[0].Type == D3DXPT_FLOAT, "!!");
						inputs[a].type = MFShader_IT_Matrix;
						inputs[a].numRows = constant[0].Rows;
						break;
					case D3DXPC_OBJECT:
						MFDebug_Assert(constant[0].Type == D3DXPT_SAMPLER2D, "!!");
						inputs[a].type = MFShader_IT_Sampler;
						break;
					case D3DXPC_STRUCT:
						MFDebug_Assert(false, "??");
						break;
				}
			}
		}

		pShader->pInputs = (MFShaderInput*)MFHeap_Alloc(desc.Constants*sizeof(MFShaderInput) + nameBytes);
		pShader->numInputs = desc.Constants;

		MFCopyMemory(pShader->pInputs, inputs, desc.Constants*sizeof(MFShaderInput));

		char *pName = (char*)(pShader->pInputs + desc.Constants);
		for(UINT a=0; a<desc.Constants; ++a)
		{
			pShader->pInputs[a].pName = MFString_Copy(pName, pShader->pInputs[a].pName);
			pName += MFString_Length(pName) + 1;
		}

		pConstantTable->Release();
	}

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
