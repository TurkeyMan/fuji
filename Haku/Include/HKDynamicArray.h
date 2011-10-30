#pragma once
#if !defined(_HKDYNAMICARRAY_H)
#define _HKDYNAMICARRAY_H

#include "HKEvent.h"

template<class T>
class HKDynamicArray
{
public:
	typedef HKEvent3<T&, int, HKDynamicArray&> HKDynamicArrayEvent;

	HKDynamicArray();
	HKDynamicArray(int maxItems);
	~HKDynamicArray();

	T& operator[](int i) const;

	T& get(int i) const;

	T& front() const;
	T& back() const;

	T& push();
	T& push(T &item);
	T& pop();

	int reserve(int maxItems);

	int length() const;

	void resize(int length);
	void clear();

	void touch(int i) const;

	HKDynamicArrayEvent onInsert;
	HKDynamicArrayEvent onChange;
	HKDynamicArrayEvent onRemove;

protected:
	int alloc(int count);

	T *pItems;
	int count;
	int allocated;
};

#include "HKDynamicArray.inl"

#endif
