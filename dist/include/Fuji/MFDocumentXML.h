#if !defined(_MFPARSEXML_H)
#define _MFPARSEXML_H

struct MFDocumentXML;
class MFXMLAttribute;

class MFXMLNode
{
public:
	inline const char *Name();
	inline const char *Value();
	inline const char *Attribute(const char *pAttribute);
	inline int IntAttribute(const char *pAttribute);
	inline float FloatAttribute(const char *pAttribute);

	inline MFXMLNode *FirstChild(const char *pName = NULL);
	inline MFXMLNode *NextSibling(const char *pName = NULL);

	inline MFXMLAttribute *FirstAttribute(const char *pName = NULL);
};

class MFXMLAttribute
{
public:
	inline const char *Name();
	inline const char *Value();

	inline MFXMLAttribute *Next(const char *pName = NULL);
};

MF_API MFDocumentXML *MFParseXML_Parse(char *pDocument, bool bCopyBuffer = true);
MF_API MFDocumentXML *MFParseXML_ParseFile(const char *pFilename);
MF_API void MFParseXML_DestroyDocument(MFDocumentXML *pDocument);

MF_API MFXMLNode *MFParseXML_RootNode(MFDocumentXML *pDocument, const char *pName = NULL);
MF_API MFXMLNode *MFParseXML_FirstChild(MFXMLNode *pNode, const char *pName = NULL);
MF_API MFXMLNode *MFParseXML_NextSibling(MFXMLNode *pNode, const char *pName = NULL);
MF_API const char *MFParseXML_NodeName(MFXMLNode *pNode);
MF_API const char *MFParseXML_NodeValue(MFXMLNode *pNode);

MF_API MFXMLAttribute *MFParseXML_FirstAttribute(MFXMLNode *pNode, const char *pName = NULL);
MF_API MFXMLAttribute *MFParseXML_NextAttribute(MFXMLAttribute *pNode, const char *pName = NULL);
MF_API const char *MFParseXML_AttributeName(MFXMLAttribute *pNode);
MF_API const char *MFParseXML_AttributeValue(MFXMLAttribute *pNode);


// C++ interface (for convenience)

inline const char *MFXMLNode::Name()
{
	return MFParseXML_NodeName(this);
}

inline const char *MFXMLNode::Value()
{
	return MFParseXML_NodeValue(this);
}

inline const char *MFXMLNode::Attribute(const char *pAttribute)
{
	MFXMLAttribute *pAttr = MFParseXML_FirstAttribute(this, pAttribute);
	if(pAttr)
		return MFParseXML_AttributeValue(pAttr);
	return NULL;
}

inline int MFXMLNode::IntAttribute(const char *pAttribute)
{
	return MFString_AsciiToInteger(Attribute(pAttribute));
}

inline float MFXMLNode::FloatAttribute(const char *pAttribute)
{
	return MFString_AsciiToFloat(Attribute(pAttribute));
}

inline MFXMLNode *MFXMLNode::FirstChild(const char *pName)
{
	return MFParseXML_FirstChild(this, pName);
}

inline MFXMLNode *MFXMLNode::NextSibling(const char *pName)
{
	return MFParseXML_NextSibling(this, pName);
}

inline MFXMLAttribute *MFXMLNode::FirstAttribute(const char *pName)
{
	return MFParseXML_FirstAttribute(this, pName);
}

inline const char *MFXMLAttribute::Name()
{
	return MFParseXML_AttributeName(this);
}

inline const char *MFXMLAttribute::Value()
{
	return MFParseXML_AttributeValue(this);
}

inline MFXMLAttribute *MFXMLAttribute::Next(const char *pName)
{
	return MFParseXML_NextAttribute(this, pName);
}

#endif // _MFPARSEXML_H
