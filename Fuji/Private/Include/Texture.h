#if !defined(_TEXTURE_H)
#define _TEXTURE_H

#include "DebugMenu.h"
#include <map>
#include <string>

void Texture_InitModule();
void Texture_DeinitModule();

class Texture
{
public:
	Texture();

	// member functions
	static Texture* LoadTexture(const char *filename, bool generateMipChain = true);
	void Release();
	void SetTexture();

	static void UseNone();

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

	virtual float ListDraw(bool selected, Vector3 pos, float maxWidth);
	virtual void ListUpdate(bool selected);
	virtual Vector3 GetDimensions(float maxWidth);

	int selection;
};

extern std::map<std::string, Texture> gTextureBank;


#endif // _TEXTURE_H
