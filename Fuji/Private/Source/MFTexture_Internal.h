#if !defined(_TEXTURE_INTERNAL_H)
#define _TEXTURE_INTERNAL_H

#if defined(_WINDOWS)
#include <d3d9.h>
#endif

#include "MFTexture.h"

// foreward declarations
struct MFTexture;
struct MFTextureSurfaceLevel;

// functions
void MFTexture_InitModule();
void MFTexture_DeinitModule();

void MFTexture_CreatePlatformSpecific(MFTexture *pTexture, bool generateMipChain);

// texture TemplateData
struct MFTextureTemplateData
{
	uint32 magicNumber;

	MFTextureFormat imageFormat;
	uint32 platformFormat;

	int mipLevels;

	// flags
	uint32 alpha	: 1; // 0 = Opaque, 1 = Full Alpha, 3 = 1bit Alpha
	uint32 swizzled	: 1; // 0 = Linear, 1 = Swizzled
	uint32 flags	: 29;

	// padding
	uint32 res[2];

	MFTextureSurfaceLevel *pSurfaces;
};

struct MFTextureSurfaceLevel
{
	int width, height;
	int bitsPerPixel;

	int xBlocks, yBlocks;
	int bitsPerBlock;

	char *pImageData;
	int bufferLength;

	char *pPaletteEntries;
	int paletteBufferLength;

	uint32 res[2];
};

// texture structure
struct MFTexture
{
	// data members
	char name[64];

	MFTextureTemplateData *pTemplateData;
	int refCount;

#if defined(_XBOX)
	IDirect3DTexture8 *pTexture;
#elif defined(_WINDOWS)
	IDirect3DTexture9 *pTexture;
#elif defined(_LINUX)
	GLuint textureID;
#else
	// nothing
#endif
};

#if !defined(_FUJI_UTIL)
// a debug menu texture information display object
#include "DebugMenu_Internal.h"

class TextureBrowser : public MenuObject
{
public:
	TextureBrowser();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, const MFVector &pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual MFVector GetDimensions(float maxWidth);

	int selection;
};
#endif

extern uint32 gMFTexturePlatformFormat[FP_Max][TexFmt_Max];

#endif
