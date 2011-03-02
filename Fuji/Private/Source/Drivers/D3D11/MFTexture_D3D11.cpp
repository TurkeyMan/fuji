#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_D3D11
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_D3D11
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_D3D11
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_D3D11
	#define MFTexture_Destroy MFTexture_Destroy_D3D11
#endif

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"

/**** Globals ****/

/**** Functions ****/

void MFTexture_InitModulePlatformSpecific()
{
}

void MFTexture_DeinitModulePlatformSpecific()
{
}

void MFTexture_Release()
{

}

void MFTexture_Recreate()
{

}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFTextureFormat targetFormat)
{
	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	return 0;
}

#endif // MF_RENDERER
