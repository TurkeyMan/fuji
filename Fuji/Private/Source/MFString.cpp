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

void MFString_InitModule()
{
	MFObjectPoolGroupConfig groups[] =
	{
		{ 5, 256, 128 },
		{ 16, 128, 64 },
		{ 128, 32, 16 },
		{ 1024, 4, 2}
	};

	stringHeap.Init(groups, sizeof(groups) / sizeof(groups[0]));
	stringPool.Init(sizeof(MFStringData), 512, 128);
}

void MFString_DeinitModule()
{
	stringPool.Deinit();
	stringHeap.Deinit();
}

void MFCopyMemory(void *pDest, const void *pSrc, uint32 size)
{
	memcpy(pDest, pSrc, size);
}

void MFMemSet(void *pDest, int value, uint32 size)
{
	memset(pDest, value, size);
}

void MFZeroMemory(void *pDest, uint32 size)
{
	memset(pDest, 0, size);
}

int MFMemCompare(const void *pBuf1, const void *pBuf2, uint32 size)
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

	MFString_CopyN(pBuffer, pSource, n);
	pBuffer[n] = 0;

	gStringOffset += n+1;

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

bool MFString_IsNumber(const char *pString, bool bAllowHex)
{
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

int MFString_AsciiToInteger(const char *pString, bool bAllowHex)
{
	int number = 0;

	if(bAllowHex && pString[0] == '0' && pString[1] == 'x')
	{
		// hex number
		pString += 2;
		while(*pString)
		{
			int digit = *pString;
			if(!MFIsHex(digit))
				return number;
			number <<= 4;
			number += MFIsNumeric(digit) ? digit - '0' : MFToLower(digit) - 'a';
		}
	}
	else
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

float MFString_AsciiToFloat(const char *pString, bool bAllowHex)
{
	int64 number = 0;
	float frac = 1;

	if(bAllowHex && pString[0] == '0' && pString[1] == 'x')
	{
		// hex number
		pString += 2;
		while(*pString)
		{
			int digit = *pString++;
			if(!MFIsHex(digit))
				return (float)number;
			number <<= 4;
			number += MFIsNumeric(digit) ? digit - '0' : MFToLower(digit) - 'a';
		}
	}
	else
	{
		// decimal number
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
	}

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
		*pBuffer++ = MFString_DecodeUTF8(pString, &bytes);
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

MFString::MFString(int preallocatedBytes)
{
	pData = MFStringData::Alloc();
	pData->bytes = 0;
	pData->pMemory = (char*)stringHeap.Alloc(preallocatedBytes, &pData->allocated);
	pData->pMemory[0] = 0;
}

MFString& MFString::operator=(const char *pString)
{
	if(pData)
	{
		pData->Release();
		pData = NULL;
	}

	if(pString)
	{
		pData = MFStringData::Alloc();
		pData->bytes = MFString_Length(pString);
		pData->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
		MFString_Copy(pData->pMemory, pString);
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
	MFString t(pString);
	return *this += t;
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
		// make sure nothing else references this string since we are going to modify it
		Detach();

		int sumBytes = pData->bytes + string.pData->bytes;
		int bytesNeeded = sumBytes + 1;
		if(pData->allocated >= bytesNeeded)
		{
			MFString_Copy(pData->pMemory + pData->bytes, string.pData->pMemory);
		}
		else
		{
			// overflowed the string buffer, need to realloc...
			char *pNew = (char*)stringHeap.Alloc(bytesNeeded, &pData->allocated);
			MFString_CopyCat(pNew, pData->pMemory, string.pData->pMemory);

			if(pData->allocated)
				stringHeap.Free(pData->pMemory);

			pData->pMemory = pNew;
		}
		pData->bytes = sumBytes;
	}

	return *this;
}

MFString MFString::operator+(const char *pString) const
{
	MFString t(*this);
	return t += pString;
}

MFString MFString::operator+(const MFString &string) const
{
	MFString t(*this);
	return t += string;
}

MFString operator+(const char *pString, const MFString &string)
{
	MFString t(pString, true);
	return t += string;
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
	if(pData && pData->refCount > 1)
	{
		MFStringData *pNew = MFStringData::Alloc();
		pNew->bytes = pData->bytes;
		pNew->pMemory = (char*)stringHeap.Alloc(pData->bytes + 1, &pData->allocated);
		MFString_Copy(pNew->pMemory, pData->pMemory);

		--pData->refCount;
		pData = pNew;
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

	int nRes = vsprintf(NULL, pFormat, arglist);
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
	MFString t(pData->bytes + 1);
	t.pData->bytes = pData->bytes;

	// copy upper case
	for(int a=0; a<pData->bytes + 1; ++a)
		t.pData->pMemory[a] = MFToUpper(pData->pMemory[a]);

	return t;
}

MFString MFString::Lower() const
{
	if(!pData)
		return *this;

	// allocate a new string
	MFString t(pData->bytes + 1);
	t.pData->bytes = pData->bytes;

	// copy lower case string
	for(int a=0; a<pData->bytes + 1; ++a)
		t.pData->pMemory[a] = MFToLower(pData->pMemory[a]);

	return t;
}

MFString& MFString::Trim(bool bFront, bool bEnd, const char *pCharacters)
{
	if(pData)
	{
		const char *pSrc = pData->pMemory;
		int offset = 0;

		// trim start
		if(bFront)
		{
			while(pSrc[offset] && MFString_Chr(pCharacters, pSrc[offset]))
				++offset;
		}

		int count = pData->bytes - offset;

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

		*this = SubStr(offset, count);
	}

	return *this;
}

MFString MFString::SubStr(int offset, int count) const
{
	if(!pData)
		return *this;

	// allocate a new string
	MFString t(count+1);
	t.pData->bytes = count;

	// copy sub string
	for(int a=0; a<count; ++a)
		t.pData->pMemory[a] = pData->pMemory[offset + a];
	t.pData->pMemory[count] = 0;

	return t;
}
