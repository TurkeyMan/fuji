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
