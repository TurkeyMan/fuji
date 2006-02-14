#if !defined(_MFFONT_INTERNAL_H)
#define _MFFONT_INTERNAL_H

#include "MFFont.h"

#include "MFMaterial.h"

struct MFFont
{
	MFMaterial *pMaterial;

	float height;
	float spaceWidth;

	uint8 charwidths[256];
};

void MFFont_InitModule();
void MFFont_DeinitModule();

#endif // _MFFONT_INTERNAL_H
