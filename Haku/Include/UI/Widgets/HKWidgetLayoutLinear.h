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
	~HKWidgetLayoutLinear();

	Orientation GetOrientation() const { return orientation; }

	void SetOrientation(Orientation orientation) { this->orientation = orientation; }

protected:
	Orientation orientation;

	virtual void ArrangeChildren();
};

#endif
