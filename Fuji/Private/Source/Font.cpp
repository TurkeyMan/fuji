#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "Display.h"
#include "Texture.h"
#include "Font.h"

IDirect3DVertexBuffer8 *fontBuffer;

Font debugFont;

void Font_InitModule()
{
	CALLSTACK("Font_InitModule");

	pd3dDevice->CreateVertexBuffer(2048*6*sizeof(FontVertex), NULL, NULL, NULL, &fontBuffer);

	debugFont.LoadFont("D:\\Data\\Font\\Arial");
}

void Font_DeinitModule()
{
	CALLSTACK("Font_DeinitModule");

	debugFont.Release();

	fontBuffer->Release();
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

	FontVertex *v;
	int textlen = strlen(text);

	DBGASSERT(textlen < 2048, "Exceeded Font Vertex Buffer Limit");

	float x,y,w,h, p, cwidth;

	fontBuffer->Lock(0, 0, (BYTE**)&v, NULL);

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

		v->pos.Set(pos_x, pos_y, pos_z);
		v->colour = colour;
		v->u = x;
		v->v = y;
		v++;
		v->pos.Set(pos_x + cwidth, pos_y + (invert?-height:height), pos_z);
		v->colour = colour;
		v->u = x+w;
		v->v = y+h;
		v++;
		v->pos.Set(pos_x, pos_y + (invert?-height:height), pos_z);
		v->colour = colour;
		v->u = x;
		v->v = y+h;
		v++;
		v->pos.Set(pos_x, pos_y, pos_z);
		v->colour = colour;
		v->u = x;
		v->v = y;
		v++;
		v->pos.Set(pos_x + cwidth, pos_y, pos_z);
		v->colour = colour;
		v->u = x+w;
		v->v = y;
		v++;
		v->pos.Set(pos_x + cwidth, pos_y + (invert?-height:height), pos_z);
		v->colour = colour;
		v->u = x+w;
		v->v = y+h;
		v++;

		pos_x += cwidth;
	}

	fontBuffer->Unlock();

	pTexture->SetTexture();

	pd3dDevice->SetStreamSource(0, fontBuffer, sizeof(FontVertex));
	pd3dDevice->SetVertexShader(FontVertex::FVF);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, textlen*2);

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
