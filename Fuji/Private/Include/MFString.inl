
// some useful string parsing functions
inline bool MFIsWhite(int c)
{
	return c==' ' || c=='\t';
}

inline bool MFIsAlpha(int c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}

inline bool MFIsNumeric(int c)
{
	return c>='0' && c<='9';
}

inline bool MFIsAlphaNumeric(int c)
{
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || (c=='_');
}

inline bool MFIsNewline(int c)
{
	return c=='\n' || c=='\r';
}

inline bool MFIsLower(int c)
{
	return c >= 'a' && c <= 'z';
}

inline bool MFIsUpper(int c)
{
	return c >= 'A' && c <= 'Z';
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
	while((*pBuffer++ = *pString++)) { }
	while((*pBuffer++ = *pString2++)) { }
	return s;
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
