#include "Haku.h"
#include "UI/HKWidgetEvent.h"

#include "MFObjectPool.h"

#define MAX(X, Y) ((X)>(Y)?(X):(Y))

MFObjectPool *pEventPool = NULL;
static const int maxEventSize = MAX(
									MAX(
										MAX(
											MAX(
												MAX(
													MAX(
														MAX(sizeof(HKWidgetEnabledEvent),
														sizeof(HKWidgetVisibilityEvent)),
													sizeof(HKWidgetFocusEvent)),
												sizeof(HKWidgetMoveEvent)),
											sizeof(HKWidgetResizeEvent)),
										sizeof(HKWidgetInputEvent)),
									sizeof(HKWidgetInputActionEvent)),
								sizeof(HKWidgetInputTextEvent));

void HKWidgetEvent::Init()
{
	if(!pEventPool)
		pEventPool = new MFObjectPool(maxEventSize, 128, 128);
}

void HKWidgetEvent::Deinit()
{
	if(pEventPool)
	{
		delete pEventPool;
		pEventPool = NULL;
	}
}

HKEventInfo *HKWidgetEvent::Alloc(HKWidget *pSender)
{
	HKEventInfo *pInfo = (HKEventInfo*)pEventPool->Alloc();
	pInfo->pSender = pSender;
	pInfo->pUserData = NULL;
	return pInfo;
}

void HKWidgetEvent::Free(HKEventInfo *pInfo)
{
	pEventPool->Free(pInfo);
}
