/**** Defines ****/

/**** Includes ****/

#include "Fuji.h"
#include "MFTexture_Internal.h"
#include "Display_Internal.h"
#include "MFFileSystem_Internal.h"
#include "MFPtrList.h"

#include <GL/glx.h>
#include <GL/glu.h>

/**** Globals ****/

extern MFPtrListDL<MFTexture> gTextureBank;
extern MFTexture *pNoneTexture;

struct LinuxFormat
{
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

LinuxFormat gLinuxFormats[] =
{
	{4, GL_BGRA_EXT, GL_UNSIGNED_BYTE},
	{4, GL_RGBA, GL_UNSIGNED_BYTE}
};

static const int gMaxLinuxFormats = sizeof(gLinuxFormats) / sizeof(LinuxFormat);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	MFDebug_Assert(pTemplate->platformFormat < (uint32)gMaxLinuxFormats, "Platform format is undefined...");

	if(generateMipChain)
	{
		MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[0];
		gluBuild2DMipmaps(GL_TEXTURE_2D, gLinuxFormats[pTemplate->platformFormat].internalFormat, pSurf->width, pSurf->height, gLinuxFormats[pTemplate->platformFormat].format, gLinuxFormats[pTemplate->platformFormat].type, pSurf->pImageData);
	}
	else
	{
		for(int a=0; a<pTemplate->mipLevels; a++)
		{
			MFTextureSurfaceLevel *pSurf = &pTemplate->pSurfaces[a];
			glTexImage2D(GL_TEXTURE_2D, a, gLinuxFormats[pTemplate->platformFormat].internalFormat, pSurf->width, pSurf->height, 0, gLinuxFormats[pTemplate->platformFormat].format, gLinuxFormats[pTemplate->platformFormat].type, pSurf->pImageData);
		}
	}
}

MFTexture* MFTexture_CreateFromRawData(const char *pName, void *pData, int width, int height, MFTextureFormat format, uint32 flags, bool generateMipChain, uint32 *pPalette)
{
	MFCALLSTACK;

	MFTexture *pTexture = MFTexture_FindTexture(pName);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create();
		pTexture->refCount = 0;

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &(pTexture->textureID));
		glBindTexture(GL_TEXTURE_2D, pTexture->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		int internalFormat = gMFTexturePlatformFormat[FP_Linux][format];

		if(generateMipChain)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, gLinuxFormats[internalFormat].internalFormat, width, height, gLinuxFormats[internalFormat].format, gLinuxFormats[internalFormat].type, pData);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, gLinuxFormats[internalFormat].internalFormat, width, height, 0, gLinuxFormats[internalFormat].format, gLinuxFormats[internalFormat].type, pData);
		}

		MFString_Copy(pTexture->name, pName);

		// create template data
		char *pTemplate = (char*)MFHeap_Alloc(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel));

		pTexture->pTemplateData = (MFTextureTemplateData*)pTemplate;
		pTexture->pTemplateData->pSurfaces = (MFTextureSurfaceLevel*)(pTemplate + sizeof(MFTextureTemplateData));

		pTexture->pTemplateData->imageFormat = format;
		pTexture->pTemplateData->platformFormat = internalFormat;

		pTexture->pTemplateData->mipLevels = 1;

		pTexture->pTemplateData->pSurfaces->width = width;
		pTexture->pTemplateData->pSurfaces->height = height;
		pTexture->pTemplateData->pSurfaces->pImageData = (char*)pData;
	}

	pTexture->refCount++;

	return pTexture;
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
