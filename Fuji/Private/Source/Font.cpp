#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "Display.h"
#include "Texture.h"
#include "Font.h"

int Font::LoadFont(char *filename)
{
	HANDLE file;
	char tempbuffer[1024];
	uint32 bytesread;

	strcpy(tempbuffer, filename);
	strcat(tempbuffer, ".tga");

	texture.LoadTexture(tempbuffer);

	strcpy(tempbuffer, filename);
	strcat(tempbuffer, ".dat");

	file = CreateFile(tempbuffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	ReadFile(file, charwidths, 256, (LPDWORD)&bytesread, NULL);
	CloseHandle(file);
	
	return 0;
}

void Font::Release()
{
	texture.ReleaseTexture();
}

int Font::DrawText(float pos_x, float pos_y, float pos_z, float height, uint32 colour, char *text, bool invert)
{
	IDirect3DVertexBuffer8 *vb;
	FontVertex *v;
	int textlen = strlen(text);

	float x,y,w,h, p, cwidth;

	pd3dDevice->CreateVertexBuffer(textlen*6*sizeof(FontVertex), NULL, NULL, NULL, &vb);

	vb->Lock(0, 0, (BYTE**)&v, NULL);

	for(int i=0; i<textlen; i++)
	{
		x = (float)(text[i] & 0x0F) * (float)(texture.width / 16);
		y = (float)(text[i] >> 4) * (float)(texture.height / 16);

		w = (float)charwidths[text[i]];
		h = (float)texture.height/16.0f;

		x /= (float)texture.width;
		y /= (float)texture.height;
		w /= (float)texture.width;
		h /= (float)texture.height;

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

	vb->Unlock();

	texture.SetTexture();

	pd3dDevice->SetStreamSource(0, vb, sizeof(FontVertex));
	pd3dDevice->SetVertexShader(FontVertex::FVF);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, textlen*2);

	vb->Release();

	return 0;
}

int Font::DrawText(float pos_x, float pos_y, float height, uint32 colour, char *text, bool invert)
{
	return DrawText(pos_x, pos_y, 0, height, colour, text, invert);
}

int Font::DrawTextf(float pos_x, float pos_y, float height, uint32 colour, char *format, ...)
{
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
