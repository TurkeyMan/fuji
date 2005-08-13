#include "Common.h"
#include "MFTexture_Internal.h"

const char * const gpMFTextureFormatStrings[TexFmt_Max] =
{
	"A8R8G8B8",
	"A8B8G8R8",
	"A2R10G10B10",
	"A2B10G10R10",
	"A16B16G16R16",
	"R5G6B5",
	"A1R5G5B5",
	"A4R4G4B4",
	"A4B4G4R4",
	"ABGR_F16",
	"ABGR_F32",
	"I8",
	"I4",
	"DXT1",
	"DXT2",
	"DXT3",
	"DXT4",
	"DXT5"
};

uint32 gMFTexturePlatformAvailability[TexFmt_Max] =
{
	BIT(FP_PC)|BIT(FP_XBox),	// TexFmt_A8R8G8B8
	BIT(FP_PSP),				// TexFmt_A8B8G8R8

	BIT(FP_PC),					// TexFmt_A2R10G10B10
	BIT(FP_PC),					// TexFmt_A2B10G10R10
	BIT(FP_PC),					// TexFmt_A16B16G16R16

	BIT(FP_PC),					// TexFmt_R5G6B5
	BIT(FP_PC),					// TexFmt_A1R5G5B5

	BIT(FP_PC),					// TexFmt_A4R4G4B4
	BIT(FP_PSP),				// TexFmt_A4B4G4R4

	BIT(FP_PC),					// TexFmt_ABGR_F16
	BIT(FP_PC),					// TexFmt_ABGR_F32

	BIT(FP_PSP),				// TexFmt_I8
	BIT(FP_PSP),				// TexFmt_I4

	BIT(FP_PC)|BIT(FP_XBox)|BIT(FP_PSP),	// TexFmt_DXT1
	BIT(FP_PC)|BIT(FP_XBox),				// TexFmt_DXT2
	BIT(FP_PC)|BIT(FP_XBox)|BIT(FP_PSP),	// TexFmt_DXT3
	BIT(FP_PC)|BIT(FP_XBox),				// TexFmt_DXT4
	BIT(FP_PC)|BIT(FP_XBox)|BIT(FP_PSP)		// TexFmt_DXT5
};

uint32 gMFTextureBitsPerPixel[TexFmt_Max] =
{
	32,	// TexFmt_A8R8G8B8
	32,	// TexFmt_A8B8G8R8

	32,	// TexFmt_A2R10G10B10
	32,	// TexFmt_A2B10G10R10
	64,	// TexFmt_A16B16G16R16

	16,	// TexFmt_R5G6B5
	16,	// TexFmt_A1R5G5B5

	16,	// TexFmt_A4R4G4B4
	16,	// TexFmt_A4B4G4R4

	64,	// TexFmt_ABGR_F16
	128,// TexFmt_ABGR_F32

	8,	// TexFmt_I8
	4,	// TexFmt_I4

	4,	// TexFmt_DXT1
	8,	// TexFmt_DXT2
	8,	// TexFmt_DXT3
	8,	// TexFmt_DXT4
	8	// TexFmt_DXT5
};

uint32 gMFTexturePlatformFormat[FP_Max][TexFmt_Max] =
{
	{ // PC
		21,	// D3DFMT_A8R8G8B8		// TexFmt_A8R8G8B8
		32,	// D3DFMT_A8B8G8R8		// TexFmt_A8B8G8R8
		35,	// D3DFMT_A2R10G10B10	// TexFmt_A2R10G10B10
		31,	// D3DFMT_A2B10G10R10	// TexFmt_A2B10G10R10
		36,	// D3DFMT_A16B16G16R16	// TexFmt_A16B16G16R16
		23,	// D3DFMT_R5G6B5		// TexFmt_R5G6B5
		25,	// D3DFMT_A1R5G5B5		// TexFmt_A1R5G5B5
		26,	// D3DFMT_A4R4G4B4		// TexFmt_A4R4G4B4
		0,	//						// TexFmt_A4B4G4R4
		113,// D3DFMT_A16B16G16R16F	// TexFmt_ABGR_F16
		116,// D3DFMT_A32B32G32R32F	// TexFmt_ABGR_F32
		41,	// D3DFMT_P8			// TexFmt_I8
		0,	//						// TexFmt_I4
		MAKEFOURCC('D', 'X', 'T', '1'),	// D3DFMT_DXT1	// TexFmt_DXT1
		MAKEFOURCC('D', 'X', 'T', '2'),	// D3DFMT_DXT2	// TexFmt_DXT2
		MAKEFOURCC('D', 'X', 'T', '3'),	// D3DFMT_DXT3	// TexFmt_DXT3
		MAKEFOURCC('D', 'X', 'T', '4'),	// D3DFMT_DXT4	// TexFmt_DXT4
		MAKEFOURCC('D', 'X', 'T', '5')	// D3DFMT_DXT5	// TexFmt_DXT5
	},

	{ // XBox
		0, // TexFmt_A8R8G8B8
		0, // TexFmt_A8B8G8R8
		0, // TexFmt_A2R10G10B10
		0, // TexFmt_A2B10G10R10
		0, // TexFmt_A16B16G16R16
		0, // TexFmt_R5G6B5
		0, // TexFmt_A1R5G5B5
		0, // TexFmt_A4R4G4B4
		0, // TexFmt_A4B4G4R4
		0, // TexFmt_ABGR_F16
		0, // TexFmt_ABGR_F32
		0, // TexFmt_I8
		0, // TexFmt_I4
		0, // TexFmt_DXT1
		0, // TexFmt_DXT2
		0, // TexFmt_DXT3
		0, // TexFmt_DXT4
		0  // TexFmt_DXT5
	},

	{ // Linux
		0, // TexFmt_A8R8G8B8
		0, // TexFmt_A8B8G8R8
		0, // TexFmt_A2R10G10B10
		0, // TexFmt_A2B10G10R10
		0, // TexFmt_A16B16G16R16
		0, // TexFmt_R5G6B5
		0, // TexFmt_A1R5G5B5
		0, // TexFmt_A4R4G4B4
		0, // TexFmt_A4B4G4R4
		0, // TexFmt_ABGR_F16
		0, // TexFmt_ABGR_F32
		0, // TexFmt_I8
		0, // TexFmt_I4
		0, // TexFmt_DXT1
		0, // TexFmt_DXT2
		0, // TexFmt_DXT3
		0, // TexFmt_DXT4
		0  // TexFmt_DXT5
	},

	{ // PSP
		0,	//					TexFmt_A8R8G8B8
		3,	// SCEGU_PF8888, //	TexFmt_A8B8G8R8
		0,	//					TexFmt_A2R10G10B10
		0,	//					TexFmt_A2B10G10R10
		0,	//					TexFmt_A16B16G16R16
		0,	// SCEGU_PF5650, //	TexFmt_R5G6B5
		1,	// SCEGU_PF5551, //	TexFmt_A1R5G5B5
		0,	//					TexFmt_A4R4G4B4
		2,	// SCEGU_PF4444, //	TexFmt_A4B4G4R4
		0,	//					TexFmt_ABGR_F16
		0,	//					TexFmt_ABGR_F32
		5,	// SCEGU_PFIDX8, //	TexFmt_I8
		4,	// SCEGU_PFIDX4, //	TexFmt_I4
		8,	// SCEGU_PFDXT1, //	TexFmt_DXT1
		0,	//					TexFmt_DXT2
		9,	// SCEGU_PFDXT3, //	TexFmt_DXT3
		0,	//					TexFmt_DXT4
		10	// SCEGU_PFDXT5, //	TexFmt_DXT5
	}
};

