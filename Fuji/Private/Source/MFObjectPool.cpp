#include "Fuji.h"
#include "MFHeap.h"
#include "MFObjectPool.h"

void MFObjectPool::Init(int _objectSize, int numObjects, int growObjects)
{
	objectSize = _objectSize;
	maxItems = numObjects;
	grow = growObjects;

	bytes = _objectSize * numObjects;
	pMemory = (char*)MFHeap_Alloc(bytes + sizeof(void**)*numObjects);

	ppItems = (void**)(pMemory + bytes);
	for(int a=0; a<numObjects; ++a)
		ppItems[a] = pMemory + _objectSize*a;

	pNext = NULL;
}

void MFObjectPool::Deinit()
{
	if(pNext)
	{
		pNext->Deinit();
		MFHeap_Free(pNext);
	}

	if(pMemory)
		MFHeap_Free(pMemory);
}

void *MFObjectPool::Alloc()
{
	if(allocated < maxItems)
	{
		return ppItems[allocated++];
	}
	else
	{
		if(pNext)
		{
			return pNext->Alloc();
		}
		else if(grow)
		{
			pNext = (MFObjectPool*)MFHeap_Alloc(sizeof(MFObjectPool));
			pNext->Init(objectSize, grow, grow);
			return pNext->Alloc();
		}
	}

	return NULL;
}

void *MFObjectPool::AllocAndZero()
{
	void *pNew = Alloc();
	if(pNew)
		MFZeroMemory(pNew, objectSize);
	return pNew;
}

int MFObjectPool::Free(void *pItem)
{
	if(pItem >= pMemory && pItem < pMemory + bytes)
	{
		for(int a=0; a<allocated; ++a)
		{
			if(ppItems[a] == pItem)
			{
				void *pItem = ppItems[a];
				ppItems[a] = ppItems[--allocated];
				ppItems[allocated] = pItem;
				return 1;
			}
		}
	}

	if(pNext)
		return pNext->Free(pItem);
	return 0;
}

void MFObjectPoolGroup::Init(MFObjectPoolGroupConfig *_pPools, int numPools)
{
	pConfig = (MFObjectPoolGroupConfig*)MFHeap_Alloc(sizeof(MFObjectPoolGroupConfig)*numPools + sizeof(MFObjectPool)*numPools);
	pPools = (MFObjectPool*)&pConfig[numPools];

	MFCopyMemory(pConfig, _pPools, sizeof(MFObjectPoolGroupConfig)*numPools);

	for(int a=0; a<numPools; ++a)
	{
		pPools[a].Init(pConfig[a].objectSize, pConfig[a].numObjects, pConfig[a].growObjects);
	}
}

void MFObjectPoolGroup::Deinit()
{
	for(int a=0; a<numPools; ++a)
		pPools[a].Deinit();
	MFHeap_Free(pConfig);
}

void *MFObjectPoolGroup::Alloc(int bytes, int *pAllocated)
{
	for(int a=0; a<numPools; ++a)
	{
		if(bytes <= pConfig[a].objectSize)
		{
			if(pAllocated)
				*pAllocated = pConfig[a].objectSize;
			return pPools[a].Alloc();
		}
	}

	if(pAllocated)
		*pAllocated = bytes;
	return MFHeap_Alloc(bytes);
}

void *MFObjectPoolGroup::AllocAndZero(int bytes, int *pAllocated)
{
	int size = 0;
	void *pNew = Alloc(bytes, &size);
	if(pNew)
		MFZeroMemory(pNew, size);
	if(pAllocated)
		*pAllocated = size;
	return pNew;
}

void MFObjectPoolGroup::Free(void *pItem)
{
	for(int a=0; a<numPools; ++a)
	{
		if(pPools[a].Free(pItem))
			return;
	}

	MFHeap_Free(pItem);
}
