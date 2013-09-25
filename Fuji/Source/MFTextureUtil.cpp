#include "Fuji.h"
#include "MFTexture_Internal.h"

const char *gpMFImageFormatStrings[ImgFmt_Max] =
{
	"A8R8G8B8",
	"A8B8G8R8",
	"B8G8R8A8",
	"R8G8B8A8",

	"R8G8B8",
	"B8G8R8",

	"A2R10G10B10",
	"A2B10G10R10",

	"A16B16G16R16",

	"R5G6B5",
	"R6G5B5",
	"B6G5R5",

	"A1R5G5B5",
	"R5G5B5A1",
	"A1B5G5R5",

	"A4R4G4B4",
	"A4B4G4R4",
	"R4G4B4A4",

	"ABGR_F16",
	"ABGR_F32",

	"R11G11B10_F",
	"R9G9B9_E5",

	"I8",
	"I4",

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

	"PSP_DXT1",
	"PSP_DXT3",
	"PSP_DXT5",

	// platform specific swizzled formats
	"XB_A8R8G8B8s",
	"XB_A8B8G8R8s",
	"XB_B8G8R8A8s",
	"XB_R8G8B8A8s",

	"XB_R5G6B5s",
	"XB_R6G5B5s",

	"XB_A1R5G5B5s",
	"XB_R5G5B5A1s",

	"XB_A4R4G4B4s",
	"XB_R4G4B4A4s",

	"PSP_A8B8G8R8s",
	"PSP_B5G6R5s",
	"PSP_A1B5G5R5s",
	"PSP_A4B4G4R4s",

	"PSP_I8s",
	"PSP_I4s",

	"PSP_DXT1s",
	"PSP_DXT3s",
	"PSP_DXT5s",
};

uint32 gMFImagePlatformAvailability[ImgFmt_Max] =
{
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),		// ImgFmt_A8R8G8B8
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),	// ImgFmt_A8B8G8R8
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_B8G8R8A8
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_R8G8B8A8

	MFBIT(MFRD_D3D9),															// ImgFmt_R8G8B8
	0,																			// ImgFmt_B8G8R8

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_A2R10G10B10
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_A2B10G10R10

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_A16B16G16R16

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_R5G6B5
	MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),											// ImgFmt_R6G5B5
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_B5G6R5

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_A1R5G5B5
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),										// ImgFmt_R5G5B5A1
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),							// ImgFmt_A1B5G5R5

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),		// ImgFmt_A4R4G4B4
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_A4B4G4R4
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_XBox),										// ImgFmt_R4G4B4A4

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F16
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_ABGR_F32

	MFBIT(MFRD_D3D11),															// ImgFmt_R11G11B10_F
	MFBIT(MFRD_D3D11),															// ImgFmt_R9G9B9_E5

	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),		// ImgFmt_I8
	MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),											// ImgFmt_I4

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),		// ImgFmt_D16
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_D24X8
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL),										// ImgFmt_D32
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_D32F

	MFBIT(MFRD_D3D9),															// ImgFmt_D15S1
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360),						// ImgFmt_D24S8
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11),											// ImgFmt_D24FS8
	MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),										// ImgFmt_D32FS8X24

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT1 // 1 bit alpha
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),							// ImgFmt_DXT2
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT3 // 3 bit discreet alpha
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),							// ImgFmt_DXT4
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_X360)|MFBIT(MFRD_XBox),	// ImgFmt_DXT5 // 8 bit interpolated alpha

	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT1
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT3
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT5

	// platform specific swizzled formats
	MFBIT(MFRD_XBox),	// ImgFmt_XB_A8R8G8B8
	MFBIT(MFRD_XBox),	// ImgFmt_XB_A8B8G8R8
	MFBIT(MFRD_XBox),	// ImgFmt_XB_B8G8R8A8
	MFBIT(MFRD_XBox),	// ImgFmt_XB_R8G8B8A8

	MFBIT(MFRD_XBox),	// ImgFmt_XB_R5G6B5
	MFBIT(MFRD_XBox),	// ImgFmt_XB_R6G5B5

	MFBIT(MFRD_XBox),	// ImgFmt_XB_A1R5G5B5
	MFBIT(MFRD_XBox),	// ImgFmt_XB_R5G5B5A1

	MFBIT(MFRD_XBox),	// ImgFmt_XB_A4R4G4B4
	MFBIT(MFRD_XBox),	// ImgFmt_XB_R4G4B4A4

	MFBIT(MFRD_PSP),	// ImgFmt_PSP_A8B8G8R8s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_B5G6R5s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_A1B5G5R5s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_A4B4G4R4s

	MFBIT(MFRD_PSP),	// ImgFmt_PSP_I8s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_I4s

	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT1s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT3s
	MFBIT(MFRD_PSP),	// ImgFmt_PSP_DXT5s
};

uint8 gMFImageBitsPerPixel[ImgFmt_Max] =
{
	32,	// ImgFmt_A8R8G8B8
	32,	// ImgFmt_A8B8G8R8
	32,	// ImgFmt_B8G8R8A8
	32,	// ImgFmt_R8G8B8A8

	24,	// ImgFmt_R8G8B8
	24,	// ImgFmt_B8G8R8

	32,	// ImgFmt_A2R10G10B10
	32,	// ImgFmt_A2B10G10R10

	64,	// ImgFmt_A16B16G16R16

	16,	// ImgFmt_R5G6B5
	16,	// ImgFmt_R6G5B5
	16,	// ImgFmt_B5G6R5

	16,	// ImgFmt_A1R5G5B5
	16,	// ImgFmt_R5G5B5A1
	16,	// ImgFmt_A1B5G5R5

	16,	// ImgFmt_A4R4G4B4
	16,	// ImgFmt_A4B4G4R4
	16,	// ImgFmt_R4G4B4A4

	64,	// ImgFmt_ABGR_F16
	128,// ImgFmt_ABGR_F32

	32, // ImgFmt_R11G11B10_F
	32, // ImgFmt_R9G9B9_E5

	8,	// ImgFmt_I8
	4,	// ImgFmt_I4

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

	4,	// ImgFmt_PSP_DXT1
	8,	// ImgFmt_PSP_DXT3
	8,	// ImgFmt_PSP_DXT5

	32,	// ImgFmt_XB_A8R8G8B8
	32,	// ImgFmt_XB_A8B8G8R8
	32,	// ImgFmt_XB_B8G8R8A8
	32,	// ImgFmt_XB_R8G8B8A8

	16,	// ImgFmt_XB_R5G6B5
	16,	// ImgFmt_XB_R6G5B5

	16,	// ImgFmt_XB_A1R5G5B5
	16,	// ImgFmt_XB_R5G5B5A1

	16,	// ImgFmt_XB_A4R4G4B4
	16,	// ImgFmt_XB_R4G4B4A4

	32,	// ImgFmt_PSP_A8B8G8R8s
	16,	// ImgFmt_PSP_B5G6R5s
	16,	// ImgFmt_PSP_A1B5G5R5s
	16,	// ImgFmt_PSP_A4B4G4R4s

	8,	// ImgFmt_PSP_I8s
	4,	// ImgFmt_PSP_I4s

	4,	// ImgFmt_PSP_DXT1s
	8,	// ImgFmt_PSP_DXT3s
	8,	// ImgFmt_PSP_DXT5s
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
int MFRenderer_GetCurrentRendererPlugin();
MFRendererDrivers MFTexture_GetCurrentDisplayDriver()
{
#if MF_RENDERER == MF_DRIVER_PLUGIN
	// runtime check...
	int current = MFRenderer_GetCurrentRendererPlugin();
	if(current == MF_DRIVER_D3D9)
		return MFRD_D3D9;
	else if(current == MF_DRIVER_D3D11)
		return MFRD_D3D11;
	else if(current == MF_DRIVER_OPENGL)
		return MFRD_OpenGL;
	return MFRD_Unknown;
#elif MF_RENDERER == MF_DRIVER_D3D9
	return MFRD_D3D9;
#elif MF_RENDERER == MF_DRIVER_D3D11
	return MFRD_D3D11;
#elif MF_RENDERER == MF_DRIVER_OPENGL
	return MFRD_OpenGL;
#elif MF_RENDERER == MF_DRIVER_X360
	return MFRD_X360;
#elif MF_RENDERER == MF_DRIVER_XBOX
	return MFRD_XBox;
#elif MF_RENDERER == MF_DRIVER_PSP
	return MFRD_PSP;
#elif MF_RENDERER == MF_DRIVER_PS2
	return MFRD_PS2;
#else
	return MFRD_Unknown;
#endif
}

MF_API bool MFTexture_IsFormatAvailable(int format)
{
	MFRendererDrivers driver = MFTexture_GetCurrentDisplayDriver();
	if(driver == MFRD_Unknown)
		return format == ImgFmt_A8R8G8B8;
	return (gMFImagePlatformAvailability[format] & MFBIT(driver)) != 0;
}
#endif
