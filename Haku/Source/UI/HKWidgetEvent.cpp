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

void HKWidgetEventInfo::Init()
{
	if(!pEventPool)
		pEventPool = new MFObjectPool(maxEventSize, 128, 128);
}

void HKWidgetEventInfo::Deinit()
{
	if(pEventPool)
	{
		delete pEventPool;
		pEventPool = NULL;
	}
}

HKWidgetEventInfo *HKWidgetEventInfo::Alloc(HKWidget *pSender)
{
	HKWidgetEventInfo *pInfo = (HKWidgetEventInfo*)pEventPool->Alloc();
	pInfo->pSender = pSender;
	pInfo->pUserData = NULL;
	return pInfo;
}

void HKWidgetEventInfo::Free(HKWidgetEventInfo *pInfo)
{
	pEventPool->Free(pInfo);
}
