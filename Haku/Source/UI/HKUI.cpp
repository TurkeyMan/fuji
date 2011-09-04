#include "Haku.h"
#include "HKFactory.h"
#include "UI/HKUI.h"
#include "UI/HKWidgetEvent.h"
#include "UI/HKInputSource.h"

#include "UI/Widgets/HKWidgetButton.h"

HKUserInterface *HKUserInterface::pActive = NULL;
HKFactory<HKWidget> *HKUserInterface::pFactory = NULL;

void HKUserInterface::Init()
{
	if(!pFactory)
	{
		pFactory = new HKFactory<HKWidget>();

		HKWidgetFactory::FactoryType *pWidget = pFactory->RegisterType("HKWidget", HKWidget::Create, NULL);

		pFactory->RegisterType("HKWidgetButton", HKWidgetButton::Create, pWidget);
	}
}

void HKUserInterface::Deinit()
{
	if(pFactory)
	{
		delete pFactory;
		pFactory = NULL;
	}
}

HKWidgetFactory::FactoryType *HKUserInterface::RegisterWidget(const char *pWidgetType, HKWidgetFactory::CreateFunc createDelegate, HKWidgetFactory::FactoryType *pParent)
{
	return pFactory->RegisterType(pWidgetType, createDelegate, pParent);
}

HKWidgetFactory::FactoryType *HKUserInterface::FindWidgetType(const char *pWidgetType)
{
	return pFactory->FindType(pWidgetType);
}

HKWidget *HKUserInterface::CreateWidget(const char *pWidgetType)
{
	HKWidget *pWidget = pFactory->Create(pWidgetType);
//	pWidget->SetRenderDelegate(pRenderer->FindRenderer(pWidgetType);
	return pWidget;
}

HKUserInterface::HKUserInterface()
{
	MFZeroMemory(pFocusList, sizeof(pFocusList));
	MFZeroMemory(pHoverList, sizeof(pHoverList));

	pRoot = NULL;
	pRenderer = NULL;

	pInputManager = new HKInputManager();
	pInputManager->OnInputEvent += fastdelegate::MakeDelegate(this, &HKUserInterface::OnInputEvent);
}

HKUserInterface::~HKUserInterface()
{
	delete pInputManager;
}

void HKUserInterface::Update()
{
	pInputManager->Update();
}

void HKUserInterface::Draw()
{
}

HKWidget *HKUserInterface::SetFocus(HKInputSource *pSource, HKWidget *pFocusWidget)
{
	HKWidget *pOld = pFocusList[pSource->sourceID];
	pFocusList[pSource->sourceID] = pFocusWidget;
	return pOld;
}

void HKUserInterface::OnInputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
{
	HKWidget *pFocusWidget = pFocusList[ev.pSource->sourceID];
	if(pFocusWidget)
	{
		if(pFocusWidget->InputEvent(manager, ev))
			return;
	}

	if(ev.pSource->device == IDD_Mouse || ev.pSource->device == IDD_TouchPanel)
	{
		// positional events will be sent to the hierarchy
		MFVector pos = { ev.hover.x, ev.hover.y, 0.f, 1.f };
		MFVector dir = { 0.f, 0.f, 1.f, 1.f };
		HKWidget *pWidget = pRoot->IntersectWidget(pos, dir);

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
			// send the input event
			if(pWidget->InputEvent(manager, ev))
				return;
		}
	}
}
