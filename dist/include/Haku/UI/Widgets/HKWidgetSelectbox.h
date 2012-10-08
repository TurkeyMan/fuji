#pragma once
#if !defined(_HKWIDGET_SELECTBOX_H)
#define _HKWIDGET_SELECTBOX_H

#include "UI/Widgets/HKWidgetListbox.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"

#include "Fuji/MFFont.h"

class HKWidgetSelectbox : public HKWidgetLayoutFrame
{
public:
	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "Selectbox"; }

	HKWidgetSelectbox(HKWidgetType *pType);
	virtual ~HKWidgetSelectbox();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	void Bind(HKListAdapter &adapter);
	void Unbind();

	HKListAdapter *GetAdapter() const { return pAdapter; }

	int GetNumItems() const { return pAdapter ? pAdapter->GetNumItems() : 0; }
	int GetSelection() const { return selection; }

	void SetSelection(int item);

	HKWidgetEvent OnSelChanged;

protected:
	virtual bool InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev);

	HKListAdapter *pAdapter;
	HKWidgetListbox *pListbox;

	HKWidget *pSelectedView;

	HKInputSource *pExclusiveSource;

	int selection;

	void OnItemTouched(int position, HKListAdapter &adapter);

	void OnListClick(HKWidget &widget, const HKWidgetEventInfo &ev);
};

#endif
