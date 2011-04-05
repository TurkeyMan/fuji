#include "MFHeap.h"

template<class T>
inline MFArray<T>::MFArray()
{
	pData = NULL;
	allocated = 0;
	count = 0;
}

template<class T>
inline MFArray<T>::MFArray(int _count)
{
	pData = (T*)MFHeap_Alloc(sizeof(T) * _count);
	for(int a=0; a<_count; a++) new(&pData[a]) T();
	allocated = count = _count;
}

template<class T>
inline MFArray<T>::~MFArray()
{
	MFHeap_Free(pData);
}

template<class T>
inline T& MFArray<T>::operator[](int x)
{
	MFDebug_Assert(x >= 0, "Index out of bounds.");

	if(x >= allocated)
	{
		int oldAlloc = allocated;
		if(allocated == 0)
			allocated = 16;
		while(x >= allocated)
			allocated *= 2;

		pData = (T*)MFHeap_Realloc(pData, sizeof(T) * allocated);
		for(int a=oldAlloc; a<allocated; a++)
			new(&pData[a]) T();
	}

	count = MFMax(count, x+1);

	return pData[x];
}

template<class T>
inline const T& MFArray<T>::operator[](int x) const
{
	MFDebug_Assert(x >= 0 && x < count, "Index out of bounds.");
	return pData[x];
}

template<class T>
inline int MFArray<T>::size() const
{
	return count;
}

template<class T>
inline void MFArray<T>::resize(int x)
{
	if(x >= allocated)
	{
		int oldAlloc = allocated;
		if(allocated == 0)
			allocated = 16;
		while(x >= allocated)
			allocated *= 2;

		pData = (T*)MFHeap_Realloc(pData, sizeof(T) * allocated);
		for(int a=oldAlloc; a<allocated; a++)
			new(&pData[a]) T();
	}

	count = x;
}

template<class T>
inline void MFArray<T>::clear()
{
	count = 0;
}

template<class T>
inline T& MFArray<T>::push()
{
	return operator[](size());
}

template<class T>
inline T& MFArray<T>::push(const T &x)
{
	return operator[](size()) = x;
}

template<class T>
inline T& MFArray<T>::pop()
{
	--count;

	return pData[count-1];
}

template<class T>
inline T& MFArray<T>::front()
{
	return operator[](0);
}

template<class T>
inline const T& MFArray<T>::front() const
{
	return operator[](0);
}

template<class T>
inline T& MFArray<T>::back()
{
	return pData[count-1];
}

template<class T>
inline const T& MFArray<T>::back() const
{
	return pData[count-1];
}

template<class T>
inline T* MFArray<T>::getpointer() const
{
	return pData;
}
