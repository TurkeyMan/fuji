#if !defined(_HKWIDGETEVENT_H)
#define _HKWIDGETEVENT_H

#include "HKEvent.h"

class HKInputSource;
class HKWidget;

class HKWidgetEvent
{
public:
	static void Init();
	static void Deinit();

	static HKEventInfo *Alloc(HKWidget *pSender);
	static void Free(HKEventInfo *pEvent);
};

struct HKWidgetEnabledEvent : public HKEventInfo
{
	bool bEnabled;
};

struct HKWidgetVisibilityEvent : public HKEventInfo
{
	bool bVisible;
};

struct HKWidgetFocusEvent : public HKEventInfo
{
	bool bGainedFocus;

	HKWidget *pGainedFocus;
	HKWidget *pLostFocus;
};

struct HKWidgetMoveEvent : public HKEventInfo
{
	MFVector newPos;
	MFVector oldPos;
};

struct HKWidgetResizeEvent : public HKEventInfo
{
	MFVector newSize;
	MFVector oldSize;
};

struct HKWidgetInputEvent : public HKEventInfo
{
	HKInputSource *pSource;
};

struct HKWidgetInputActionEvent : public HKEventInfo
{
	HKInputSource *pSource;
	MFVector pos;
	MFVector oldPos;
};

struct HKWidgetInputTextEvent : public HKEventInfo
{
	HKInputSource *pSource;
	int unicode;
};

#endif
