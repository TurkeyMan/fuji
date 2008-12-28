#include "Fuji.h"
#include "MFSystem.h"
#include "MFFileSystem.h"
#include "MFHeap.h"
#include "MFString.h"
#include "MFModel_Internal.h"
#include "Asset/MFIntModel.h"


/**** Structures ****/


/**** Globals ****/

static char *gFileExtensions[] =
{
	".dae",
	".x",
	".ase",
	".obj",
	".md2",
	".md3",
	".memd2"
};


/**** Functions ****/

MFIntModel *MFIntModel_CreateFromFile(const char *pFilename, MFIntModel **ppModel)
{

  return NULL;
}

MFIntModel *MFIntModel_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntModelFormat format)
{

  return NULL;
}

void MFIntModel_Optimise(MFIntModel *pModel)
{

}

void MFIntModel_CreateRuntimeData(MFIntModel *pModel, void **ppOutput, uint32 *pSize, MFPlatform platform)
{

}

void MFIntModel_Destroy(MFIntModel *pModel)
{

}

/*
MFIntTexture *MFIntTexture_CreateFromFile(const char *pFilename)
{
	// find format
	const char *pExt = MFString_GetFileExtension(pFilename);

	int format;
	for(format=0; format<MFIMF_Max; ++format)
	{
		if(!MFString_Compare(pExt, gFileExtensions[format]))
			break;
	}
	if(format == MFIMF_Max)
		return NULL;

	// load file
	uint32 size;
	char *pData = MFFileSystem_Load(pFilename, &size);
	if(!pData)
		return NULL;

	// load the image
	MFIntTexture *pImage = MFIntTexture_CreateFromFileInMemory(pData, size, (MFIntTextureFormat)format);

	// free file
	MFHeap_Free(pData);

	return pImage;
}

MFIntTexture *MFIntTexture_CreateFromFileInMemory(const void *pMemory, uint32 size, MFIntTextureFormat format)
{
	MFIntTexture *pImage = NULL;

	switch(format)
	{
		case MFIMF_TGA:
			pImage = LoadTGA(pMemory, size);
			break;
		case MFIMF_BMP:
			pImage = LoadBMP(pMemory, size);
			break;
		case MFIMF_PNG:
#if defined(MF_ENABLE_PNG)
			pImage = LoadPNG(pMemory, size);
#else
			MFDebug_Assert(false, "PNG support is not enabled in this build.");
#endif
			break;
		default:
			MFDebug_Assert(false, "Unsupported image format.");
	}

	if(pImage)
	{
		// scan for alpha information
		MFIntTexture_ScanImage(pImage);

		// build the mip chain
		MFIntTexture_FilterMipMaps(pImage, 0, 0);
	}

	return pImage;
}

void MFIntTexture_Destroy(MFIntTexture *pTexture)
{
	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		if(pTexture->pSurfaces[a].pData)
			MFHeap_Free(pTexture->pSurfaces[a].pData);
	}

	if(pTexture->pSurfaces)
		MFHeap_Free(pTexture->pSurfaces);

	if(pTexture)
		MFHeap_Free(pTexture);
}

void MFIntTexture_CreateRuntimeData(MFIntTexture *pTexture, MFTextureTemplateData **ppTemplateData, uint32 *pSize, MFPlatform platform, uint32 flags, MFTextureFormat targetFormat)
{
	*ppTemplateData = NULL;
	if(pSize)
		*pSize = 0;

	// choose target image format
	if(targetFormat == TexFmt_Unknown)
		targetFormat = ChooseBestFormat(pTexture, platform);

	// check minimum pitch
	MFDebug_Assert((pTexture->pSurfaces[0].width*MFTexture_GetBitsPerPixel(targetFormat)) / 8 >= 16, "Textures should have a minimum pitch of 16 bytes.");

	// check power of 2 dimensions
	MFDebug_Assert(IsPowerOf2(pTexture->pSurfaces[0].width) && IsPowerOf2(pTexture->pSurfaces[0].height), "Texture dimensions are not a power of 2.");

	// begin processing...
	if(flags & MFITF_PreMultipliedAlpha)
		PremultiplyAlpha(pTexture);

	// calculate texture data size..
	uint32 imageBytes = (uint32)MFALIGN(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pTexture->numSurfaces, 0x100);

	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		imageBytes += (pTexture->pSurfaces[a].width * pTexture->pSurfaces[a].height * MFTexture_GetBitsPerPixel(targetFormat)) / 8;

		// add palette
		uint32 paletteBytes = 0;

		if(targetFormat == TexFmt_I8)
			paletteBytes = 4*256;
		if(targetFormat == TexFmt_I4)
			paletteBytes = 4*16;

		imageBytes += paletteBytes;
	}

	// allocate buffer
//	MFHeap_SetAllocAlignment(4096);
	char *pOutputBuffer = (char*)MFHeap_Alloc(imageBytes);

	MFTextureTemplateData *pTemplate = (MFTextureTemplateData*)pOutputBuffer;
	MFZeroMemory(pTemplate, sizeof(MFTextureTemplateData));

	pTemplate->magicNumber = MFMAKEFOURCC('F','T','E','X');

	pTemplate->imageFormat = targetFormat;

	if(targetFormat >= TexFmt_XB_A8R8G8B8s)
		pTemplate->flags |= TEX_Swizzled;

	if(!pTexture->opaque)
	{
		if(pTexture->oneBitAlpha)
			pTemplate->flags |= 3;
		else
			pTemplate->flags |= 1;
	}

	if(flags & MFITF_PreMultipliedAlpha)
		pTemplate->flags |= TEX_PreMultipliedAlpha;

	pTemplate->mipLevels = pTexture->numSurfaces;
	pTemplate->pSurfaces = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	MFTextureSurfaceLevel *pSurfaceLevels = (MFTextureSurfaceLevel*)(pOutputBuffer + sizeof(MFTextureTemplateData));

	char *pDataPointer = pOutputBuffer + MFALIGN(sizeof(MFTextureTemplateData) + sizeof(MFTextureSurfaceLevel)*pTexture->numSurfaces, 0x100);

	for(int a=0; a<pTexture->numSurfaces; a++)
	{
		MFZeroMemory(&pSurfaceLevels[a], sizeof(MFTextureSurfaceLevel));

		pSurfaceLevels[a].width = pTexture->pSurfaces[a].width;
		pSurfaceLevels[a].height = pTexture->pSurfaces[a].height;
		pSurfaceLevels[a].bitsPerPixel = MFTexture_GetBitsPerPixel(targetFormat);

		pSurfaceLevels[a].xBlocks = -1;
		pSurfaceLevels[a].yBlocks = -1;
		pSurfaceLevels[a].bitsPerBlock = -1;

		pSurfaceLevels[a].pImageData = pDataPointer;
		pSurfaceLevels[a].bufferLength = (pTexture->pSurfaces[a].width*pTexture->pSurfaces[a].height * MFTexture_GetBitsPerPixel(targetFormat)) / 8;
		pDataPointer += pSurfaceLevels[a].bufferLength;

		uint32 paletteBytes = 0;

		if(targetFormat == TexFmt_I8)
			paletteBytes = 4*256;
		if(targetFormat == TexFmt_I4)
			paletteBytes = 4*16;

		if(paletteBytes)
		{
			pSurfaceLevels[a].pImageData = pDataPointer;
			pSurfaceLevels[a].paletteBufferLength = paletteBytes;
			pDataPointer += paletteBytes;
		}

		// convert surface
		ConvertSurface(&pTexture->pSurfaces[a], &pSurfaceLevels[a], targetFormat, platform);
	}

	// fix up pointers
	for(int a=0; a<pTemplate->mipLevels; a++)
	{
		MFFixUp(pTemplate->pSurfaces[a].pImageData, pOutputBuffer, 0);
		MFFixUp(pTemplate->pSurfaces[a].pPaletteEntries, pOutputBuffer, 0);
	}
	MFFixUp(pTemplate->pSurfaces, pOutputBuffer, 0);

	*ppTemplateData = (MFTextureTemplateData*)pOutputBuffer;
	if(pSize)
		*pSize = imageBytes;
}
*/
