#if !defined(_HKWIDGET_BUTTON_H)
#define _HKWIDGET_BUTTON_H

#include "UI/Widgets/HKWidgetLabel.h"
#include "UI/HKWidgetRenderer.h"

class HKWidgetButton : public HKWidgetLabel
{
public:
	static HKWidget *Create();

	enum ButtonFlags
	{
		BF_TriggerOnDown = 0x1,
		BF_StateButton = 0x2
	};

	HKWidgetButton();
	virtual	~HKWidgetButton();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	bool GetPressed() const { return bPressed; }
	bool GetState() const { return bState; }

	void SetState(bool bState) { this->bState = bState; }
	void SetButtonFlags(uint32 flags) { buttonFlags = flags; }

	// state change events
	HKWidgetEvent OnClicked;

protected:
	uint32 buttonFlags;

	bool bDown;
	bool bPressed;
	bool bState;

	void ButtonDown(HKWidget &widget, HKWidgetEventInfo &ev);
	void ButtonUp(HKWidget &widget, HKWidgetEventInfo &ev);
	void Hover(HKWidget &widget, HKWidgetEventInfo &ev);

	static const EnumKeypair sButtonFlagKeys[];
};

class HKWidgetRendererButton : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
