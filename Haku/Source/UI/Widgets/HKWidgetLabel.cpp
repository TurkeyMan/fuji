#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetLabel.h"

#include "MFFont.h"

HKWidget *HKWidgetLabel::Create()
{
	return new HKWidgetLabel();
}


HKWidgetLabel::HKWidgetLabel()
{
	pTypeName = "HKWidgetLabel";

	colour = MFVector::black;

	pFont = MFFont_GetDebugFont();
	justification = MFFontJustify_Top_Left;

	textHeight = MFFont_GetFontHeight(pFont);
}

HKWidgetLabel::~HKWidgetLabel()
{
}

void HKWidgetLabel::SetLabel(MFString label)
{
	this->label = label;

	if(!label.IsEmpty())
	{
		// resize the widget accordingly
		float height, width = MFFont_GetStringWidth(pFont, label.CStr(), textHeight, 0.f, -1, &height);
		SetSize(MakeVector(width, height));
	}
}


HKWidgetRenderer *HKWidgetRendererLabel::Create()
{
	return new HKWidgetRendererLabel();
}

void HKWidgetRendererLabel::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetLabel &label = (HKWidgetLabel&)widget;

	const MFVector &size = widget.GetSize();
	const MFVector &colour = widget.GetColour();
	MFString l = label.GetLabel();

	if(!l.IsEmpty())
		MFFont_DrawTextJustified(label.GetFont(), l.CStr(), MFVector::zero, size.x, size.y, label.GetTextJustification(), label.GetTextHeight(), colour, -1, worldTransform);
}
