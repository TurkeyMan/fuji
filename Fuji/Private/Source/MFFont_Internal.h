#if !defined(_MFFONT_INTERNAL_H)
#define _MFFONT_INTERNAL_H

#include "MFFont.h"
#include "MFMaterial.h"

enum MFFontFlags
{
	MFFF_Bold = 1,
	MFFF_Italic = 2,
	MFFF_Unicode = 4,
	MFFF_Smooth = 8
};

struct MFFontChar
{
	uint16 id;
	uint16 x, y;
	uint16 width, height;
	uint16 xadvance;
	int8 xoffset, yoffset;
	uint8 page;
	uint8 channel;
};

struct MFFont
{
	const char *pName;

	int size;
	int height;
	int base;
	int spaceWidth;
	float xScale, yScale;
	uint32 flags;

	MFFontChar *pChars;
	int numChars;

	uint16 *pCharacterMapping;
	int maxMapping;

	MFMaterial **ppPages;
	int numPages;
};

void MFFont_InitModule();
void MFFont_DeinitModule();

#endif // _MFFONT_INTERNAL_H
