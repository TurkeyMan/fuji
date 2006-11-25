
// some useful string parsing functions
inline bool MFIsWhite(int c)
{
	return c==' ' || c=='\t';
}

inline bool MFIsAlpha(int c)
{
#if defined(MFLOCALE_ENGLISH_ONLY)
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
#elif defined(MFLOCALE_BASIC_LATIN)
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c >= 'à' && c <= 'þ') || (c >= 'À' && c <= 'Þ');
#elif defined(MFLOCALE_BASIC_LATIN_GREEK_CRYLLIC)
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c >= 'à' && c <= 'þ') || (c >= 'À' && c <= 'Þ') || (c >= 0x0391 && c <= 0x03AB) || (c >= 0x03B1 && c <= 0x03CB) || (c >= 0x0410 && c <= 0x044F);
#endif
}

inline bool MFIsNumeric(int c)
{
	return c>='0' && c<='9';
}

inline bool MFIsAlphaNumeric(int c)
{
#if defined(MFLOCALE_ENGLISH_ONLY)
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='_');
#elif defined(MFLOCALE_BASIC_LATIN)
	return (c>='a' && c<='z') || (c >= 'à' && c <= 'þ') || (c>='A' && c<='Z') || (c >= 'À' && c <= 'Þ') || (c>='0' && c<='9') || (c=='_');
#elif defined(MFLOCALE_BASIC_LATIN_GREEK_CRYLLIC)
	return (c>='a' && c<='z') || (c >= 'à' && c <= 'þ') || (c>='A' && c<='Z') || (c >= 'À' && c <= 'Þ') || (c >= 0x0391 && c <= 0x03AB) || (c >= 0x03B1 && c <= 0x03CB) || (c >= 0x0410 && c <= 0x044F) || (c>='0' && c<='9') || (c=='_');
#endif
}

inline bool MFIsNewline(int c)
{
	return c=='\n' || c=='\r';
}

inline bool MFIsLower(int c)
{
#if defined(MFLOCALE_ENGLISH_ONLY)
	return (c >= 'a' && c <= 'z');
#elif defined(MFLOCALE_BASIC_LATIN)
	return (c >= 'a' && c <= 'z') || (c >= 'à' && c <= 'þ');
#elif defined(MFLOCALE_BASIC_LATIN_GREEK_CRYLLIC)
	return (c >= 'a' && c <= 'z') || (c >= 'à' && c <= 'þ') || (c >= 0x03B1 && c <= 0x03CB) || (c >= 0x0430 && c <= 0x044F);
#endif
}

inline bool MFIsUpper(int c)
{
#if defined(MFLOCALE_ENGLISH_ONLY)
	return (c >= 'A' && c <= 'Z');
#elif defined(MFLOCALE_BASIC_LATIN)
	return (c >= 'A' && c <= 'Z') || (c >= 'À' && c <= 'Þ');
#elif defined(MFLOCALE_BASIC_LATIN_GREEK_CRYLLIC)
	return (c >= 'A' && c <= 'Z') || (c >= 'À' && c <= 'Þ') || (c >= 0x0391 && c <= 0x03AB) || (c >= 0x0410 && c <= 0x042F);
#endif
}

inline int MFToLower(int c)
{
	return MFIsUpper(c) ? (c | 0x20) : c;
}

inline int MFToUpper(int c)
{
	return MFIsLower(c) ? (c & ~0x20) : c;
}

inline char* MFSeekNewline(char *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline const char* MFSeekNewline(const char *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline char* MFSkipWhite(char *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline const char* MFSkipWhite(const char *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline int MFString_Length(const char *pString)
{
	const char *pT = pString;
	while(*pT) ++pT;
	return (uint32&)pT - (uint32&)pString;
}

inline char* MFString_Copy(char *pBuffer, const char *pString)
{
	char *s = pBuffer;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline char* MFString_CopyN(char *pBuffer, const char *pString, int maxChars)
{
	char *dscan;
	const char *sscan;

	dscan = pBuffer;
	sscan = pString;
	while(maxChars > 0)
	{
		--maxChars;
		if((*dscan++ = *sscan++) == '\0')
			break;
	}
	while(maxChars-- > 0)
		*dscan++ = '\0';

	return pBuffer;
}

inline char* MFString_Cat(char *pBuffer, const char *pString)
{
	char *s = pBuffer;
	while(*pBuffer) pBuffer++;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline char* MFString_CopyCat(char *pBuffer, const char *pString, const char *pString2)
{
	char *s = pBuffer;
	while((*pBuffer = *pString++)) { ++pBuffer; }
	while((*pBuffer++ = *pString2++)) { }
	return s;
}


//
// UTF8 support
//

#define RET_ILSEQ -2
#define RET_ILUNI -1

inline int MFString_GetNumBytesInMBChar(const char *pMBChar)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	const unsigned char *pMB = (const unsigned char*)pMBChar;
	unsigned char c = pMB[0];

	if(c < 0x80)
		return 1;
	else if(c < 0xc2)
		return RET_ILSEQ;
	else if(c < 0xe0)
	{
		if(!((pMB[1] ^ 0x80) < 0x40))
			return RET_ILSEQ;
		return 2;
	}
	else if(c < 0xf0)
	{
		if(!((pMB[1] ^ 0x80) < 0x40 && (pMB[2] ^ 0x80) < 0x40 && (c >= 0xe1 || pMB[1] >= 0xa0)))
			return RET_ILSEQ;
		return 3;
	}
	else
		return RET_ILSEQ;
#else
	return 1;
#endif
}

inline char *MFString_NextChar(const char *pString)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	int bytes = MFString_GetNumBytesInMBChar(pString);
	return (char*)pString + (bytes > 0 ? bytes : 1);
#else
	return pString + 1;
#endif
}

inline char *MFString_PrevChar(const char *pString)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	// theres GOTTA be a better way of doing this...
	int c = *((uint8*)pString - 1);

	if(c < 128)
		return (char*)pString-1;
	else
	{
		if((c & 0xC0) != 0x80)
			return (char*)pString-1;
		else
		{
			// 2 bytes
			int d = *((uint8*)pString - 2);

			if((d & 0xE0) == 0xC0)
			{
				return (char*)pString - 2;
			}
			else if((d & 0xC0) == 0x80)
			{
				// 3 bytes
				int e = *((uint8*)pString - 3);

				if((e & 0xF0) == 0xE0)
				{
					return (char*)pString - 3;
				}
				else
				{
					// illegal sequence
					return (char*)pString - 1;
				}
			}
			else
			{
				// illegal sequence
				return (char*)pString - 1;
			}
		}
	}
#else
	return pString - 1;
#endif
}

inline int MFString_MBToWChar(const char *pMBChar, uint16 *pWC)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	const uint8 *pMB = (const uint8*)pMBChar;
	int c = pMB[0];

	if(c < 0x80)
	{
		*pWC = (uint16)c;
		return 1;
	}
	else if(c < 0xc2)
	{
		return RET_ILSEQ;
	}
	else if(c < 0xe0)
	{
		if(!((pMB[1] ^ 0x80) < 0x40))
			return RET_ILSEQ;
		*pWC = (uint16)(((uint32) (c & 0x1f) << 6) | (uint32) (pMB[1] ^ 0x80));
		return 2;
	}
	else if(c < 0xf0)
	{
		if(!((pMB[1] ^ 0x80) < 0x40 && (pMB[2] ^ 0x80) < 0x40 && (c >= 0xe1 || pMB[1] >= 0xa0)))
			return RET_ILSEQ;
		*pWC = (uint16)(((uint32) (c & 0x0f) << 12) | ((uint32) (pMB[1] ^ 0x80) << 6) | (uint32) (pMB[2] ^ 0x80));
		return 3;
	}
	else
		return RET_ILSEQ;
#else
	*pWC = *(uint8*)pMBChar;
	return 1;
#endif
}

inline int MFString_WCharToMB(int wc, char *pMBChar)
{
	unsigned char *pMB = (unsigned char*)pMBChar;
	int count;

	if(wc < 0x80)
		count = 1;
	else if(wc < 0x800)
		count = 2;
	else if(wc < 0x10000)
		count = 3;
	else
		return RET_ILUNI;

	switch(count)
	{
		case 3: pMB[2] = (unsigned char)(0x80 | (wc & 0x3f)); wc = wc >> 6; wc |= 0x800;
		case 2: pMB[1] = (unsigned char)(0x80 | (wc & 0x3f)); wc = wc >> 6; wc |= 0xc0;
		case 1: pMB[0] = (unsigned char)wc;
	}

	return count;
}

inline int MFString_GetNumChars(const char *pString)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	int bytes = MFString_GetNumBytesInMBChar(pString);
	int numChars = 0;

	while(*pString)
	{
		if(bytes > 0)
		{
			pString += bytes;
			bytes = MFString_GetNumBytesInMBChar(pString);
		}
		else
			++pString;

		++numChars;
	}

	return numChars;
#else
	return MFString_Length(pString);
#endif
}


//
// unicode support
//

inline uint16* MFSeekNewlineW(uint16 *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline const uint16* MFSeekNewlineW(const uint16 *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline uint16* MFSkipWhiteW(uint16 *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline const uint16* MFSkipWhiteW(const uint16 *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline int MFWString_Length(const uint16 *pString)
{
	const uint16 *pT = pString;
	while(*pT) ++pT;
	return ((uint32&)pT - (uint32&)pString) >> 1;
}

inline uint16* MFWString_Copy(uint16 *pBuffer, const uint16 *pString)
{
	uint16 *s = pBuffer;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline uint16* MFWString_CopyN(uint16 *pBuffer, const uint16 *pString, int maxChars)
{
	uint16 *dscan;
	const uint16 *sscan;

	dscan = pBuffer;
	sscan = pString;
	while(maxChars > 0)
	{
		--maxChars;
		if((*dscan++ = *sscan++) == '\0')
			break;
	}
	while(maxChars-- > 0)
		*dscan++ = '\0';

	return pBuffer;
}

inline uint16* MFWString_Cat(uint16 *pBuffer, const uint16 *pString)
{
	uint16 *s = pBuffer;
	while(*pBuffer) pBuffer++;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline uint16* MFWString_CopyCat(uint16 *pBuffer, const uint16 *pString, const uint16 *pString2)
{
	uint16 *s = pBuffer;
	while((*pBuffer++ = *pString++)) { }
	while((*pBuffer++ = *pString2++)) { }
	return s;
}
