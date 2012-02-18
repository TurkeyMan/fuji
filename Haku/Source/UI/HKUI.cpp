#include "Haku.h"
#include "HKFactory.h"
#include "UI/HKUI.h"
#include "UI/HKWidgetEvent.h"
#include "UI/HKInputSource.h"
#include "UI/HKWidgetStyle.h"

#include "UI/Widgets/HKWidgetLabel.h"
#include "UI/Widgets/HKWidgetButton.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"
#include "UI/Widgets/HKWidgetPrefab.h"
#include "UI/Widgets/HKWidgetTextbox.h"
#include "UI/Widgets/HKWidgetListbox.h"
#include "UI/Widgets/HKWidgetSelectbox.h"

#include "MFDisplay.h"

HKUserInterface *HKUserInterface::pActive = NULL;
HKFactory<HKWidget> *HKUserInterface::pFactory = NULL;
HKFactory<HKWidgetRenderer> *HKUserInterface::pRendererFactory = NULL;

MFOpenHashTable<HKWidgetEvent::Delegate> HKUserInterface::eventHandlerRegistry;

MFSystemCallbackFunction HKUserInterface::pChainResizeCallback = NULL;

void HKUserInterface::Init()
{
	HKWidgetStyle::Init();

//	pChainResizeCallback = MFSystem_RegisterSystemCallback(MFCB_DisplayResize, ResizeCallback);
	pChainResizeCallback = MFSystem_RegisterSystemCallback(MFCB_DisplayReset, ResizeCallback);

	if(!pFactory)
	{
		pFactory = new HKFactory<HKWidget>();

		HKWidgetFactory::FactoryType *pWidget = RegisterWidget<HKWidget>(NULL);

		HKWidgetFactory::FactoryType *pLabel = RegisterWidget<HKWidgetLabel>(pWidget);
		RegisterWidget<HKWidgetButton>(pLabel);
		HKWidgetFactory::FactoryType *pFrame = RegisterWidget<HKWidgetLayoutFrame>(pWidget);
		RegisterWidget<HKWidgetLayoutLinear>(pWidget);
		RegisterWidget<HKWidgetPrefab>(pFrame);
		RegisterWidget<HKWidgetTextbox>(pWidget);
		RegisterWidget<HKWidgetListbox>(pWidget);
		RegisterWidget<HKWidgetSelectbox>(pFrame);
	}

	if(!pRendererFactory)
	{
		pRendererFactory = new HKFactory<HKWidgetRenderer>();

		HKWidgetRendererFactory::FactoryType *pWidget = pRendererFactory->RegisterType(HKWidget::TypeName(), HKWidgetRenderer::Create, NULL);
		pRendererFactory->RegisterType(HKWidgetLabel::TypeName(), HKWidgetRendererLabel::Create, pWidget);
		pRendererFactory->RegisterType(HKWidgetTextbox::TypeName(), HKWidgetRendererTextbox::Create, pWidget);
	}

	eventHandlerRegistry.Init(256, 256, 32);
}

void HKUserInterface::Deinit()
{
	eventHandlerRegistry.Deinit();

	if(pRendererFactory)
	{
		delete pRendererFactory;
		pRendererFactory = NULL;
	}

	if(pFactory)
	{
		delete pFactory;
		pFactory = NULL;
	}

	HKWidgetStyle::Deinit();
}

HKWidgetFactory::FactoryType *HKUserInterface::RegisterWidget(const char *pWidgetType, HKWidgetFactory::CreateFunc createDelegate, HKWidgetFactory::FactoryType *pParent)
{
	return pFactory->RegisterType(pWidgetType, createDelegate, pParent);
}

HKWidgetRendererFactory::FactoryType *HKUserInterface::RegisterWidgetRenderer(const char *pWidgetType, HKWidgetRendererFactory::CreateFunc createDelegate, HKWidgetRendererFactory::FactoryType *pParent)
{
	return pRendererFactory->RegisterType(pWidgetType, createDelegate, pParent);
}

HKWidgetFactory::FactoryType *HKUserInterface::FindWidgetType(const char *pWidgetType)
{
	return pFactory->FindType(pWidgetType);
}

HKWidget *HKUserInterface::CreateWidget(const char *pWidgetType)
{
	HKWidgetFactory::FactoryType *pType;
	HKWidget *pWidget = pFactory->Create(pWidgetType, &pType);
	if(!pWidget)
		return NULL;

	HKWidgetRenderer *pRenderer = pRendererFactory->Create(pWidgetType);
	while(!pRenderer && pType->pParent)
	{
		pType = pType->pParent;
		pRenderer = pRendererFactory->Create(pType->typeName);
	}
	if(pRenderer)
		pWidget->SetRenderer(pRenderer);

	return pWidget;
}

void HKUserInterface::RegisterEventHandler(MFString name, HKWidgetEvent::Delegate handler)
{
	eventHandlerRegistry.Add(name, handler);
}

HKWidgetEvent::Delegate& HKUserInterface::GetEventHandler(MFString name)
{
	return eventHandlerRegistry[name];
}

HKUserInterface::HKUserInterface()
{
	MFZeroMemory(pFocusList, sizeof(pFocusList));
	MFZeroMemory(pHoverList, sizeof(pHoverList));

	pRoot = CreateWidget<HKWidgetLayoutFrame>();

	MFRect rect;
//	MFView_GetOrthoRect(&rect);
	MFDisplay_GetDisplayRect(&rect);

	pRoot->SetPosition(MakeVector(rect.x, rect.y));
	pRoot->SetSize(MakeVector(rect.width, rect.height));

	pInputManager = new HKInputManager();
	pInputManager->OnInputEvent += fastdelegate::MakeDelegate(this, &HKUserInterface::OnInputEvent);
}

HKUserInterface::~HKUserInterface()
{
	delete pInputManager;
	delete pRoot;

	if(pActive == this)
		pActive = NULL;
}

void HKUserInterface::Update()
{
	pInputManager->Update();

	pRoot->Update();
}

void HKUserInterface::Draw()
{
	pRoot->Draw();
}

void HKUserInterface::AddTopLevelWidget(HKWidget *pWidget, bool bOwnWidget)
{
	pRoot->AddChild(pWidget, bOwnWidget);
}

void HKUserInterface::RemoveTopLevelWidget(HKWidget *pWidget)
{
	pRoot->RemoveChild(pWidget);
}

HKWidget *HKUserInterface::SetFocus(HKInputSource *pSource, HKWidget *pFocusWidget)
{
	HKWidget *pOld = pFocusList[pSource->sourceID];
	pFocusList[pSource->sourceID] = pFocusWidget;
	return pOld;
}

void HKUserInterface::LocaliseInput(HKInputManager::EventInfo &ev, HKWidget *pWidget, MFVector &localPos)
{
	ev.hover.x = localPos.x;
	ev.hover.y = localPos.y;

	if(ev.ev == HKInputManager::IE_Hover || ev.ev == HKInputManager::IE_Drag)
	{
		// transform delta
		MFVector transformedDelta = pWidget->GetInvTransform().TransformVector3(MakeVector(ev.hover.deltaX, ev.hover.deltaY));
		ev.hover.deltaX = transformedDelta.x;
		ev.hover.deltaY = transformedDelta.y;
	}

	if(ev.ev == HKInputManager::IE_Drag)
	{
		// transform secondary position
		MFVector transformedStart = pWidget->GetInvTransform().TransformVectorH(MakeVector(ev.drag.startX, ev.drag.startY));
		ev.drag.startX = transformedStart.x;
		ev.drag.startY = transformedStart.y;
	}
}

void HKUserInterface::OnInputEvent(HKInputManager &manager, const HKInputManager::EventInfo &ev)
{
	HKWidget *pFocusWidget = pFocusList[ev.pSource->sourceID];

	if(ev.pSource->device == IDD_Mouse || ev.pSource->device == IDD_TouchPanel)
	{
		// positional events will be sent to the hierarchy
		MFVector pos = { ev.hover.x, ev.hover.y, 0.f, 1.f };
		MFVector dir = { 0.f, 0.f, 1.f, 1.f };

		MFVector localPos;

		HKWidget *pWidget = NULL;
		if(pFocusWidget)
		{
			pWidget = pFocusWidget->IntersectWidget(pos, dir, &localPos);
			if(!pWidget)
				pWidget = pFocusWidget;
		}
		else
		{
			pWidget = pRoot->IntersectWidget(pos, dir, &localPos);
		}

		// check if the hover has changed
		HKWidget *pHover = pHoverList[ev.pSource->sourceID];
		if(pHover != pWidget)
		{
			pHoverList[ev.pSource->sourceID] = pWidget;

			if(pHover)
			{
				HKWidgetInputEvent ie(pHover, ev.pSource);
				pHover->OnHoverOut(*pHover, ie);
			}

			if(pWidget)
			{
				HKWidgetInputEvent ie(pWidget, ev.pSource);
				pWidget->OnHoverOver(*pWidget, ie);
			}
		}

		if(pWidget)
		{
			HKInputManager::EventInfo transformedEv = ev;
			LocaliseInput(transformedEv, pWidget, localPos);

			// send the input event
			if(pWidget->InputEvent(manager, transformedEv))
				return;
		}
	}
	else if(pFocusWidget)
	{
		// non-positional events
		pFocusWidget->InputEvent(manager, ev);
	}
}

void HKUserInterface::ResizeCallback()
{
	HKUserInterface &ui = HKUserInterface::Get();
	if(!&ui)
		return;

	MFRect rect;
	MFDisplay_GetDisplayRect(&rect);

	ui.pRoot->SetPosition(MakeVector(rect.x, rect.y));
	ui.pRoot->SetSize(MakeVector(rect.width, rect.height));

	if(pChainResizeCallback)
		pChainResizeCallback();
}
