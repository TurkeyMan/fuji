#if !defined(_HKWIDGET_BUTTON_H)
#define _HKWIDGET_BUTTON_H

#include "UI/Widgets/HKWidgetLabel.h"
#include "UI/HKWidgetRenderer.h"

class HKWidgetButton : public HKWidgetLabel
{
public:
	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "Button"; }

	enum ButtonFlags
	{
		BF_TriggerOnDown = 0x1,
		BF_StateButton = 0x2
	};

	HKWidgetButton(HKWidgetType *pType);
	virtual	~HKWidgetButton();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	bool GetPressed() const { return bPressed; }
	bool GetState() const { return bState; }

	void SetStylePressed(MFString style) { stylePressed = style; }
	void SetStyleState(MFString style) { styleOnState = style; }

	void SetState(bool bState) { this->bState = bState; }
	void SetButtonFlags(uint32 flags) { buttonFlags = flags; }

	// state change events
	HKWidgetEvent OnClicked;

protected:
	MFString stylePressed;
	MFString styleOnState;

	uint32 buttonFlags;

	bool bDown;
	bool bPressed;
	bool bState;

	void SetPressed(bool bPressed);
	void SetButtonState(bool bState);

	virtual void UpdateStyle();

	void ButtonDown(HKWidget &widget, const HKWidgetEventInfo &ev);
	void ButtonUp(HKWidget &widget, const HKWidgetEventInfo &ev);
	void Hover(HKWidget &widget, const HKWidgetEventInfo &ev);

	static const EnumKeypair sButtonFlagKeys[];
};

#endif
