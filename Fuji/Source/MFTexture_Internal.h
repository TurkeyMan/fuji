#pragma once
#if !defined(_MFTEXTURE_INTERNAL_H)
#define _MFTEXTURE_INTERNAL_H

#if MF_RENDERER == MF_DRIVER_D3D9
	#include <d3d9.h>
#elif MF_RENDERER == MF_DRIVER_OPENGL
	#include "../Source/Drivers/OpenGL/MFOpenGL.h"
#endif

#include "MFTexture.h"
#include "MFResource.h"

/**
 * Texture flags.
 * Flags to control the way textures are created.
 */
enum TextureFlags
{
	// Internal Flags
	TEX_Opaque = 0,						/**< Opaque texture */
	TEX_FullAlpha = 1,					/**< Texture contains full alpha */
	TEX_1BitAlpha = 2,					/**< Texture contains 1-bit transparency */
	TEX_AlphaMask = 0x3,				/**< Alpha mask. 0 = Opaque, 1 = Full Alpha, 3 = 1bit Alpha */

	TEX_PreMultipliedAlpha = MFBIT(2),	/**< Pre-multiplied alpha */
	TEX_Swizzled = MFBIT(3),			/**< Texture data is swizzled for the platform */
	TEX_RenderTarget = MFBIT(4),		/**< Texture is a render target */
	TEX_FreeImageData = MFBIT(5),		/**< If the texture should free the image data on destruction */
};

struct MFTextureSurface
{
	int width, height, depth;
	int xBlocks, yBlocks;

	uint32 imageDataOffset;
	uint32 bufferLength;
	uint32 lineStride;

	uint32 paletteEntriesOffset;
	uint16 paletteBufferLength;

	uint8 bitsPerPixel;
	uint8 bitsPerBlock;

	uint64 platformData;
};

struct MFTexture : public MFResource
{
	MFTextureType type;
	MFImageFormat imageFormat;
	int width, height, depth;
	int numElements;
	int numMips;
	uint32 createFlags;
	uint32 flags;

	uint32 reserved;

	union
	{
		MFTextureSurface *pSurfaces;
		uint64 surfacesOffset;
	};
	union
	{
		char *pImageData;
		uint64 imageDataOffset;
	};

	union
	{
		uint64 platformData;

		// fields used by platforms
#if MF_RENDERER == MF_DRIVER_XBOX
#if defined(XB_XGTEXTURES)
		IDirect3DTexture8 texture;
#endif
		IDirect3DTexture8 *pTexture;
#elif MF_RENDERER == MF_DRIVER_PS2
		unsigned int vramAddr;
#else
		void *pInternalData;
#endif
	};
};


// functions
MFInitStatus MFTexture_InitModule(int moduleId, bool bPerformInitialisation);
void MFTexture_DeinitModule();

void MFTexture_InitModulePlatformSpecific();
void MFTexture_DeinitModulePlatformSpecific();

void MFTexture_CreatePlatformSpecific(MFTexture *pTexture);
void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture);

MFTexture* MFTexture_InitTexture(const MFTextureDesc *pDesc, MFRendererDrivers renderer, size_t *pExtraBytes);

#if !defined(_FUJI_UTIL)
// a debug menu texture information display object
#include "DebugMenu_Internal.h"

class TextureBrowser : public MenuObject
{
public:
	TextureBrowser();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, const MFVector &pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual MFVector GetDimensions(float maxWidth);

	int selection;
};
#endif

#endif
