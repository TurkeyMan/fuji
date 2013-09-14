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

	"I8",
	"I4",

	"D16",
	"D24X8",
	"D24S8",

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
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_A8R8G8B8
	MFBIT(MFRD_PSP)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PS2),	// ImgFmt_A8B8G8R8
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_B8G8R8A8
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_R8G8B8A8

	MFBIT(MFRD_D3D9),															// ImgFmt_R8G8B8
	0,																			// ImgFmt_B8G8R8

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL),										// ImgFmt_A2R10G10B10
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_A2B10G10R10

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_A16B16G16R16

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_R5G6B5
	MFBIT(MFRD_XBox),															// ImgFmt_R6G5B5
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_B5G6R5

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_A1R5G5B5
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_R5G5B5A1
	MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2)|MFBIT(MFRD_OpenGL),							// ImgFmt_A1B5G5R5

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),						// ImgFmt_A4R4G4B4
	MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP),											// ImgFmt_A4B4G4R4
	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),										// ImgFmt_R4G4B4A4

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_ABGR_F16
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_OpenGL),						// ImgFmt_ABGR_F32

	MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL)|MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),		// ImgFmt_I8
	MFBIT(MFRD_PSP)|MFBIT(MFRD_PS2),											// ImgFmt_I4

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL),										// ImgFmt_D16
	MFBIT(MFRD_D3D9),															// ImgFmt_D24X8
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_OpenGL),										// ImgFmt_D24S8

	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_DXT1 // 1 bit alpha
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_XBox),											// ImgFmt_DXT2
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_DXT3 // 3 bit discreet alpha
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_XBox),											// ImgFmt_DXT4
	MFBIT(MFRD_D3D9)|MFBIT(MFRD_D3D11)|MFBIT(MFRD_XBox)|MFBIT(MFRD_OpenGL),		// ImgFmt_DXT5 // 8 bit interpolated alpha

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

uint32 gMFImageBitsPerPixel[ImgFmt_Max] =
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

	8,	// ImgFmt_I8
	4,	// ImgFmt_I4

	16,	// ImgFmt_D16
	32,	// ImgFmt_D24X8
	32,	// ImgFmt_D24S8

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

uint32 gMFImagePlatformFormat[MFRD_Max][ImgFmt_Max] =
{
	{ // D3D9
		21,	// D3DFMT_A8R8G8B8		// ImgFmt_A8R8G8B8
		32,	// D3DFMT_A8B8G8R8		// ImgFmt_A8B8G8R8
		0,	//						// ImgFmt_B8G8R8A8
		0,	//						// ImgFmt_R8G8B8A8

		20,	// D3DFMT_R8G8B8		// ImgFmt_R8G8B8
		0,	//						// ImgFmt_B8G8R8

		35,	// D3DFMT_A2R10G10B10	// ImgFmt_A2R10G10B10
		31,	// D3DFMT_A2B10G10R10	// ImgFmt_A2B10G10R10

		36,	// D3DFMT_A16B16G16R16	// ImgFmt_A16B16G16R16

		23,	// D3DFMT_R5G6B5		// ImgFmt_R5G6B5
		0,	//						// ImgFmt_R6G5B5
		0,	//						// ImgFmt_B5G6R5

		25,	// D3DFMT_A1R5G5B5		// ImgFmt_A1R5G5B5
		0,	//						// ImgFmt_R5G5B5A1
		0,	//						// ImgFmt_A1B5G5R5

		26,	// D3DFMT_A4R4G4B4		// ImgFmt_A4R4G4B4
		0,	//						// ImgFmt_A4B4G4R4
		0,	//						// ImgFmt_R4G4B4A4

		113,// D3DFMT_A16B16G16R16F	// ImgFmt_ABGR_F16
		116,// D3DFMT_A32B32G32R32F	// ImgFmt_ABGR_F32

		41,	// D3DFMT_P8			// ImgFmt_I8
		0,	//						// ImgFmt_I4

		80,	// D3DFMT_D16			// ImgFmt_D16
		77,	// D3DFMT_D24X8			// ImgFmt_D24X8
		75,	// D3DFMT_D24S8			// ImgFmt_D24S8

		MFMAKEFOURCC('D', 'X', 'T', '1'),	// D3DFMT_DXT1	// ImgFmt_DXT1
		MFMAKEFOURCC('D', 'X', 'T', '2'),	// D3DFMT_DXT2	// ImgFmt_DXT2
		MFMAKEFOURCC('D', 'X', 'T', '3'),	// D3DFMT_DXT3	// ImgFmt_DXT3
		MFMAKEFOURCC('D', 'X', 'T', '4'),	// D3DFMT_DXT4	// ImgFmt_DXT4
		MFMAKEFOURCC('D', 'X', 'T', '5'),	// D3DFMT_DXT5	// ImgFmt_DXT5

		0,	// ImgFmt_PSP_DXT1
		0,	// ImgFmt_PSP_DXT3
		0,	// ImgFmt_PSP_DXT5

		0,	// ImgFmt_XB_A8R8G8B8
		0,	// ImgFmt_XB_A8B8G8R8
		0,	// ImgFmt_XB_B8G8R8A8
		0,	// ImgFmt_XB_R8G8B8A8

		0,	// ImgFmt_XB_R5G6B5
		0,	// ImgFmt_XB_R6G5B5

		0,	// ImgFmt_XB_A1R5G5B5
		0,	// ImgFmt_XB_R5G5B5A1

		0,	// ImgFmt_XB_A4R4G4B4
		0,	// ImgFmt_XB_R4G4B4A4

		0,	// ImgFmt_PSP_A8B8G8R8s
		0,	// ImgFmt_PSP_B5G6R5s
		0,	// ImgFmt_PSP_A1B5G5R5s
		0,	// ImgFmt_PSP_A4B4G4R4s

		0,	// ImgFmt_PSP_I8s
		0,	// ImgFmt_PSP_I4s

		0,	// ImgFmt_PSP_DXT1s
		0,	// ImgFmt_PSP_DXT3s
		0,	// ImgFmt_PSP_DXT5s
	},

	{ // D3D11
		87,	// DXGI_FORMAT_B8G8R8A8_UNORM		// ImgFmt_A8R8G8B8
		28,	// DXGI_FORMAT_R8G8B8A8_UNORM		// ImgFmt_A8B8G8R8
		0,	//									// ImgFmt_B8G8R8A8
		0,	//									// ImgFmt_R8G8B8A8

		0,	//									// ImgFmt_R8G8B8
		0,	//									// ImgFmt_B8G8R8

		0,	//									// ImgFmt_A2R10G10B10
		24,	// DXGI_FORMAT_R10G10B10A2_UNORM	// ImgFmt_A2B10G10R10

		11,	// DXGI_FORMAT_R16G16B16A16_UNORM	// ImgFmt_A16B16G16R16

		85,	// DXGI_FORMAT_B5G6R5_UNORM			// ImgFmt_R5G6B5
		0,	//									// ImgFmt_R6G5B5
		0,	//									// ImgFmt_B5G6R5

		86,	// DXGI_FORMAT_B5G5R5A1_UNORM		// ImgFmt_A1R5G5B5
		0,	//									// ImgFmt_R5G5B5A1
		0,	//									// ImgFmt_A1B5G5R5

		0,	//									// ImgFmt_A4R4G4B4
		0,	//									// ImgFmt_A4B4G4R4
		0,	//									// ImgFmt_R4G4B4A4

		10,	// DXGI_FORMAT_R16G16B16A16_FLOAT	// ImgFmt_ABGR_F16
		2,	// DXGI_FORMAT_R32G32B32A32_FLOAT	// ImgFmt_ABGR_F32

		0,	//									// ImgFmt_I8
		0,	//									// ImgFmt_I4

		55,	// DXGI_FORMAT_D16_UNORM			// ImgFmt_D16
		0,	//									// ImgFmt_D24X8
		45,	// DXGI_FORMAT_D24_UNORM_S8_UINT	// ImgFmt_D24S8

		71,	// DXGI_FORMAT_BC1_UNORM			// ImgFmt_DXT1
		0,	//									// ImgFmt_DXT2
		74,	// DXGI_FORMAT_BC2_UNORM			// ImgFmt_DXT3
		0,	//									// ImgFmt_DXT4
		77,	// DXGI_FORMAT_BC3_UNORM			// ImgFmt_DXT5

		0,	// ImgFmt_PSP_DXT1
		0,	// ImgFmt_PSP_DXT3
		0,	// ImgFmt_PSP_DXT5

		0,	// ImgFmt_XB_A8R8G8B8
		0,	// ImgFmt_XB_A8B8G8R8
		0,	// ImgFmt_XB_B8G8R8A8
		0,	// ImgFmt_XB_R8G8B8A8

		0,	// ImgFmt_XB_R5G6B5
		0,	// ImgFmt_XB_R6G5B5

		0,	// ImgFmt_XB_A1R5G5B5
		0,	// ImgFmt_XB_R5G5B5A1

		0,	// ImgFmt_XB_A4R4G4B4
		0,	// ImgFmt_XB_R4G4B4A4

		0,	// ImgFmt_PSP_A8B8G8R8s
		0,	// ImgFmt_PSP_B5G6R5s
		0,	// ImgFmt_PSP_A1B5G5R5s
		0,	// ImgFmt_PSP_A4B4G4R4s

		0,	// ImgFmt_PSP_I8s
		0,	// ImgFmt_PSP_I4s

		0,	// ImgFmt_PSP_DXT1s
		0,	// ImgFmt_PSP_DXT3s
		0,	// ImgFmt_PSP_DXT5s
	},

	{ // XBox
		0x12,	// ImgFmt_A8R8G8B8
		0x3F,	// ImgFmt_A8B8G8R8
		0x40,	// ImgFmt_B8G8R8A8
		0x41,	// ImgFmt_R8G8B8A8

		0,		// ImgFmt_R8G8B8
		0,		// ImgFmt_B8G8R8

		0,		// ImgFmt_A2R10G10B10
		0,		// ImgFmt_A2B10G10R10

		0,		// ImgFmt_A16B16G16R16

		0x11,	// ImgFmt_R5G6B5
		0x37,	// ImgFmt_R6G5B5
		0,		// ImgFmt_B5G6R5

		0x10,	// ImgFmt_A1R5G5B5
		0x3D,	// ImgFmt_R5G5B5A1
		0,		// ImgFmt_A1B5G5R5

		0x1D,	// ImgFmt_A4R4G4B4
		0,		// ImgFmt_A4B4G4R4
		0x3E,	// ImgFmt_R4G4B4A4

		0,		// ImgFmt_ABGR_F16
		0,		// ImgFmt_ABGR_F32

		0xB,	// ImgFmt_I8
		0,		// ImgFmt_I4

		0,		// ImgFmt_D16
		0,		// ImgFmt_D24X8
		0,		// ImgFmt_D24S8

		0xC,	// ImgFmt_DXT1
		0xE,	// ImgFmt_DXT2
		0xE,	// ImgFmt_DXT3
		0xF,	// ImgFmt_DXT4
		0xF,	// ImgFmt_DXT5

		0,		// ImgFmt_PSP_DXT1
		0,		// ImgFmt_PSP_DXT3
		0,		// ImgFmt_PSP_DXT5

		0x6,	// ImgFmt_XB_A8R8G8B8
		0x3A,	// ImgFmt_XB_A8B8G8R8
		0x3B,	// ImgFmt_XB_B8G8R8A8
		0x3C,	// ImgFmt_XB_R8G8B8A8

		0x5,	// ImgFmt_XB_R5G6B5
		0x27,	// ImgFmt_XB_R6G5B5

		0x2,	// ImgFmt_XB_A1R5G5B5
		0x38,	// ImgFmt_XB_R5G5B5A1

		0x4,	// ImgFmt_XB_A4R4G4B4
		0x39,	// ImgFmt_XB_R4G4B4A4

		0,		// ImgFmt_PSP_A8B8G8R8s
		0,		// ImgFmt_PSP_B5G6R5s
		0,		// ImgFmt_PSP_A1B5G5R5s
		0,		// ImgFmt_PSP_A4B4G4R4s

		0,		// ImgFmt_PSP_I8s
		0,		// ImgFmt_PSP_I4s

		0,		// ImgFmt_PSP_DXT1s
		0,		// ImgFmt_PSP_DXT3s
		0,		// ImgFmt_PSP_DXT5s
	},

	// These are indexes into an array of platform format data to be used at runtime
	// As more formats are supported, the runtime internal format array needs to be filled out aswell..
	{ // OpenGL
		0,	// ImgFmt_A8R8G8B8
		1,	// ImgFmt_A8B8G8R8
		2,	// ImgFmt_B8G8R8A8
		3,	// ImgFmt_R8G8B8A8

		0,	// ImgFmt_R8G8B8
		0,	// ImgFmt_B8G8R8

		4,	// ImgFmt_A2R10G10B10
		5,	// ImgFmt_A2B10G10R10

		6,	// ImgFmt_A16B16G16R16

		7,	// ImgFmt_R5G6B5
		0,	// ImgFmt_R6G5B5
		8,	// ImgFmt_B5G6R5

		9,	// ImgFmt_A1R5G5B5
		10,	// ImgFmt_R5G5B5A1
		11,	// ImgFmt_A1B5G5R5

		12,	// ImgFmt_A4R4G4B4
		13,	// ImgFmt_A4B4G4R4
		14,	// ImgFmt_R4G4B4A4

		15,	// ImgFmt_ABGR_F16
		16,	// ImgFmt_ABGR_F32

		17,	// ImgFmt_I8
		0,	// ImgFmt_I4

		18,	// ImgFmt_D16
		0,	// ImgFmt_D24X8
		19,	// ImgFmt_D24S8

		20,	// ImgFmt_DXT1
		0,	// ImgFmt_DXT2
		21,	// ImgFmt_DXT3
		0,	// ImgFmt_DXT4
		22,	// ImgFmt_DXT5

		0,	// ImgFmt_PSP_DXT1
		0,	// ImgFmt_PSP_DXT3
		0,	// ImgFmt_PSP_DXT5

		0,	// ImgFmt_XB_A8R8G8B8
		0,	// ImgFmt_XB_A8B8G8R8
		0,	// ImgFmt_XB_B8G8R8A8
		0,	// ImgFmt_XB_R8G8B8A8

		0,	// ImgFmt_XB_R5G6B5
		0,	// ImgFmt_XB_R6G5B5

		0,	// ImgFmt_XB_A1R5G5B5
		0,	// ImgFmt_XB_R5G5B5A1

		0,	// ImgFmt_XB_A4R4G4B4
		0,	// ImgFmt_XB_R4G4B4A4

		0,	// ImgFmt_PSP_A8B8G8R8s
		0,	// ImgFmt_PSP_B5G6R5s
		0,	// ImgFmt_PSP_A1B5G5R5s
		0,	// ImgFmt_PSP_A4B4G4R4s

		0,	// ImgFmt_PSP_I8s
		0,	// ImgFmt_PSP_I4s

		0,	// ImgFmt_PSP_DXT1s
		0,	// ImgFmt_PSP_DXT3s
		0,	// ImgFmt_PSP_DXT5s
	},

	{ // PSP
		0,	//				 // ImgFmt_A8R8G8B8
		3,	// SCEGU_PF8888, //	ImgFmt_A8B8G8R8
		0,	//				 // ImgFmt_B8G8R8A8
		0,	//				 // ImgFmt_R8G8B8A8

		0,	//				 //	ImgFmt_R8G8B8
		0,	//				 // ImgFmt_B8G8R8

		0,	//				 // ImgFmt_A2R10G10B10
		0,	//				 // ImgFmt_A2B10G10R10

		0,	//				 // ImgFmt_A16B16G16R16

		0,	//				 //	ImgFmt_R5G6B5
		0,	//				 // ImgFmt_R6G5B5
		0,	// SCEGU_PF5650, // ImgFmt_B5G6R5

		0,	//               //	ImgFmt_A1R5G5B5
		0,	//				 // ImgFmt_R5G5B5A1
		1,	// SCEGU_PF5551, //	ImgFmt_A1B5G5R5

		0,	//				 // ImgFmt_A4R4G4B4
		2,	// SCEGU_PF4444, //	ImgFmt_A4B4G4R4
		0,	//				 // ImgFmt_R4G4B4A4

		0,	//				 // ImgFmt_ABGR_F16
		0,	//				 // ImgFmt_ABGR_F32

		5,	// SCEGU_PFIDX8, //	ImgFmt_I8
		4,	// SCEGU_PFIDX4, //	ImgFmt_I4

		0,	//				 // ImgFmt_D16
		0,	//				 // ImgFmt_D24X8
		0,	//				 // ImgFmt_D24S8

		0,	//				 // ImgFmt_DXT1
		0,	//				 // ImgFmt_DXT2
		0,	//				 // ImgFmt_DXT3
		0,	//				 // ImgFmt_DXT4
		0,	//				 // ImgFmt_DXT5

		8,	// SCEGU_PFDXT1, //	ImgFmt_PSP_DXT1
		9,	// SCEGU_PFDXT3, //	ImgFmt_PSP_DXT3
		10,	// SCEGU_PFDXT5, //	ImgFmt_PSP_DXT5

		0,	//				 // ImgFmt_XB_A8R8G8B8
		0,	//				 // ImgFmt_XB_A8B8G8R8
		0,	//				 // ImgFmt_XB_B8G8R8A8
		0,	//				 // ImgFmt_XB_R8G8B8A8

		0,	//				 // ImgFmt_XB_R5G6B5
		0,	//				 // ImgFmt_XB_R6G5B5

		0,	//				 // ImgFmt_XB_A1R5G5B5
		0,	//				 // ImgFmt_XB_R5G5B5A1

		0,	//				 // ImgFmt_XB_A4R4G4B4
		0,	//				 // ImgFmt_XB_R4G4B4A4

		3,	// SCEGU_PF8888, //	ImgFmt_PSP_A8B8G8R8s
		0,	// SCEGU_PF5650, // ImgFmt_PSP_B5G6R5s
		1,	// SCEGU_PF5551, //	ImgFmt_PSP_A1B5G5R5s
		2,	// SCEGU_PF4444, //	ImgFmt_PSP_A4B4G4R4s

		5,	// SCEGU_PFIDX8, //	ImgFmt_PSP_I8s
		4,	// SCEGU_PFIDX4, //	ImgFmt_PSP_I4s

		8,	// SCEGU_PFDXT1, //	ImgFmt_PSP_DXT1s
		9,	// SCEGU_PFDXT3, //	ImgFmt_PSP_DXT3s
		10,	// SCEGU_PFDXT5, //	ImgFmt_PSP_DXT5s
	},

	{ // PS2
		0,	//				 // ImgFmt_A8R8G8B8
		99,	// ????????????, //	ImgFmt_A8B8G8R8
		0,	//				 // ImgFmt_B8G8R8A8
		0,	//				 // ImgFmt_R8G8B8A8

		0,	//				 // ImgFmt_R8G8B8
		0,	//				 // ImgFmt_B8G8R8

		0,	//				 // ImgFmt_A2R10G10B10
		0,	//				 // ImgFmt_A2B10G10R10

		0,	//				 // ImgFmt_A16B16G16R16

		0,	//				 //	ImgFmt_R5G6B5
		0,	//				 // ImgFmt_R6G5B5
		0,	//				 // ImgFmt_B5G6R5

		0,	//				 //	ImgFmt_A1R5G5B5
		0,	//				 // ImgFmt_R5G5B5A1
		99,	// ????????????, //	ImgFmt_A1B5G5R5

		0,	//				 // ImgFmt_A4R4G4B4
		0,	//				 //	ImgFmt_A4B4G4R4
		0,	//				 // ImgFmt_R4G4B4A4

		0,	//				 // ImgFmt_ABGR_F16
		0,	//				 // ImgFmt_ABGR_F32

		99,	// ????????????, //	ImgFmt_I8
		99,	// ????????????, //	ImgFmt_I4

		0,	//				 // ImgFmt_D16
		0,	//				 // ImgFmt_D24X8
		0,	//				 // ImgFmt_D24S8

		0,	//				 // ImgFmt_DXT1
		0,	//				 // ImgFmt_DXT2
		0,	//				 // ImgFmt_DXT3
		0,	//				 // ImgFmt_DXT4
		0,	//				 // ImgFmt_DXT5

		0,	//				 //	ImgFmt_PSP_DXT1
		0,	//				 //	ImgFmt_PSP_DXT3
		0,	//				 //	ImgFmt_PSP_DXT5

		0,	//				 // ImgFmt_XB_A8R8G8B8
		0,	//				 // ImgFmt_XB_A8B8G8R8
		0,	//				 // ImgFmt_XB_B8G8R8A8
		0,	//				 // ImgFmt_XB_R8G8B8A8

		0,	//				 // ImgFmt_XB_R5G6B5
		0,	//				 // ImgFmt_XB_R6G5B5

		0,	//				 // ImgFmt_XB_A1R5G5B5
		0,	//				 // ImgFmt_XB_R5G5B5A1

		0,	//				 // ImgFmt_XB_A4R4G4B4
		0,	//				 // ImgFmt_XB_R4G4B4A4

		0,	//				 //	ImgFmt_PSP_A8B8G8R8s
		0,	//				 // ImgFmt_PSP_B5G6R5s
		0,	//				 //	ImgFmt_PSP_A1B5G5R5s
		0,	//				 //	ImgFmt_PSP_A4B4G4R4s

		0,	//				 //	ImgFmt_PSP_I8s
		0,	//				 //	ImgFmt_PSP_I4s

		0,	//				 //	ImgFmt_PSP_DXT1s
		0,	//				 //	ImgFmt_PSP_DXT3s
		0,	//				 //	ImgFmt_PSP_DXT5s
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
#elif MF_RENDERER == MF_DRIVER_XBOX
	return MFRD_XBox;
#elif MF_RENDERER == MF_DRIVER_OPENGL
	return MFRD_OpenGL;
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
