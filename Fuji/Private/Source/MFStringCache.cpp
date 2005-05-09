//
// Mount Fuji Simple string cache.
//
//=============================================================================
#include "common.h"
#include "MFStringCache.h"

MFStringCache *MFStringCache::Create(int maxSize)
{
	MFStringCache *pCache = (MFStringCache *)Heap_Alloc(sizeof(MFStringCache));
	pCache->size = maxSize;
	pCache->pMem = (char *)Heap_Alloc(maxSize);
	pCache->pMem[0] = 0;
	return pCache;
}

void MFStringCache::Destroy(MFStringCache *pCache)
{
	Heap_Free(pCache->pMem);
	Heap_Free(pCache);
}

const char *MFStringCache::Add(const char *pNewString)
{
	DBGASSERT(pNewString, "Cannot add NULL string");

	// find the string
	char *pCurr = pMem;
	int newLength = strlen(pNewString)+1;
	while (pCurr[0] && pCurr < &pMem[size])
	{
		int length = strlen(pCurr);
		if (!strcmpi(pCurr, pNewString))
		{
			// found string
			return pCurr;
		}
		pCurr += length+1;
	}

	DBGASSERT(&pCurr[newLength+1] > &pMem[size], "No memory for string!");

	strcpy(pCurr, pNewString);
	pCurr[newLength]=0;
	return pCurr;
}
