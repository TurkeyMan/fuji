#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D11
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D11
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D11
	#define MFMat_Standard_CreateInstancePlatformSpecific MFMat_Standard_CreateInstancePlatformSpecific_D3D11
	#define MFMat_Standard_DestroyInstancePlatformSpecific MFMat_Standard_DestroyInstancePlatformSpecific_D3D11
#endif

#include "MFRenderState_Internal.h"
#include "MFMaterial.h"
#include "MFMaterial_Internal.h"
#include "Materials/MFMat_Standard_Internal.h"
#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFView.h"

#include "../Shaders/Registers.h"

#include "../MFRenderer_D3D11.h"

#include "../Shaders/MatStandard_s.h"
#include "../Shaders/MatStandard_a.h"

#include "../Shaders/MatStandard_ps.h"

struct CBMaterial
{
	MFVector mTexMatrix[2];
    MFVector vMeshColor;
	MFVector gModelColour;
	MFVector gColourMask;
};

struct MFMat_Standard_Data_D3D11 : public MFMat_Standard_Data
{
	ID3D11Buffer *pConstantBuffer;

	CBMaterial cbMaterial;
};




static MFMaterial *pSetMaterial = 0;
extern uint32 renderSource;
extern uint32 currentRenderFlags;


extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

const uint8 *g_pVertexShaderData = g_vs_main_s;
unsigned int g_vertexShaderSize  = sizeof(g_vs_main_s);

const uint8 *g_pPixelShaderData = g_ps_main;
unsigned int g_pixelShaderSize  = sizeof(g_ps_main);

static ID3D11VertexShader *gpVertexShader = NULL;
static ID3D11PixelShader *gpPixelShader = NULL;



int MFMat_Standard_RegisterMaterial(MFMaterialType *pType)
{
	pType->instanceDataSize = sizeof(MFMat_Standard_Data_D3D11);

	g_pd3dDevice->CreateVertexShader(g_pVertexShaderData, g_vertexShaderSize, NULL, &gpVertexShader);
	g_pd3dDevice->CreatePixelShader(g_pPixelShaderData, g_pixelShaderSize, NULL, &gpPixelShader);

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	if(gpPixelShader)
		gpPixelShader->Release();
	if(gpVertexShader)
		gpVertexShader->Release();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial, MFRendererState &state)
{
//	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;

	if(state.pMatrixStatesSet[MFSCM_WorldViewProjection] != state.pMatrixStates[MFSCM_WorldViewProjection])
	{
		MFMatrix *pWVP = state.getDerivedMatrix(MFSCM_WorldViewProjection);
		state.pMatrixStates[MFSCM_WorldViewProjection] = pWVP;

		// ???
	}

	if(state.pMatrixStatesSet[MFSCM_UV0] != state.pMatrixStates[MFSCM_UV0])
	{
		MFMatrix *pUV0 = state.pMatrixStates[MFSCM_UV0];
		state.pMatrixStatesSet[MFSCM_UV0] = pUV0;

		// ???
	}

	if(state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] != state.pVectorStates[MFSCV_MaterialDiffuseColour])
	{
		MFVector *pDiffuseColour = state.pVectorStates[MFSCV_MaterialDiffuseColour];
		state.pVectorStatesSet[MFSCV_MaterialDiffuseColour] = pDiffuseColour;

		// ???
	}

	bool bDetailPresent = state.isSet(MFSB_CT_Bool, MFSCB_DetailMapSet);
	bool bDiffusePresent = state.isSet(MFSB_CT_Bool, MFSCB_DiffuseSet);

	if(bDetailPresent)
	{
		// set detail map
		MFTexture *pDetail = state.pTextures[MFSCT_DetailMap];
		if(state.pTexturesSet[MFSCT_DetailMap] != pDetail)
		{
			state.pTexturesSet[MFSCT_DetailMap] = pDetail;

			// ???
//			ID3D11ShaderResourceView *pSRV = (ID3D11ShaderResourceView*)pData->textures[pData->diffuseMapIndex].pTexture->pInternalData;
//			g_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		}

		// set detail map sampler
		MFSamplerState *pDetailSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DetailMapSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] != pDetailSamp)
		{
			state.pRenderStatesSet[MFSCRS_DetailMapSamplerState] = pDetailSamp;

			// ???
//			ID3D11ShaderResourceView *pSRV = (ID3D11ShaderResourceView*)pData->textures[pData->diffuseMapIndex].pTexture->pInternalData;
//			g_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_DetailMap] != NULL)
		{
			state.pTexturesSet[MFSCT_DetailMap] = NULL;

			// ???
//			pd3dDevice->SetTexture(1, NULL);
		}
	}

	if(bDiffusePresent)
	{
		// set diffuse map
		MFTexture *pDiffuse = state.pTextures[MFSCT_Diffuse];
		if(state.pTexturesSet[MFSCT_Diffuse] != pDiffuse)
		{
			state.pTexturesSet[MFSCT_Diffuse] = pDiffuse;

			// ???
//			ID3D11ShaderResourceView *pSRV = (ID3D11ShaderResourceView*)pData->textures[pData->diffuseMapIndex].pTexture->pInternalData;
//			g_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		}

		// set diffuse map sampler
		MFSamplerState *pDiffuseSamp = (MFSamplerState*)state.pRenderStates[MFSCRS_DiffuseSamplerState];
		if(state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] != pDiffuseSamp)
		{
			state.pRenderStatesSet[MFSCRS_DiffuseSamplerState] = pDiffuseSamp;

			// ???
//			MFMat_Standard_SetSamplerState(0, pDiffuseSamp);
		}
	}
	else
	{
		if(state.pTexturesSet[MFSCT_Diffuse] != NULL)
		{
			state.pTexturesSet[MFSCT_Diffuse] = NULL;

			// ???
//			pd3dDevice->SetTexture(0, NULL);
		}
	}

	// configure the texture combiner (can we cache this?)
	if(state.boolChanged(MFSCB_DetailMapSet) || state.boolChanged(MFSCB_DiffuseSet) || state.boolChanged(MFSCB_User0))
	{
		const uint32 mask = MFBIT(MFSCB_DetailMapSet) | MFBIT(MFSCB_DiffuseSet) | MFBIT(MFSCB_User0);
		state.boolsSet = (state.boolsSet & ~mask) | (state.bools & mask);

		g_pImmediateContext->PSSetShader(gpPixelShader, NULL, 0);
/*
		if(bDetailPresent)
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
			pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		}
		else
		{
			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			if(state.getBool(MFSCB_User0))
			{
				// we need to scale the colour intensity by the vertex alpha since it wont happen during the blend.
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

//				pd3dDevice->SetTextureStageState(1, D3DTSS_CONSTANT, 0);

				pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEMP);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			}
			else
			{
				pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
		}
*/
	}

	// blend state
	MFBlendState *pBlendState = (MFBlendState*)state.pRenderStates[MFSCRS_BlendState];
	if(state.pRenderStatesSet[MFSCRS_BlendState] != pBlendState)
	{
		state.pRenderStatesSet[MFSCRS_BlendState] = pBlendState;

		float blend[4] = { 0, 0, 0, 0 };
		ID3D11BlendState *pBS = (ID3D11BlendState*)pBlendState->pPlatformData;
		g_pImmediateContext->OMSetBlendState(pBS, blend, 0xFFFFFFFF);
	}

	// rasteriser state
	MFRasteriserState *pRasteriserState = (MFRasteriserState*)state.pRenderStates[MFSCRS_RasteriserState];
	if(state.pRenderStatesSet[MFSCRS_RasteriserState] != pRasteriserState)
	{
		state.pRenderStatesSet[MFSCRS_RasteriserState] = pRasteriserState;

		ID3D11RasterizerState *pRS = (ID3D11RasterizerState*)pRasteriserState->pPlatformData;
		g_pImmediateContext->RSSetState(pRS);
	}

	// depth/stencil state
	MFDepthStencilState *pDSState = (MFDepthStencilState*)state.pRenderStates[MFSCRS_DepthStencilState];
	if(state.pRenderStatesSet[MFSCRS_DepthStencilState] != pDSState)
	{
		state.pRenderStatesSet[MFSCRS_DepthStencilState] = pDSState;

		ID3D11DepthStencilState *pDSS = (ID3D11DepthStencilState*)pDSState->pPlatformData;
		g_pImmediateContext->OMSetDepthStencilState(pDSS, 0);
	}

	// set animation matrices
	if(state.getBool(MFSCB_Animated))
	{
		MFDebug_Assert(false, "Animation not yet supported! :(");

//		for(int b=0; b<gNumBonesInBatch; b++)
//			MFRendererPC_SetAnimationMatrix(b, pAnimMats[pCurrentBatch[b]]);

//		pd3dDevice->SetVertexShader(pVS_a);
	}
	else
		g_pImmediateContext->VSSetShader(gpVertexShader, NULL, 0);




/*
	g_pImmediateContext->VSSetShader(pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(pPixelShader, NULL, 0);

	if(pSetMaterial != pMaterial)
	{
		bool premultipliedAlpha = false;

		static const float blend[4] = {0.0f, 0.0f, 0.0f, 0.0f};

		g_pImmediateContext->RSSetState(pData->pRasterizerState);
		g_pImmediateContext->OMSetBlendState(pData->pBlendState, blend, 0xFFFFFFFF);
		g_pImmediateContext->VSSetSamplers(0, 1, &pData->pSamplerState);
		g_pImmediateContext->PSSetSamplers(0, 1, &pData->pSamplerState);

		g_pImmediateContext->UpdateSubresource(pData->pConstantBuffer, 0, NULL, &pData->cbMaterial, 0, 0);

		g_pImmediateContext->VSSetConstantBuffers(n_cbMaterial, 1, &pData->pConstantBuffer);
		g_pImmediateContext->PSSetConstantBuffers(n_cbMaterial, 1, &pData->pConstantBuffer);

		//// set some render states
		//if(pData->detailMapIndex)
		//{
		//	// HACK: for compound multitexturing
		//	IDirect3DTexture9 *pDiffuse = (IDirect3DTexture9*)pData->pTextures[pData->diffuseMapIndex]->pInternalData;
		//	IDirect3DTexture9 *pDetail = (IDirect3DTexture9*)pData->pTextures[pData->detailMapIndex]->pInternalData;

		//	MFRendererPC_SetTexture(0, pDetail);
		//	MFRendererPC_SetTexture(1, pDiffuse);

		//	MFRendererPC_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		//	MFRendererPC_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//	MFRendererPC_SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		//	MFRendererPC_SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		//	MFRendererPC_SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//	MFRendererPC_SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		//	MFRendererPC_SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//	MFRendererPC_SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		//	MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		//	MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		//	MFRendererPC_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		//	MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		//	MFRendererPC_SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//	MFRendererPC_SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		//	MFRendererPC_SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
		//	MFRendererPC_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
		//	MFRendererPC_SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		//	MFRendererPC_SetTextureMatrix(pData->textureMatrix);
		//}
		//else
		if(pData->textures[pData->diffuseMapIndex].pTexture)
		{
			ID3D11ShaderResourceView *pSRV = (ID3D11ShaderResourceView*)pData->textures[pData->diffuseMapIndex].pTexture->pInternalData;

			g_pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

			//MFRendererPC_SetTexture(0, pTexture);
			//MFRendererPC_SetTexture(1, NULL);

			//MFRendererPC_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			//MFRendererPC_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			//MFRendererPC_SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			//MFRendererPC_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			//MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			//MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			//MFRendererPC_SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			//MFRendererPC_SetTextureMatrix(pData->textureMatrix);

			//premultipliedAlpha = !!(pData->pTextures[pData->diffuseMapIndex]->pTemplateData->flags & TEX_PreMultipliedAlpha);

			if(premultipliedAlpha)
			{
//				// we need to scale the colour intensity by the vertex alpha since it wont happen during the blend.
//				MFRendererPC_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
//				MFRendererPC_SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
//
////				MFRendererPC_SetTextureStageState(1, D3DTSS_CONSTANT, 0);
//
//				MFRendererPC_SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
//				MFRendererPC_SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEMP);
//				MFRendererPC_SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			}
			else
			{
				//MFRendererPC_SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				//MFRendererPC_SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}
		}
		else
		{
			//MFRendererPC_SetTexture(0, NULL);
		}

		//switch (pData->materialType&MF_BlendMask)
		//{
		//	case 0:
		//		MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//		break;
		//	case MF_AlphaBlend:
		//		MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//		MFRendererPC_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		//		MFRendererPC_SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
		//		MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//		break;
		//	case MF_Additive:
		//		MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//		MFRendererPC_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		//		MFRendererPC_SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
		//		MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//		break;
		//	case MF_Subtractive:
		//		MFRendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//		MFRendererPC_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		//		MFRendererPC_SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
		//		MFRendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		//		break;
		//}

		//if (pData->materialType & MF_Mask)
		//{
		//	MFRendererPC_SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		//	MFRendererPC_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		//	MFRendererPC_SetRenderState(D3DRS_ALPHAREF, 0xFF);
		//}
		//else
		//{
		//	MFRendererPC_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		//}

		//switch (pData->materialType&MF_CullMode)
		//{
		//	case 0<<6:
		//		MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		//		break;
		//	case 1<<6:
		//		MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		//		break;
		//	case 2<<6:
		//		MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		//		break;
		//	case 3<<6:
		//		// 'default' ?
		//		MFRendererPC_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		//		break;
		//}

		//if(!(pData->materialType&MF_NoZRead) || !(pData->materialType&MF_NoZWrite))
		//{
		//	MFRendererPC_SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		//	MFRendererPC_SetRenderState(D3DRS_ZWRITEENABLE, (pData->materialType&MF_NoZWrite) ? FALSE : TRUE);
		//	MFRendererPC_SetRenderState(D3DRS_ZFUNC, (pData->materialType&MF_NoZRead) ? D3DCMP_ALWAYS : D3DCMP_LESSEQUAL);
		//}
		//else
		//{
		//	MFRendererPC_SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		//}
	}

	//MFRendererPC_SetColourMask(1, 0, 1, 0);

	//// set animation matrices...
	//if (pAnimMats && pCurrentBatch)
	//{
	//	for(int b=0; b<gNumBonesInBatch; b++)
	//		MFRendererPC_SetAnimationMatrix(b, pAnimMats[pCurrentBatch[b]]);
	//}

	//if(MFRendererPC_GetNumWeights() > 0)
	//	MFRendererPC_SetVertexShader(pVS_a);
	//else
	//	MFRendererPC_SetVertexShader(pVS_s);
*/
	return 0;
}

void MFMat_Standard_CreateInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;

	MFMatrix mat = pData->textureMatrix;
	mat.Transpose();

	pData->cbMaterial.mTexMatrix[0] = mat.GetXAxis();
	pData->cbMaterial.mTexMatrix[1] = mat.GetYAxis();
	pData->cbMaterial.vMeshColor = MFVector::white;
	pData->cbMaterial.gModelColour = MFVector::white;
	pData->cbMaterial.gColourMask = MakeVector(1.0f, 0.0f, 1.0f, 0.0f);

	D3D11_BUFFER_DESC desc;
	MFZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(pData->cbMaterial);
	desc.Usage = D3D11_USAGE_DEFAULT;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pd3dDevice->CreateBuffer(&desc, NULL, &pData->pConstantBuffer);

	MFRenderer_D3D11_SetDebugName(pData->pConstantBuffer, pMaterial->pName);
}

void MFMat_Standard_DestroyInstancePlatformSpecific(MFMaterial *pMaterial)
{
	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;

	if(pData->pConstantBuffer) pData->pConstantBuffer->Release();
}

#endif // MF_RENDERER
