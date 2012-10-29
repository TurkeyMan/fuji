module fuji.image;

public import fuji.fuji;

/**
* Texture format.
* Describes a texture format.
*/
enum MFImageFormat
{
	Unknown = -1, /**< Unknown texture format */

	A8R8G8B8,	/**< 32bit BGRA format */
	A8B8G8R8,	/**< 32bit RGBA format */
	B8G8R8A8,	/**< 32bit ARGB format */
	R8G8B8A8,	/**< 32bit ABGR format */

	R8G8B8,	/**< 24bit BGR format */
	B8G8R8,	/**< 24bit RGB format */

	A2R10G10B10,	/**< 32bit BGRA format with 10 bits per colour channel */
	A2B10G10R10,	/**< 32bit RGBA format with 10 bits per colour channel */

	A16B16G16R16, /**< 64bit RGBA format with 16 bits per colour channel */

	R5G6B5,		/**< 16bit BGR format with no alpha */
	R6G5B5,		/**< 16bit BGR format with no alpha and 6 bits for red */
	B5G6R5,		/**< 16bit RGB format with no alpha */

	A1R5G5B5,	/**< 16bit BGRA format with 1 bit alpha */
	R5G5B5A1,	/**< 16bit ABGR format with 1 bit alpha */
	A1B5G5R5,	/**< 16bit RGBA format with 1 bit alpha */

	A4R4G4B4,	/**< 16bit BGRA format with 4 bits per colour channel */
	A4B4G4R4,	/**< 16bit RGBA format with 4 bits per colour channel */
	R4G4B4A4,	/**< 16bit ABGR format with 4 bits per colour channel */

	ABGR_F16,	/**< 64bit RGBA floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	ABGR_F32,	/**< 128bit RGBA floating point format */

	I8,			/**< 8bit paletted format */
	I4,			/**< 4bit paletted format */

	DXT1,		/**< Compressed DXT1 texture */
	DXT2,		/**< Compressed DXT2 texture */
	DXT3,		/**< Compressed DXT3 texture */
	DXT4,		/**< Compressed DXT4 texture */
	DXT5,		/**< Compressed DXT5 texture */

	PSP_DXT1,	/**< Special DXT1 for PSP */
	PSP_DXT3,	/**< Special DXT3 for PSP */
	PSP_DXT5,	/**< Special DXT5 for PSP */

	// platform specific swizzled formats
	XB_A8R8G8B8s,	/**< 32bit BGRA format, swizzled for XBox */
	XB_A8B8G8R8s,	/**< 32bit RGBA format, swizzled for XBox */
	XB_B8G8R8A8s,	/**< 32bit ARGB format, swizzled for XBox */
	XB_R8G8B8A8s,	/**< 32bit ABGR format, swizzled for XBox */

	XB_R5G6B5s,		/**< 16bit BGR format, swizzled for XBox */
	XB_R6G5B5s,		/**< 16bit BGR format, swizzled for XBox */

	XB_A1R5G5B5s,	/**< 16bit BGRA format, swizzled for XBox */
	XB_R5G5B5A1s,	/**< 16bit ABGR format, swizzled for XBox */

	XB_A4R4G4B4s,	/**< 16bit BGRA format, swizzled for XBox */
	XB_R4G4B4A4s,	/**< 16bit ABGR format, swizzled for XBox */

	PSP_A8B8G8R8s,	/**< 32bit RGBA format, swizzled for PSP */
	PSP_B5G6R5s,		/**< 16bit RGB format, swizzled for PSP */
	PSP_A1B5G5R5s,	/**< 16bit RGBA format, swizzled for PSP */
	PSP_A4B4G4R4s,	/**< 16bit RGBA format, swizzled for PSP */

	PSP_I8s,		/**< 8bit paletted format, swizzled for PSP */
	PSP_I4s,		/**< 4bit paletted format, swizzled for PSP */

	PSP_DXT1s,		/**< DXT1, swizzled for PSP */
	PSP_DXT3s,		/**< DXT3, swizzled for PSP */
	PSP_DXT5s,		/**< DXT5, swizzled for PSP */

	Max,			/**< Max texture format */

	SelectNicest = 0x1000,			/**< Select the nicest format. */
	SelectNicest_NoAlpha = 0x1001,	/**< Select the nicest format with no alpha channel. */
	SelectFastest = 0x1002,			/**< Select the fastest format. */
	SelectFastest_Masked = 0x1003,	/**< Select the fastest format requiring only a single bit of alpha. */
	SelectFastest_NoAlpha = 0x1004	/**< Select the fastest format with no alpha channel. */
}

/**
* Scaling algorithm.
* Supported scaling algorithms.
*/
enum MFScalingAlgorithm
{
	Unknown = -1,
	None = 0,	// no scaling
	Nearest,	// nearest filtering: any size
	Bilinear,	// bilinear filtering: any size
	Box,		// box filtering: 1/2x (common for mip generation)
	HQX,		// 'High Quality nX' algorithm: 2x, 3x, 4x
	AdvMAME,	// 'Advance MAME' algorithm: 2x, 3x, 4x
	Eagle,		// 'Eagle' algorithm: 2x
	SuperEagle,	// 'Super Eagle' algorithm: 2x
	_2xSaI,		// '2x Scale and Interpolate' algorithm: 2x
	Super2xSaI	// 'Super 2x Scale and Interpolate' algorithm: 2x
}

/**
* Image scaling data.
* Image scaling data.
*/
struct MFScaleImage
{
	void *pSourceImage;
	int sourceWidth;
	int sourceHeight;
	int sourceStride;

	void *pTargetBuffer;
	int targetWidth;
	int targetHeight;
	int targetStride;

	MFImageFormat sourceFormat = MFImageFormat.Unknown;
	MFScalingAlgorithm algorithm = MFScalingAlgorithm.Unknown;
}


extern (C) void MFImage_Scale(MFScaleImage *pScaleData);

/**
* Get a string representing the texture format.
* Gets a human readable string representing the texture format.
* @param format Texture format to get the name of.
* @return Pointer to a string representing the texture format.
* @see MFTexture_GetPlatformAvailability(), MFImage_GetBitsPerPixel()
*/
extern (C) const(char)* MFImage_GetFormatString(int format);

/**
* Gets all platforms that support the specified texture format in hardware.
* Gets a variable representing which platforms support the specified texture format in hardware.
* @param format Format to test for hardware support.
* @return Result is a bitfield where each bit represents hardware support for a specific platform. Platform support can be tested, for example, using: ( result & MFBit(FP_PC) ) != 0.
* @see MFImage_GetFormatString(), MFImage_GetBitsPerPixel()
*/
extern (C) uint MFImage_GetPlatformAvailability(int format);

/**
* Tests to see if a texture format is available on a specified platform.
* Tests if a texture format is supported in hardware on a specified platform.
* @param format Texture format to be tested.
* @param platform Platform to test for hardware support.
* @return Returns true if specified format is supported in hardware.
* @see MFTexture_GetPlatformAvailability()
*/
extern (C) bool MFImage_IsAvailableOnPlatform(int format, int platform);

/**
* Get the average number of bits per pixel for a specified format.
* Get the average number of bits per pixel for the specified format.
* @param format Name for the texture being created.
* @return Returns the number of bits per pixel for the specified format. If a compressed format is specified, the average number of bits per pixel is returned.
* @see MFTexture_GetPlatformAvailability(), MFImage_GetFormatString()
*/
extern (C) int MFImage_GetBitsPerPixel(int format);
