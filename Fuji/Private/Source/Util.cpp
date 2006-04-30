#include <stdio.h>
#include <ctype.h>

#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "Util.h"
#include "MFVector.h"
#include "MFPtrList.h"

#if !defined(_FUJI_UTIL)
#include "Display_Internal.h"
#include "MFPrimitive.h"
#include "MFFont.h"
#include "MFView.h"
#endif

void *gEmptyPtrList[2];
void *gEmptyPtrListDL[2];

// platform strings/names
static const char * const gPlatformStrings[FP_Max] =
{
	"PC",
	"XB",
	"LNX",
	"PSP",
	"PS2",
	"DC",
	"GC",
	"OSX",
	"AMI",
	"X360",
	"PS3"
};

static const char * const gPlatformNames[FP_Max] =
{
	"PC",
	"XBox",
	"Linux",
	"Playstation Portable",
	"Playstation 2",
	"Dreamcast",
	"Gamecube",
	"Mac OSX",
	"Amiga",
	"XBox 360",
	"Platstation 3"
};

static MFEndian gPlatformEndian[FP_Max] =
{
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
};

const char * MFSystem_GetPlatformString(int platform)
{
	return gPlatformStrings[platform];
}

const char * MFSystem_GetPlatformName(int platform)
{
	return gPlatformNames[platform];
}

MFEndian MFSystem_GetPlatformEndian(int platform)
{
	return gPlatformEndian[platform];
}

// Return only the last portion of the module name (exclude the path)
char *ModuleName(char *pSourceFileName)
{
	int x = MFString_Length(pSourceFileName)-1;

	while(x >= 0 && pSourceFileName[x] != '/' && pSourceFileName[x] != '\\')
	{
		--x;
	}

	return x == -1 ? pSourceFileName : &pSourceFileName[x+1];
}

// CRC functions
uint32 crc32_table[256];
#define CRC32_POLY 0x04c11db7
void CrcInit()
{
	int i, j;
	uint32 c;

	for (i = 0; i < 256; ++i)
	{
		for (c = i << 24, j = 8; j > 0; --j)
		{
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		}
		crc32_table[i] = c;
	}
}

// generate a unique Crc number for this buffer
uint32 Crc(char *buffer, int length)
{
	char *p;
	uint32 crc;

	crc = 0xffffffff;       /* preload shift register, per CRC-32 spec */
	for (p = buffer; length > 0; ++p, --length)
		crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];
	return ~crc;            /* transmit complement, per CRC-32 spec */
}

