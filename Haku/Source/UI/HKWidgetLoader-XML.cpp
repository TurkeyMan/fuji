#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/HKUI.h"

#include "MFFileSystem.h"
#include "MFHeap.h"

#include <tinyxml.h>

static HKWidget *ParseElement(TiXmlElement *pElement)
{
	// create widget
	const char *pType = pElement->Value();
	HKWidget *pWidget = HKUserInterface::CreateWidget(pType);
	if(!pWidget)
		return NULL;

	// apply properties
	TiXmlAttribute *pAtt = pElement->FirstAttribute();
	while(pAtt)
	{
		pWidget->SetProperty(pAtt->Name(), pAtt->Value());
		pAtt = pAtt->Next();
	}

	// load children
	TiXmlElement *pChildElement = pElement->FirstChildElement();
	while(pChildElement)
	{
		HKWidget *pChild = ParseElement(pChildElement);
		if(pChild)
		{
			HKWidgetLayout *pLayout = (HKWidgetLayout*)pWidget;
			pLayout->AddChild(pChild, true);
		}

		pChildElement = pChildElement->NextSiblingElement();
	}

	return pWidget;
}

HKWidget *HKWidget_CreateFromXML(const char *pFilename)
{
	size_t len;
	char *pFile = MFFileSystem_Load(pFilename, &len, true);

	if(!pFile)
		return NULL;

	TiXmlDocument doc;
	doc.Parse(pFile);

	if(doc.Error())
	{
		MFHeap_Free(pFile);
		return NULL;
	}

	TiXmlElement *pElement = doc.FirstChildElement();
	return ParseElement(pElement);
}
