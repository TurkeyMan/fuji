#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetButton.h"

const EnumKeypair HKWidgetButton::sButtonFlagKeys[] =
{
	{ "TriggerOnDown", BF_TriggerOnDown },
	{ "StateButton", BF_StateButton }
};

HKWidget *HKWidgetButton::Create()
{
	return new HKWidgetButton();
}


HKWidgetButton::HKWidgetButton()
{
	pTypeName = "HKWidgetButton";

	buttonFlags = 0;

	bDown = false;
	bPressed = false;
	bState = false;

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

void HKWidgetButton::SetPropertyB(const char *pProperty, bool bValue)
{
	if(!MFString_CaseCmp(pProperty, "button_state"))
		SetState(bValue);
	else
		HKWidget::SetPropertyB(pProperty, bValue);
}

void HKWidgetButton::SetPropertyI(const char *pProperty, int value)
{
	if(!MFString_CaseCmp(pProperty, "button_flags"))
		SetButtonFlags(value);
	else
		HKWidget::SetPropertyI(pProperty, value);
}

void HKWidgetButton::SetPropertyS(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "button_state"))
		SetState(HKWidget_GetBoolFromString(pValue));
	else if(!MFString_CaseCmp(pProperty, "text"))
		label = pValue;
	else if(!MFString_CaseCmp(pProperty, "button_flags"))
		SetButtonFlags(HKWidget_GetBitfieldValue(pValue, sButtonFlagKeys));
	else
		HKWidget::SetPropertyS(pProperty, pValue);
}

bool HKWidgetButton::GetPropertyB(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "button_pressed"))
		return GetPressed();
	else if(!MFString_CaseCmp(pProperty, "button_state"))
		return GetState();
	return HKWidget::GetPropertyB(pProperty);
}

int HKWidgetButton::GetPropertyI(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "button_flags"))
		return (int)buttonFlags;
	return HKWidget::GetPropertyI(pProperty);
}

MFString HKWidgetButton::GetPropertyS(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "text"))
		return name;
	else if(!MFString_CaseCmp(pProperty, "button_flags"))
		return HKWidget_GetBitfieldFromValue(buttonFlags, sButtonFlagKeys);
	return HKWidget::GetPropertyS(pProperty);
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
	HKWidgetButton &button = (HKWidgetButton&)widget;

	const MFVector &size = widget.GetSize();
	const MFVector &colour = widget.GetColour();
	MFString label = button.GetLabel();

	MFPrimitive_DrawUntexturedQuad(0, 0, size.x, size.y, MFVector::black, worldTransform);
	MFPrimitive_DrawUntexturedQuad(1, 1, size.x - 2, size.y - 2, button.GetPressed() ? MFVector::blue : colour, worldTransform);
	if(!label.IsEmpty())
		MFFont_DrawTextJustified(MFFont_GetDebugFont(), label.CStr(), MFVector::zero, size.x, size.y, MFFontJustify_Center, 10.f, MFVector::black, -1, worldTransform);
}
