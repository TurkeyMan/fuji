#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_D3D11 || defined(MF_RENDERPLUGIN_D3D11)

#if defined(MF_RENDERPLUGIN_D3D11)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_D3D11
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_D3D11
	#define MFTexture_Release MFTexture_Release_D3D11
	#define MFTexture_Recreate MFTexture_Recreate_D3D11
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_D3D11
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_D3D11
	#define MFTexture_DestroyPlatformSpecific MFTexture_DestroyPlatformSpecific_D3D11
	#define MFTexture_Update MFTexture_Update_D3D11
	#define MFTexture_Map MFTexture_Map_D3D11
	#define MFTexture_Unmap MFTexture_Unmap_D3D11
#endif

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"
#include "MFDebug.h"
#include "MFHeap.h"
#include "MFPtrList.h"

#include "MFRenderer_D3D11.h"


/**** Globals ****/

extern MFTexture *pNoneTexture;

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pImmediateContext;

static DXGI_FORMAT gD3D11Format[4][ImgFmt_Max] =
{
	// unorm
	{
		DXGI_FORMAT_B8G8R8A8_UNORM,			// ImgFmt_A8R8G8B8
		DXGI_FORMAT_R8G8B8A8_UNORM,			// ImgFmt_A8B8G8R8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8A8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8

		DXGI_FORMAT_R8G8_UNORM,				// ImgFmt_G8R8

		DXGI_FORMAT_R8_UNORM,				// ImgFmt_L8
		DXGI_FORMAT_A8_UNORM,				// ImgFmt_A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8L8

		DXGI_FORMAT_B5G6R5_UNORM,			// ImgFmt_R5G6B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R6G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B5G6R5

		DXGI_FORMAT_B5G5R5A1_UNORM,			// ImgFmt_A1R5G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G5B5A1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1B5G5R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4R4G4B4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4B4G4R4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R4G4B4A4

		DXGI_FORMAT_R16G16B16A16_UNORM,		// ImgFmt_A16B16G16R16

		DXGI_FORMAT_R16G16_UNORM,			// ImgFmt_G16R16

		DXGI_FORMAT_R16_UNORM,				// ImgFmt_L16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16L16

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2R10G10B10
		DXGI_FORMAT_R10G10B10A2_UNORM,		// ImgFmt_A2B10G10R10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R10G11B11
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10

		DXGI_FORMAT_R16G16B16A16_FLOAT,		// ImgFmt_ABGR_F16
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// ImgFmt_ABGR_F32
		DXGI_FORMAT_R16G16_FLOAT,			// ImgFmt_GR_F16
		DXGI_FORMAT_R32G32_FLOAT,			// ImgFmt_GR_F32
		DXGI_FORMAT_R16_FLOAT,				// ImgFmt_R_F16
		DXGI_FORMAT_R32_FLOAT,				// ImgFmt_R_F32

		DXGI_FORMAT_R11G11B10_FLOAT,		// ImgFmt_R11G11B10_F
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP,		// ImgFmt_R9G9B9_E5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P4

		DXGI_FORMAT_D16_UNORM,				// ImgFmt_D16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24X8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32
		DXGI_FORMAT_D32_FLOAT,				// ImgFmt_D32F

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D15S1
		DXGI_FORMAT_D24_UNORM_S8_UINT,		// ImgFmt_D24S8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24FS8
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,	// ImgFmt_D32FS8X24

		DXGI_FORMAT_BC1_UNORM,				// ImgFmt_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT2
		DXGI_FORMAT_BC2_UNORM,				// ImgFmt_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT4
		DXGI_FORMAT_BC3_UNORM,				// ImgFmt_DXT5
		DXGI_FORMAT_BC4_UNORM,				// ImgFmt_ATI1
		DXGI_FORMAT_BC5_UNORM,				// ImgFmt_ATI2
		DXGI_FORMAT_BC6H_UF16,				// ImgFmt_BPTC_F
		DXGI_FORMAT_BC7_UNORM,				// ImgFmt_BPTC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_CTX1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EACx2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGB
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA_EXPLICIT
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ASTC

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT5
	},

	// snorm
	{
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8R8G8B8
		DXGI_FORMAT_R8G8B8A8_SNORM,			// ImgFmt_A8B8G8R8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8A8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8

		DXGI_FORMAT_R8G8_SNORM,				// ImgFmt_G8R8

		DXGI_FORMAT_R8_SNORM,				// ImgFmt_L8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8L8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G6B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R6G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B5G6R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1R5G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G5B5A1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1B5G5R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4R4G4B4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4B4G4R4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R4G4B4A4

		DXGI_FORMAT_R16G16B16A16_SNORM,		// ImgFmt_A16B16G16R16

		DXGI_FORMAT_R16G16_SNORM,			// ImgFmt_G16R16

		DXGI_FORMAT_R16_SNORM,				// ImgFmt_L16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16L16

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2R10G10B10
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2B10G10R10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R10G11B11
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10

		DXGI_FORMAT_R16G16B16A16_FLOAT,		// ImgFmt_ABGR_F16
		DXGI_FORMAT_R32G32B32A32_FLOAT,		// ImgFmt_ABGR_F32
		DXGI_FORMAT_R16G16_FLOAT,			// ImgFmt_GR_F16
		DXGI_FORMAT_R32G32_FLOAT,			// ImgFmt_GR_F32
		DXGI_FORMAT_R16_FLOAT,				// ImgFmt_R_F16
		DXGI_FORMAT_R32_FLOAT,				// ImgFmt_R_F32

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R9G9B9_E5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P4

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24X8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32
		DXGI_FORMAT_D32_FLOAT,				// ImgFmt_D32F

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D15S1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24S8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24FS8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32FS8X24

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT5
		DXGI_FORMAT_BC4_SNORM,				// ImgFmt_ATI1
		DXGI_FORMAT_BC5_SNORM,				// ImgFmt_ATI2
		DXGI_FORMAT_BC6H_SF16,				// ImgFmt_BPTC_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_BPTC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_CTX1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EACx2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGB
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA_EXPLICIT
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ASTC

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT5
	},

	// uint
	{
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8R8G8B8
		DXGI_FORMAT_R8G8B8A8_UINT,			// ImgFmt_A8B8G8R8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8A8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8

		DXGI_FORMAT_R8G8_UINT,				// ImgFmt_G8R8

		DXGI_FORMAT_R8_UINT,				// ImgFmt_L8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8L8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G6B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R6G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B5G6R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1R5G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G5B5A1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1B5G5R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4R4G4B4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4B4G4R4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R4G4B4A4

		DXGI_FORMAT_R16G16B16A16_UINT,		// ImgFmt_A16B16G16R16

		DXGI_FORMAT_R16G16_UINT,			// ImgFmt_G16R16

		DXGI_FORMAT_R16_UINT,				// ImgFmt_L16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16L16

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2R10G10B10
		DXGI_FORMAT_R10G10B10A2_UINT,		// ImgFmt_A2B10G10R10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R10G11B11
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ABGR_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ABGR_F32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_GR_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_GR_F32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R_F32

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R9G9B9_E5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P4

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24X8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32F

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D15S1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24S8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24FS8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32FS8X24

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATI1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATI2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_BPTC_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_BPTC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_CTX1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EACx2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGB
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA_EXPLICIT
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ASTC

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT5
	},

	// sint
	{
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8R8G8B8
		DXGI_FORMAT_R8G8B8A8_SINT,			// ImgFmt_A8B8G8R8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8A8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R8G8B8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B8G8R8

		DXGI_FORMAT_R8G8_SINT,				// ImgFmt_G8R8

		DXGI_FORMAT_R8_SINT,				// ImgFmt_L8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A8L8

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G6B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R6G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_B5G6R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1R5G5B5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R5G5B5A1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A1B5G5R5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4R4G4B4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A4B4G4R4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R4G4B4A4

		DXGI_FORMAT_R16G16B16A16_SINT,		// ImgFmt_A16B16G16R16

		DXGI_FORMAT_R16G16_SINT,			// ImgFmt_G16R16

		DXGI_FORMAT_R16_SINT,				// ImgFmt_L16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A16L16

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2R10G10B10
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_A2B10G10R10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R10G11B11
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ABGR_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ABGR_F32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_GR_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_GR_F32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R_F16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R_F32

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R11G11B10_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_R9G9B9_E5

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_P4

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D16
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24X8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32F

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D15S1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24S8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D24FS8
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_D32FS8X24

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT4
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_DXT5
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATI1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATI2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_BPTC_F
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_BPTC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_CTX1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ETC2_EAC
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_EACx2
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGB_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC_RGBA_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_2bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PVRTC2_4bpp
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGB
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA_EXPLICIT
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ATCRGBA
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_ASTC

		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT1
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT3
		DXGI_FORMAT_UNKNOWN,				// ImgFmt_PSP_DXT5
	}
};

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
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	HRESULT hr;
	pTexture->pInternalData = NULL;

	// create texture
	DXGI_FORMAT platformFormat = gD3D11Format[(pTexture->imageFormat>>8)&3][pTexture->imageFormat];
//	hr = D3DX11CreateTextureFromMemory(pd3dDevice, pTexture->pSurfaces[0].width, pTexture->pSurfaces[0].height, generateMipChain ? 0 : 1, 0, platformFormat, D3DPOOL_MANAGED, (IDirect3DTexture9**)&pTexture->pInternalData);

	int pitch = (MFImage_GetBitsPerPixel(pTexture->imageFormat) / 8) * pTexture->pSurfaces[0].width;

	D3D11_TEXTURE2D_DESC desc;
	MFZeroMemory(&desc, sizeof(desc));
	desc.Width = pTexture->pSurfaces[0].width;
	desc.Height = pTexture->pSurfaces[0].height;
	desc.MipLevels = 1; //generateMipChain ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = platformFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT; // = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (pTexture->createFlags & MFTCF_GenerateMips) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	D3D11_SUBRESOURCE_DATA data[16];
	MFZeroMemory(&data, sizeof(data));
	data[0].pSysMem = (void*)(size_t)pTexture->pSurfaces[0].platformData;
	data[0].SysMemPitch = pitch;

	ID3D11Texture2D* pTex = NULL;

	hr = g_pd3dDevice->CreateTexture2D(&desc, data, &pTex);

//	MFDebug_Assert(hr != D3DERR_NOTAVAILABLE, MFStr("LoadTexture failed: D3DERR_NOTAVAILABLE, 0x%08X", hr));
//	MFDebug_Assert(hr != D3DERR_OUTOFVIDEOMEMORY, MFStr("LoadTexture failed: D3DERR_OUTOFVIDEOMEMORY, 0x%08X", hr));
//	MFDebug_Assert(hr != D3DERR_INVALIDCALL, MFStr("LoadTexture failed: D3DERR_INVALIDCALL, 0x%08X", hr));

	MFDebug_Assert(hr == S_OK, MFStr("Failed to create texture '%s'.", pTexture->pName));

	if (SUCCEEDED(hr))
	{
		MFRenderer_D3D11_SetDebugName(pTex, pTexture->pName);

		// filter mip levels
//		if (generateMipChain)
//			D3DX11FilterTexture(NULL, pTex, 0, D3DX11_FILTER_BOX);

		ID3D11ShaderResourceView *pSRV = NULL;

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		MFZeroMemory(&desc, sizeof(desc));
		desc.Format = platformFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = (uint32)-1;

		hr = g_pd3dDevice->CreateShaderResourceView(pTex, &desc, &pSRV);
		pTex->Release();

		pTexture->pInternalData = pSRV;
	}
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	if(pTexture->pInternalData)
	{
		ID3D11ShaderResourceView *pSRV = (ID3D11ShaderResourceView*)pTexture->pInternalData;
		pSRV->Release();
	}
}

MF_API bool MFTexture_Update(MFTexture *pTexture, int element, int mipLevel, const void *pData, size_t lineStride, size_t sliceStride)
{
	return false;
}

MF_API bool MFTexture_Map(MFTexture *pTexture, int element, int mipLevel, MFLockedTexture *pLock)
{
	return false;
}

MF_API void MFTexture_Unmap(MFTexture *pTexture, int element, int mipLevel)
{
}

#endif // MF_RENDERER
