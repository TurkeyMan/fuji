#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_PSP

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <pspdisplay.h>
#include <pspgu.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

uint8 gPSPFormat[ImgFmt_Max] =
{
	0,	//				// ImgFmt_A8R8G8B8
	3,	// SCEGU_PF8888	// ImgFmt_A8B8G8R8
	0,	//				// ImgFmt_B8G8R8A8
	0,	//				// ImgFmt_R8G8B8A8

	0,	//				// ImgFmt_R8G8B8
	0,	//				// ImgFmt_B8G8R8

	0,	//				// ImgFmt_B8G8

	0,	//				// ImgFmt_L8
	0,	//				// ImgFmt_A8
	0,	//				// ImgFmt_A8L8

	0,	//				// ImgFmt_R5G6B5
	0,	//				// ImgFmt_R6G5B5
	0,	// SCEGU_PF5650	// ImgFmt_B5G6R5

	0,	//				// ImgFmt_A1R5G5B5
	0,	//				// ImgFmt_R5G5B5A1
	1,	// SCEGU_PF5551	// ImgFmt_A1B5G5R5

	0,	//				// ImgFmt_A4R4G4B4
	2,	// SCEGU_PF4444	// ImgFmt_A4B4G4R4
	0,	//				// ImgFmt_R4G4B4A4

	0,	//				// ImgFmt_A16B16G16R16

	0,	//				// ImgFmt_G16R16

	0,	//				// ImgFmt_L16
	0,	//				// ImgFmt_A16
	0,	//				// ImgFmt_A16L16

	0,	//				// ImgFmt_A2R10G10B10
	0,	//				// ImgFmt_A2B10G10R10

	0,	//				// ImgFmt_R10G11B11
	0,	//				// ImgFmt_R11G11B10

	0,	//				// ImgFmt_ABGR_F16
	0,	//				// ImgFmt_ABGR_F32
	0,	//				// ImgFmt_GR_F16
	0,	//				// ImgFmt_GR_F32
	0,	//				// ImgFmt_R_F16
	0,	//				// ImgFmt_R_F32

	0,	//				// ImgFmt_R11G11B10_F
	0,	//				// ImgFmt_R9G9B9_E5

	5,	// SCEGU_PFIDX8	// ImgFmt_P8
	4,	// SCEGU_PFIDX4	// ImgFmt_P4

	0,	//				// ImgFmt_D16
	0,	//				// ImgFmt_D24X8
	0,	//				// ImgFmt_D32
	0,	//				// ImgFmt_D32F

	0,	//				// ImgFmt_D15S1
	0,	//				// ImgFmt_D24S8
	0,	//				// ImgFmt_D24FS8
	0,	//				// ImgFmt_D32FS8X24

	0,	//				// ImgFmt_DXT1
	0,	//				// ImgFmt_DXT2
	0,	//				// ImgFmt_DXT3
	0,	//				// ImgFmt_DXT4
	0,	//				// ImgFmt_DXT5
	0,	//				// ImgFmt_ATI1
	0,	//				// ImgFmt_ATI2
	0,	//				// ImgFmt_BPTC_F
	0,	//				// ImgFmt_BPTC
	0,	//				// ImgFmt_CTX1
	0,	//				// ImgFmt_ETC1
	0,	//				// ImgFmt_ETC2
	0,	//				// ImgFmt_EAC
	0,	//				// ImgFmt_ETC2_EAC
	0,	//				// ImgFmt_EACx2
	0,	//				// ImgFmt_PVRTC_RGB_2bpp
	0,	//				// ImgFmt_PVRTC_RGB_4bpp
	0,	//				// ImgFmt_PVRTC_RGBA_2bpp
	0,	//				// ImgFmt_PVRTC_RGBA_4bpp
	0,	//				// ImgFmt_PVRTC2_2bpp
	0,	//				// ImgFmt_PVRTC2_4bpp
	0,	//				// ImgFmt_ATCRGB
	0,	//				// ImgFmt_ATCRGBA_EXPLICIT
	0,	//				// ImgFmt_ATCRGBA
	0,	//				// ImgFmt_ASTC

	8,	// SCEGU_PFDXT1	// ImgFmt_PSP_DXT1
	9,	// SCEGU_PFDXT3	// ImgFmt_PSP_DXT3
	10,	// SCEGU_PFDXT5	// ImgFmt_PSP_DXT5

	0,	//				// ImgFmt_XB_A8R8G8B8
	0,	//				// ImgFmt_XB_A8B8G8R8
	0,	//				// ImgFmt_XB_B8G8R8A8
	0,	//				// ImgFmt_XB_R8G8B8A8

	0,	//				// ImgFmt_XB_R5G6B5
	0,	//				// ImgFmt_XB_R6G5B5

	0,	//				// ImgFmt_XB_A1R5G5B5
	0,	//				// ImgFmt_XB_R5G5B5A1

	0,	//				// ImgFmt_XB_A4R4G4B4
	0,	//				// ImgFmt_XB_R4G4B4A4

	3,	// SCEGU_PF8888	// ImgFmt_PSP_A8B8G8R8s
	0,	// SCEGU_PF5650	// ImgFmt_PSP_B5G6R5s
	1,	// SCEGU_PF5551	// ImgFmt_PSP_A1B5G5R5s
	2,	// SCEGU_PF4444	// ImgFmt_PSP_A4B4G4R4s

	5,	// SCEGU_PFIDX8	// ImgFmt_PSP_I8s
	4,	// SCEGU_PFIDX4	// ImgFmt_PSP_I4s

	8,	// SCEGU_PFDXT1	// ImgFmt_PSP_DXT1s
	9,	// SCEGU_PFDXT3	// ImgFmt_PSP_DXT3s
	10,	// SCEGU_PFDXT5	// ImgFmt_PSP_DXT5s
};

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture)
{
	// no processing required on PSP..
}

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		MFHeap_Free(pTexture->pTemplateData);

		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif
