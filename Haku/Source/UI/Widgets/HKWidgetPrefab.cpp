#include "Haku.h"
#include "UI/HKUI.h"
#include "UI/Widgets/HKWidgetPrefab.h"
#include "UI/HKWidgetLoader-XML.h"

HKWidget *HKWidgetPrefab::Create()
{
	return new HKWidgetPrefab();
}

HKWidgetPrefab::HKWidgetPrefab()
{
	pTypeName = "HKWidgetPrefab";
}

HKWidgetPrefab::~HKWidgetPrefab()
{
}

void HKWidgetPrefab::SetProperty(const char *pProperty, const char *pValue)
{
	if(!MFString_CaseCmp(pProperty, "prefab"))
		LoadPrefab(pValue);
	else
		HKWidget::SetProperty(pProperty, pValue);
}

MFString HKWidgetPrefab::GetProperty(const char *pProperty)
{
	if(!MFString_CaseCmp(pProperty, "prefab"))
		return prefab;
	return HKWidget::GetProperty(pProperty);
}

void HKWidgetPrefab::LoadPrefab(const char *pPrefab)
{
	prefab = pPrefab;

	if(MFString_EndsWith(pPrefab, ".xml"))
	{
		HKWidget *pPrefabRoot = HKWidget_CreateFromXML(pPrefab);
		AddChild(pPrefabRoot);
	}
	else
	{
		//... error?
	}
}

void HKWidgetPrefab::ArrangeChildren()
{
	for(int a=0; a<children.size(); ++a)
	{
		children[a].pChild->SetSize(GetSize());
	}
}
