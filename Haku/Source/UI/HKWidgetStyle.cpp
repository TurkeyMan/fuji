#include "Haku.h"
#include "UI/HKWidgetStyle.h"
#include "UI/HKWidget.h"

#include "MFFileSystem.h"

#include "tinyxml/tinyxml.h"

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
	uint32 len;
	char *pFile = MFFileSystem_Load(pFilename, &len, true);

	if(!pFile)
		return false;

	TiXmlDocument doc;
	{
		MFHEAP_SCOPE(MFHT_ActiveTemporary);
		doc.Parse(pFile);
	}

	if(doc.Error())
	{
		MFHeap_Free(pFile);
		return false;
	}

	// build the node tree
	TiXmlElement *pElement = doc.FirstChildElement();
	while(pElement)
	{
		if(!MFString_CaseCmp(pElement->Value(), "Resources"))
		{
			TiXmlElement *pStyle = pElement->FirstChildElement();
			while(pStyle)
			{
				if(!MFString_CaseCmp(pStyle->Value(), "Style"))
				{
					const char *pName = pStyle->Attribute("id");
					const char *pParent = pStyle->Attribute("parent");

					HKWidgetStyle &style = sStyles.Create(pName);
					style.name = pName;
					style.parent = pParent;

					TiXmlElement *pProperty = pStyle->FirstChildElement();
					while(pProperty)
					{
						if(!MFString_CaseCmp(pProperty->Value(), "Property"))
						{
							const char *pPropertyName = pProperty->Attribute("id");
							MFDebug_Assert(pPropertyName, "Expected 'id=...'. Property name is not defined!");

							const char *pValue = pProperty->GetText();

							Property &p = style.properties.push();
							++style.numProperties;

							p.property = pPropertyName;
							p.property = pValue;
						}
						else if(!MFString_CaseCmp(pProperty->Value(), "Properties"))
						{
							TiXmlAttribute *pProp = pProperty->FirstAttribute();
							while(pProp)
							{
								Property &p = style.properties.push();
								++style.numProperties;

								p.property = pProp->Name();
								p.value = pProp->Value();

								pProp = pProp->Next();
							}
						}

						pProperty = pProperty->NextSiblingElement();
					}
				}

				pStyle = pStyle->NextSiblingElement();
			}
		}

		pElement = pElement->NextSiblingElement();
	}

	MFHeap_Free(pFile);
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
