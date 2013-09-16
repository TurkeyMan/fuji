
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
	return MFIsAlpha(c) || MFIsNumeric(c);
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

inline char* MFSeekDelimiter(char *pC, const char *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				return pC;
		}
		++pC;
	}
	return pC;
}

inline const char* MFSeekDelimiter(const char *pC, const char *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				return pC;
		}
		++pC;
	}
	return pC;
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

inline char* MFSkipDelimiters(char *pC, const char *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				goto keep_going;
		}
		return pC;
	keep_going:
		++pC;
	}
	return pC;
}

inline const char* MFSkipDelimiters(const char *pC, const char *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				goto keep_going;
		}
		return pC;
	keep_going:
		++pC;
	}
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

inline size_t MFString_Length(const char *pString)
{
	const char *pT = pString;
	while(*pT) ++pT;
	return pT - pString;
}

inline size_t MFString_LengthN(const char *pString, size_t maxChars)
{
	const char *pT = pString;
	while(*pT && maxChars--) ++pT;
	return pT - pString;
}

inline char* MFString_Copy(char *pBuffer, const char *pString)
{
	char *s = pBuffer;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline char* MFString_CopyN(char *pBuffer, const char *pString, size_t maxChars)
{
	char *dscan;
	const char *sscan;

	dscan = pBuffer;
	sscan = pString;
	while(maxChars > 0)
	{
		if((*dscan++ = *sscan++) == '\0')
			break;
		--maxChars;
	}
	while(maxChars > 0)
	{
		*dscan++ = '\0';
		--maxChars;
	}

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

inline char* MFString_Chr(const char *pString, int c)
{
	do
	{
		if(*pString == (char)c)
			return (char*)pString;
	}
	while(*pString++);

	return (NULL);
}

inline char* MFString_RChr(const char *pSource, int c)
{
	char *pLast;

	for(pLast = NULL; *pSource; pSource++)
	{
		if(c == *pSource)
			pLast = (char*)pSource;
	}

	return pLast;
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
	return t == 0 ? 0 : 1;
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

inline wchar_t* MFSeekDelimiterW(wchar_t *pC, const wchar_t *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				return pC;
		}
		++pC;
	}
	return pC;
}

inline const wchar_t* MFSeekDelimiterW(const wchar_t *pC, const wchar_t *pDelimiters)
{
	while(*pC)
	{
		for(int i=0; pDelimiters[i]; ++i)
		{
			if(*pC == pDelimiters[i])
				return pC;
		}
		++pC;
	}
	return pC;
}

inline wchar_t* MFSeekNewlineW(wchar_t *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline const wchar_t* MFSeekNewlineW(const wchar_t *pC)
{
	while(!MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsNewline(*pC)) pC++;
	return pC;
}

inline wchar_t* MFSeekNextWordW(wchar_t *pC)
{
	while(!MFIsWhite(*pC) && !MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsWhite(*pC) || MFIsNewline(*pC)) pC++;
	return pC;
}

inline const wchar_t* MFSeekNextWordW(const wchar_t *pC)
{
	while(!MFIsWhite(*pC) && !MFIsNewline(*pC) && *pC != 0) pC++;
	while(MFIsWhite(*pC) || MFIsNewline(*pC)) pC++;
	return pC;
}

inline wchar_t* MFSkipWhiteW(wchar_t *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline const wchar_t* MFSkipWhiteW(const wchar_t *pC)
{
	while(MFIsWhite(*pC)) pC++;
	return pC;
}

inline size_t MFWString_Length(const wchar_t *pString)
{
	const wchar_t *pT = pString;
	while(*pT) ++pT;
	return pT - pString;
}

inline wchar_t* MFWString_Copy(wchar_t *pBuffer, const wchar_t *pString)
{
	wchar_t *s = pBuffer;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline wchar_t* MFWString_CopyN(wchar_t *pBuffer, const wchar_t *pString, size_t maxChars)
{
	wchar_t *dscan;
	const wchar_t *sscan;

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

inline wchar_t* MFWString_Cat(wchar_t *pBuffer, const wchar_t *pString)
{
	wchar_t *s = pBuffer;
	while(*pBuffer) pBuffer++;
	while((*pBuffer++ = *pString++)) { }
	return s;
}

inline wchar_t* MFWString_CopyCat(wchar_t *pBuffer, const wchar_t *pString, const wchar_t *pString2)
{
	wchar_t *s = pBuffer;
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

inline char MFString::operator[](size_t index)
{
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

inline bool MFString::BeginsWith(const char *pString) const
{
	if(!pString)
		return true;
	if(!pData)
		return (const char *)pData == pString;
	return MFString_BeginsWith(pData->pMemory, pString);
}

inline bool MFString::BeginsWith(const MFString &string) const
{
	if(!string.pData)
		return true;
	if(!pData)
		return pData == string.pData;
	return MFString_BeginsWith(pData->pMemory, string.pData->pMemory);
}

inline bool MFString::EndsWith(const char *pString) const
{
	if(!pString)
		return true;
	if(!pData)
		return (const char *)pData == pString;
	return MFString_EndsWith(pData->pMemory, pString);
}

inline bool MFString::EndsWith(const MFString &string) const
{
	if(!string.pData)
		return true;
	if(!pData)
		return pData == string.pData;
	return MFString_EndsWith(pData->pMemory, string.pData->pMemory);
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

MF_API uint32 MFUtil_HashString(const char *pString);
inline uint32 MFString::GetHash() const
{
	return pData ? MFUtil_HashString(pData->pMemory) : 0;
}

inline MFString& MFString::Insert(int offset, MFString string)
{
	return Replace(offset, 0, string);
}

inline MFArray<MFString>& MFString::SplitLines(MFArray<MFString> &output)
{
	return this->Split(output, "\r\n");
}
