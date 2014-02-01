#include "Fuji.h"

#include "Asset/MFIntShader.h"
#include "MFShader_Internal.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFStringCache.h"

// VS2012 introduces the new Windows SDK, which includes DirectX
#if _MSC_VER < 1700
	#define USE_D3DX
#endif

#if defined(USE_D3DX)
	#include <d3dx9.h>
	#pragma comment(lib, "d3dx9")

	#include <D3DX11async.h>
	#pragma comment(lib, "d3dx11")
#else
	#include <D3Dcompiler.h>
	#pragma comment(lib, "D3dcompiler")
#endif

#if defined(USE_D3DX)
	bool MFIntShader_CompileShaderD3DX9(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
	bool MFIntShader_CompileShaderD3DX11(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
#else
	bool MFIntShader_CompileShaderWinSDK(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
#endif
bool MFIntShader_CompileShaderOpenGL(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);


bool MFIntShader_CreateShader(MFShaderType shaderType, MFShaderMacro *pMacros, const char *pFilename, const char *pShaderSource, int line, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver)
{
	*ppOutput = NULL;
	if(pSize)
		*pSize = 0;

	MFShaderTemplate shaderTemplate;
	MFShaderInput inputs[256];
	MFZeroMemory(&shaderTemplate, sizeof(shaderTemplate));
	shaderTemplate.shaderType = shaderType;
	shaderTemplate.pInputs = inputs;

	MFStringCache *pStrings = MFStringCache_Create(64 * 1024);

#if defined(MF_DEBUG)
	bool bDebug = true;
#else
	bool bDebug = false;
#endif

	bool bSucceeded = false;
	switch(renderDriver)
	{
		case MFRD_D3D9:
#if defined(USE_D3DX)
			bSucceeded = MFIntShader_CompileShaderD3D9(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
#else
			bSucceeded = MFIntShader_CompileShaderWinSDK(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
#endif
			break;
		case MFRD_D3D11:
#if defined(USE_D3DX)
			bSucceeded = MFIntShader_CompileShaderD3D11(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
#else
			bSucceeded = MFIntShader_CompileShaderWinSDK(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
#endif
			break;
		case MFRD_OpenGL:
			bSucceeded = MFIntShader_CompileShaderOpenGL(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
			break;
		default:
			switch(platform)
			{
				case FP_XBox:
					// xbox uses something close to dx8 shaders
					break;
				default:
					break;
			}
	}

	if(!bSucceeded)
		return false;

	// assemble template into file
	size_t stringsSize = MFStringCache_GetSize(pStrings);
	char *pStringData = MFStringCache_GetCache(pStrings);

	size_t size = sizeof(MFShaderTemplate) + shaderTemplate.bytes + sizeof(MFShaderInput)*shaderTemplate.numInputs * stringsSize;
	MFShaderTemplate *pTemplate = (MFShaderTemplate*)MFHeap_AllocAndZero(size);

	pTemplate->shaderType = shaderTemplate.shaderType;

	pTemplate->pInputs = (MFShaderInput*)&pTemplate[1];
	pTemplate->numInputs = shaderTemplate.numInputs;
	MFCopyMemory(pTemplate->pInputs, shaderTemplate.pInputs, sizeof(MFShaderInput)*shaderTemplate.numInputs);

	pTemplate->pProgram = (void*)&pTemplate->pInputs[shaderTemplate.numInputs];
	pTemplate->bytes = shaderTemplate.bytes;
	MFCopyMemory(pTemplate->pProgram, shaderTemplate.pProgram, shaderTemplate.bytes);

	char *pTemplateStrings = (char*)pTemplate->pProgram + shaderTemplate.bytes;
	MFCopyMemory(pTemplateStrings, pStringData, stringsSize);

	// fix down all the pointers
	size_t stringOffset = pTemplateStrings - (char*)pTemplate;
	for(int i = 0; i < pTemplate->numInputs; ++i)
		MFFixUp(pTemplate->pInputs[i].pName, pStringData - stringOffset, 0);
	MFFixUp(pTemplate->pInputs, pTemplate, 0);
	MFFixUp(pTemplate->pProgram, pTemplate, 0);

	// free working data
	MFHeap_Free(shaderTemplate.pProgram);
	MFStringCache_Destroy(pStrings);

	*ppOutput = pTemplate;
	*pSize = size;

	return true;
}

MF_API bool MFIntShader_CreateFromString(MFShaderType shaderType, const char *pShaderSource, const char *pFile, int line, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver)
{
	return MFIntShader_CreateShader(shaderType, pMacros, pFile, pShaderSource, line, ppOutput, pSize, platform, renderDriver);
}

MF_API bool MFIntShader_CreateFromFile(MFShaderType shaderType, const char *pFilename, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver)
{
	return MFIntShader_CreateShader(shaderType, pMacros, pFilename, NULL, 0, ppOutput, pSize, platform, renderDriver);
}


#include <stdio.h>
static HRESULT OpenInclude(bool bSystemInclude, const char *pFileName, const void *pParentData, const void **ppData, uint32 *pBytes)
{
	size_t bytes;
	char *pFile = MFFileSystem_Load(pFileName, &bytes);
	if(!pFile)
		return E_FAIL;

	pFileName = MFFileSystem_ResolveSystemPath(pFileName, true);
	char lineDirective[256];
	int lineBytes = sprintf(lineDirective, "#line 1 \"%s\"\r\n", pFileName);

	char *pInclude = (char*)MFHeap_Alloc(lineBytes + bytes);
	MFCopyMemory(pInclude, lineDirective, lineBytes);
	MFCopyMemory(pInclude + lineBytes, pFile, bytes);
	bytes += lineBytes;
	MFHeap_Free(pFile);

	*ppData = pInclude;
	*pBytes = (UINT)bytes;

	return S_OK;
}

#if defined(USE_D3DX)
class FujiIncludeHandler : public ID3DXInclude
{
	STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		return OpenInclude(IncludeType == D3DXINC_SYSTEM, pFileName, pParentData, ppData, pBytes);
	}

	STDMETHOD(Close)(LPCVOID pData)
	{
		MFHeap_Free((void*)pData);
		return S_OK;
	}
};

bool MFIntShader_CompileShaderD3DX9(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings)
{
	ID3DXBuffer *pProgram = NULL;
	ID3DXBuffer *pErrors = NULL;
	ID3DXConstantTable *pConstantTable = NULL;

	FujiIncludeHandler includeHandler;

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

	const char *pShaderModel = pTemplate->shaderType == MFST_VertexShader ? "vs_3_0" : "ps_3_0";

	DWORD flags = bDebug ? D3DXSHADER_DEBUG | D3DXSHADER_OPTIMIZATION_LEVEL0 : D3DXSHADER_OPTIMIZATION_LEVEL3;

	if(pShaderSource)
	{
		size_t sourceLen = MFString_Length(pShaderSource);
		if(pFilename)
		{
			pFilename = MFFileSystem_ResolveSystemPath(pFilename, true);
			char lineDirective[256];
			int lineBytes = sprintf(lineDirective, "#line %d \"%s\"\r\n", line, pFilename);

			char *pNewSource = (char*)MFHeap_Alloc(lineBytes + sourceLen);
			MFCopyMemory(pNewSource, lineDirective, lineBytes);
			MFCopyMemory(pNewSource + lineBytes, pShaderSource, sourceLen);
			sourceLen += lineBytes;
			pShaderSource = pNewSource;
		}

		HRESULT hr = D3DXCompileShader(pShaderSource, (UINT)sourceLen, pMacros ? macros : NULL, &includeHandler, "main", pShaderModel, flags, &pProgram, &pErrors, &pConstantTable);

		if(pErrors)
		{
			MFDebug_Message((char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		// if we allocated a new buffer to append the #line directive
		if(pFilename)
			MFHeap_Free((char*)pShaderSource);

		if(hr != D3D_OK)
			return false;
	}
	else if(pFilename)
	{
		HRESULT hr = D3DXCompileShaderFromFile(MFFileSystem_ResolveSystemPath(pFilename, true), pMacros ? macros : NULL, NULL, "main", pShaderModel, flags, &pProgram, &pErrors, &pConstantTable);

		if(pErrors)
		{
			MFDebug_Message((char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		if(hr != D3D_OK)
			return false;
	}

	if(pProgram)
	{
		pTemplate->bytes = pProgram->GetBufferSize();
		pTemplate->pProgram = MFHeap_Alloc(pTemplate->bytes);
		MFCopyMemory(pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
		pProgram->Release();
	}

	if(pConstantTable)
	{
		D3DXCONSTANTTABLE_DESC desc;
		pConstantTable->GetDesc(&desc);

		pTemplate->numInputs = (int)desc.Constants;

		for(UINT a = 0; a<desc.Constants; ++a)
		{
			D3DXCONSTANT_DESC constant[4];
			UINT count = 4;

			D3DXHANDLE hConstant = pConstantTable->GetConstant(NULL, a);
			pConstantTable->GetConstantDesc(hConstant, constant, &count);
			MFDebug_Assert(count == 1, "??");

			if(count)
			{
				pTemplate->pInputs[a].pName = MFStringCache_Add(pStrings, constant[0].Name);

				pTemplate->pInputs[a].type = MFShader_IT_Unknown;
				pTemplate->pInputs[a].constantRegister = constant[0].RegisterIndex;
				pTemplate->pInputs[a].numRegisters = constant[0].RegisterCount;

				pTemplate->pInputs[a].numRows = 1;
				pTemplate->pInputs[a].columnMajor = 0;
				pTemplate->pInputs[a].numElements = constant[0].Elements;

				switch(constant[0].Class)
				{
					case D3DXPC_SCALAR:
						switch(constant[0].Type)
						{
							case D3DXPT_FLOAT:
								pTemplate->pInputs[a].type = MFShader_IT_Float;
								break;
							case D3DXPT_INT:
								pTemplate->pInputs[a].type = MFShader_IT_Int;
								break;
							case D3DXPT_BOOL:
								pTemplate->pInputs[a].type = MFShader_IT_Bool;
								break;
							default:
								MFDebug_Assert(false, "??");
						}
						break;
					case D3DXPC_VECTOR:
						MFDebug_Assert(constant[0].Type == D3DXPT_FLOAT, "!!");
						pTemplate->pInputs[a].type = MFShader_IT_Vector;
						break;
					case D3DXPC_MATRIX_COLUMNS:
						inputs[a].columnMajor = 1;
					case D3DXPC_MATRIX_ROWS:
						MFDebug_Assert(constant[0].Type == D3DXPT_FLOAT, "!!");
						pTemplate->pInputs[a].type = MFShader_IT_Matrix;
						pTemplate->pInputs[a].numRows = constant[0].Rows;
						break;
					case D3DXPC_OBJECT:
						MFDebug_Assert(constant[0].Type == D3DXPT_SAMPLER2D, "!!");
						pTemplate->pInputs[a].type = MFShader_IT_Sampler;
						break;
					case D3DXPC_STRUCT:
						MFDebug_Assert(false, "??");
						break;
				}
			}
		}

		pConstantTable->Release();
	}

	return true;
}


bool MFIntShader_CompileShaderD3DX11(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings)
{
	D3D10_SHADER_MACRO macros[256];

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

	ID3D10Blob *pProgram = NULL;
	ID3D10Blob *pErrors = NULL;

	UINT flags = bDebug ? D3D10_SHADER_DEBUG | D3D10_SHADER_OPTIMIZATION_LEVEL0 : D3D10_SHADER_OPTIMIZATION_LEVEL3;

	if(pShaderSource)
	{
		HRESULT hr = D3DX11CompileFromMemory(pShaderSource, MFString_Length(pShaderSource), pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, NULL, &pProgram, &pErrors, NULL);
		if(hr != S_OK)
			return false;
	}
	else if(pFilename)
	{
		HRESULT hr = D3DX11CompileFromFile(pFilename, NULL, NULL, "main", "vs_3_0", flags, 0, NULL, &pProgram, &pErrors, NULL);
		if(hr != S_OK)
			return false;
	}

	if(pProgram)
	{
		pTemplate->bytes = pProgram->GetBufferSize();
		pTemplate->pProgram = MFHeap_Alloc(pTemplate->bytes);
		MFCopyMemory(pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
		pProgram->Release();
	}

	if(pErrors)
		pErrors->Release();

	return true;
}
#else

// code from Wine which reads the constant table from a shader (since ID3DXConstantTable is not supported with D3DCompile)
#include <vector>
#include <string>
#include <cstdint>

enum EREGISTER_SET
{
	RS_BOOL,
	RS_INT4,
	RS_FLOAT4,
	RS_SAMPLER
};

struct ConstantDesc
{
	std::string Name;
	EREGISTER_SET RegisterSet;
	int RegisterIndex;
	int RegisterCount;
	int Rows;
	int Columns;
	int Elements;
	int StructMembers;
	size_t Bytes;
};

class ConstantTable
{
public:
	bool Create(const void* data);

	size_t GetConstantCount() const
	{
		return m_constants.size();
	}
	const std::string& GetCreator() const
	{
		return m_creator;
	}

	const ConstantDesc* GetConstantByIndex(size_t i) const
	{
		return &m_constants[i];
	}
	const ConstantDesc* GetConstantByName(const std::string& name) const;

private:
	std::vector<ConstantDesc> m_constants;
	std::string m_creator;
};

// Structs
struct CTHeader
{
	uint32_t Size;
	uint32_t Creator;
	uint32_t Version;
	uint32_t Constants;
	uint32_t ConstantInfo;
	uint32_t Flags;
	uint32_t Target;
};

struct CTInfo
{
	uint32_t Name;
	uint16_t RegisterSet;
	uint16_t RegisterIndex;
	uint16_t RegisterCount;
	uint16_t Reserved;
	uint32_t TypeInfo;
	uint32_t DefaultValue;
};

struct CTType
{
	uint16_t Class;
	uint16_t Type;
	uint16_t Rows;
	uint16_t Columns;
	uint16_t Elements;
	uint16_t StructMembers;
	uint32_t StructMemberInfo;
};

// Shader instruction opcodes
const uint32_t SIO_COMMENT = 0x0000FFFE;
const uint32_t SIO_END = 0x0000FFFF;
const uint32_t SI_OPCODE_MASK = 0x0000FFFF;
const uint32_t SI_COMMENTSIZE_MASK = 0x7FFF0000;
const uint32_t CTAB_CONSTANT = 0x42415443;

// Member functions
bool ConstantTable::Create(const void* data)
{
	const uint32_t* ptr = static_cast<const uint32_t*>(data);
	while(*++ptr != SIO_END)
	{
		if((*ptr & SI_OPCODE_MASK) == SIO_COMMENT)
		{
			// Check for CTAB comment
			uint32_t comment_size = (*ptr & SI_COMMENTSIZE_MASK) >> 16;
			if(*(ptr + 1) != CTAB_CONSTANT)
			{
				ptr += comment_size;
				continue;
			}

			// Read header
			const char* ctab = reinterpret_cast<const char*>(ptr + 2);
			size_t ctab_size = (comment_size - 1) * 4;

			const CTHeader* header = reinterpret_cast<const CTHeader*>(ctab);
			if(ctab_size < sizeof(*header) || header->Size != sizeof(*header))
				return false;
			m_creator = ctab + header->Creator;

			// Read constants
			m_constants.reserve(header->Constants);
			const CTInfo* info = reinterpret_cast<const CTInfo*>(ctab + header->ConstantInfo);
			for(uint32_t i = 0; i < header->Constants; ++i)
			{
				const CTType* type = reinterpret_cast<const CTType*>(ctab + info[i].TypeInfo);

				// Fill struct
				ConstantDesc desc;
				desc.Name = ctab + info[i].Name;
				desc.RegisterSet = static_cast<EREGISTER_SET>(info[i].RegisterSet);
				desc.RegisterIndex = info[i].RegisterIndex;
				desc.RegisterCount = info[i].RegisterCount;
				desc.Rows = type->Rows;
				desc.Columns = type->Columns;
				desc.Elements = type->Elements;
				desc.StructMembers = type->StructMembers;
				desc.Bytes = 4 * desc.Elements * desc.Rows * desc.Columns;
				m_constants.push_back(desc);
			}
			return true;
		}
	}
	return false;
}

const ConstantDesc* ConstantTable::GetConstantByName(const std::string& name) const
{
	std::vector<ConstantDesc>::const_iterator it;
	for(it = m_constants.begin(); it != m_constants.end(); ++it)
	{
		if(it->Name == name)
			return &(*it);
	}
	return NULL;
}


class FujiIncludeHandler : public ID3DInclude
{
	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		return OpenInclude(IncludeType == D3D_INCLUDE_SYSTEM, pFileName, pParentData, ppData, pBytes);
	}

	HRESULT Close(LPCVOID pData)
	{
		MFHeap_Free((void*)pData);
		return S_OK;
	}
};

bool MFIntShader_CompileShaderWinSDK(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings)
{
	ID3DBlob *pProgram = NULL;
	ID3DBlob *pErrors = NULL;

	FujiIncludeHandler includeHandler;

	D3D_SHADER_MACRO macros[256];
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

	const char *pShaderModel = pTemplate->shaderType == MFST_VertexShader ? "vs_3_0" : "ps_3_0";

	UINT flags = bDebug ? D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0 : D3DCOMPILE_OPTIMIZATION_LEVEL3;

	if(pShaderSource)
	{
		size_t sourceLen = MFString_Length(pShaderSource);
		if(pFilename)
		{
			pFilename = MFFileSystem_ResolveSystemPath(pFilename, true);
			char lineDirective[256];
			int lineBytes = sprintf(lineDirective, "#line %d \"%s\"\r\n", line, pFilename);

			char *pNewSource = (char*)MFHeap_Alloc(lineBytes + sourceLen);
			MFCopyMemory(pNewSource, lineDirective, lineBytes);
			MFCopyMemory(pNewSource + lineBytes, pShaderSource, sourceLen);
			sourceLen += lineBytes;
			pShaderSource = pNewSource;
		}

		HRESULT hr = D3DCompile(pShaderSource, sourceLen, pFilename, pMacros ? macros : NULL, &includeHandler, "main", pShaderModel, flags, 0, &pProgram, &pErrors);

		if(pErrors)
		{
			MFDebug_Message((char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		// if we allocated a new buffer to append the #line directive
		if(pFilename)
			MFHeap_Free((char*)pShaderSource);

		if(hr != S_OK)
			return false;
	}
	else if(pFilename)
	{
		HRESULT hr = D3DCompileFromFile(MFString_UFT8AsWChar(pFilename), pMacros ? macros : NULL, &includeHandler, "main", pShaderModel, flags, 0, &pProgram, &pErrors);

		if(pErrors)
		{
			MFDebug_Message((char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		if(hr != S_OK)
			return false;
	}

	if(pProgram)
	{
		pTemplate->bytes = pProgram->GetBufferSize();
		pTemplate->pProgram = MFHeap_Alloc(pTemplate->bytes);
		MFCopyMemory(pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
		pProgram->Release();
	}

	ConstantTable constants;
	if(pTemplate->pProgram && constants.Create(pTemplate->pProgram))
	{
		size_t numConstants = constants.GetConstantCount();
		pTemplate->numInputs = (int)numConstants;
		for(size_t a = 0; a < numConstants; ++a)
		{
			const ConstantDesc *pConstant = constants.GetConstantByIndex(a);

			pTemplate->pInputs[a].pName = MFStringCache_Add(pStrings, pConstant->Name.c_str());

			pTemplate->pInputs[a].type = MFShader_IT_Unknown;
			pTemplate->pInputs[a].constantRegister = pConstant->RegisterIndex;
			pTemplate->pInputs[a].numRegisters = pConstant->RegisterCount;

			pTemplate->pInputs[a].numRows = 1;
			pTemplate->pInputs[a].columnMajor = 0;
			pTemplate->pInputs[a].numElements = pConstant->Elements;

			switch(pConstant->RegisterSet)
			{
				// TODO: we need to improve this to support scalar float/int's, and detect matrix orientation
				case RS_BOOL:
					pTemplate->pInputs[a].type = MFShader_IT_Bool;
					break;
				case RS_INT4:
					MFDebug_Assert(false, "int vector's not supported...");
					break;
				case RS_FLOAT4:
					// is it a matrix?
					if(pConstant->Rows > 1 && pConstant->Columns > 1)
					{
						pTemplate->pInputs[a].columnMajor = 0; // TODO: how can we find this out?
						pTemplate->pInputs[a].type = MFShader_IT_Matrix;
						pTemplate->pInputs[a].numRows = pConstant->Rows;
					}
					else
						pTemplate->pInputs[a].type = MFShader_IT_Vector;
					break;
				case RS_SAMPLER:
					pTemplate->pInputs[a].type = MFShader_IT_Sampler;
					break;
			}
		}
	}

	// TODO: sould we compress or strip the shader?
	//D3DStripShader
	//D3DCompressShaders // Note: this might be used to combine many shaders in an effect?

	return true;
}
#endif

// TODO: this is a lot of work... ideally, we would support those libs that convert HLSL -> GLSL
bool MFIntShader_CompileShaderOpenGL(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings)
{
	if(pShaderSource)
	{
		pTemplate->bytes = MFString_Length(pShaderSource) + 1;
		pTemplate->pProgram = (char*)MFCopyMemory(MFHeap_Alloc(pTemplate->bytes), pShaderSource, pTemplate->bytes);
	}
	else if(pFilename)
	{
		pTemplate->pProgram = MFFileSystem_Load(pFilename, &pTemplate->bytes, 1);
		pTemplate->bytes += 1;
	}

	return true;
}
