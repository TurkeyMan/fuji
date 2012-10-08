#include "Haku.h"
#include "UI/HKWidgetStyle.h"
#include "UI/HKWidget.h"

#include "Fuji/MFDocumentXML.h"

MFOpenHashTable<HKWidgetStyle> HKWidgetStyle::sStyles;

void HKWidgetStyle::Init()
{
	sStyles.Init(256, 16, 16);
}

void HKWidgetStyle::Deinit()
{
	sStyles.Deinit();
}

bool HKWidgetStyle::LoadStylesFromXML(const char *pFilename)
{
	// attempt to load the xml document
	MFDocumentXML *pDoc = MFParseXML_ParseFile(pFilename);
	if(!pDoc)
		return false;

	// build the node tree
	MFXMLNode *pElement = MFParseXML_RootNode(pDoc);
	while(pElement)
	{
		if(!MFString_CaseCmp(pElement->Name(), "Resources"))
		{
			MFXMLNode *pStyle = pElement->FirstChild();
			while(pStyle)
			{
				if(!MFString_CaseCmp(pStyle->Name(), "Style"))
				{
					const char *pName = pStyle->Attribute("id");
					const char *pParent = pStyle->Attribute("parent");

					HKWidgetStyle &style = sStyles.Create(pName);
					style.name = pName;
					style.parent = pParent;

					MFXMLNode *pProperty = pStyle->FirstChild();
					while(pProperty)
					{
						if(!MFString_CaseCmp(pProperty->Name(), "Property"))
						{
							const char *pPropertyName = pProperty->Attribute("id");
							MFDebug_Assert(pPropertyName, "Expected 'id=...'. Property name is not defined!");

							const char *pValue = pProperty->Value();

							Property &p = style.properties.push();
							++style.numProperties;

							p.property = pPropertyName;
							p.property = pValue;
						}
						else if(!MFString_CaseCmp(pProperty->Name(), "Properties"))
						{
							MFXMLAttribute *pProp = pProperty->FirstAttribute();
							while(pProp)
							{
								Property &p = style.properties.push();
								++style.numProperties;

								p.property = pProp->Name();
								p.value = pProp->Value();

								pProp = pProp->Next();
							}
						}

						pProperty = pProperty->NextSibling();
					}
				}

				pStyle = pStyle->NextSibling();
			}
		}

		pElement = pElement->NextSibling();
	}

	MFParseXML_DestroyDocument(pDoc);
	return true;
}

HKWidgetStyle* HKWidgetStyle::FindStyle(const char *pStyle)
{
	return sStyles.Get(pStyle);
}

void HKWidgetStyle::Apply(HKWidget *pWidget)
{
	if(!parent.IsEmpty())
	{
		// apply parent properties
		HKWidgetStyle *pStyle = FindStyle(parent.CStr());
		if(pStyle)
			pStyle->Apply(pWidget);
	}

	// apply properties
	for(int a=0; a<numProperties; ++a)
		pWidget->SetProperty(properties[a].property.CStr(), properties[a].value.CStr());
}
