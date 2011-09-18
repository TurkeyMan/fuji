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

	static HKWidget *Create();

	HKWidgetLayoutLinear();
	virtual ~HKWidgetLayoutLinear();

	virtual void SetPropertyS(const char *pProperty, const char *pValue);
	virtual MFString GetPropertyS(const char *pProperty);

	Orientation GetOrientation() const { return orientation; }

	void SetOrientation(Orientation orientation) { this->orientation = orientation; }

protected:
	Orientation orientation;

	virtual void ArrangeChildren();

	static const EnumKeypair sOrientationKeys[];
};

#endif
