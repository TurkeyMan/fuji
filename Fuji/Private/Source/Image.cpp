#include "Common.h"
#include "Image.h"
#include "MFFileSystem.h"

#if defined(_WINDOWS) || defined(_XBOX)
#pragma pack(1)
#endif
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
} _PACKED;

#if defined(_WINDOWS) || defined(_XBOX)
#pragma pack ()
#endif

FujiImage::FujiImage()
{
	pixels = NULL;
	width = height = 0;
	bitsPerPixel = bytesPerPixel = 0;
}

FujiImage::~FujiImage()
{
	if(pixels != NULL)
	{
		Heap_TFree(pixels);
		pixels = NULL;
	}

	width = height = 0;
	bitsPerPixel = bytesPerPixel = 0;
}

void FujiImage::Convert(ImageFormat toFormat)
{
	CALLSTACK;

	unsigned char *p = (unsigned char *)pixels;
	unsigned char temp;

	if((bitsPerPixel == 24) && (toFormat == FUJI_RGBA))
		toFormat = FUJI_RGB;

	if((bitsPerPixel == 32) && (toFormat == FUJI_RGB))
		toFormat = FUJI_RGBA;

	if(toFormat == format)
		return;

	if((format == FUJI_BGRA && toFormat == FUJI_RGBA) || (format == FUJI_RGBA && toFormat == FUJI_BGRA))
	{
		for(uint32 i=0; i < (width * height); i++)
		{
			temp = p[0];
			p[0] = p[2];
			p[2] = temp;

			p += 4;
		}
	}
	else if((format == FUJI_BGR && toFormat == FUJI_RGB) || (format == FUJI_RGB && toFormat == FUJI_BGR)) 
	{
		for(uint32 i=0; i < (width * height); i++)
		{
			temp = p[0];
			p[0] = p[2];
			p[2] = temp;

			p += 3;
		}
	}
	else
	{
		LOGD(STR("Unhandled conversion pair!"));
	}
}

void FujiImage::VFlip(void)
{
	CALLSTACK;

	unsigned char *tempPixels, *top, *bottom;
	uint32 verticalRange, horizSpan;

	if(height < 2)
		return;

	verticalRange = height / 2;
	horizSpan = bytesPerPixel * width;

	tempPixels = (unsigned char *)Heap_TAlloc(horizSpan);

	top = (unsigned char *)pixels;
	bottom = (unsigned char *)pixels + (width * (height - 1) * bytesPerPixel);

	for(uint32 line=0; line < verticalRange; line++)
	{
		memcpy(tempPixels, top, horizSpan);
		memcpy(top, bottom, horizSpan);
		memcpy(bottom, tempPixels, horizSpan);

		top += horizSpan;
		bottom -= horizSpan;
	}

	Heap_TFree(tempPixels);
}

FujiImage* LoadTGA(const char *filename, bool flipped)
{
	CALLSTACK;

	unsigned char *contents, *position;
	bool isSavedFlipped = true;

	uint32 bytesRead;

	Heap_ActivateTempMemOverride(true);
	contents = (unsigned char *)MFFileSystem_Load(filename, &bytesRead);
	Heap_ActivateTempMemOverride(false);

	if(contents == NULL || bytesRead < (sizeof(TgaHeader) + 1))
	{
		return(NULL);
	}

	position = contents;
	struct TgaHeader *header = (struct TgaHeader *)position;
	position += sizeof(struct TgaHeader);

	if((header->imageType != 2) && (header->imageType != 10))
	{
		LOGD(STR("Failed loading image: %s (Unhandled TGA type (%d))", filename, header->imageType));
		Heap_TFree(contents);
		return(NULL);
	}

	if((header->bpp != 24) && (header->bpp != 32))
	{
		LOGD(STR("Failed loading image: %s (Invalid colour depth (%d))", filename, header->bpp));
		Heap_TFree(contents);
		return(NULL);
	}

	if((header->flags & 0xC0))
	{
		LOGD(STR("Failed loading image: %s (Interleaved images not supported)", filename));
		Heap_TFree(contents);
		return(NULL);
	}

	if((header->flags & 0x20) >> 5)
	{
		isSavedFlipped = false;
	}

	if((position + header->idLength + (header->colourMapLength * header->colourMapBits * header->colourMapType)) >= contents + bytesRead)
	{
		LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
		Heap_TFree(contents);
		return(NULL);
	}

	position += header->idLength;

	FujiImage *image = new FujiImage;
	if(header->bpp == 24)
	{
		image->bitsPerPixel = 24;
		image->bytesPerPixel = 3;
		image->format = FUJI_BGR;
	}
	else // 32
	{
		image->bitsPerPixel = 32;
		image->bytesPerPixel = 4;
		image->format = FUJI_BGRA;
	}

	image->width = header->width;
	image->height = header->height;

	image->pixels = Heap_TAlloc(header->width * header->height * image->bytesPerPixel);

	if(header->imageType == 10) // The hard way
	{
		uint32 pixelsRead = 0;

		while(pixelsRead < (uint32)(header->width * header->height))
		{
			if(position >= contents + bytesRead)
			{
				LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
				delete image;
				Heap_TFree(contents);
				return(NULL);
			}

			if(*position & 0x80) // Run length packet
			{
				uint8 length = ((*position) & 0x7F) + 1;

				position += 1;

				if((position + image->bytesPerPixel) > contents + bytesRead)
				{
					LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
					delete image;
					Heap_TFree(contents);
					return(NULL);
				}

				if((pixelsRead + length) > (uint32)(header->width * header->height))
				{
					LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
					delete image;
					Heap_TFree(contents);
					return(NULL);
				}

				unsigned char pixel[4];
				uint8 i;

				for(i = 0; i < image->bytesPerPixel; i++)
				{
					pixel[i] = position[i];
				}

				for(i = 0; i < length; i++)
				{
					for(uint8 j = 0; j < image->bytesPerPixel; j++)
					{
						((unsigned char *)image->pixels)[(pixelsRead * image->bytesPerPixel) + j] = pixel[j];
					}

					++pixelsRead;
				}

				position += image->bytesPerPixel;
			}
			else
			{ // Raw packet
				uint8 length = ((*position) & 0x7F) + 1;

				position += 1;

				if((position + (image->bytesPerPixel * length)) > contents + bytesRead)
				{
					LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
					delete image;
					Heap_TFree(contents);
					return(NULL);
				}

				if((pixelsRead + length) > (uint32)(header->width * header->height))
				{
					LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
					delete image;
					Heap_TFree(contents);
					return(NULL);
				}

				memcpy(&(((unsigned char *)image->pixels)[pixelsRead * image->bytesPerPixel]), position, length * image->bytesPerPixel);
				pixelsRead += length;
				position += image->bytesPerPixel * length;
			}
		}
	}
	else // 2 - The easy way
	{
		if((position + (image->bytesPerPixel * (header->width * header->height))) > contents + bytesRead)
		{
			LOGD(STR("Failed loading image: %s (Unexpected end of file)", filename));
			delete image;
			Heap_TFree(contents);
			return(NULL);
		}

		memcpy(image->pixels, position, header->width * header->height * image->bytesPerPixel);
	}

	Heap_TFree(contents);
	
	if(flipped)
	{
		if(!isSavedFlipped)
		{
			image->VFlip();
		}
	}
	else
	{
		if(isSavedFlipped)
		{
			image->VFlip();
		}
	}

	return(image);
}
