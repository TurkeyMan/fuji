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
	MFObjectPool() { ppItems = NULL; pMemory = NULL; pNext = NULL; bytes = allocated = maxItems = objectSize = grow = 0; }
	MFObjectPool(int objectSize, int numObjects, int growObjects = 0) { Init(objectSize, numObjects, growObjects); }
	~MFObjectPool() { Deinit(); }

	void Init(size_t objectSize, int numObjects, int growObjects = 0, void *pMemory = NULL, size_t bytes = 0);
	void Deinit();

	void *Alloc();
	void *AllocAndZero();
	int Free(void *pItem);

	uint32 GetObjectSize() { return objectSize; }

	uint32 GetTotalMemory();
	uint32 GetAllocatedMemory();
	uint32 GetOverheadMemory();

	int GetNumReserved();
	int GetNumAllocated();

	void *GetItem(int index);

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

	uint32 GetTotalMemory();
	uint32 GetAllocatedMemory();
	uint32 GetOverheadMemory();

	int GetNumReserved();
	int GetNumAllocated();

	int GetNumPools() { return numPools; }
	MFObjectPool *GetPool(int pool) { return pPools + pool; }

private:
	MFObjectPoolGroupConfig *pConfig;
	int numPools;
	int overflows;

	MFObjectPool *pPools;

	MFMutex mutex;
};

#endif

/** @} */
