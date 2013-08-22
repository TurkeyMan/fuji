/**
 * @file MFTexture.h
 * @brief Declares structures and functions for managing textures.
 * @author Manu Evans
 * @defgroup MFTexture Texture Management
 * @{
 */

#pragma once
#if !defined(_MFTEXTURE_H)
#define _MFTEXTURE_H

#include "MFImage.h"
#include "MFVector.h"

/**
 * @struct MFTexture
 * Represents a Fuji Texture.
 */
struct MFTexture;

/**
 * Texture flags.
 * Flags to control the way textures are created.
 */
enum TextureFlags
{
	// Internal Flags
	TEX_AlphaMask = 0x3,				/**< Alpha mask. 0 = Opaque, 1 = Full Alpha, 3 = 1bit Alpha */
	TEX_PreMultipliedAlpha = MFBIT(2),	/**< Pre-multiplied alpha */
	TEX_Swizzled = MFBIT(3),			/**< Texture data is swizzled for the platform */
	TEX_RenderTarget = MFBIT(4),		/**< Texture is a render target */

	// User Flags
	TEX_CopyMemory = MFBIT(8)			/**< Takes a copy of the image buffer when calling MFTexture_CreateFromRawData() */
};

// interface functions

/**
 * Create a texture.
 * Creates a texture from the filesystem.
 * @param pName Name of texture to read from the filesystem.
 * @param generateMipChain If true, a mip-chain will be generated for the texture.
 * @return Pointer to an MFTexture structure representing the newly created texture.
 * @remarks If the specified texture has already been created, MFTexture_Create will return a new reference to the already created texture.
 * @see MFTexture_CreateDynamic(), MFTexture_CreateFromRawData(), MFTexture_CreateRenderTarget(), MFTexture_Release()
 */
MF_API MFTexture* MFTexture_Create(const char *pName, bool generateMipChain = true);

/**
 * Create a dynamic texture.
 * Creates a dynamic texture.
 * @param pName Name of the texture being created.
 * @param width Image width.
 * @param height Image height.
 * @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
 * @param flags Texture creation flags.
 * @return Pointer to an MFTexture structure representing the newly created texture.
 * @remarks If the specified texture has already been created, MFTexture_CreateDynamic will fail.
 * @see MFTexture_Create(), MFTexture_CreateFromRawData(), MFTexture_CreateRenderTarget(), MFTexture_Release()
 */
MF_API MFTexture* MFTexture_CreateDynamic(const char *pName, int width, int height, MFImageFormat format, uint32 flags = 0);

/**
 * Create a texture from raw data.
 * Creates a texture from a raw data buffer.
 * @param pName Name of the texture being created.
 * @param pData Pointer to a buffer containing the image data
 * @param width Image width.
 * @param height Image height.
 * @param format Format of the image data being read. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
 * @param flags Texture creation flags.
 * @param generateMipChain If true, a mip-chain will be generated for the texture.
 * @param pPalette Pointer to palette data. Use NULL for non-paletted image formats.
 * @return Pointer to an MFTexture structure representing the newly created texture.
 * @remarks If ImgFmt_A8R8G8B8 is used, and it is not supported by the platform natively, a copy of the image is taken and the data is swizzled to the best available 32bit format on the target platform. Use MFTexture_GetPlatformAvailability() or MFImage_IsAvailableOnPlatform() to determine what formats are supported on a particular platform.
 * @see MFTexture_Create(), MFTexture_Release(), MFTexture_GetPlatformAvailability(), MFImage_IsAvailableOnPlatform()
 */
MF_API MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFImageFormat format, uint32 flags = 0, bool generateMipChain = true, uint32 *pPalette = 0);

/**
 * Create a scaled texture from raw data.
 * Creates a texture from a raw data buffer that us scaled using a given algorithm.
 * @param pName Name of the texture being created.
 * @param pData Pointer to a buffer containing the image data
 * @param sourceWidth Source image width.
 * @param sourceHeight Source image height.
 * @param texWidth Texture width.
 * @param texHeight Texture height.
 * @param format Format of the image data being read. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
 * @param algorithm Scaling algorithm to be used.
 * @param flags Texture creation flags.
 * @param pPalette Pointer to palette data. Use NULL for non-paletted image formats.
 * @return Pointer to an MFTexture structure representing the newly created texture.
 * @remarks If ImgFmt_A8R8G8B8 is used, and it is not supported by the platform natively, a copy of the image is taken and the data is swizzled to the best available 32bit format on the target platform. Use MFTexture_GetPlatformAvailability() or MFImage_IsAvailableOnPlatform() to determine what formats are supported on a particular platform.
 * @see MFTexture_CreateFromRawData(), MFTexture_Create(), MFTexture_Release(), MFTexture_GetPlatformAvailability(), MFImage_IsAvailableOnPlatform()
 */
MF_API MFTexture* MFTexture_ScaleFromRawData(const char *pName, void *pData, int sourceWidth, int sourceHeight, int texWidth, int texHeight, MFImageFormat format, MFScalingAlgorithm algorithm, uint32 flags = 0, uint32 *pPalette = 0);

/**
 * Creates a render target texture.
 * Creates a render target texture.
 * @param pName Name of the texture being created.
 * @param width Width of render target.
 * @param height Height of render target.
 * @return Pointer to an MFTexture structure representing the newly created render target texture.
 * @see MFTexture_Create(), MFTexture_Release()
 */
MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFImageFormat targetFormat = ImgFmt_SelectNicest);

/**
 * Release an MFTexture instance.
 * Release a reference to an MFTexture and destroy when the reference count reaches 0.
 * @param pTexture Texture instance to be destroyed.
 * @return Returns the new reference count of the texture. If the returned reference count is 0, the texture is destroyed.
 * @see MFTexture_Create()
 */
MF_API int MFTexture_Release(MFTexture *pTexture);

/**
 * Find an existing texture.
 * Finds an existing instance of the specified texture and returns a pointer. If the texture is not found, NULL is returned.
 * @param pName Name of texture to find.
 * @return Returns a pointer to the texture if it is found, otherwise NULL is returned.
 * @remarks Note that MFTexture_Find increments the textures reference count so you must release the reference when finished.
 * @see MFTexture_Create()
 */
MF_API MFTexture* MFTexture_Find(const char *pName);

/**
 * Create a blank plain coloured texture.
 * Create a new texture that is a solid colour.
 * @param pName Name for the texture being created.
 * @param colour Colour to fill the texture when it is created.
 * @return Returns a pointer to a newly created blank texture.
 * @see MFTexture_Create()
 */
MF_API MFTexture* MFTexture_CreateBlank(const char *pName, const MFVector &colour);

/**
 * Tests to see if a image format is available on the current platform.
 * Tests if a image format is supported in hardware on the current platform.
 * @param format Image format to be tested.
 * @return Returns true if specified format is supported in hardware.
 * @see MFImage_GetPlatformAvailability()
 */
MF_API bool MFTexture_IsFormatAvailable(int format);

MF_API void MFTexture_GetTextureDimensions(const MFTexture *pTexture, int *pWidth, int *pHeight);


// C++ API
#include "Fuji/MFResource.h"

namespace Fuji
{
	class Texture : public Resource
	{
	public:
		struct Dimensions
		{
			int width, height;
		};

		inline Texture()											{}
		inline Texture(const Texture &from)							: Resource(from) {}
		inline Texture(MFTexture *pFrom)							: Resource((MFResource*)pFrom) {}
		inline Texture(const char *pName)							{ pResource = (MFResource*)MFTexture_Create(pName); }

		inline Texture& operator=(const Texture& from)
		{
			Release();
			pResource = from.pResource;
			AddRef();
			return *this;
		}

		inline operator MFTexture*()								{ return (MFTexture*)pResource; }

		inline Texture& Create(const char *pName)
		{
			Release();
			pResource = (MFResource*)MFTexture_Create(pName);
			return *this;
		}

		inline Dimensions Size() const								{ Dimensions dim; MFTexture_GetTextureDimensions((MFTexture*)pResource, &dim.width, &dim.height); return dim; }
	};
}

#endif // _MFTEXTURE_H

/** @} */
