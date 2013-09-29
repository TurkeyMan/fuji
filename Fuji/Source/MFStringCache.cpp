//
// Mount Fuji Simple string cache.
//
//=============================================================================
#include "Fuji.h"
#include "MFHeap.h"
#include "MFStringCache.h"

struct MFStringCache
{
	size_t size;
	size_t used;
	char *pMem;
};


MF_API MFStringCache* MFStringCache_Create(size_t maxSize)
{
	MFCALLSTACK;

	MFStringCache *pCache;
	pCache = (MFStringCache*)MFHeap_Alloc(sizeof(MFStringCache) + maxSize);

	pCache->size = maxSize;
	pCache->pMem = (char*)&pCache[1];
	pCache->pMem[0] = pCache->pMem[1] = 0;
	pCache->used = 2;

	return pCache;
}

MF_API void MFStringCache_Destroy(MFStringCache *pCache)
{
	MFCALLSTACK;

	MFHeap_Free(pCache);
}

MF_API void MFStringCache_Clear(MFStringCache *pCache)
{
	MFCALLSTACK;

	pCache->pMem[0] = pCache->pMem[1] = 0;
	pCache->used = 2;
}

MF_API const char *MFStringCache_Add(MFStringCache *pCache, const char *pNewString)
{
	MFCALLSTACK;

	MFDebug_Assert(pCache, "NULL String cache!");
	MFDebug_Assert(pNewString, "Cannot add NULL string");

	if(!*pNewString)
		return pCache->pMem;

	// find the string
	char *pCurr = pCache->pMem + 1;
	size_t newLength = MFString_Length(pNewString)+1;

	while (pCurr[0] && pCurr < pCache->pMem + pCache->size)
	{
		if (!MFString_Compare(pCurr, pNewString))
		{
			// found string
			return pCurr;
		}
		pCurr += MFString_Length(pCurr)+1;
	}

	if(pCurr+newLength+1 > pCache->pMem+pCache->size)
		return NULL;

	MFString_Copy(pCurr, pNewString);
	pCurr[newLength]=0;
	pCache->used = ((uintp)pCurr + newLength) - (uintp)pCache->pMem + 1;
	return pCurr;
}

MF_API const char *MFStringCache_AddN(MFStringCache *pCache, const char *pNewString, size_t length)
{
	MFCALLSTACK;

	MFDebug_Assert(pCache, "NULL String cache!");
	MFDebug_Assert(pNewString, "Cannot add NULL string");

	if(!*pNewString)
		return pCache->pMem;

	for(size_t i=0; i<length; ++i)
	{
		if(pNewString[i] == 0)
		{
			length = i;
			break;
		}
	}

	// find the string
	char *pCurr = pCache->pMem + 1;
	size_t newLength = length+1;

	while (pCurr[0] && pCurr < pCache->pMem + pCache->size)
	{
		if (!MFString_CompareN(pCurr, pNewString, length) && pCurr[length] == 0)
		{
			// found string
			return pCurr;
		}
		pCurr += MFString_Length(pCurr)+1;
	}

	if(pCurr+newLength+1 > pCache->pMem+pCache->size)
		return NULL;

	MFString_CopyN(pCurr, pNewString, length);
	pCurr[length]=0;
	pCurr[newLength]=0;
	pCache->used = ((uintp)pCurr + newLength) - (uintp)pCache->pMem + 1;
	return pCurr;
}

// get cache for direct read/write
MF_API char* MFStringCache_GetCache(MFStringCache *pCache)
{
	return pCache->pMem;
}

// get the actual used size of the cache
MF_API size_t MFStringCache_GetSize(MFStringCache *pCache)
{
	return pCache->used;
}

// use this if you have filled the cache yourself
MF_API void MFStringCache_SetSize(MFStringCache *pCache, size_t used)
{
	pCache->used = used;
}


//
// unicode support
//

struct MFWStringCache
{
	size_t size;
	size_t used;
	wchar_t *pMem;
};


MF_API MFWStringCache* MFWStringCache_Create(size_t maxSize)
{
	MFCALLSTACK;

	MFWStringCache *pCache;
	pCache = (MFWStringCache*)MFHeap_Alloc(sizeof(MFWStringCache) + maxSize);

	pCache->size = maxSize;
	pCache->pMem = (wchar_t*)&pCache[1];
	pCache->pMem[0] = 0;
	pCache->used = 2;

	return pCache;
}

MF_API void MFWStringCache_Destroy(MFWStringCache *pCache)
{
	MFCALLSTACK;

	MFHeap_Free(pCache);
}

MF_API const wchar_t *MFWStringCache_Add(MFWStringCache *pCache, const wchar_t *pNewString)
{
	MFCALLSTACK;

	MFDebug_Assert(pCache, "NULL String cache!");
	MFDebug_Assert(pNewString, "Cannot add NULL string");

	// find the string
	wchar_t *pCurr = pCache->pMem;
	size_t newLength = MFWString_Length(pNewString)+1;

	while (pCurr[0] && pCurr < &pCache->pMem[pCache->size])
	{
		if (!MFWString_Compare(pCurr, pNewString))
		{
			// found string
			return pCurr;
		}
		pCurr += MFWString_Length(pCurr)+1;
	}

	MFDebug_Assert(&pCurr[newLength+1] < &pCache->pMem[pCache->size], "No memory for string!");

	MFWString_Copy(pCurr, pNewString);
	pCurr[newLength]=0;
	pCache->used = (((uintp)pCurr + newLength) - (uintp)pCache->pMem + 1) * 2;
	return pCurr;
}

// get cache for direct read/write
MF_API wchar_t* MFWStringCache_GetCache(MFWStringCache *pCache)
{
	return pCache->pMem;
}

// get the actual used size of the cache
MF_API size_t MFWStringCache_GetSize(MFWStringCache *pCache)
{
	return pCache->used;
}

// use this if you have filled the cache yourself
MF_API void MFWStringCache_SetSize(MFWStringCache *pCache, size_t used)
{
	pCache->used = used;
}
