#include "Haku.h"
#include "UI/HKWidgetLayoutDescriptor.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/HKUI.h"

#include "MFFileSystem.h"
#include "MFHeap.h"

#include "tinyxml/tinyxml.h"

HKWidgetLayoutDescriptor::HKWidgetLayoutDescriptor()
{
	pRoot = NULL;
}

HKWidgetLayoutDescriptor::~HKWidgetLayoutDescriptor()
{
	DestroyNode(pRoot);
}

bool HKWidgetLayoutDescriptor::LoadFromXML(const char *pFilename)
{
	// destroy any existing descriptor
	DestroyNode(pRoot);

	// attempt to load the xml document
	size_t len;
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
	pRoot = ParseElement(pElement);

	MFHeap_Free(pFile);
	return true;
}

HKWidget *HKWidgetLayoutDescriptor::Spawn()
{
	return Spawn(pRoot);
}

HKWidget *HKWidgetLayoutDescriptor::Spawn(Node *pNode)
{
	// create widget
	HKWidget *pWidget = HKUserInterface::CreateWidget(pNode->type.CStr());
	if(!pWidget)
		return NULL;

	// apply properties
	for(int a=0; a<pNode->numAttributes; ++a)
		pWidget->SetProperty(pNode->pAttributes[a].property.CStr(), pNode->pAttributes[a].value.CStr());

	// spawn children
	for(int a=0; a<pNode->numChildren; ++a)
	{
		HKWidget *pChild = Spawn(pNode->ppChildren[a]);
		if(pChild)
		{
			HKWidgetLayout *pLayout = (HKWidgetLayout*)pWidget;
			pLayout->AddChild(pChild, true);
		}
	}

	return pWidget;
}

void HKWidgetLayoutDescriptor::DestroyNode(Node *pNode)
{
	if(!pNode)
		return;

	// destroy the child nodes
	for(int a=0; a<pNode->numChildren; ++a)
		DestroyNode(pNode->ppChildren[a]);

	// and free this node
	MFHeap_Free(pNode);
}

HKWidgetLayoutDescriptor::Node *HKWidgetLayoutDescriptor::ParseElement(TiXmlElement *pElement)
{
	// count child items
	int numAttributes = 0;
	for(TiXmlAttribute *pAtt = pElement->FirstAttribute(); pAtt; pAtt = pAtt->Next())
		++numAttributes;

	int numChildren = 0;
	for(TiXmlElement *pChildElement = pElement->FirstChildElement(); pChildElement; pChildElement = pChildElement->NextSiblingElement())
		++numChildren;

	// allocate the node
	Node *pNode = (Node*)MFHeap_Alloc(sizeof(Node) + sizeof(Node::Attribute)*numAttributes + sizeof(Node*)*numChildren);
	pNode->pAttributes = (Node::Attribute*)(pNode + 1);
	pNode->ppChildren = (Node**)(pNode->pAttributes + numAttributes);
	pNode->numAttributes = numAttributes;
	pNode->numChildren = numChildren;

	// assign name
	pNode->type = pElement->Value();

	// get properties
	TiXmlAttribute *pAtt = pElement->FirstAttribute();
	for(int a=0; a<numAttributes; ++a)
	{
		pNode->pAttributes[a].property = pAtt->Name();
		pNode->pAttributes[a].value = pAtt->Value();
		pAtt = pAtt->Next();
	}

	// and parse the children
	TiXmlElement *pChildElement = pElement->FirstChildElement();
	for(int a=0; a<numChildren; ++a)
	{
		pNode->ppChildren[a] = ParseElement(pChildElement);
		pChildElement = pChildElement->NextSiblingElement();
	}

	return pNode;
}
