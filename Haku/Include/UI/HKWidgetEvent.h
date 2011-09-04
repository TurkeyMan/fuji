#if !defined(_HKWIDGETEVENT_H)
#define _HKWIDGETEVENT_H

#include "HKEvent.h"

struct HKInputSource;
class HKWidget;

struct HKWidgetEventInfo
{
	HKWidgetEventInfo() {}
	HKWidgetEventInfo(HKWidget *pSender)
	{
		this->pSender = pSender;
		pUserData = NULL;
	}

	HKWidget *pSender;
	void *pUserData;
};

typedef HKEvent2<HKWidget &, HKWidgetEventInfo &> HKWidgetEvent;

struct HKWidgetEnabledEvent : public HKWidgetEventInfo
{
	HKWidgetEnabledEvent(HKWidget *pSender, bool bEnabled)
		: HKWidgetEventInfo(pSender)
	{
		this->bEnabled = bEnabled;
	}

	bool bEnabled;
};

struct HKWidgetVisibilityEvent : public HKWidgetEventInfo
{
	HKWidgetVisibilityEvent(HKWidget *pSender, bool bVisible)
		: HKWidgetEventInfo(pSender)
	{
		this->bVisible = bVisible;
	}

	bool bVisible;
};

struct HKWidgetFocusEvent : public HKWidgetEventInfo
{
	HKWidgetFocusEvent(HKWidget *pSender, bool bGainedFocus)
		: HKWidgetEventInfo(pSender)
	{
		this->bGainedFocus = bGainedFocus;
	}

	bool bGainedFocus;

	HKWidget *pGainedFocus;
	HKWidget *pLostFocus;
};

struct HKWidgetMoveEvent : public HKWidgetEventInfo
{
	HKWidgetMoveEvent(HKWidget *pSender)
		: HKWidgetEventInfo(pSender)
	{
	}

	MFVector newPos;
	MFVector oldPos;
};

struct HKWidgetResizeEvent : public HKWidgetEventInfo
{
	HKWidgetResizeEvent(HKWidget *pSender)
		: HKWidgetEventInfo(pSender)
	{
	}

	MFVector newSize;
	MFVector oldSize;
};

struct HKWidgetInputEvent : public HKWidgetEventInfo
{
	HKWidgetInputEvent(HKWidget *pSender, HKInputSource *pSource)
		: HKWidgetEventInfo(pSender)
	{
		this->pSource = pSource;
	}

	HKInputSource *pSource;
};

struct HKWidgetInputActionEvent : public HKWidgetEventInfo
{
	HKWidgetInputActionEvent(HKWidget *pSender, HKInputSource *pSource)
		: HKWidgetEventInfo(pSender)
	{
		this->pSource = pSource;
	}

	HKInputSource *pSource;
	MFVector pos;
	MFVector delta;
};

struct HKWidgetInputTextEvent : public HKWidgetEventInfo
{
	HKWidgetInputTextEvent(HKWidget *pSender, HKInputSource *pSource, int unicode)
		: HKWidgetEventInfo(pSender)
	{
		this->pSource = pSource;
		this->unicode = unicode;
	}

	HKInputSource *pSource;
	int unicode;
};

#endif
