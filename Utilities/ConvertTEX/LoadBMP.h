#if !defined(_LOAD_BMP_H)
#define _LOAD_BMP_H

enum BMPCompressionType
{
	RGB = 0,			/* No compression - straight BGR data */
	RLE8 = 1,		/* 8-bit run-length compression */
	RLE4 = 2,		/* 4-bit run-length compression */
	BITFIELDS = 3	/* RGB bitmap with RGB masks */
};

#pragma pack(1)
struct BMPHeader
{
   unsigned short int type;                 /* Magic identifier            */
   unsigned int size;                       /* File size in bytes          */
   unsigned short int reserved1, reserved2;
   unsigned int offset;                     /* Offset to image data, bytes */
};

struct BMPInfoHeader
{
   unsigned int size;               /* Header size in bytes      */
   int width,height;                /* Width and height of image */
   unsigned short int planes;       /* Number of colour planes   */
   unsigned short int bits;         /* Bits per pixel            */
   unsigned int compression;        /* Compression type          */
   unsigned int imagesize;          /* Image size in bytes       */
   int xresolution,yresolution;     /* Pixels per meter          */
   unsigned int ncolours;           /* Number of colours         */
   unsigned int importantcolours;   /* Important colours         */
};
#pragma pack ()

struct BMPPaletteEntry /**** Colormap entry structure ****/
{
	unsigned char  rgbBlue;          /* Blue value */
	unsigned char  rgbGreen;         /* Green value */
	unsigned char  rgbRed;           /* Red value */
	unsigned char  rgbReserved;      /* Reserved */
};

SourceImage* LoadBMP(const char *pFilename);

#endif
