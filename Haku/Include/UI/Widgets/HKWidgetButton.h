#if !defined(_HKWIDGET_BUTTON_H)
#define _HKWIDGET_BUTTON_H

#include "UI/HKWidget.h"
#include "UI/HKWidgetRenderer.h"

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

	MFString GetLabel() const { return label; }
	bool GetPressed() const { return bPressed; }
	bool GetState() const { return bState; }

	void SetLabel(MFString label) { this->label = label; }
	void SetButtonFlags(uint32 flags) { buttonFlags = flags; }

	// state change events
	HKWidgetEvent OnClicked;

protected:
	MFString label;

	uint32 buttonFlags;

	bool bDown;
	bool bPressed;
	bool bState;

	void ButtonDown(HKWidget &widget, HKWidgetEventInfo &ev);
	void ButtonUp(HKWidget &widget, HKWidgetEventInfo &ev);
	void ButtonOver(HKWidget &widget, HKWidgetEventInfo &ev);
	void ButtonOut(HKWidget &widget, HKWidgetEventInfo &ev);
};

class HKWidgetRendererButton : public HKWidgetRenderer
{
public:
	static HKWidgetRenderer *Create();
protected:
	virtual void Render(const HKWidget &widget, const MFMatrix &worldTransform);
};

#endif
