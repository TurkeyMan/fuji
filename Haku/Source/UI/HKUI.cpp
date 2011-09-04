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
	HKWidgetEventInfo::Init();

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

	HKWidgetEventInfo::Deinit();
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

void HKUserInterface::OnInputEvent(HKInputManager &manager, HKInputManager::EventInfo &ev)
{
	// do something with the new input event...
	int x = 0;
}
