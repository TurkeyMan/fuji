#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#include "Vector4.h"

struct Texture;

// interface functions
Texture* Texture_Create(const char *pName, bool generateMipChain = true);
Texture* Texture_CreateFromRawData(const char *pName, void *pData, int width, int height, int format, uint32 flags = 0, bool generateMipChain = true, uint32 *pPalette = 0);
Texture* Texture_CreateRenderTarget(const char *pName, int width, int height);

int Texture_Destroy(Texture *pTexture);	// returns new reference count..

// platform independant functions
Texture* Texture_FindTexture(const char *pName);
Texture* Texture_CreateBlank(const char *pName, const Vector4 &colour);

// some enum's
enum TextureFormats
{
	TEXF_Unknown = 0,

	TEXF_A8R8G8B8,
	TEXF_R8G8B8,
	TEXF_R5G6B5,
	TEXF_X1R5G65B5,
	TEXF_A8,

	TEXF_I8,
	TEXF_I4
};

enum CreateTextureFlags
{
	TEX_VerticalMirror = 1
};

#endif // _TEXTURE_H
