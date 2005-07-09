#if !defined(_TEXTURE_INTERNAL_H)
#define _TEXTURE_INTERNAL_H

#if defined(_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#endif

#include "DebugMenu_Internal.h"
#include "Texture.h"

// functions
void Texture_InitModule();
void Texture_DeinitModule();

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

#endif
