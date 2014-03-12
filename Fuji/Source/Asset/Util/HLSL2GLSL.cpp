#include "Fuji.h"
#include "MFModule.h"
#include "Asset/Util/HLSL2GLSL.h"

#if !defined(MF_MIDDLEWARE)

#if defined(MF_COMPILER_VISUALC)
	#pragma comment(lib, "FujiMiddleware")
#endif

#else

#include <stdio.h>
char gErrorBuffer[1024];

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

static_assert(sizeof(sSemantics)/sizeof(sSemantics[0]) == sizeof(sSemanticNames)/sizeof(sSemanticNames[0]), "Mismatching attribute declaration arrays!");

MF_API const char *HLSL2GLSL_TranslateShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **pError)
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
			*pError = "hlsl2glsl can only convert vertex and pixel shaders.";
			return NULL;
	}

	ETargetVersion version = bGLES ? ETargetGLSL_ES_100 : ETargetGLSL_120;

	// create a compiler
	ShHandle compiler = Hlsl2Glsl_ConstructCompiler(language);

	// parse the code
	int success = Hlsl2Glsl_Parse(compiler, pShaderSource, version, 0);

	// set translation options
	Hlsl2Glsl_SetUserAttributeNames(compiler, sSemantics, sSemanticNames, sizeof(sSemantics)/sizeof(sSemantics[0]));
//	Hlsl2Glsl_UseUserVaryings(compiler, true);

	// translate to GLSL
	if(success)
		success = Hlsl2Glsl_Translate(compiler, "main", version, 0);

	// were there errors?
	if(!success)
	{
		strcpy(gErrorBuffer, Hlsl2Glsl_GetInfoLog(compiler));
		*pError = gErrorBuffer;
		Hlsl2Glsl_DestructCompiler(compiler);
		return NULL;
	}

	// and see what we got...
	pShaderSource = strdup(Hlsl2Glsl_GetShader(compiler));

//	int numUniforms = Hlsl2Glsl_GetUniformCount(compiler);
//	const ShUniformInfo *pUniforms = Hlsl2Glsl_GetUniformInfo(compiler);

	// we're done
	Hlsl2Glsl_DestructCompiler(compiler);

	return pShaderSource;
}
#else
MF_API const char *HLSL2GLSL_TranslateShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **pError)
{
	*pError = "hlsl2glsl not available!";
	return NULL;
}
#endif

#if defined(SUPPORT_GLSL_OPTIMIZER)
MF_API const char *HLSL2GLSL_OptimiseShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **ppOutput)
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
			*ppOutput = "glsl_optimizer can only optimise vertex and pixel shaders.";
			return NULL;
	}

	unsigned int options = kGlslOptionSkipPreprocessor; // <- Is this valid?
	glslopt_shader *pShader = glslopt_optimize(pCtx, shaderType, pShaderSource, options);

	if(glslopt_get_status(pShader))
	{
		// TODO: this allocates with malloc!
		pShaderSource = strdup(glslopt_get_output(pShader));

		int math, tex, flow;
		glslopt_shader_get_stats(pShader, &math, &tex, &flow);

		if(pFilename)
			sprintf(gErrorBuffer, "GLSL shader '%s' optimised successfully. Math ops: %d  Tex ops: %d  Flow ops: %d", pFilename, math, tex, flow);
		else
			sprintf(gErrorBuffer, "GLSL shader optimised successfully. Math ops: %d  Tex ops: %d  Flow ops: %d", math, tex, flow);
		*ppOutput = gErrorBuffer;
	}
	else
	{
		pShaderSource = NULL;

		strcpy(gErrorBuffer, glslopt_get_log(pShader));
		*ppOutput = gErrorBuffer;
	}

	glslopt_shader_delete(pShader);

//	glslopt_cleanup(pCtx);	// TODO: Should we clean this up one day?

	return pShaderSource;
}
#else
MF_API const char *HLSL2GLSL_OptimiseShader(const char *pShaderSource, MFShaderType type, bool bGLES, const char *pFilename, const char **ppOutput)
{
	*pError = "glsl_optimizer not available!";
	return NULL;
}
#endif

MF_API void HLSL2GLSL_FreeShader(const char *pShader)
{
	free((char*)pShader);
}

#endif
