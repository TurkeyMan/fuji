#if !defined(_HKWIDGET_FRAMELAYOUT_H)
#define _HKWIDGET_FRAMELAYOUT_H

#include "UI/Widgets/HKWidgetLayout.h"

class HKWidgetLayoutFrame : public HKWidgetLayout
{
public:
	static HKWidget *Create();

	HKWidgetLayoutFrame();
	~HKWidgetLayoutFrame();

protected:
	virtual void ArrangeChildren();
};

#endif
