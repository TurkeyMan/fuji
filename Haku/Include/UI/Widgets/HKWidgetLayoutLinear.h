#if !defined(_HKWIDGET_LINEARLAYOUT_H)
#define _HKWIDGET_LINEARLAYOUT_H

#include "UI/Widgets/HKWidgetLayout.h"

class HKWidgetLayoutLinear : public HKWidgetLayout
{
public:
	enum Orientation
	{
		Horizontal,
		Vertical
	};

	static HKWidget *Create(HKWidgetType *pType);
	static const char *TypeName() { return "LinearLayout"; }

	HKWidgetLayoutLinear(HKWidgetType *pType);
	virtual ~HKWidgetLayoutLinear();

	virtual void SetProperty(const char *pProperty, const char *pValue);
	virtual MFString GetProperty(const char *pProperty);

	Orientation GetOrientation() const { return orientation; }

	void SetOrientation(Orientation orientation) { this->orientation = orientation; }

protected:
	Orientation orientation;

	virtual void ArrangeChildren();

	static const EnumKeypair sOrientationKeys[];
};

#endif
