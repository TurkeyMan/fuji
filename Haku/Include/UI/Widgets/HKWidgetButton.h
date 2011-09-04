#if !defined(_HKWIDGET_BUTTON_H)
#define _HKWIDGET_BUTTON_H

#include "UI/HKWidget.h"

class HKWidgetButton : public HKWidget
{
public:
	static HKWidget *Create();

	enum ButtonFlags
	{
		BF_TriggerOnDown = 0x1,
		BF_StateButton = 0x2
	};

	HKWidgetButton();
	~HKWidgetButton();

	void SetButtonFlags(uint32 flags) { buttonFlags = flags; }

	// state change events
	HKWidgetEvent OnClicked;

protected:
	uint32 buttonFlags;

	bool bDown;
	bool bPressed;
	bool bState;

	void ButtonDown(HKWidget &widget, HKWidgetEventInfo *pEvent);
	void ButtonUp(HKWidget &widget, HKWidgetEventInfo *pEvent);
	void ButtonOver(HKWidget &widget, HKWidgetEventInfo *pEvent);
	void ButtonOut(HKWidget &widget, HKWidgetEventInfo *pEvent);
};

#endif
