#if !defined(_FONT_H)
#define _FONT_H

#include "Texture.h"
#include "Vector3.h"

class Font
{
public:
	// member functions
	int LoadFont(const char *filename);
	void Release();

	int DrawText(float pos_x, float pos_y, float height, uint32 colour, const char *text, bool invert = false);
	int DrawText(float pos_x, float pos_y, float pos_z, float height, uint32 colour, const char *text, bool invert = false);
	int DrawText(const Vector3 &pos, float height, uint32 colour, const char *text, bool invert = false);

	int DrawTextf(float pos_x, float pos_y, float height, uint32 colour, const char *format, ...);
	int DrawTextf(float pos_x, float pos_y, float pos_z, float height, uint32 colour, const char *format, ...);
	int DrawTextf(const Vector3 &pos, float height, uint32 colour, const char *format, ...);

	// data members
	Texture *pTexture;
	uint8 charwidths[256];
};

void Font_InitModule();
void Font_DeinitModule();

extern Font debugFont;

#endif // _FONT_H
