#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D9 || defined(MF_RENDERPLUGIN_D3D9)

#if defined(MF_RENDERPLUGIN_D3D9)
	#define MFMat_Standard_RegisterMaterial MFMat_Standard_RegisterMaterial_D3D9
	#define MFMat_Standard_UnregisterMaterial MFMat_Standard_UnregisterMaterial_D3D9
	#define MFMat_Standard_Begin MFMat_Standard_Begin_D3D9
	#define MFMat_Standard_CreateInstance MFMat_Standard_CreateInstance_D3D9
	#define MFMat_Standard_DestroyInstance MFMat_Standard_DestroyInstance_D3D9
#endif

#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFView_Internal.h"
#include "../MFRenderer_D3D9.h"
#include "Materials/MFMat_Standard_Internal.h"

#include "../Shaders/MatStandard_s.h"
#include "../Shaders/MatStandard_a.h"

static MFMaterial *pSetMaterial = 0;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

extern IDirect3DDevice9 *pd3dDevice;
IDirect3DVertexShader9 *pVS_s = NULL;
IDirect3DVertexShader9 *pVS_a = NULL;

extern const MFMatrix *pAnimMats;
extern int gNumAnimMats;

extern const uint16 *pCurrentBatch;
extern int gNumBonesInBatch;

static const uint32 sFilterModes[] =
{
    D3DTEXF_NONE,
    D3DTEXF_POINT,
    D3DTEXF_LINEAR,
    D3DTEXF_ANISOTROPIC
};

static const uint32 sAddressModes[] =
{
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER,
    D3DTADDRESS_MIRRORONCE
};


int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	MFCALLSTACK;

	pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(g_vs_main_s), &pVS_s);
	pd3dDevice->CreateVertexShader(reinterpret_cast<const DWORD*>(g_vs_main_a), &pVS_a);

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	MFCALLSTACK;

	pVS_s->Release();
	pVS_a->Release();
}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
		bool premultipliedAlpha = false;

		// set some render states
		if(pData->detailMapIndex)
		{
			// HACK: for compound multitexturing
			MFMat_Standard_Data::Texture &diffuse = pData->textures[pData->diffuseMapIndex];
			MFMat_Standard_Data::Texture &detail = pData->textures[pData->detailMapIndex];

			IDirect3DTexture9 *pDiffuse = (IDirect3DTexture9*)diffuse.pTexture->pInternalData;
			IDirect3DTexture9 *pDetail = (IDirect3DTexture9*)detail.pTexture->pInternalData;

			pd3dDevice->SetTexture(0, pDetail);
			pd3dDevice->SetTexture(1, pDiffuse);

			// set the mip filter to nearest!!!
			pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, sFilterModes[detail.minFilter]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, sFilterModes[detail.magFilter]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, sFilterModes[detail.mipFilter]);
			pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, sFilterModes[diffuse.minFilter]);
			pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, sFilterModes[diffuse.magFilter]);
			pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, sFilterModes[diffuse.mipFilter]);

			pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, sAddressModes[detail.addressU]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, sAddressModes[detail.addressV]);
			pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, sAddressModes[diffuse.addressU]);
			pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, sAddressModes[diffuse.addressV]);

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

			MFRendererPC_SetTextureMatrix(pData->textureMatrix);
		}
		else if(pData->textures[pData->diffuseMapIndex].pTexture)
		{
			MFMat_Standard_Data::Texture &diffuse = pData->textures[pData->diffuseMapIndex];

			IDirect3DTexture9 *pTexture = (IDirect3DTexture9*)diffuse.pTexture->pInternalData;

			pd3dDevice->SetTexture(0, pTexture);
			pd3dDevice->SetTexture(1, NULL);

			pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, sFilterModes[diffuse.minFilter]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, sFilterModes[diffuse.magFilter]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, sFilterModes[diffuse.mipFilter]);

			pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, sAddressModes[diffuse.addressU]);
			pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, sAddressModes[diffuse.addressV]);

			pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			MFRendererPC_SetTextureMatrix(pData->textureMatrix);

			premultipliedAlpha = !!(pData->textures[pData->diffuseMapIndex].pTexture->pTemplateData->flags & TEX_PreMultipliedAlpha);

			if(premultipliedAlpha)
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
		else
		{
			pd3dDevice->SetTexture(0, NULL);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				break;
			case MF_AlphaBlend:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
				pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
				pd3dDevice->SetRenderState(D3DRS_SRCBLEND, premultipliedAlpha ? D3DBLEND_ONE : D3DBLEND_SRCALPHA);
				pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
		}

		if(pData->materialType & MF_Mask)
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
		}
		else
		{
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}

		switch((pData->materialType & MF_CullMode) >> 6)
		{
			case 0:
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				break;
			case 1:
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
			case 2:
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
				break;
			case 3:
				// 'default' ?
				pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
		}

		if(!(pData->materialType&MF_NoZRead) || !(pData->materialType&MF_NoZWrite))
		{
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, (pData->materialType&MF_NoZWrite) ? FALSE : TRUE);
			pd3dDevice->SetRenderState(D3DRS_ZFUNC, (pData->materialType&MF_NoZRead) ? D3DCMP_ALWAYS : D3DCMP_LESSEQUAL);
		}
		else
		{
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}
	}

	MFRendererPC_SetColourMask(1, 0, 1, 0);

	// set animation matrices...
	if(pAnimMats && pCurrentBatch)
	{
		for(int b=0; b<gNumBonesInBatch; b++)
			MFRendererPC_SetAnimationMatrix(b, pAnimMats[pCurrentBatch[b]]);
	}

	if(MFRendererPC_GetNumWeights() > 0)
		MFRendererPC_SetVertexShader(pVS_a);
	else
		MFRendererPC_SetVertexShader(pVS_s);

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	MFZeroMemory(pData, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend | 1<<6 /* back face culling */;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;

	pData->alphaRef = 1.0f;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->textures[a].pTexture);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}

#endif // MF_RENDERER
