#include "Fuji.h"
#include "MFHeap.h"
#include "Display.h"
#include "MFFileSystem.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive.h"
#include "MFFileSystem.h"

#include "Materials/MFMat_Standard.h"

#include <string.h>
#include <stdio.h>

MFFont *gpDebugFont;

void MFFont_InitModule()
{
	MFCALLSTACK;

	gpDebugFont = MFFont_Create("Arial");
}

void MFFont_DeinitModule()
{
	MFCALLSTACK;

	MFFont_Destroy(gpDebugFont);
}

MFFont* MFFont_Create(const char *pFilename)
{
	MFCALLSTACK;

	MFFont *pFont = NULL;

	MFFile* hFile = MFFileSystem_Open(MFStr("%s.dat", pFilename));
	MFDebug_Assert(hFile, MFStr("Unable to open charinfo file for font '%s'", pFilename));

	if(hFile)
	{
		pFont = (MFFont*)MFHeap_Alloc(sizeof(MFFont));

		MFFile_Read(hFile, pFont->charwidths, 256);
		MFFile_Close(hFile);

		pFont->pMaterial = MFMaterial_Create(pFilename);
		MFDebug_Assert(!MFString_Compare(pFont->pMaterial->pType->pTypeName, "Standard"), "Fonts MUST be created from a 'Standard' material.");

		MFTexture *pTexture;
		MFMaterial_GetParamater(pFont->pMaterial, MFMatStandard_DifuseMap, 0, &pTexture);

		pFont->height = pTexture->pTemplateData->pSurfaces[0].height / 16.0f;
		pFont->spaceWidth = (float)pFont->charwidths[(int)' '];
	}

	return pFont;
}

void MFFont_Destroy(MFFont *pFont)
{
	MFCALLSTACK;

	MFMaterial_Destroy(pFont->pMaterial);
	MFHeap_Free(pFont);
}

MFFont* MFFont_GetDebugFont()
{
	return gpDebugFont;
}

float MFFont_GetFontHeight(MFFont *pFont)
{
	return pFont->height;
}

float MFFont_GetCharacterWidth(MFFont *pFont, int character, float scale, int fontPage)
{
	return (float)pFont->charwidths[character] * scale;
}

float MFFont_GetStringWidth(MFFont *pFont, const char *pText, float height, int maxLen, float *pTotalHeight)
{
	float scale = height / pFont->height;
	float width = 0.0f;
	float totalWidth = 0.0f;
	float totalHeight = 0.0f;

	while(*pText && maxLen)
	{
		switch(*pText)
		{
			case '\n':
				totalWidth = MFMax(width, totalWidth);
				width = 0.0f;
				totalHeight += height;
				break;
			case ' ':
				width += pFont->spaceWidth * scale;
				break;
			default:
				width += (float)pFont->charwidths[(int)*pText] * scale;
				break;
		}

		++pText;
		--maxLen;
	}

	totalWidth = MFMax(width, totalWidth);

	if(pTotalHeight)
		*pTotalHeight = totalHeight + height;

	return totalWidth;
}

static int GetRenderableLength(const char *pText, int *pTotal)
{
	int count = 0, renderable = 0;

	while(*pText)
	{
		if(*pText > 32)
			++renderable;
		++count;
		++pText;
	}

	*pTotal = count;

	return renderable;
}

int MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pFont->pMaterial->pInstanceData;

	int renderable, textlen;
	renderable = GetRenderableLength(pText, &textlen);

	if(renderable == 0)
		return 0;

	float x,y,w,h, p, cwidth;

	MFMaterial_SetMaterial(pFont->pMaterial);
	MFPrimitive(PT_QuadList|PT_Prelit);

	MFBegin(renderable*2);

	MFSetColour(colour);

	float texWidth = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].width;
	float texHeight = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].height;
	float cellWidth = texWidth / 16.0f;
	float cellHeight = texHeight / 16.0f;
	float uScale = 1.0f / texWidth;
	float vScale = 1.0f / texHeight;

	float pos_x = pos.x;
	float pos_y = pos.y;
	float pos_z = pos.z;

	h = cellHeight * vScale;

	for(int i=0; i<textlen; i++)
	{
		uint8 c = pText[i];

		if(c <= 32)
		{
			switch(pText[i])
			{
				case '\n':
					pos_x = pos.x;
					pos_y += height;
					break;
				case ' ':
					pos_x += height * ((pFont->spaceWidth * uScale)/h);
					break;
			}
		}
		else
		{
			x = (float)(c & 0x0F) * cellWidth * uScale;
			y = (float)(c >> 4) * cellWidth * vScale;
			w = (float)pFont->charwidths[c] * uScale;

			p = w/h;
			cwidth = height*p;

			MFSetTexCoord1(x, y);
			MFSetPosition(pos_x, pos_y, pos_z);
			MFSetTexCoord1(x+w, y+h);
			MFSetPosition(pos_x + cwidth, pos_y + height, pos_z);

			pos_x += cwidth;
		}
	}

	MFEnd();

	return 0;
}

int MFFont_DrawText(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pText)
{
	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, pText);
}

int MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, pos, height, colour, buffer);
}

int MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, buffer);
}
