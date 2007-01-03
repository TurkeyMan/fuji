#if !defined(_LOAD_TARGA_H)
#define _LOAD_TARGA_H

#pragma pack(1)
struct TgaHeader
{
	uint8 idLength;
	uint8 colourMapType;
	uint8 imageType;

	uint16 colourMapStart;
	uint16 colourMapLength;
	uint8 colourMapBits;

	uint16 xStart;
	uint16 yStart;
	uint16 width;
	uint16 height;
	uint8 bpp;
	uint8 flags;
};
#pragma pack ()

#endif
