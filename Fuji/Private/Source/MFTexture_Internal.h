#if !defined(_TEXTURE_INTERNAL_H)
#define _TEXTURE_INTERNAL_H

#if defined(_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include "DebugMenu_Internal.h"
#include "MFTexture.h"

// functions
void MFTexture_InitModule();
void MFTexture_DeinitModule();

// texture TemplateData
struct MFTextureSurfaceLevel;

struct MFTextureTemplateData
{
	uint32 magicNumber;

	MFTextureFormats imageFormat;
	uint32 platformFormat;

	int mipLevels;
	int opaque;

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

// a debug menu texture information display object
class TextureBrowser : public MenuObject
{
public:
	TextureBrowser();

	virtual void Draw();
	virtual void Update();

	virtual float ListDraw(bool selected, const Vector3 &pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	int selection;
};

extern const char * const gpMFTextureFormatStrings[TexFmt_Max];
extern uint32 gMFTextureBitsPerPixel[TexFmt_Max];
extern uint32 gMFTexturePlatformFormat[TexFmt_Max];

#endif
