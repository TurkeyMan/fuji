template<class T>
inline HKDynamicArray<T>::HKDynamicArray()
{
	pItems = NULL;
	count = allocated = 0;
}

template<class T>
inline HKDynamicArray<T>::HKDynamicArray(int maxItems)
{
	pItems = NULL;
	count = allocated = 0;

	reserve(maxItems);
}

template<class T>
inline HKDynamicArray<T>::~HKDynamicArray()
{
	clear();

	if(pItems)
		MFHeap_Free(pItems);
}

template<class T>
inline T& HKDynamicArray<T>::operator[](int i) const
{
	MFDebug_Assert(i >= 0 && i < count, "Index out of bounds.");
	return pItems[i];
}

template<class T>
inline T& HKDynamicArray<T>::get(int i) const
{
	MFDebug_Assert(i >= 0 && i < count, "Index out of bounds.");
	return pItems[i];
}

template<class T>
inline T& HKDynamicArray<T>::front() const
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pItems[0];
}

template<class T>
inline T& HKDynamicArray<T>::back() const
{
	MFDebug_Assert(count > 0, "Array is empty.");
	return pItems[count - 1];
}

template<class T>
inline T& HKDynamicArray<T>::push()
{
	alloc(count + 1);

	int i = count++;

	T& newItem = pItems[i];
	new(&newItem) T();

	onInsert(newItem, i, *this);
	onChange(newItem, i, *this);

	return newItem;
}

template<class T>
inline T& HKDynamicArray<T>::push(T &item)
{
	alloc(count + 1);

	int i = count++;

	T& newItem = pItems[i];
	new(&newItem) T(item);

	onInsert(newItem, i, *this);
	onChange(newItem, i, *this);

	return newItem;
}

template<class T>
inline T& HKDynamicArray<T>::pop()
{
	MFDebug_Assert(count > 0, "Array is empty.");

	T &item = pItems[--count];

	onRemove(item, count, *this);
	item.~T();

	return item;
}

template<class T>
inline int HKDynamicArray<T>::length() const
{
	return count;
}

template<class T>
inline void HKDynamicArray<T>::clear()
{
	resize(0);
}

template<class T>
inline void HKDynamicArray<T>::touch(int i) const
{
	onChange(pItems[i], i, *this);
}

template<class T>
inline int HKDynamicArray<T>::reserve(int maxItems)
{
	return alloc(maxItems);
}

template<class T>
inline void HKDynamicArray<T>::resize(int length)
{
	while(count < length)
		push();

	while(count > length)
		pop();
}

template<class T>
inline int HKDynamicArray<T>::alloc(int count)
{
	if(count <= allocated)
		return allocated;

	int toAlloc = pItems ? allocated : 8;
	while(toAlloc < count)
		toAlloc *= 2;

	if(toAlloc > allocated)
	{
		pItems = (T*)MFHeap_Realloc(pItems, sizeof(T) * toAlloc);
		allocated = toAlloc;
	}

	return allocated;
}
