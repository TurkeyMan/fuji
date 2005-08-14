#include "Common.h"
#include "Display.h"
#include "MFFileSystem.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Font.h"
#include "Primitive.h"
#include "MFFileSystem.h"

#include "Materials/Mat_Standard.h"

#include <string.h>
#include <stdio.h>

Font *gpDebugFont;

void Font_InitModule()
{
	CALLSTACK;

	gpDebugFont = Font_Create("Arial");
}

void Font_DeinitModule()
{
	CALLSTACK;

	Font_Destroy(gpDebugFont);
}

Font* Font_Create(const char *pFilename)
{
	CALLSTACK;

	Font *pFont = NULL;

	MFFile* hFile = MFFileSystem_Open(STR("%s.dat", pFilename));
	DBGASSERT(hFile, STR("Unable to open charinfo file for font '%s'", pFilename));

	if(hFile)
	{
		pFont = (Font*)Heap_Alloc(sizeof(Font));

		MFFile_Read(hFile, pFont->charwidths, 256);
		MFFile_Close(hFile);

		pFont->pMaterial = MFMaterial_Create(pFilename);
		DBGASSERT(!strcmp(pFont->pMaterial->pType->pTypeName, "Standard"), "Fonts MUST be created from a 'Standard' material.");
	}

	return pFont;
}

void Font_Destroy(Font *pFont)
{
	CALLSTACK;

	MFMaterial_Destroy(pFont->pMaterial);
	Heap_Free(pFont);
}

int Font_DrawText(Font *pFont, float pos_x, float pos_y, float pos_z, float height, const MFVector &colour, const char *text, bool invert)
{
	CALLSTACK;

	Mat_Standard_Data *pData = (Mat_Standard_Data*)pFont->pMaterial->pInstanceData;

	int textlen = strlen(text);

	DBGASSERT(textlen < 2048, "Exceeded Font Vertex Buffer Limit");

	float x,y,w,h, p, cwidth;

	MFMaterial_SetMaterial(pFont->pMaterial);
	MFPrimitive(PT_TriList|PT_Prelit);

	MFBegin(textlen*2*3);

	MFSetColour(colour);

	float texWidth = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].width;
	float texHeight = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].height;

	for(int i=0; i<textlen; i++)
	{
		x = (float)((uint8)text[i] & 0x0F) * (float)(pData->pTextures[0]->pTemplateData->pSurfaces[0].width / 16);
		y = (float)((uint8)text[i] >> 4) * (float)(pData->pTextures[0]->pTemplateData->pSurfaces[0].height / 16);

		w = (float)pFont->charwidths[(uint8)text[i]];
		h = texHeight*(1.0f/16.0f);

		x /= texWidth;
		y /= texHeight;
		w /= texWidth;
		h /= texHeight;

		p = w/h;
		cwidth = height*p;

		MFSetTexCoord1(x, y);
		MFSetPosition(pos_x, pos_y, pos_z);
		MFSetTexCoord1(x+w, y+h);
		MFSetPosition(pos_x + cwidth, pos_y + (invert?-height:height), pos_z);
		MFSetTexCoord1(x, y+h);
		MFSetPosition(pos_x, pos_y + (invert?-height:height), pos_z);
		MFSetTexCoord1(x, y);
		MFSetPosition(pos_x, pos_y, pos_z);
		MFSetTexCoord1(x+w, y);
		MFSetPosition(pos_x + cwidth, pos_y, pos_z);
		MFSetTexCoord1(x+w, y+h);
		MFSetPosition(pos_x + cwidth, pos_y + (invert?-height:height), pos_z);

		pos_x += cwidth;
	}

	MFEnd();

	return 0;
}

int Font_DrawText(Font *pFont, float pos_x, float pos_y, float height, const MFVector &colour, const char *text, bool invert)
{
	return Font_DrawText(pFont, pos_x, pos_y, 0, height, colour, text, invert);
}

int Font_DrawText(Font *pFont, const MFVector &pos, float height, const MFVector &colour, const char *text, bool invert)
{
	return Font_DrawText(pFont, pos.x, pos.y, pos.z, height, colour, text, invert);
}

int Font_DrawTextf(Font *pFont, float pos_x, float pos_y, float height, const MFVector &colour, const char *format, ...)
{
	CALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, format);

	vsprintf(buffer, format, args);

	return Font_DrawText(pFont, pos_x, pos_y, 0, height, colour, buffer);
}

int Font_DrawTextf(Font *pFont, float pos_x, float pos_y, float pos_z, float height, const MFVector &colour, const char *format, ...)
{
	CALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, format);

	vsprintf(buffer, format, args);

	return Font_DrawText(pFont, pos_x, pos_y, pos_z, height, colour, buffer);
}

int Font_DrawTextf(Font *pFont, const MFVector &pos, float height, const MFVector &colour, const char *format, ...)
{
	CALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, format);

	vsprintf(buffer, format, args);

	return Font_DrawText(pFont, pos.x, pos.y, pos.z, height, colour, buffer);
}
