#if !defined(_MFTEXTURE_H)
#define _MFTEXTURE_H

#include "Vector4.h"

struct MFTexture;

// some enum's
enum MFTextureFormats
{
	TexFmt_Unknown = -1,

	TexFmt_A8R8G8B8,	// PC/XBox
	TexFmt_A8B8G8R8,	// PSP

	TexFmt_A2R10G10B10,	// DX9
	TexFmt_A2B10G10R10,	// DX9
	TexFmt_A16B16G16R16,// DX9

	TexFmt_R5G6B5,		// PC
	TexFmt_A1R5G5B5,	// PC

	TexFmt_A4R4G4B4,	// PC
	TexFmt_A4B4G4R4,	// PSP

	TexFmt_ABGR_F16,	// DX9  s1-e5-m10 - seeeeemmmmmmmmmm
	TexFmt_ABGR_F32,	// DX9

	TexFmt_I8,			// PSP
	TexFmt_I4,			// PSP

	TexFmt_DXT1,		// PSP/PC/XBox
	TexFmt_DXT2,
	TexFmt_DXT3,		// PSP/PC/XBox
	TexFmt_DXT4,
	TexFmt_DXT5,		// PSP/PC/XBox

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
MFTexture* MFTexture_CreateBlank(const char *pName, const Vector4 &colour);

#endif // _TEXTURE_H
