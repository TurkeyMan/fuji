#if !defined(_FONT_H)
#define _FONT_H

#include "MFMaterial.h"
#include "Vector3.h"

#if defined(DrawText)
#undef DrawText
#endif

struct Font
{
	MFMaterial *pMaterial;
	uint8 charwidths[256];
};

void Font_InitModule();
void Font_DeinitModule();

// member functions
Font* Font_Create(const char *filename);
void Font_Destroy(Font *pFont);

int Font_DrawText(Font *pFont, float pos_x, float pos_y, float height, const Vector4 &colour, const char *text, bool invert = false);
int Font_DrawText(Font *pFont, float pos_x, float pos_y, float pos_z, float height, const Vector4 &colour, const char *text, bool invert = false);
int Font_DrawText(Font *pFont, const Vector3 &pos, float height, const Vector4 &colour, const char *text, bool invert = false);

int Font_DrawTextf(Font *pFont, float pos_x, float pos_y, float height, const Vector4 &colour, const char *format, ...);
int Font_DrawTextf(Font *pFont, float pos_x, float pos_y, float pos_z, float height, const Vector4 &colour, const char *format, ...);
int Font_DrawTextf(Font *pFont, const Vector3 &pos, float height, const Vector4 &colour, const char *format, ...);

extern Font *gpDebugFont;

#endif // _FONT_H
