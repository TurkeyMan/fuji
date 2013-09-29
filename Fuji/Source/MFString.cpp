#include "Fuji.h"
#include "MFString.h"
#include "MFHeap.h"
#include "MFObjectPool.h"

#include <stdio.h>
#include <stdarg.h>
#define  stricmp strcasecmp

#include <string.h>

MFALIGN_BEGIN(16)
static char gStringBuffer[1024*128]
MFALIGN_END(16);

static uint32 gStringOffset;

static MFObjectPool stringPool;
static MFObjectPoolGroup stringHeap;

static const MFObjectPoolGroupConfig gStringGroups[] =
{
	{ 5, 256, 128 },
	{ 16, 512, 128 },
	{ 128, 32, 16 },
	{ 1024, 4, 4 }
};

MFInitStatus MFString_InitModule()
{
	stringHeap.Init(gStringGroups, sizeof(gStringGroups) / sizeof(gStringGroups[0]));
	stringPool.Init(sizeof(MFStringData), 128, 128);

	return MFIS_Succeeded;
}

void MFString_DeinitModule()
{
	stringPool.Deinit();
	stringHeap.Deinit();
}

MF_API MFString MFString_GetStats()
{
	size_t overhead = stringPool.GetTotalMemory() + stringPool.GetOverheadMemory() + stringHeap.GetOverheadMemory();
	size_t waste = 0, averageSize = 0;

	// calculate waste
	int numStrings = stringPool.GetNumAllocated();
	for(int a=0; a<numStrings; ++a)
	{
		MFStringData *pString = (MFStringData*)stringPool.GetItem(a);
		size_t allocated = pString->GetAllocated();
		if(allocated)
		{
			size_t bytes = pString->GetBytes();
			averageSize += bytes;
			waste += allocated - bytes;
		}
	}

	if(numStrings)
		averageSize /= numStrings;

	MFString desc;
    desc.Reserve(1024);
	desc = MFStr("String heap memory: " MFFMT_SIZE_T " allocated/" MFFMT_SIZE_T " reserved + " MFFMT_SIZE_T " overhead  Waste: " MFFMT_SIZE_T "  Average length: " MFFMT_SIZE_T "\n\tPool: %d/%d", stringHeap.GetAllocatedMemory(), stringHeap.GetTotalMemory(), overhead, waste, averageSize, stringPool.GetNumAllocated(), stringPool.GetNumReserved());

	int numGroups = stringHeap.GetNumPools();
	for(int a=0; a<numGroups; ++a)
	{
		MFObjectPool *pPool = stringHeap.GetPool(a);
		desc += MFStr("\n\t\t" MFFMT_SIZE_T " byte: %d/%d", pPool->GetObjectSize(), pPool->GetNumAllocated(), pPool->GetNumReserved());
	}

	return desc;
}

MF_API void MFString_Dump()
{
	MFString temp = MFString_GetStats();

	MFDebug_Log(1, "\n-------------------------------------------------------------------------------------------------------");
	MFDebug_Log(1, temp.CStr());

	// dump all strings...
	MFDebug_Log(1, "");

	int numStrings = stringPool.GetNumAllocated();
	for(int a=0; a<numStrings; ++a)
	{
		MFStringData *pString = (MFStringData*)stringPool.GetItem(a);
		MFDebug_Log(1, MFStr("%d refs, " MFFMT_SIZE_T "b: \"%s\"", pString->GetRefCount(), pString->GetAllocated(), pString->GetMemory()));
	}
}

MF_API void *MFCopyMemory(void *pDest, const void *pSrc, size_t size)
{
	return memcpy(pDest, pSrc, size);
}

MF_API void *MFMemSet(void *pDest, int value, size_t size)
{
	return memset(pDest, value, size);
}

MF_API void *MFZeroMemory(void *pDest, size_t size)
{
	return memset(pDest, 0, size);
}

MF_API int MFMemCompare(const void *pBuf1, const void *pBuf2, size_t size)
{
	return memcmp(pBuf1, pBuf2, size);
}

MF_API char* MFString_Dup(const char *pString)
{
	int len = MFString_Length(pString);
	char *pNew = (char*)MFHeap_Alloc(len + 1);
	MFString_Copy(pNew, pString);
	return pNew;
}

MF_API const char * MFString_ToLower(const char *pString)
{
	char *pBuffer = &gStringBuffer[gStringOffset];
	int len = MFString_Length(pString);

	gStringOffset += len+1;

	char *pT = pBuffer;
	while(*pString)
	{
		*pT = (char)MFToLower(*pString);
		++pT;
		++pString;
	}

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	return pBuffer;
}

MF_API const char * MFString_ToUpper(const char *pString)
{
	char *pBuffer = &gStringBuffer[gStringOffset];
	int len = MFString_Length(pString);

	gStringOffset += len+1;

	char *pT = pBuffer;
	while(*pString)
	{
		*pT = (char)MFToUpper(*pString);
		++pT;
		++pString;
	}

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	return pBuffer;
}

MF_API const char * MFStr(const char *format, ...)
{
	va_list arglist;
	char *pBuffer = &gStringBuffer[gStringOffset];
	int nRes = 0;

	va_start(arglist, format);

	nRes = vsprintf(pBuffer, format, arglist);
	gStringOffset += nRes+1;

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	va_end(arglist);

	return pBuffer;
}

MF_API const char * MFStrN(const char *pSource, size_t n)
{
	char *pBuffer = &gStringBuffer[gStringOffset];

	MFString_CopyN(pBuffer, pSource, (int)n);
	pBuffer[n] = 0;

	gStringOffset += (uint32)n+1;

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	return pBuffer;
}

MF_API int MFString_Compare(const char *pString1, const char *pString2)
{
	while(*pString1 == *pString2++)
	{
		if(*pString1++ == 0)
			return 0;
	}

	return (*(const unsigned char *)pString1 - *(const unsigned char *)(pString2 - 1));
}

MF_API int MFString_CompareN(const char *pString1, const char *pString2, size_t n)
{
	if(n == 0)
		return 0;

	do
	{
		if(*pString1 != *pString2++)
			return (*(const unsigned char *)pString1 - *(const unsigned char *)(pString2 - 1));

		if(*pString1++ == 0)
			break;
	}
	while(--n != 0);

	return 0;
}

MF_API int MFString_CaseCmp(const char *pSource1, const char *pSource2)
{
	register unsigned int c1, c2;

	do
	{
		c1 = MFToUpper(*pSource1++);
		c2 = MFToUpper(*pSource2++);
	}
	while(c1 && (c1 == c2));

	return c1 - c2;
}

MF_API int MFString_CaseCmpN(const char *pSource1, const char *pSource2, size_t n)
{
	register int c = 0;

	while(n)
	{
		if((c = MFToUpper(*pSource1) - MFToUpper(*pSource2++) ) != 0 || !*pSource1++)
			break;
		n--;
	}

	return c;
}

MF_API bool MFString_PatternMatch(const char *pPattern, const char *pFilename, const char **ppMatchDirectory, bool bCaseSensitive)
{
	if(!pPattern || !pFilename)
		return false;

	while(*pPattern && *pFilename)
	{
		if(*pPattern == '?')
		{
			if(*pFilename == '/' || *pFilename == '\\')
				break;
		}
		else if(*pPattern == '*')
		{
			++pPattern;

			bool match = false;
			while(*pFilename)
			{
				match = MFString_PatternMatch(pPattern, pFilename, ppMatchDirectory);
				if(match)
					break;
				if(*pFilename == '/' || *pFilename == '\\')
					break;
				++pFilename;
			}

			return match;
		}
		else if((bCaseSensitive && *pPattern != *pFilename) || (!bCaseSensitive && MFToLower(*pPattern) != MFToLower(*pFilename)))
			break;

		++pPattern;
		++pFilename;
	}

	if(*pPattern == 0)
	{
		if((*pFilename == '/' || *pFilename == '\\') && ppMatchDirectory)
			*ppMatchDirectory = pFilename + 1;

		if((ppMatchDirectory && *ppMatchDirectory) || *pFilename == 0)
			return true;
	}

	return false;
}

MF_API const char* MFStr_URLEncodeString(const char *pString, const char *pExcludeChars)
{
	char *pBuffer = &gStringBuffer[gStringOffset];

	int sourceLen = MFString_Length(pString);
	int destLen = 0;

	for(int a=0; a<sourceLen; ++a)
	{
		int c = (uint8)pString[a];
		if(MFIsAlphaNumeric(c) || MFString_Chr("-_.!~*'()", c) || (pExcludeChars && MFString_Chr(pExcludeChars, c)))
			pBuffer[destLen++] = (char)c;
		else if(c == ' ')
			pBuffer[destLen++] = '+';
		else
			destLen += sprintf(pBuffer + destLen, "%%%02X", c);
	}

	pBuffer[destLen] = 0;

	gStringOffset += destLen+1;
	if(gStringOffset >= sizeof(gStringBuffer) - 1024)
		gStringOffset = 0;

	return pBuffer;
}

MF_API size_t MFString_URLEncode(char *pDest, const char *pString, const char *pExcludeChars)
{
	size_t sourceLen = MFString_Length(pString);
	size_t destLen = 0;

	for(size_t a=0; a<sourceLen; ++a)
	{
		int c = (uint8)pString[a];
		if(MFIsAlphaNumeric(c) || MFString_Chr("-_.!~*'()", c) || (pExcludeChars && MFString_Chr(pExcludeChars, c)))
		{
			if(pDest)
				pDest[destLen] = (char)c;
			destLen++;
		}
		else if(c == ' ')
		{
			if(pDest)
				pDest[destLen] = '+';
			destLen++;
		}
		else
		{
			if(pDest)
				destLen += sprintf(pDest + destLen, "%%%02X", c);
			else
				destLen += 3; // *** surely this can't write more than 3 chars? '%xx'
		}
	}

	if(pDest)
		pDest[destLen] = 0;

	return destLen;
}

MF_API bool MFString_IsNumber(const char *pString, bool bAllowHex)
{
	pString = MFSkipWhite(pString);

	int numDigits = 0;

	if(bAllowHex && pString[0] == '0' && pString[1] == 'x')
	{
		// hex number
		pString += 2;
		while(*pString)
		{
			if(!MFIsHex(*pString++))
				return false;
			++numDigits;
		}
	}
	else
	{
		// decimal number
		if(*pString == '-' || *pString == '+')
			++pString;

		bool bHasDot = false;
		while(*pString)
		{
			if(!MFIsNumeric(*pString) && (bHasDot || *pString != '.'))
				return false;
			if(*pString++ == '.')
			{
				bHasDot = true;
				numDigits = 0;
			}
			else
				++numDigits;
		}
	}

	return numDigits > 0 ? true : false;
}

MF_API int MFString_AsciiToInteger(const char *pString, bool bDetectBase, int base, const char **ppNextChar)
{
	pString = MFSkipWhite(pString);

	int number = 0;

	if(base == 16 || (bDetectBase && ((pString[0] == '0' && pString[1] == 'x') || pString[0] == '$')))
	{
		// hex number
		if(pString[0] == '0' && pString[1] == 'x')
			pString += 2;
		else if(pString[0] == '$')
			pString += 1;

		while(*pString)
		{
			int digit = *pString++;
			if(!MFIsHex(digit))
				return number;
			number <<= 4;
			number += MFIsNumeric(digit) ? digit - '0' : MFToLower(digit) - 'a' + 10;
		}
	}
	else if(base == 2 || (bDetectBase && pString[0] == 'b'))
	{
		if(pString[0] == 'b')
			++pString;

		while(*pString == '0' || *pString == '1')
		{
			number <<= 1;
			number |= *pString - '0';
		}
	}
	else if(base == 10)
	{
		// decimal number
		bool neg = false;
		if(*pString == '-' || *pString == '+')
		{
			neg = *pString == '-';
			++pString;
		}

		while(*pString)
		{
			if(!MFIsNumeric(*pString))
				return neg ? -number : number;
			number = number*10 + (*pString++) - '0';
		}

		if(neg)
			number = -number;
	}

	if(ppNextChar)
		*ppNextChar = pString;

	return number;
}

MF_API float MFString_AsciiToFloat(const char *pString, const char **ppNextChar)
{
	pString = MFSkipWhite(pString);

	int64 number = 0;
	float frac = 1;

	// floating poiont number
	bool neg = false;
	if(*pString == '-' || *pString == '+')
	{
		neg = *pString == '-';
		++pString;
	}

	bool bHasDot = false;
	while(*pString)
	{
		int digit = *pString++;
		if(!MFIsNumeric(digit) && (bHasDot || digit != '.'))
			return (float)(neg ? -number : number) * frac;
		if(digit == '.')
			bHasDot = true;
		else
		{
			number = number*10 + digit - '0';
			if(bHasDot)
				frac *= 0.1f;
		}
	}

	if(neg)
		number = -number;

	if(ppNextChar)
		*ppNextChar = pString;

	return (float)number * frac;
}

MF_API int MFString_Enumerate(const char *pString, const char *const *ppKeys, size_t numKeys, bool bCaseSensitive)
{
	for(size_t i=0; i<numKeys; ++i)
	{
		if(bCaseSensitive ? !MFString_Compare(pString, ppKeys[i]) : !MFString_CaseCmp(pString, ppKeys[i]))
			return i;
	}
	return -1;
}

#if 0

char* MFString_Copy(char *pDest, const char *pSrc)
{
#if !defined(PREFER_SPEED_OVER_SIZE)
	char *s = pDest;
	while(*pDest++ = *pSrc++) { }
	return s;
#else
	char *dst = dst0;
	_CONST char *src = src0;
	long *aligned_dst;
	_CONST long *aligned_src;

	/* If SRC or DEST is unaligned, then copy bytes.  */
	if (!UNALIGNED (src, dst))
	{
		aligned_dst = (long*)dst;
		aligned_src = (long*)src;

		/* SRC and DEST are both "long int" aligned, try to do "long int"
		sized copies.  */
		while (!DETECTNULL(*aligned_src))
		{
			*aligned_dst++ = *aligned_src++;
		}

		dst = (char*)aligned_dst;
		src = (char*)aligned_src;
	}

	while (*dst++ = *src++)
		;
	return dst0;
#endif
}

char* MFString_CopyN(char *pDest, const char *pSrc, int n)
{
#if !defined(PREFER_SPEED_OVER_SIZE)
	char *dscan;
	const char *sscan;

	dscan = pDest;
	sscan = pSrc;
	while(n > 0)
	{
		--n;
		if((*dscan++ = *sscan++) == '\0')
			break;
	}
	while(n-- > 0)
		*dscan++ = '\0';

	return pDest;
#else
	char *dst = dst0;
	_CONST char *src = src0;
	long *aligned_dst;
	_CONST long *aligned_src;

	/* If SRC and DEST is aligned and count large enough, then copy words.  */
	if(!UNALIGNED (src, dst) && !TOO_SMALL (count))
	{
		aligned_dst = (long*)dst;
		aligned_src = (long*)src;

		/* SRC and DEST are both "long int" aligned, try to do "long int"
		sized copies.  */
		while(count >= sizeof (long int) && !DETECTNULL(*aligned_src))
		{
			count -= sizeof (long int);
			*aligned_dst++ = *aligned_src++;
		}

		dst = (char*)aligned_dst;
		src = (char*)aligned_src;
	}

	while(count > 0)
	{
		--count;
		if((*dst++ = *src++) == '\0')
			break;
	}

	while(count-- > 0)
		*dst++ = '\0';

	return dst0;
#endif
}

char* MFString_Cat(char *pDest, const char *pSrc)
{
#if !defined(PREFER_SPEED_OVER_SIZE)
	char *s = pDest;
	while(*pDest) pDest++;
	while(*pDest++ = *pSrc++) { }
	return s;
#else
	char *s = s1;


	/* Skip over the data in s1 as quickly as possible.  */
	if (ALIGNED (s1))
	{
		unsigned long *aligned_s1 = (unsigned long *)s1;
		while (!DETECTNULL (*aligned_s1))
			aligned_s1++;

		s1 = (char *)aligned_s1;
	}

	while (*s1)
		s1++;

	/* s1 now points to the its trailing null character, we can
	just use strcpy to do the work for us now.

	?!? We might want to just include strcpy here.
	Also, this will cause many more unaligned string copies because
	s1 is much less likely to be aligned.  I don't know if its worth
	tweaking strcpy to handle this better.  */
	MFString_Copy(s1, s2);

	return s;
#endif
}

char* MFString_CopyCat(char *pDest, const char *pSrc, const char *pSrc2)
{
#if !defined(PREFER_SPEED_OVER_SIZE)
	char *s = pDest;
	while(*pDest = *pSrc++) { ++pDest; }
	while(*pDest++ = *pSrc2++) { }
	return s;
#else
	char *dst = dst0;
	_CONST char *src = src0;
	long *aligned_dst;
	_CONST long *aligned_src;

	/* If SRC or DEST is unaligned, then copy bytes.  */
	if (!UNALIGNED (src, dst))
	{
		aligned_dst = (long*)dst;
		aligned_src = (long*)src;

		/* SRC and DEST are both "long int" aligned, try to do "long int"
		sized copies.  */
		while (!DETECTNULL(*aligned_src))
		{
			*aligned_dst++ = *aligned_src++;
		}

		dst = (char*)aligned_dst;
		src = (char*)aligned_src;
	}

	while (*dst++ = *src++)
		;
	return dst0;
#endif /* not PREFER_SIZE_OVER_SPEED */
}

#endif


//
// UTF8 support
//

MF_API size_t MFWString_CopyUTF8ToUTF16(wchar_t *pBuffer, const char *pString)
{
	const wchar_t *pStart = pBuffer;
	int bytes;

	while(*pString)
	{
		*pBuffer++ = (wchar_t)MFString_DecodeUTF8(pString, &bytes);
		pString += bytes;
	}
	*pBuffer = 0;

	return pBuffer - pStart;
}

MF_API size_t MFString_CopyUTF16ToUTF8(char *pBuffer, const wchar_t *pString)
{
	const wchar_t *pStart = pString;

	while(*pString)
		pBuffer += MFString_EncodeUTF8(*pString++, pBuffer);
	*pBuffer = 0;

	return pString - pStart;
}

MF_API wchar_t* MFString_UFT8AsWChar(const char *pUTF8String, size_t *pNumChars)
{
	// count number of actual characters in the string
	size_t numChars = MFString_GetNumChars(pUTF8String);

	// get some space in the MFStr buffer
	if(gStringOffset & 1)
		++gStringOffset;

	wchar_t *pBuffer = (wchar_t*)&gStringBuffer[gStringOffset];
	gStringOffset += numChars*2 + 2;

	// if we wrapped the string buffer
	if(gStringOffset >= sizeof(gStringBuffer) - 1024)
	{
		gStringOffset = 0;
		pBuffer = (wchar_t*)gStringBuffer;
	}

	// copy the string
	MFWString_CopyUTF8ToUTF16(pBuffer, pUTF8String);

	if(pNumChars)
		*pNumChars = numChars;

	return pBuffer;
}


//
// unicode support
//

int MFWString_Compare(const wchar_t *pString1, const wchar_t *pString2)
{
	while(*pString1 == *pString2++)
	{
		if(*pString1++ == 0)
			return 0;
	}

	return (*(const uint16 *)pString1 - *(const uint16 *)(pString2 - 1));
}

int MFWString_CaseCmp(const wchar_t *pSource1, const wchar_t *pSource2)
{
	register unsigned int c1, c2;

	do
	{
		c1 = MFToUpper(*pSource1++);
		c2 = MFToUpper(*pSource2++);
	}
	while(c1 && (c1 == c2));

	return c1 - c2;
}


/**** MFString Functions ****/

MFStringData *MFStringData::Alloc()
{
	MFStringData *pData = (MFStringData*)stringPool.Alloc();
	pData->refCount = 1;
	pData->pMemory = NULL;
	pData->allocated = pData->bytes = 0;
	return pData;
}

void MFStringData::Destroy()
{
	if(allocated)
		stringHeap.Free(pMemory);
	stringPool.Free(this);
}

MFString::MFString(const char *pString, bool bHoldStaticPointer)
{
	if(!pString)
	{
		pData = NULL;
	}
	else
	{
		pData = MFStringData::Alloc();
		pData->bytes = MFString_Length(pString);
		if(bHoldStaticPointer)
		{
			pData->pMemory = (char*)pString;
		}
		else
		{
			pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
			MFString_Copy(pData->pMemory, pString);
		}
	}
}

MFString::MFString(const char *pString, size_t maxChars)
{
	if(!pString)
	{
		pData = NULL;
	}
	else
	{
		pData = MFStringData::Alloc();
		pData->bytes = MFString_LengthN(pString, (int)maxChars);

		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);

		MFString_CopyN(pData->pMemory, pString, (int)pData->bytes);
		pData->pMemory[pData->bytes] = 0;
	}
}

MFString& MFString::operator=(const char *pString)
{
	if(pString)
	{
		int bytes = MFString_Length(pString);

		Reserve(bytes + 1, true);

		MFString_Copy(pData->pMemory, pString);
		pData->bytes = bytes;
	}
	else if(pData)
	{
		pData->Release();
		pData = NULL;
	}

	return *this;
}

MFString& MFString::operator=(const MFString &string)
{
	if(pData)
		pData->Release();

	pData = string.pData;
	if(pData)
		pData->AddRef();

	return *this;
}

MFString& MFString::operator+=(char c)
{
	Reserve(NumBytes() + 2);
	pData->pMemory[pData->bytes++] = c;
	pData->pMemory[pData->bytes] = 0;
	return *this;
}

MFString& MFString::operator+=(const char *pString)
{
	if(!pString || *pString == 0)
		return *this;

	if(IsEmpty())
	{
		*this = pString;
	}
	else
	{
		size_t sumBytes = pData->bytes + MFString_Length(pString);
		Reserve(sumBytes + 1);
		MFString_Copy(pData->pMemory + pData->bytes, pString);
		pData->bytes = sumBytes;
	}

	return *this;
}

MFString& MFString::operator+=(const MFString &string)
{
	if(string.IsEmpty())
		return *this;

	if(IsEmpty())
	{
		*this = string;
	}
	else
	{
		size_t sumBytes = pData->bytes + string.pData->bytes;
		Reserve(sumBytes + 1);
		MFString_Copy(pData->pMemory + pData->bytes, string.pData->pMemory);
		pData->bytes = sumBytes;
	}

	return *this;
}

MFString MFString::operator+(char c) const
{
	MFString s = *this;
	s += c;
	return s;
}

MFString MFString::operator+(const char *pString) const
{
	if(!pString || *pString == 0)
		return *this;

	if(IsEmpty())
		return MFString(pString);

	size_t bytes = pData->bytes + MFString_Length(pString);

	MFString t;
    t.Reserve(bytes + 1);
	MFString_CopyCat(t.pData->pMemory, pData->pMemory, pString);
	t.pData->bytes = bytes;

	return t;
}

MFString MFString::operator+(const MFString &string) const
{
	if(string.IsEmpty())
		return *this;

	if(IsEmpty())
		return string;

	size_t bytes = pData->bytes + string.pData->bytes;

	MFString t;
    t.Reserve(bytes + 1);
	MFString_CopyCat(t.pData->pMemory, pData->pMemory, string.pData->pMemory);
	t.pData->bytes = bytes;

	return t;
}

MFString operator+(const char *pString, const MFString &string)
{
	if(string.IsEmpty())
		return MFString(pString);

	if(!pString || *pString == 0)
		return string;

	return MFString::Static(pString) + string;
}

MFString& MFString::SetStaticString(const char *pStaticString)
{
	if(pData)
	{
		pData->Release();
		pData = NULL;
	}

	if(pStaticString)
	{
		pData = MFStringData::Alloc();
		pData->bytes = MFString_Length(pStaticString);
		pData->pMemory = (char*)pStaticString;
	}

	return *this;
}

MFString& MFString::FromUTF16(const wchar_t *pString)
{
	if(pData)
	{
		pData->Release();
		pData = NULL;
	}

	if(pString)
	{
		pData = MFStringData::Alloc();

		size_t len = 0;
		const wchar_t *pS = pString;
		while(*pS)
			len += MFString_EncodeUTF8(*pS++, NULL);
		pData->bytes = len;

		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
		MFString_CopyUTF16ToUTF8(pData->pMemory, (const wchar_t*)pString);
	}

	return *this;
}

MFString& MFString::Detach(size_t reserveBytes)
{
	if(pData && (pData->refCount > 1 || pData->allocated == 0))
	{
		MFStringData *pNew = MFStringData::Alloc();
		pNew->bytes = pData->bytes;
		pNew->pMemory = (char*)stringHeap.Alloc(MFMax(pNew->bytes + 1, reserveBytes), &pNew->allocated);
		MFString_Copy(pNew->pMemory, pData->pMemory);

		pData->Release();
		pData = pNew;
	}

	return *this;
}

MFString& MFString::Reserve(size_t bytes, bool bClearString)
{
	// detach instance
	Detach(bytes);

	// allocate memory
	if(!pData)
	{
		pData = MFStringData::Alloc();
		pData->pMemory = (char*)stringHeap.Alloc(bytes, &pData->allocated);
		pData->pMemory[0] = 0;
		pData->bytes = 0;
	}
	else
	{
		if(bytes > pData->allocated)
		{
			bool bNeedFree = pData->allocated != 0;

			char *pNew = (char*)stringHeap.Alloc(bytes, &pData->allocated);

			if(!bClearString)
				MFString_Copy(pNew, pData->pMemory);

			if(bNeedFree)
				stringHeap.Free(pData->pMemory);
			pData->pMemory = pNew;
		}

		if(bClearString)
		{
			pData->bytes = 0;
			pData->pMemory[0] = 0;
		}
	}

	return *this;
}

MFString& MFString::Sprintf(const char *pFormat, ...)
{
	if(pData)
	{
		pData->Release();
		pData = NULL;
	}

	va_list arglist;
	va_start(arglist, pFormat);

	int nRes = vsnprintf(NULL, 0, pFormat, arglist);
	va_start(arglist, pFormat);

	if(nRes >= 0)
	{
		pData = MFStringData::Alloc();
		pData->bytes = nRes;
		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
		vsprintf(pData->pMemory, pFormat, arglist);
	}

	va_end(arglist);

	return *this;
}

MFString MFString::Format(const char *pFormat, ...)
{
	MFString t;

	va_list arglist;
	va_start(arglist, pFormat);

	int nRes = vsnprintf(NULL, 0, pFormat, arglist);
	va_start(arglist, pFormat);

	if(nRes >= 0)
	{
		t.pData = MFStringData::Alloc();
		t.pData->bytes = nRes;
		t.pData->pMemory = (char*)stringHeap.Alloc(t.pData->bytes + 1, &t.pData->allocated);
		vsprintf(t.pData->pMemory, pFormat, arglist);
	}

	va_end(arglist);

	return t;
}

MFString& MFString::FromInt(int number)
{
	Sprintf("%d", number);
	return *this;
}

MFString& MFString::FromFloat(float number)
{
	Sprintf("%g", number);
	return *this;
}

MFString MFString::Upper() const
{
	if(!pData)
		return *this;

	// allocate a new string
	MFString t;
    t.Reserve(pData->bytes + 1);
	t.pData->bytes = pData->bytes;

	// copy upper case
	for(size_t a=0; a<pData->bytes + 1; ++a)
		t.pData->pMemory[a] = (char)MFToUpper(pData->pMemory[a]);

	return t;
}

MFString MFString::Lower() const
{
	if(!pData)
		return *this;

	// allocate a new string
	MFString t;
    t.Reserve(pData->bytes + 1);
	t.pData->bytes = pData->bytes;

	// copy lower case string
	for(size_t a=0; a<pData->bytes + 1; ++a)
		t.pData->pMemory[a] = (char)MFToLower(pData->pMemory[a]);

	return t;
}

MFString& MFString::Trim(bool bFront, bool bEnd, const char *pCharacters)
{
	if(pData)
	{
		const char *pSrc = pData->pMemory;
		size_t offset = 0;

		// trim start
		if(bFront)
		{
			while(pSrc[offset] && MFString_Chr(pCharacters, pSrc[offset]))
				++offset;
		}

		size_t count = pData->bytes - offset;

		// trim end
		if(bEnd)
		{
			const char *pEnd = pSrc + offset + count - 1;
			while(count && MFString_Chr(pCharacters, *pEnd))
			{
				--count;
				--pEnd;
			}
		}

		*this = SubStr((int)offset, (int)count);
	}

	return *this;
}

MFString& MFString::PadLeft(int minLength, const char *pPadding)
{
	// check if the string is already long enough
	int len = NumBytes();
	if(len >= minLength)
		return *this;

	// reserve enough memory
	Reserve(minLength + 1);
	pData->bytes = minLength;

	// move string
	int preBytes = minLength - len;
	for(int a=len; a>=0; --a)
		pData->pMemory[a + preBytes] = pData->pMemory[a];

	// pre-pad the string
	int padLen = MFString_Length(pPadding);
	for(int a=0, b=0; a<preBytes; ++a, ++b)
	{
		if(b >= padLen)
			b = 0;

		pData->pMemory[a] = pPadding[b];
	}

	return *this;
}

MFString& MFString::PadRight(int minLength, const char *pPadding, bool bAlignPadding)
{
	// check if the string is already long enough
	int len = NumBytes();
	if(len >= minLength)
		return *this;

	// reserve enough memory
	Reserve(minLength + 1);
	pData->bytes = minLength;
	pData->pMemory[minLength] = 0;

	// pad the string
	int padLen = MFString_Length(pPadding);
	int b = bAlignPadding ? len%padLen : 0;
	for(int a=len; a<minLength; ++a, ++b)
	{
		if(b >= padLen)
			b = 0;

		pData->pMemory[a] = pPadding[b];
	}

	return *this;
}

MFString MFString::SubStr(int offset, int count) const
{
	if(!pData)
		return *this;

	// limit within the strings range
	int maxChars = (int)pData->bytes - offset;
	if(count < 0 || count > maxChars)
		count = maxChars;

	// bail if we don't need to do anything
	if(count == (int)pData->bytes)
		return *this;

	// allocate a new string
	MFString t;
    t.Reserve(count + 1);
	t.pData->bytes = count;

	// copy sub string
	MFString_CopyN(t.pData->pMemory, pData->pMemory + offset, count);
	t.pData->pMemory[count] = 0;

	return t;
}

MFString& MFString::Truncate(int length)
{
	if(pData && (size_t)length < pData->bytes)
	{
		Detach();
		pData->bytes = length;
		pData->pMemory[length] = 0;
	}

	return *this;
}

MFString MFString::GetExtension() const
{
	int dot = FindCharReverse('.');
	if(dot > FindCharReverse('/') && dot > FindCharReverse('\\'))
		return SubStr(dot);
	return MFString();
}

MFString& MFString::TruncateExtension()
{
	int dot = FindCharReverse('.');
	if(dot >= 0)
	{
		pData->pMemory[dot] = 0;
		pData->bytes = dot;
	}

	return *this;
}

MFString& MFString::ClearRange(int offset, int length)
{
	if(!pData)
		return *this;

	// limit within the strings range
	int maxChars = (int)pData->bytes - offset;
	if(length > maxChars)
		length = maxChars;

	// bail if we don't need to do anything
	if(length <= 0)
		return *this;

	// clear the range
	Detach();

	int postBytes = (int)pData->bytes - (offset + length);
	pData->bytes -= length;

	char *pReplace = pData->pMemory + offset;
	const char *pTail = pReplace + length;

	for(int a=0; a <= postBytes; ++a)
		pReplace[a] = pTail[a];

	return *this;
}

MFString& MFString::Replace(int offset, int range, MFString string)
{
	if(!pData)
	{
		pData = string.pData;
		if(pData)
			pData->AddRef();
		return *this;
	}

	// limit within the strings range
	offset = MFMin(offset, (int)pData->bytes);
	int maxChars = (int)pData->bytes - offset;
	if(range > maxChars)
		range = maxChars;

	// bail if we don't need to do anything
	int strLen = string.NumBytes();
	if(range == 0 && strLen == 0)
		return *this;

	int reposition = strLen - range;
	int newSize = (int)pData->bytes + reposition;

	// reserve memory for the new string
	Reserve(newSize);

	// move tail into place
	if(reposition)
	{
		int tailOffset = offset + range;
		char *pSrc = pData->pMemory + tailOffset;
		char *pDest = pSrc + reposition;

		if(pDest < pSrc)
		{
			while(*pSrc)
				*pDest++ = *pSrc++;
			*pDest = 0;
		}
		else
		{
			int len = (int)pData->bytes - tailOffset;
			while(len >= 0)
			{
				pDest[len] = pSrc[len];
				--len;
			}
		}
	}

	// insert string
	if(strLen)
		MFString_CopyN(pData->pMemory + offset, string.pData->pMemory, strLen);

	pData->bytes = newSize;

	return *this;
}

int MFString::FindChar(int c, int startOffset) const
{
	if(pData)
	{
		const char *pStart = pData->pMemory + startOffset;
		const char *pT = pStart;
		while(*pT)
		{
			// decode utf8
			int t;
			int bytes = MFString_DecodeUTF8(pT, &t);

			// check if the characters match
			if(t == c)
				return (int)(pT - pStart);

			// progress to next char
			pT += bytes;
		}
	}

	return -1;
}

int MFString::FindCharReverse(int c) const
{
	if(pData)
	{
		const char *pT = pData->pMemory + pData->bytes;
		while(pT >= pData->pMemory)
		{
			// decode utf8
			int t;
			MFString_DecodeUTF8(pT, &t);

			// check if the characters match
			if(t == c)
				return (int)(pT - pData->pMemory);

			// progress to prev char
			pT = MFString_PrevChar(pT);
		}
	}

	return -1;
}

MFString& MFString::Join(const MFArray<MFString> &strings, const char *pSeparator, const char *pTokenPrefix, const char *pTokenSuffix, const char *pBefore, const char *pAfter)
{
	MFString result;
	size_t numTokens = strings.size();

	// TODO: we should resize the string in advance...

	if(pBefore)
		result += pBefore;

	// TODO: alternate loops with different availability of parameters?
	for(size_t i=0; i<numTokens; ++i)
	{
		if(pSeparator && i>0)
			result += pSeparator;

		if(pTokenPrefix)
			result += pTokenPrefix;

		result += strings[i];

		if(pTokenSuffix)
			result += pTokenSuffix;
	}

	if(pAfter)
		result += pAfter;

	*this = result;
	return *this;
}

MFArray<MFString>& MFString::Split(MFArray<MFString> &output, const char *pDelimiters)
{
	output.clear();

	if(!pData)
		return output;

	const char *pText = pData->pMemory;
	while(*pText)
	{
		const char *pEnd = MFSeekDelimiter(pText, pDelimiters);
		output.push(MFString(pText, (size_t)(pEnd - pText)));
		pText = MFSkipDelimiters(pEnd, pDelimiters);
	}

	return output;
}

int MFString::Enumerate(const MFArray<MFString> keys, bool bCaseSensitive)
{
	if(IsEmpty())
		return -1;
	for(size_t i=0; i<keys.size(); ++i)
	{
		if(bCaseSensitive ? Equals(keys[i]) : EqualsInsensitive(keys[i]))
			return (int)i;
	}
	return -1;
}

MFString MFString::StripToken(const char *pDelimiters)
{
	if(!pData)
		return NULL;

	// find token
	const char *pText = CStr();
	const char *pToken = MFSkipDelimiters(pText, pDelimiters);
	const char *pEnd = MFSeekDelimiter(pToken, pDelimiters);
	const char *pTrim = MFSkipDelimiters(pEnd, pDelimiters);

	// capture the token
	MFString token(pToken, (size_t)(pEnd - pToken));

	// strip from source string
	size_t offset = pTrim - pText;
	if(offset > 0)
	{
		Detach();

		for(size_t i = offset; i <= pData->bytes; ++i)
			pData->pMemory[i - offset] = pData->pMemory[i];
		pData->bytes -= offset;
	}

	return token;
}

static MFString GetNextBit(const char *&pFormat)
{
	MFString format;

	while(*pFormat)
	{
		if(*pFormat == '%')
		{
			if(pFormat[1] == '%')
				++pFormat;
			else
				break;
		}
		else if(*pFormat == '\\')
		{
			if(pFormat[1] == 't')
			{
				format += '\t';
				pFormat += 2;
				continue;
			}
			else if(pFormat[1] == 'n')
			{
				format += '\n';
				pFormat += 2;
				continue;
			}
			else if(pFormat[1] == 'r')
			{
				format += '\r';
				pFormat += 2;
				continue;
			}
			else
				++pFormat;
		}

		format += *pFormat++;
	}

	return format;
}

static int Match(const char *pString, const char *pFormat)
{
	if(!pFormat)
		return 0;

	const char *pStart = pString;
	while(*pFormat)
	{
		if(*pFormat == '?')
		{
			if(*pString == 0)
				return -1;
		}
/*
		elseif(*pFormat == '*')
		{
			pString = MFString_Chr(pString, pFormat[1]);
			++pFormat;
			continue;
		}
*/
		else if(*pString != *pFormat)
			return -1;

		++pString;
		++pFormat;
	}

	return (int)(pString - pStart);
}

int MFString::Parse(const char *pFormat, ...)
{
	if(!pData || !pFormat)
		return 0;

	va_list arglist;
	va_start(arglist, pFormat);

	const char *pS = pData->pMemory;
	int numArgs = 0;

	MFString format = GetNextBit(pFormat);
	int numChars = Match(pS, format.CStr());

	while(*pFormat && numChars >= 0)
	{
		pS += numChars;
		++pFormat;

		int length = -1;

		// gather format options...
		while(*pFormat)
		{
			int c = MFToLower(*pFormat++);
			if(c == 's')
			{
				MFString *pStr = va_arg(arglist, MFString*);
				++numArgs;

				format = GetNextBit(pFormat);

				if(length >= 0)
				{
					MFString s(pS, (size_t)length);
					*pStr = s;

					numChars = Match(pS, format.CStr());
				}
				else if(format.NumBytes() == 0)
				{
					*pStr = pS;
				}
				else
				{
					const char *pEnd = pS;
					while(*pEnd && (numChars = Match(pEnd, format.CStr())) < 0)
						++pEnd;

					MFString s(pS, (size_t)(pEnd - pS));
					*pStr = s;
				}

				pS += pStr->NumBytes();
				break;
			}
			else if(c == 'd' || c == 'i')
			{
				int *pInt = va_arg(arglist, int*);
				++numArgs;

				bool bNeg = *pS == '-';
				if(*pS == '-' || *pS == '+')
					++pS;

				*pInt = 0;
				while(MFIsNumeric(*pS) && ((uint32&)length)-- > 0)
					*pInt = *pInt*10 + *pS++ - '0';

				if(bNeg)
					*pInt = -*pInt;

				format = GetNextBit(pFormat);
				numChars = Match(pS, format.CStr());
				break;
			}
			else if(c == 'x')
			{
				int *pInt = va_arg(arglist, int*);
				++numArgs;

				*pInt = 0;
				while(MFIsHex(*pS) && ((uint32&)length)-- > 0)
				{
					int digit = *pS++;
					*pInt = (*pInt << 4) + (MFIsNumeric(digit) ? digit - '0' : MFToLower(digit) - 'a' + 10);
				}

				format = GetNextBit(pFormat);
				numChars = Match(pS, format.CStr());
				break;
			}
			else if(c == 'f')
			{
				float *pFloat = va_arg(arglist, float*);
				++numArgs;

				*pFloat = MFString_AsciiToFloat(pS, &pS);

				format = GetNextBit(pFormat);
				numChars = Match(pS, format.CStr());
				break;
			}
			else if(MFIsNumeric(c))
			{
				// read numeric length
				length = 0;
				while(true)
				{
					length = length*10 + c - '0';
					c = *pFormat;
					if(!MFIsNumeric(c))
						break;
					++pFormat;
				}
			}
			else if(c == '*')
			{
				// read length from varargs
				length = va_arg(arglist, int);
				++numArgs;
			}
		}
	}

	va_end(arglist);

	return numArgs;
}
