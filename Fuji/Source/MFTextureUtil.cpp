#include "Fuji_Internal.h"
#include "MFTexture_Internal.h"

const char *gpMFImageFormatStrings[ImgFmt_Max] =
{
	"A8R8G8B8",
	"A8B8G8R8",
	"B8G8R8A8",
	"R8G8B8A8",

	"R8G8B8",
	"B8G8R8",

	"G8R8",

	"L8",
	"A8",
	"A8L8",

	"R5G6B5",
	"R6G5B5",
	"B6G5R5",

	"A1R5G5B5",
	"R5G5B5A1",
	"A1B5G5R5",

	"A4R4G4B4",
	"A4B4G4R4",
	"R4G4B4A4",

	"A16B16G16R16",

	"G16R16",

	"L16",
	"A16",
	"A16L16",

	"A2R10G10B10",
	"A2B10G10R10",

	"R10G11B11",
	"R11G11B10",

	"ABGR_F16",
	"ABGR_F32",
	"GR_F16",
	"GR_F32",
	"R_F16",
	"R_F32",

	"R11G11B10_F",
	"R9G9B9_E5",

	"P8",
	"P4",

	"D16",
	"D24X8",
	"D32",
	"D32F",

	"D15S1",
	"D24S8",
	"D24FS8",
	"D32FS8X24",

	"DXT1",
	"DXT2",
	"DXT3",
	"DXT4",
	"DXT5",
	"ATI1",
	"ATI2",
	"BPTC_F",
	"BPTC",
	"CTX1",
	"ETC1",
	"ETC2",
	"EAC",
	"ETC2_EAC",
	"EACx2",
	"PVRTC_RGB_2bpp",
	"PVRTC_RGB_4bpp",
	"PVRTC_RGBA_2bpp",
	"PVRTC_RGBA_4bpp",
	"PVRTC2_2bpp",
	"PVRTC2_4bpp",
	"ATCRGB",
	"ATCRGBA_EXPLICIT",
	"ATCRGBA",
	"ASTC",

	"PSP_DXT1",
	"PSP_DXT3",
	"PSP_DXT5",
};

uint8 gMFImagePlatformAvailability[4][ImgFmt_Max] =
{
	// normalised
	{
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),		// ImgFmt_A8R8G8B8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),	// ImgFmt_A8B8G8R8
		MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_B8G8R8A8
		MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_R8G8B8A8

		MFBIT(MFRD_D3D9),															// ImgFmt_R8G8B8
		0,																			// ImgFmt_B8G8R8

		MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),											// ImgFmt_G8R8

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_L8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_A8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_X360),											// ImgFmt_A8L8

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_R5G6B5
		MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),											// ImgFmt_R6G5B5
		MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_B5G6R5

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_A1R5G5B5
		MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),										// ImgFmt_R5G5B5A1
		MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),							// ImgFmt_A1B5G5R5

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),		// ImgFmt_A4R4G4B4
		MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_A4B4G4R4
		MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),										// ImgFmt_R4G4B4A4

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_A16B16G16R16

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_G16R16

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_L16
		0,																			// ImgFmt_A16
		MFBIT(MFRD_X360),															// ImgFmt_A16L16

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_A2R10G10B10
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_A2B10G10R10

		MFBIT(MFRD_X360),															// ImgFmt_R10G11B11
		MFBIT(MFRD_X360),															// ImgFmt_R11G11B10

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_GR_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_GR_F32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_R_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_R_F32

		MFBIT(MFRD_D3D11),															// ImgFmt_R11G11B10_F
		MFBIT(MFRD_D3D11),															// ImgFmt_R9G9B9_E5

		MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),		// ImgFmt_P8
		MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),											// ImgFmt_P4

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_D16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_D24X8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_D32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_D32F

		MFBIT(MFRD_D3D9),															// ImgFmt_D15S1
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_D24S8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_D24FS8
		MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),										// ImgFmt_D32FS8X24

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT1 // 1 bit alpha
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),							// ImgFmt_DXT2
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT3 // 3 bit discreet alpha
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),							// ImgFmt_DXT4
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT5 // 8 bit interpolated alpha
		MFBIT(MFRD_D3D11),															// ImgFmt_ATI1
		MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),											// ImgFmt_ATI2
		MFBIT(MFRD_D3D11),															// ImgFmt_BPTC_F
		MFBIT(MFRD_D3D11),															// ImgFmt_BPTC
		MFBIT(MFRD_X360),															// ImgFmt_CTX1
		0,																			// ImgFmt_ETC1
		0,																			// ImgFmt_ETC2
		0,																			// ImgFmt_EAC
		0,																			// ImgFmt_ETC2_EAC
		0,																			// ImgFmt_EACx2
		0,																			// ImgFmt_PVRTC_RGB_2bpp
		0,																			// ImgFmt_PVRTC_RGB_4bpp
		0,																			// ImgFmt_PVRTC_RGBA_2bpp
		0,																			// ImgFmt_PVRTC_RGBA_4bpp
		0,																			// ImgFmt_PVRTC2_2bpp
		0,																			// ImgFmt_PVRTC2_4bpp
		0,																			// ImgFmt_ATCRGB
		0,																			// ImgFmt_ATCRGBA_EXPLICIT
		0,																			// ImgFmt_ATCRGBA
		0,																			// ImgFmt_ASTC

		MFBIT(MFRD_PSP),															// ImgFmt_PSP_DXT1
		MFBIT(MFRD_PSP),															// ImgFmt_PSP_DXT3
		MFBIT(MFRD_PSP),															// ImgFmt_PSP_DXT5
	},

	// signed normalised
	{
		0,																			// ImgFmt_A8R8G8B8
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_A8B8G8R8
		0,																			// ImgFmt_B8G8R8A8
		0,																			// ImgFmt_R8G8B8A8

		0,																			// ImgFmt_R8G8B8
		0,																			// ImgFmt_B8G8R8

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_G8R8

		MFBIT(MFRD_D3D11),															// ImgFmt_L8
		0,																			// ImgFmt_A8
		0,																			// ImgFmt_A8L8

		0,																			// ImgFmt_R5G6B5
		0,																			// ImgFmt_R6G5B5
		0,																			// ImgFmt_B5G6R5

		0,																			// ImgFmt_A1R5G5B5
		0,																			// ImgFmt_R5G5B5A1
		0,																			// ImgFmt_A1B5G5R5

		0,																			// ImgFmt_A4R4G4B4
		0,																			// ImgFmt_A4B4G4R4
		0,																			// ImgFmt_R4G4B4A4

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_A16B16G16R16

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_X360),						// ImgFmt_G16R16

		MFBIT(MFRD_D3D11),															// ImgFmt_L16
		0,																			// ImgFmt_A16
		0,																			// ImgFmt_A16L16

		0,																			// ImgFmt_A2R10G10B10
		MFBIT(MFRD_X360),															// ImgFmt_A2B10G10R10

		MFBIT(MFRD_X360),															// ImgFmt_R10G11B11
		MFBIT(MFRD_X360),															// ImgFmt_R11G11B10

		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_GR_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_GR_F32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_R_F16
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_R_F32

		0,																			// ImgFmt_R11G11B10_F
		0,																			// ImgFmt_R9G9B9_E5

		0,																			// ImgFmt_P8
		0,																			// ImgFmt_P4

		0,																			// ImgFmt_D16
		0,																			// ImgFmt_D24X8
		0,																			// ImgFmt_D32
		MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_D32F

		0,																			// ImgFmt_D15S1
		0,																			// ImgFmt_D24S8
		0,																			// ImgFmt_D24FS8
		0,																			// ImgFmt_D32FS8X24

		0,																			// ImgFmt_DXT1 // 1 bit alpha
		0,																			// ImgFmt_DXT2
		0,																			// ImgFmt_DXT3 // 3 bit discreet alpha
		0,																			// ImgFmt_DXT4
		0,																			// ImgFmt_DXT5 // 8 bit interpolated alpha
		MFBIT(MFRD_D3D11),															// ImgFmt_ATI1
		MFBIT(MFRD_D3D11),															// ImgFmt_ATI2
		MFBIT(MFRD_D3D11),															// ImgFmt_BPTC_F
		0,																			// ImgFmt_BPTC
		0,																			// ImgFmt_CTX1
		0,																			// ImgFmt_ETC1
		0,																			// ImgFmt_ETC2
		0,																			// ImgFmt_EAC
		0,																			// ImgFmt_ETC2_EAC
		0,																			// ImgFmt_EACx2
		0,																			// ImgFmt_PVRTC_RGB_2bpp
		0,																			// ImgFmt_PVRTC_RGB_4bpp
		0,																			// ImgFmt_PVRTC_RGBA_2bpp
		0,																			// ImgFmt_PVRTC_RGBA_4bpp
		0,																			// ImgFmt_PVRTC2_2bpp
		0,																			// ImgFmt_PVRTC2_4bpp
		0,																			// ImgFmt_ATCRGB
		0,																			// ImgFmt_ATCRGBA_EXPLICIT
		0,																			// ImgFmt_ATCRGBA
		0,																			// ImgFmt_ASTC

		0,																			// ImgFmt_PSP_DXT1
		0,																			// ImgFmt_PSP_DXT3
		0,																			// ImgFmt_PSP_DXT5
	},

	// integer
	{
		0,																			// ImgFmt_A8R8G8B8
		MFBIT(MFRD_D3D11),															// ImgFmt_A8B8G8R8
		0,																			// ImgFmt_B8G8R8A8
		0,																			// ImgFmt_R8G8B8A8

		0,																			// ImgFmt_R8G8B8
		0,																			// ImgFmt_B8G8R8

		MFBIT(MFRD_D3D11),															// ImgFmt_G8R8

		MFBIT(MFRD_D3D11),															// ImgFmt_L8
		0,																			// ImgFmt_A8
		0,																			// ImgFmt_A8L8

		0,																			// ImgFmt_R5G6B5
		0,																			// ImgFmt_R6G5B5
		0,																			// ImgFmt_B5G6R5

		0,																			// ImgFmt_A1R5G5B5
		0,																			// ImgFmt_R5G5B5A1
		0,																			// ImgFmt_A1B5G5R5

		0,																			// ImgFmt_A4R4G4B4
		0,																			// ImgFmt_A4B4G4R4
		0,																			// ImgFmt_R4G4B4A4

		MFBIT(MFRD_D3D11),															// ImgFmt_A16B16G16R16

		MFBIT(MFRD_D3D11),															// ImgFmt_G16R16

		MFBIT(MFRD_D3D11),															// ImgFmt_L16
		0,																			// ImgFmt_A16
		0,																			// ImgFmt_A16L16

		0,																			// ImgFmt_A2R10G10B10
		MFBIT(MFRD_D3D11),															// ImgFmt_A2B10G10R10

		0,																			// ImgFmt_R10G11B11
		0,																			// ImgFmt_R11G11B10

		0,																			// ImgFmt_ABGR_F16
		0,																			// ImgFmt_ABGR_F32
		0,																			// ImgFmt_GR_F16
		0,																			// ImgFmt_GR_F32
		0,																			// ImgFmt_R_F16
		0,																			// ImgFmt_R_F32

		0,																			// ImgFmt_R11G11B10_F
		0,																			// ImgFmt_R9G9B9_E5

		0,																			// ImgFmt_P8
		0,																			// ImgFmt_P4

		0,																			// ImgFmt_D16
		0,																			// ImgFmt_D24X8
		0,																			// ImgFmt_D32
		0,																			// ImgFmt_D32F

		0,																			// ImgFmt_D15S1
		0,																			// ImgFmt_D24S8
		0,																			// ImgFmt_D24FS8
		0,																			// ImgFmt_D32FS8X24

		0,																			// ImgFmt_DXT1 // 1 bit alpha
		0,																			// ImgFmt_DXT2
		0,																			// ImgFmt_DXT3 // 3 bit discreet alpha
		0,																			// ImgFmt_DXT4
		0,																			// ImgFmt_DXT5 // 8 bit interpolated alpha
		0,																			// ImgFmt_ATI1
		0,																			// ImgFmt_ATI2
		0,																			// ImgFmt_BPTC_F
		0,																			// ImgFmt_BPTC
		0,																			// ImgFmt_CTX1
		0,																			// ImgFmt_ETC1
		0,																			// ImgFmt_ETC2
		0,																			// ImgFmt_EAC
		0,																			// ImgFmt_ETC2_EAC
		0,																			// ImgFmt_EACx2
		0,																			// ImgFmt_PVRTC_RGB_2bpp
		0,																			// ImgFmt_PVRTC_RGB_4bpp
		0,																			// ImgFmt_PVRTC_RGBA_2bpp
		0,																			// ImgFmt_PVRTC_RGBA_4bpp
		0,																			// ImgFmt_PVRTC2_2bpp
		0,																			// ImgFmt_PVRTC2_4bpp
		0,																			// ImgFmt_ATCRGB
		0,																			// ImgFmt_ATCRGBA_EXPLICIT
		0,																			// ImgFmt_ATCRGBA
		0,																			// ImgFmt_ASTC

		0,																			// ImgFmt_PSP_DXT1
		0,																			// ImgFmt_PSP_DXT3
		0,																			// ImgFmt_PSP_DXT5
	},
	
	// signed integer
	{
		0,																			// ImgFmt_A8R8G8B8
		MFBIT(MFRD_D3D11),															// ImgFmt_A8B8G8R8
		0,																			// ImgFmt_B8G8R8A8
		0,																			// ImgFmt_R8G8B8A8

		0,																			// ImgFmt_R8G8B8
		0,																			// ImgFmt_B8G8R8

		MFBIT(MFRD_D3D11),															// ImgFmt_G8R8

		MFBIT(MFRD_D3D11),															// ImgFmt_L8
		0,																			// ImgFmt_A8
		0,																			// ImgFmt_A8L8

		0,																			// ImgFmt_R5G6B5
		0,																			// ImgFmt_R6G5B5
		0,																			// ImgFmt_B5G6R5

		0,																			// ImgFmt_A1R5G5B5
		0,																			// ImgFmt_R5G5B5A1
		0,																			// ImgFmt_A1B5G5R5

		0,																			// ImgFmt_A4R4G4B4
		0,																			// ImgFmt_A4B4G4R4
		0,																			// ImgFmt_R4G4B4A4

		MFBIT(MFRD_D3D11),															// ImgFmt_A16B16G16R16

		MFBIT(MFRD_D3D11),															// ImgFmt_G16R16

		MFBIT(MFRD_D3D11),															// ImgFmt_L16
		0,																			// ImgFmt_A16
		0,																			// ImgFmt_A16L16

		0,																			// ImgFmt_A2R10G10B10
		0,																			// ImgFmt_A2B10G10R10

		0,																			// ImgFmt_R10G11B11
		0,																			// ImgFmt_R11G11B10

		0,																			// ImgFmt_ABGR_F16
		0,																			// ImgFmt_ABGR_F32
		0,																			// ImgFmt_GR_F16
		0,																			// ImgFmt_GR_F32
		0,																			// ImgFmt_R_F16
		0,																			// ImgFmt_R_F32

		0,																			// ImgFmt_R11G11B10_F
		0,																			// ImgFmt_R9G9B9_E5

		0,																			// ImgFmt_P8
		0,																			// ImgFmt_P4

		0,																			// ImgFmt_D16
		0,																			// ImgFmt_D24X8
		0,																			// ImgFmt_D32
		0,																			// ImgFmt_D32F

		0,																			// ImgFmt_D15S1
		0,																			// ImgFmt_D24S8
		0,																			// ImgFmt_D24FS8
		0,																			// ImgFmt_D32FS8X24

		0,																			// ImgFmt_DXT1 // 1 bit alpha
		0,																			// ImgFmt_DXT2
		0,																			// ImgFmt_DXT3 // 3 bit discreet alpha
		0,																			// ImgFmt_DXT4
		0,																			// ImgFmt_DXT5 // 8 bit interpolated alpha
		0,																			// ImgFmt_ATI1
		0,																			// ImgFmt_ATI2
		0,																			// ImgFmt_BPTC_F
		0,																			// ImgFmt_BPTC
		0,																			// ImgFmt_CTX1
		0,																			// ImgFmt_ETC1
		0,																			// ImgFmt_ETC2
		0,																			// ImgFmt_EAC
		0,																			// ImgFmt_ETC2_EAC
		0,																			// ImgFmt_EACx2
		0,																			// ImgFmt_PVRTC_RGB_2bpp
		0,																			// ImgFmt_PVRTC_RGB_4bpp
		0,																			// ImgFmt_PVRTC_RGBA_2bpp
		0,																			// ImgFmt_PVRTC_RGBA_4bpp
		0,																			// ImgFmt_PVRTC2_2bpp
		0,																			// ImgFmt_PVRTC2_4bpp
		0,																			// ImgFmt_ATCRGB
		0,																			// ImgFmt_ATCRGBA_EXPLICIT
		0,																			// ImgFmt_ATCRGBA
		0,																			// ImgFmt_ASTC

		0,																			// ImgFmt_PSP_DXT1
		0,																			// ImgFmt_PSP_DXT3
		0,																			// ImgFmt_PSP_DXT5
	},
};

uint8 gMFImageBitsPerPixel[ImgFmt_Max] =
{
	32,	// ImgFmt_A8R8G8B8
	32,	// ImgFmt_A8B8G8R8
	32,	// ImgFmt_B8G8R8A8
	32,	// ImgFmt_R8G8B8A8

	24,	// ImgFmt_R8G8B8
	24,	// ImgFmt_B8G8R8

	16,	// ImgFmt_G8R8

	8,	// ImgFmt_L8
	8,	// ImgFmt_A8
	16,	// ImgFmt_A8L8

	16,	// ImgFmt_R5G6B5
	16,	// ImgFmt_R6G5B5
	16,	// ImgFmt_B5G6R5

	16,	// ImgFmt_A1R5G5B5
	16,	// ImgFmt_R5G5B5A1
	16,	// ImgFmt_A1B5G5R5

	16,	// ImgFmt_A4R4G4B4
	16,	// ImgFmt_A4B4G4R4
	16,	// ImgFmt_R4G4B4A4

	64,	// ImgFmt_A16B16G16R16

	32,	// ImgFmt_G16R16

	16,	// ImgFmt_L16
	16,	// ImgFmt_A16
	32,	// ImgFmt_A16L16

	32,	// ImgFmt_A2R10G10B10
	32,	// ImgFmt_A2B10G10R10

	32,	// ImgFmt_R10G11B11
	32,	// ImgFmt_R11G11B10

	64,	// ImgFmt_ABGR_F16
	128,// ImgFmt_ABGR_F32
	32,	// ImgFmt_GR_F16
	64,	// ImgFmt_GR_F32
	16,	// ImgFmt_R_F16
	32,	// ImgFmt_R_F32

	32, // ImgFmt_R11G11B10_F
	32, // ImgFmt_R9G9B9_E5

	8,	// ImgFmt_P8
	4,	// ImgFmt_P4

	16, // ImgFmt_D16
	32, // ImgFmt_D24X8
	32, // ImgFmt_D32
	32, // ImgFmt_D32F

	16, // ImgFmt_D15S1
	32, // ImgFmt_D24S8
	32, // ImgFmt_D24FS8
	64, // ImgFmt_D32FS8X24

	4,	// ImgFmt_DXT1
	8,	// ImgFmt_DXT2
	8,	// ImgFmt_DXT3
	8,	// ImgFmt_DXT4
	8,	// ImgFmt_DXT5
	4,	// ImgFmt_ATI1
	8,	// ImgFmt_ATI2
	8,	// ImgFmt_BPTC_F
	8,	// ImgFmt_BPTC
	4,	// ImgFmt_CTX1
	4,	// ImgFmt_ETC1
	4,	// ImgFmt_ETC2
	4,	// ImgFmt_EAC
	8,	// ImgFmt_ETC2_EAC
	8,	// ImgFmt_EACx2
	2,	// ImgFmt_PVRTC_RGB_2bpp
	4,	// ImgFmt_PVRTC_RGB_4bpp
	2,	// ImgFmt_PVRTC_RGBA_2bpp
	4,	// ImgFmt_PVRTC_RGBA_4bpp
	2,	// ImgFmt_PVRTC2_2bpp
	4,	// ImgFmt_PVRTC2_4bpp
	4,	// ImgFmt_ATCRGB
	8,	// ImgFmt_ATCRGBA_EXPLICIT
	8,	// ImgFmt_ATCRGBA
	8,	// ImgFmt_ASTC

	4,	// ImgFmt_PSP_DXT1
	8,	// ImgFmt_PSP_DXT3
	8,	// ImgFmt_PSP_DXT5
};

int8 gMFImageAutoFormat[MFRD_Max][0x40] =
{
	{ // D3D9
		ImgFmt_A8R8G8B8, // ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectHDR
		ImgFmt_R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2R10G10B10, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2R10G10B10, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2R10G10B10, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2R10G10B10, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D15S1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D24FS8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // D3D11
		ImgFmt_A8R8G8B8, // ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_R9G9B9_E5, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_R11G11B10_F, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // OpenGL
		ImgFmt_A8R8G8B8, // ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNicest
		ImgFmt_R4G4B4A4, // ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_R5G6B5, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_R9G9B9_E5, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_R5G5B5A1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		ImgFmt_R4G4B4A4, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_R5G6B5, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_A8R8G8B8, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_A2B10G10R10, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_R5G5B5A1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_ABGR_F16, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24X8, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D16, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D32F, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		ImgFmt_D24S8, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		ImgFmt_D32FS8X24, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // XBox 360
		-1, // ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNicest
		-1, // ImgFmt_SelectFastest
		-1, // ImgFmt_SelectHDR
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // XBox
		-1, // ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNicest
		-1, // ImgFmt_SelectFastest
		-1, // ImgFmt_SelectHDR
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // PSP
		-1, // ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNicest
		-1, // ImgFmt_SelectFastest
		-1, // ImgFmt_SelectHDR
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	},
	{ // PS2
		-1, // ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNicest
		-1, // ImgFmt_SelectFastest
		-1, // ImgFmt_SelectHDR
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectRenderTarget | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepth | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1,
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_SelectNoAlpha | ImgFmt_SelectHDR
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectDefault
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectNicest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectFastest
		-1, // ImgFmt_SelectDepthStencil | ImgFmt_Select1BitAlpha | ImgFmt_SelectHDR
		-1,
		-1,
		-1,
		-1
	}
};

#if !defined(_FUJI_UTIL)
#include "MFRenderer.h"

MF_API bool MFTexture_IsFormatAvailable(int format)
{
	MFRendererDrivers driver = MFRenderer_GetCurrentRenderDriver();
	if(driver == MFRD_Unknown)
		return format == ImgFmt_A8R8G8B8;
	return (gMFImagePlatformAvailability[(format>>8)&3][format] & MFBIT(driver)) != 0;
}
#endif
