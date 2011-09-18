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
	virtual	~HKWidgetButton();

	virtual void SetPropertyB(const char *pProperty, bool bValue);
	virtual void SetPropertyI(const char *pProperty, int value);
	virtual void SetPropertyS(const char *pProperty, const char *pValue);
	virtual bool GetPropertyB(const char *pProperty);
	virtual int GetPropertyI(const char *pProperty);
	virtual MFString GetPropertyS(const char *pProperty);

	MFString GetLabel() const { return label; }
	bool GetPressed() const { return bPressed; }
	bool GetState() const { return bState; }

	void SetLabel(MFString label) { this->label = label; }
	void SetState(bool bState) { this->bState = bState; }
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
