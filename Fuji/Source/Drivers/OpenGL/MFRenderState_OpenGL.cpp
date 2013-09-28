#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFRenderState_InitModulePlatformSpecific MFRenderState_InitModulePlatformSpecific_OpenGL
	#define MFRenderState_DeinitModulePlatformSpecific MFRenderState_DeinitModulePlatformSpecific_OpenGL
	#define MFBlendState_CreatePlatformSpecific MFBlendState_CreatePlatformSpecific_OpenGL
	#define MFBlendState_DestroyPlatformSpecific MFBlendState_DestroyPlatformSpecific_OpenGL
	#define MFSamplerState_CreatePlatformSpecific MFSamplerState_CreatePlatformSpecific_OpenGL
	#define MFSamplerState_DestroyPlatformSpecific MFSamplerState_DestroyPlatformSpecific_OpenGL
	#define MFDepthStencilState_CreatePlatformSpecific MFDepthStencilState_CreatePlatformSpecific_OpenGL
	#define MFDepthStencilState_DestroyPlatformSpecific MFDepthStencilState_DestroyPlatformSpecific_OpenGL
	#define MFRasteriserState_CreatePlatformSpecific MFRasteriserState_CreatePlatformSpecific_OpenGL
	#define MFRasteriserState_DestroyPlatformSpecific MFRasteriserState_DestroyPlatformSpecific_OpenGL
#endif

#include "MFRenderState_Internal.h"
#include "MFOpenGL.h"

static const GLint glTexFilters[] =
{
	// mip none
	GL_NEAREST,					// MFMatStandard_TexFilter_None
	GL_NEAREST,					// MFMatStandard_TexFilter_Point
	GL_LINEAR,					// MFMatStandard_TexFilter_Linear
	GL_LINEAR,					// MFMatStandard_TexFilter_Anisotropic

	// mip nearest
	GL_NEAREST_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_NEAREST,	// MFMatStandard_TexFilter_Anisotropic

	// mip linear
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Anisotropic

	// mip anisotropic (can't declare anisotropic mip filter)
	0,	// MFMatStandard_TexFilter_None
	0,	// MFMatStandard_TexFilter_Point
	0,	// MFMatStandard_TexFilter_Linear
	0,	// MFMatStandard_TexFilter_Anisotropic
};

static const GLint glTexAddressing[MFTexAddressMode_Max] =
{
	GL_REPEAT,					// MFMatStandard_TexAddress_Wrap
	GL_MIRRORED_REPEAT,			// MFMatStandard_TexAddress_Mirror
	GL_CLAMP_TO_EDGE,			// MFMatStandard_TexAddress_Clamp
	GL_CLAMP_TO_BORDER,			// MFMatStandard_TexAddress_Border
	GL_MIRROR_CLAMP_TO_EDGE_EXT // MFMatStandard_TexAddress_MirrorOnce
};

static const GLenum glBlendOp[MFBlendOp_BlendOpCount] =
{
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX
};

static const GLenum glBlendArg[MFBlendArg_Max] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE,
	GL_CONSTANT_COLOR,
	GL_ONE_MINUS_CONSTANT_COLOR,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_COLOR,
	GL_SRC1_ALPHA,
	GL_ONE_MINUS_SRC1_ALPHA,
};

static const GLenum glCompareFunc[MFComparisonFunc_Max] =
{
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};


void MFRenderState_InitModulePlatformSpecific()
{
}

void MFRenderState_DeinitModulePlatformSpecific()
{
}

bool MFBlendState_CreatePlatformSpecific(MFBlendState *pBS)
{
	return true;
}

void MFBlendState_DestroyPlatformSpecific(MFBlendState *pBS)
{
}

bool MFSamplerState_CreatePlatformSpecific(MFSamplerState *pSS)
{
	GLuint sampler;
	glGenSamplers(1, &sampler);
	pSS->pPlatformData = (void*)(size_t)sampler;

	// configure the sampler
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, glTexFilters[pSS->stateDesc.minFilter | (pSS->stateDesc.mipFilter << 2)]);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, glTexFilters[pSS->stateDesc.magFilter]);

	bool bUseAnisotropic = pSS->stateDesc.minFilter == MFTexFilter_Anisotropic || pSS->stateDesc.magFilter == MFTexFilter_Anisotropic;
	glSamplerParameteri(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, bUseAnisotropic ? pSS->stateDesc.maxAnisotropy : 1);

	glSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, pSS->stateDesc.minLOD);
	glSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, pSS->stateDesc.maxLOD);
	glSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS, pSS->stateDesc.mipLODBias);

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, glTexAddressing[pSS->stateDesc.addressU]);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, glTexAddressing[pSS->stateDesc.addressV]);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, glTexAddressing[pSS->stateDesc.addressW]);

	glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, (float*)&pSS->stateDesc.borderColour);

	glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, pSS->stateDesc.bEnableComparison ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
	glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, glCompareFunc[pSS->stateDesc.comparisonFunc]);

	MFCheckForOpenGLError(true);
	return true;
}

void MFSamplerState_DestroyPlatformSpecific(MFSamplerState *pSS)
{
	GLuint sampler = (GLuint)(size_t)pSS->pPlatformData;
	glDeleteSamplers(1, &sampler);
}

bool MFDepthStencilState_CreatePlatformSpecific(MFDepthStencilState *pDSS)
{
	return true;
}

void MFDepthStencilState_DestroyPlatformSpecific(MFDepthStencilState *pDSS)
{
}

bool MFRasteriserState_CreatePlatformSpecific(MFRasteriserState *pRS)
{
	return true;
}

void MFRasteriserState_DestroyPlatformSpecific(MFRasteriserState *pRS)
{
}

#endif // MF_RENDERER
