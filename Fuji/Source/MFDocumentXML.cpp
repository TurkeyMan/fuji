#include "Fuji_Internal.h"
#include "MFHeap.h"
#include "MFFileSystem.h"
#include "MFDocumentXML.h"

#define RAPIDXML_NO_EXCEPTIONS
#define RAPIDXML_NO_STDLIB

namespace std {
	typedef size_t size_t;
}

#define assert(x) MFDebug_Assert(x, "XML parse assert failed")

namespace rapidxml
{
    void parse_error_handler(const char *what, void *where)
	{
		MFDebug_Log(1, "XML parse error:");
		MFDebug_Log(1, what);
	}
}

#include "Middleware/rapidxml-1.13/rapidxml.hpp"
using namespace rapidxml;

struct MFDocumentXML
{
	char *pText;
	bool bOwnsBuffer;

	xml_document<> doc;
};

MF_API MFDocumentXML *MFParseXML_ParseFile(const char *pFilename)
{
	char *pBuffer = MFFileSystem_Load(pFilename, NULL, 1);
	if(!pBuffer)
		return NULL;

	MFDocumentXML *pDoc = MFParseXML_Parse(pBuffer, false);
	pDoc->bOwnsBuffer = true;

	return pDoc;
}

MF_API MFDocumentXML *MFParseXML_Parse(char *pDocument, bool bCopyBuffer)
{
	MFDocumentXML *pDoc = (MFDocumentXML*)MFHeap_Alloc(sizeof(MFDocumentXML));

	if(bCopyBuffer)
	{
		pDoc->pText = (char*)MFHeap_Alloc(MFString_Length(pDocument) + 1);
		MFString_Copy(pDoc->pText, pDocument);
		pDoc->bOwnsBuffer = true;
	}
	else
	{
		pDoc->pText = pDocument;
		pDoc->bOwnsBuffer = false;
	}

	// load xml
	new(&pDoc->doc) xml_document<>;
	pDoc->doc.parse<parse_validate_closing_tags>(pDoc->pText);

	return pDoc;
}

MF_API void MFParseXML_DestroyDocument(MFDocumentXML *pDocument)
{
	// delete buffer
	if(pDocument)
	{
		// destroy the xml object
		pDocument->doc.clear();
		pDocument->doc.~xml_document<>();

		if(pDocument->bOwnsBuffer)
			MFHeap_Free(pDocument->pText);
		MFHeap_Free(pDocument);
	}
}

MF_API MFXMLNode *MFParseXML_RootNode(MFDocumentXML *pDocument, const char *pName)
{
	xml_node<> *pNode = pDocument->doc.first_node(pName, 0, false);
	return (MFXMLNode*)pNode;
}

MF_API MFXMLNode *MFParseXML_FirstChild(MFXMLNode *_pNode, const char *pName)
{
	xml_node<> *pNode = (xml_node<>*)_pNode;
	return (MFXMLNode*)pNode->first_node(pName, 0, false);
}

MF_API MFXMLNode *MFParseXML_NextSibling(MFXMLNode *_pNode, const char *pName)
{
	xml_node<> *pNode = (xml_node<>*)_pNode;
	return (MFXMLNode*)pNode->next_sibling(pName, 0, false);
}

MF_API const char *MFParseXML_NodeName(MFXMLNode *_pNode)
{
	xml_node<> *pNode = (xml_node<>*)_pNode;
	return pNode->name();
}

MF_API const char *MFParseXML_NodeValue(MFXMLNode *_pNode)
{
	xml_node<> *pNode = (xml_node<>*)_pNode;
	return pNode->value();
}

MF_API MFXMLAttribute *MFParseXML_FirstAttribute(MFXMLNode *_pNode, const char *pName)
{
	xml_node<> *pNode = (xml_node<>*)_pNode;
	return (MFXMLAttribute*)pNode->first_attribute(pName, 0, false);
}

MF_API MFXMLAttribute *MFParseXML_NextAttribute(MFXMLAttribute *_pAttribute, const char *pName)
{
	xml_attribute<> *pAttribute = (xml_attribute<>*)_pAttribute;
	return (MFXMLAttribute*)pAttribute->next_attribute(pName, 0, false);
}

MF_API const char *MFParseXML_AttributeName(MFXMLAttribute *_pAttribute)
{
	xml_attribute<> *pAttribute = (xml_attribute<>*)_pAttribute;
	return pAttribute->name();
}

MF_API const char *MFParseXML_AttributeValue(MFXMLAttribute *_pAttribute)
{
	xml_attribute<> *pAttribute = (xml_attribute<>*)_pAttribute;
	return pAttribute->value();
}
