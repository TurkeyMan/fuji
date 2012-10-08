#include "Haku.h"
#include "UI/HKWidget.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/HKUI.h"

#include "Fuji/MFFileSystem.h"
#include "Fuji/MFHeap.h"
#include "Fuji/MFDocumentXML.h"

static HKWidget *ParseElement(MFXMLNode *pElement)
{
	// create widget
	const char *pType = pElement->Name();
	HKWidget *pWidget = HKUserInterface::CreateWidget(pType);
	if(!pWidget)
		return NULL;

	// apply properties
	MFXMLAttribute *pAtt = pElement->FirstAttribute();
	while(pAtt)
	{
		pWidget->SetProperty(pAtt->Name(), pAtt->Value());
		pAtt = pAtt->Next();
	}

	// load children
	MFXMLNode *pChildElement = pElement->FirstChild();
	while(pChildElement)
	{
		HKWidget *pChild = ParseElement(pChildElement);
		if(pChild)
		{
			HKWidgetLayout *pLayout = (HKWidgetLayout*)pWidget;
			pLayout->AddChild(pChild, true);
		}

		pChildElement = pChildElement->NextSibling();
	}

	return pWidget;
}

HKWidget *HKWidget_CreateFromXML(const char *pFilename)
{
	// attempt to load the xml document
	MFDocumentXML *pDoc = MFParseXML_ParseFile(pFilename);
	if(!pDoc)
		return NULL;

	MFXMLNode *pElement = MFParseXML_RootNode(pDoc);
	HKWidget *pWidget = ParseElement(pElement);

	MFParseXML_DestroyDocument(pDoc);

	return pWidget;
}
