#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetListbox.h"

const EnumKeypair HKWidgetListbox::sOrientationKeys[] =
{
	{ "Horizontal", Horizontal },
	{ "Vertical", Vertical },
	{ NULL, 0 }
};

HKWidget *HKWidgetListbox::Create()
{
	return new HKWidgetListbox();
}


HKWidgetListbox::HKWidgetListbox()
{
	pTypeName = "HKWidgetListbox";

	orientation = Horizontal;

	pAdapter = NULL;

	contentSize = 0.f;
	prevScrollOffset = scrollOffset = 0.f;
	velocity = 0.f;

	bDragable = true;
	bClickable = true;

	bDragging = false;
}

HKWidgetListbox::~HKWidgetListbox()
{
	Unbind();
}

void HKWidgetListbox::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "orientation"))
		SetOrientation((Orientation)HKWidget_GetEnumValue(pValue, sOrientationKeys));
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
	if(!bDragging && velocity != 0.f)
	{
		// apply scroll velocity
		velocity *= 1.f - MFSystem_TimeDelta();
		if(velocity < 0.01f)
		{
			velocity = 0.f;
			scrollOffset = MFFloor(scrollOffset);
		}

		scrollOffset += velocity;

		MFClamp(0.f, scrollOffset, contentSize);
	}

	if(scrollOffset != prevScrollOffset)
	{
		prevScrollOffset = scrollOffset;
		ArrangeChildren();
	}
}

bool HKWidgetListbox::InputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
{
	// try and handle the input event in some standard ways...
	switch(ev.ev)
	{
		case HKInputManager::IE_Down:
		{
			// immediately stop the thing from scrolling
			velocity = 0.f;
			scrollOffset = MFFloor(scrollOffset);
			break;
		}
		case HKInputManager::IE_Up:
		{
			bDragging = false;
			break;
		}
		case HKInputManager::IE_Drag:
		{
			// scroll the contents
			float delta = orientation == Horizontal ? ev.drag.deltaX : ev.drag.deltaY;
			scrollOffset += delta;

			velocity = velocity*.3f + (delta / MFSystem_TimeDelta())*.7f;

			bDragging = true;
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

	for(int a=0; a<numChildren; ++a)
	{
		HKWidget *pWidget = GetChild(a);
		if(pWidget->GetVisible() == Gone)
			continue;

		const MFVector &cMargin = pWidget->GetLayoutMargin();
		const MFVector &cSize = pWidget->GetSize();

		MFVector tPos = pPos + MakeVector(cMargin.x, cMargin.y);
		MFVector tSize = MFMax(pSize - MakeVector(cMargin.x + cMargin.z, cMargin.y + cMargin.w), MFVector::zero);

		MFVector newSize = cSize;

		if(orientation == Horizontal)
		{
			pPos.x += cSize.x + cMargin.x + cMargin.z;
			pWidget->SetPosition(tPos);
			newSize.y = tSize.y;
		}
		else
		{
			pPos.y += cSize.y + cMargin.y + cMargin.w;
			pWidget->SetPosition(tPos);
			newSize.x = tSize.x;
		}

		ResizeChild(pWidget, newSize);
	}
}

void HKWidgetListbox::Bind(HKListAdapter &adapter)
{
	pAdapter = &adapter;
	adapter.onInsertItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnInsert);
	adapter.onRemoveItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnRemove);
	adapter.onTouchItem += fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnChange);

	// populate the children with each item
	int numItems = adapter.GetNumItems();
	for(int a=0; a<numItems; ++a)
	{
		HKWidget *pItem = adapter.GetItemView(a);
		AddChild(pItem, true);

		// make child clickable
		//...
		// subscribe to childs onDown and onClick
		//... prodice item-click events of some sort
	}

	scrollOffset = 0.f;
}

void HKWidgetListbox::Unbind()
{
	if(pAdapter)
	{
		pAdapter->onInsertItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnInsert);
		pAdapter->onRemoveItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnRemove);
		pAdapter->onTouchItem -= fastdelegate::MakeDelegate(this, &HKWidgetListbox::OnChange);
		pAdapter = NULL;
	}

	// unsubscribe from child click events
	//...

	ClearChildren();
}

void HKWidgetListbox::OnInsert(int position, HKListAdapter &adapter)
{
	// insert...
}

void HKWidgetListbox::OnRemove(int position, HKListAdapter &adapter)
{
	// remove...
}

void HKWidgetListbox::OnChange(int position, HKListAdapter &adapter)
{
	adapter.UpdateItemView(position, children[position].pChild);
}
