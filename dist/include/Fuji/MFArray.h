#if !defined(_MFARRAY_H)
#define _MFARRAY_H

#if defined(_MSC_VER)
#pragma warning(disable: 4345)
#endif

#include "MFHeap.h"
#include "MFObjectPool.h"

#include <vector>

template<class T>
class MFAllocator : public std::allocator<T>
{
public:
	pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
	{
		// TODO: what to do with 'hint'?
		return (pointer)MFHeap_Alloc(sizeof(T)*n);
	}

	void deallocate(pointer p, size_type n)
	{
		MFHeap_Free(p);
	}
};

template<class T, size_t PoolSize, size_t GrowSize = 0>
class MFPoolAllocator : public std::allocator<T>
{
public:
	MFPoolAllocator()
	: pool(sizeof(T), PoolSize, GrowSize)
	{}

	pointer allocate(size_type n, allocator<void>::const_pointer hint = 0)
	{
		// TODO: what to do with 'hint'?
		return (pointer)pool.Alloc();
	}

	void deallocate(pointer p, size_type n)
	{
		Ppool.Free(p);
	}

private:
	MFObjectPool pool;
};


template<class T>
class MFArray
{
public:
/*
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;

  reverse_iterator rbegin()
    { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }
  reverse_iterator rend()
    { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }

  size_type size() const { return size_type(end() - begin()); }
  size_type max_size() const;
  size_type capacity() const;
  bool empty() const;

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

	MFArray();
	MFArray(size_t maxItems);
	MFArray(const MFArray &from);
	~MFArray();

	T& operator[](size_t i);
	const T& operator[](size_t i) const;

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	T& push();
	T& push(const T &item);
	T& pop();

	size_t reserve(size_t maxItems);

	size_t size() const;
	void resize(size_t length);
	void clear();

	void remove(size_t i);

	T* getPointer() const;
	T* getCopy() const;

	T* find(const T &item);
	const T* find(const T &item) const;

	bool contains(const T &item) const;

	void qsort(int (*pPredicate)(const T *p1, const T *p2));

protected:
	size_t alloc(size_t count);

	size_t count;
	size_t allocated;
	T *pData;
};

#include "MFArray.inl"

#endif
