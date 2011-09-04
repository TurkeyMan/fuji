#if !defined(_HKWIDGETEVENT_H)
#define _HKWIDGETEVENT_H

#include "HKEvent.h"

struct HKInputSource;
class HKWidget;

class HKWidgetEventInfo
{
public:
	HKWidget *pSender;
	void *pUserData;

	// static methods to manage the pool
	static void Init();
	static void Deinit();

	static HKWidgetEventInfo *Alloc(HKWidget *pSender);
	static void Free(HKWidgetEventInfo *pEvent);
};

typedef HKEvent2<HKWidget &, HKWidgetEventInfo *> HKWidgetEvent;

struct HKWidgetEnabledEvent : public HKWidgetEventInfo
{
	bool bEnabled;
};

struct HKWidgetVisibilityEvent : public HKWidgetEventInfo
{
	bool bVisible;
};

struct HKWidgetFocusEvent : public HKWidgetEventInfo
{
	bool bGainedFocus;

	HKWidget *pGainedFocus;
	HKWidget *pLostFocus;
};

struct HKWidgetMoveEvent : public HKWidgetEventInfo
{
	MFVector newPos;
	MFVector oldPos;
};

struct HKWidgetResizeEvent : public HKWidgetEventInfo
{
	MFVector newSize;
	MFVector oldSize;
};

struct HKWidgetInputEvent : public HKWidgetEventInfo
{
	HKInputSource *pSource;
};

struct HKWidgetInputActionEvent : public HKWidgetEventInfo
{
	HKInputSource *pSource;
	MFVector pos;
	MFVector oldPos;
};

struct HKWidgetInputTextEvent : public HKWidgetEventInfo
{
	HKInputSource *pSource;
	int unicode;
};

#endif
