#include "Fuji_Internal.h"

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
#include "MFEffect_Internal.h"
#include "Util.h"

#include "../MFOpenGL.h"


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


int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFEffectTechnique *pTechnique = NULL;
	if(pData->pEffect)
		pTechnique = MFEffect_GetTechnique(pData->pEffect, state);
	MFDebug_Assert(pTechnique, "No technique!");

	MFEffectData_OpenGL &techniqueData = *(MFEffectData_OpenGL*)pTechnique->pPlatformData;

	if(pTechnique != state.pTechniqueSet)
	{
		state.pTechniqueSet = pTechnique;
		glUseProgram(techniqueData.program);

		// need to clear all the cache states
		//... or ignore the state caching for now
	}

	// bools
/*	do a bitscan loop over the bool states
	uint32 boolState = state.bools & state.rsSet[MFSB_CT_Bool];

	uint32 req = pTechnique->renderStateRequirements[MFSB_CT_Bool];
	if(req)
	{
		uint32 vsReq = pVS->renderStateRequirements[MFSB_CT_Bool];
		uint32 psReq = pPS->renderStateRequirements[MFSB_CT_Bool];
		if((state.boolsSet & req) != (boolState & req))
		{
			BOOL bools[32];
			for(uint32 i=0, b=1; i<MFSCB_Max; ++i, b<<=1)
				bools[i] = (boolState & b) != 0;

			if((state.boolsSet & vsReq) != (boolState & vsReq))
				pd3dDevice->SetVertexShaderConstantB(0, bools,  32);
			if((state.boolsSet & psReq) != (boolState & psReq))
				pd3dDevice->SetPixelShaderConstantB(0, bools,  32);
		}
	}
*/

	// matrices
	uint32 req = pTechnique->renderStateRequirements[MFSB_CT_Matrix];
	uint32 i;
	while(MFUtil_BitScanReverse(req, &i))
	{
		uint32 b = MFBIT(i);
		req ^= b;

//		if(state.pMatrixStatesSet[i] != state.pMatrixStates[i])
		{
			MFMatrix *pM;
			if(i > MFSCM_DerivedStart)
				pM = state.getDerivedMatrix((MFStateConstant_Matrix)i);
			else
				pM = state.pMatrixStates[i];
//			state.pMatrixStatesSet[i] = pM;

			GLint uniform = techniqueData.uniformLocation[MFSB_CT_Matrix][i];
			glUniformMatrix4fv(uniform, 1, GL_TRUE, (float*)pM);
		}
	}

	// vectors
	req = pTechnique->renderStateRequirements[MFSB_CT_Vector];
	while(MFUtil_BitScanReverse(req, &i))
	{
		uint32 b = MFBIT(i);
		req ^= b;

//		if(state.pVectorStatesSet[i] != state.pVectorStates[i])
		{
			MFVector *pV = state.pVectorStates[i];
//			state.pVectorStatesSet[i] = pV;

			GLint uniform = techniqueData.uniformLocation[MFSB_CT_Vector][i];
			glUniform4fv(uniform, 1, (float*)pV);
		}
	}

	// textures
	req = pTechnique->renderStateRequirements[MFSB_CT_Texture];
	while(MFUtil_BitScanReverse(req, &i))
	{
		req ^= MFBIT(i);

		MFTexture *pT = state.pTextures[i];
		if(state.pTexturesSet[i] != pT)
		{
			state.pTexturesSet[i] = pT;
			glActiveTexture(GL_TEXTURE0 + i);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)pT->pInternalData);
		}
/*
		else
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glDisable(GL_TEXTURE_2D);
		}
*/

		MFSamplerState *pS = (MFSamplerState*)state.pRenderStates[MFSCRS_DiffuseSamplerState + i];
//		if(state.pRenderStatesSet[MFSCRS_DiffuseSamplerState + i] != pS)
		{
//			state.pRenderStatesSet[MFSCRS_DiffuseSamplerState + i] = pS;
			GLint uniform = techniqueData.uniformLocation[MFSB_CT_RenderState][MFSCRS_DiffuseSamplerState + i];
			GLint sampler = (GLint)(size_t)pS->pPlatformData;
			glUniform1i(uniform, i);
			glBindSampler(i, sampler);
		}
	}

	// blend state
	MFBlendState *pBlendState = (MFBlendState*)state.pRenderStates[MFSCRS_BlendState];
	if(state.pRenderStatesSet[MFSCRS_BlendState] != pBlendState)
	{
		state.pRenderStatesSet[MFSCRS_BlendState] = pBlendState;

		if(pBlendState->stateDesc.bIndependentBlendEnable)
		{
			for(int j=0; j<8; ++j)
			{
				MFBlendStateDesc::RenderTargetBlendDesc &target = pBlendState->stateDesc.renderTarget[j];
				if(target.bEnable)
				{
					glEnable(GL_BLEND);
					glBlendEquationSeparatei(j, glBlendOp[target.blendOp], glBlendOp[target.blendOpAlpha]);
					glBlendFuncSeparatei(j, glBlendArg[target.srcBlend], glBlendArg[target.destBlend], glBlendArg[target.srcBlendAlpha], glBlendArg[target.destBlendAlpha]);
				}
				else
					glDisable(GL_BLEND);
				glColorMaski(j, target.writeMask & MFColourWriteEnable_Red, target.writeMask & MFColourWriteEnable_Green, target.writeMask & MFColourWriteEnable_Blue, target.writeMask & MFColourWriteEnable_Alpha);
			}
		}
		else
		{
			MFBlendStateDesc::RenderTargetBlendDesc &target = pBlendState->stateDesc.renderTarget[0];
			if(target.bEnable)
			{
				glEnable(GL_BLEND);
				glBlendEquationSeparate(glBlendOp[target.blendOp], glBlendOp[target.blendOpAlpha]);
				glBlendFuncSeparate(glBlendArg[target.srcBlend], glBlendArg[target.destBlend], glBlendArg[target.srcBlendAlpha], glBlendArg[target.destBlendAlpha]);
			}
			else
				glDisable(GL_BLEND);
			glColorMask(target.writeMask & MFColourWriteEnable_Red, target.writeMask & MFColourWriteEnable_Green, target.writeMask & MFColourWriteEnable_Blue, target.writeMask & MFColourWriteEnable_Alpha);
		}
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
			default:
				MFUNREACHABLE;
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

#if !defined(MF_OPENGL_ES)
		if(state.getBool(MFSCB_AlphaTest))
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, pRS->x);
		}
		else
			glDisable(GL_ALPHA_TEST);
#else
		// TODO: do something here...
		//I guess we need to implement the alpha test in the shader...
#endif
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

	// set animation matrices
	if(state.getBool(MFSCB_Animated))
	{
		MFDebug_Assert(false, "TODO!");
//		for(uint32 b=0; b<state.matrixBatch.numMatrices; b++)
//			MFRendererPC_SetAnimationMatrix(b, state.animation.pMatrices[state.matrixBatch.pIndices[b]]);
	}

	// set viewport
	if(state.pViewportSet != state.pViewport)
	{
		if(!state.pViewport)
			MFRenderer_ResetViewport();
		else
			MFRenderer_SetViewport(state.pViewport);
		state.pViewportSet = state.pViewport;
	}

	MFCheckForOpenGLError(true);

	// update the bools 'set' state
	state.boolsSet = state.bools & state.rsSet[MFSB_CT_Bool];

	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
}

#endif
