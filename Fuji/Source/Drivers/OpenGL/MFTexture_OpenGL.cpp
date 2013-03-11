#include "Fuji.h"

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

#if !defined(MF_OPENGL_ES)
// opengl format table.. man, opengl's texture format management is a complete shambles!!!!
// i havent verified that all of these formats are correct, some might be wrong...
GLFormat gGLFormats[] =
{
	{ GL_RGBA8, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV },						// ImgFmt_A8R8G8B8
	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV },							// ImgFmt_A8B8G8R8
	{ GL_RGBA8, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8 },							// ImgFmt_B8G8R8A8
	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 },								// ImgFmt_R8G8B8A8
	{ GL_RGB10_A2, GL_BGRA_EXT, GL_UNSIGNED_INT_2_10_10_10_REV },				// ImgFmt_A2R10G10B10
	{ GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV },					// ImgFmt_A2B10G10R10
	{ GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT },									// ImgFmt_A16B16G16R16
	{ GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },								// ImgFmt_R5G6B5
																				// ImgFmt_R6G5B5
	{ GL_RGB5, GL_BGR_EXT, GL_UNSIGNED_SHORT_5_6_5_REV },						// ImgFmt_B5G6R5
	{ GL_RGB5_A1, GL_BGRA_EXT, GL_UNSIGNED_SHORT_1_5_5_5_REV },					// ImgFmt_A1R5G5B5
	{ GL_RGB5_A1, GL_ABGR_EXT, GL_UNSIGNED_SHORT_5_5_5_1 },						// ImgFmt_R5G5B5A1
	{ GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },							// ImgFmt_A1B5G5R5
	{ GL_RGBA4, GL_BGRA_EXT, GL_UNSIGNED_SHORT_4_4_4_4_REV },					// ImgFmt_A4R4G4B4
	{ GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },							// ImgFmt_A4B4G4R4
	{ GL_RGBA4, GL_ABGR_EXT, GL_UNSIGNED_SHORT_4_4_4_4 },						// ImgFmt_R4G4B4A4
	{ GL_RGBA16F_ARB, GL_RGBA, GL_HALF_FLOAT_ARB },								// ImgFmt_ABGR_F16
	{ GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT },										// ImgFmt_ABGR_F32
	{ GL_COLOR_INDEX8_EXT, GL_COLOR_INDEX, GL_UNSIGNED_BYTE },					// ImgFmt_I8
																				// ImgFmt_I4
	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0 },	// ImgFmt_DXT1
																				// ImgFmt_DXT2
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0 },	// ImgFmt_DXT3
																				// ImgFmt_DXT4
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0 }	// ImgFmt_DXT5
};
#else
// opengl format table.. man, opengl's texture format management is a complete shambles!!!!
// i havent verified that all of these formats are correct, some might be wrong...
GLFormat gGLFormats[] =
{
	{ GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE },										// ImgFmt_A8R8G8B8
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE },										// ImgFmt_A8B8G8R8
	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_B8G8R8A8
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_R8G8B8A8
	{ 0, GL_BGRA, 0 },															// ImgFmt_A2R10G10B10   **
	{ 0, GL_RGBA, 0 },															// ImgFmt_A2B10G10R10   **
	{ 0, GL_RGBA, 0 },															// ImgFmt_A16B16G16R16  **
	{ GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 },								// ImgFmt_R5G6B5
																				// ImgFmt_R6G5B5
	{ GL_RGB, GL_RGB, 0 },														// ImgFmt_B5G6R5
	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_A1R5G5B5
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1 },							// ImgFmt_R5G5B5A1
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_A1B5G5R5
	{ GL_RGBA, GL_BGRA, 0 },													// ImgFmt_A4R4G4B4
	{ GL_RGBA, GL_RGBA, 0 },													// ImgFmt_A4B4G4R4
	{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4 },							// ImgFmt_R4G4B4A4
	{ 0, GL_RGBA, 0 },															// ImgFmt_ABGR_F16
	{ 0, GL_RGBA, 0 },															// ImgFmt_ABGR_F32
	{ 0, 0, GL_UNSIGNED_BYTE },													// ImgFmt_I8
																				// ImgFmt_I4
	{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0 },	// ImgFmt_DXT1
																				// ImgFmt_DXT2
	{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0 },	// ImgFmt_DXT3
																				// ImgFmt_DXT4
	{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0 }	// ImgFmt_DXT5
};
#endif

static const int gMaxGLFormats = sizeof(gGLFormats) / sizeof(GLFormat);

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

	uint32 platformFormat = MFTexture_GetPlatformFormatID(pTemplate->imageFormat, MFRD_OpenGL);
	MFDebug_Assert(platformFormat < (uint32)gMaxGLFormats, "Platform format is undefined...");

	GLFormat &format = gGLFormats[platformFormat];

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, pTemplate->mipLevels);
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
	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = (MFTexture*)MFHeap_Alloc(sizeof(MFTexture));
		pTexture->type = MFRT_Texture;
		pTexture->hash = MFUtil_HashString(pName);
		pTexture->refCount = 0;

		MFResource_AddResource(pTexture);

		if(targetFormat & ImgFmt_SelectNicest)
		{
#if defined(MF_IPHONE)
			switch(targetFormat)
			{
				case ImgFmt_SelectNicest:
					targetFormat = ImgFmt_A8R8G8B8;
					break;
				case ImgFmt_SelectNicest_NoAlpha:
					targetFormat = ImgFmt_R8G8B8;
					break;
				case ImgFmt_SelectFastest:
					targetFormat = ImgFmt_R4G4B4A4;
					break;
				case ImgFmt_SelectFastest_Masked:
					targetFormat = ImgFmt_R5G5B5A1;
					break;
				case ImgFmt_SelectFastest_NoAlpha:
					targetFormat = ImgFmt_R5G6B5;
					break;
			}
#else
			targetFormat = ImgFmt_A8R8G8B8;
#endif
		}

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

		MFString_CopyN(pTexture->name, pName, sizeof(pTexture->name) - 1);
		pTexture->name[sizeof(pTexture->name) - 1] = 0;

		// create the targets
		GLuint frameBufferID, textureID;

		// create the texture
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureID);
		pTexture->pInternalData = (void*)(uintp)textureID;

		glBindTexture(GL_TEXTURE_2D, textureID);
#if !defined(MF_OPENGL_ES)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
#endif

		uint32 platformFormat = MFTexture_GetPlatformFormatID(targetFormat, MFRD_OpenGL);
		GLFormat &format = gGLFormats[platformFormat];
		glTexImage2D(GL_TEXTURE_2D, 0, format.internalFormat, pSurface->width, pSurface->height, 0, format.format, format.type, NULL);

		// create the frame buffer
		glGenFramebuffers(1, &frameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
		pSurface->pImageData = (char*)(uintp)frameBufferID;

		// and it to the framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		uint32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		MFDebug_Assert(status == GL_FRAMEBUFFER_COMPLETE, "Incomplete frame buffer!");

		MFRenderer_SetDeviceRenderTarget();

		if(!MFCheckForOpenGLError())
			MFDebug_Log(0, "RenderTarget created successfully!");
	}

	pTexture->refCount++;

	return pTexture;
}

void MFTexture_DestroyPlatformSpecific(MFTexture *pTexture)
{
	MFCALLSTACK;

	GLuint *pTextures = (GLuint*)&pTexture->pInternalData;
	glDeleteTextures(1, pTextures);
}

#endif
