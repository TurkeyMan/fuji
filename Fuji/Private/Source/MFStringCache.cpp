//
// Mount Fuji Simple string cache.
//
//=============================================================================
#include "Common.h"
#include "Heap.h"
#include "MFStringCache.h"

MFStringCache *MFStringCache::Create(uint32 maxSize)
{
	MFStringCache *pCache = (MFStringCache *)Heap_Alloc(sizeof(MFStringCache));
	pCache->size = maxSize;
	pCache->pMem = (char *)Heap_Alloc(maxSize);
	pCache->pMem[0] = 0;
	pCache->used = 1;
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
	int newLength = (int)strlen(pNewString)+1;
	while (pCurr[0] && pCurr < &pMem[size])
	{
		int length = (int)strlen(pCurr);
		if (!stricmp(pCurr, pNewString))
		{
			// found string
			return pCurr;
		}
		pCurr += length+1;
	}

	DBGASSERT(&pCurr[newLength+1] < &pMem[size], "No memory for string!");

	strcpy(pCurr, pNewString);
	pCurr[newLength]=0;
	used = ((uint32&)pCurr + newLength) - (uint32&)pMem + 1;
	return pCurr;
}
