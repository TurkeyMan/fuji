#include "Fuji.h"
#include "MFPoolHeap.h"
#include "MFHeap.h"

void MFPoolHeap::Init(int num, int size, void * pMem, int memsize)
{
	MFDebug_Assert(num > 0 && size >= 4 && (size & 3) == 0, "Bad args");

	itemSize = size;
	numItems = num;
	pNext = NULL;
#if !defined(MF_RETAIL)
	peakNumUsed = 0;
#endif

	// Get the memory for the heap
	if(pMem)
	{
		MFDebug_Assert(memsize > num*size, "Not enought memory");

		pStorage = pMem;
		bOwnStorage = false;
	}
	else
	{
		pStorage = MFHeap_Alloc(num*size);
		bOwnStorage = true;
	}

	DeleteAll();
}

void MFPoolHeap::Destroy()
{
	MFDebug_Assert(itemSize != 0, "Not initialised");

	itemSize = 0;

	if(bOwnStorage)
		MFHeap_Free(pStorage);
}

size_t MFPoolHeap::Size() const
{
	return itemSize;
}

int MFPoolHeap::Num() const
{
	return numItems;
}

#if !defined(MF_RETAIL)
int MFPoolHeap::NumUsed() const
{
	return numUsed;
}

int MFPoolHeap::PeakNumUsed() const
{
	return peakNumUsed;
}

void MFPoolHeap::Dump() const
{
	MFDebug_Log(2, MFStr("Count %d, Used %d, Item Size %d bytes\n", numItems, numUsed, itemSize));
/*
	char *pEntry = (char*)pStorage;
	for(int i = 0; i < numItems; ++i)
	{
		bool bUsed = true;
		// check if it is in the free list
		void **pFree = pFreeList;
		while(pFree != NULL)
		{
			if(pEntry == (char*)pFree)
			{
				bUsed = false;
				pFree = NULL;
			}
			else
			{
				pFree = (void**)*pFree;
			}
		}

		if(bUsed)
			MFDebug_Log(MFStr("%d x \"%s\"\n", *(int*)pEntry, pEntry + sizeof(int)));

		pEntry += itemSize;
	}
*/
}
#endif

void *MFPoolHeap::Alloc()
{
	void *pItem = pFreeList;
	pFreeList = *(void**)pFreeList;

#if !defined(MF_RETAIL)
	if(pItem != NULL)
	{
		numUsed++;
		if(peakNumUsed < numUsed)
			peakNumUsed++;
	} 
#endif

	return pItem;
}

void MFPoolHeap::Delete(void *pItem)
{
	if(pItem)
	{
#if defined(_DEBUG)
		MFMemSet(pItem, 0xfe, itemSize);
#endif

		*(void**)pItem = pFreeList;
		pFreeList = pItem;

#if !defined(MF_RETAIL)
		numUsed--;
#endif
	}
}

void MFPoolHeap::DeleteAll()
{
#if !defined(MF_RETAIL)
	numUsed = 0;
#endif

	pFreeList = pStorage;

	char *pNext = (char*)pStorage;
	for(int i=0; i < numItems-1; ++i)
	{
		*(void**)pNext = pNext + itemSize;
		pNext += itemSize;
	}
	*(void**)pNext = NULL;
}

bool MFPoolHeap::IsFromThisHeap(void *pItem) const
{
	return pItem >= pStorage && pItem < (char*)pStorage + numItems*itemSize;
}

MFPoolHeap *MFPoolHeap::GetNextHeap() const
{
	return pNext;
}

void MFPoolHeap::SetNextHeap(MFPoolHeap *pHeap)
{
	pNext = pHeap;
}

void MFPoolHeapCollection::Init(int _numHeaps, const int *pNum, const int *pSizes, void *pMem, int memsize)
{
	MFDebug_Assert(_numHeaps > 0 && pNum && pSizes, "Bad parameters");

	numHeaps = _numHeaps;
#if !defined(MF_RETAIL)
	overflows = 0;
#endif

	if(pMem)
	{
		int size = (sizeof(MFPoolHeap*)+sizeof(MFPoolHeap))*numHeaps;
		for(int i=0; i<numHeaps; ++i)
			size += pNum[i]*pSizes[i];

		MFDebug_Assert(memsize >= size, "Not enough memory");
		pHeaps = (MFPoolHeap**)pMem;
		pMem = ((MFPoolHeap**)pMem) + numHeaps;
	}
	else
	{
		pHeaps = (MFPoolHeap**)MFHeap_Alloc(sizeof(MFPoolHeap*)*numHeaps);
	}

	int lastSize = 0;
	for(int i = 0; i < numHeaps; ++i)
	{
		const int size = pSizes[i];
		const int num = pNum[i];

		MFDebug_Assert(size > lastSize && num > 0, "Bad heap params");
		lastSize = size;

		if(pMem)
		{
			pHeaps[i] = (MFPoolHeap*)pMem;
			pMem = ((MFPoolHeap*)pMem) + 1;

			const int memorySize = size*num;
			pHeaps[i]->Init(num, size, pMem, memorySize);

			pMem = ((char *)pMem) + memorySize;
		}
		else
		{
			pHeaps[i] = (MFPoolHeap*)MFHeap_Alloc(sizeof(MFPoolHeap));
			pHeaps[i]->Init(num, size);
		}
	}
}

void MFPoolHeapCollection::Destroy()
{
	for(int i = 0; i < numHeaps; ++i)
	{
		pHeaps[i]->Destroy();

		// TODO: *** FIX ME!!!
		MFHeap_Free(pHeaps[i]);
	}

	MFHeap_Free(pHeaps);
}

void * MFPoolHeapCollection::Alloc(uint32 size)
{
	for(int i = 0; i < numHeaps; ++i)
	{
		MFPoolHeap *pHeap = pHeaps[i];

		const uint32 heapSize = pHeap->Size();
		if(size <= heapSize)
		{
			void *pMem = pHeap->Alloc();
			if(pMem)
				return pMem;
		}
	}

	// didn't get anything from the pools, get it from the main heap as an emergency fallback
#if !defined(MF_RETAIL)
	overflows++;
#endif

	return MFHeap_Alloc(size);
}

void *MFPoolHeapCollection::Realloc(void *pItem, uint32 size)
{
	if(!pItem)
		return Alloc(size);

	for(int i = 0; i < numHeaps; ++i)
	{
		MFPoolHeap *pHeap = pHeaps[i];

		if(pHeap->IsFromThisHeap(pItem))
		{
			uint32 heapSize = pHeap->Size();
			if(size <= heapSize)
				return pItem;

			void *pNewItem = Alloc(size);
			MFCopyMemory(pNewItem, pItem, heapSize);
			Delete(pItem);
			return pNewItem;
		}
	}

	return MFHeap_Realloc(pItem, size);
}

void MFPoolHeapCollection::Delete(void * pMem)
{
	for(int i = 0; i < numHeaps; ++i)
	{
		MFPoolHeap *pHeap = pHeaps[i];

		if(pHeap->IsFromThisHeap(pMem))
		{
			pHeap->Delete(pMem);
			return;
		}
	}

	MFHeap_Free(pMem);
}

#if !defined(MF_RETAIL)
int MFPoolHeapCollection::NumHeaps() const
{
	return numHeaps;
}

int MFPoolHeapCollection::SizeOfHeapItem(int heapIndex) const
{
	MFDebug_Assert(heapIndex >= 0 && heapIndex < numHeaps, "Invalid heap index");
	return pHeaps[heapIndex]->Size();
}

int MFPoolHeapCollection::NumInHeap(int heapIndex) const
{
	MFDebug_Assert(heapIndex >= 0 && heapIndex < numHeaps, "Invalid heap index");
	return pHeaps[heapIndex]->Num();
}

int MFPoolHeapCollection::NumUsedInHeap(int heapIndex) const
{
	MFDebug_Assert(heapIndex >= 0 && heapIndex < numHeaps, "Invalid heap index");
	return pHeaps[heapIndex]->NumUsed();
}

int MFPoolHeapCollection::PeakNumUsedInHeap(int heapIndex) const
{
	MFDebug_Assert(heapIndex >= 0 && heapIndex < numHeaps, "Invalid heap index");
	return pHeaps[heapIndex]->PeakNumUsed();
}

int MFPoolHeapCollection::PoolOverflows() const
{
	return overflows;
}

void MFPoolHeapCollection::Dump() const
{
	for(int i = 0; i < numHeaps; ++i)
	{
		pHeaps[i]->Dump();
	}
}
#endif


void MFPoolHeapExpanding::Init(int num, int size, int _expandNum, int maxNum)
{
	MFDebug_Assert(_expandNum > 0, "Bad expansion size");

	expandNum = _expandNum;
	numberTilFull = maxNum - num;
	heap.Init(num, size);
}

void MFPoolHeapExpanding::Destroy()
{
	MFPoolHeap *pHeap = heap.GetNextHeap();
	while(pHeap != NULL)
	{
		MFPoolHeap *pNextHeap = pHeap->GetNextHeap();
		pHeap->Destroy();
		MFHeap_Free(pHeap);
		pHeap = pNextHeap;
	}
	heap.Destroy();
}

void * MFPoolHeapExpanding::Alloc()
{
	void *pMem = heap.Alloc();
	if(pMem == NULL)
	{
		MFPoolHeap *pHeap = (MFPoolHeap*)MFHeap_Alloc(sizeof(MFPoolHeap));
		pHeap->Init(expandNum, heap.Size());

		pHeap->SetNextHeap(heap.GetNextHeap());
		heap.SetNextHeap(pHeap);

		pMem = pHeap->Alloc();

		// Add the rest of the new memory to the existing heap
		for(int i = 0; i < expandNum - 1; ++i)
			heap.Delete(pHeap->Alloc());

		numberTilFull -= expandNum;
		MFDebug_Assert(numberTilFull >= 0, "The heap has grown larger than the maximum size requested");
	}
	return pMem;
}

#if !defined(MF_RETAIL)
int MFPoolHeapExpanding::NumItemsAlloced()
{
	int count = heap.NumUsed();

	for(MFPoolHeap *pHeap = heap.GetNextHeap(); pHeap; pHeap = pHeap->GetNextHeap())
		count += pHeap->NumUsed();

	return count;
}
#endif
