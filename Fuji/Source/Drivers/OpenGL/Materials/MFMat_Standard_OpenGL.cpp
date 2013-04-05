#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_OpenGL
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_OpenGL
	#define MFMat_Standard_Begin MFMat_Standard_Begin_OpenGL
	#define MFMat_Standard_CreateInstancePlatformSpecific MFMat_Standard_CreateInstancePlatformSpecific_OpenGL
	#define MFMat_Standard_DestroyInstancePlatformSpecific MFMat_Standard_DestroyInstancePlatformSpecific_OpenGL
#endif

#include "MFRenderState_Internal.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Standard_Internal.h"

#include "../MFOpenGL.h"

static MFMaterial *pSetMaterial = 0;

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

	// mip anisotropic
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_None
	GL_NEAREST_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Point
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Linear
	GL_LINEAR_MIPMAP_LINEAR,	// MFMatStandard_TexFilter_Anisotropic
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

#if defined(MF_OPENGL_SUPPORT_SHADERS)
	static GLuint gDefVertexShader = 0;
	static GLuint gDefFragmentShaderUntextured = 0;
	static GLuint gDefFragmentShaderTextured = 0;
	static GLuint gDefFragmentShaderMultiTextured = 0;

	static GLuint gDefShaderProgramUntextured = 0;
	static GLuint gDefShaderProgramTextured = 0;
	static GLuint gDefShaderProgramMultiTextured = 0;

	static const GLchar gVertexShader[] = "					\n\
		uniform mat4 wvMatrix;								\n\
		uniform mat4 wvpMatrix;								\n\
		uniform mat4 texMatrix;								\n\
															\n\
		attribute vec3 vPos;								\n\
		attribute vec3 vNormal;								\n\
		attribute vec4 vColour;								\n\
		attribute vec2 vUV0;								\n\
		attribute vec2 vUV1;								\n\
															\n\
		varying vec4 oColour;								\n\
		varying vec2 oUV0;									\n\
		varying vec2 oUV1;									\n\
															\n\
		void main()											\n\
		{													\n\
			oColour = vColour;								\n\
			oUV0 = vUV0;									\n\
			oUV1 = vUV1;									\n\
			gl_Position = wvpMatrix * vec4(vPos, 1.0);		\n\
		}													";

	static const GLchar gFragmentShaderUntextured[] = "			\n\
		varying vec4 oColour;									\n\
		void main(void)											\n\
		{														\n\
			gl_FragColor = oColour;								\n\
		}														";

	static const GLchar gFragmentShaderTextured[] = "			\n\
		uniform sampler2D diffuse;								\n\
		varying vec4 oColour;									\n\
		varying vec2 oUV0;										\n\
		void main(void)											\n\
		{														\n\
			gl_FragColor = texture2D(diffuse, oUV0) * oColour;	\n\
		}														";

	static const GLchar gFragmentShaderMultiTextured[] = "		\n\
		uniform sampler2D diffuse;								\n\
		uniform sampler2D detail;								\n\
		varying vec4 oColour;									\n\
		varying vec2 oUV0;										\n\
		varying vec2 oUV1;										\n\
		void main(void)											\n\
		{														\n\
			vec4 image = texture2D(diffuse, oUV0);				\n\
			vec4 colour = texture2D(detail, oUV0) * oColour;	\n\
			gl_FragColor = vec4(image.xyz, 0) + colour;			\n\
		}														";
#endif

int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
#if defined(MF_OPENGL_SUPPORT_SHADERS)
	if(MFOpenGL_UseShaders())
	{
		// probably compile the shaders now i guess...
		gDefVertexShader = MFRenderer_OpenGL_CompileShader(gVertexShader, MFOGL_ShaderType_VertexShader);
		gDefFragmentShaderUntextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderUntextured, MFOGL_ShaderType_FragmentShader);
		gDefFragmentShaderTextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderTextured, MFOGL_ShaderType_FragmentShader);
		gDefFragmentShaderMultiTextured = MFRenderer_OpenGL_CompileShader(gFragmentShaderMultiTextured, MFOGL_ShaderType_FragmentShader);

#if defined(MF_OPENGL_ES)
		glReleaseShaderCompiler();
#endif

		// create and link a program
		gDefShaderProgramUntextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderUntextured);
		gDefShaderProgramTextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderTextured);
		gDefShaderProgramMultiTextured = MFRenderer_OpenGL_CreateProgram(gDefVertexShader, gDefFragmentShaderMultiTextured);
	}
#endif

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
#if defined(MF_OPENGL_SUPPORT_SHADERS)
	if(MFOpenGL_UseShaders())
	{
		glDeleteProgram(gDefShaderProgramUntextured);
		glDeleteProgram(gDefShaderProgramTextured);
		glDeleteProgram(gDefShaderProgramMultiTextured);

		glDeleteShader(gDefVertexShader);
		glDeleteShader(gDefFragmentShaderUntextured);
		glDeleteShader(gDefFragmentShaderTextured);
		glDeleteShader(gDefFragmentShaderMultiTextured);
	}
#endif
}

inline void MFMat_Standard_SetSamplerState(MFSamplerState *pSampler, int sampler, const char *pName)
{
	glActiveTexture(GL_TEXTURE0 + sampler);

//	int minFilter = tex.pTexture->pTemplateData->mipLevels > 1 ? (tex.minFilter | (tex.mipFilter << 2)) : tex.minFilter;
	int minFilter = (pSampler->stateDesc.mipFilter << 2) | pSampler->stateDesc.minFilter;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexFilters[minFilter]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glTexFilters[pSampler->stateDesc.magFilter]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glTexAddressing[pSampler->stateDesc.addressU]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glTexAddressing[pSampler->stateDesc.addressV]);

#if defined(MF_OPENGL_SUPPORT_SHADERS)
	if(MFOpenGL_UseShaders())
		MFRenderer_OpenGL_SetUniformS(pName, sampler);
#endif
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFCALLSTACK;

#if MFMatStandard_TexFilter_Max > 4
	#error "glTexFilters only supports 4 mip filters..."
#endif

	bool bDetailPresent = state.isSet(MFSB_CT_Bool, MFSCB_DetailMapSet);
	bool bDiffusePresent = state.isSet(MFSB_CT_Bool, MFSCB_DiffuseSet);

	if(bDetailPresent)
	{
		// set detail map
		MFTexture *pDetail = state.pTextures[MFSCT_DetailMap];
		if(state.pTexturesSet[MFSCT_DetailMap] != pDetail)
		{
			state.pTexturesSet[MFSCT_DetailMap] = pDetail;

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)pDetail->pInternalData);
		}

		// set detail map sampler
		MFSamplerState *pDetailSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DetailMapSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] != pDetailSamp)
		{
			state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] = pDetailSamp;

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(MFOpenGL_UseShaders())
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramMultiTextured);
#endif
			MFMat_Standard_SetSamplerState(pDetailSamp, 0, "detail");
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_DetailMap] != NULL)
		{
			state.pTexturesSet[MFSCT_DetailMap] = NULL;

			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
		}
	}

	if(bDiffusePresent)
	{
		int diffuseSlot = bDetailPresent ? 1 : 0;

		// set diffuse map
		MFTexture *pDiffuse = state.pTextures[MFSCT_Diffuse];
		if(state.pTexturesSet[MFSCT_Diffuse] != pDiffuse || state.boolChanged(MFSCB_DetailMapSet))
		{
			state.pTexturesSet[MFSCT_Diffuse] = pDiffuse;

			glActiveTexture(GL_TEXTURE0 + diffuseSlot);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)pDiffuse->pInternalData);
		}

		// set diffuse map sampler
		MFSamplerState *pDiffuseSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DiffuseSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] != pDiffuseSamp || state.boolChanged(MFSCB_DetailMapSet))
		{
			state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] = pDiffuseSamp;

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(!bDetailPresent && MFOpenGL_UseShaders())
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramTextured);
#endif
			MFMat_Standard_SetSamplerState(pDiffuseSamp, diffuseSlot, "diffuse");
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_Diffuse] != NULL)
		{
			state.pTexturesSet[MFSCT_Diffuse] = NULL;

			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);

#if defined(MF_OPENGL_SUPPORT_SHADERS)
			if(MFOpenGL_UseShaders())
				MFRenderer_OpenGL_SetShaderProgram(gDefShaderProgramUntextured);
#endif
		}
	}

#if !defined(MF_OPENGL_ES) || MF_OPENGL_ES_VER < 2
	// configure the texture combiner (can we cache this?)
	if(state.boolChanged(MFSCB_DetailMapSet) || state.boolChanged(MFSCB_DiffuseSet) || state.boolChanged(MFSCB_User0))
	{
		const uint32 mask = MFBIT(MFSCB_DetailMapSet) | MFBIT(MFSCB_DiffuseSet) | MFBIT(MFSCB_User0);
		state.boolsSet = (state.boolsSet & ~mask) | (state.bools & mask);

		if(bDetailPresent)
		{
			glActiveTexture(GL_TEXTURE0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glActiveTexture(GL_TEXTURE1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			if(state.getBool(MFSCB_User0))
			{
				// premultiplied alpha?
			}
			else
			{
				// and not...
			}
		}
	}
#endif

	if(state.pMatrixStatesSet[MFSCM_Projection] != state.pMatrixStates[MFSCM_Projection])
	{
		MFMatrix *pProj = state.pMatrixStates[MFSCM_Projection];
		state.pMatrixStatesSet[MFSCM_Projection] = pProj;

		MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_Projection, *pProj);
	}

	if(state.pMatrixStatesSet[MFSCM_WorldView] != state.pMatrixStates[MFSCM_WorldView])
	{
		MFMatrix *pWV = state.getDerivedMatrix(MFSCM_WorldView);
		state.pMatrixStates[MFSCM_WorldView] = pWV;

		MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_WorldView, *pWV);
	}

	if(state.pMatrixStatesSet[MFSCM_UV0] != state.pMatrixStates[MFSCM_UV0])
	{
		MFMatrix *pUV0 = state.pMatrixStates[MFSCM_UV0];
		state.pMatrixStatesSet[MFSCM_UV0] = pUV0;

		if(bDetailPresent)
		{
			glActiveTexture(GL_TEXTURE1);
			MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_Texture, *pUV0);
		}
		glActiveTexture(GL_TEXTURE0);
		MFRenderer_OpenGL_SetMatrix(MFOGL_MatrixType_Texture, *pUV0);
	}
/*
	if(state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] != state.pVectorStates[MFSCV_MaterialDiffuseColour])
	{
		MFVector *pDiffuseColour = state.pVectorStates[MFSCV_MaterialDiffuseColour];
		state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] = pDiffuseColour;

//		pd3dDevice->SetVertexShaderConstantF(r_modelColour, (float*)pDiffuseColour, 1);
	}
*/

	// blend state
	MFBlendState *pBlendState = (MFBlendState*)state.pRenderStates[MFSCRS_BlendState];
	if(state.pRenderStatesSet[MFSCRS_BlendState] != pBlendState)
	{
		state.pRenderStatesSet[MFSCRS_BlendState] = pBlendState;

		MFBlendStateDesc::RenderTargetBlendDesc &target = pBlendState->stateDesc.renderTarget[0];
		if(target.bEnable)
		{
			glEnable(GL_BLEND);

			glBlendEquation(glBlendOp[target.blendOp]);
			glBlendFuncSeparate(glBlendArg[target.srcBlend], glBlendArg[target.destBlend], glBlendArg[target.srcBlendAlpha], glBlendArg[target.destBlendAlpha]);
//			glBlendFunc(glBlendArg[target.srcBlend], glBlendArg[target.destBlend]);
		}
		else
			glDisable(GL_BLEND);
	}

	// rasteriser state
	MFRasteriserState *pRasteriserState = (MFRasteriserState*)state.pRenderStates[MFSCRS_RasteriserState];
	if(state.pRenderStatesSet[MFSCRS_RasteriserState] != pRasteriserState)
	{
		state.pRenderStatesSet[MFSCRS_RasteriserState] = pRasteriserState;

		switch(pRasteriserState->stateDesc.cullMode)
		{
			case MFCullMode_None:
				glDisable(GL_CULL_FACE);
				break;
			case MFCullMode_CCW:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CW);
				glCullFace(GL_BACK);
				break;
			case MFCullMode_CW:
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				glCullFace(GL_BACK);
				break;
		}
	}

	// depth/stencil state
	MFDepthStencilState *pDSState = (MFDepthStencilState*)state.pRenderStates[MFSCRS_DepthStencilState];
	if(state.pRenderStatesSet[MFSCRS_DepthStencilState] != pDSState)
	{
		state.pRenderStatesSet[MFSCRS_DepthStencilState] = pDSState;

		if(pDSState->stateDesc.bDepthEnable)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(glCompareFunc[pDSState->stateDesc.depthFunc]);
			glDepthMask(pDSState->stateDesc.depthWriteMask == MFDepthWriteMask_Zero ? GL_FALSE : GL_TRUE);
		}
		else
			glDisable(GL_DEPTH_TEST);
	}

	// setup alpha test
	if(state.boolChanged(MFSCB_AlphaTest) || (state.pVectorStatesSet[MFSCV_RenderState] != state.pVectorStates[MFSCV_RenderState] && state.getBool(MFSCB_AlphaTest)))
	{
		MFVector *pRS = state.pVectorStates[MFSCV_RenderState];
		state.pVectorStatesSet[MFSCV_RenderState] = pRS;
		state.boolsSet = (state.boolsSet & ~MFBIT(MFSCB_AlphaTest)) | (state.bools & MFBIT(MFSCB_AlphaTest));

		if(state.getBool(MFSCB_AlphaTest))
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, pRS->x);
		}
		else
			glDisable(GL_ALPHA_TEST);
	}
/*
	// set clour/alpha scales
	if(state.pVectorStatesSet[MFSCV_User0] != state.pVectorStates[MFSCV_User0])
	{
		MFVector *pMask = state.pVectorStates[MFSCV_User0];
		state.pVectorStatesSet[MFSCV_User0] = pMask;

//		pd3dDevice->SetVertexShaderConstantF(r_colourMask, (float*)pMask, 1);
	}
*/

	MFCheckForOpenGLError(true);

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

#endif
