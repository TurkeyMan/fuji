#if !defined(_FONT_H)
#define _FONT_H

#include "Texture.h"

class Font
{
public:
	// member functions
	int LoadFont(char *filename);
	void Release();

	int DrawText(float pos_x, float pos_y, float height, uint32 colour, char *text, bool invert = false);
	int DrawText(float pos_x, float pos_y, float pos_z, float height, uint32 colour, char *text, bool invert = false);

	int DrawTextf(float pos_x, float pos_y, float height, uint32 colour, char *format, ...);
	int DrawTextf(float pos_x, float pos_y, float pos_z, float height, uint32 colour, char *format, ...);

	// data members
	Texture texture;
	uint8 charwidths[256];
};

#endif // _FONT_H
