module fuji.c.MFTexture;

public import fuji.c.MFImage;
import fuji.c.Fuji;
import fuji.c.MFResource;
import fuji.c.MFVector;

import fuji.dbg;

nothrow:
@nogc:

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
	Opaque = 0,						/**< Opaque texture */
	FullAlpha = 1,					/**< Texture contains full alpha */
	_1BitAlpha = 3,					/**< Texture contains 1-bit transparency */
	AlphaMask = 0x3,				/**< Alpha mask. 0 = Opaque, 1 = Full Alpha, 3 = 1bit Alpha */
	PreMultipliedAlpha = MFBit!2,	/**< Pre-multiplied alpha */
	Swizzled = MFBit!3,				/**< Texture data is swizzled for the platform */
	RenderTarget = MFBit!4,			/**< Texture is a render target */
}

enum MFTextureType
{
	Unknown = -1,

	_1D = 0,
	_2D,
	_3D,
	Cubemap
}

enum MFTextureCreateFlags
{
	Static = 0,
	Dynamic = 1,
	Scratch = 2,
	RenderTarget = 3,
	TypeMask = 0x3,

	GenerateMips = MFBit!2,
	PremultipliedAlpha = MFBit!3
}

struct MFTextureDesc
{
	MFTextureType type = MFTextureType.Unknown;
	MFImageFormat format = MFImageFormat.Unknown;
	int width;
	int height;
	int depth;
	int arrayElements;
	int mipLevels;
	uint32 flags;
}

struct MFLockedTexture
{
	void *pData;
	size_t width, height, depth;
	size_t strideInBytes, sliceInBytes;
}


// interface functions

/**
* Create a texture.
* Creates a texture.
* @param pName Name of the texture being created.
* @param pDesc Description of the texture to create.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create will fail.
* @see MFTexture_Create2D(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
extern (C) MFTexture* MFTexture_Create(const(char)* pName, const(MFTextureDesc)* pDesc);

/**
* Create a 1D texture.
* Creates a 1D texture.
* @param pName Name of the texture being created.
* @param width Image width.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create2D will fail.
* @see MFTexture_Create(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_Create1D(const(char)* pName, int width, MFImageFormat format, uint flags = 0)
{
	MFTextureDesc desc = MFTextureDesc(MFTextureType._1D, format, width, 0, 0, 0, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a 2D texture.
* Creates a 2D texture.
* @param pName Name of the texture being created.
* @param width Image width.
* @param height Image height.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create2D will fail.
* @see MFTexture_Create(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_Create2D(const(char)* pName, int width, int height, MFImageFormat format, uint flags = 0)
{
	MFTextureDesc desc = MFTextureDesc(MFTextureType._2D, format, width, height, 0, 0, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a 3D texture.
* Creates a 3D texture.
* @param pName Name of the texture being created.
* @param width Image width.
* @param height Image height.
* @param depth Image depth.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create3D will fail.
* @see MFTexture_Create(), MFTexture_Create2D(), MFTexture_CreateCubemap(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_Create3D(const(char)* pName, int width, int height, int depth, MFImageFormat format, uint flags = 0)
{
	MFTextureDesc desc = MFTextureDesc(MFTextureType._3D, format, width, height, depth, 0, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a cubemap.
* Creates a cubemap.
* @param pName Name of the texture being created.
* @param width Image width.
* @param height Image height.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_CreateCubemap will fail.
* @see MFTexture_Create(), MFTexture_Create2D(), MFTexture_Create3D(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_CreateCubemap(const(char)* pName, int width, int height, MFImageFormat format, uint flags = 0)
{
	MFTextureDesc desc = MFTextureDesc(MFTextureType.Cubemap, format, width, height, 0, 0, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a 1D texture array.
* Creates a 1D texture array.
* @param pName Name of the texture being created.
* @param width Image width.
* @param elements Number of array elements.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create2DArray will fail.
* @see MFTexture_Create(), MFTexture_Create2D(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_Create1DArray(const(char)* pName, int width, int elements, MFImageFormat format, uint flags = 0)
{
	assert(elements > 0, "Array texture must have more than 0 elements!");
	MFTextureDesc desc = MFTextureDesc(MFTextureType._1D, format, width, 0, 0, elements, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a 2D texture array.
* Creates a 2D texture array.
* @param pName Name of the texture being created.
* @param width Image width.
* @param height Image height.
* @param elements Number of array elements.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_Create2DArray will fail.
* @see MFTexture_Create(), MFTexture_Create2D(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create3DArray(), MFTexture_CreateCubemapArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_Create2DArray(const(char)* pName, int width, int height, int elements, MFImageFormat format, uint flags = 0)
{
	assert(elements > 0, "Array texture must have more than 0 elements!");
	MFTextureDesc desc = MFTextureDesc(MFTextureType._2D, format, width, height, 0, elements, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a cubemap texture array.
* Creates a cubemap texture array.
* @param pName Name of the texture being created.
* @param width Image width.
* @param height Image height.
* @param elements Number of array elements.
* @param format Format of the image data. Only formats supported by the platform and ImgFmt_A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture representing the newly created texture, or NULL if creation fails.
* @remarks If the named texture has already been created, MFTexture_CreateCubemapArray will fail.
* @see MFTexture_Create(), MFTexture_Create2D(), MFTexture_Create3D(), MFTexture_CreateCubemap(), MFTexture_Create2DArray(), MFTexture_Create3DArray(), MFTexture_CreateFromFile(), MFTexture_CreateFromRawData(), MFTexture_Release()
*/
MFTexture* MFTexture_CreateCubemapArray(const(char)* pName, int width, int height, int elements, MFImageFormat format, uint flags = 0)
{
	assert(elements > 0, "Array texture must have more than 0 elements!");
	MFTextureDesc desc = MFTextureDesc(MFTextureType.Cubemap, format, width, height, 0, elements, ((flags & MFTextureCreateFlags.GenerateMips) ? 0 : 1), flags);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a texture.
* Creates a texture from the filesystem.
* @param pName Name of texture to read from the filesystem.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @return Pointer to an MFTexture structure representing the newly created texture.
* @remarks If the specified texture has already been created, MFTexture_Create will return a new reference to the already created texture.
* @see MFTexture_CreateDynamic(), MFTexture_CreateFromRawData(), MFTexture_CreateRenderTarget(), MFTexture_Release()
*/
extern (C) MFTexture* MFTexture_CreateFromFile(const(char)* pName, uint flags = MFTextureCreateFlags.GenerateMips);

/**
* Create a texture from raw data.
* Creates a texture from a raw data buffer.
* @param pName Name of the texture being created.
* @param pData Pointer to a buffer containing the image data
* @param width Image width.
* @param height Image height.
* @param format Format of the image data being read. Only formats supported by the platform and MFImageFormat.A8R8G8B8 can be used.
* @param flags Combination of creation flags from the MFTextureCreateFlags enumerated type.
* @param pPalette Pointer to palette data. Use NULL for non-paletted image formats.
* @return Pointer to an MFTexture structure representing the newly created texture, or NULL if creation fails.
* @remarks If MFImageFormat.A8R8G8B8 is used, and it is not supported by the platform natively, a copy of the image is taken and the data is swizzled to the best available 32bit format on the target platform. Use MFTexture_GetPlatformAvailability() or MFImage_IsAvailableOnPlatform() to determine what formats are supported on a particular platform.
* @see MFTexture_Create(), MFTexture_Release(), MFTexture_GetPlatformAvailability(), MFImageFormat.IsAvailableOnPlatform()
*/
extern (C) MFTexture* MFTexture_CreateFromRawData(const(char)* pName, const(void)* pData, int width, int height, MFImageFormat format, uint flags = MFTextureCreateFlags.GenerateMips, const(uint)* pPalette = null);

/**
* Creates a render target texture.
* Creates a render target texture.
* @param pName Name for the render target being created.
* @param width Width of render target.
* @param height Height of render target.
* @param targetFormat Render target format.
* @return Pointer to an MFTexture structure representing the newly created render target texture, or NULL if creation fails.
* @see MFTexture_Create(), MFTexture_Release()
*/
MFTexture* MFTexture_CreateRenderTarget(const(char)* pName, int width, int height, MFImageFormat targetFormat = MFImageFormat.SelectRenderTarget)
{
	MFTextureDesc desc = MFTextureDesc(MFTextureType._2D, targetFormat, width, height, 0, 0, 1, MFTextureCreateFlags.RenderTarget);
	return MFTexture_Create(pName, &desc);
}

/**
* Create a blank plain coloured texture.
* Create a new texture that is a solid colour.
* @param pName Name for the texture being created.
* @param colour Colour to fill the texture when it is created.
* @return A pointer to a newly created blank texture.
* @see MFTexture_Create()
*/
extern (C) MFTexture* MFTexture_CreateBlank(const(char)* pName, ref const(MFVector) colour);

/**
* Create a scaled texture from raw data.
* Creates a texture from a raw data buffer that us scaled using a given algorithm.
* @param pName Name of the texture being created.
* @param pData Pointer to a buffer containing the image data
* @param sourceWidth Source image width.
* @param sourceHeight Source image height.
* @param texWidth Texture width.
* @param texHeight Texture height.
* @param format Format of the image data being read. Only formats supported by the platform and MFImageFormat.A8R8G8B8 can be used.
* @param algorithm Scaling algorithm to be used.
* @param flags Texture creation flags.
* @param pPalette Pointer to palette data. Use NULL for non-paletted image formats.
* @return Pointer to an MFTexture structure representing the newly created texture.
* @remarks If MFImageFormat.A8R8G8B8 is used, and it is not supported by the platform natively, a copy of the image is taken and the data is swizzled to the best available 32bit format on the target platform. Use MFTexture_GetPlatformAvailability() or MFImage_IsAvailableOnPlatform() to determine what formats are supported on a particular platform.
* @see MFTexture_CreateFromRawData(), MFTexture_Create(), MFTexture_Release(), MFTexture_GetPlatformAvailability(), MFImageFormat.IsAvailableOnPlatform()
*/
extern (C) MFTexture* MFTexture_ScaleFromRawData(const(char)* pName, void* pData, int sourceWidth, int sourceHeight, int texWidth, int texHeight, MFImageFormat format, MFScalingAlgorithm algorithm, uint flags = 0, uint* pPalette = null);

/**
* Release an MFTexture instance.
* Release a reference to an MFTexture and destroy when the reference reaches 0.
* @param pTexture Texture instance to be destroyed.
* @return The new reference count of the texture. If the returned reference count is 0, the texture is destroyed.
* @see MFTexture_Create()
*/
extern (C) int MFTexture_Release(MFTexture* pTexture);

/**
* Find an existing texture.
* Finds an existing instance of the specified texture and returns a pointer. If the texture is not found, NULL is returned.
* @param pName Name of texture to find.
* @return A pointer to the texture if it is found, otherwise NULL is returned.
* @remarks Note that MFTexture_Find increments the textures reference count so you must release the reference when finished.
* @see MFTexture_Create()
*/
extern (C) MFTexture* MFTexture_Find(const(char)* pName);

extern (C) bool MFTexture_Update(MFTexture* pTexture, int element, int mipLevel, const(void)* pData, size_t lineStride = 0, size_t sliceStride = 0);
extern (C) bool MFTexture_Map(MFTexture* pTexture, int element, int mipLevel, MFLockedTexture* pLock);
extern (C) void MFTexture_Unmap(MFTexture* pTexture, int element, int mipLevel);

/**
* Tests to see if a texture format is available on the current platform.
* Tests if a texture format is supported in hardware on the current platform.
* @param format Texture format to be tested.
* @return True if specified format is supported in hardware.
* @see MFTexture_GetPlatformAvailability()
*/
extern (C) bool MFTexture_IsFormatAvailable(int format);

extern (C) void MFTexture_GetTextureDimensions(const(MFTexture)* pTexture, int* pWidth, int* pHeight, int *pDepth = null) pure;
