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


// globals

MFFont *gpDebugFont;

// foreward declarations

float MFFont_GetStringWidthW(MFFont *pFont, const uint16 *pText, float height, int maxLen, float *pTotalHeight);
int MFFont_DrawTextW(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const uint16 *pText, int maxChars);


// font functions

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

	// load font file
	MFFont *pFont = (MFFont*)MFFileSystem_Load(MFStr("%s.fft", pFilename));
	MFDebug_Assert(pFont, MFStr("Unable to load font '%s'", pFilename));

	// fixup pointers
	(char*&)pFont->pName += (uint32&)pFont;
	(char*&)pFont->pCharacterMapping += (uint32&)pFont;
	(char*&)pFont->pChars += (uint32&)pFont;
	(char*&)pFont->ppPages += (uint32&)pFont;

	for(int a=0; a<pFont->numPages; a++)
	{
		(char*&)pFont->ppPages[a] += (uint32&)pFont;
	}

	// create materials
	for(int a=0; a<pFont->numPages; a++)
	{
		pFont->ppPages[a] = MFMaterial_Create((const char *)pFont->ppPages[a]);
	}

	// set space width
	int id = pFont->pCharacterMapping[' '];
	pFont->spaceWidth = pFont->pChars[id].xadvance;

	return pFont;
}

void MFFont_Destroy(MFFont *pFont)
{
	MFCALLSTACK;

	// destroy materials
	for(int a=0; a<pFont->numPages; a++)
	{
		MFMaterial_Destroy(pFont->ppPages[a]);
	}

	// destroy font
	MFHeap_Free(pFont);
}

MFFont* MFFont_GetDebugFont()
{
	return gpDebugFont;
}

float MFFont_GetFontHeight(MFFont *pFont)
{
	return (float)pFont->height;
}

float MFFont_GetCharacterWidth(MFFont *pFont, int character, float scale)
{
	int id = pFont->pCharacterMapping[character];
	return (float)pFont->pChars[id].xadvance * scale;
}

float MFFont_GetStringWidth(MFFont *pFont, const char *pText, float height, int maxLen, float *pTotalHeight)
{
	if(*(uint16*)pText == 0xFEFF)
		return MFFont_GetStringWidthW(pFont, (uint16*)pText, height, maxLen, pTotalHeight);

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
				width += pFont->spaceWidth*scale;
				break;
			default:
			{
				int id = pFont->pCharacterMapping[(int)*pText];
				width += (float)pFont->pChars[id].xadvance * scale;
				break;
			}
		}

		++pText;
		--maxLen;
	}

	totalWidth = MFMax(width, totalWidth);

	if(pTotalHeight)
		*pTotalHeight = totalHeight + height;

	return totalWidth;
}

static int GetRenderableLength(MFFont *pFont, const char *pText, int *pTotal, int page, int *pNextPage, int maxLen)
{
	if(page == -1)
		return 0;

	int count = 0, renderable = 0;

	*pNextPage = 99999;

	while(*pText && maxLen)
	{
		if(*pText > 32)
		{
			MFDebug_Assert(*pText < pFont->maxMapping, "Character is unavailable in this font!");

			int id = pFont->pCharacterMapping[(int)*pText];
			int p = pFont->pChars[id].page;
			if(p == page)
				++renderable;
			else
			{
				if(p > page)
					*pNextPage = MFMin(*pNextPage, p);
			}
		}
		++count;
		++pText;
		--maxLen;
	}

	if(*pNextPage == 99999)
		*pNextPage = -1;

	*pTotal = count;

	return renderable;
}

int MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText, int maxChars)
{
	MFCALLSTACK;

	if(*(uint16*)pText == 0xFEFF)
		return MFFont_DrawTextW(pFont, pos, height, colour, (uint16*)pText, maxChars);

	int page = 0;
	int nextPage = -1;

	// get the number of renderable chars for this page
	int renderable, textlen;
	renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage, maxChars);

	// HACK: if there was none renderable on the first page, we better find a page where there is...
	if(renderable == 0 && nextPage != -1)
	{
		page = nextPage;
		renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage, maxChars);
	}

	float &uScale = pFont->xScale;
	float &vScale = pFont->yScale;

	float scale = height / pFont->height;

	while(renderable)
	{
		float x,y,x2,y2,px,py;

		MFMaterial_SetMaterial(pFont->ppPages[page]);
		MFPrimitive(PT_QuadList|PT_Prelit);

		MFBegin(renderable*2);

		MFSetColour(colour);

		float pos_x = pos.x;
		float pos_y = pos.y;
		float pos_z = pos.z;

		textlen = maxChars < 0 ? textlen : MFMin(textlen, maxChars);

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
						pos_x += pFont->spaceWidth * scale;
						break;
				}
			}
			else
			{
				int id = pFont->pCharacterMapping[c];
				MFFontChar &ch = pFont->pChars[id];

				if(ch.page == page)
				{
					x = (float)ch.x * uScale;
					y = (float)ch.y * vScale;
					x2 = x + (float)(ch.width) * uScale;
					y2 = y + (float)(ch.height) * vScale;

					px = pos_x + (float)ch.xoffset*scale;
					py = pos_y + (float)ch.yoffset*scale;

					MFSetTexCoord1(x, y);
					MFSetPosition(px, py, pos_z);
					MFSetTexCoord1(x2, y2);
					MFSetPosition(px + (float)ch.width*scale, py + (float)ch.height*scale, pos_z);
				}

				pos_x += (float)ch.xadvance * scale;
			}
		}

		MFEnd();

		page = nextPage;
		renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage, maxChars);
	}

	return 0;
}

float MFFont_GetStringWidthW(MFFont *pFont, const uint16 *pText, float height, int maxLen, float *pTotalHeight)
{
	float scale = height / pFont->height;
	float width = 0.0f;
	float totalWidth = 0.0f;
	float totalHeight = 0.0f;

	if(*pText == 0xFEFF)
		++pText;

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
				width += pFont->spaceWidth*scale;
				break;
			default:
			{
				int id = pFont->pCharacterMapping[(int)*pText];
				width += (float)pFont->pChars[id].xadvance * scale;
				break;
			}
		}

		++pText;
		--maxLen;
	}

	totalWidth = MFMax(width, totalWidth);

	if(pTotalHeight)
		*pTotalHeight = totalHeight + height;

	return totalWidth;
}

static int GetRenderableLengthW(MFFont *pFont, const uint16 *pText, int *pTotal, int page, int *pNextPage, int maxLen)
{
	if(page == -1)
		return 0;

	int count = 0, renderable = 0;

	*pNextPage = 99999;

	while(*pText && maxLen)
	{
		if(*pText > 32)
		{
			int id = pFont->pCharacterMapping[(int)*pText];
			int p = pFont->pChars[id].page;
			if(p == page)
				++renderable;
			else
			{
				if(p > page)
					*pNextPage = MFMin(*pNextPage, p);
			}
		}
		++count;
		++pText;
		--maxLen;
	}

	if(*pNextPage == 99999)
		*pNextPage = -1;

	*pTotal = count;

	return renderable;
}

int MFFont_DrawTextW(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const uint16 *pText, int maxChars)
{
	MFCALLSTACK;

	int page = 0;
	int nextPage = -1;

	if(*pText == 0xFEFF)
		++pText;

	// get the number of renderable chars for this page
	int renderable, textlen;
	renderable = GetRenderableLengthW(pFont, pText, &textlen, page, &nextPage, maxChars);

	// HACK: if there was none renderable on the first page, we better find a page where there is...
	if(renderable == 0 && nextPage != -1)
	{
		page = nextPage;
		renderable = GetRenderableLengthW(pFont, pText, &textlen, page, &nextPage, maxChars);
	}

	float &uScale = pFont->xScale;
	float &vScale = pFont->yScale;

	float scale = height / pFont->height;

	while(renderable)
	{
		float x,y,x2,y2,px,py;

		MFMaterial_SetMaterial(pFont->ppPages[page]);
		MFPrimitive(PT_QuadList|PT_Prelit);

		MFBegin(renderable*2);

		MFSetColour(colour);

		float pos_x = pos.x;
		float pos_y = pos.y;
		float pos_z = pos.z;

		textlen = maxChars < 0 ? textlen : MFMin(textlen, maxChars);

		for(int i=0; i<textlen; i++)
		{
			uint16 c = pText[i];

			if(c <= 32)
			{
				switch(pText[i])
				{
					case '\n':
						pos_x = pos.x;
						pos_y += height;
						break;
					case ' ':
						pos_x += pFont->spaceWidth * scale;
						break;
				}
			}
			else
			{
				int id = pFont->pCharacterMapping[c];
				MFFontChar &ch = pFont->pChars[id];

				if(ch.page == page)
				{
					x = (float)ch.x * uScale;
					y = (float)ch.y * vScale;
					x2 = x + (float)(ch.width) * uScale;
					y2 = y + (float)(ch.height) * vScale;

					px = pos_x + (float)ch.xoffset*scale;
					py = pos_y + (float)ch.yoffset*scale;

					MFSetTexCoord1(x, y);
					MFSetPosition(px, py, pos_z);
					MFSetTexCoord1(x2, y2);
					MFSetPosition(px + (float)ch.width*scale, py + (float)ch.height*scale, pos_z);
				}

				pos_x += (float)ch.xadvance * scale;
			}
		}

		MFEnd();

		page = nextPage;
		renderable = GetRenderableLengthW(pFont, pText, &textlen, page, &nextPage, maxChars);
	}

	return 0;
}

int MFFont_DrawText(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pText, int maxChars)
{
	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, pText, maxChars);
}

int MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, pos, height, colour, buffer, -1);
}

int MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, buffer, -1);
}
