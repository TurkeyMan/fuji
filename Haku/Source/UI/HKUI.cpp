#include "Haku.h"
#include "HKFactory.h"
#include "UI/HKUI.h"
#include "UI/HKWidgetEvent.h"
#include "UI/HKInputSource.h"

HKUserInterface *HKUserInterface::pActive = NULL;
HKFactory<HKWidget> *pFactory = NULL;

void HKUserInterface::Init()
{
	HKWidgetEvent::Init();
	HKInputSource::Init();

	if(!pFactory)
	{
		pFactory = new HKFactory<HKWidget>();

		HKWidgetFactory::FactoryType *pWidget = pFactory->RegisterType("HKWidget", HKWidget::Create, NULL);
	}
}

void HKUserInterface::Deinit()
{
	if(pFactory)
	{
		delete pFactory;
		pFactory = NULL;
	}

	HKInputSource::Deinit();
	HKWidgetEvent::Deinit();
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

}

HKUserInterface::~HKUserInterface()
{

}

void HKUserInterface::Update()
{

}

void HKUserInterface::Draw()
{

}
