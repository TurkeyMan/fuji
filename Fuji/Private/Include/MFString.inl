
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

inline int MFToLower(int c)
{
	return c | 0x20;
}

inline int MFToUpper(int c)
{
	return c & ~0x20;
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

inline void MFString_Copy(char *pBuffer, const char *pString)
{
	while(*pString)
	{
		*pBuffer = *pString;
		++pBuffer;
		++pString;
	}

	*pBuffer = 0;
}

inline void MFString_CopyN(char *pBuffer, const char *pString, int maxChars)
{
	while(*pString && maxChars)
	{
		*pBuffer = *pString;
		++pBuffer;
		++pString;
		--maxChars;
	}

	if(maxChars)
		*pBuffer = 0;
}
