#include "Fuji_Internal.h"

#if MF_RENDERER == MF_DRIVER_OPENGL || defined(MF_RENDERPLUGIN_OPENGL)

#if defined(MF_RENDERPLUGIN_OPENGL)
	#define MFTexture_InitModulePlatformSpecific MFTexture_InitModulePlatformSpecific_OpenGL
	#define MFTexture_DeinitModulePlatformSpecific MFTexture_DeinitModulePlatformSpecific_OpenGL
	#define MFTexture_CreatePlatformSpecific MFTexture_CreatePlatformSpecific_OpenGL
	#define MFTexture_CreateRenderTarget MFTexture_CreateRenderTarget_OpenGL
	#define MFTexture_DestroyPlatformSpecific MFTexture_DestroyPlatformSpecific_OpenGL
#endif

/**** Includes ****/

#include "MFSystem.h"
#include "MFRenderer.h"
#include "MFTexture_Internal.h"
#include "MFDisplay_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include "MFOpenGL.h"

/**** Globals ****/

extern MFTexture *pNoneTexture;

struct GLFormat
{
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

// opengl format table.. man, opengl's texture format management is a complete shambles!!!!
// i havent verified that all of these formats are correct, some might be wrong...
#if !defined(MF_OPENGL_ES)
GLFormat gGLFormats[ImgFmt_Max] =
{
	{ GL_RGBA8, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV },						// ImgFmt_A8R8G8B8
	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV },							// ImgFmt_A8B8G8R8
	{ GL_RGBA8, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8 },							// ImgFmt_B8G8R8A8
	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 },								// ImgFmt_R8G8B8A8

	{ 0, 0, 0 },																// ImgFmt_R8G8B8
	{ 0, 0, 0 },																// ImgFmt_B8G8R8

	{ 0, 0, 0 },																// ImgFmt_G8R8

	{ 0, 0, 0 },																// ImgFmt_L8
	{ 0, 0, 0 },																// ImgFmt_A8
	{ 0, 0, 0 },																// ImgFmt_A8L8

	{ GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },								// ImgFmt_R5G6B5
	{ 0, 0, 0 },																// ImgFmt_R6G5B5
	{ GL_RGB5, GL_BGR_EXT, GL_UNSIGNED_SHORT_5_6_5_REV },						// ImgFmt_B5G6R5

	{ GL_RGB5_A1, GL_BGRA_EXT, GL_UNSIGNED_SHORT_1_5_5_5_REV },					// ImgFmt_A1R5G5B5
	{ GL_RGB5_A1, GL_ABGR_EXT, GL_UNSIGNED_SHORT_5_5_5_1 },						// ImgFmt_R5G5B5A1
	{ GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },							// ImgFmt_A1B5G5R5

	{ GL_RGBA4, GL_BGRA_EXT, GL_UNSIGNED_SHORT_4_4_4_4_REV },					// ImgFmt_A4R4G4B4
	{ GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },							// ImgFmt_A4B4G4R4
	{ GL_RGBA4, GL_ABGR_EXT, GL_UNSIGNED_SHORT_4_4_4_4 },						// ImgFmt_R4G4B4A4

	{ GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT },									// ImgFmt_A16B16G16R16

	{ GL_RG16, GL_RGBA, GL_UNSIGNED_SHORT },									// ImgFmt_G16R16

	{ 0, 0, 0 },																// ImgFmt_L16
	{ 0, 0, 0 },																// ImgFmt_A16
	{ 0, 0, 0 },																// ImgFmt_A16L16

	{ GL_RGB10_A2, GL_BGRA_EXT, GL_UNSIGNED_INT_2_10_10_10_REV },				// ImgFmt_A2R10G10B10
	{ GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },					// ImgFmt_A2B10G10R10

	{ 0, 0, 0 },																// ImgFmt_R10G11B11
	{ 0, 0, 0 },																// ImgFmt_R11G11B10

	{ GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT_ARB },								// ImgFmt_ABGR_F16
	{ GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT },										// ImgFmt_ABGR_F32
	{ GL_RG16F, GL_RGBA, GL_HALF_FLOAT_ARB },									// ImgFmt_GR_F16
	{ GL_RG32F, GL_RGBA, GL_FLOAT },											// ImgFmt_GR_F32
	{ GL_R16F, GL_RGBA, GL_HALF_FLOAT_ARB },									// ImgFmt_R_F16
	{ GL_R32F, GL_RGBA, GL_FLOAT },												// ImgFmt_R_F32

	{ GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV },				// ImgFmt_R11G11B10_F
	{ GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV },						// ImgFmt_R9G9B9_E5

	{ GL_COLOR_INDEX8_EXT, GL_COLOR_INDEX, GL_UNSIGNED_BYTE },					// ImgFmt_P8
	{ 0, 0, 0 },																// ImgFmt_P4

	{ GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT },			// ImgFmt_D16
	{ GL_DEPTH_COMPONENT24, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },			// ImgFmt_D24X8
	{ GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },				// ImgFmt_D32
	{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT },					// ImgFmt_D32F

	{ 0, 0, 0 },																// ImgFmt_D15S1
	{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },			// ImgFmt_D24S8
	{ 0, 0, 0 },																// ImgFmt_D24FS8
	{ GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV },	// ImgFmt_D32FS8X24

	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0 },	// ImgFmt_DXT1
	{ 0, 0, 0 },																// ImgFmt_DXT2
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0 },	// ImgFmt_DXT3
	{ 0, 0, 0 },																// ImgFmt_DXT4
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0 },	// ImgFmt_DXT5
	{ 0, 0, 0 },																// ImgFmt_ATI1
	{ GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI, GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI, 0 },			// ImgFmt_ATI2
	{ GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB, 0 },	// ImgFmt_BPTC_F
	{ GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, GL_COMPRESSED_RGBA_BPTC_UNORM_ARB, 0 },					// ImgFmt_BPTC
//		COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
//		COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
	{ 0, 0, 0 },																// ImgFmt_CTX1
	{ 0, 0, 0 },	// ETC1_RGB8_OES											// ImgFmt_ETC1
	{ 0, 0, 0 },	// COMPRESSED_RGB8_ETC2										// ImgFmt_ETC2
//		COMPRESSED_SRGB8_ETC2
//		COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
//		COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
	{ 0, 0, 0 },	// COMPRESSED_R11_EAC										// ImgFmt_EAC
//		COMPRESSED_SIGNED_R11_EAC
	{ 0, 0, 0 },	// COMPRESSED_RGBA8_ETC2_EAC								// ImgFmt_ETC2_EAC
//		COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
	{ 0, 0, 0 },	// COMPRESSED_RG11_EAC										// ImgFmt_EACx2
//		COMPRESSED_SIGNED_RG11_EAC
	{ 0, 0, 0 },	// COMPRESSED_RGB_PVRTC_2BPPV1_IMG							// ImgFmt_PVRTC_RGB_2bpp
	{ 0, 0, 0 },	// COMPRESSED_RGB_PVRTC_4BPPV1_IMG							// ImgFmt_PVRTC_RGB_4bpp
	{ 0, 0, 0 },	// COMPRESSED_RGBA_PVRTC_2BPPV1_IMG							// ImgFmt_PVRTC_RGBA_2bpp
	{ 0, 0, 0 },	// COMPRESSED_RGBA_PVRTC_4BPPV1_IMG							// ImgFmt_PVRTC_RGBA_4bpp
	{ 0, 0, 0 },	// COMPRESSED_RGBA_PVRTC_2BPPV2_IMG							// ImgFmt_PVRTC2_2bpp
	{ 0, 0, 0 },	// COMPRESSED_RGBA_PVRTC_4BPPV2_IMG							// ImgFmt_PVRTC2_4bpp
	{ 0, 0, 0 },	// ATC_RGB_AMD												// ImgFmt_ATCRGB
	{ 0, 0, 0 },	// ATC_RGBA_EXPLICIT_ALPHA_AMD								// ImgFmt_ATCRGBA_EXPLICIT
	{ 0, 0, 0 },	// ATC_RGBA_INTERPOLATED_ALPHA_AMD							// ImgFmt_ATCRGBA
	{ 0, 0, 0 },	// TODO: there are lots of these...							// ImgFmt_ASTC

	{ 0, 0, 0 },																// ImgFmt_PSP_DXT1
	{ 0, 0, 0 },																// ImgFmt_PSP_DXT3
	{ 0, 0, 0 },																// ImgFmt_PSP_DXT5
};
#else
GLFormat gGLFormats[] =
{
	{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE },										// ImgFmt_A8R8G8B8
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },										// ImgFmt_A8B8G8R8
	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_B8G8R8A8
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_R8G8B8A8

	{ 0, 0, 0 },																// ImgFmt_R8G8B8
	{ 0, 0, 0 },																// ImgFmt_B8G8R8

	{ 0, 0, 0 },																// ImgFmt_G8R8

	{ 0, 0, 0 },																// ImgFmt_L8
	{ 0, 0, 0 },																// ImgFmt_A8
	{ 0, 0, 0 },																// ImgFmt_A8L8

	{ GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },								// ImgFmt_R5G6B5
	{ 0, 0, 0 },																// ImgFmt_R6G5B5
	{ GL_RGB, GL_RGB, 0 },														// ImgFmt_B5G6R5

	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_A1R5G5B5
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },							// ImgFmt_R5G5B5A1
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_A1B5G5R5

	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_A4R4G4B4
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_A4B4G4R4
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },							// ImgFmt_R4G4B4A4

	{ 0, GL_RGBA, 0 },															// ImgFmt_A16B16G16R16  **

	{ 0, GL_RGBA, 0 },															// ImgFmt_G16R16  **

	{ 0, 0, 0 },																// ImgFmt_L16
	{ 0, 0, 0 },																// ImgFmt_A16
	{ 0, 0, 0 },																// ImgFmt_A16L16

	{ 0, GL_BGRA, 0 },															// ImgFmt_A2R10G10B10   **
	{ 0, GL_RGBA, 0 },															// ImgFmt_A2B10G10R10   **

	{ 0, 0, 0 },																// ImgFmt_R10G11B11
	{ 0, 0, 0 },																// ImgFmt_R11G11B10

	{ 0, GL_RGBA, 0 },															// ImgFmt_ABGR_F16
	{ 0, GL_RGBA, 0 },															// ImgFmt_ABGR_F32
	{ 0, GL_RGBA, 0 },															// ImgFmt_GR_F16
	{ 0, GL_RGBA, 0 },															// ImgFmt_GR_F32
	{ 0, GL_RGBA, 0 },															// ImgFmt_R_F16
	{ 0, GL_RGBA, 0 },															// ImgFmt_R_F32

	{ 0, 0, 0 },// GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV				// ImgFmt_R11G11B10_F
	{ 0, 0, 0 },// GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV							// ImgFmt_R9G9B9_E5

	{ 0, 0, GL_UNSIGNED_BYTE },													// ImgFmt_P8
	{ 0, 0, 0 },																// ImgFmt_P4

	{ GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT },			// ImgFmt_D16
	{ 0, 0, 0 },																// ImgFmt_D15S1
	{ GL_DEPTH_COMPONENT24, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },			// ImgFmt_D24X8
	{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8 },			// ImgFmt_D24S8
	{ 0, 0, 0 },																// ImgFmt_D24FS8
	{ GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT },				// ImgFmt_D32
	{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT },					// ImgFmt_D32F
	{ GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV },	// ImgFmt_D32FS8X24

	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0 },	// ImgFmt_DXT1
	{ 0, 0, 0 },																// ImgFmt_DXT2
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0 },	// ImgFmt_DXT3
	{ 0, 0, 0 },																// ImgFmt_DXT4
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0 },	// ImgFmt_DXT5
	{ 0, 0, 0 },																// ImgFmt_ATI1
	{ 0, 0, 0 },																// ImgFmt_ATI2
	{ 0, 0, 0 },																// ImgFmt_BPTC_F
	{ 0, 0, 0 },																// ImgFmt_BPTC
	{ 0, 0, 0 },																// ImgFmt_CTX1
	{ 0, 0, 0 },																// ImgFmt_ETC1
	{ 0, 0, 0 },																// ImgFmt_ETC2
	{ 0, 0, 0 },																// ImgFmt_EAC
	{ 0, 0, 0 },																// ImgFmt_ETC2_EAC
	{ 0, 0, 0 },																// ImgFmt_EACx2
	{ 0, 0, 0 },																// ImgFmt_PVRTC_RGB_2bpp
	{ 0, 0, 0 },																// ImgFmt_PVRTC_RGB_4bpp
	{ 0, 0, 0 },																// ImgFmt_PVRTC_RGBA_2bpp
	{ 0, 0, 0 },																// ImgFmt_PVRTC_RGBA_4bpp
	{ 0, 0, 0 },																// ImgFmt_PVRTC2_2bpp
	{ 0, 0, 0 },																// ImgFmt_PVRTC2_4bpp
	{ 0, 0, 0 },																// ImgFmt_ATCRGB
	{ 0, 0, 0 },																// ImgFmt_ATCRGBA_EXPLICIT
	{ 0, 0, 0 },																// ImgFmt_ATCRGBA
	{ 0, 0, 0 },																// ImgFmt_ASTC

	{ 0, 0, 0 },																// ImgFmt_PSP_DXT1
	{ 0, 0, 0 },																// ImgFmt_PSP_DXT3
	{ 0, 0, 0 },																// ImgFmt_PSP_DXT5
};
#endif

/**** Functions ****/

void MFTexture_InitModulePlatformSpecific()
{
}

void MFTexture_DeinitModulePlatformSpecific()
{
}

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	GLuint textureID;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	pTexture->pInternalData = (void*)(uintp)textureID;

	pTemplate->imageFormat = MFImage_ResolveFormat(pTemplate->imageFormat, MFRD_OpenGL);
	MFDebug_Assert(pTemplate->imageFormat != ImgFmt_Unknown, "Invalid texture format!");

	GLFormat &format = gGLFormats[pTemplate->imageFormat];
	MFDebug_Assert(format.internalFormat != 0, "Unsupported image format!");

	if(generateMipChain && pTemplate->mipLevels == 1)
	{
		int numMips = 0;
		// build mips from the top surface
		//...

		// set this to however many mips we just constructed
#if !defined(MF_OPENGL_ES)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numMips);
#endif

		MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[0];
		MFCheckForOpenGLError();
		glTexImage2D(GL_TEXTURE_2D, 0, format.internalFormat, pSurf->width, pSurf->height, 0, format.format, format.type, pSurf->pImageData);

		// set the mips
		//...
	}
	else
	{
#if !defined(MF_OPENGL_ES)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, pTemplate->mipLevels - 1);
#endif

		for(int a=0; a<pTemplate->mipLevels; a++)
		{
			MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[a];
			MFCheckForOpenGLError();
			glTexImage2D(GL_TEXTURE_2D, a, format.internalFormat, pSurf->width, pSurf->height, 0, format.format, format.type, pSurf->pImageData);
		}
	}

	MFCheckForOpenGLError();
}

MF_API MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height, MFImageFormat targetFormat)
{
	MFTexture *pTexture = MFTexture_Find(pName);

	if(!pTexture)
	{
		size_t nameLen = pName ? MFString_Length(pName) + 1 : 0;
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture) + nameLen);

		if(pName)
			pName = MFString_Copy((char*)&pTexture[1], pName);

		MFResource_AddResource(pTexture, MFRT_Texture, MFUtil_HashString(pName) ^ 0x7e407e40, pName);

		targetFormat = MFImage_ResolveFormat(targetFormat, MFRD_OpenGL);
		MFDebug_Assert(targetFormat != ImgFmt_Unknown, "Invalid texture format!");

		// allocate an MFTexture template
		pTexture->pTemplateData = (MFTextureTemplateData*)MFHeap_AllocAndZero(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)&pTexture->pTemplateData[1];
		pTexture->pTemplateData->magicNumber = MFMAKEFOURCC('F','T','E','X');
		pTexture->pTemplateData->imageFormat = targetFormat;
		pTexture->pTemplateData->mipLevels = 1;
		pTexture->pTemplateData->flags = TEX_RenderTarget;

		int bitsPerPixel = MFImage_GetBitsPerPixel(pTexture->pTemplateData->imageFormat);
		//int imageSize = (width * height * bitsPerPixel) >> 3;
		MFTextureSurfaceLevel *pSurface = &pTexture->pTemplateData->pSurfaces[0];
		pSurface->width = width;
		pSurface->height = height;
		pSurface->bitsPerPixel = bitsPerPixel;
		pSurface->xBlocks = width;
		pSurface->yBlocks = height;
		pSurface->bitsPerBlock = bitsPerPixel;
		pSurface->pImageData = NULL;
		pSurface->bufferLength = 0;
		pSurface->pPaletteEntries = NULL;
		pSurface->paletteBufferLength = 0;

		// create the targets
		GLuint textureID;

		// create the texture
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureID);
		pTexture->pInternalData = (void*)(uintp)textureID;

		glBindTexture(GL_TEXTURE_2D, textureID);
#if !defined(MF_OPENGL_ES)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
#endif

		GLFormat &format = gGLFormats[targetFormat];
		MFDebug_Assert(format.internalFormat != 0, "Unsupported image format!");

		glTexImage2D(GL_TEXTURE_2D, 0, format.internalFormat, pSurface->width, pSurface->height, 0, format.format, format.type, NULL);

		if(!MFCheckForOpenGLError())
			MFDebug_Log(0, "RenderTarget created successfully!");
	}

	return pTexture;
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	GLuint texture = (GLuint)(size_t)pTexture->pInternalData;
	glDeleteTextures(1, &texture);
}

#endif
