#pragma once
#if !defined(_HKWIDGET_LISTBOX_H)
#define _HKWIDGET_LISTBOX_H

#include "UI/Widgets/HKWidgetLayout.h"
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

template<class T>
class HKArrayAdapter : public HKListAdapter
{
	HKArrayAdapter(HKDynamicArray<T> &_array) : array(_array)
	{
		array.onInsert += onInsertItem.GetDelegate();
		array.onRemove += onRemoveItem.GetDelegate();
		array.onChange += onTouchItem.GetDelegate();
	}

	virtual ~HKArrayAdapter()
	{
		array.onInsert -= onInsertItem.GetDelegate();
		array.onRemove -= onRemoveItem.GetDelegate();
		array.onChange -= onTouchItem.GetDelegate();
	}

	virtual int GetNumItems() { return array.length(); };

protected:
	HKDynamicArray<T> &array;
};

class HKWidgetListbox : public HKWidgetLayout
{
public:
	enum Orientation
	{
		Horizontal,
		Vertical
	};

	static HKWidget *Create();

	HKWidgetListbox();
	virtual ~HKWidgetListbox();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	Orientation GetOrientation() const { return orientation; }

	void SetOrientation(Orientation orientation) { this->orientation = orientation; }

	void Bind(HKListAdapter &adapter);
	void Unbind();

	HKListAdapter *GetAdapter() const { return pAdapter; }

	HKWidgetEvent OnSelChanged;

protected:
	Orientation orientation;

	HKListAdapter *pAdapter;
	HKWidgetLayout *pChildren;

	int selection;
	bool bHoverSelect;

	float contentSize;
	float scrollOffset, prevScrollOffset;
	float velocity;

	bool bDragging;

	virtual void Update();
	virtual bool InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev);
	virtual void ArrangeChildren();

	void OnInsert(int position, HKListAdapter &adapter);
	void OnRemove(int position, HKListAdapter &adapter);
	void OnChange(int position, HKListAdapter &adapter);

	static const EnumKeypair sOrientationKeys[];
};

#endif
