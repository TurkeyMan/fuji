/**
 * @file MFObjectPool.h
 * @brief Provides a suite of fast pool allocators.
 * @author Manu Evans
 * @defgroup MFObjectPool Object pool
 * @{
 */

#if !defined(_MFOBJECTPOOL_H)
#define _MFOBJECTPOOL_H

#include "MFThread.h"

class MFObjectPool
{
public:
	MFObjectPool() { ppItems = NULL; pMemory = NULL; pNext = NULL; bytes = objectSize = 0; allocated = maxItems = grow = 0; }
	MFObjectPool(size_t objectSize, int numObjects, int growObjects = 0) { Init(objectSize, numObjects, growObjects); }
	~MFObjectPool() { Deinit(); }

	void Init(size_t objectSize, int numObjects, int growObjects = 0, void *pMemory = NULL, size_t bytes = 0);
	void Deinit();

	void *Alloc();
	void *AllocAndZero();
	int Free(void *pItem);

	bool Owns(const void *pItem) const;

	size_t GetObjectSize() const { return objectSize; }

	size_t GetTotalMemory() const;
	size_t GetAllocatedMemory() const;
	size_t GetOverheadMemory() const;

	int GetNumReserved() const;
	int GetNumAllocated() const;

	void *GetItem(int index) const;

private:
	size_t objectSize;
	int maxItems;
	int allocated;
	int grow;

	char *pMemory;
	void **ppItems;
	size_t bytes;
	bool bOwnMemory;

	MFObjectPool *pNext;

	MFMutex mutex;
};


struct MFObjectPoolGroupConfig
{
	size_t objectSize;
	int numObjects;
	int growObjects;
};

class MFObjectPoolGroup
{
public:
	void Init(const MFObjectPoolGroupConfig *pPools, int numPools);
	void Deinit();

	void *Alloc(size_t bytes, size_t *pAllocated);
	void *AllocAndZero(size_t bytes, size_t *pAllocated);
	void Free(void *pItem);

	bool Owns(const void *pItem) const;

	size_t GetTotalMemory() const;
	size_t GetAllocatedMemory() const;
	size_t GetOverheadMemory() const;

	int GetNumReserved() const;
	int GetNumAllocated() const;

	int GetNumPools() const { return numPools; }
	MFObjectPool *GetPool(int pool) const { return pPools + pool; }

private:
	MFObjectPoolGroupConfig *pConfig;
	int numPools;
	int overflows;

	MFObjectPool *pPools;

	MFMutex mutex;
};

#endif

/** @} */
