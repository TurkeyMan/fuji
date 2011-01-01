#include "Fuji.h"
#include "MFHeap.h"
#include "MFObjectPool.h"

void MFObjectPool::Init(int _objectSize, int numObjects, int growObjects)
{
	objectSize = _objectSize;
	maxItems = numObjects;
	grow = growObjects;
	allocated = 0;

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
		pNext = NULL;
	}

	if(pMemory)
	{
		MFHeap_Free(pMemory);
		pMemory = NULL;
	}
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

uint32 MFObjectPool::GetTotalMemory()
{
	return objectSize * GetNumReserved();
}

uint32 MFObjectPool::GetAllocatedMemory()
{
	return objectSize * GetNumAllocated();
}

uint32 MFObjectPool::GetOverheadMemory()
{
	return sizeof(void**) * GetNumReserved() + sizeof(*this);
}

int MFObjectPool::GetNumReserved()
{
	return maxItems + (pNext ? pNext->GetNumReserved() : 0);
}

int MFObjectPool::GetNumAllocated()
{
	return allocated + (pNext ? pNext->GetNumAllocated() : 0);
}

void *MFObjectPool::GetItem(int index)
{
	if(index < maxItems)
		return ppItems[index];

	if(pNext)
		return pNext->GetItem(index - maxItems);

	return NULL;
}

void MFObjectPoolGroup::Init(const MFObjectPoolGroupConfig *_pPools, int _numPools)
{
	pConfig = (MFObjectPoolGroupConfig*)MFHeap_Alloc(sizeof(MFObjectPoolGroupConfig)*_numPools + sizeof(MFObjectPool)*_numPools);
	pPools = (MFObjectPool*)&pConfig[_numPools];
	numPools = _numPools;

	MFCopyMemory(pConfig, _pPools, sizeof(MFObjectPoolGroupConfig)*_numPools);

	for(int a=0; a<_numPools; ++a)
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

uint32 MFObjectPoolGroup::GetTotalMemory()
{
	int total = 0;
	for(int a=0; a<numPools; ++a)
		total += pPools[a].GetTotalMemory();
	return total;
}

uint32 MFObjectPoolGroup::GetAllocatedMemory()
{
	int allocated = 0;
	for(int a=0; a<numPools; ++a)
		allocated += pPools[a].GetAllocatedMemory();
	return allocated;
}

uint32 MFObjectPoolGroup::GetOverheadMemory()
{
	int overhead = 0;
	for(int a=0; a<numPools; ++a)
		overhead += pPools[a].GetOverheadMemory();
	return overhead;
}

int MFObjectPoolGroup::GetNumReserved()
{
	int reserved = 0;
	for(int a=0; a<numPools; ++a)
		reserved += pPools[a].GetNumReserved();
	return reserved;
}

int MFObjectPoolGroup::GetNumAllocated()
{
	int allocated = 0;
	for(int a=0; a<numPools; ++a)
		allocated += pPools[a].GetNumReserved();
	return allocated;
}
