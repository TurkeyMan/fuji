#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "Display.h"
#include "Texture.h"
#include "Font.h"
#include "Primitive.h"

Font debugFont;

void Font_InitModule()
{
	CALLSTACK("Font_InitModule");

	debugFont.LoadFont(File_SystemPath("Font\\Arial"));
}

void Font_DeinitModule()
{
	CALLSTACK("Font_DeinitModule");

	debugFont.Release();
}

int Font::LoadFont(char *filename)
{
	CALLSTACK("Font::LoadFont");

	HANDLE file;
	char tempbuffer[1024];
	uint32 bytesread;

	strcpy(tempbuffer, filename);
	strcat(tempbuffer, ".tga");

	pTexture = Texture::LoadTexture(tempbuffer);

	strcpy(tempbuffer, filename);
	strcat(tempbuffer, ".dat");

	file = CreateFile(tempbuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	ReadFile(file, charwidths, 256, (LPDWORD)&bytesread, NULL);
	CloseHandle(file);

	return 0;
}

void Font::Release()
{
	CALLSTACK("Font::Release");

	pTexture->Release();
}

int Font::DrawText(float pos_x, float pos_y, float pos_z, float height, uint32 colour, char *text, bool invert)
{
	CALLSTACKc("Font::DrawText");

	int textlen = strlen(text);

	DBGASSERT(textlen < 2048, "Exceeded Font Vertex Buffer Limit");

	float x,y,w,h, p, cwidth;

	MFPrimitive(PT_TriList|PT_Prelit);

	MFBegin(textlen*2*3);

	for(int i=0; i<textlen; i++)
	{
		x = (float)((uint8)text[i] & 0x0F) * (float)(pTexture->width / 16);
		y = (float)((uint8)text[i] >> 4) * (float)(pTexture->height / 16);

		w = (float)charwidths[(uint8)text[i]];
		h = (float)pTexture->height/16.0f;

		x /= (float)pTexture->width;
		y /= (float)pTexture->height;
		w /= (float)pTexture->width;
		h /= (float)pTexture->height;

		p = w/h;
		cwidth = height*p;

		MFSetColour(colour);
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

	pTexture->SetTexture();

	MFEnd();

	return 0;
}

int Font::DrawText(float pos_x, float pos_y, float height, uint32 colour, char *text, bool invert)
{
	return DrawText(pos_x, pos_y, 0, height, colour, text, invert);
}

int Font::DrawText(Vector3 pos, float height, uint32 colour, char *text, bool invert)
{
	return DrawText(pos.x, pos.y, pos.z, height, colour, text, invert);
}

int Font::DrawTextf(float pos_x, float pos_y, float height, uint32 colour, char *format, ...)
{
	CALLSTACK("Font::DrawTextf");

	va_list args;

	int len;
	char *buffer;

	va_start(args, format);
	len = _vscprintf(format, args)+1;
	buffer = new char[len];

	vsprintf(buffer, format, args);

	return DrawText(pos_x, pos_y, 0, height, colour, buffer);

	delete[] buffer;
}

int Font::DrawTextf(float pos_x, float pos_y, float pos_z, float height, uint32 colour, char *format, ...)
{
	CALLSTACK("Font::DrawTextf");

	va_list args;

	int len;
	char *buffer;

	va_start(args, format);
	len = _vscprintf(format, args)+1;
	buffer = new char[len];

	vsprintf(buffer, format, args);

	return DrawText(pos_x, pos_y, pos_z, height, colour, buffer);

	delete[] buffer;
}

int Font::DrawTextf(Vector3 pos, float height, uint32 colour, char *format, ...)
{
	CALLSTACK("Font::DrawTextf");

	va_list args;

	int len;
	char *buffer;

	va_start(args, format);
	len = _vscprintf(format, args)+1;
	buffer = new char[len];

	vsprintf(buffer, format, args);

	return DrawText(pos.x, pos.y, pos.z, height, colour, buffer);

	delete[] buffer;
}
