/**** Defines ****/

/**** Includes ****/

#include "Common.h"
#include "Texture.h"
#include "Display.h"

/**** Globals ****/

/**** Functions ****/

Texture* Texture_CreateFromRawData(void *pData, uint32 width, uint32 height, uint32 format, uint32 flags, uint32 *pPalette)
{
	return NULL;
}

Texture* Texture_CreateBlank(const Vector4 &colour, uint32 width, uint32 height, uint32 format)
{
	return NULL;
}

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	Texture *pTexture = FindTexture(filename);

	if(!pTexture)
	{
		pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
		pTexture->refCount = 0;
	}

	if(!pTexture->refCount)
	{
		return NULL;
	}

	pTexture->refCount++;

	return pTexture;
}

void Texture::Release()
{
	refCount--;

	if(!refCount)
	{
		// destroy
	}
}

void Texture::SetTexture(int texUnit)
{

}

void Texture::UseNone(int texUnit)
{

}

