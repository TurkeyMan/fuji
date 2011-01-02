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
	"WM"
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
	"Windows Mobile"
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
uint32 MFUtil_Crc(const char *pBuffer, int length)
{
	uint32 crc = 0xffffffff;		// preload shift register, per CRC-32 spec

	for(; length > 0; ++pBuffer, --length)
		crc = (crc << 8) ^ gCrc32Table[(crc >> 24) ^ *pBuffer];

	return ~crc;			// transmit complement, per CRC-32 spec
}

// generate a unique Crc value for this string, strings are treated case INSENSITIVE
uint32 MFUtil_CrcString(const char *pString)
{
	uint32 crc = 0xffffffff;		// preload shift register, per CRC-32 spec

	for(; *pString; ++pString)
		crc = (crc << 8) ^ gCrc32Table[(crc >> 24) ^ (*pString | 0x20)];

	return ~crc;			// transmit complement, per CRC-32 spec
}

#define USE_FNV
//#define USE_SUPERFASTHASH

// generate a fast hash value for this string, strings are treated case INSENSITIVE
#if defined(USE_FNV)

// using the FNV-1a hash algorithm
uint32 MFUtil_HashString(const char *pString)
{
	uint32 hash = 2166136261;

	while(*pString)
		hash = (hash ^ (uint32)*pString++) * 16777619;

	return hash;
}

#elif defined(USE_SUPERFASTHASH)

// using the SuperFastHash algorithm
#if defined(MF_ARCH_X86) // any little endian system that can perform unaligned loads
//	#define get16bits(d) (*(uint16*)(d))
	#define get32bits(d) (*(uint32*)(d))
#else
//	#define get16bits(d) ((((uint8*)(d))[1] << 8) | ((uint8*)(d))[0])
	#define get32bits(d) ((((uint8*)(d))[3] << 24) | ((((uint8*)(d))[2] << 16) | ((((uint8*)(d))[1] << 8) | ((uint8*)(d))[0])
#endif

uint32 MFUtil_HashString(const char *data)
{
//	uint32 hash = len, tmp;
	uint32 hash = (uint8)*data, tmp;

	// Main loop
	uint32 d, hasZero;
	while(1)
	{
		d = get32bits(data);
		data += 4;

		hasZero = (d - 0x01010101UL) & ~d & 0x80808080UL;
		if(hasZero)
			break;

		hash  += (d & 0xFFFF);
		tmp    = ((d >> 16) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		hash  += hash >> 11;
	}

	// Handle end cases
	if((hasZero & 0x808080) == 0)
	{
		hash += (d & 0xFFFF);
		hash ^= hash << 16;
		hash ^= (d >> 16) << 18;
		hash += hash >> 11;
	}
	else if((hasZero & 0x8080) == 0)
	{
		hash += (d & 0xFFFF);
		hash ^= hash << 11;
		hash += hash >> 17;
	}
	else if(hasZero & 0x80 == 0)
	{
		hash += (d & 0xFF);
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	// Force "avalanching" of final 127 bits
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

#else

// using the one-at-a-time algorithm
uint32 MFUtil_HashString(const char *s)
{
    uint32 hash = 0;

    while(*s)
    {
        hash += *s++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

#endif

int MFUtil_NextPowerOf2(int x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
}
