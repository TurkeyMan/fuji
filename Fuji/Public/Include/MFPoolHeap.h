#if !defined(_MFPOOLHEAP_H)
#define _MFPOOLHEAP_H

class MFPoolHeap
{
	friend class MFPoolHeapExpanding;
public:
	void Init(int num, size_t size, void *pMem = NULL, size_t memsize = 0);
	void Destroy();

	size_t Size() const;
	void *Alloc();
	void Delete(void *pItem);
	void DeleteAll();
	bool IsFromThisHeap(void *pItem) const;
	int Num() const;
#if !defined(MF_RETAIL)
	int NumUsed() const;
	int PeakNumUsed() const;
	void Dump() const;
	void *GetStorage() const { return pStorage; }
#endif

	MFPoolHeap *GetNextHeap() const;
	void SetNextHeap(MFPoolHeap* pHeap);

private:
	void *pFreeList;
	size_t itemSize;
	int numItems;
	void *pStorage;
	bool bOwnStorage;
	MFPoolHeap *pNext;
#if !defined(MF_RETAIL)
	int numUsed;
	int peakNumUsed;
#endif
};

class MFPoolHeapCollection
{
public:
	void Init(int numHeaps, const int *pNum, const size_t *pSizes, void *pMem = NULL, size_t memsize = 0);
	void Destroy();

	void *Alloc(size_t size);
	void *Realloc(void *pItem, size_t size);
	void Delete(void *pItem);

#if !defined(MF_RETAIL)
	int NumHeaps() const;
	size_t SizeOfHeapItem(int heapIndex) const;
	int NumInHeap(int heapIndex) const;
	int NumUsedInHeap(int heapIndex) const;
	int PeakNumUsedInHeap(int heapIndex) const;
	int PoolOverflows() const;
	void Dump() const;
#endif

private:
	int numHeaps;
	MFPoolHeap **pHeaps;
#if !defined(MF_RETAIL)
	int overflows;
#endif
};

class MFPoolHeapExpanding
{
public:
	void Init(int num, int size, int expandNum, int maxNum = 0x7FFFFFFF);
	void Destroy();

	void *Alloc();
	void Delete(void *pItem) { heap.Delete(pItem); }

	int NumItemsAlloced() { return heap.NumUsed(); }

private:
	MFPoolHeap heap;
	int expandNum;
	int numberTilFull;
};

// Calculates the size of the memory needed to hold a fixed size heap pool less storage.  Storage for each heap is num * size
#define MKFIXEDSIZEHEAPPOOLMEMSIZEBASE(numHeaps)  (sizeof(MKFixedSizeHeapPool)+(sizeof(MKFixedSizeHeap)+sizeof(MKLockFreePointer)+sizeof(MKFixedSizeHeap *))*(numHeaps))

#endif
