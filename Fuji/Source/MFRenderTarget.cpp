#include "Fuji_Internal.h"
#include "MFRenderTarget_Internal.h"
#include "MFTexture_Internal.h"
#include "Util.h"

static void MFRenderTarget_Destroy(MFResource *pRes)
{
	MFRenderTarget *pRenderTarget = (MFRenderTarget*)pRes;

	MFRenderTarget_DestroyPlatformSpecific(pRenderTarget);

	for(int a=0; a<8; ++a)
	{
		if(pRenderTarget->pColourTargets[a])
			MFTexture_Release(pRenderTarget->pColourTargets[a]);
	}
	if(pRenderTarget->pDepthStencil)
		MFTexture_Release(pRenderTarget->pDepthStencil);

	MFHeap_Free(pRenderTarget);
}

MFInitStatus MFRenderTarget_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFRT_RenderTarget = MFResource_Register("MFRenderTarget", &MFRenderTarget_Destroy);

	MFRenderTarget_InitModulePlatformSpecific();

	return MFIS_Succeeded;
}

void MFRenderTarget_DeinitModule()
{
	MFRenderTarget_DeinitModulePlatformSpecific();
}

MF_API MFRenderTarget* MFRenderTarget_Create(MFRenderTargetDesc *pDesc)
{
	for(int a=0; a<8; ++a)
	{
		if(pDesc->colourTargets[a].pSurface)
		{
			MFDebug_Assert(pDesc->colourTargets[a].pSurface->flags & TEX_RenderTarget, "Texture is not a render target!");
			MFResource_AddRef((MFResource*)pDesc->colourTargets[a].pSurface);
		}
		else if(pDesc->colourTargets[a].format != ImgFmt_Unknown)
			pDesc->colourTargets[a].pSurface = MFTexture_CreateRenderTarget(MFStr("%s_RenderTarget%d", pDesc->pName ? pDesc->pName : "Unnamed", a), pDesc->width, pDesc->height, pDesc->colourTargets[a].format);
	}

	if(pDesc->depthStencil.pSurface)
	{
		MFDebug_Assert(pDesc->depthStencil.pSurface->flags & TEX_RenderTarget, "Texture is not a render target!");
		MFResource_AddRef((MFResource*)pDesc->depthStencil.pSurface);
	}
	else if(pDesc->depthStencil.format != ImgFmt_Unknown)
		pDesc->depthStencil.pSurface = MFTexture_CreateRenderTarget(MFStr("%s_DepthStencil", pDesc->pName ? pDesc->pName : "Unnamed"), pDesc->width, pDesc->height, pDesc->depthStencil.format);

	size_t nameLen = pDesc->pName ? MFString_Length(pDesc->pName) + 1 : 0;
	MFRenderTarget *pRT = (MFRenderTarget*)MFHeap_Alloc(sizeof(MFRenderTarget) + nameLen);
	pRT->width = pDesc->width;
	pRT->height = pDesc->height;
	pRT->availableColourTargets = 0;
	for(int a=0; a<8; ++a)
	{
		pRT->pColourTargets[a] = pDesc->colourTargets[a].pSurface;
		if(pDesc->colourTargets[a].pSurface)
			pRT->availableColourTargets = 1 << a;
	}
	pRT->pDepthStencil = pDesc->depthStencil.pSurface;
	pRT->bHasDepth = pRT->pDepthStencil != NULL;
	pRT->bHasStencil = pRT->pDepthStencil && (pRT->pDepthStencil->imageFormat >= ImgFmt_D15S1 && pRT->pDepthStencil->imageFormat <= ImgFmt_D32FS8X24);

	MFRenderTarget_CreatePlatformSpecific(pRT);

	const char *pName = pDesc->pName ? MFString_Copy((char*)&pRT[1], pDesc->pName) : NULL;
	MFResource_AddResource(pRT, MFRT_RenderTarget, (uint32)MFUtil_HashPointer(pRT), pName);

	return pRT;
}

MF_API MFRenderTarget* MFRenderTarget_CreateSimple(const char *pName, int width, int height, MFImageFormat colourTargetFormat, MFImageFormat depthStencilFormat)
{
	MFRenderTargetDesc desc;
	desc.pName = pName;
	desc.width = width;
	desc.height = height;
	desc.colourTargets[0].format = colourTargetFormat;
	desc.depthStencil.format = depthStencilFormat;

	return MFRenderTarget_Create(&desc);
}

MF_API int MFRenderTarget_Release(MFRenderTarget *pRenderTarget)
{
	return MFResource_Release(pRenderTarget);
}

MF_API MFTexture* MFRenderTarget_GetColourTarget(MFRenderTarget *pRenderTarget, int index)
{
	return pRenderTarget->pColourTargets[index];
}

MF_API MFTexture* MFRenderTarget_GetDepthStencil(MFRenderTarget *pRenderTarget)
{
	return pRenderTarget->pDepthStencil;
}
