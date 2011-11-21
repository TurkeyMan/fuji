#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D11
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D11
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D11
	#define MFMat_Standard_CreateInstance MFMat_Standard_CreateInstance_D3D11
	#define MFMat_Standard_DestroyInstance MFMat_Standard_DestroyInstance_D3D11
#endif

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
	ID3D11SamplerState* pSamplerState;
	ID3D11RasterizerState* pRasterizerState;
	ID3D11BlendState *pBlendState;
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

ID3D11VertexShader *pVertexShader = NULL;
ID3D11PixelShader *pPixelShader = NULL;



int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	MFCALLSTACK;
	
	g_pd3dDevice->CreateVertexShader(g_pVertexShaderData, g_vertexShaderSize, NULL, &pVertexShader);
	g_pd3dDevice->CreatePixelShader(g_pPixelShaderData, g_pixelShaderSize, NULL, &pPixelShader);

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;
	
	if (pPixelShader) pPixelShader->Release();
	if (pVertexShader) pVertexShader->Release();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;
	
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

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data_D3D11));
	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data_D3D11));
	
	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend | 1<<6 /* back face culling */;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;

	pData->alphaRef = 1.0f;
	
	//--
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc;
	MFZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = g_pd3dDevice->CreateSamplerState(&samplerDesc, &pData->pSamplerState);


	MFRenderer_D3D11_SetDebugName(pData->pSamplerState, pMaterial->pName);

	//--
	
	D3D11_RASTERIZER_DESC rasterizerDesc;
	MFZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = g_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &pData->pRasterizerState);

	MFRenderer_D3D11_SetDebugName(pData->pRasterizerState, pMaterial->pName);

	//--

	MFTexture *pDiffuse = pData->textures[pData->diffuseMapIndex].pTexture;

	const bool premultipliedAlpha = pDiffuse && ((pDiffuse->pTemplateData->flags & TEX_PreMultipliedAlpha) != 0);

	D3D11_BLEND_DESC blendDesc;
	MFZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


	switch (pData->materialType & MF_BlendMask)
	{
		case 0:
			blendDesc.RenderTarget[0].BlendEnable = false;
			break;

		case MF_AlphaBlend:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;

		case MF_Additive:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;

		case MF_Subtractive:
			blendDesc.RenderTarget[0].BlendEnable = true;
			blendDesc.RenderTarget[0].SrcBlend = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].SrcBlendAlpha = premultipliedAlpha ? D3D11_BLEND_ONE : D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
			break;
	}

	hr = g_pd3dDevice->CreateBlendState(&blendDesc, &pData->pBlendState);
	
	MFRenderer_D3D11_SetDebugName(pData->pBlendState, pMaterial->pName);

	//--

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

	hr = g_pd3dDevice->CreateBuffer(&desc, NULL, &pData->pConstantBuffer);

	MFRenderer_D3D11_SetDebugName(pData->pConstantBuffer, pMaterial->pName);
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data_D3D11 *pData = (MFMat_Standard_Data_D3D11*)pMaterial->pInstanceData;

	for(uint32 a = 0; a < pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->textures[a].pTexture);
	}

	if(pData->pSamplerState) pData->pSamplerState->Release();
	if(pData->pRasterizerState) pData->pRasterizerState->Release();
	if(pData->pBlendState) pData->pBlendState->Release();
	if(pData->pConstantBuffer) pData->pConstantBuffer->Release();

	MFHeap_Free(pMaterial->pInstanceData);
}

#endif // MF_RENDERER
