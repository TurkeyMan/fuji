#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#if defined(_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include "DebugMenu.h"

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

void Texture_InitModule();
void Texture_DeinitModule();

class Texture
{
public:
	// member functions
	static Texture* LoadTexture(const char *filename, bool generateMipChain = true);
	void Release();
	void SetTexture(int texUnit = 0);

	static void UseNone(int texUnit = 0);

	// data members
	char name[64];

	int refCount;
	int width, height;

#if defined(_XBOX)
	IDirect3DTexture8 *texture;
	D3DFORMAT format;
#elif defined(_WINDOWS)
	IDirect3DTexture9 *texture;
	D3DFORMAT format;
#elif defined(_LINUX)
	GLuint textureID;
#else
	// nothing
#endif
};

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

Texture *FindTexture(const char *pName);

Texture *Texture_CreateFromRawData(void *pData, uint32 width, uint32 height, uint32 format, uint32 flags = 0, uint32 *pPalette = 0);
Texture *Texture_CreateBlank(const Vector4 &colour, uint32 width, uint32 height, uint32 format);

extern PtrList<Texture> gTextureBank;


#endif // _TEXTURE_H
