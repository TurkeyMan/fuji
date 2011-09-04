#include "Haku.h"
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

void HKWidgetButton::ButtonDown(HKWidget &sender, HKWidgetEventInfo *pEvent)
{
	HKWidgetInputEvent *pDown = (HKWidgetInputEvent *)pEvent;

	if(buttonFlags & BF_TriggerOnDown)
	{
		if(buttonFlags & BF_StateButton)
			bState = !bState;

		if(!OnClicked.IsEmpty())
		{
			HKWidgetInputEvent *pClickEvent = (HKWidgetInputEvent*)HKWidgetEventInfo::Alloc(this);
			pClickEvent->pSource = pDown->pSource;
			OnClicked(*this, pClickEvent);
			HKWidgetEventInfo::Free(pClickEvent);
		}
	}
	else
	{
		bDown = true;
		bPressed = true;
	}
}

void HKWidgetButton::ButtonUp(HKWidget &sender, HKWidgetEventInfo *pEvent)
{
	HKWidgetInputEvent *pUp = (HKWidgetInputEvent *)pEvent;

	bDown = false;

	if(bPressed)
	{
		bPressed = false;

		if(buttonFlags & BF_StateButton)
			bState = !bState;

		if(!OnClicked.IsEmpty())
		{
			HKWidgetInputEvent *pClickEvent = (HKWidgetInputEvent*)HKWidgetEventInfo::Alloc(this);
			pClickEvent->pSource = pUp->pSource;
			OnClicked(*this, pClickEvent);
			HKWidgetEventInfo::Free(pClickEvent);
		}
	}
}

void HKWidgetButton::ButtonOver(HKWidget &sender, HKWidgetEventInfo *pEvent)
{
	if(bDown)
		bPressed = true;
}

void HKWidgetButton::ButtonOut(HKWidget &sender, HKWidgetEventInfo *pEvent)
{
	if(bDown)
		bPressed = false;
}
