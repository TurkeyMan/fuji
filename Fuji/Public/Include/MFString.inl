
// some useful string parsing functions
inline bool MFIsWhite(int c)
{
	return c==' ' || c=='\t' || c==0x3000;
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

inline bool MFIsHex(int c)
{
	return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F');
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
	if(pC[0] == '\r' && pC[1] == '\n')
		pC += 2;
	else if(MFIsNewline(*pC))
		++pC;
	return pC;
}

inline const char* MFSeekNewline(const char *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	if(pC[0] == '\r' && pC[1] == '\n')
		pC += 2;
	else if(MFIsNewline(*pC))
		++pC;
	return pC;
}

inline char* MFSeekNextWord(char *pC)
{
	while(!MFIsWhite(*pC) && !MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsWhite(*pC) || MFIsNewline(*pC)) pC++;
	return pC;
}

inline const char* MFSeekNextWord(const char *pC)
{
	while(!MFIsWhite(*pC) && !MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsWhite(*pC) || MFIsNewline(*pC)) pC++;
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

inline char* MFTokeniseLine(char *pString, char **ppNext)
{
	char *pC = pString;
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	if(pC[0] == '\r' && pC[1] == '\n')
	{
		pC[0] = 0;
		pC += 2;
	}
	else if(MFIsNewline(*pC))
	{
		*pC++ = 0;
	}
	if(ppNext)
		*ppNext = pC;
	return pString;
}

inline int MFString_Length(const char *pString)
{
	const char *pT = pString;
	while(*pT) ++pT;
	return (int)((uintp)pT - (uintp)pString);
}

inline int MFString_LengthN(const char *pString, int maxChars)
{
	const char *pT = pString;
	while(*pT && maxChars--) ++pT;
	return (int)((uintp)pT - (uintp)pString);
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
	while(maxChars-- > 0)
	{
		if((*dscan++ = *sscan++) == '\0')
			break;
	}
	while(maxChars-- > 0)
		*dscan++ = '\0';

	return pBuffer;
}

inline char* MFString_CopyUntil(char *pBuffer, const char *pString, int terminator)
{
	char *s = pBuffer;
	while(*pString && *pString != terminator)
		*pBuffer++ = *pString++;
	*pBuffer = 0;
	return s;
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

inline char* MFString_GetFileExtension(const char *pFilename)
{
	char *pExt = NULL;
	for(; *pFilename; pFilename++)
	{
		int c = *pFilename;
		if(c == '.')
			pExt = (char*)pFilename;
		else if(c == '/' || c == '\\')
			pExt = NULL;
	}
	return pExt;
}

inline bool MFString_EndsWith(const char *pString, const char *pEnd)
{
	int stringLen = MFString_Length(pString);
	int endLen = MFString_Length(pEnd);

	if(endLen > stringLen)
		return false;

	return !MFString_Compare(pString + (stringLen - endLen), pEnd);
}

inline bool MFString_BeginsWith(const char *pString, const char *pBegin)
{
	while(*pBegin)
	{
		if(*pString != *pBegin)
			return false;
		++pBegin;
		++pString;
	}

	return true;
}

inline const char* MFStr_GetFileName(const char *pFilename)
{
	const char *pSlash = MFString_RChr(pFilename, '/');
	const char *pSlash2 = MFString_RChr(pFilename, '\\');
	pSlash = pSlash > pSlash2 ? pSlash : pSlash2;
	return pSlash ? pSlash + 1 : pFilename;
}

inline const char* MFStr_GetFileNameWithoutExtension(const char *pFilename)
{
	const char *pNoPath = MFStr_GetFileName(pFilename);
	const char *pDot = MFString_RChr(pNoPath, '.');
	return pDot ? MFStrN(pNoPath, pDot - pNoPath) : pNoPath;
}

inline const char* MFStr_GetFilePath(const char *pFilename)
{
	const char *pSlash = MFString_RChr(pFilename, '/');
	const char *pSlash2 = MFString_RChr(pFilename, '\\');
	pSlash = pSlash > pSlash2 ? pSlash : pSlash2;
	return pSlash ? MFStrN(pFilename, (pSlash + 1) - pFilename) : "";
}

inline const char* MFStr_TruncateExtension(const char *pFilename)
{
	const char *pDot = MFString_RChr(pFilename, '.');
	return pDot ? MFStrN(pFilename, pDot - pFilename) : pFilename;
}


//
// UTF8 support
//

inline int MFString_EncodeUTF8(int c, char *pMBChar)
{
	int count = 0;

	if(c < 0x80)
		count = 1;
	else if(c < 0x800)
		count = 2;
	else if(c < 0x10000)
		count = 3;

	if(!pMBChar)
		return count;

	uint8 *pMB = (uint8*)pMBChar;
	switch(count)
	{
		case 3: pMB[2] = (uint8)(0x80 | (c & 0x3f)); c >>= 6; c |= 0x800;
		case 2: pMB[1] = (uint8)(0x80 | (c & 0x3f)); c >>= 6; c |= 0xc0;
		case 1: pMB[0] = (uint8)c;
	}

	return count;
}

inline int MFString_DecodeUTF8(const char *pMBChar, int *pC)
{
	const uint8 *pMB = (const uint8*)pMBChar;
	int t = *pMB;

	if(t >= 128)
	{    
		if((t&0xE0) == 0xC0)
		{
			if(pC)
				*pC = ((int)(t&0x1F) << 6) | (pMB[1]&0x3F);
			return 2;
		}
		else if((t&0xF0) == 0xE0)
		{
			if(pC)
				*pC = ((int)(t&0x0F) << 12) | ((int)(pMB[1]&0x3F) << 6) | (pMB[2]&0x3F);
			return 3;
		}
	}

	if(pC)
		*pC = t;
	return 1;
}

inline char *MFString_NextChar(const char *pChar)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	int t = *(uint8*)pChar;
	if(t >= 128)
	{
		if((t&0xE0) == 0xC0)
			return (char*)pChar+2;
		else if((t&0xF0) == 0xE0)
			return (char*)pChar+3;
	}
	return (char*)pChar+1;
#else
	return pChar+1;
#endif
}

inline char *MFString_PrevChar(const char *pChar)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	int t = ((uint8*)pChar)[-1];
	if(t >= 128)
	{
		if((t&0xC0) == 0x80 && (pChar[-2]&0xE0) == 0xC0)
			return (char*)pChar-2;
		else if((t&0xC0) == 0x80 && (pChar[-2]&0xC0) == 0x80 && (pChar[-3]&0xF0) == 0xE0)
			return (char*)pChar-3;
	}
	return (char*)pChar-1;
#else
	return pChar-1;
#endif
}

inline int MFString_GetNumChars(const char *pString)
{
#if defined(MFLOCALE_UTF8_SUPPORT)
	int numChars = 0;
	while(*pString)
	{
		++numChars;
		pString = MFString_NextChar(pString);
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
	return (int)(((uintp)pT - (uintp)pString) >> 1);
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


///// MFString Functions /////

struct MFStringData
{
	friend class MFString;
public:
	int GetRefCount() const { return refCount; }
	size_t GetBytes() const { return bytes + 1; }
	size_t GetAllocated() const { return allocated; }
	const char *GetMemory() const { return pMemory; }

private:
	static MFStringData *Alloc();
	void Destroy();

	int AddRef() { ++refCount;  return refCount; }
	int Release() { if(--refCount == 0) { Destroy(); return 0; } return refCount; }

	// members
	char *pMemory;
	size_t bytes;
	size_t allocated;
	int refCount;
};

inline MFString::MFString()
{
	pData = NULL;
}

inline MFString::MFString(const MFString &string)
{
	pData = string.pData;
	if(pData)
		pData->AddRef();
}

inline MFString::~MFString()
{
	if(pData)
		pData->Release();
}

inline MFString MFString::Static(const char *pStatic)
{
	MFString t(pStatic, true);
	return t;
}

inline bool MFString::operator!() const
{
	return !pData;
}

inline MFString::operator bool() const
{
	return pData != NULL;
}

inline char MFString::operator[](int index)
{
	if(!pData)
		return 0;
	return pData->pMemory[index];
}

inline bool MFString::operator==(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData == pString;
	return MFString_Compare(pData->pMemory, pString) == 0;
}

inline bool MFString::operator==(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData == string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) == 0;
}

inline bool MFString::operator!=(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData != pString;
	return MFString_Compare(pData->pMemory, pString) != 0;
}

inline bool MFString::operator!=(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData != string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) != 0;
}

inline bool MFString::operator<=(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData <= pString;
	return MFString_Compare(pData->pMemory, pString) <= 0;
}

inline bool MFString::operator<=(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData <= string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) <= 0;
}

inline bool MFString::operator>=(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData >= pString;
	return MFString_Compare(pData->pMemory, pString) >= 0;
}

inline bool MFString::operator>=(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData >= string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) >= 0;
}

inline bool MFString::operator<(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData < pString;
	return MFString_Compare(pData->pMemory, pString) < 0;
}

inline bool MFString::operator<(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData < string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) < 0;
}

inline bool MFString::operator>(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData > pString;
	return MFString_Compare(pData->pMemory, pString) > 0;
}

inline bool MFString::operator>(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData > string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) > 0;
}

inline bool MFString::Equals(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData == pString;
	return MFString_Compare(pData->pMemory, pString) == 0;
}

inline bool MFString::Equals(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData == string.pData;
	return MFString_Compare(pData->pMemory, string.pData->pMemory) == 0;
}

inline bool MFString::EqualsInsensitive(const char *pString) const
{
	if(!pData || !pString)
		return (const char *)pData == pString;
	return MFString_CaseCmp(pData->pMemory, pString) == 0;
}

inline bool MFString::EqualsInsensitive(const MFString &string) const
{
	if(!pData || !string.pData)
		return pData == string.pData;
	return MFString_CaseCmp(pData->pMemory, string.pData->pMemory) == 0;
}

inline MFString MFString::Duplicate() const
{
	MFString t(*this);
	return t;
}

inline int MFString::ToInt(int base) const
{
	return pData ? MFString_AsciiToInteger(pData->pMemory, true, base) : 0;
}

inline float MFString::ToFloat() const
{
	return pData ? MFString_AsciiToFloat(pData->pMemory) : 0.f;
}

inline const char *MFString::CStr() const
{
	return pData ? pData->pMemory : NULL;
}

inline int MFString::NumBytes() const
{
	return pData ? (int)pData->bytes : 0;
}

inline int MFString::NumChars() const
{
	return pData ? MFString_GetNumChars(pData->pMemory) : 0;
}

inline bool MFString::IsNull() const
{
	return pData == NULL;
}

inline bool MFString::IsEmpty() const
{
	return !pData || pData->bytes == 0;
}

inline bool MFString::IsNumeric() const
{
	return pData ? MFString_IsNumber(pData->pMemory) : false;
}

inline uint32 MFString::GetHash() const
{
	uint32 MFUtil_HashString(const char *pString);
	return pData ? MFUtil_HashString(pData->pMemory) : 0;
}

inline MFString& MFString::Insert(int offset, MFString string)
{
	return Replace(offset, 0, string);
}
