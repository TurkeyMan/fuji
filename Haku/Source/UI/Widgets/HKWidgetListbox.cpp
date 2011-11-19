#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetListbox.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"

const EnumKeypair HKWidgetListbox::sOrientationKeys[] =
{
	{ "Horizontal", Horizontal },
	{ "Vertical", Vertical },
	{ NULL, 0 }
};

HKWidget *HKWidgetListbox::Create(HKWidgetType *pType)
{
	return new HKWidgetListbox(pType);
}


HKWidgetListbox::HKWidgetListbox(HKWidgetType *pType)
: HKWidgetLayout(pType)
{
	orientation = Vertical;

	pAdapter = NULL;
	pOldFocus = NULL;

	contentSize = 0.f;
	prevScrollOffset = scrollOffset = 0.f;
	velocity = 0.f;

	flags = 0;

	bDragging = false;

	bDragable = true;
	bClickable = true;

	padding = MakeVector(2,2,2,2);
}

HKWidgetListbox::~HKWidgetListbox()
{
	Unbind();
}

void HKWidgetListbox::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		SetOrientation((Orientation)HKWidget_GetEnumValue(pValue, sOrientationKeys));
	else if(!MFString_CaseCmp(pProperty, "hoverSelect"))
		flags |= HKWidget_GetBoolFromString(pValue) ? HoverSelect : 0;
	else if(!MFString_CaseCmp(pProperty, "onSelChanged"))
		HKWidget_BindWidgetEvent(OnSelChanged, pValue);
	HKWidgetLayout::SetProperty(pProperty, pValue);
}

MFString HKWidgetListbox::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		return HKWidget_GetEnumFromValue(GetOrientation(), sOrientationKeys);
	return HKWidgetLayout::GetProperty(pProperty);
}

void HKWidgetListbox::Update()
{
	if(!bDragging)
	{
		if(velocity != 0.f)
		{
			// apply scroll velocity
			velocity *= 1.f - MFSystem_TimeDelta()*10.f;
			if(velocity < 0.01f)
				velocity = 0.f;

			scrollOffset += velocity * MFSystem_TimeDelta();
		}

		if(scrollOffset > 0.f)
		{
			scrollOffset = MFMax(scrollOffset - MFMax(scrollOffset * 10.f * MFSystem_TimeDelta(), 1.f), 0.f);
		}
		else
		{
			float listSize = orientation == Horizontal ? size.x - (padding.x + padding.z) : size.y - (padding.y + padding.w);
			float overflow = MFMin(listSize - (contentSize + scrollOffset), -scrollOffset);
			if(overflow > 0.f)
			{
				scrollOffset = MFMin(scrollOffset + MFMax(overflow * 10.f * MFSystem_TimeDelta(), 1.f), scrollOffset + overflow);
			}
		}
	}

	scrollOffset = MFFloor(scrollOffset);
	if(scrollOffset != prevScrollOffset)
	{
		prevScrollOffset = scrollOffset;
		ArrangeChildren();
	}
}

bool HKWidgetListbox::InputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			// immediately stop the thing from scrolling
			velocity = 0.f;
			scrollOffset = MFFloor(scrollOffset);

			// if the down stroke is outside the listbox, we have triggered a non-click
			MFRect rect = { 0.f, 0.f, size.x, size.y };
			if(!MFTypes_PointInRect(ev.down.x, ev.down.y, &rect))
			{
				HKWidgetSelectEvent sel(this, -1);
				OnClicked(*this, sel);
			}
			break;
		}
		case HKInputManager::IE_Up:
		{
			if(bDragging)
			{
				bDragging = false;
				GetUI().SetFocus(ev.pSource, pOldFocus);
			}
			break;
		}
		case HKInputManager::IE_Drag:
		{
			// scroll the contents
			float delta = orientation == Horizontal ? ev.drag.deltaX : ev.drag.deltaY;
			scrollOffset += delta;

			const float smooth = 0.5f;
			velocity = velocity*smooth + (delta / MFSystem_TimeDelta())*(1.f-smooth);

			if(!bDragging)
			{
				bDragging = true;
				pOldFocus = GetUI().SetFocus(ev.pSource, this);
			}
			break;
		}
		default:
			break;
	}

	return HKWidget::InputEvent(manager, ev);
}

void HKWidgetListbox::ArrangeChildren()
{
	// early out?
	int numChildren = GetNumChildren();
	if(numChildren == 0)
		return;

	MFVector pPos = orientation == Horizontal ? MakeVector(padding.x + scrollOffset, padding.y) : MakeVector(padding.x, padding.y + scrollOffset);
	MFVector pSize = MakeVector(size.x - (padding.x + padding.z), size.y - (padding.y + padding.w));

	contentSize = 0.f;

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		if(pWidget->GetVisible() == Gone)
			continue;

		const MFVector &cMargin = pWidget->GetLayoutMargin();
		const MFVector &cSize = pWidget->GetSize();

		MFVector tPos = pPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = MFMax(pSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w), MFVector::zero);

		if(orientation == Horizontal)
		{
			float itemSize = cSize.x + cMargin.x + cMargin.z;
			contentSize += itemSize;
			pPos.x += itemSize;
			pWidget->SetPosition(tPos);
			pWidget->SetHeight(tSize.y);
		}
		else
		{
			float itemSize = cSize.y + cMargin.y + cMargin.w;
			contentSize += itemSize;
			pPos.y += itemSize;
			pWidget->SetPosition(tPos);
			pWidget->SetWidth(tSize.x);
		}
	}
}

float HKWidgetListbox::GetMaxSize() const
{
	if(orientation == Horizontal)
		return contentSize + padding.x + padding.z;
	else
		return contentSize + padding.y + padding.w;
}

void HKWidgetListbox::SetSelection(int item)
{
	if(selection != item)
	{
		if(selection > -1)
			children[selection].pChild->GetRenderer()->SetProperty("background_colour", "0,0,0,0");
		if(item > -1)
			children[item].pChild->GetRenderer()->SetProperty("background_colour", "0,0,1,0.6f");

		selection = item;

		HKWidgetSelectEvent ev(this, item);
		OnSelChanged(*this, ev);
	}
}

void HKWidgetListbox::AddView(HKWidget *pView)
{
	// it might be better to write a custom ListItem widget here, Frame might be a bit heavy for the purpose...
	HKWidgetLayoutFrame *pFrame = GetUI().CreateWidget<HKWidgetLayoutFrame>();
	pFrame->AddChild(pView, true);
	pFrame->SetClickable(true);
	pFrame->SetHoverable(true);

	// make child clickable
	pFrame->OnDown += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnItemDown);
	pFrame->OnTap += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnItemClick);
	pFrame->OnHoverOver += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnItemOver);
	pFrame->OnHoverOut += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnItemOut);

	AddChild(pFrame, true);
}

void HKWidgetListbox::Bind(HKListAdapter &adapter)
{
	Unbind();

	pAdapter = &adapter;

	// subscribe for list adapter events
	adapter.onInsertItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnInsert);
	adapter.onRemoveItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnRemove);
	adapter.onTouchItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnChange);

	// populate the children with each item
	int numItems = adapter.GetNumItems();
	for(int a=0; a<numItems; ++a)
	{
		HKWidget *pItem = adapter.GetItemView(a);
		AddView(pItem);
	}
}

void HKWidgetListbox::Unbind()
{
	if(pAdapter)
	{
		// unsubscribe from adapter
		pAdapter->onInsertItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnInsert);
		pAdapter->onRemoveItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnRemove);
		pAdapter->onTouchItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnChange);
		pAdapter = NULL;
	}

	selection = -1;
	scrollOffset = 0.f;

	ClearChildren();
}

void HKWidgetListbox::OnInsert(int position, HKListAdapter &adapter)
{
	SetSelection(-1);

	HKWidget *pView = adapter.GetItemView(position);
	AddView(pView);
}

void HKWidgetListbox::OnRemove(int position, HKListAdapter &adapter)
{
	SetSelection(-1);

	RemoveChild(position);
}

void HKWidgetListbox::OnChange(int position, HKListAdapter &adapter)
{
	adapter.UpdateItemView(position, GetItemView(position));
}

void HKWidgetListbox::OnItemDown(HKWidget &widget, const HKWidgetEventInfo &ev)
{
	// a down stroke should immediately stop any innertial scrolling
	velocity = 0.f;
	scrollOffset = MFFloor(scrollOffset);

	if(!(flags & HoverSelect))
		SetSelection(GetChildIndex(&widget));
}

void HKWidgetListbox::OnItemClick(HKWidget &widget, const HKWidgetEventInfo &ev)
{
	HKWidgetSelectEvent sel(ev.pSender, GetChildIndex(&widget));
	OnClicked(*this, sel);
}

void HKWidgetListbox::OnItemOver(HKWidget &widget, const HKWidgetEventInfo &ev)
{
	if(flags & HoverSelect)
		SetSelection(GetChildIndex(&widget));
}

void HKWidgetListbox::OnItemOut(HKWidget &widget, const HKWidgetEventInfo &ev)
{
	if(flags & HoverSelect)
		SetSelection(-1);
}

int HKWidgetListbox::GetChildIndex(HKWidget *pWidget)
{
	for(int a=0; a<children.size(); ++a)
	{
		if(children[a].pChild == pWidget)
			return a;
	}
	return -1;
}
