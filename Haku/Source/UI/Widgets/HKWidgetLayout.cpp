#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayout.h"

const EnumKeypair HKWidgetLayout::sFirFlagsKeys[] =
{
	{ "FitContent", FitContentVertical|FitContentHorizontal },
	{ "FitContentVertical", FitContentVertical },
	{ "FitContentHorizontal", FitContentHorizontal },
	{ NULL, 0 }
};

HKWidgetLayout::HKWidgetLayout()
{
	pTypeName = "HKWidgetLayout";

	OnResize += fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);

	padding = MFVector::zero;
	fitFlags = 0;
}

HKWidgetLayout::~HKWidgetLayout()
{
	OnResize -= fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);

	for(int a=0; a<children.size(); ++a)
	{
		if(children[a].bOwnChild)
			delete children[a].pChild;
	}
}

int HKWidgetLayout::AddChild(HKWidget *pChild, bool bOwnChild)
{
	int id = children.size();

	Child child = { pChild, bOwnChild };
	children.push(child);

	pChild->pParent = this;
	pChild->OnLayoutChanged += fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);

	ArrangeChildren();

	return id;
}

void HKWidgetLayout::RemoveChild(int index)
{
	MFDebug_Assert(false, "Need to remove child from the list");

//	children[index].pChild->OnLayoutChanged -= fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);
//	ArrangeChildren()
}

void HKWidgetLayout::ClearChildren()
{
	for(int a=0; a<children.size(); ++a)
	{
		if(children[a].bOwnChild)
			delete children[a].pChild;
	}

	children.clear();
}

int HKWidgetLayout::GetNumChildren() const
{
	return children.size();
}

HKWidget *HKWidgetLayout::GetChild(int index) const
{
	return children[index].pChild;
}

void HKWidgetLayout::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "padding"))
		SetPadding(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "layout_flags"))
		SetFitFlags(HKWidget_GetBitfieldValue(pValue, sFirFlagsKeys));
	else
		HKWidget::SetProperty(pProperty, pValue);
}

MFString HKWidgetLayout::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "layout_flags"))
		return HKWidget_GetBitfieldFromValue(fitFlags, sFirFlagsKeys);
	return HKWidget::GetProperty(pProperty);
}

void HKWidgetLayout::SetPadding(const MFVector &padding)
{
	if(this->padding != padding)
	{
		this->padding = padding;

		// potentially 
		MFVector newSize = MFMax(size, MakeVector(padding.x + padding.z, padding.y + padding.w));
		if(newSize != size)
			Resize(newSize);
		else
			ArrangeChildren();
	}
}

void HKWidgetLayout::SetFitFlags(uint32 fitFlags)
{
	if(this->fitFlags != fitFlags)
	{
		this->fitFlags = fitFlags;
		ArrangeChildren();
	}
}

void HKWidgetLayout::OnLayoutDirty(HKWidget &child, HKWidgetEventInfo &ev)
{
	// we may need to rearrange the children
	ArrangeChildren();
}
