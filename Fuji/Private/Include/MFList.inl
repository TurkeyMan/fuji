template <class T>
inline MFList<T>::MFList()
{
	pHead = NULL;
}

template <class T>
inline MFList<T>::~MFList()
{
}

template <class T>
inline void MFList<T>::push(T *pItem)
{
	pItem->pNext = pHead;
	pHead = pItem;
}

template <class T>
inline void MFList<T>::insert(T *pItem, T *pAfter)
{
	if(pAfter)
	{
		pItem->pNext = pAfter->pNext;
		pAfter->pNext = pItem;
	}
	else
	{
		push(pItem);
	}
}

template <class T>
inline void MFList<T>::remove(T *pItem)
{
	if(pItem == pHead)
	{
		pHead = pItem->pNext;
		pItem->pNext = NULL;
	}
	else
	{
		for(T *pT = pHead; pT; pT = pT->pNext)
		{
			if(pT->pNext == pItem)
			{
				pT->pNext = pItem->pNext;
				pItem->pNext = NULL;
				break;
			}
		}
	}
}

template <class T>
inline T *MFList<T>::head()
{
	return pHead;
}

template <class T>
inline T *MFList<T>::next(T *pItem)
{
	return pItem->pNext;
}

template <class T>
inline MFDoubleLinkedList<T>::MFDoubleLinkedList()
{
	pHead = pTail = NULL;
}

template <class T>
inline MFDoubleLinkedList<T>::~MFDoubleLinkedList()
{
}

template <class T>
inline void MFDoubleLinkedList<T>::pushfront(T *pItem)
{
	pItem->pPrev = NULL;
	pItem->pNext = pHead;
	pHead = pItem;
	if(!pTail)
		pTail = pItem;
}

template <class T>
inline void MFDoubleLinkedList<T>::pushback(T *pItem)
{
	pItem->pNext = NULL;
	pItem->pPrev = pTail;
	pTail = pItem;
	if(!pHead)
		pHead = pItem;
}

template <class T>
inline void MFDoubleLinkedList<T>::insert(T *pItem, T *pAfter)
{
	if(!pAfter)
	{
		pushback(pItem);
	}
	else
	{
		pItem->pPrev = pAfter;
		pItem->pNext = pAfter->pNext;
		pAfter->pNext = pItem;
		if(pItem->pNext)
			pItem->pNext->pPrev = pItem;
		else
			pTail = pItem;
	}
}

template <class T>
inline void MFDoubleLinkedList<T>::insertbefore(T *pItem, T *pBefore)
{
	if(!pBefore)
	{
		pushfront(pItem);
	}
	else
	{
		pItem->pNext = pBefore;
		pItem->pPrev = pBefore->pPrev;
		pBefore->pPrev = pItem;
		if(pItem->pPrev)
			pItem->pPrev->pNext = pItem;
		else
			pHead = pItem;
	}
}

template <class T>
inline void MFDoubleLinkedList<T>::remove(T *pItem)
{
	if(pItem->pPrev)
		pItem->pPrev->pNext = pItem->pNext;
	else
		pHead = pItem->pNext;

	if(pItem->pNext)
		pItem->pNext->pPrev = pItem->pPrev;
	else
		pTail = pItem->pPrev;

	pItem->pPrev = pItem->pNext = NULL;
}

template <class T>
inline T *MFDoubleLinkedList<T>::head()
{
	return pHead;
}

template <class T>
inline T *MFDoubleLinkedList<T>::tail()
{
	return pTail;
}

template <class T>
inline T *MFDoubleLinkedList<T>::next(T *pItem)
{
	return pItem->pNext;
}

template <class T>
inline T *MFDoubleLinkedList<T>::prev(T *pItem)
{
	return pItem->pPrev;
}
