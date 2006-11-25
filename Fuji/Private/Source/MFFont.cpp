#include "Fuji.h"
#include "MFHeap.h"
#include "Display.h"
#include "MFFileSystem.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive.h"
#include "MFFileSystem.h"
#include "MFView.h"

#include "Materials/MFMat_Standard.h"

#include <string.h>
#include <stdio.h>


// globals

#define MAX_TEXT_LINES          30           // maximum number of lines we can handle in one draw text

struct TextLine
{
  uint8 *pStartChar, *pEndChar;     // first and last chars (inclusive)
  float length;                     // width in virtual screen coords
};

struct TextLineData
{
  TextLine lines[MAX_TEXT_LINES];
  int nmbrOfLines;
  float fontScale;
  float totalHeight;
};

static uint8 gFontJustify[MFFontJustify_Max] =
{
  0x00, // MFFontJustify_Top_Left,
  0x01, // MFFontJustify_Top_Center,
  0x02, // MFFontJustify_Top_Right,
  0x03, // MFFontJustify_Top_Full,

  0x10, // MFFontJustify_Center_Left,
  0x11, // MFFontJustify_Center,
  0x12, // MFFontJustify_Center_Right,
  0x13, // MFFontJustify_Center_Full,

  0x20, // MFFontJustify_Bottom_Left,
  0x21, // MFFontJustify_Bottom_Center,
  0x22, // MFFontJustify_Bottom_Right,
  0x23, // MFFontJustify_Bottom_Full,
};

static MFFont *gpDebugFont;

struct CharHistory
{
	float width;
	uint16 c;
	uint16 offset;
};

// this is used to quickly calculate word wraping and stuff.
static CharHistory gCharHistory[256];


// foreward declarations

float MFFont_GetStringWidthW(MFFont *pFont, const uint16 *pText, float height, float lineWidth, int maxLen, float *pTotalHeight);
float MFFont_DrawTextW(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const uint16 *pText, int maxChars, const MFMatrix &ltw);


// helper functions

inline bool MFFontInternal_IsValidWrapPoint(int c)
{
  return MFIsWhite(c);
}

inline int GetHorizontalJustification(MFFontJustify justification)
{
  return gFontJustify[justification] & 0xF;
}

inline int GetVerticalJustification(MFFontJustify justification)
{
  return gFontJustify[justification] >> 4;
}

// calculates the height and scale from source data
float MFFontInternal_CalcHeightAndScale(MFFont *pFont, float height, float *pScale)
{
  if (pFont == NULL)
    pFont = gpDebugFont;

  // calculate font height and scale from desired height
  float fontHeight = (float)pFont->height;

  if(height == 0.0f) // use native font height
  {
    height = fontHeight;
    *pScale = 1.0f;
  }
  else
  {
    *pScale = height / fontHeight;
  }

  if(!MFView_IsOrtho())
    height = -height;

  return height;
}


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
	MFFixUp(pFont->pName, pFont, 1);
	MFFixUp(pFont->pCharacterMapping, pFont, 1);
	MFFixUp(pFont->pChars, pFont, 1);
	MFFixUp(pFont->ppPages, pFont, 1);

	for(int a=0; a<pFont->numPages; a++)
	{
		MFFixUp(pFont->ppPages[a], pFont, 1);
	}

	// create materials
	for(int a=0; a<pFont->numPages; a++)
	{
//		MFTexture *pTex = MFTexture_Create((const char *)pFont->ppPages[a], false);
		pFont->ppPages[a] = MFMaterial_Create((const char *)pFont->ppPages[a]);
//		if(pTex) MFTexture_Destroy(pTex);
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
	if(pFont == NULL) pFont = gpDebugFont;

	return (float)pFont->height;
}

float MFFont_GetCharacterWidth(MFFont *pFont, int character)
{
	if(pFont == NULL) pFont = gpDebugFont;

	int id = pFont->pCharacterMapping[character];
	return (float)pFont->pChars[id].xadvance;
}

MFVector MFFont_GetCharPos(MFFont *pFont, const char *pText, int charIndex, float height)
{
	if(pFont == NULL) pFont = gpDebugFont;

	MFVector currentPos = MFVector::zero;
	float scale;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float spaceWidth = pFont->spaceWidth * scale;

	uint16 c;
	int bytes = MFString_MBToWChar(pText, &c);

	while(*pText && charIndex)
	{
		if(bytes <= 0)
		{
			c = *(uint8*)pText;
			++pText;
		}

		switch(c)
		{
			case '\n':
				currentPos.x = 0.0f;
				currentPos.y += height;
				break;
			case ' ':
				currentPos.x += spaceWidth;
				break;
			default:
			{
				int id = pFont->pCharacterMapping[c];
				currentPos.x += (float)pFont->pChars[id].xadvance * scale;
				break;
			}
		}

		if(bytes > 0)
		{
			pText += bytes;
			bytes = MFString_MBToWChar(pText, &c);
		}

		--charIndex;
	}

	return currentPos;
}

float MFFont_GetStringWidth(MFFont *pFont, const char *pText, float height, float lineWidth, int maxLen, float *pTotalHeight)
{
	if(*(const uint16*)pText == 0xFEFF)
		return MFFont_GetStringWidthW(pFont, (const uint16*)pText, height, lineWidth, maxLen, pTotalHeight);

	if(pFont == NULL) pFont = gpDebugFont;

	CharHistory *pH = gCharHistory;
	const char *pLineStart = pText;
	float width = 0.0f;
	float maxWidth = 0.0f;
	float totalHeight = 0.0f;
	float scale;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float spaceWidth = pFont->spaceWidth * scale;

	uint16 c;
	int bytes = MFString_MBToWChar(pText, &c);

	while(*pText && maxLen)
	{
		if(bytes <= 0)
			c = *(uint8*)pText;

		if(c == '\n')
		{
			maxWidth = MFMax(width, maxWidth);
			width = 0.0f;
			totalHeight += height;
			pLineStart = pText + (bytes > 0 ? bytes : 1);
			pH = gCharHistory - 1;
		}
		else
		{
			switch(c)
			{
				case ' ':
					pH->width = spaceWidth;
					width += spaceWidth;
					break;
				default:
				{
					int id = pFont->pCharacterMapping[c];
					pH->width = (float)pFont->pChars[id].xadvance * scale;
					width += pH->width;
					break;
				}
			}

			pH->c = c;
			pH->offset = (uint16)(pText - pLineStart);
		}

		// check for and handle word wrapping
		if(lineWidth > 0.0f && width > lineWidth)
		{
			// we have exceeded a line width

			// here we need to recurse backward to find a valid wrap point and submit to DrawString
			float wrapWidth = width;
			CharHistory *pT = pH;
			while(pT > gCharHistory)
			{
				wrapWidth -= pT->width;

				if(MFFontInternal_IsValidWrapPoint(pT->c) && !MFFontInternal_IsValidWrapPoint(pT[-1].c))
				{
					pText = pLineStart + pT->offset;

					// this is a valid wrap point.. how convenient..
					while(MFFontInternal_IsValidWrapPoint(*pText))
					{
						++pText;
						--maxLen;
					}
					break;
				}

				--pT;
				++maxLen;
			}

			if(pT == gCharHistory)
			{
				// no valid wrap point was found so force wrap at current character
				wrapWidth = width - pH->width;
			}

			// get the maximum width
			maxWidth = MFMax(wrapWidth, maxWidth);

			// bump pos
			width = 0.0f;
			totalHeight += height;

			// update character positions
			pLineStart = pText;
			pH = gCharHistory;
		}
		else
		{
			if(bytes <= 0)
				++pText;
			else
			{
				pText += bytes;
				bytes = MFString_MBToWChar(pText, &c);
			}

			++pH;
			--maxLen;
		}
	}

	maxWidth = MFMax(width, maxWidth);

	if(pTotalHeight)
		*pTotalHeight = totalHeight + height;

	return maxWidth;
}

static int GetRenderableLength(MFFont *pFont, const char *pText, int *pTotal, int page, int *pNextPage, int maxLen)
{
	if(page == -1) return 0;
	if(pFont == NULL) pFont = gpDebugFont;

	int count = 0, renderable = 0;

	*pNextPage = 99999;

	uint16 c;
	int bytes = MFString_MBToWChar(pText, &c);

	while(*pText && maxLen)
	{
		if(bytes <= 0)
		{
			c = *(uint8*)pText;
			++pText;
		}

		if(c > 32)
		{
			MFDebug_Assert(c < pFont->maxMapping, "Character is unavailable in this font!");

			int id = pFont->pCharacterMapping[(int)c];
			int p = pFont->pChars[id].page;
			if(p == page)
				++renderable;
			else
			{
				if(p > page)
					*pNextPage = MFMin(*pNextPage, p);
			}
		}

		if(bytes > 0)
		{
			pText += bytes;
			bytes = MFString_MBToWChar(pText, &c);
		}

		++count;
		--maxLen;
	}

	if(*pNextPage == 99999)
		*pNextPage = -1;

	*pTotal = count;

	return renderable;
}

float MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText, int maxChars, const MFMatrix &ltw)
{
	MFCALLSTACK;

	if(*(const uint16*)pText == 0xFEFF)
		return MFFont_DrawTextW(pFont, pos, height, colour, (const uint16*)pText, maxChars, ltw);

	if(pFont == NULL) pFont = gpDebugFont;

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

	float uScale = pFont->xScale;
	float vScale = pFont->yScale;
	float scale;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float spaceWidth = pFont->spaceWidth * scale;

	float pos_x;
	float pos_y = pos.y;
	float pos_z;

	while(renderable)
	{
		float x,y,x2,y2,px,py;

		MFMaterial_SetMaterial(pFont->ppPages[page]);
		MFPrimitive(PT_QuadList|PT_Prelit);
		MFSetMatrix(ltw);

		MFBegin(renderable*2);

		MFSetColour(colour);

		pos_x = pos.x;
		pos_y = pos.y;
		pos_z = pos.z;

		textlen = maxChars < 0 ? textlen : MFMin(textlen, maxChars);

		const char *pRenderString = pText;

		uint16 c;
		int bytes = MFString_MBToWChar(pRenderString, &c);

		for(int i=0; i<textlen; i++)
		{
			if(bytes <= 0)
			{
				c = *(uint8*)pRenderString;
				++pRenderString;
			}

			if(c <= 32)
			{
				switch(c)
				{
					case '\n':
						pos_x = pos.x;
						pos_y += height;
						break;
					case ' ':
						pos_x += spaceWidth;
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

			if(bytes > 0)
			{
				pRenderString += bytes;
				bytes = MFString_MBToWChar(pRenderString, &c);
			}
		}

		MFEnd();

		page = nextPage;
		renderable = GetRenderableLength(pFont, pText, &textlen, page, &nextPage, maxChars);
	}

	return (pos_y-pos.y) + height;
}

float MFFont_DrawText(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pText, int maxChars, const MFMatrix &ltw)
{
	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, pText, maxChars, ltw);
}

float MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, pos, height, colour, buffer, -1);
}

float MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...)
{
	MFCALLSTACK;

	char buffer[1024];

	va_list args;
	va_start(args, pFormat);

	vsprintf(buffer, pFormat, args);

	return MFFont_DrawText(pFont, MakeVector(x, y, 0.0f), height, colour, buffer, -1);
}


//////////////////////////////
// word wrapping functions...

int MFFont_GetNextWrapPoint(MFFont *pFont, const char *pText, float lineWidth, float height, int *pLastSignificantCharacter)
{
	CharHistory *pH = gCharHistory;
	const char *pC = pText;
	const char *pLineStart = pText;
	float currentPos = 0.0f;
	float scale;
	int numChars = 0;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float spaceWidth = pFont->spaceWidth * scale;

	uint16 c;
	int bytes = MFString_MBToWChar(pC, &c);

	while(*pC)
	{
		if(bytes <= 0)
			c = *(uint8*)pC;

		if(c == '\n')
		{
			currentPos = 0.0f;
			pLineStart = pLineStart = pC + (bytes > 0 ? bytes : 1);
			pH = gCharHistory - 1;
			break;
		}
		else
		{
			if(c == ' ')
			{
				currentPos += spaceWidth;
				pH->width = spaceWidth;
			}
			else
			{
				int id = pFont->pCharacterMapping[c];
				pH->width = (float)pFont->pChars[id].xadvance * scale;
				currentPos += pH->width;
			}

			pH->c = c;
			pH->offset = (uint16)(pC - pLineStart);
		}

		if(lineWidth > 0.0f && currentPos > lineWidth)
		{
			// we have exceeded a line width

			// here we need to recurse backward to find a valid wrap point and submit to DrawString
			CharHistory *pT = pH;
			while(pT > gCharHistory)
			{
				if(MFFontInternal_IsValidWrapPoint(pT->c))
				{
					// this is a valid wrap point.. how convenient..
					pC = pLineStart + pT->offset;
					break;
				}

				--pT;
			}

			while(MFFontInternal_IsValidWrapPoint(*pC))
				pC = MFString_NextChar(pC);

			int offset = MFMax(pC - pText, MFString_GetNumBytesInMBChar(pText));

			if(pT == gCharHistory)
			{
				// no valid wrap point was found so force wrap at current character
				pC = pText + offset;
			}

			break;
		}

		if(bytes > 0)
		{
			pC += bytes;
			bytes = MFString_MBToWChar(pC, &c);
		}
		else
			++pC;

		++pH;
	}

	int charOffset = pC - pText;

	if(pLastSignificantCharacter)
	{
		*pLastSignificantCharacter = charOffset;

		while(MFIsWhite(pText[*pLastSignificantCharacter-1]) && *pLastSignificantCharacter > 0)
		{
			--*pLastSignificantCharacter;
		}
	}

	return charOffset;
}

#define _WRAP_EXCLUDES_TRAILING_WHITESPACE

MFVector MFFont_GetCharPosJustified(MFFont *pFont, const char *pText, float textHeight, int charIndex, float boxWidth, float boxHeight, MFFontJustify justification)
{
  const char *pCurrentLine = pText;

  MFVector currentPos = MFVector::zero;
  float scale;
  int lineEnd, lastSignificantChar;

  // calculate height and scale
  float height = MFFontInternal_CalcHeightAndScale(pFont, textHeight, &scale);

  // justify vertically (this is slow)
  if(justification > MFFontJustify_Top_Full)
  {
    // find the initial y offset
    float totalHeight = 0.0f;

    // calculate total height
    while(*pCurrentLine)
    {
      lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, boxWidth, textHeight, &lastSignificantChar);

      totalHeight += height;
      pCurrentLine += lineEnd;
    }

    // justify according to justification mode
    if(justification != MFFontJustify_Center_Left && justification != MFFontJustify_Center && justification != MFFontJustify_Center_Right)
    {
      // bottom justified
      currentPos.y = boxHeight - totalHeight;
    }
    else
    {
      // center justified
      currentPos.y = boxHeight*0.5f - totalHeight*0.5f;
    }

    pCurrentLine = pText;
  }

  // process each line
  while(*pCurrentLine)
  {
    // get next line
    lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, boxWidth, textHeight, &lastSignificantChar);

    if(charIndex < lineEnd || !pCurrentLine[lineEnd])
    {
      // if we are not left justified, we need to calculate the x offset
      if(justification != MFFontJustify_Top_Left && justification != MFFontJustify_Center_Left && justification != MFFontJustify_Bottom_Left)
      {
#if defined(_WRAP_EXCLUDES_TRAILING_WHITESPACE)
        float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, boxWidth, lastSignificantChar);
#else
        float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, boxWidth, lineEnd);
#endif

        if(justification != MFFontJustify_Top_Center && justification != MFFontJustify_Center && justification != MFFontJustify_Bottom_Center)
        {
          // right justified
          currentPos.x = boxWidth - width;
        }
        else
        {
          // center justified
          currentPos.x = boxWidth*0.5f - width*0.5f;
        }
      }

#if defined(_WRAP_EXCLUDES_TRAILING_WHITESPACE)
      currentPos += MFFont_GetCharPos(pFont, pCurrentLine, MFMin(lastSignificantChar, charIndex), textHeight);
#else
      currentPos += MFFont_GetCharPos(pFont, pCurrentLine, charIndex, textHeight);
#endif

      break;
    }

    // increment height
    currentPos.y += height;

    // and increment string
    pCurrentLine += lineEnd;
    charIndex -= lineEnd;
  }

  return currentPos;
}


float MFFont_DrawTextJustified(MFFont *pFont, const char *pText, const MFVector &pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, const MFVector &color, int numChars, const MFMatrix &ltw)
{
  const char *pCurrentLine = pText;

  MFVector currentPos = MFVector::zero;
  float scale;
  int lineEnd, lastSignificantChar;

  // calculate height and scale
  float height = MFFontInternal_CalcHeightAndScale(pFont, textHeight, &scale);

  // justify vertically (this is slow)
  if(justification > MFFontJustify_Top_Full)
  {
    // find the initial y offset
    float totalHeight = 0.0f;

    // calculate total height
    while(*pCurrentLine)
    {
      lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, boxWidth, textHeight, &lastSignificantChar);

      totalHeight += height;
      pCurrentLine += lineEnd;
    }

    // justify according to justification mode
    if(justification != MFFontJustify_Center_Left && justification != MFFontJustify_Center && justification != MFFontJustify_Center_Right)
    {
      // bottom justified
      currentPos.y = boxHeight - totalHeight;
    }
    else
    {
      // center justified
      currentPos.y = boxHeight*0.5f - totalHeight*0.5f;
    }

    pCurrentLine = pText;
  }

  // process each line
  while(*pCurrentLine)
  {
    // get next line
    lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, boxWidth, textHeight, &lastSignificantChar);

    // if we are not left justified, we need to calculate the x offset
    if(justification != MFFontJustify_Top_Left && justification != MFFontJustify_Center_Left && justification != MFFontJustify_Bottom_Left)
    {
#if defined(_WRAP_EXCLUDES_TRAILING_WHITESPACE)
      float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, boxWidth, lastSignificantChar);
#else
      float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, boxWidth, lineEnd);
#endif

      if(justification != MFFontJustify_Top_Center && justification != MFFontJustify_Center && justification != MFFontJustify_Bottom_Center)
      {
        // right justified
        currentPos.x = boxWidth - width;
      }
      else
      {
        // center justified
        currentPos.x = boxWidth*0.5f - width*0.5f;
      }
    }

    MFFont_DrawText(pFont, currentPos+pos, textHeight, color, pCurrentLine, (int)MFMin((uint32)lastSignificantChar, (uint32)numChars), ltw);

    // increment height
    currentPos.y += height;

	// if we exceeded the number of characters to render
	if((uint32)numChars < (uint32)lineEnd)
		break;
	numChars -= lineEnd;

    // and increment string
    pCurrentLine += lineEnd;
  }

  return currentPos.y;
}

float MFFont_DrawTextAnchored(MFFont *pFont, const char *pText, const MFVector &pos, MFFontJustify justification, float lineWidth, float textHeight, const MFVector &color, int numChars, const MFMatrix &ltw)
{
  const char *pCurrentLine = pText;

  MFVector currentPos = MFVector::zero;
  float scale;
  int lineEnd, lastSignificantChar;
  float startY;

  // calculate height and scale
  float height = MFFontInternal_CalcHeightAndScale(pFont, textHeight, &scale);

  // justify vertically (this is slow)
  if(GetVerticalJustification(justification) > 0)
  {
    // find the initial y offset
    float totalHeight = 0.0f;

    // calculate total height
    while(*pCurrentLine)
    {
      lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, lineWidth, textHeight, &lastSignificantChar);

      totalHeight += height;
      pCurrentLine += lineEnd;
    }

    // justify according to justification mode
    if(GetVerticalJustification(justification) > 1)
    {
      // bottom justified
      currentPos.y = -totalHeight;
    }
    else
    {
      // center justified
      currentPos.y = -totalHeight*0.5f;
    }

    pCurrentLine = pText;
  }

  startY = currentPos.y;

  // process each line
  while(*pCurrentLine)
  {
    // get next line
    lineEnd = MFFont_GetNextWrapPoint(pFont, pCurrentLine, lineWidth, textHeight, &lastSignificantChar);

    // if we are not left justified, we need to calculate the x offset
    if(GetHorizontalJustification(justification) > 0)
    {
//#if defined(_WRAP_EXCLUDES_TRAILING_WHITESPACE)
      float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, lineWidth, lastSignificantChar);
//#else
//      float width = MFFont_GetStringWidth(pFont, pCurrentLine, textHeight, lineWidth, lineEnd);
//#endif

      if(GetHorizontalJustification(justification) > 1)
      {
        // right justified
        currentPos.x = -width;
      }
      else
      {
        // center justified
        currentPos.x = -width*0.5f;
      }
    }

    MFFont_DrawText(pFont, currentPos+pos, textHeight, color, pCurrentLine, (int)MFMin((uint32)lastSignificantChar, (uint32)numChars), ltw);

    // increment height
    currentPos.y += height;

	// if we exceeded the number of characters to render
	if((uint32)numChars < (uint32)lineEnd)
		break;
	numChars -= lineEnd;

    // and increment string
    pCurrentLine += lineEnd;
  }

  return currentPos.y - startY;
}



/////////////////////////////////
// unicode variants

float MFFont_GetStringWidthW(MFFont *pFont, const uint16 *pText, float height, float lineWidth, int maxLen, float *pTotalHeight)
{
	if(pFont == NULL) pFont = gpDebugFont;

	if(*pText == 0xFEFF)
		++pText;

	const uint16 *pC = pText;
	const uint16 *pLineStart = pText;
	float width = 0.0f;
	float maxWidth = 0.0f;
	float totalHeight = 0.0f;
	float scale;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float spaceWidth = pFont->spaceWidth * scale;

	while(*pC && maxLen)
	{
		switch(*pC)
		{
			case '\n':
				maxWidth = MFMax(width, maxWidth);
				width = 0.0f;
				totalHeight += height;
				break;
			case ' ':
				width += spaceWidth;
				break;
			default:
			{
				int id = pFont->pCharacterMapping[(int)*pC];
				width += (float)pFont->pChars[id].xadvance * scale;
				break;
			}
		}

		// check for and handle word wrapping
		if(lineWidth > 0.0f && width > lineWidth)
		{
			// we have exceeded a line width

			// here we need to recurse backward to find a valid wrap point and submit to DrawString
			float wrapWidth = width;
			const uint16 *pT = pC;
			while(pT > pLineStart)
			{
				int id = pFont->pCharacterMapping[(int)*pT];
				wrapWidth -= (float)pFont->pChars[id].xadvance * scale;

				if(MFFontInternal_IsValidWrapPoint(*pT))
				{
					// this is a valid wrap point.. how convenient..
					pC = pT;
					while(MFFontInternal_IsValidWrapPoint(*pC))
					{
						++pC;
						--maxLen;
					}
					break;
				}

				--pT;
				++maxLen;
			}

			if(pT == pLineStart)
			{
				// no valid wrap point was found so force wrap at current character
				int id = pFont->pCharacterMapping[(int)*pC];
				wrapWidth = width - (float)pFont->pChars[id].xadvance * scale;
			}

			// get the maximum width
			maxWidth = MFMax(wrapWidth, maxWidth);

			// bump pos
			width = 0.0f;
			totalHeight += height;

			// update character positions
			pLineStart = pC;
		}
		else
		{
			++pC;
			--maxLen;
		}
	}

	maxWidth = MFMax(width, maxWidth);

	if(pTotalHeight)
		*pTotalHeight = totalHeight + height;

	return maxWidth;
}

static int GetRenderableLengthW(MFFont *pFont, const uint16 *pText, int *pTotal, int page, int *pNextPage, int maxLen)
{
	if(page == -1) return 0;
	if(pFont == NULL) pFont = gpDebugFont;

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

float MFFont_DrawTextW(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const uint16 *pText, int maxChars, const MFMatrix &ltw)
{
	MFCALLSTACK;

	if(pFont == NULL) pFont = gpDebugFont;

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

	float uScale = pFont->xScale;
	float vScale = pFont->yScale;
	float scale;

	// calculate height and scale
	height = MFFontInternal_CalcHeightAndScale(pFont, height, &scale);

	float pos_x;
	float pos_y = pos.y;
	float pos_z;

	while(renderable)
	{
		float x,y,x2,y2,px,py;

		MFMaterial_SetMaterial(pFont->ppPages[page]);
		MFPrimitive(PT_QuadList|PT_Prelit);
		MFSetMatrix(ltw);

		MFBegin(renderable*2);

		MFSetColour(colour);

		pos_x = pos.x;
		pos_y = pos.y;
		pos_z = pos.z;

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

	return (pos_y-pos.y) + height;
}
