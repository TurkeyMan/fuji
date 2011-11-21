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

void MFString_InitModule()
{
	stringHeap.Init(gStringGroups, sizeof(gStringGroups) / sizeof(gStringGroups[0]));
	stringPool.Init(sizeof(MFStringData), 128, 128);
}

void MFString_DeinitModule()
{
	stringPool.Deinit();
	stringHeap.Deinit();
}

MFString MFString_GetStats()
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

	MFString desc(1024);
	desc = MFStr("String heap memory: %d allocated/%d reserved + %d overhead  Waste: %d  Average length: %d\n\tPool: %d/%d", stringHeap.GetAllocatedMemory(), stringHeap.GetTotalMemory(), overhead, waste, averageSize, stringPool.GetNumAllocated(), stringPool.GetNumReserved());

	int numGroups = stringHeap.GetNumPools();
	for(int a=0; a<numGroups; ++a)
	{
		MFObjectPool *pPool = stringHeap.GetPool(a);
		desc += MFStr("\n\t\t%d byte: %d/%d", pPool->GetObjectSize(), pPool->GetNumAllocated(), pPool->GetNumReserved());
	}

	return desc;
}

void MFString_Dump()
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
		MFDebug_Log(1, MFStr("%d refs, %db: \"%s\"", pString->GetRefCount(), pString->GetAllocated(), pString->GetMemory()));
	}
}

void MFCopyMemory(void *pDest, const void *pSrc, size_t size)
{
	memcpy(pDest, pSrc, size);
}

void MFMemSet(void *pDest, int value, size_t size)
{
	memset(pDest, value, size);
}

void MFZeroMemory(void *pDest, size_t size)
{
	memset(pDest, 0, size);
}

int MFMemCompare(const void *pBuf1, const void *pBuf2, size_t size)
{
	return memcmp(pBuf1, pBuf2, size);
}

const char * MFString_ToLower(const char *pString)
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

const char * MFString_ToUpper(const char *pString)
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

const char * MFStr(const char *format, ...)
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

const char * MFStrN(const char *pSource, size_t n)
{
	char *pBuffer = &gStringBuffer[gStringOffset];

	MFString_CopyN(pBuffer, pSource, (int)n);
	pBuffer[n] = 0;

	gStringOffset += (uint32)n+1;

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	return pBuffer;
}

int MFString_Compare(const char *pString1, const char *pString2)
{
	while(*pString1 == *pString2++)
	{
		if(*pString1++ == 0)
			return 0;
	}

	return (*(const unsigned char *)pString1 - *(const unsigned char *)(pString2 - 1));
}

int MFString_CompareN(const char *pString1, const char *pString2, int n)
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

int MFString_CaseCmp(const char *pSource1, const char *pSource2)
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

int MFString_CaseCmpN(const char *pSource1, const char *pSource2, uint32 n)
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

char* MFString_Chr(const char *pString, int c)
{
	do
	{
		if(*pString == (char)c)
			return (char*)pString;
	}
	while(*pString++);

	return (NULL);
}

char* MFString_RChr(const char *pSource, int c)
{
	char *pLast;

	for(pLast = NULL; *pSource; pSource++)
	{
		if(c == *pSource)
			pLast = (char*)pSource;
	}

	return pLast;
}

bool MFString_PatternMatch(const char *pPattern, const char *pFilename, const char **ppMatchDirectory, bool bCaseSensitive)
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
			while(!(match = MFString_PatternMatch(pPattern, pFilename, ppMatchDirectory)) && *pFilename)
			{
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

const char* MFStr_URLEncodeString(const char *pString, const char *pExcludeChars)
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

int MFString_URLEncode(char *pDest, const char *pString, const char *pExcludeChars)
{
	int sourceLen = MFString_Length(pString);
	int destLen = 0;

	for(int a=0; a<sourceLen; ++a)
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

bool MFString_IsNumber(const char *pString, bool bAllowHex)
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

int MFString_AsciiToInteger(const char *pString, bool bDetectBase, int base)
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
			number *= 10;
			number += (*pString++) - '0';
		}

		if(neg)
			number = -number;
	}

	return number;
}

float MFString_AsciiToFloat(const char *pString)
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
			number *= 10;
			number += digit - '0';
			if(bHasDot)
				frac *= 0.1f;
		}
	}

	if(neg)
		number = -number;

	return (float)number * frac;
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

int MFString_CopyUTF8ToUTF16(uint16 *pBuffer, const char *pString)
{
	const char *pStart = pString;
	int bytes;

	while(*pString)
	{
		*pBuffer++ = (uint16)MFString_DecodeUTF8(pString, &bytes);
		pString += bytes;
	}
	*pBuffer = 0;

	return (int)(pString - pStart);
}

int MFString_CopyUTF16ToUTF8(char *pBuffer, const uint16 *pString)
{
	const uint16 *pStart = pString;

	while(*pString)
		pBuffer += MFString_EncodeUTF8(*pString++, pBuffer);
	*pBuffer = 0;

	return (int)(pString - pStart);
}

uint16* MFString_UFT8AsWChar(const char *pUTF8String, int *pNumChars)
{
	// count number of actual characters in the string
	int numChars = MFString_GetNumChars(pUTF8String);

	// get some space in the MFStr buffer
	if(gStringOffset & 1)
		++gStringOffset;

	uint16 *pBuffer = (uint16*)&gStringBuffer[gStringOffset];
	gStringOffset += numChars*2 + 2;

	// if we wrapped the string buffer
	if(gStringOffset >= sizeof(gStringBuffer) - 1024)
	{
		gStringOffset = 0;
		pBuffer = (uint16*)gStringBuffer;
	}

	// copy the string
	MFString_CopyUTF8ToUTF16(pBuffer, pUTF8String);

	if(pNumChars)
		*pNumChars = numChars;

	return pBuffer;
}


//
// unicode support
//

int MFWString_Compare(const uint16 *pString1, const uint16 *pString2)
{
	while(*pString1 == *pString2++)
	{
		if(*pString1++ == 0)
			return 0;
	}

	return (*(const uint16 *)pString1 - *(const uint16 *)(pString2 - 1));
}

int MFWString_CaseCmp(const uint16 *pSource1, const uint16 *pSource2)
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

MFString::MFString(const char *pString, int maxChars)
{
	if(!pString)
	{
		pData = NULL;
	}
	else
	{
		pData = MFStringData::Alloc();
		pData->bytes = MFString_LengthN(pString, maxChars);

		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);

		MFString_CopyN(pData->pMemory, pString, (int)pData->bytes);
		pData->pMemory[pData->bytes] = 0;
	}
}

MFString::MFString(int preallocatedBytes)
{
	pData = MFStringData::Alloc();
	pData->bytes = 0;
	pData->pMemory = (char*)stringHeap.Alloc(preallocatedBytes, &pData->allocated);
	pData->pMemory[0] = 0;
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

MFString& MFString::operator+=(const char *pString)
{
	return *this += MFString::Static(pString);
}

MFString& MFString::operator+=(const MFString &string)
{
	if(!string)
		return *this;

	if(!pData)
	{
		string.pData->AddRef();
		pData = string.pData;
	}
	else
	{
		size_t sumBytes = pData->bytes + string.pData->bytes;
		size_t bytesNeeded = sumBytes + 1;

		Reserve(bytesNeeded);

		MFString_Copy(pData->pMemory + pData->bytes, string.pData->pMemory);
		pData->bytes = sumBytes;
	}

	return *this;
}

MFString MFString::operator+(const MFString &string) const
{
	if(!string)
		return *this;

	if(!pData)
		return string;

	size_t bytes = pData->bytes + string.pData->bytes;

	MFString t((int)bytes + 1);
	t.pData->bytes = bytes;

	MFString_CopyCat(t.pData->pMemory, pData->pMemory, string.pData->pMemory);

	return t;
}

MFString MFString::operator+(const char *pString) const
{
	return this->operator+(MFString::Static(pString));
}

MFString operator+(const char *pString, const MFString &string)
{
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

		int len = 0;
		const wchar_t *pS = pString;
		while(*pS)
			len += MFString_EncodeUTF8(*pS++, NULL);
		pData->bytes = len;

		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
		MFString_CopyUTF16ToUTF8(pData->pMemory, (const uint16*)pString);
	}

	return *this;
}

MFString& MFString::Detach()
{
	if(pData && (pData->refCount > 1 || pData->allocated == 0))
	{
		MFStringData *pNew = MFStringData::Alloc();
		pNew->bytes = pData->bytes;
		pNew->pMemory = (char*)stringHeap.Alloc(pNew->bytes + 1, &pNew->allocated);
		MFString_Copy(pNew->pMemory, pData->pMemory);

		pData->Release();
		pData = pNew;
	}

	return *this;
}

MFString& MFString::Reserve(size_t bytes, bool bClearString)
{
	// detach instance
	if(pData && pData->refCount > 1)
	{
		pData->Release();
		pData = NULL;
	}

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
	MFString t((int)pData->bytes + 1);
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
	MFString t((int)pData->bytes + 1);
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
	MFString t(count+1);
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
