#if !defined(_MFOPENHASHTABLE_H)
#define _MFOPENHASHTABLE_H

#include "MFHeap.h"
#include "MFObjectPool.h"

//#define SUPPORT_FLEXIBLE_TABLE_SIZE

template <typename T>
class MFOpenHashTable
{
private:
	struct Item
	{
		Item() {}
		Item(const T &item) : item(item) {}

		T item;
		uint32 hash;
		Item *pNext;
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
	T& Create(MFString key)					{ return Create(key.GetHash()); }
	T& Create(const char *pKey)				{ return Create(MFUtil_HashString(pKey)); }

	T& Add(uint32 hash, const T& item);
	T& Add(MFString key, const T& item)		{ return Add(key.GetHash(), item); }
	T& Add(const char *pKey, const T& item)	{ return Add(MFUtil_HashString(pKey), item); }

	void Destroy(uint32 hash);
	void Destroy(MFString key)				{ Destroy(key.GetHash()); }
	void Destroy(const char *pKey)			{ Destroy(MFUtil_HashString(pKey)); }

	bool Destroy(const T& item);
	bool DestroyItem(const T& item);	// this will only destroy an item that was allocated by this hash table

	Iterator First() const;
	Iterator Next(Iterator pI) const;

	Iterator NextMatch(Iterator pI) const;

	int GetNumItems() const { return itemCount; }

protected:
	MFObjectPool itemPool;
	Item **ppItems;
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
inline void MFOpenHashTable<T>::Init(int _tableSize, int maxItems, int growItems)
{
#if !defined(SUPPORT_FLEXIBLE_TABLE_SIZE)
	_tableSize = 256;
#endif
	itemPool.Init(sizeof(Item), maxItems, growItems);
	ppItems = (Item**)MFHeap_AllocAndZero(sizeof(Item*)*_tableSize);
	tableSize = _tableSize;
	itemCount = 0;
}

template <typename T>
inline void MFOpenHashTable<T>::Deinit()
{
	itemPool.Deinit();
	MFHeap_Free(ppItems);
	tableSize = 0;
	itemCount = 0;
}

template <typename T>
inline typename MFOpenHashTable<T>::Iterator MFOpenHashTable<T>::Get(uint32 hash) const
{
	uint32 i = GetTableIndex(hash);
	Item *pItem = ppItems[i];

	while(pItem && pItem->hash != hash)
		pItem = pItem->pNext;

	return pItem ? &pItem->item : (Iterator)NULL;
}

template <typename T>
inline T& MFOpenHashTable<T>::Create(uint32 hash)
{
	Item *pNew = (Item*)itemPool.Alloc();
	new(&pNew->item) T;

	pNew->hash = hash;

	uint32 i = GetTableIndex(hash);
	pNew->pNext = ppItems[i];
	ppItems[i] = pNew;

	++itemCount;

	return pNew->item;
}

template <typename T>
inline T& MFOpenHashTable<T>::Add(uint32 hash, const T& item)
{
	Item *pNew = (Item*)itemPool.Alloc();
	new(&pNew->item) T(item);

	pNew->hash = hash;

	uint32 i = GetTableIndex(hash);
	pNew->pNext = ppItems[i];
	ppItems[i] = pNew;

	++itemCount;

	return pNew->item;
}

template <typename T>
inline void MFOpenHashTable<T>::Destroy(uint32 hash)
{
	uint32 i = GetTableIndex(hash);

	Item *pTemp = ppItems[i];
	if(!pTemp)
		return;

	Item *pDel = NULL;
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
inline bool MFOpenHashTable<T>::DestroyItem(const T& item)
{
	if(itemPool.Owns(&item))
	{
		Item &i = (Item&)item;
		Destroy(i.hash);
		return true;
	}

	return false;
}

template <typename T>
inline bool MFOpenHashTable<T>::Destroy(const T& item)
{
	if(DestroyItem(item))
		return true;

	Item *pDel = NULL;
	for(int a=0; a<tableSize; ++a)
	{
		Item *pTemp = ppItems[a];
		if(!pTemp)
			continue;

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
		return true;
	}

	return false;
}

template <typename T>
inline typename MFOpenHashTable<T>::Iterator MFOpenHashTable<T>::First() const
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
inline typename MFOpenHashTable<T>::Iterator MFOpenHashTable<T>::Next(Iterator pItem) const
{
	if(!pItem)
		return First();

	Item *pI = (Item*)pItem;
	uint32 i = GetTableIndex(pI->hash);
	Item *pTemp = ppItems[i];

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
inline typename MFOpenHashTable<T>::Iterator MFOpenHashTable<T>::NextMatch(Iterator pItem) const
{
	if(pItem == NULL)
		return NULL;

	Item *pI = (Item*)pItem;
	uint32 i = GetTableIndex(pI->hash);
	Item *pTemp = ppItems[i];

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
