#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#if defined(_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include "DebugMenu.h"

struct Texture;

// functions
void Texture_InitModule();
void Texture_DeinitModule();

// interface functions
Texture* Texture_Create(const char *pName, bool generateMipChain = true);
Texture* Texture_CreateFromRawData(const char *pName, void *pData, int width, int height, int format, uint32 flags = 0, bool generateMipChain = true, uint32 *pPalette = 0);
Texture* Texture_CreateRenderTarget(const char *pName, int width, int height);

int Texture_Destroy(Texture *pTexture);	// returns new reference count..

// platform independant functions
Texture* Texture_FindTexture(const char *pName);
Texture* Texture_CreateBlank(const char *pName, const Vector4 &colour);

// this is TEMPORARY! and must die soon!
void Texture_UseNone();

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

// texture structure
struct Texture
{
	// data members
	char name[64];

	int refCount;
	int width, height;

#if defined(_XBOX)
	IDirect3DTexture8 *pTexture;
	D3DFORMAT format;
#elif defined(_WINDOWS)
	IDirect3DTexture9 *pTexture;
	D3DFORMAT format;
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

#endif // _TEXTURE_H
