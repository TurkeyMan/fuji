#if !defined(_UTIL_H)
#define _UTIL_H

MFInitStatus MFUtil_InitModule();

// CRC functions
void MFUtil_CrcInit(); // generates some crc tables - system should call this once
MF_API uint32 MFUtil_Crc(const char *pBuffer, size_t length); // generate a unique Crc number for this buffer
MF_API uint32 MFUtil_CrcString(const char *pString);	// generate a unique Crc number for this string

inline uint32 MFUtil_HashBuffer(const void *pBuffer, size_t length)
{
	return MFUtil_Crc((const char*)pBuffer, length);
}

MF_API uint32 MFUtil_HashString(const char *pString); // generate a very fast hash value from a string

inline size_t MFUtil_HashPointer(const void *pPointer)
{
	// TODO: this function could do with some work...
	// perhaps do it differently on 64bit platforms, or use some
	// platform specific knowledge about the memspace...
	return (size_t)pPointer >> 6;
/*
	// uber hash
	size_t Value = (size_t)pPointer;
    Value = ~Value + (Value << 15);
    Value = Value ^ (Value >> 12);
    Value = Value + (Value << 2);
    Value = Value ^ (Value >> 4);
    Value = Value * 2057;
    Value = Value ^ (Value >> 16);
    return Value;
*/
}

template<typename T>
inline T MFUtil_NextPowerOf2(T x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
}

#if defined(MF_COMPILER_VISUALC)
	#include <intrin.h>
	__forceinline bool MFUtil_BitScanReverse(uint32 value, uint32 *pIndex)
	{
		return _BitScanReverse((unsigned long*)pIndex, value) != 0;
	}
	__forceinline bool MFUtil_BitScanReverse64(uint64 value, uint32 *pIndex)
	{
#if defined(MF_64BIT)
		return _BitScanReverse64((unsigned long*)pIndex, value) != 0;
#else
		if(_BitScanReverse((unsigned long*)pIndex, value >> 32))
		{
			*pIndex += 32;
			return true;
		}
		else
			return _BitScanReverse((unsigned long*)pIndex, (unsigned long)value) != 0;
#endif
	}
#elif defined(MF_COMPILER_GCC) || defined(MF_COMPILER_CLANG)
	__forceinline bool MFUtil_BitScanReverse(uint32 value, uint32 *pIndex)
	{
		*pIndex = 31 - (uint32)__builtin_clz(value);
		return value != 0;
	}
	__forceinline bool MFUtil_BitScanReverse64(uint64 value, uint32 *pIndex)
	{
		*pIndex = 63 - (uint32)__builtin_clzll(value);
		return value != 0;
	}
#else
	#pragma message "I'm sure the machine has an opcode for this!"
	bool MFUtil_BitScanReverse(uint32 value, uint32 *pIndex)
	{
		if(x == 0)
			return false;
		int n = 0;
		if ((x & 0xFFFF0000) == 0) { n += 16; x <<= 16; }
		if ((x & 0xFF000000) == 0) { n += 8; x <<= 8; }
		if ((x & 0xF0000000) == 0) { n += 4; x <<= 4; }
		if ((x & 0xC0000000) == 0) { n += 2, x <<= 2; }
		if ((x & 0x80000000) == 0) { n += 1, x <<= 1; }
		*pIndex = 31 - n;
		return true;
	}
	bool MFUtil_BitScanReverse64(uint64 value, uint32 *pIndex)
	{
		if(x == 0)
			return false;
		uint32 n = 0;
		if ((x & 0xFFFFFFFF00000000ULL) == 0) { n += 32; x <<= 32; }
		if ((x & 0xFFFF000000000000ULL) == 0) { n += 16; x <<= 16; }
		if ((x & 0xFF00000000000000ULL) == 0) { n += 8; x <<= 8; }
		if ((x & 0xF000000000000000ULL) == 0) { n += 4; x <<= 4; }
		if ((x & 0xC000000000000000ULL) == 0) { n += 2, x <<= 2; }
		if ((x & 0x8000000000000000ULL) == 0) { n += 1, x <<= 1; }
		*pIndex = 63 - n;
		return true;
	}
#endif

// endian flipping
#if defined(MFBIG_ENDIAN)
#define MFEndian_HostToBig(x)
#define MFEndian_HostToLittle(x) MFEndian_Flip(x)
#define MFEndian_LittleToHost(x) MFEndian_Flip(x)
#define MFEndian_BigToHost(x)
#else
#define MFEndian_HostToBig(x) MFEndian_Flip(x)
#define MFEndian_HostToLittle(x)
#define MFEndian_LittleToHost(x)
#define MFEndian_BigToHost(x) MFEndian_Flip(x)
#endif

template <typename T>
inline void MFEndian_Flip(T *pData)
{
	register char t[sizeof(T)];
	const char *pBytes = (const char*)pData;

	for(uint32 a=0; a<sizeof(T); a++)
	{
		t[a] = pBytes[sizeof(T)-1-a];
	}

	*pData = *(T*)t;
}

template<typename T>
inline void MFFixUp(T* &pPointer, void *pBase, int fix)
{
	if(pPointer)
	{
		intp offset = (intp&)pBase;

		if(!fix)
			offset = -offset;

		pPointer = (T*)((char*)pPointer + offset);
	}
}

#endif
