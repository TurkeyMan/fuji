#include "Haku.h"
#include "UI/HKWidgetLayoutDescriptor.h"
#include "UI/Widgets/HKWidgetLayout.h"
#include "UI/HKUI.h"

#include "Fuji/MFFileSystem.h"
#include "Fuji/MFHeap.h"
#include "Fuji/MFDocumentXML.h"

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
	MFDocumentXML *pDoc = MFParseXML_ParseFile(pFilename);
	if(!pDoc)
		return false;

	// build the node tree
	MFXMLNode *pElement = MFParseXML_RootNode(pDoc);
	pRoot = ParseElement(pElement);

	MFParseXML_DestroyDocument(pDoc);
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

HKWidgetLayoutDescriptor::Node *HKWidgetLayoutDescriptor::ParseElement(MFXMLNode *pElement)
{
	// count child items
	int numAttributes = 0;
	for(MFXMLAttribute *pAtt = pElement->FirstAttribute(); pAtt; pAtt = pAtt->Next())
		++numAttributes;

	int numChildren = 0;
	for(MFXMLNode *pChildElement = pElement->FirstChild(); pChildElement; pChildElement = pChildElement->NextSibling())
		++numChildren;

	// allocate the node
	Node *pNode = (Node*)MFHeap_Alloc(sizeof(Node) + sizeof(Node::Attribute)*numAttributes + sizeof(Node*)*numChildren);
	pNode->pAttributes = (Node::Attribute*)(pNode + 1);
	pNode->ppChildren = (Node**)(pNode->pAttributes + numAttributes);
	pNode->numAttributes = numAttributes;
	pNode->numChildren = numChildren;

	// assign name
	pNode->type = pElement->Name();

	// get properties
	MFXMLAttribute *pAtt = pElement->FirstAttribute();
	for(int a=0; a<numAttributes; ++a)
	{
		pNode->pAttributes[a].property = pAtt->Name();
		pNode->pAttributes[a].value = pAtt->Value();
		pAtt = pAtt->Next();
	}

	// and parse the children
	MFXMLNode *pChildElement = pElement->FirstChild();
	for(int a=0; a<numChildren; ++a)
	{
		pNode->ppChildren[a] = ParseElement(pChildElement);
		pChildElement = pChildElement->NextSibling();
	}

	return pNode;
}
