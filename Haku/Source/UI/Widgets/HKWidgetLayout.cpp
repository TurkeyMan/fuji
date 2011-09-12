#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLayout.h"


HKWidgetLayout::HKWidgetLayout()
{
	pTypeName = "HKWidgetLayout";

	OnResize += fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);

	margin = MFVector::zero;
	fitFlags = 0;
}

HKWidgetLayout::~HKWidgetLayout()
{
	OnResize -= fastdelegate::MakeDelegate(this, &HKWidgetLayout::OnLayoutDirty);
}

int HKWidgetLayout::AddChild(HKWidget *pChild, Justification justification, float weight, const MFVector &margin)
{
	int id = children.size();

	Child &c = children.push();
	c.pChild = pChild;
	c.weight = weight;
	c.margin = margin;
	c.justification = justification;

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

void HKWidgetLayout::SetMargin(const MFVector &margin)
{
	if(this->margin != margin)
	{
		this->margin = margin;
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

void HKWidgetLayout::SetChildWeight(int index, float weight)
{
	if(children[index].weight != weight)
	{
		children[index].weight = weight;
		ArrangeChildren();
	}
}

void HKWidgetLayout::SetChildMargin(int index, const MFVector &margin)
{
	if(children[index].margin != margin)
	{
		children[index].margin = margin;
		ArrangeChildren();
	}
}

void HKWidgetLayout::SetChildJustification(int index, Justification justification)
{
	if(children[index].justification != justification)
	{
		children[index].justification = justification;
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
	const MFVector &margin = layout.GetMargin();

	MFPrimitive_DrawUntexturedQuad(0, 0, size.x, size.y, MFVector::black, worldTransform);
	MFPrimitive_DrawUntexturedQuad(1, 1, size.x - 2, size.y - 2, MFVector::blue, worldTransform);
	MFPrimitive_DrawUntexturedQuad(margin.x, margin.y, size.x - margin.x - margin.z, size.y - margin.y - margin.w, colour, worldTransform);
}
