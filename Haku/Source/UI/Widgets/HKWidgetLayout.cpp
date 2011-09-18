#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayout.h"

const EnumKeypair HKWidgetLayout::sFirFlagsKeys[] =
{
	{ "FitContentVertical", FitContentVertical },
	{ "FitContentHorizontal", FitContentHorizontal }
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
	pChild->OnResize += fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);

	ArrangeChildren();

	return id;
}

void HKWidgetLayout::RemoveChild(int index)
{
	MFDebug_Assert(false, "Need to remove child from the list");

//	children[index].pChild->OnResize -= fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnChildResize);
//	ArrangeChildren()
}

int HKWidgetLayout::GetNumChildren() const
{
	return children.size();
}

HKWidget *HKWidgetLayout::GetChild(int index) const
{
	return children[index].pChild;
}

void HKWidgetLayout::SetPropertyV(const char *pProperty, const MFVector& value)
{
	if(!MFString_CaseCmp(pProperty, "layout_padding"))
		SetPadding(value);
	else
		HKWidget::SetPropertyV(pProperty, value);
}

void HKWidgetLayout::SetPropertyS(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "layout_padding"))
		SetPadding(HKWidget_GetVectorFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "layout_flags"))
		SetFitFlags(HKWidget_GetBitfieldValue(pValue, sFirFlagsKeys));
	else
		HKWidget::SetPropertyS(pProperty, pValue);
}

const MFVector &HKWidgetLayout::GetPropertyV(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "layout_padding"))
		return GetPadding();
	return HKWidget::GetPropertyV(pProperty);
}

MFString HKWidgetLayout::GetPropertyS(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "layout_flags"))
		HKWidget_GetBitfieldFromValue(fitFlags, sFirFlagsKeys);
	return HKWidget::GetPropertyS(pProperty);
}

void HKWidgetLayout::SetPadding(const MFVector &padding)
{
	if(this->padding != padding)
	{
		this->padding = padding;
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


HKWidgetRenderer *HKWidgetRendererLayout::Create()
{
	return new HKWidgetRendererLayout();
}

#include "MFPrimitive.h"

void HKWidgetRendererLayout::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetLayout &layout = (HKWidgetLayout&)widget;

	const MFVector &size = widget.GetSize();
	const MFVector &colour = widget.GetColour();
	const MFVector &margin = layout.GetPadding();

	MFPrimitive_DrawUntexturedQuad(0, 0, size.x, size.y, MFVector::black, worldTransform);
	MFPrimitive_DrawUntexturedQuad(1, 1, size.x - 2, size.y - 2, MFVector::blue, worldTransform);
	MFPrimitive_DrawUntexturedQuad(margin.x, margin.y, size.x - margin.x - margin.z, size.y - margin.y - margin.w, colour, worldTransform);
}
