#if !defined(_HKOPENHASHTABLE_H)
#define _HKOPENHASHTABLE_H

#include "MFHeap.h"
#include "MFObjectPool.h"

//#define SUPPORT_FLEXIBLE_TABLE_SIZE

template <typename T>
class HKOpenHashTable
{
private:
	struct HashItem
	{
		T item;
		uint32 hash;
		HashItem *pNext;
	};

public:
	typedef T* Iterator;

	void Init(int _tableSize, int maxItems, int growItems = 0);
	void Deinit();

	T& operator[](MFString key) const		{ return *Get(key.GetHash()); }
	T& operator[](const char *pKey) const	{ return *Get(MFUtil_HashString(pKey)); }

	Iterator Get(uint32 hash) const;
	Iterator Get(MFString key) const		{ return Get(key.GetHash()); }
	Iterator Get(const char *pKey) const	{ return Get(MFUtil_HashString(pKey)); }

	T& Create(uint32 hash);
	T& Create(T& item, MFString key)		{ return Create(key.GetHash()); }
	T& Create(T& item, const char *pKey)	{ return Create(MFUtil_HashString(pKey)); }

	T& Add(uint32 hash, const T& item);
	T& Add(MFString key, const T& item)		{ return Add(key.GetHash(), item); }
	T& Add(const char *pKey, const T& item)	{ return Add(MFUtil_HashString(pKey), item); }

	void Destroy(uint32 hash);
	void Destroy(MFString key)				{ Destroy(key.GetHash()); }
	void Destroy(const char *pKey)			{ Destroy(MFUtil_HashString(pKey)); }

	void Destroy(const T& item);

	Iterator First() const;
	Iterator Next(Iterator pI) const;

	Iterator NextMatch(Iterator pI) const;

protected:
	MFObjectPool itemPool;
	HashItem **ppItems;
	int tableSize;
	int itemCount;

	uint32 GetTableIndex(uint32 hash) const
	{
#if !defined(SUPPORT_FLEXIBLE_TABLE_SIZE)
		// 8 bit hash folding... we'll just fold the bottom 16 bits :/
		return ((hash >> 8) ^ hash) & (0xFF);
#else
		return hash % tableSize;
#endif
	}
};

template <typename T>
inline void HKOpenHashTable<T>::Init(int _tableSize, int maxItems, int growItems)
{
#if !defined(SUPPORT_FLEXIBLE_TABLE_SIZE)
	_tableSize = 256;
#endif
	itemPool.Init(sizeof(HashItem), maxItems, growItems);
	ppItems = (HashItem**)MFHeap_AllocAndZero(sizeof(HashItem*)*_tableSize);
	tableSize = _tableSize;
	itemCount = 0;
}

template <typename T>
inline void HKOpenHashTable<T>::Deinit()
{
	itemPool.Deinit();
	MFHeap_Free(ppItems);
	tableSize = 0;
	itemCount = 0;
}

template <typename T>
inline typename HKOpenHashTable<T>::Iterator HKOpenHashTable<T>::Get(uint32 hash) const
{
	uint32 i = GetTableIndex(hash);
	HashItem *pItem = ppItems[i];

	while(pItem && pItem->hash != hash)
		pItem = pItem->pNext;

	return pItem ? &pItem->item : (Iterator)NULL;
}

template <typename T>
inline T& HKOpenHashTable<T>::Create(uint32 hash)
{
	HashItem *pNew = (HashItem*)itemPool.Alloc();
	new(&pNew->item) T();

	pNew->hash = hash;

	uint32 i = GetTableIndex(hash);
	pNew->pNext = ppItems[i];
	ppItems[i] = pNew;

	++itemCount;

	return pNew->item;
}

template <typename T>
inline T& HKOpenHashTable<T>::Add(uint32 hash, const T& item)
{
	T& newItem = Create(hash);
	newItem = item;
	return newItem;
}

template <typename T>
inline void HKOpenHashTable<T>::Destroy(uint32 hash)
{
	uint32 i = GetTableIndex(hash);

	HashItem *pTemp = ppItems[i];
	HashItem *pDel = NULL;
	if(pTemp->hash == hash)
	{
		pDel = pTemp;
		ppItems[i] = pTemp->pNext;
	}
	else
	{
		while(pTemp->pNext && pTemp->hash != hash)
			pTemp = pTemp->pNext;
		if(pTemp->pNext)
		{
			pDel = pTemp->pNext;
			pTemp->pNext = pTemp->pNext->pNext;
		}
	}

	if(pDel)
	{
		pDel->item.~T();
		itemPool.Free(pDel);
		--itemCount;
	}
}

template <typename T>
inline void HKOpenHashTable<T>::Destroy(const T& item)
{
	HashItem *pDel = NULL;
	for(int a=0; a<tableSize; ++a)
	{
		HashItem *pTemp = ppItems[a];
		if(pTemp->item == item)
		{
			pDel = pTemp;
			ppItems[a] = pTemp->pNext;
			break;
		}
		else
		{
			while(pTemp->pNext && pTemp->pNext->item != item)
				pTemp = pTemp->pNext;
			if(pTemp->pNext)
			{
				pDel = pTemp->pNext;
				pTemp->pNext = pTemp->pNext->pNext;
				break;
			}
		}
	}

	if(pDel)
	{
		pDel->item.~T();
		itemPool.Free(pDel);
		--itemCount;
	}
}

template <typename T>
inline typename HKOpenHashTable<T>::Iterator HKOpenHashTable<T>::First() const
{
	if(!itemCount)
		return NULL;

	for(int a=0; a<tableSize; ++a)
	{
		if(ppItems[a])
			return &ppItems[a]->item;
	}
	return NULL;
}

template <typename T>
inline typename HKOpenHashTable<T>::Iterator HKOpenHashTable<T>::Next(Iterator pItem) const
{
	if(!pItem)
		return First();

	HashItem *pI = (HashItem*)pItem;
	uint32 i = GetTableIndex(pI->hash);
	HashItem *pTemp = ppItems[i];

	while(pTemp && &pTemp->item != pItem)
		pTemp = pTemp->pNext;

	if(!pTemp)
		return NULL;
	else if(pTemp->pNext)
		return &pTemp->pNext->item;
	else
	{
		for(int a=i+1; a<tableSize; ++a)
		{
			if(ppItems[a])
				return &ppItems[a]->item;
		}
	}
	return NULL;
}

template <typename T>
inline typename HKOpenHashTable<T>::Iterator HKOpenHashTable<T>::NextMatch(Iterator pItem) const
{
	if(pItem == NULL)
		return NULL;

	HashItem *pI = (HashItem*)pItem;
	uint32 i = GetTableIndex(pI->hash);
	HashItem *pTemp = ppItems[i];

	while(pTemp && &pTemp->item != pItem)
		pTemp = pTemp->pNext;

	if(pTemp)
	{
		pTemp = pTemp->pNext;
		while(pTemp)
		{
			if(pTemp->hash == pI->hash)
				return &pTemp->item;
			pTemp = pTemp->pNext;
		}
	}

	return NULL;
}

#endif
