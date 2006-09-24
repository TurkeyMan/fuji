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

static int GetRenderableLength(MFFont *pFont, const char *pText, int *pTotal, int page, int *pNextPage)
{
	if(page == -1)
		return 0;

	int count = 0, renderable = 0;

	*pNextPage = -1;

	while(*pText)
	{
		if(*pText > 32)
		{
			int id = pFont->pCharacterMapping[(int)*pText];
			if(pFont->pChars[id].page == page)
				++renderable;
		}
		++count;
		++pText;
	}

	*pTotal = count;

	return renderable;
}

int MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText)
{
	MFCALLSTACK;

	int page = 0;
	int nextPage = -1;

	// get the number of renderable chars for this page
	int renderable, textlen;
	renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage);

	// HACK: if there was none renderable on the first page, we better find a page where there is...
	if(renderable == 0 && nextPage != -1)
		renderable = GetRenderableLength(pFont, pText, &textlen, nextPage, &nextPage);

	float &uScale = pFont->xScale;
	float &vScale = pFont->yScale;
	float halfU = uScale * 0.5f;
	float halfV = vScale * 0.5f;

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
		renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage);
	}

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
