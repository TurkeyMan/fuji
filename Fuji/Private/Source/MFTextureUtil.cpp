#include "Fuji.h"
#include "MFTexture_Internal.h"

static const char * const gpMFTextureFormatStrings[TexFmt_Max] =
{
	"A8R8G8B8",
	"A8B8G8R8",
	"B8G8R8A8",
	"R8G8B8A8",

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

static uint32 gMFTexturePlatformAvailability[TexFmt_Max] =
{
	MFBIT(FP_PC)|MFBIT(FP_XBox)|MFBIT(FP_Linux),	// TexFmt_A8R8G8B8
	MFBIT(FP_PSP)|MFBIT(FP_XBox)|MFBIT(FP_Linux),	// TexFmt_A8B8G8R8
	MFBIT(FP_XBox),					// TexFmt_B8G8R8A8
	MFBIT(FP_XBox),					// TexFmt_R8G8B8A8

	MFBIT(FP_PC),					// TexFmt_A2R10G10B10
	MFBIT(FP_PC),					// TexFmt_A2B10G10R10

	MFBIT(FP_PC),					// TexFmt_A16B16G16R16

	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_R5G6B5
	MFBIT(FP_XBox),					// TexFmt_R6G5B5
	MFBIT(FP_PSP),					// TexFmt_B5G6R5

	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_A1R5G5B5
	MFBIT(FP_XBox),					// TexFmt_R5G5B5A1
	MFBIT(FP_PSP),					// TexFmt_A1B5G5R5

	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_A4R4G4B4
	MFBIT(FP_PSP),					// TexFmt_A4B4G4R4
	MFBIT(FP_XBox),					// TexFmt_R4G4B4A4

	MFBIT(FP_PC),					// TexFmt_ABGR_F16
	MFBIT(FP_PC),					// TexFmt_ABGR_F32

	MFBIT(FP_PSP)|MFBIT(FP_XBox),	// TexFmt_I8
	MFBIT(FP_PSP),					// TexFmt_I4

	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_DXT1
	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_DXT2
	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_DXT3
	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_DXT4
	MFBIT(FP_PC)|MFBIT(FP_XBox),	// TexFmt_DXT5

	MFBIT(FP_PSP),	// TexFmt_PSP_DXT1
	MFBIT(FP_PSP),	// TexFmt_PSP_DXT3
	MFBIT(FP_PSP),	// TexFmt_PSP_DXT5

	// platform specific swizzled formats
	MFBIT(FP_XBox),	// TexFmt_XB_A8R8G8B8
	MFBIT(FP_XBox),	// TexFmt_XB_A8B8G8R8
	MFBIT(FP_XBox),	// TexFmt_XB_B8G8R8A8
	MFBIT(FP_XBox),	// TexFmt_XB_R8G8B8A8

	MFBIT(FP_XBox),	// TexFmt_XB_R5G6B5
	MFBIT(FP_XBox),	// TexFmt_XB_R6G5B5

	MFBIT(FP_XBox),	// TexFmt_XB_A1R5G5B5
	MFBIT(FP_XBox),	// TexFmt_XB_R5G5B5A1

	MFBIT(FP_XBox),	// TexFmt_XB_A4R4G4B4
	MFBIT(FP_XBox),	// TexFmt_XB_R4G4B4A4

	MFBIT(FP_PSP),	// TexFmt_PSP_A8B8G8R8s
	MFBIT(FP_PSP),	// TexFmt_PSP_B5G6R5s
	MFBIT(FP_PSP),	// TexFmt_PSP_A1B5G5R5s
	MFBIT(FP_PSP),	// TexFmt_PSP_A4B4G4R4s

	MFBIT(FP_PSP),	// TexFmt_PSP_I8s
	MFBIT(FP_PSP),	// TexFmt_PSP_I4s

	MFBIT(FP_PSP),	// TexFmt_PSP_DXT1s
	MFBIT(FP_PSP),	// TexFmt_PSP_DXT3s
	MFBIT(FP_PSP),	// TexFmt_PSP_DXT5s
};

static uint32 gMFTextureBitsPerPixel[TexFmt_Max] =
{
	32,	// TexFmt_A8R8G8B8
	32,	// TexFmt_A8B8G8R8
	32,	// TexFmt_B8G8R8A8
	32,	// TexFmt_R8G8B8A8

	32,	// TexFmt_A2R10G10B10
	32,	// TexFmt_A2B10G10R10

	64,	// TexFmt_A16B16G16R16

	16,	// TexFmt_R5G6B5
	16,	// TexFmt_R6G5B5
	16,	// TexFmt_B5G6R5

	16,	// TexFmt_A1R5G5B5
	16,	// TexFmt_R5G5B5A1
	16,	// TexFmt_A1B5G5R5

	16,	// TexFmt_A4R4G4B4
	16,	// TexFmt_A4B4G4R4
	16,	// TexFmt_R4G4B4A4

	64,	// TexFmt_ABGR_F16
	128,// TexFmt_ABGR_F32

	8,	// TexFmt_I8
	4,	// TexFmt_I4

	4,	// TexFmt_DXT1
	8,	// TexFmt_DXT2
	8,	// TexFmt_DXT3
	8,	// TexFmt_DXT4
	8,	// TexFmt_DXT5

	4,	// TexFmt_PSP_DXT1
	8,	// TexFmt_PSP_DXT3
	8,	// TexFmt_PSP_DXT5

	32,	// TexFmt_XB_A8R8G8B8
	32,	// TexFmt_XB_A8B8G8R8
	32,	// TexFmt_XB_B8G8R8A8
	32,	// TexFmt_XB_R8G8B8A8

	16,	// TexFmt_XB_R5G6B5
	16,	// TexFmt_XB_R6G5B5

	16,	// TexFmt_XB_A1R5G5B5
	16,	// TexFmt_XB_R5G5B5A1

	16,	// TexFmt_XB_A4R4G4B4
	16,	// TexFmt_XB_R4G4B4A4

	32,	// TexFmt_PSP_A8B8G8R8s
	16,	// TexFmt_PSP_B5G6R5s
	16,	// TexFmt_PSP_A1B5G5R5s
	16,	// TexFmt_PSP_A4B4G4R4s

	8,	// TexFmt_PSP_I8s
	4,	// TexFmt_PSP_I4s

	4,	// TexFmt_PSP_DXT1s
	8,	// TexFmt_PSP_DXT3s
	8,	// TexFmt_PSP_DXT5s
};

uint32 gMFTexturePlatformFormat[FP_Max][TexFmt_Max] =
{
	{ // PC
		21,	// D3DFMT_A8R8G8B8		// TexFmt_A8R8G8B8
		32,	// D3DFMT_A8B8G8R8		// TexFmt_A8B8G8R8
		0,	//						// TexFmt_B8G8R8A8
		0,	//						// TexFmt_R8G8B8A8

		35,	// D3DFMT_A2R10G10B10	// TexFmt_A2R10G10B10
		31,	// D3DFMT_A2B10G10R10	// TexFmt_A2B10G10R10

		36,	// D3DFMT_A16B16G16R16	// TexFmt_A16B16G16R16

		23,	// D3DFMT_R5G6B5		// TexFmt_R5G6B5
		0,	//						// TexFmt_R6G5B5
		0,	//						// TexFmt_B5G6R5

		25,	// D3DFMT_A1R5G5B5		// TexFmt_A1R5G5B5
		0,	//						// TexFmt_R5G5B5A1
		0,	//						// TexFmt_A1B5G5R5

		26,	// D3DFMT_A4R4G4B4		// TexFmt_A4R4G4B4
		0,	//						// TexFmt_A4B4G4R4
		0,	//						// TexFmt_R4G4B4A4

		113,// D3DFMT_A16B16G16R16F	// TexFmt_ABGR_F16
		116,// D3DFMT_A32B32G32R32F	// TexFmt_ABGR_F32

		41,	// D3DFMT_P8			// TexFmt_I8
		0,	//						// TexFmt_I4

		MFMAKEFOURCC('D', 'X', 'T', '1'),	// D3DFMT_DXT1	// TexFmt_DXT1
		MFMAKEFOURCC('D', 'X', 'T', '2'),	// D3DFMT_DXT2	// TexFmt_DXT2
		MFMAKEFOURCC('D', 'X', 'T', '3'),	// D3DFMT_DXT3	// TexFmt_DXT3
		MFMAKEFOURCC('D', 'X', 'T', '4'),	// D3DFMT_DXT4	// TexFmt_DXT4
		MFMAKEFOURCC('D', 'X', 'T', '5'),	// D3DFMT_DXT5	// TexFmt_DXT5

		0,	// TexFmt_PSP_DXT1
		0,	// TexFmt_PSP_DXT3
		0,	// TexFmt_PSP_DXT5

		0,	// TexFmt_XB_A8R8G8B8
		0,	// TexFmt_XB_A8B8G8R8
		0,	// TexFmt_XB_B8G8R8A8
		0,	// TexFmt_XB_R8G8B8A8

		0,	// TexFmt_XB_R5G6B5
		0,	// TexFmt_XB_R6G5B5

		0,	// TexFmt_XB_A1R5G5B5
		0,	// TexFmt_XB_R5G5B5A1

		0,	// TexFmt_XB_A4R4G4B4
		0,	// TexFmt_XB_R4G4B4A4

		0,	// TexFmt_PSP_A8B8G8R8s
		0,	// TexFmt_PSP_B5G6R5s
		0,	// TexFmt_PSP_A1B5G5R5s
		0,	// TexFmt_PSP_A4B4G4R4s

		0,	// TexFmt_PSP_I8s
		0,	// TexFmt_PSP_I4s

		0,	// TexFmt_PSP_DXT1s
		0,	// TexFmt_PSP_DXT3s
		0,	// TexFmt_PSP_DXT5s
	},

	{ // XBox
		0x12,	// TexFmt_A8R8G8B8
		0x3F,	// TexFmt_A8B8G8R8
		0x40,	// TexFmt_B8G8R8A8
		0x41,	// TexFmt_R8G8B8A8

		0,		// TexFmt_A2R10G10B10
		0,		// TexFmt_A2B10G10R10

		0,		// TexFmt_A16B16G16R16

		0x11,	// TexFmt_R5G6B5
		0x37,	// TexFmt_R6G5B5
		0,		// TexFmt_B5G6R5

		0x10,	// TexFmt_A1R5G5B5
		0x3D,	// TexFmt_R5G5B5A1
		0,		// TexFmt_A1B5G5R5

		0x1D,	// TexFmt_A4R4G4B4
		0,		// TexFmt_A4B4G4R4
		0x3E,	// TexFmt_R4G4B4A4

		0,		// TexFmt_ABGR_F16
		0,		// TexFmt_ABGR_F32

		0xB,	// TexFmt_I8
		0,		// TexFmt_I4

		0xC,	// TexFmt_DXT1
		0xE,	// TexFmt_DXT2
		0xE,	// TexFmt_DXT3
		0xF,	// TexFmt_DXT4
		0xF,	// TexFmt_DXT5

		0,		// TexFmt_PSP_DXT1
		0,		// TexFmt_PSP_DXT3
		0,		// TexFmt_PSP_DXT5

		0x6,	// TexFmt_XB_A8R8G8B8
		0x3A,	// TexFmt_XB_A8B8G8R8
		0x3B,	// TexFmt_XB_B8G8R8A8
		0x3C,	// TexFmt_XB_R8G8B8A8

		0x5,	// TexFmt_XB_R5G6B5
		0x27,	// TexFmt_XB_R6G5B5

		0x2,	// TexFmt_XB_A1R5G5B5
		0x38,	// TexFmt_XB_R5G5B5A1

		0x4,	// TexFmt_XB_A4R4G4B4
		0x39,	// TexFmt_XB_R4G4B4A4

		0,		// TexFmt_PSP_A8B8G8R8s
		0,		// TexFmt_PSP_B5G6R5s
		0,		// TexFmt_PSP_A1B5G5R5s
		0,		// TexFmt_PSP_A4B4G4R4s

		0,		// TexFmt_PSP_I8s
		0,		// TexFmt_PSP_I4s

		0,		// TexFmt_PSP_DXT1s
		0,		// TexFmt_PSP_DXT3s
		0,		// TexFmt_PSP_DXT5s
	},

	// These are indexes into an array of platform format data to be used at runtime
	// As more formats are supported, the runtime internal format array needs to be filled out aswell..
	{ // Linux
		0,	// TexFmt_A8R8G8B8
		1,	// TexFmt_A8B8G8R8
		0,	// TexFmt_B8G8R8A8
		0,	// TexFmt_R8G8B8A8

		0,	// TexFmt_A2R10G10B10
		0,	// TexFmt_A2B10G10R10

		0,	// TexFmt_A16B16G16R16

		0,	// TexFmt_R5G6B5
		0,	// TexFmt_R6G5B5
		0,	// TexFmt_B5G6R5

		0,	// TexFmt_A1R5G5B5
		0,	// TexFmt_R5G5B5A1
		0,	// TexFmt_A1B5G5R5

		0,	// TexFmt_A4R4G4B4
		0,	// TexFmt_A4B4G4R4
		0,	// TexFmt_R4G4B4A4

		0,	// TexFmt_ABGR_F16
		0,	// TexFmt_ABGR_F32

		0,	// TexFmt_I8
		0,	// TexFmt_I4

		0,	// TexFmt_DXT1
		0,	// TexFmt_DXT2
		0,	// TexFmt_DXT3
		0,	// TexFmt_DXT4
		0,	// TexFmt_DXT5

		0,	// TexFmt_DXT1
		0,	// TexFmt_DXT3
		0,	// TexFmt_DXT5

		0,	// TexFmt_XB_A8R8G8B8
		0,	// TexFmt_XB_A8B8G8R8
		0,	// TexFmt_XB_B8G8R8A8
		0,	// TexFmt_XB_R8G8B8A8

		0,	// TexFmt_XB_R5G6B5
		0,	// TexFmt_XB_R6G5B5

		0,	// TexFmt_XB_A1R5G5B5
		0,	// TexFmt_XB_R5G5B5A1

		0,	// TexFmt_XB_A4R4G4B4
		0,	// TexFmt_XB_R4G4B4A4

		0,	// TexFmt_PSP_A8B8G8R8s
		0,	// TexFmt_PSP_B5G6R5s
		0,	// TexFmt_PSP_A1B5G5R5s
		0,	// TexFmt_PSP_A4B4G4R4s

		0,	// TexFmt_PSP_I8s
		0,	// TexFmt_PSP_I4s

		0,	// TexFmt_PSP_DXT1s
		0,	// TexFmt_PSP_DXT3s
		0,	// TexFmt_PSP_DXT5s
	},

	{ // PSP
		0,	//				 // TexFmt_A8R8G8B8
		3,	// SCEGU_PF8888, //	TexFmt_A8B8G8R8
		0,	//				 // TexFmt_B8G8R8A8
		0,	//				 // TexFmt_R8G8B8A8

		0,	//				 // TexFmt_A2R10G10B10
		0,	//				 // TexFmt_A2B10G10R10

		0,	//				 // TexFmt_A16B16G16R16

		0,	//				 //	TexFmt_R5G6B5
		0,	//				 // TexFmt_R6G5B5
		0,	// SCEGU_PF5650, // TexFmt_B5G6R5

		0,	//               //	TexFmt_A1R5G5B5
		0,	//				 // TexFmt_R5G5B5A1
		1,	// SCEGU_PF5551, //	TexFmt_A1B5G5R5

		0,	//				 // TexFmt_A4R4G4B4
		2,	// SCEGU_PF4444, //	TexFmt_A4B4G4R4
		0,	//				 // TexFmt_R4G4B4A4

		0,	//				 // TexFmt_ABGR_F16
		0,	//				 // TexFmt_ABGR_F32

		5,	// SCEGU_PFIDX8, //	TexFmt_I8
		4,	// SCEGU_PFIDX4, //	TexFmt_I4

		0,	//				 // TexFmt_DXT1
		0,	//				 // TexFmt_DXT2
		0,	//				 // TexFmt_DXT3
		0,	//				 // TexFmt_DXT4
		0,	//				 // TexFmt_DXT5

		8,	// SCEGU_PFDXT1, //	TexFmt_PSP_DXT1
		9,	// SCEGU_PFDXT3, //	TexFmt_PSP_DXT3
		10,	// SCEGU_PFDXT5, //	TexFmt_PSP_DXT5

		0,	//				 // TexFmt_XB_A8R8G8B8
		0,	//				 // TexFmt_XB_A8B8G8R8
		0,	//				 // TexFmt_XB_B8G8R8A8
		0,	//				 // TexFmt_XB_R8G8B8A8

		0,	//				 // TexFmt_XB_R5G6B5
		0,	//				 // TexFmt_XB_R6G5B5

		0,	//				 // TexFmt_XB_A1R5G5B5
		0,	//				 // TexFmt_XB_R5G5B5A1

		0,	//				 // TexFmt_XB_A4R4G4B4
		0,	//				 // TexFmt_XB_R4G4B4A4

		3,	// SCEGU_PF8888, //	TexFmt_PSP_A8B8G8R8s
		0,	// SCEGU_PF5650, // TexFmt_PSP_B5G6R5s
		1,	// SCEGU_PF5551, //	TexFmt_PSP_A1B5G5R5s
		2,	// SCEGU_PF4444, //	TexFmt_PSP_A4B4G4R4s

		5,	// SCEGU_PFIDX8, //	TexFmt_PSP_I8s
		4,	// SCEGU_PFIDX4, //	TexFmt_PSP_I4s

		8,	// SCEGU_PFDXT1, //	TexFmt_PSP_DXT1s
		9,	// SCEGU_PFDXT3, //	TexFmt_PSP_DXT3s
		10,	// SCEGU_PFDXT5, //	TexFmt_PSP_DXT5s
	}
};

const char * const MFTexture_GetFormatString(int format)
{
	return gpMFTextureFormatStrings[format];
}

uint32 MFTexture_GetPlatformAvailability(int format)
{
	return gMFTexturePlatformAvailability[format];
}

bool MFTexture_IsAvailableOnPlatform(int format, int platform)
{
	return (gMFTexturePlatformAvailability[format] & MFBIT(platform)) != 0;
}

int MFTexture_GetBitsPerPixel(int format)
{
	return gMFTextureBitsPerPixel[format];
}
