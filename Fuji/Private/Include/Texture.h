#if !defined(_TEXTURE_H)
#define _TEXTURE_H

class Texture
{
public:
	// member functions
	int LoadTexture(char *filename, bool generateMipChain = true);
	void ReleaseTexture();
	void SetTexture();

	static void UseNone();

	// data members
	int width, height;

#if defined(_XBOX)
	IDirect3DTexture8 *texture;
	D3DFORMAT format;
#elif define(_WINDOWS)
	IDirect3DTexture9 *texture;
	D3DFORMAT format;
#else
	// nothing
#endif
};

#endif // _TEXTURE_H
