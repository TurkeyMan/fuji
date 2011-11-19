#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetButton.h"

const EnumKeypair HKWidgetButton::sButtonFlagKeys[] =
{
	{ "TriggerOnDown", BF_TriggerOnDown },
	{ "StateButton", BF_StateButton },
	{ NULL, 0 }
};

HKWidget *HKWidgetButton::Create(HKWidgetType *pType)
{
	return new HKWidgetButton(pType);
}


HKWidgetButton::HKWidgetButton(HKWidgetType *pType)
: HKWidgetLabel(pType)
{
	buttonFlags = 0;

	bClickable = true;
	bHoverable = true;

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
	if(!MFString_CaseCmp(pProperty, "style_pressed"))
		SetStylePressed(pValue);
	else if(!MFString_CaseCmp(pProperty, "style_selected"))
		SetStyleState(pValue);
	else if(!MFString_CaseCmp(pProperty, "button_state"))
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

void HKWidgetButton::SetPressed(bool bPressed)
{
	if(bPressed != this->bPressed)
	{
		this->bPressed = bPressed;
		UpdateStyle();
	}
}

void HKWidgetButton::SetButtonState(bool bState)
{
	if(bState != this->bState)
	{
		this->bState = bState;
		UpdateStyle();
	}
}

void HKWidgetButton::UpdateStyle()
{
	if(!bEnabled && !styleDisabled.IsEmpty())
		ApplyStyle(styleDisabled);
	else if(bPressed && !stylePressed.IsEmpty())
		ApplyStyle(stylePressed);
	else if(bState && !styleOnState.IsEmpty())
		ApplyStyle(styleOnState);
	else if(!style.IsEmpty())
		ApplyStyle(style);
}

void HKWidgetButton::ButtonDown(HKWidget &sender, const HKWidgetEventInfo &ev)
{
	if(!bEnabled)
		return;

	HKWidgetInputEvent down = (HKWidgetInputEvent&)ev;

	if(buttonFlags & BF_TriggerOnDown)
	{
		if(buttonFlags & BF_StateButton)
			SetButtonState(!bState);

		HKWidgetInputEvent clickEvent(this, down.pSource);
		OnClicked(*this, clickEvent);
	}
	else
	{
		bDown = true;
		SetPressed(true);

		GetUI().SetFocus(down.pSource, this);
	}
}

void HKWidgetButton::ButtonUp(HKWidget &sender, const HKWidgetEventInfo &ev)
{
	if(!bEnabled)
		return;

	HKWidgetInputEvent &up = (HKWidgetInputEvent&)ev;

	bDown = false;

	GetUI().SetFocus(up.pSource, NULL);

	if(bPressed)
	{
		SetPressed(false);

		if(buttonFlags & BF_StateButton)
			SetButtonState(!bState);

		HKWidgetInputEvent clickEvent(this, up.pSource);
		OnClicked(*this, clickEvent);
	}
}

void HKWidgetButton::Hover(HKWidget &sender, const HKWidgetEventInfo &ev)
{
	if(!bEnabled)
		return;

	HKWidgetMoveEvent &hover = (HKWidgetMoveEvent&)ev;

	if(bDown)
	{
		MFRect rect = { 0, 0, size.x, size.y };
		if(MFTypes_PointInRect(hover.newPos.x, hover.newPos.y, &rect))
			SetPressed(true);
		else
			SetPressed(false);
	}
}
