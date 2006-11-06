#if !defined(_UTIL_H)
#define _UTIL_H

// CRC functions
void MFUtil_CrcInit(); // generates some crc tables - system should call this once
uint32 MFUtil_Crc(char *buffer, int length); // generate a unique Crc number for this buffer
uint32 MFUtil_CrcString(char *pString);	// generate a unique Crc number for this string
uint32 MFUtil_HashString(char *pString); // generate a very fast hash value from a string

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
		int offset = (int&)pBase;

		if(!fix)
			offset = -offset;

		pPointer = (T*)((char*)pPointer + offset);
	}
}

#endif
