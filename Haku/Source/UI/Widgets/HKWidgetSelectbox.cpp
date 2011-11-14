#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetSelectbox.h"

#include "MFFont.h"

HKWidget *HKWidgetSelectbox::Create(HKWidgetType *pType)
{
	return new HKWidgetSelectbox(pType);
}


HKWidgetSelectbox::HKWidgetSelectbox(HKWidgetType *pType)
: HKWidgetLayoutFrame(pType)
{
	pListbox = HKUserInterface::Get().CreateWidget<HKWidgetListbox>();
	pListbox->SetProperty("background_colour", "black");
	pListbox->SetFlags(HKWidgetListbox::HoverSelect);
	pListbox->OnClicked += fastdelegate::MakeDelegate(this, &HKWidgetSelectbox::OnListClick);

	pAdapter = NULL;

	pExclusiveSource = NULL;

	selection = -1;

	bClickable = true;

	padding = MakeVector(2,2,2,2);
}

HKWidgetSelectbox::~HKWidgetSelectbox()
{
	Unbind();

	delete pListbox;
}

bool HKWidgetSelectbox::InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			// calculate size and position
			MFMatrix mat = GetTransform();

			MFVector scale = MakeVector(mat.GetXAxis().Magnitude3(), mat.GetYAxis().Magnitude3(), mat.GetZAxis().Magnitude3());
			MFVector size = GetSize();

			MFVector pos = mat.GetTrans();
			pos.y += size.y * scale.y;

			size.y = MFMin(pListbox->GetMaxSize(), 300.f);

			pListbox->SetScale(scale);
			pListbox->SetPosition(pos);
			pListbox->SetSize(size);

			// show listbox
			pExclusiveSource = ev.pSource;
			GetUI().AddTopLevelWidget(pListbox, false);
			GetUI().SetFocus(pExclusiveSource, pListbox);
			break;
		}
		default:
			break;
	}

	return HKWidgetLayoutFrame::InputEvent(manager, ev);
}

void HKWidgetSelectbox::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "onSelChanged"))
		HKWidget_BindWidgetEvent(OnSelChanged, pValue);
	else
		HKWidgetLayoutFrame::SetProperty(pProperty, pValue);
}

MFString HKWidgetSelectbox::GetProperty(const char *pProperty)
{
	return HKWidgetLayoutFrame::GetProperty(pProperty);
}

void HKWidgetSelectbox::Bind(HKListAdapter &adapter)
{
	Unbind();

	pAdapter = &adapter;

	// subscribe for list adapter events
//	adapter.onInsertItem += fastdelegate::MakeDelegate(this, &HKWidgetSelectbox::OnInsert);
//	adapter.onRemoveItem += fastdelegate::MakeDelegate(this, &HKWidgetSelectbox::OnRemove);
	adapter.onTouchItem += fastdelegate::MakeDelegate(this, &HKWidgetSelectbox::OnItemTouched);

	// bind also to the listbox
	pListbox->Bind(adapter);

	if(adapter.GetNumItems() > 0)
		pListbox->SetSelection(0);
}

void HKWidgetSelectbox::Unbind()
{
	// unbind the listbox
	pListbox->Unbind();

	if(pAdapter)
	{
/*
		// unsubscribe from adapter
		pAdapter->onInsertItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnInsert);
		pAdapter->onRemoveItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnRemove);
*/
		pAdapter->onTouchItem -= fastdelegate::MakeDelegate(this, &HKWidgetSelectbox::OnItemTouched);

		pAdapter = NULL;
	}

	ClearChildren();

	selection = -1;
}

void HKWidgetSelectbox::SetSelection(int item)
{
	if(selection != item)
	{
		selection = item;

		pListbox->SetSelection(item);

		// set the selected item
		// TODO: rework this >_< (messy!!)
		ClearChildren();
		if(item > -1)
		{
			// create the selection view
			pSelectedView = pAdapter->GetItemView(item);
			pAdapter->UpdateItemView(item, pSelectedView);
			AddChild(pSelectedView, true);
		}
		else
		{
			pSelectedView = NULL;
		}

		HKWidgetSelectEvent ev(this, item);
		OnSelChanged(*this, ev);
	}
}

void HKWidgetSelectbox::OnItemTouched(int position, HKListAdapter &adapter)
{
	// update the current view
	if(position == selection)
		adapter.UpdateItemView(position, pSelectedView);
	else if(selection == -1)
		SetSelection(0);
}

void HKWidgetSelectbox::OnListClick(HKWidget &widget, const HKWidgetEventInfo &ev)
{
	HKWidgetSelectEvent &sel = (HKWidgetSelectEvent&)ev;
	if(sel.selection > -1)
		SetSelection(sel.selection);

	// hide listbox
	GetUI().SetFocus(pExclusiveSource, NULL);
	GetUI().RemoveTopLevelWidget(pListbox);

	pExclusiveSource = NULL;
}
