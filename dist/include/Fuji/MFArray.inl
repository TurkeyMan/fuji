#include "MFHeap.h"

template<class T>
inline MFArray<T>::MFArray()
{
	pData = NULL;
	allocated = 0;
	count = 0;
}

template<class T>
inline MFArray<T>::MFArray(const MFArray &from)
{
	pData = NULL;
	allocated = 0;
	count = 0;

	for(int i=0; i<from.size(); ++i)
		push(from[i]);
}

template<class T>
inline MFArray<T>::MFArray(int maxItems)
{
	pData = NULL;
	count = allocated = 0;

	reserve(maxItems);
}

template<class T>
inline MFArray<T>::~MFArray()
{
	clear();

	if(pData)
		MFHeap_Free(pData);
}

template<class T>
inline T& MFArray<T>::operator[](int i)
{
	MFDebug_Assert(i >= 0 && i < count, "Index out of bounds.");
	return pData[i];
}

template<class T>
inline const T& MFArray<T>::operator[](int i) const
{
	MFDebug_Assert(i >= 0 && i < count, "Index out of bounds.");
	return pData[i];
}

template<class T>
inline int MFArray<T>::reserve(int maxItems)
{
	return alloc(maxItems);
}

template<class T>
inline int MFArray<T>::size() const
{
	return count;
}

template<class T>
inline void MFArray<T>::resize(int maxItems)
{
	while(count < maxItems)
		push();

	while(count > maxItems)
		pop();
}

template<class T>
inline void MFArray<T>::clear()
{
	resize(0);
}

template<class T>
inline void MFArray<T>::remove(int i)
{
	for(int a=i; a<count; ++a)
		pData[a] = pData[a + 1];
	--count;
}

template<class T>
inline T& MFArray<T>::push()
{
	alloc(count + 1);
	T& newItem = pData[count++];
	new(&newItem) T();
	return newItem;
}

template<class T>
inline T& MFArray<T>::push(const T &item)
{
	alloc(count + 1);
	T& newItem = pData[count++];
	new(&newItem) T(item);
	return newItem;
}

template<class T>
inline T& MFArray<T>::pop()
{
	MFDebug_Assert(count > 0, "Array is empty.");

	T &item = pData[--count];
	item.~T();
	return item;
}

template<class T>
inline T& MFArray<T>::front()
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pData[0];
}

template<class T>
inline const T& MFArray<T>::front() const
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pData[0];
}

template<class T>
inline T& MFArray<T>::back()
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pData[count - 1];
}

template<class T>
inline const T& MFArray<T>::back() const
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pData[count - 1];
}

template<class T>
inline T* MFArray<T>::getPointer() const
{
	return pData;
}

template<class T>
T* MFArray<T>::getCopy() const
{
	T *pCopy = (T*)MFHeap_Alloc(count * sizeof(T));
	for(int a=0; a<count; ++a)
		new(pCopy + a) T(pData[a]);
	return pCopy;
}

template<class T>
inline int MFArray<T>::alloc(int count)
{
	if(count <= allocated)
		return allocated;

	int toAlloc = pData ? allocated : 8;
	while(toAlloc < count)
		toAlloc *= 2;

	if(toAlloc > allocated)
	{
		pData = (T*)MFHeap_Realloc(pData, sizeof(T) * toAlloc);
		allocated = toAlloc;
	}

	return allocated;
}

template<class T>
inline T* MFArray<T>::find(const T &item)
{
	for(int a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return &pData[a];
	}
	return NULL;
}

template<class T>
inline const T* MFArray<T>::find(const T &item) const
{
	for(int a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return &pData[a];
	}
	return NULL;
}

template<class T>
inline bool MFArray<T>::contains(const T &item) const
{
	for(int a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return true;
	}
	return false;
}

template<class T>
inline void MFArray<T>::qsort(int (*pPredicate)(const T *p1, const T *p2))
{
	::qsort(pData, count, sizeof(T), (int(*)(const void*, const void*))pPredicate);
}
