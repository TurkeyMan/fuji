#include "Fuji.h"
#include "MFHeap.h"

#include "ConvertTex.h"
#include "IntImage.h"

#include "png.h"

SourceImage* LoadPNG(const char *pFilename)
{
	FILE *pFile = fopen(pFilename, "rb");

	if(!pFile)
		return NULL;

	uint8 header[8];
	fread(header, 1, 8, pFile);

	if(png_sig_cmp(header, 0, 8))
	{
		printf("Not a PNG file..");
		return NULL;
	}

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	// initialize stuff
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, pFile);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = info_ptr->width;
	height = info_ptr->height;
	color_type = info_ptr->color_type;
	bit_depth = info_ptr->bit_depth;

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// read file
	setjmp(png_jmpbuf(png_ptr));

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y=0; y<height; y++)
		row_pointers[y] = (png_byte*)malloc(info_ptr->rowbytes);

	png_read_image(png_ptr, row_pointers);

	fclose(pFile);

	// allocate internal image structures
	SourceImage *pImage = (SourceImage*)malloc(sizeof(SourceImage));

	pImage->mipLevels = 1;
	pImage->pLevels = (SourceImageLevel*)malloc(sizeof(SourceImageLevel));

	pImage->pLevels[0].pData = (Pixel*)malloc(sizeof(Pixel)*width*height);
	pImage->pLevels[0].width = width;
	pImage->pLevels[0].height = height;

	Pixel *pPixel = pImage->pLevels[0].pData;

	switch(color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)*p * (1.0f/255.0f);
						pPixel->g = (float)*p * (1.0f/255.0f);
						pPixel->b = (float)*p * (1.0f/255.0f);
						pPixel->a = 1.0f;

						++p;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)*p * (1.0f/65535.0f);
						pPixel->g = (float)*p * (1.0f/65535.0f);
						pPixel->b = (float)*p * (1.0f/65535.0f);
						pPixel->a = 1.0f;

						++p;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_PALETTE:
			break;
		case PNG_COLOR_TYPE_RGB:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[1] * (1.0f/255.0f);
						pPixel->b = (float)p[2] * (1.0f/255.0f);
						pPixel->a = 1.0f;

						p += 3;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[1] * (1.0f/65535.0f);
						pPixel->b = (float)p[2] * (1.0f/65535.0f);
						pPixel->a = 1.0f;

						p += 3;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[1] * (1.0f/255.0f);
						pPixel->b = (float)p[2] * (1.0f/255.0f);
						pPixel->a = (float)p[3] * (1.0f/255.0f);

						p += 4;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[1] * (1.0f/65535.0f);
						pPixel->b = (float)p[2] * (1.0f/65535.0f);
						pPixel->a = (float)p[3] * (1.0f/65535.0f);

						p += 4;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			for(int y=0; y<height; ++y)
			{
				if(bit_depth == 8)
				{
					uint8 *p = row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/255.0f);
						pPixel->g = (float)p[0] * (1.0f/255.0f);
						pPixel->b = (float)p[0] * (1.0f/255.0f);
						pPixel->a = (float)p[1] * (1.0f/255.0f);

						p += 2;
						++pPixel;
					}
				}
				else if(bit_depth == 16)
				{
					uint16 *p = (uint16*)row_pointers[y];

					for(int x=0; x<width; ++x)
					{
						pPixel->r = (float)p[0] * (1.0f/65535.0f);
						pPixel->g = (float)p[0] * (1.0f/65535.0f);
						pPixel->b = (float)p[0] * (1.0f/65535.0f);
						pPixel->a = (float)p[1] * (1.0f/65535.0f);

						p += 2;
						++pPixel;
					}
				}
				else
				{
					MFDebug_Assert(false, "Invalid bit depth!");
				}
			}
			break;
	}

	// free image
	for(int y=0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	// scan for alpha information
	ScanImage(pImage);

	return pImage;
}
