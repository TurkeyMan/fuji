#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetButton.h"

const EnumKeypair HKWidgetButton::sButtonFlagKeys[] =
{
	{ "TriggerOnDown", BF_TriggerOnDown },
	{ "StateButton", BF_StateButton },
	{ NULL, 0 }
};

HKWidget *HKWidgetButton::Create()
{
	return new HKWidgetButton();
}


HKWidgetButton::HKWidgetButton()
{
	pTypeName = "HKWidgetButton";

	buttonFlags = 0;

	bClickable = true;

	bDown = false;
	bPressed = false;
	bState = false;

	textJustification = MFFontJustify_Center;

	// hook up some events
	OnDown += fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonDown);
	OnUp += fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonUp);
	OnHover += fastdelegate::MakeDelegate(this, &HKWidgetButton::Hover);
}

HKWidgetButton::~HKWidgetButton()
{
	// unhook the events
	OnDown -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonDown);
	OnUp -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonUp);
	OnHover -= fastdelegate::MakeDelegate(this, &HKWidgetButton::Hover);
}

void HKWidgetButton::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "button_state"))
		SetState(HKWidget_GetBoolFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "button_flags"))
		SetButtonFlags(HKWidget_GetBitfieldValue(pValue, sButtonFlagKeys));
	else if(!MFString_CaseCmp(pProperty, "onClicked"))
		HKWidget_BindWidgetEvent(OnClicked, pValue);
	else
		HKWidgetLabel::SetProperty(pProperty, pValue);
}

MFString HKWidgetButton::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "button_flags"))
		return HKWidget_GetBitfieldFromValue(buttonFlags, sButtonFlagKeys);
	return HKWidgetLabel::GetProperty(pProperty);
}

void HKWidgetButton::ButtonDown(HKWidget &sender, HKWidgetEventInfo &ev)
{
	HKWidgetInputEvent down = (HKWidgetInputEvent&)ev;

	if(buttonFlags & BF_TriggerOnDown)
	{
		if(buttonFlags & BF_StateButton)
			bState = !bState;

		HKWidgetInputEvent clickEvent(this, down.pSource);
		OnClicked(*this, clickEvent);
	}
	else
	{
		bDown = true;
		bPressed = true;

		GetUI().SetFocus(down.pSource, this);
	}
}

void HKWidgetButton::ButtonUp(HKWidget &sender, HKWidgetEventInfo &ev)
{
	HKWidgetInputEvent &up = (HKWidgetInputEvent&)ev;

	bDown = false;

	HKUserInterface::Get().SetFocus(up.pSource, NULL);

	if(bPressed)
	{
		bPressed = false;

		if(buttonFlags & BF_StateButton)
			bState = !bState;

		HKWidgetInputEvent clickEvent(this, up.pSource);
		OnClicked(*this, clickEvent);
	}
}

void HKWidgetButton::Hover(HKWidget &sender, HKWidgetEventInfo &ev)
{
	HKWidgetMoveEvent &hover = (HKWidgetMoveEvent&)ev;

	if(bDown)
	{
		MFRect rect = { 0, 0, size.x, size.y };
		if(MFTypes_PointInRect(hover.newPos.x, hover.newPos.y, &rect))
			bPressed = true;
		else
			bPressed = false;
	}
}


HKWidgetRenderer *HKWidgetRendererButton::Create()
{
	return new HKWidgetRendererButton();
}

#include "MFPrimitive.h"
#include "MFFont.h"

void HKWidgetRendererButton::Render(const HKWidget &widget, const MFMatrix &worldTransform)
{
	HKWidgetRenderer::Render(widget, worldTransform);

	HKWidgetButton &button = (HKWidgetButton&)widget;

	const MFVector &size = widget.GetSize();

	if(pBackground)
	{
	}
	else
	{
		const MFVector &colour = widget.GetColour();
		if(colour.w > 0.f)
		{

			MFPrimitive_DrawUntexturedQuad(0, 0, size.x, size.y, MFVector::black, worldTransform);
			MFPrimitive_DrawUntexturedQuad(1, 1, size.x - 2, size.y - 2, button.GetPressed() ? colour * MFVector::grey : colour, worldTransform);
		}
	}

	MFString label = button.GetText();
	if(!label.IsEmpty())
	{
		MFFont *pFont = button.GetFont();
		float height = button.GetTextHeight();
		float shadowDepth = button.GetShadowDepth();
		MFFontJustify j = button.GetTextJustification();
		const MFVector &textColour = button.GetTextColour();
		const char *pString = label.CStr();

		if(shadowDepth > 0.f)
			MFFont_DrawTextJustified(pFont, pString, MakeVector(shadowDepth, shadowDepth), size.x, size.y, j, height, MFVector::black, -1, worldTransform);
		MFFont_DrawTextJustified(pFont, pString, MFVector::zero, size.x, size.y, j, height, button.GetPressed() ? textColour * MFVector::grey : textColour, -1, worldTransform);
	}
}
