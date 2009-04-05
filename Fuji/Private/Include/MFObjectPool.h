/**
 * @file MFObjectPool.h
 * @brief Provides a suite of fast pool allocators.
 * @author Manu Evans
 * @defgroup MFObjectPool Object pool
 * @{
 */

#if !defined(_MFOBJECTPOOL_H)
#define _MFOBJECTPOOL_H

class MFObjectPool
{
public:
	MFObjectPool() { ppItems = NULL; pMemory = NULL; pNext = NULL; bytes = allocated = maxItems = objectSize = grow = 0; }
	MFObjectPool(int objectSize, int numObjects, int growObjects = 0) { Init(objectSize, numObjects, growObjects); }
	~MFObjectPool() { Deinit(); }

	void Init(int objectSize, int numObjects, int growObjects = 0);
	void Deinit();

	void *Alloc();
	void *AllocAndZero();
	int Free(void *pItem);

private:
	int objectSize;
	int maxItems;
	int allocated;
	int grow;

	void **ppItems;
	char *pMemory;
	int bytes;

	MFObjectPool *pNext;
};


struct MFObjectPoolGroupConfig
{
	int objectSize;
	int numObjects;
	int growObjects;
};

class MFObjectPoolGroup
{
public:
	void Init(MFObjectPoolGroupConfig *pPools, int numPools);
	void Deinit();

	void *Alloc(int bytes, int *pAllocated);
	void *AllocAndZero(int bytes, int *pAllocated);
	void Free(void *pItem);

private:
	MFObjectPoolGroupConfig *pConfig;
	int numPools;

	MFObjectPool *pPools;
};

#endif

/** @} */
