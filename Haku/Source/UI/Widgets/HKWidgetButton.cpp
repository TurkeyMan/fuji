#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetButton.h"

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
	OnHoverOver += fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonOver);
	OnHoverOut += fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonOut);
}

HKWidgetButton::~HKWidgetButton()
{
	// unhook the events
	OnDown -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonDown);
	OnUp -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonUp);
	OnHoverOver -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonOver);
	OnHoverOut -= fastdelegate::MakeDelegate(this, &HKWidgetButton::ButtonOut);
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

void HKWidgetButton::ButtonOver(HKWidget &sender, HKWidgetEventInfo &ev)
{
	if(bDown)
		bPressed = true;
}

void HKWidgetButton::ButtonOut(HKWidget &sender, HKWidgetEventInfo &ev)
{
	if(bDown)
		bPressed = false;
}
