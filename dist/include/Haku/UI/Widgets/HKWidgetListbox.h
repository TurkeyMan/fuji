#pragma once
#if !defined(_HKWIDGET_LISTBOX_H)
#define _HKWIDGET_LISTBOX_H

#include "Haku/UI/Widgets/HKWidgetLayout.h"
#include "Haku/UI/HKListAdapter.h"

class HKWidgetListbox : public HKWidgetLayout
{
public:
	enum Orientation
	{
		Horizontal,
		Vertical
	};

	enum Flags
	{
		HoverSelect = 1
	};

	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "Listbox"; }

	HKWidgetListbox(HKWidgetType *pType);
	virtual ~HKWidgetListbox();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	void Bind(HKListAdapter &adapter);
	void Unbind();

	HKListAdapter *GetAdapter() const { return pAdapter; }

	int GetNumItems() const { return pAdapter ? pAdapter->GetNumItems() : 0; }
	HKWidget *GetItemView(int item) const { return children[item].pChild->GetChild(0); }

	Orientation GetOrientation() const { return orientation; }
	int GetSelection() { return selection; }

	float GetMaxSize() const;

	void SetOrientation(Orientation orientation) { this->orientation = orientation; }
	void SetFlags(uint32 flags) { this->flags = flags; }
	void SetSelection(int item);

	HKWidgetEvent OnSelChanged;
	HKWidgetEvent OnClicked;

protected:
	Orientation orientation;

	HKListAdapter *pAdapter;
	HKWidget *pOldFocus;

	int selection;

	float contentSize;
	float scrollOffset, prevScrollOffset;
	float velocity;

	uint32 flags;

	bool bDragging;

	void AddView(HKWidget *pView);

	virtual void Update();
	virtual bool InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev);
	virtual void ArrangeChildren();

	void OnInsert(int position, HKListAdapter &adapter);
	void OnRemove(int position, HKListAdapter &adapter);
	void OnChange(int position, HKListAdapter &adapter);

	void OnItemDown(HKWidget &widget, const HKWidgetEventInfo &ev);
	void OnItemClick(HKWidget &widget, const HKWidgetEventInfo &ev);
	void OnItemOver(HKWidget &widget, const HKWidgetEventInfo &ev);
	void OnItemOut(HKWidget &widget, const HKWidgetEventInfo &ev);

	int GetChildIndex(HKWidget *pWidget);

	static const EnumKeypair sOrientationKeys[];
};

#endif
