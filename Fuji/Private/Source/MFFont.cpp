#include "Fuji.h"
#include "MFHeap.h"
#include "Display.h"
#include "MFFileSystem.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "MFFont_Internal.h"
#include "MFPrimitive.h"
#include "MFFileSystem.h"

#include "Materials/Mat_Standard.h"

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
		MFDebug_Assert(!strcmp(pFont->pMaterial->pType->pTypeName, "Standard"), "Fonts MUST be created from a 'Standard' material.");
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

int MFFont_DrawText(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pText)
{
	MFCALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pFont->pMaterial->pInstanceData;

	int textlen = strlen(pText);

	MFDebug_Assert(textlen < 2048, "Exceeded Font Vertex Buffer Limit");

	float x,y,w,h, p, cwidth;

	MFMaterial_SetMaterial(pFont->pMaterial);
	MFPrimitive(PT_QuadList|PT_Prelit);

	MFBegin(textlen*2);

	MFSetColour(colour);

	float texWidth = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].width;
	float texHeight = (float)pData->pTextures[0]->pTemplateData->pSurfaces[0].height;

	float pos_x = pos.x;
	float pos_y = pos.y;
	float pos_z = pos.z;

	for(int i=0; i<textlen; i++)
	{
		x = (float)((uint8)pText[i] & 0x0F) * (float)(pData->pTextures[0]->pTemplateData->pSurfaces[0].width / 16);
		y = (float)((uint8)pText[i] >> 4) * (float)(pData->pTextures[0]->pTemplateData->pSurfaces[0].height / 16);

		w = (float)pFont->charwidths[(uint8)pText[i]];
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
		MFSetPosition(pos_x + cwidth, pos_y + height, pos_z);

		pos_x += cwidth;
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
