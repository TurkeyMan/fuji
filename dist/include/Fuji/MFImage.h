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
	ImgFmt_Unknown = -1, /**< Unknown image format */

	ImgFmt_A8R8G8B8,	/**< 32bit BGRA format */
	ImgFmt_A8B8G8R8,	/**< 32bit RGBA format */
	ImgFmt_B8G8R8A8,	/**< 32bit ARGB format */
	ImgFmt_R8G8B8A8,	/**< 32bit ABGR format */

	ImgFmt_R8G8B8,	/**< 24bit BGR format */
	ImgFmt_B8G8R8,	/**< 24bit RGB format */

	ImgFmt_A2R10G10B10,	/**< 32bit BGRA format with 10 bits per colour channel */
	ImgFmt_A2B10G10R10,	/**< 32bit RGBA format with 10 bits per colour channel */

	ImgFmt_A16B16G16R16, /**< 64bit RGBA format with 16 bits per colour channel */

	ImgFmt_R5G6B5,		/**< 16bit BGR format with no alpha */
	ImgFmt_R6G5B5,		/**< 16bit BGR format with no alpha and 6 bits for red */
	ImgFmt_B5G6R5,		/**< 16bit RGB format with no alpha */

	ImgFmt_A1R5G5B5,	/**< 16bit BGRA format with 1 bit alpha */
	ImgFmt_R5G5B5A1,	/**< 16bit ABGR format with 1 bit alpha */
	ImgFmt_A1B5G5R5,	/**< 16bit RGBA format with 1 bit alpha */

	ImgFmt_A4R4G4B4,	/**< 16bit BGRA format with 4 bits per colour channel */
	ImgFmt_A4B4G4R4,	/**< 16bit RGBA format with 4 bits per colour channel */
	ImgFmt_R4G4B4A4,	/**< 16bit ABGR format with 4 bits per colour channel */

	ImgFmt_ABGR_F16,	/**< 64bit RGBA floating point format - 16bit floats are described as follows, sign1-exp5-mantissa10 - seeeeemmmmmmmmmm */
	ImgFmt_ABGR_F32,	/**< 128bit RGBA floating point format */

	ImgFmt_I8,			/**< 8bit paletted format */
	ImgFmt_I4,			/**< 4bit paletted format */

	ImgFmt_DXT1,		/**< Compressed DXT1 image */
	ImgFmt_DXT2,		/**< Compressed DXT2 image */
	ImgFmt_DXT3,		/**< Compressed DXT3 image */
	ImgFmt_DXT4,		/**< Compressed DXT4 image */
	ImgFmt_DXT5,		/**< Compressed DXT5 image */

	ImgFmt_PSP_DXT1,	/**< Special DXT1 for PSP */
	ImgFmt_PSP_DXT3,	/**< Special DXT3 for PSP */
	ImgFmt_PSP_DXT5,	/**< Special DXT5 for PSP */

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

	ImgFmt_SelectNicest = 0x1000,			/**< Select the nicest format. */
	ImgFmt_SelectNicest_NoAlpha = 0x1001,	/**< Select the nicest format with no alpha channel. */
	ImgFmt_SelectFastest = 0x1002,			/**< Select the fastest format. */
	ImgFmt_SelectFastest_Masked = 0x1003,	/**< Select the fastest format requiring only a single bit of alpha. */
	ImgFmt_SelectFastest_NoAlpha = 0x1004,	/**< Select the fastest format with no alpha channel. */

	ImgFmt_ForceInt = 0x7FFFFFFF			/**< Force image format to int type */
};

/**
 * Scaling algorithm.
 * Supported scaling algorithms.
 */
enum MFScalingAlgorithm
{
	SA_Unknown = -1,
	SA_None = 0,	// no scaling
	SA_Nearest,		// nearest filtering: any size
	SA_Bilinear,	// bilinear filtering: any size
	SA_Box,			// box filtering: 1/2x (common for mip generation)
	SA_HQX,			// 'High Quality nX' algorithm: 2x, 3x, 4x
	SA_AdvMAME,		// 'Advance MAME' algorithm: 2x, 3x, 4x
	SA_Eagle,		// 'Eagle' algorithm: 2x
	SA_SuperEagle,	// 'Super Eagle' algorithm: 2x
	SA_2xSaI,		// '2x Scale and Interpolate' algorithm: 2x
	SA_Super2xSaI,	// 'Super 2x Scale and Interpolate' algorithm: 2x

	SA_Max,
	SA_ForceInt = 0x7FFFFFFF
};

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

	MFImageFormat format;
	MFScalingAlgorithm algorithm;
};

// interface functions

MF_API void MFImage_Scale(MFScaleImage *pScaleData);

/**
 * Get a string representing the image format.
 * Gets a human readable string representing the image format.
 * @param format Image format to get the name of.
 * @return Pointer to a string representing the image format.
 * @see MFImage_GetPlatformAvailability(), MFImage_GetBitsPerPixel()
 */
MF_API const char * const MFImage_GetFormatString(int format);

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
 * @return Returns true if specified format is supported in hardware.
 * @see MFImage_GetPlatformAvailability()
 */
MF_API bool MFImage_IsAvailableOnPlatform(int format, int platform);

/**
 * Get the average number of bits per pixel for a specified format.
 * Get the average number of bits per pixel for the specified format.
 * @param format Image format to query.
 * @return Returns the number of bits per pixel for the specified format. If a compressed format is specified, the average number of bits per pixel is returned.
 * @see MFImage_GetPlatformAvailability(), MFImage_GetFormatString()
 */
MF_API int MFImage_GetBitsPerPixel(int format);

#endif // _MFIMAGE_H

/** @} */
