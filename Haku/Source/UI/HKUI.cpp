#include "Haku.h"
#include "HKFactory.h"
#include "UI/HKUI.h"
#include "UI/HKWidgetEvent.h"
#include "UI/HKInputSource.h"

#include "UI/Widgets/HKWidgetLabel.h"
#include "UI/Widgets/HKWidgetButton.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/Widgets/HKWidgetLayoutLinear.h"
#include "UI/Widgets/HKWidgetLayoutFrame.h"
#include "UI/Widgets/HKWidgetPrefab.h"

#include "MFDisplay.h"

HKUserInterface *HKUserInterface::pActive = NULL;
HKFactory<HKWidget> *HKUserInterface::pFactory = NULL;
HKFactory<HKWidgetRenderer> *HKUserInterface::pRendererFactory = NULL;

MFSystemCallbackFunction HKUserInterface::pChainResizeCallback = NULL;

void HKUserInterface::Init()
{
//	pChainResizeCallback = MFSystem_RegisterSystemCallback(MFCB_DisplayResize, ResizeCallback);
	pChainResizeCallback = MFSystem_RegisterSystemCallback(MFCB_DisplayReset, ResizeCallback);

	if(!pFactory)
	{
		pFactory = new HKFactory<HKWidget>();

		HKWidgetFactory::FactoryType *pWidget = pFactory->RegisterType("HKWidget", HKWidget::Create, NULL);

		HKWidgetFactory::FactoryType *pLabel = pFactory->RegisterType("HKWidgetLabel", HKWidgetLabel::Create, pWidget);
		pFactory->RegisterType("HKWidgetButton", HKWidgetButton::Create, pLabel);
		pFactory->RegisterType("HKWidgetLayoutFrame", HKWidgetLayoutFrame::Create, pWidget);
		pFactory->RegisterType("HKWidgetLayoutLinear", HKWidgetLayoutLinear::Create, pWidget);
		pFactory->RegisterType("HKWidgetPrefab", HKWidgetPrefab::Create, pWidget);
	}

	if(!pRendererFactory)
	{
		pRendererFactory = new HKFactory<HKWidgetRenderer>();

		HKWidgetRendererFactory::FactoryType *pWidget = pRendererFactory->RegisterType("HKWidget", HKWidgetRenderer::Create, NULL);
		pRendererFactory->RegisterType("HKWidgetLabel", HKWidgetRendererLabel::Create, pWidget);
		pRendererFactory->RegisterType("HKWidgetButton", HKWidgetRendererButton::Create, pWidget);
		pRendererFactory->RegisterType("HKWidgetLayoutFrame", HKWidgetRenderer::Create, pWidget);
		pRendererFactory->RegisterType("HKWidgetLayoutLinear", HKWidgetRenderer::Create, pWidget);
		pRendererFactory->RegisterType("HKWidgetpREFAB", HKWidgetRenderer::Create, pWidget);
	}
}

void HKUserInterface::Deinit()
{
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
	HKWidget *pWidget = pFactory->Create(pWidgetType);
	HKWidgetRenderer *pRenderer = pRendererFactory->Create(pWidgetType);
	if(pRenderer)
		pWidget->SetRenderer(pRenderer);
	return pWidget;
}

HKUserInterface::HKUserInterface()
{
	MFZeroMemory(pFocusList, sizeof(pFocusList));
	MFZeroMemory(pHoverList, sizeof(pHoverList));

	pRoot = (HKWidgetLayoutFrame*)CreateWidget("HKWidgetLayoutFrame");

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
		if(ev.pSource->device == IDD_Mouse || ev.pSource->device == IDD_TouchPanel)
		{
			// transform the event into local space...
			MFVector pos = { ev.hover.x, ev.hover.y, 0.f, 1.f };
			MFVector dir = { 0.f, 0.f, 1.f, 1.f };

			MFVector localPos;
			pFocusWidget->IntersectWidget(pos, dir, &localPos);
			ev.hover.x = localPos.x;
			ev.hover.y = localPos.y;
		}

		if(pFocusWidget->InputEvent(manager, ev))
			return;
	}

	if(ev.pSource->device == IDD_Mouse || ev.pSource->device == IDD_TouchPanel)
	{
		// positional events will be sent to the hierarchy
		MFVector pos = { ev.hover.x, ev.hover.y, 0.f, 1.f };
		MFVector dir = { 0.f, 0.f, 1.f, 1.f };

		MFVector localPos;
		HKWidget *pWidget = pRoot->IntersectWidget(pos, dir, &localPos);

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
			ev.hover.x = localPos.x;
			ev.hover.y = localPos.y;

			// send the input event
			if(pWidget->InputEvent(manager, ev))
				return;
		}
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
