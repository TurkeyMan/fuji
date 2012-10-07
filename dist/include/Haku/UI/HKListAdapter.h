#pragma once
#if !defined(_HKLISTADAPTER_H)
#define _HKLISTADAPTER_H

#include "HKEvent.h"
#include "HKDynamicArray.h"

class HKListAdapter
{
public:
	typedef HKEvent2<int, HKListAdapter&> HKListAdapterEvent;

	HKListAdapter() {}
	virtual ~HKListAdapter() {}

	virtual int GetNumItems() = 0;

	virtual HKWidget *GetItemView(int item) = 0;
	virtual void UpdateItemView(int item, HKWidget *pLayout) = 0;

	HKListAdapterEvent onInsertItem;
	HKListAdapterEvent onRemoveItem;
	HKListAdapterEvent onTouchItem;
};

template<typename T>
class HKArrayAdapter : public HKListAdapter
{
public:
	HKArrayAdapter(HKDynamicArray<T> &array) : array(array)
	{
		array.onInsert += fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnInsert);
		array.onRemove += fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnRemove);
		array.onChange += fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnTouch);
	}

	virtual ~HKArrayAdapter()
	{
		array.onInsert -= fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnInsert);
		array.onRemove -= fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnRemove);
		array.onChange -= fastdelegate::MakeDelegate(this, &HKArrayAdapter::OnTouch);
	}

	virtual int GetNumItems() { return array.length(); };

	virtual HKWidget *GetItemView(int index, T& item) = 0;
	virtual void UpdateItemView(int index, T& item, HKWidget *pLayout) = 0;

	HKDynamicArray<T> &GetArray() { return array; }

protected:
	HKDynamicArray<T> &array;

	HKWidget *GetItemView(int item) { return GetItemView(item, array[item]); }
	void UpdateItemView(int item, HKWidget *pLayout) { UpdateItemView(item, array[item], pLayout); }

	void OnInsert(T&, int item, HKDynamicArray<T>&) { onInsertItem(item, *this); }
	void OnRemove(T&, int item, HKDynamicArray<T>&) { onRemoveItem(item, *this); }
	void OnTouch(T&, int item, HKDynamicArray<T>&)  { onTouchItem(item, *this); }
};

#endif
