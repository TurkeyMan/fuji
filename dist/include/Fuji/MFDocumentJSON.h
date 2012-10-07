#if !defined(_MFPARSEJSON_H)
#define _MFPARSEJSON_H

enum MFJSONType
{
	MFJT_NullType = 0,		//!< null
	MFJT_FalseType = 1,		//!< false
	MFJT_TrueType = 2,		//!< true
	MFJT_ObjectType = 3,	//!< object
	MFJT_ArrayType = 4,		//!< array
	MFJT_StringType = 5,	//!< string
	MFJT_NumberType = 6,	//!< number
};

struct MFDocumentJSON;

class MFJSONValue
{
public:
	inline MFJSONType Type();

	inline bool IsNull();
	inline bool Bool();
	inline int Int();
	inline float Float();
	inline const char *String();

	inline size_t Length();
	inline MFJSONValue *At(size_t index);

	inline MFJSONValue *Member(const char *pName);
};

MFDocumentJSON *MFParseJSON_Parse(char *pDocument, bool bCopyBuffer = true);
MFDocumentJSON *MFParseJSON_ParseFile(const char *pFilename);
void MFParseJSON_DestroyDocument(MFDocumentJSON *pDocument);

MFJSONValue *MFParseJSON_Root(MFDocumentJSON *pDocument);

MFJSONType MFParseJSON_Type(MFJSONValue *pValue);
bool MFParseJSON_IsNull(MFJSONValue *pValue);
bool MFParseJSON_GetBool(MFJSONValue *pValue);
int MFParseJSON_GetInt(MFJSONValue *pValue);
float MFParseJSON_GetFloat(MFJSONValue *pValue);
const char *MFParseJSON_GetString(MFJSONValue *pValue);

size_t MFParseJSON_GetLength(MFJSONValue *pValue);
MFJSONValue *MFParseJSON_GetElement(MFJSONValue *pValue, size_t index);

MFJSONValue *MFParseJSON_GetMember(MFJSONValue *pValue, const char *pName);


// C++ interface (for convenience)
inline MFJSONType MFJSONValue::Type()
{
	return MFParseJSON_Type(this);
}

inline bool MFJSONValue::IsNull()
{
	return MFParseJSON_IsNull(this);
}

inline bool MFJSONValue::Bool()
{
	return MFParseJSON_GetBool(this);
}

inline int MFJSONValue::Int()
{
	return MFParseJSON_GetInt(this);
}

inline float MFJSONValue::Float()
{
	return MFParseJSON_GetFloat(this);
}

inline const char *MFJSONValue::String()
{
	return MFParseJSON_GetString(this);
}

inline size_t MFJSONValue::Length()
{
	return MFParseJSON_GetLength(this);
}

inline MFJSONValue *MFJSONValue::At(size_t index)
{
	return MFParseJSON_GetElement(this, index);
}

inline MFJSONValue *MFJSONValue::Member(const char *pName)
{
	return MFParseJSON_GetMember(this, pName);
}

#endif // _MFPARSEJSON_H
