template<class T>
const T* MFHashList<T>::operator[](const char *pHashString) const
{
	uint32 crc = MFUtil_CrcString(pHashBuffer);
	uint32 id = crc%listSize;

	//.....
}

template<class T>
T* MFHashList<T>::operator[](const char *pHashString)
{
	uint32 crc = MFUtil_CrcString(pHashBuffer);
	uint32 id = crc%listSize;

	//.....
}

template<class T>
void MFHashList<T>::Init(const char *pGroupName, uint32 maxElements)
{
	pList = (MFHashListItem*)MFHeap_Alloc(sizeof(T)*maxElements);
	MFZeroMemory(pList, sizeof(T)*maxElements);
}

template<class T>
void MFHashList<T>::Deinit()
{
	MFHeap_Free(pList);
	pList = NULL;
}

template<class T>
T* MFHashList<T>::Create(T* pItem, const char *pHashBuffer, int len = -1)
{
	uint32 crc = len == -1 ? MFUtil_CrcString(pHashBuffer) : MFUtil_Crc(pHashBuffer, len);
	uint32 id = crc%listSize;

	//.....
}

template<class T>
void MFHashList<T>::Destroy(const char *pHashBuffer, int len = -1)
{
	uint32 crc = len == -1 ? MFUtil_CrcString(pHashBuffer) : MFUtil_Crc(pHashBuffer, len);
	uint32 id = crc%listSize;

	//.....

	// NOTE: destroying an item may make following items that were meant for this key inaccessable.. following items should be shuffled back into this space..
}

template<class T>
void MFHashList<T>::Destroy(uint32 hash)
{
	uint32 id = hash%listSize;

	//.....

	// NOTE: destroying an item may make following items that were meant for this key inaccessable.. following items should be shuffled back into this space..
}

template<class T>
void MFHashList<T>::DestroyItem(T* pItem)
{
	for(int a=0; a<listSize; a++)
	{
		if(pList[a].pItem == pItem)
		{
			pList[a].pItem = NULL;
			pList[a].hash = 0;
			break;
		}
	}

	// NOTE: destroying an item may make following items that were meant for this key inaccessable.. following items should be shuffled back into this space..
}

template<class T>
T* MFHashList<T>::Find(const char *pHashBuffer, int len = -1)
{
	uint32 crc = len == -1 ? MFUtil_CrcString(pHashBuffer) : MFUtil_Crc(pHashBuffer, len);
	uint32 id = crc%listSize;

	uint32 i = id;

	while(pList[i].pItem)
	{
		if(crc == pList[id].hash)
			return pList[id].pItem;

		++i;
		if(i == numItems) i==0;
		if(i == id) return NULL;
	}

	return NULL;
}

template<class T>
void MFHashList<T>::Clear()
{
	for(int a=0; a<listSize; a++)
	{
		pList[a].pItem = NULL;
	}

	numItems = 0;
}

template<class T>
int MFHashList<T>::GetNumItems()
{
	return numItems;
}

template<class T>
int MFHashList<T>::GetMaxItems()
{
	return listSize;
}

template<class T>
bool MFHashList<T>::IsFull()
{
	return numItems == listSize;
}

template<class T>
bool MFHashList<T>::IsEmpty()
{
	return numItems == 0;
}
