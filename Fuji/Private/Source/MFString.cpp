#include "Fuji.h"
#include "MFString.h"

#include <stdio.h>
#include <stdarg.h>

#include <string.h>

static char gStringBuffer[1024*128];
static uint32 gStringOffset;


void MFCopyMemory(void *pDest, const void *pSrc, uint32 size)
{
	memcpy(pDest, pSrc, size);
}

void MFSetMemory(void *pDest, int value, uint32 size)
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

const char * MFStrN(const char *pSource, int n)
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
	while(*pDest++ = *pSrc++) { }
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
