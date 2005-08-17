#if !defined(_MFTEXTURE_H)
#define _MFTEXTURE_H

#include "MFVector.h"

struct MFTexture;

// some enum's
enum MFTextureFormats
{
	TexFmt_Unknown = -1,

	TexFmt_A8R8G8B8,	// XBox/PC
	TexFmt_A8B8G8R8,	// XBox/PSP/DX9
	TexFmt_B8G8R8A8,	// XBox
	TexFmt_R8G8B8A8,	// XBox

	TexFmt_A2R10G10B10,	// DX9
	TexFmt_A2B10G10R10,	// DX9

	TexFmt_A16B16G16R16,// DX9

	TexFmt_R5G6B5,		// PC/XBox
	TexFmt_R6G5B5,		// XBox
	TexFmt_B5G6R5,		// PSP

	TexFmt_A1R5G5B5,	// PC/XBox
	TexFmt_R5G5B5A1,	// XBox
	TexFmt_A1B5G5R5,	// PSP

	TexFmt_A4R4G4B4,	// PC/XBox
	TexFmt_A4B4G4R4,	// PSP
	TexFmt_R4G4B4A4,	// XBox

	TexFmt_ABGR_F16,	// DX9  s1-e5-m10 - seeeeemmmmmmmmmm
	TexFmt_ABGR_F32,	// DX9

	TexFmt_I8,			// PSP
	TexFmt_I4,			// PSP

	TexFmt_DXT1,		// PC/XBox
	TexFmt_DXT2,		// PC/XBox
	TexFmt_DXT3,		// PC/XBox
	TexFmt_DXT4,		// PC/XBox
	TexFmt_DXT5,		// PC/XBox

	TexFmt_PSP_DXT1,	// PSP
	TexFmt_PSP_DXT3,	// PSP
	TexFmt_PSP_DXT5,	// PSP

	// platform specific swizzled formats
	TexFmt_XB_A8R8G8B8s,	// XBox
	TexFmt_XB_A8B8G8R8s,	// XBox
	TexFmt_XB_B8G8R8A8s,	// XBox
	TexFmt_XB_R8G8B8A8s,	// XBox

	TexFmt_XB_R5G6B5s,		// XBox
	TexFmt_XB_R6G5B5s,		// XBox

	TexFmt_XB_A1R5G5B5s,	// XBox
	TexFmt_XB_R5G5B5A1s,	// XBox

	TexFmt_XB_A4R4G4B4s,	// XBox
	TexFmt_XB_R4G4B4A4s,	// XBox

	TexFmt_PSP_A8B8G8R8s,	// PSP
	TexFmt_PSP_B5G6R5s,		// PSP
	TexFmt_PSP_A1B5G5R5s,	// PSP
	TexFmt_PSP_A4B4G4R4s,	// PSP

	TexFmt_PSP_I8s,			// PSP
	TexFmt_PSP_I4s,			// PSP

	TexFmt_PSP_DXT1s,		// PSP
	TexFmt_PSP_DXT3s,		// PSP
	TexFmt_PSP_DXT5s,		// PSP

	TexFmt_Max
};

enum CreateTextureFlags
{
	TEX_VerticalMirror = 1
};


// interface functions
MFTexture* MFTexture_Create(const char *pName, bool generateMipChain = true);
MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormats format, uint32 flags = 0, bool generateMipChain = true, uint32 *pPalette = 0);
MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height);

int MFTexture_Destroy(MFTexture *pTexture);	// returns new reference count..

// platform independant functions
MFTexture* MFTexture_FindTexture(const char *pName);
MFTexture* MFTexture_CreateBlank(const char *pName, const MFVector &colour);

#endif // _TEXTURE_H
