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

	bAutoHeight = true;
	textHeight = MFFont_GetFontHeight(pFont);
}

HKWidgetLabel::~HKWidgetLabel()
{
}

void HKWidgetLabel::SetPropertyF(const char *pProperty, float value)
{
	if(!MFString_CaseCmp(pProperty, "text_height"))
		SetTextHeight(value);
	else
		HKWidget::SetPropertyF(pProperty, value);
}

void HKWidgetLabel::SetPropertyS(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "text_height"))
		SetTextHeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "text"))
		SetLabel(pValue);
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
		pFont = MFFont_Create(pValue);
	}
	else if(!MFString_CaseCmp(pProperty, "text_justification"))
		SetTextJustification((MFFontJustify)HKWidget_GetEnumValue(pValue, sJustifyKeys));
	else
		HKWidget::SetPropertyS(pProperty, pValue);
}

float HKWidgetLabel::GetPropertyF(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text_height"))
		return GetTextHeight();
	return HKWidget::GetPropertyF(pProperty);
}

int HKWidgetLabel::GetPropertyI(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text_justification"))
		return (int)justification;
	return HKWidget::GetPropertyI(pProperty);
}

MFString HKWidgetLabel::GetPropertyS(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		return GetLabel();
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
//		return MFFont_GetFontName(pFont);
		return NULL;
	}
	else if(!MFString_CaseCmp(pProperty, "text_justification"))
		return HKWidget_GetEnumFromValue(GetTextJustification(), sJustifyKeys);
	return HKWidget::GetPropertyS(pProperty);
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
