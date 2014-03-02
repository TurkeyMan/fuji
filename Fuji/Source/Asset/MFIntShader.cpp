#include "Fuji.h"

#include "Asset/MFIntShader.h"
#include "MFShader_Internal.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFStringCache.h"

#if defined(MF_WINDOWS) || defined(MF_LINUX)
#define SUPPORT_HLSL
#define SUPPORT_HLSL2GLSL
#define SUPPORT_GLSL_OPTIMIZER
#endif
#define SUPPORT_GLSL
#if defined(MF_WINDOWS) // TODO: support Cg compiler on linux/mac?
//#define SUPPORT_CG
#endif

#if defined(MF_WINDOWS)
	#define SUPPORT_D3D
#endif
#if defined(MF_WINDOWS) || defined(MF_LINUX) || defined(MF_OSX) || defined(MF_ANDROID) || defined(MF_IPHONE)
	#define SUPPORT_OPENGL

	#if defined(SUPPORT_HLSL)
		#if defined(SUPPORT_HLSL2GLSL)
			#include "hlsl2glslfork/hlsl2glsl.h"

            #if defined(MF_COMPILER_VISUALC)
                #if defined(NDEBUG)
                    #pragma comment(lib, "hlsl2glsl")
                #else
                    #pragma comment(lib, "hlsl2glsl_d")
                #endif
            #endif
		#endif

		#if defined(SUPPORT_GLSL_OPTIMIZER)
			#include "glsl_optimizer/glsl_optimizer.h"

            #if defined(MF_COMPILER_VISUALC)
                #if defined(NDEBUG)
                    #pragma comment(lib, "glsl_optimizer")
                #else
                    #pragma comment(lib, "glsl_optimizer_d")
                #endif
			#endif
		#endif
	#endif
#endif
#if defined(SUPPORT_CG) && (defined(SUPPORT_D3D) || defined(SUPPORT_OPENGL))
	#include "Cg/cg.h"
	#pragma comment(lib, "cg")

	#if defined(SUPPORT_D3D)
		#include "Cg/cgD3D9.h"
		#pragma comment(lib, "cgD3D9")

		#include "Cg/cgD3D11.h"
		#pragma comment(lib, "cgD3D11")
	#endif
	#if defined(SUPPORT_OPENGL)
		#include "Cg/cgGL.h"
		#pragma comment(lib, "cgGL")
	#endif
#endif

#if defined(SUPPORT_D3D)
	// VS2012 introduces the new Windows SDK, which includes DirectX
	#if defined(_MSC_VER) && _MSC_VER < 1700
		#define USE_D3DX
	#endif

	#if defined(SUPPORT_HLSL)
		#if defined(USE_D3DX)
			#include <d3dx9.h>
			#pragma comment(lib, "d3dx9")

			#include <D3DX11async.h>
			#pragma comment(lib, "d3dx11")

			bool MFIntShader_CompileShaderD3DX9(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
			bool MFIntShader_CompileShaderD3DX11(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
		#else
			#include <D3Dcompiler.h>
			#pragma comment(lib, "D3dcompiler")

			bool MFIntShader_CompileShaderWinSDK(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings);
		#endif
	#endif
#endif
#if defined(SUPPORT_OPENGL)
	bool MFIntShader_CompileShaderOpenGL(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings, MFShaderLanguage language, bool bGLES);
#endif
#if defined(SUPPORT_CG)
	bool MFIntShader_CompileShaderCg(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings, MFShaderLanguage language, MFRendererDrivers renderDriver);
#endif


bool MFIntShader_CreateShader(MFShaderType shaderType, MFShaderMacro *pMacros, const char *pFilename, const char *pShaderSource, int line, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language)
{
	*ppOutput = NULL;
	if(pSize)
		*pSize = 0;

#if !defined(SUPPORT_HLSL)
	MFDebug_Assert(language != MFSL_HLSL, "Unable to build HLSL shaders!");
	if(language == MFSL_HLSL)
		return false;
#endif
#if !defined(SUPPORT_GLSL)
	MFDebug_Assert(language != MFSL_GLSL, "Unable to build GLSL shaders!");
	if(language == MFSL_GLSL)
		return false;
#endif
#if !defined(SUPPORT_CG)
	MFDebug_Assert(language != MFSL_Cg, "Unable to build Cg shaders!");
	if(language == MFSL_Cg)
		return false;
#endif

	MFDebug_Assert(language != MFSL_Unknown, "Unknown shader language!");
	if(language == MFSL_Unknown)
		return false;

	MFShaderTemplate shaderTemplate;
	MFShaderInput inputs[256];
	MFZeroMemory(&shaderTemplate, sizeof(shaderTemplate));
	shaderTemplate.shaderType = shaderType;
	shaderTemplate.pInputs = inputs;

	MFStringCache *pStrings = MFStringCache_Create(64 * 1024);

#if defined(DEBUG)
	bool bDebug = true;
#else
	bool bDebug = false;
#endif

	bool bSucceeded = false;
	switch(renderDriver)
	{
		case MFRD_D3D9:
			if(language == MFSL_HLSL)
			{
#if defined(SUPPORT_D3D) && defined(SUPPORT_HLSL)
	#if defined(USE_D3DX)
				bSucceeded = MFIntShader_CompileShaderD3DX9(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
	#else
				bSucceeded = MFIntShader_CompileShaderWinSDK(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
	#endif
#else
				MFDebug_Assert(false, "D3D9 not supported!");
#endif
			}
			else if(language == MFSL_GLSL)
			{
				MFDebug_Assert(false, "Unable to build GLSL shaders for D3D9!");
			}
			else if(language == MFSL_Cg)
			{
#if defined(SUPPORT_CG)
				bSucceeded = MFIntShader_CompileShaderCg(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings, language, renderDriver);
#endif
			}
			break;
		case MFRD_D3D11:
			if(language == MFSL_HLSL)
			{
#if defined(SUPPORT_D3D) && defined(SUPPORT_HLSL)
	#if defined(USE_D3DX)
				bSucceeded = MFIntShader_CompileShaderD3DX11(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
	#else
				bSucceeded = MFIntShader_CompileShaderWinSDK(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings);
	#endif
#else
				MFDebug_Assert(false, "D3D11 not supported!");
#endif
			}
			else if(language == MFSL_GLSL)
			{
				MFDebug_Assert(false, "Unable to build GLSL shaders for D3D11!");
			}
			else if(language == MFSL_Cg)
			{
#if defined(SUPPORT_CG)
				bSucceeded = MFIntShader_CompileShaderCg(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings, language, renderDriver);
#endif
			}
			break;
		case MFRD_OpenGL:
			if(language == MFSL_HLSL || language == MFSL_GLSL)
			{
#if defined(SUPPORT_OPENGL)
				bSucceeded = MFIntShader_CompileShaderOpenGL(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings, language, false);
#else
				MFDebug_Assert(false, "OpenGL not supported!");
#endif
			}
			else if(language == MFSL_Cg)
			{
#if defined(SUPPORT_CG)
				bSucceeded = MFIntShader_CompileShaderCg(&shaderTemplate, pMacros, bDebug, pFilename, pShaderSource, line, pStrings, language, renderDriver);
#endif
			}
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
	MFCopyMemory((void*)pTemplate->pProgram, shaderTemplate.pProgram, shaderTemplate.bytes);

	char *pTemplateStrings = (char*)pTemplate->pProgram + shaderTemplate.bytes;
	MFCopyMemory(pTemplateStrings, pStringData, stringsSize);

	// fix down all the pointers
	size_t stringOffset = pTemplateStrings - (char*)pTemplate;
	for(int i = 0; i < pTemplate->numInputs; ++i)
		MFFixUp(pTemplate->pInputs[i].pName, pStringData - stringOffset, 0);
	MFFixUp(pTemplate->pInputs, pTemplate, 0);
	MFFixUp(pTemplate->pProgram, pTemplate, 0);

	// free working data
	MFHeap_Free((void*)shaderTemplate.pProgram);
	MFStringCache_Destroy(pStrings);

	*ppOutput = pTemplate;
	*pSize = size;

	return true;
}

static MFShaderLanguage MFIntShader_GetLanguageFromFilename(const char *pFilename)
{
		const char *pExt = MFString_GetFileExtension(pFilename);
		if(!MFString_CaseCmp(pExt, ".hlsl"))
			return MFSL_HLSL;
		else if(!MFString_CaseCmp(pExt, ".glsl"))
			return MFSL_HLSL;
		else if(!MFString_CaseCmp(pExt, ".cg"))
			return MFSL_Cg;
		else if(!MFString_CaseCmp(pExt, ".vsh"))
			return MFSL_VSAsm;
		else if(!MFString_CaseCmp(pExt, ".psh"))
			return MFSL_PSAsm;
		return MFSL_Unknown;
}

MF_API bool MFIntShader_CreateFromString(MFShaderType shaderType, const char *pShaderSource, const char *pFile, int line, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language)
{
	if(language == MFSL_Unknown && pFile)
		language = MFIntShader_GetLanguageFromFilename(pFile);
	return MFIntShader_CreateShader(shaderType, pMacros, pFile, pShaderSource, line, ppOutput, pSize, platform, renderDriver, language);
}

MF_API bool MFIntShader_CreateFromFile(MFShaderType shaderType, const char *pFilename, MFShaderMacro *pMacros, void **ppOutput, size_t *pSize, MFPlatform platform, MFRendererDrivers renderDriver, MFShaderLanguage language)
{
	if(language == MFSL_Unknown)
		language = MFIntShader_GetLanguageFromFilename(pFilename);
	return MFIntShader_CreateShader(shaderType, pMacros, pFilename, NULL, 0, ppOutput, pSize, platform, renderDriver, language);
}


#include <stdio.h>
static bool OpenInclude(bool bSystemInclude, const char *pFileName, const void *pParentData, const void **ppData, uint32 *pBytes)
{
	size_t bytes;
	char *pFile = MFFileSystem_Load(pFileName, &bytes);
	if(!pFile)
		return false;

	pFileName = MFFileSystem_ResolveSystemPath(pFileName, true);
	char lineDirective[256];
	int lineBytes = sprintf(lineDirective, "#line 1 \"%s\"\r\n", pFileName);

	char *pInclude = (char*)MFHeap_Alloc(lineBytes + bytes + 1);
	MFCopyMemory(pInclude, lineDirective, lineBytes);
	MFCopyMemory(pInclude + lineBytes, pFile, bytes);
	bytes += lineBytes;
	pInclude[bytes] = 0;
	MFHeap_Free(pFile);

	*ppData = pInclude;
	*pBytes = (uint32)bytes;

	return true;
}

#if defined(SUPPORT_D3D) && defined(SUPPORT_HLSL)
#if defined(USE_D3DX)
class FujiIncludeHandler : public ID3DXInclude
{
	STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		return OpenInclude(IncludeType == D3DXINC_SYSTEM, pFileName, pParentData, ppData, pBytes) ? S_OK : E_FAIL;
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
		MFCopyMemory((void*)pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
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
						pTemplate->pInputs[a].columnMajor = 1;
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
		MFCopyMemory((void*)pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
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
		return OpenInclude(IncludeType == D3D_INCLUDE_SYSTEM, pFileName, pParentData, ppData, pBytes) ? S_OK : E_FAIL;
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

	size_t sourceLen;
	bool bFromFile = false;
	if(!pShaderSource)
	{
		// load the shader from file...
		pShaderSource = MFFileSystem_Load(pFilename, &sourceLen);
		bFromFile = true;
	}
	else
	{
		sourceLen = MFString_Length(pShaderSource);
	}

	if(pShaderSource)
	{
		if(pFilename)
		{
			pFilename = MFFileSystem_ResolveSystemPath(pFilename, true);
			char lineDirective[256];
			int lineBytes = sprintf(lineDirective, "#line %d \"%s\"\r\n", line, pFilename);

			char *pNewSource = (char*)MFHeap_Alloc(lineBytes + sourceLen);
			MFCopyMemory(pNewSource, lineDirective, lineBytes);
			MFCopyMemory(pNewSource + lineBytes, pShaderSource, sourceLen);
			sourceLen += lineBytes;

			if(bFromFile)
				MFHeap_Free((char*)pShaderSource);

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
	else
	{
		MFDebug_Warn(2, MFStr("Couldn't load shader: %s", pFilename));
		return false;
	}

	if(pProgram)
	{
		pTemplate->bytes = pProgram->GetBufferSize();
		pTemplate->pProgram = MFHeap_Alloc(pTemplate->bytes);
		MFCopyMemory((void*)pTemplate->pProgram, pProgram->GetBufferPointer(), pTemplate->bytes);
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
#endif

#if defined(SUPPORT_OPENGL)

static char *MFIntShader_Preprocess(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line)
{
	char *pSource = (char*)pShaderSource;
	bool bSourceAllocated = false;

	const char *pScan = pSource;
	const char *pOffset;
	while((pOffset = MFString_Chr(pScan, '#')) != NULL)
	{
		if(!MFString_CompareN(pOffset, "#include", 8))
		{
			// find end of line, and end of include path
			const char *pEndLine = MFString_Chr(pOffset + 8, '\n');
			while(MFIsNewline(pEndLine[-1]))
				--pEndLine;
			const char *pEndInclude = pEndLine;
			while(pEndInclude > pOffset + 8 && MFIsWhite(pEndInclude[-1]))
				--pEndInclude;
			const char *pInclude = MFStrN(pOffset + 8, pEndInclude - pOffset - 8);
			while(MFIsWhite(*pInclude))
				++pInclude;

			// pInclude should be our include string
			size_t len = MFString_Length(pInclude);
			if(len == 0)
				return NULL;

			// see if it is a system include
			bool bSystem = false;
			if(*pInclude == '<' && pInclude[len-1] == '>')
				bSystem = true;
			else if(*pInclude != '"' || pInclude[len-1] != '"')
				return NULL;
			pInclude = MFStrN(pInclude + 1, len - 2);

			// load the include file
			const char *pIncludeFile;
			uint32 includeLen;
			if(!OpenInclude(bSystem, pInclude, NULL, (const void**)&pIncludeFile, &includeLen))
				return NULL;

			const char *pPreprocessedInclude = MFIntShader_Preprocess(pTemplate, pMacros, bDebug, pInclude, pIncludeFile, 0);
			if(pIncludeFile != pPreprocessedInclude)
				MFHeap_Free((char*)pIncludeFile);

			// concatenate the current file on either side
			size_t newSize = pOffset - pSource + includeLen + MFString_Length(pEndLine) + 1;
			char *pNewSource = (char*)MFHeap_Alloc(newSize);
			MFCopyMemory(pNewSource, pSource, pOffset - pScan);
			MFCopyMemory(pNewSource + (pOffset - pScan), pPreprocessedInclude, includeLen);
			MFString_Copy(pNewSource + (pOffset - pScan) + includeLen, pEndLine);

			// release intermediates
			MFHeap_Free((void*)pPreprocessedInclude);
			if(bSourceAllocated)
				MFHeap_Free((char*)pSource);

			// we're done!
			pSource = pNewSource;
			pOffset = pSource + (pOffset - pScan) + includeLen;
		}
		else if(!MFString_CompareN(pOffset, "#define", 7))
		{
			MFDebug_Assert(false, "Unsupported preprocessor directive: #define");
		}
		else if(!MFString_CompareN(pOffset, "#if", 3))
		{
			MFDebug_Assert(false, "Unsupported preprocessor directive: #if");
		}
		else if(!MFString_CompareN(pOffset, "#elif", 5))
		{
			MFDebug_Assert(false, "Unsupported preprocessor directive: #elif");
		}
		else if(!MFString_CompareN(pOffset, "#else", 5))
		{
			MFDebug_Assert(false, "Unsupported preprocessor directive: #else");
		}

		pScan = pOffset + 1;
	}

	return pSource;
}

#if defined(SUPPORT_HLSL) && defined(SUPPORT_HLSL2GLSL)
static EAttribSemantic sSemantics[] = {
	EAttrSemPosition,
	EAttrSemPosition1,
	EAttrSemPosition2,
	EAttrSemPosition3,
	EAttrSemNormal,
	EAttrSemNormal1,
	EAttrSemNormal2,
	EAttrSemNormal3,
	EAttrSemColor0,
	EAttrSemColor1,
	EAttrSemColor2,
	EAttrSemColor3,
	EAttrSemTex0,
	EAttrSemTex1,
	EAttrSemTex2,
	EAttrSemTex3,
	EAttrSemTex4,
	EAttrSemTex5,
	EAttrSemTex6,
	EAttrSemTex7,
	EAttrSemTex8,
	EAttrSemTex9,
	EAttrSemTangent,
	EAttrSemTangent1,
	EAttrSemTangent2,
	EAttrSemTangent3,
	EAttrSemBinormal,
	EAttrSemBinormal1,
	EAttrSemBinormal2,
	EAttrSemBinormal3,
	EAttrSemBlendWeight,
	EAttrSemBlendWeight1,
	EAttrSemBlendWeight2,
	EAttrSemBlendWeight3,
	EAttrSemBlendIndices,
	EAttrSemBlendIndices1,
	EAttrSemBlendIndices2,
	EAttrSemBlendIndices3
};

static const char *sSemanticNames[] = {
	"vPos",
	"vPos1",
	"vPos2",
	"vPos3",
	"vNormal",
	"vNormal1",
	"vNormal2",
	"vNormal3",
	"vColour0",
	"vColour1",
	"vColour2",
	"vColour3",
	"vUV0",
	"vUV1",
	"vUV2",
	"vUV3",
	"vUV4",
	"vUV5",
	"vUV6",
	"vUV7",
	"vUV8",
	"vUV9",
	"vTangent",
	"vTangent1",
	"vTangent2",
	"vTangent3",
	"vBiNormal",
	"vBiNormal1",
	"vBiNormal2",
	"vBiNormal3",
	"vWeights",
	"vWeights1",
	"vWeights2",
	"vWeights3",
	"vIndices",
	"vIndices1",
	"vIndices2",
	"vIndices3"
};

const char *MFIntShader_TranslateShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename)
{
	static bool bIsInitialised = false;
	if(!bIsInitialised)
	{
		Hlsl2Glsl_Initialize();
		bIsInitialised = true;
	}

	// Work out which language we should use...
	EShLanguage language;
	switch(type)
	{
		case MFST_VertexShader:	language = EShLangVertex;	break;
		case MFST_PixelShader:	language = EShLangFragment;	break;
		default:
			MFDebug_Assert(false, "hlsl2glsl can only convert vertex and pixel shaders.");
			return NULL;
	}

	ETargetVersion version = bGLES ? ETargetGLSL_ES_100 : ETargetGLSL_120;

	// create a compiler
	ShHandle compiler = Hlsl2Glsl_ConstructCompiler(language);

	// parse the code
	int success = Hlsl2Glsl_Parse(compiler, pShaderSource, version, 0);

	// set translation options
	MFDebug_Assert(sizeof(sSemantics)/sizeof(sSemantics[0]) == sizeof(sSemanticNames)/sizeof(sSemanticNames[0]), "Mismatching attribute declaration arrays!");
	Hlsl2Glsl_SetUserAttributeNames(compiler, sSemantics, sSemanticNames, sizeof(sSemantics)/sizeof(sSemantics[0]));
//	Hlsl2Glsl_UseUserVaryings(compiler, true);

	// translate to GLSL
	if(success)
		success = Hlsl2Glsl_Translate(compiler, "main", version, 0);

	// were there errors?
	if(!success)
	{
		const char *pLog = Hlsl2Glsl_GetInfoLog(compiler);
		MFDebug_Warn(1, pLog);
		Hlsl2Glsl_DestructCompiler(compiler);
		return NULL;
	}

	// and see what we got...
	pShaderSource = MFString_Dup(Hlsl2Glsl_GetShader(compiler));

//	int numUniforms = Hlsl2Glsl_GetUniformCount(compiler);
//	const ShUniformInfo *pUniforms = Hlsl2Glsl_GetUniformInfo(compiler);

	// we're done
	Hlsl2Glsl_DestructCompiler(compiler);

	return pShaderSource;
}
#endif

#if defined(SUPPORT_GLSL_OPTIMIZER)
const char *MFIntShader_OptimiseShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename)
{
	static bool bIsOptimiserInitialised = false;
	static glslopt_ctx *pCtx = NULL;
	if(!bIsOptimiserInitialised)
	{
		pCtx = glslopt_initialize(bGLES ? kGlslTargetOpenGLES20 : kGlslTargetOpenGL);
		bIsOptimiserInitialised = true;
	}

	glslopt_shader_type shaderType;
	switch(type)
	{
		case MFST_VertexShader:	shaderType = kGlslOptShaderVertex;	break;
		case MFST_PixelShader:	shaderType = kGlslOptShaderFragment;	break;
		default:
			MFDebug_Assert(false, "glsl_optimizer can only optimise vertex and pixel shaders.");
			return pShaderSource;
	}

	unsigned int options = kGlslOptionSkipPreprocessor; // <- Is this valid?
	glslopt_shader *pShader = glslopt_optimize(pCtx, shaderType, pShaderSource, options);

	if(glslopt_get_status(pShader))
	{
		pShaderSource = MFString_Dup(glslopt_get_output(pShader));
	}
	else
	{
		const char *pLog = glslopt_get_log(pShader);
		MFDebug_Warn(1, pLog);
		glslopt_shader_delete(pShader);
		return pShaderSource;
	}

	int math, tex, flow;
	glslopt_shader_get_stats(pShader, &math, &tex, &flow);
	MFDebug_Warn(2, MFStr("GLSL shader%s optimised successfully. Math ops: %d  Tex ops: %d  Flow ops: %d", pFilename ? MFStr(" '%s'", pFilename) : "", math, tex, flow));

	glslopt_shader_delete(pShader);

//	glslopt_cleanup(pCtx);	// TODO: Should we clean this up one day?

	return pShaderSource;
}
#endif

bool MFIntShader_CompileShaderOpenGL(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings, MFShaderLanguage language, bool bGLES)
{
	bool bAllocated = false;

	if(!pShaderSource && pFilename)
	{
		pShaderSource = MFFileSystem_Load(pFilename, &pTemplate->bytes, 1);
		bAllocated = true;
	}

	// run the preprocessor
	char *pPreProcessed = MFIntShader_Preprocess(pTemplate, pMacros, bDebug, pFilename, pShaderSource, line);
	if(pPreProcessed != pShaderSource)
	{
		if(bAllocated)
			MFHeap_Free((char*)pShaderSource);

		pShaderSource = pPreProcessed;
		bAllocated = true;
	}

	// translate HLSL to GLSL if available
#if defined(SUPPORT_HLSL) && defined(SUPPORT_HLSL2GLSL)
	if(language == MFSL_HLSL)
	{
		const char *pTranslatedShader = MFIntShader_TranslateShader(pShaderSource, pTemplate->shaderType, bGLES, pFilename);
		if(pTranslatedShader != pShaderSource)
		{
			if(bAllocated)
				MFHeap_Free((char*)pShaderSource);
			if(!pTranslatedShader)
				return false;
			pShaderSource = pTranslatedShader;
			bAllocated = true;
		}
	}
#else
	MFDebug_Assert(language != MFSL_HLSL, "hlsl2glsl library unavailable!");
#endif

	// optimise the shader if available
#if defined(SUPPORT_GLSL_OPTIMIZER)
	const char *pOptimisedShader = MFIntShader_OptimiseShader(pShaderSource, pTemplate->shaderType, bGLES, pFilename);
	if(pOptimisedShader != pShaderSource)
	{
		if(bAllocated)
			MFHeap_Free((char*)pShaderSource);
		pShaderSource = pOptimisedShader;
		bAllocated = true;
	}
#endif

	// stash the glsl source as the shader program data
	if(pShaderSource)
	{
		pTemplate->bytes = MFString_Length(pShaderSource) + 1;
		if(bAllocated)
			pTemplate->pProgram = pShaderSource;
		else
			pTemplate->pProgram = (char*)MFCopyMemory(MFHeap_Alloc(pTemplate->bytes), pShaderSource, pTemplate->bytes);
	}

	return true;
}
#endif

#if defined(SUPPORT_CG)
bool MFIntShader_CompileShaderCg(MFShaderTemplate *pTemplate, MFShaderMacro *pMacros, bool bDebug, const char *pFilename, const char *pShaderSource, int line, MFStringCache *pStrings, MFShaderLanguage language, MFRendererDrivers renderDriver)
{
	// TODO: compile Cg shader...
	// target D3D9/D3D11/OpenGL

	MFDebug_Assert(false, "TODO");

	return false;
}
#endif
