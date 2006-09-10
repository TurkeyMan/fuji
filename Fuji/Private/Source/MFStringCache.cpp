//
// Mount Fuji Simple string cache.
//
//=============================================================================
#include "Fuji.h"
#include "MFHeap.h"
#include "MFStringCache.h"

struct MFStringCache
{
	uint32 size;
	uint32 used;
	char *pMem;
};


MFStringCache* MFStringCache_Create(uint32 maxSize)
{
	MFCALLSTACK;

	MFStringCache *pCache;
	pCache = (MFStringCache*)MFHeap_Alloc(sizeof(MFStringCache) + maxSize);

	pCache->size = maxSize;
	pCache->pMem = (char*)&pCache[1];
	pCache->pMem[0] = 0;
	pCache->used = 1;

	return pCache;
}

void MFStringCache_Destroy(MFStringCache *pCache)
{
	MFCALLSTACK;

	MFHeap_Free(pCache);
}

const char *MFStringCache_Add(MFStringCache *pCache, const char *pNewString)
{
	MFCALLSTACK;

	MFDebug_Assert(pCache, "NULL String cache!");
	MFDebug_Assert(pNewString, "Cannot add NULL string");

	// find the string
	char *pCurr = pCache->pMem;
	int newLength = MFString_Length(pNewString)+1;

	while (pCurr[0] && pCurr < &pCache->pMem[pCache->size])
	{
		int length = MFString_Length(pCurr);
		if (!MFString_Compare(pCurr, pNewString))
		{
			// found string
			return pCurr;
		}
		pCurr += length+1;
	}

	MFDebug_Assert(&pCurr[newLength+1] < &pCache->pMem[pCache->size], "No memory for string!");

	MFString_Copy(pCurr, pNewString);
	pCurr[newLength]=0;
	pCache->used = ((uint32&)pCurr + newLength) - (uint32&)pCache->pMem + 1;
	return pCurr;
}

// get cache for direct read/write
char* MFStringCache_GetCache(MFStringCache *pCache)
{
	return pCache->pMem;
}

// get the actual used size of the cache
uint32 MFStringCache_GetSize(MFStringCache *pCache)
{
	return pCache->used;
}

// use this if you have filled the cache yourself
void MFStringCache_SetSize(MFStringCache *pCache, uint32 used)
{
	pCache->used = used;
}
