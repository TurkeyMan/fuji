module fuji.c.MFImage;

nothrow:

/**
* Texture format.
* Describes a texture format.
*/
enum MFImageFormat
{
	Unknown = -1,		/**< Unknown image format */

	// standard precision formats
	A8R8G8B8,			/**< 32bit BGRA format */
	A8B8G8R8,			/**< 32bit RGBA format */
	B8G8R8A8,			/**< 32bit ARGB format */
	R8G8B8A8,			/**< 32bit ABGR format */

	R8G8B8,				/**< 24bit BGR format */
	B8G8R8,				/**< 24bit RGB format */

	G8R8,				/**< 16bit RG format */

	L8,					/**< 8bit liminance format: RGBA = LLL1 */
	A8,					/**< 8bit alpha only format: RGBA = 111A */
	A8L8,				/**< 16bit liminance+alpha format: RGBA = LLLA */

	// low precision formats
	R5G6B5,				/**< 16bit BGR format with no alpha */
	R6G5B5,				/**< 16bit BGR format with no alpha and 6 bits for red */
	B5G6R5,				/**< 16bit RGB format with no alpha */

	A1R5G5B5,			/**< 16bit BGRA format with 1 bit alpha */
	R5G5B5A1,			/**< 16bit ABGR format with 1 bit alpha */
	A1B5G5R5,			/**< 16bit RGBA format with 1 bit alpha */

	A4R4G4B4,			/**< 16bit BGRA format with 4 bits per colour channel */
	A4B4G4R4,			/**< 16bit RGBA format with 4 bits per colour channel */
	R4G4B4A4,			/**< 16bit ABGR format with 4 bits per colour channel */

	// high precision formats
	A16B16G16R16,		/**< 64bit RGBA format with 16 bits per colour channel */

	G16R16,				/**< 32bit RG format with 16 bits per colour channel */

	L16,				/**< 16bit liminance format: RGBA = LLL1 */
	A16,				/**< 16bit alpha only format: RGBA = 111A */
	A16L16,				/**< 32bit liminance+alpha format with 16 bits per channel: RGBA = LLLA */

	// packed high precision formats
	A2R10G10B10,		/**< 32bit BGRA format with 10 bits per colour channel */
	A2B10G10R10,		/**< 32bit RGBA format with 10 bits per colour channel */

	R10G11B11,			/**< 32bit BGR format with 10 bits for red and 11 bits for green and blue */
	R11G11B10,			/**< 32bit BGR format with 11 bits for red and green and 10 bits for blue */

	// float formats
	ABGR_F16,			/**< 64bit RGBA floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	ABGR_F32,			/**< 128bit RGBA floating point format */
	GR_F16,				/**< 32bit RG floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	GR_F32,				/**< 64bit RG floating point format */
	R_F16,				/**< 16bit R floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	R_F32,				/**< 32bit R floating point format */

	// packed float formats
	R11G11B10_F,		/**< 32bit RGB floating point format - each component has 5bit exponent, no sign */
	R9G9B9_E5,			/**< 32bit RGB floating point format with shared exponent, no sign */

	// palette formats
	P8,					/**< 8bit paletted format */
	P4,					/**< 4bit paletted format */

	// depth formats
	D16,				/**< 16bit depth format */
	D24X8,				/**< 24bit depth format */
	D32,				/**< 32bit depth format */
	D32F,				/**< 32bit floating point depth format */

	// depth/stencil formats
	D15S1,				/**< 15bit depth format with 1bit stencil */
	D24S8,				/**< 24bit depth format with 8bit stencil */
	D24FS8,				/**< 24bit floating point depth format with 8bit stencil */
	D32FS8X24,			/**< 32bit floating point depth format with 8bit stencil */

	// compressed formats
	DXT1,				/**< Compressed DXT1/BC1 format */
	DXT2,				/**< Compressed DXT2 format */
	DXT3,				/**< Compressed DXT3/BC2 format */
	DXT4,				/**< Compressed DXT4 format */
	DXT5,				/**< Compressed DXT5/BC3 format */
	ATI1,				/**< Compressed 3Dc+/BC4 alpha format */
	ATI2,				/**< Compressed 3Dc/DXN/BC5/LATC/RGTC RG/UV format */
	BPTC_F,				/**< Compressed BPTC_FLOAT/BC6H HDR floating point RGB format */
	BPTC,				/**< Compressed BPTC/BC7 RGBA format */
	CTX1,				/**< Compressed CTX1 RG/UV format */
	ETC1,				/**< Compressed ETC1 RGB format */
	ETC2,				/**< Compressed ETC2 RGB format */
	EAC,				/**< Compressed EAC alpha format */
	ETC2_EAC,			/**< Compressed ETC2+EAC RGBA format */
	EACx2,				/**< Compressed EACx2 RG/UV format */
	PVRTC_RGB_2bpp,		/**< Compressed PVRTC 2bpp RGB format */
	PVRTC_RGB_4bpp,		/**< Compressed PVRTC 4bpp RGB format */
	PVRTC_RGBA_2bpp,	/**< Compressed PVRTC 2bpp RGBA format */
	PVRTC_RGBA_4bpp,	/**< Compressed PVRTC 4bpp RGBA format */
	PVRTC2_2bpp,		/**< Compressed PVRTC2 2bpp RGBA format */
	PVRTC2_4bpp,		/**< Compressed PVRTC2 4bpp RGBA format */
	ATCRGB,				/**< Compressed ATC1/ATITC RGB format */
	ATCRGBA_EXPLICIT,	/**< Compressed ATC3/ATITC RGBA format with explicit alpha */
	ATCRGBA,			/**< Compressed ATC5/ATITC RGBA format */
	ASTC,				/**< Compressed ASTC format */

	PSP_DXT1,			/**< Special DXT1 for PSP */
	PSP_DXT3,			/**< Special DXT3 for PSP */
	PSP_DXT5,			/**< Special DXT5 for PSP */

	Max,				/**< Max image format */

	// format flags
	Signed = 0x100,		/**< Components are signed. */
	Integer = 0x200,	/**< Components are non-normalised integers. */
	Linear = 0x400,		/**< Image uses linear colour space. (sRGB is the assumed default) */
	Swizzle = 0x800,	/**< Image is swizzled. (for some target platform given when swizzling) */

	// auto-select formats
	SelectDefault = 0x1000,			/**< Select the default format. It will be a format that performs well without sacrificing quality, and contains alpha. */
	SelectNicest = 0x1001,			/**< Select the nicest format. */
	SelectFastest = 0x1002,			/**< Select the fastest format. */
	SelectHDR = 0x1003,				/**< Select a HDR format. */

	SelectNoAlpha = 0x1004,			/**< Select a format with no alpha. */
	Select1BitAlpha = 0x1008,		/**< Select a format optimised for 1-bit alpha. */

	SelectRenderTarget = 0x1010,	/**< Select a format that can be used as a render target. */
	SelectDepth = 0x1020,			/**< Select a format that can be used as a depth target. */
	SelectDepthStencil = 0x1030,	/**< Select a format that can be used as a depth and stencil target. */
}

enum MFImageFormat_Signed(MFImageFormat format) = format | MFImageFormat.Signed;
enum MFImageFormat_Integer(MFImageFormat format) = format | MFImageFormat.Integer;
enum MFImageFormat_SignedInteger(MFImageFormat format) = format | MFImageFormat.Signed | MFImageFormat.Integer;
enum MFImageFormat_Linear(MFImageFormat format) = format | MFImageFormat.Linear;
enum MFImageFormat_Swizzle(MFImageFormat format) = format | MFImageFormat.Swizzle;

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


/**
* Convert between image formats.
* Converts an image from one format to another.
* @param width Image width.
* @param height Image height.
* @param pInput Pointer to the source image.
* @param inputFormat Format of source image.
* @param pOutput Pointer to a buffer that receives the converted image.
* @param outputFormat Format to convert inage to.
* @return None.
*/
extern (C) void MFImage_Convert(uint width, uint height, const(void)* pInput, MFImageFormat inputFormat, void* pOutput, MFImageFormat outputFormat);

/**
* Scale an image using a non-trivial scaling algorithm.
* Scales an image using a non-trivial scaling algorithm.
* @param pScaleData An MFScaleImage struct to describe the scale operation.
* @return None.
*/
extern (C) void MFImage_Scale(MFScaleImage* pScaleData);

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
* @return True if specified format is supported in hardware.
* @see MFTexture_GetPlatformAvailability()
*/
extern (C) bool MFImage_IsAvailableOnPlatform(int format, int platform);

/**
* Get the average number of bits per pixel for a specified format.
* Get the average number of bits per pixel for the specified format.
* @param format Name for the texture being created.
* @return The number of bits per pixel for the specified format. If a compressed format is specified, the average number of bits per pixel is returned.
* @see MFTexture_GetPlatformAvailability(), MFImage_GetFormatString()
*/
extern (C) int MFImage_GetBitsPerPixel(int format);
