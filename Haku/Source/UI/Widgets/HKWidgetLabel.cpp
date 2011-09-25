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

	textColour = MFVector::black;

	pFont = MFFont_GetDebugFont();
	bOwnFont = false;

	textJustification = MFFontJustify_Top_Left;

	bAutoTextHeight = true;
	textHeight = MFFont_GetFontHeight(pFont);
}

HKWidgetLabel::~HKWidgetLabel()
{
	if(bOwnFont)
		MFFont_Destroy(pFont);
}

void HKWidgetLabel::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		SetText(pValue);
	else if(!MFString_CaseCmp(pProperty, "text_colour"))
		SetTextColour(HKWidget_GetColourFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "text_height"))
		SetTextHeight(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "text_shadowDepth"))
		SetShadowDepth(MFString_AsciiToFloat(pValue));
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
		if(bOwnFont)
			MFFont_Destroy(pFont);
		pFont = MFFont_Create(pValue);
		bOwnFont = true;
	}
	else if(!MFString_CaseCmp(pProperty, "text_align"))
		SetTextJustification((MFFontJustify)HKWidget_GetEnumValue(pValue, sJustifyKeys));
	else
		HKWidget::SetProperty(pProperty, pValue);
}

MFString HKWidgetLabel::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		return GetText();
	else if(!MFString_CaseCmp(pProperty, "text_font"))
	{
		if(bOwnFont)
			return font;
		else
//			return MFFont_GetFontName(pFont);
			return NULL;
	}
	else if(!MFString_CaseCmp(pProperty, "text_align"))
		return HKWidget_GetEnumFromValue(GetTextJustification(), sJustifyKeys);
	return HKWidget::GetProperty(pProperty);
}

void HKWidgetLabel::SetText(MFString text)
{
	this->text = text;

	if(!text.IsEmpty())
	{
		// resize the widget accordingly
		float height, width = MFFont_GetStringWidth(pFont, text.CStr(), textHeight, 0.f, -1, &height);
		SetSize(MakeVector(width, height));
	}
}


HKWidgetRenderer *HKWidgetRendererLabel::Create()
{
	return new HKWidgetRendererLabel();
}

void HKWidgetRendererLabel::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetRenderer::Render(widget, worldTransform);

	HKWidgetLabel &label = (HKWidgetLabel&)widget;

	MFString l = label.GetText();
	if(!l.IsEmpty())
	{
		MFFont *pFont = label.GetFont();
		float height = label.GetTextHeight();
		float shadowDepth = label.GetShadowDepth();
		const MFVector &size = widget.GetSize();
		const MFVector &textColour = label.GetTextColour();
		MFFontJustify j = label.GetTextJustification();
		const char *pString = l.CStr();

		if(shadowDepth > 0.f)
			MFFont_DrawTextJustified(pFont, pString, MakeVector(shadowDepth, shadowDepth), size.x, size.y, j, height, MFVector::black, -1, worldTransform);
		MFFont_DrawTextJustified(pFont, pString, MFVector::zero, size.x, size.y, j, height, textColour, -1, worldTransform);
	}
}
