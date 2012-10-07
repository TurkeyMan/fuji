#pragma once
#if !defined(_HKWIDGET_LAYOUTDESCRIPTOR_H)
#define _HKWIDGET_LAYOUTDESCRIPTOR_H

#include "HKWidget.h"

class MFXMLNode;

class HKWidgetLayoutDescriptor
{
public:
	HKWidgetLayoutDescriptor();
	~HKWidgetLayoutDescriptor();

	bool LoadFromXML(const char *pFilename);

	HKWidget *Spawn();

protected:
	struct Node
	{
		struct Attribute
		{
			MFString property;
			MFString value;
		};

		MFString type;
		Attribute *pAttributes;
		Node **ppChildren;
		int numChildren, numAttributes;
	};

	Node *pRoot;

	HKWidget *Spawn(Node *pNode);
	void DestroyNode(Node *pNode);

	static Node *ParseElement(MFXMLNode *pElement);
};

#endif
