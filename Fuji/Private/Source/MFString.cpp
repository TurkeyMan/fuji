#include "Fuji.h"
#include "MFString.h"

#include <stdio.h>
#include <stdarg.h>

static char gStringBuffer[1024*128];
static uint32 gStringOffset;

char* MFStr(const char *format, ...)
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

char* MFStrN(const char *pSource, int n)
{
	char *pBuffer = &gStringBuffer[gStringOffset];

	strncpy(pBuffer, pSource, n);
	pBuffer[n] = 0;

	gStringOffset += n+1;

	if(gStringOffset >= sizeof(gStringBuffer) - 1024) gStringOffset = 0;

	return pBuffer;
}

int MFString_CaseCmp(const char *pSource1, const char *pSource2)
{
	while(*pSource1 != '\0' && tolower(*pSource1) == tolower(*pSource2))
	{
		pSource1++;
		pSource2++;
	}

	return tolower(*(unsigned char *)pSource1) - tolower(*(unsigned char *)pSource2);
}

int MFString_CaseCmpN(const char *pSource1, const char *pSource2, uint32 n)
{
	if(n == 0)
		return 0;

	while(n-- != 0 && tolower(*pSource1) == tolower(*pSource2))
	{
		if(n == 0 || *pSource1 == '\0' || *pSource2 == '\0')
			break;
		pSource1++;
		pSource2++;
	}

	return tolower(*(unsigned char *)pSource1) - tolower(*(unsigned char *)pSource2);
}

char* MFString_RChr(const char *pSource, int c)
{
	const char *pLast = NULL;

	if(c)
	{
		while((pSource=strchr(pSource, c)) != NULL)
		{
			pLast = pSource;
			pSource++;
		}
	}
	else
	{
		pLast = strchr(pSource, c);
	}

	return (char *)pLast;
}
