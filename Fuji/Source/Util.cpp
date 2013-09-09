#include <stdio.h>
#include <ctype.h>

#include "Fuji.h"
#include "MFSystem_Internal.h"
#include "Util.h"
#include "MFVector.h"
#include "MFPtrList.h"

#if !defined(_FUJI_UTIL)
#include "MFDisplay_Internal.h"
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
	"PS3",
	"WII",
	"SYM",
	"IPH",
	"AND",
	"WM",
	"NACL"
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
	"Platstation 3",
	"Nintendo Wii",
	"Symbian",
	"iPhone",
	"Android",
	"Windows Mobile",
	"Native Client"
};

static MFEndian gPlatformEndian[FP_Max] =
{
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,	// on big endian linux we will do runtime endian flipping
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_BigEndian,
	MFEndian_LittleEndian,	// on big endian mac's we will do runtime endian flipping
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_BigEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian,
	MFEndian_LittleEndian
};

MF_API const char * MFSystem_GetPlatformString(int platform)
{
	return gPlatformStrings[platform];
}

MF_API const char * MFSystem_GetPlatformName(int platform)
{
	return gPlatformNames[platform];
}

MF_API MFEndian MFSystem_GetPlatformEndian(int platform)
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

MFInitStatus MFUtil_InitModule()
{
	MFUtil_CrcInit();

	return MFIS_Succeeded;
}

// CRC functions
static uint32 gCrc32Table[256];
#define CRC32_POLY 0x04c11db7
void MFUtil_CrcInit()
{
	int i, j;
	uint32 c;

	for (i = 0; i < 256; ++i)
	{
		for (c = i << 24, j = 8; j > 0; --j)
		{
			c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
		}
		gCrc32Table[i] = c;
	}
}

// generate a unique Crc value for this buffer
MF_API uint32 MFUtil_Crc(const char *pBuffer, size_t length)
{
	uint32 crc = 0xffffffff;		// preload shift register, per CRC-32 spec

	for(; length > 0; ++pBuffer, --length)
		crc = (crc << 8) ^ gCrc32Table[(crc >> 24) ^ *(const uint8*)pBuffer];

	return ~crc;			// transmit complement, per CRC-32 spec
}

// generate a unique Crc value for this string, strings are treated case INSENSITIVE
MF_API uint32 MFUtil_CrcString(const char *pString)
{
	uint32 crc = 0xffffffff;		// preload shift register, per CRC-32 spec

	for(; *pString; ++pString)
		crc = (crc << 8) ^ gCrc32Table[(crc >> 24) ^ (*(const uint8*)pString | 0x20)];

	return ~crc;			// transmit complement, per CRC-32 spec
}

#define USE_FNV

// generate a fast hash value for this string, strings are treated case INSENSITIVE
#if defined(USE_FNV)

// using the FNV-1a hash algorithm
MF_API uint32 MFUtil_HashString(const char *pString)
{
	uint32 hash = 2166136261;

	while(*pString)
		hash = (hash ^ (uint32)MFToLower(*pString++)) * 16777619;

	return hash;
}

#else

// using the one-at-a-time algorithm
MF_API uint32 MFUtil_HashString(const char *s)
{
    uint32 hash = 0;

    while(*s)
    {
        hash += MFToLower(*s++);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

#endif
