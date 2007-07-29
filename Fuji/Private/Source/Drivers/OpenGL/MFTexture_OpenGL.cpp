#include "Fuji.h"

#if MF_RENDERER == MF_DRIVER_OPENGL

/**** Defines ****/

/**** Includes ****/

#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#if defined(MF_LINUX) || defined(MF_OSX)
	#include <GL/glx.h>
#endif
#include <GL/glu.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

struct GLFormat
{
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

GLFormat gGLFormats[] =
{
	{4, GL_BGRA_EXT, GL_UNSIGNED_BYTE},
	{4, GL_RGBA, GL_UNSIGNED_BYTE}
};

static const int gMaxGLFormats = sizeof(gGLFormats) / sizeof(GLFormat);

/**** Functions ****/

// interface functions
void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain)
{
	MFCALLSTACK;

	MFTextureTemplateData *pTemplate = pTexture->pTemplateData;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &(pTexture->textureID));
	glBindTexture(GL_TEXTURE_2D, pTexture->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	MFDebug_Assert(pTemplate->platformFormat < (uint32)gMaxGLFormats, "Platform format is undefined...");

	if(generateMipChain)
	{
		MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[0];
		gluBuild2DMipmaps(GL_TEXTURE_2D, gGLFormats[pTemplate->platformFormat].internalFormat, pSurf->width, pSurf->height, gGLFormats[pTemplate->platformFormat].format, gGLFormats[pTemplate->platformFormat].type, pSurf->pImageData);
	}
	else
	{
		for(int a=0; a<pTemplate->mipLevels; a++)
		{
			MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[a];
			glTexImage2D(GL_TEXTURE_2D, a, gGLFormats[pTemplate->platformFormat].internalFormat, pSurf->width, pSurf->height, 0, gGLFormats[pTemplate->platformFormat].format, gGLFormats[pTemplate->platformFormat].type, pSurf->pImageData);
		}
	}
}

MFTexture* MFTexture_CreateRenderTarget(const char *pName, int width, int height)
{
	MFCALLSTACK;

	MFDebug_Assert(false, "Not Written...");

	return NULL;
}

int MFTexture_Destroy(MFTexture *pTexture)
{
	MFCALLSTACK;

	pTexture->refCount--;

	// if no references left, destroy texture
	if(!pTexture->refCount)
	{
		glDeleteTextures(1, &pTexture->textureID);

		MFHeap_Free(pTexture->pTemplateData);
		gTextureBank.Destroy(pTexture);

		return 0;
	}

	return pTexture->refCount;
}

#endif
