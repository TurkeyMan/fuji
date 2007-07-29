#if !defined(_TEXTURE_INTERNAL_H)
#define _TEXTURE_INTERNAL_H

#if MF_RENDERER == MF_DRIVER_D3D9
	#include <d3d9.h>
#elif MF_RENDERER == MF_DRIVER_OPENGL
	#if defined(MF_WINDOWS)
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
	#include <GL/gl.h>
#endif

#include "MFTexture.h"

// we need a list of all available display drivers
enum MFDisplayDrivers
{
	MFDD_D3D9,
	MFDD_XBox,
	MFDD_OpenGL,
	MFDD_PSP,
	MFDD_PS2,

	MFDD_Max,
	MFDD_ForceInt = 0x7FFFFFFF
};

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

	uint32 flags;

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

#if MF_RENDERER == MF_DRIVER_XBOX
#if defined(XB_XGTEXTURES)
	IDirect3DTexture8 texture;
#endif
	IDirect3DTexture8 *pTexture;
#elif MF_RENDERER == MF_DRIVER_D3D9
	IDirect3DTexture9 *pTexture;
#elif MF_RENDERER == MF_DRIVER_OPENGL
	GLuint textureID;
#elif MF_RENDERER == MF_DRIVER_PS2
	unsigned int vramAddr;
#else
	// nothing
#endif

	MFTextureTemplateData *pTemplateData;
	int refCount;
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

extern uint32 gMFTexturePlatformFormat[MFDD_Max][TexFmt_Max];

#endif
