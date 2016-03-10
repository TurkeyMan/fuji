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

	for(size_t i=0; i<from.size(); ++i)
		push(from[i]);
}

template<class T>
inline MFArray<T>::MFArray(size_t maxItems)
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
inline T& MFArray<T>::operator[](size_t i)
{
	MFDebug_Assert(i < count, "Index out of bounds.");
	return pData[i];
}

template<class T>
inline const T& MFArray<T>::operator[](size_t i) const
{
	MFDebug_Assert(i < count, "Index out of bounds.");
	return pData[i];
}

template<class T>
inline size_t MFArray<T>::reserve(size_t maxItems)
{
	return alloc(maxItems);
}

template<class T>
inline size_t MFArray<T>::size() const
{
	return count;
}

template<class T>
inline void MFArray<T>::resize(size_t maxItems)
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
inline void MFArray<T>::remove(size_t i)
{
	for(size_t a=i; a<count; ++a)
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
	for(size_t a=0; a<count; ++a)
		new(pCopy + a) T(pData[a]);
	return pCopy;
}

template<class T>
inline size_t MFArray<T>::alloc(size_t _count)
{
	if(_count <= allocated)
		return allocated;

	size_t toAlloc = pData ? allocated : 8;
	while(toAlloc < _count)
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
	for(size_t a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return &pData[a];
	}
	return NULL;
}

template<class T>
inline const T* MFArray<T>::find(const T &item) const
{
	for(size_t a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return &pData[a];
	}
	return NULL;
}

template<class T>
inline bool MFArray<T>::contains(const T &item) const
{
	for(size_t a=0; a<count; ++a)
	{
		if(pData[a] == item)
			return true;
	}
	return false;
}

#include <cstdlib>

template<class T>
inline void MFArray<T>::qsort(int (*pPredicate)(const T *p1, const T *p2))
{
	::qsort(pData, count, sizeof(T), (int(*)(const void*, const void*))pPredicate);
}


/*
// STL interface:
template<class T>
inline MFArray<T>::iterator MFArray<T>::begin()
{
	return pData;
}

template<class T>
inline MFArray<T>::const_iterator MFArray<T>::begin() const
{
	return pData;
}

template<class T>
inline MFArray<T>::iterator MFArray<T>::end()
{
	return pData + count;
}

template<class T>
inline MFArray<T>::const_iterator MFArray<T>::end() const
{
	return pData + count;
}

  reverse_iterator rbegin()
    { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }
  reverse_iterator rend()
    { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }

template<class T>
inline MFArray<T>::size_type MFArray<T>::size() const
{
	return count;
}

template<class T>
inline MFArray<T>::size_type MFArray<T>::max_size() const
{
	return (size_type)-1;
}

template<class T>
inline MFArray<T>::size_type MFArray<T>::capacity() const
{
	return (size_type)allocated;
}

template<class T>
inline MFArray<T>::bool MFArray<T>::empty() const
{
	return count == 0;
}

  reference operator[](size_type __n) { return *(begin() + __n); }
  const_reference operator[](size_type __n) const { return *(begin() + __n); }

    reference at(size_type __n)
    { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const
    { _M_range_check(__n); return (*this)[__n]; }

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }

  void push_back(const _Tp& __x) {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish, __x);
      ++_M_finish;
    }
    else
      _M_insert_aux(end(), __x);
  }
  void push_back() {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish);
      ++_M_finish;
    }
    else
      _M_insert_aux(end());
  }
  void swap(vector<_Tp, _Alloc>& __x) {
    __STD::swap(_M_start, __x._M_start);
    __STD::swap(_M_finish, __x._M_finish);
    __STD::swap(_M_end_of_storage, __x._M_end_of_storage);
  }

  iterator insert(iterator __position, const _Tp& __x) {
    size_type __n = __position - begin();
    if (_M_finish != _M_end_of_storage && __position == end()) {
      construct(_M_finish, __x);
      ++_M_finish;
    }
    else
      _M_insert_aux(__position, __x);
    return begin() + __n;
  }
  iterator insert(iterator __position) {
    size_type __n = __position - begin();
    if (_M_finish != _M_end_of_storage && __position == end()) {
      construct(_M_finish);
      ++_M_finish;
    }
    else
      _M_insert_aux(__position);
    return begin() + __n;
  }

  void pop_back() {
    --_M_finish;
    destroy(_M_finish);
  }
  iterator erase(iterator __position) {
    if (__position + 1 != end())
      copy(__position + 1, _M_finish, __position);
    --_M_finish;
    destroy(_M_finish);
    return __position;
  }
  iterator erase(iterator __first, iterator __last) {
    iterator __i = copy(__last, _M_finish, __first);
    destroy(__i, _M_finish);
    _M_finish = _M_finish - (__last - __first);
    return __first;
  }

  void resize(size_type __new_size, const _Tp& __x) {
    if (__new_size < size()) 
      erase(begin() + __new_size, end());
    else
      insert(end(), __new_size - size(), __x);
  }
  void resize(size_type __new_size) { resize(__new_size, _Tp()); }
  void clear() { erase(begin(), end()); }

  void reserve(size_type __n) {
    if (capacity() < __n) {
      const size_type __old_size = size();
      iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __tmp;
      _M_finish = __tmp + __old_size;
      _M_end_of_storage = _M_start + __n;
    }
  }
*/
