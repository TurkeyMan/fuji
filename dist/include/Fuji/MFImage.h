/**
 * @file MFImage.h
 * @brief General image processing API.
 * @author Manu Evans
 * @defgroup MFImage Image Manipulation
 * @{
 */

#if !defined(_MFIMAGE_H)
#define _MFIMAGE_H

/**
 * Image format.
 * Describes a image format.
 */
enum MFImageFormat
{
	ImgFmt_Unknown = -1,	/**< Unknown image format */

	ImgFmt_A8R8G8B8,		/**< 32bit BGRA format */
	ImgFmt_A8B8G8R8,		/**< 32bit RGBA format */
	ImgFmt_B8G8R8A8,		/**< 32bit ARGB format */
	ImgFmt_R8G8B8A8,		/**< 32bit ABGR format */

	ImgFmt_R8G8B8,			/**< 24bit BGR format */
	ImgFmt_B8G8R8,			/**< 24bit RGB format */

	ImgFmt_A2R10G10B10,		/**< 32bit BGRA format with 10 bits per colour channel */
	ImgFmt_A2B10G10R10,		/**< 32bit RGBA format with 10 bits per colour channel */

	ImgFmt_A16B16G16R16,	/**< 64bit RGBA format with 16 bits per colour channel */

	ImgFmt_R5G6B5,			/**< 16bit BGR format with no alpha */
	ImgFmt_R6G5B5,			/**< 16bit BGR format with no alpha and 6 bits for red */
	ImgFmt_B5G6R5,			/**< 16bit RGB format with no alpha */

	ImgFmt_A1R5G5B5,		/**< 16bit BGRA format with 1 bit alpha */
	ImgFmt_R5G5B5A1,		/**< 16bit ABGR format with 1 bit alpha */
	ImgFmt_A1B5G5R5,		/**< 16bit RGBA format with 1 bit alpha */

	ImgFmt_A4R4G4B4,		/**< 16bit BGRA format with 4 bits per colour channel */
	ImgFmt_A4B4G4R4,		/**< 16bit RGBA format with 4 bits per colour channel */
	ImgFmt_R4G4B4A4,		/**< 16bit ABGR format with 4 bits per colour channel */

	ImgFmt_ABGR_F16,		/**< 64bit RGBA floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	ImgFmt_ABGR_F32,		/**< 128bit RGBA floating point format */

	ImgFmt_R11G11B10_F,		/**< 32bit RGB floating point format - each component has 5bit exponent, no sign */
	ImgFmt_R9G9B9_E5,		/**< 32bit RGB floating point format with shared exponent, no sign */

	ImgFmt_I8,				/**< 8bit paletted format */
	ImgFmt_I4,				/**< 4bit paletted format */

	ImgFmt_D16,				/**< 16bit depth format */
	ImgFmt_D24X8,			/**< 24bit depth format */
	ImgFmt_D32,				/**< 32bit depth format */
	ImgFmt_D32F,			/**< 32bit floating point depth format */

	ImgFmt_D15S1,			/**< 15bit depth format with 1bit stencil */
	ImgFmt_D24S8,			/**< 24bit depth format with 8bit stencil */
	ImgFmt_D24FS8,			/**< 24bit floating point depth format with 8bit stencil */
	ImgFmt_D32FS8X24,		/**< 32bit floating point depth format with 8bit stencil */

	ImgFmt_DXT1,			/**< Compressed DXT1/BC1 format */
	ImgFmt_DXT2,			/**< Compressed DXT2 format */
	ImgFmt_DXT3,			/**< Compressed DXT3/BC2 format */
	ImgFmt_DXT4,			/**< Compressed DXT4 format */
	ImgFmt_DXT5,			/**< Compressed DXT5/BC3 format */
//	ImgFmt_ATI1,			/**< Compressed 3Dc+/BC4 format */
//	ImgFmt_ATI2,			/**< Compressed 3Dc/DXN/BC5 format */
//	ImgFmt_BPTC_F,			/**< Compressed BPTC_FLOAT/BC6H floating point format */
//	ImgFmt_BPTC,			/**< Compressed BPTC/BC7 format */
//	ImgFmt_CTX1,			/**< Compressed CTX1 format */
//	ImgFmt_ETC1,			/**< Compressed ETC1 format */
//	ImgFmt_ETC2,			/**< Compressed ETC2 format */
//	ImgFmt_EAC,				/**< Compressed EAC format */
//	ImgFmt_PVRTC,			/**< Compressed PVRTC format */
//	ImgFmt_PVRTC2,			/**< Compressed PVRTC2 format */
//	ImgFmt_ATCRGB,			/**< Compressed ATC1/ATITC RGB format */
//	ImgFmt_ATCRGBA_EXPLICIT,/**< Compressed ATC3/ATITC RGBA format with explicit alpha */
//	ImgFmt_ATCRGBA,			/**< Compressed ATC5/ATITC RGBA format */
//	ImgFmt_ASTC,			/**< Compressed ASTC format */

	ImgFmt_PSP_DXT1,		/**< Special DXT1 for PSP */
	ImgFmt_PSP_DXT3,		/**< Special DXT3 for PSP */
	ImgFmt_PSP_DXT5,		/**< Special DXT5 for PSP */

	// platform specific swizzled formats
	ImgFmt_XB_A8R8G8B8s,	/**< 32bit BGRA format, swizzled for XBox */
	ImgFmt_XB_A8B8G8R8s,	/**< 32bit RGBA format, swizzled for XBox */
	ImgFmt_XB_B8G8R8A8s,	/**< 32bit ARGB format, swizzled for XBox */
	ImgFmt_XB_R8G8B8A8s,	/**< 32bit ABGR format, swizzled for XBox */

	ImgFmt_XB_R5G6B5s,		/**< 16bit BGR format, swizzled for XBox */
	ImgFmt_XB_R6G5B5s,		/**< 16bit BGR format, swizzled for XBox */

	ImgFmt_XB_A1R5G5B5s,	/**< 16bit BGRA format, swizzled for XBox */
	ImgFmt_XB_R5G5B5A1s,	/**< 16bit ABGR format, swizzled for XBox */

	ImgFmt_XB_A4R4G4B4s,	/**< 16bit BGRA format, swizzled for XBox */
	ImgFmt_XB_R4G4B4A4s,	/**< 16bit ABGR format, swizzled for XBox */

	ImgFmt_PSP_A8B8G8R8s,	/**< 32bit RGBA format, swizzled for PSP */
	ImgFmt_PSP_B5G6R5s,		/**< 16bit RGB format, swizzled for PSP */
	ImgFmt_PSP_A1B5G5R5s,	/**< 16bit RGBA format, swizzled for PSP */
	ImgFmt_PSP_A4B4G4R4s,	/**< 16bit RGBA format, swizzled for PSP */

	ImgFmt_PSP_I8s,			/**< 8bit paletted format, swizzled for PSP */
	ImgFmt_PSP_I4s,			/**< 4bit paletted format, swizzled for PSP */

	ImgFmt_PSP_DXT1s,		/**< DXT1, swizzled for PSP */
	ImgFmt_PSP_DXT3s,		/**< DXT3, swizzled for PSP */
	ImgFmt_PSP_DXT5s,		/**< DXT5, swizzled for PSP */

	ImgFmt_Max,				/**< Max image format */

	ImgFmt_SelectDefault = 0x1000,		/**< Select the default format. It will be a format that performs well without sacrificing quality, and contains alpha. */
	ImgFmt_SelectNicest = 0x1001,		/**< Select the nicest format. */
	ImgFmt_SelectFastest = 0x1002,		/**< Select the fastest format. */
	ImgFmt_SelectHDR = 0x1003,			/**< Select a HDR format. */

	ImgFmt_SelectNoAlpha = 0x1004,		/**< Select a format with no alpha. */
	ImgFmt_Select1BitAlpha = 0x1008,	/**< Select a format optimised for 1-bit alpha. */

	ImgFmt_SelectRenderTarget = 0x1010,	/**< Select a format that can be used as a render target. */
	ImgFmt_SelectDepth = 0x1020,		/**< Select a format that can be used as a depth target. */
	ImgFmt_SelectDepthStencil = 0x1030,	/**< Select a format that can be used as a depth and stencil target. */

	ImgFmt_ForceInt = 0x7FFFFFFF		/**< Force image format to int type */
};

/**
 * Scaling algorithm.
 * Supported scaling algorithms.
 */
enum MFScalingAlgorithm
{
	SA_Unknown = -1,			/**< Unknown algorithm */
	SA_None = 0,				/**< No scaling */
	SA_Nearest,					/**< Nearest filtering: any size */
	SA_Bilinear,				/**< Bilinear filtering: any size */
	SA_Box,						/**< Box filtering: 1/2x (common for mip generation) */
	SA_HQX,						/**< 'High Quality nX' algorithm: 2x, 3x, 4x */
	SA_AdvMAME,					/**< 'Advance MAME' algorithm: 2x, 3x, 4x */
	SA_Eagle,					/**< 'Eagle' algorithm: 2x */
	SA_SuperEagle,				/**< 'Super Eagle' algorithm: 2x */
	SA_2xSaI,					/**< '2x Scale and Interpolate' algorithm: 2x */
	SA_Super2xSaI,				/**< 'Super 2x Scale and Interpolate' algorithm: 2x */

	SA_Max,						/**< Maximum scaling algorithm */
	SA_ForceInt = 0x7FFFFFFF	/**< Force MFScalingAlgorithm to an int type */
};

/**
 * Image scaling data.
 * Image scaling data.
 */
struct MFScaleImage
{
	void *pSourceImage;				/**< Pointer to the source image data */
	int sourceWidth;				/**< Width of source image */
	int sourceHeight;				/**< Height of source image */
	int sourceStride;				/**< Stride of source image in pixels */

	void *pTargetBuffer;			/**< Pointer to a target buffer */
	int targetWidth;				/**< Width of target image */
	int targetHeight;				/**< Height of target image */
	int targetStride;				/**< Stride of target image in pixels */

	MFImageFormat format;			/**< The format of the source image */
	MFScalingAlgorithm algorithm;	/**< The scaling algorithm to use */
};

// interface functions

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
MF_API void MFImage_Convert(uint32 width, uint32 height, const void *pInput, MFImageFormat inputFormat, void *pOutput, MFImageFormat outputFormat);

/**
 * Scale an image using a non-trivial scaling algorithm.
 * Scales an image using a non-trivial scaling algorithm.
 * @param pScaleData An MFScaleImage struct to describe the scale operation.
 * @return None.
 */
MF_API void MFImage_Scale(MFScaleImage *pScaleData);

/**
 * Get a string representing the image format.
 * Gets a human readable string representing the image format.
 * @param format Image format to get the name of.
 * @return Pointer to a string representing the image format.
 * @see MFImage_GetPlatformAvailability(), MFImage_GetBitsPerPixel()
 */
MF_API const char * MFImage_GetFormatString(int format);

/**
 * Gets all platforms that support the specified image format in hardware.
 * Gets a variable representing which platforms support the specified image format in hardware.
 * @param format Format to test for hardware support.
 * @return Result is a bitfield where each bit represents hardware support for a specific platform. Platform support can be tested, for example, using: ( result & MFBIT(FP_PC) ) != 0.
 * @see MFImage_GetFormatString(), MFImage_GetBitsPerPixel()
 */
MF_API uint32 MFImage_GetPlatformAvailability(int format);

/**
 * Tests to see if a image format is available on a specified platform.
 * Tests if a image format is supported in hardware on a specified platform.
 * @param format Image format to be tested.
 * @param platform Platform to test for hardware support.
 * @return True if specified format is supported in hardware.
 * @see MFImage_GetPlatformAvailability()
 */
MF_API bool MFImage_IsAvailableOnPlatform(int format, int platform);

/**
 * Resolve an image format for a given platform.
 * Resolves an auto-selected (ImgFmt_Select*) image format for the given platform.
 * @param format Image format to resolve.
 * @param driver Rendering driver to resolve for.
 * @return An \a MFImageFormat that matches the criteria.
 * @see MFImage_GetPlatformAvailability()
 */
MF_API MFImageFormat MFImage_ResolveFormat(int format, MFRendererDrivers driver);

/**
 * Get the average number of bits per pixel for a specified format.
 * Get the average number of bits per pixel for the specified format.
 * @param format Image format to query.
 * @return The number of bits per pixel for the specified format. If a compressed format is specified, the average number of bits per pixel is returned.
 * @see MFImage_GetPlatformAvailability(), MFImage_GetFormatString()
 */
MF_API int MFImage_GetBitsPerPixel(int format);

#endif // _MFIMAGE_H

/** @} */
