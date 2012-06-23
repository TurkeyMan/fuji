#include "Fuji.h"
#include "MFHeap.h"
#include "MFFileSystem.h"
#include "MFDocumentJSON.h"

#define RAPIDJSON_NO_INT64DEFINE
typedef int64 int64_t;
typedef uint64 uint64_t;

#if defined(MF_ENDIAN_BIG)
	#define RAPIDJSON_ENDIAN 1 // RAPIDJSON_BIGENDIAN
#else
	#define RAPIDJSON_ENDIAN 0 // RAPIDJSON_LITTLEENDIAN
#endif

// do we want to include x86 here?
#if defined(MF_ARCH_X64) // x64 always has at least SSE2
	#define RAPIDJSON_SSE2
#endif

#define RAPIDJSON_ASSERT(x) MFDebug_Assert(x, "JSON parse assert failed")

class FujiAllocator {
public:
	static const bool kNeedFree = true;
	void* Malloc(size_t size) { return MFHeap_Alloc(size); }
	void* Realloc(void* originalPtr, size_t originalSize, size_t newSize) { return MFHeap_Realloc(originalPtr, newSize); }
	static void Free(void *ptr) { MFHeap_Free(ptr); }
};

// include the rapidjson templates, using the options defined above
#include "Middleware/rapidjson-0.1/document.h"
using namespace rapidjson;

struct MFDocumentJSON
{
	char *pText;
	bool bOwnsBuffer;

	Document doc;
};

MFDocumentJSON *MFParseJSON_ParseFile(const char *pFilename)
{
	char *pBuffer = MFFileSystem_Load(pFilename);
	if(!pBuffer)
		return NULL;

	MFDocumentJSON *pDoc = MFParseJSON_Parse(pBuffer, false);
	pDoc->bOwnsBuffer = true;

	return pDoc;
}

MFDocumentJSON *MFParseJSON_Parse(char *pDocument, bool bCopyBuffer)
{
	MFDocumentJSON *pDoc = (MFDocumentJSON*)MFHeap_Alloc(sizeof(MFDocumentJSON));

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

	// load json
	new(&pDoc->doc) Document;
	pDoc->doc.ParseInsitu<kParseInsituFlag>(pDoc->pText);

	if(pDoc->doc.HasParseError())
	{
		MFDebug_Warn(1, pDoc->doc.GetParseError());
		MFParseJSON_DestroyDocument(pDoc);
		return NULL;
	}

	return pDoc;
}

void MFParseJSON_DestroyDocument(MFDocumentJSON *pDocument)
{
	// delete buffer
	if(pDocument)
	{
		// destroy the JSON object
		pDocument->doc.~Document();

		if(pDocument->bOwnsBuffer)
			MFHeap_Free(pDocument->pText);
		MFHeap_Free(pDocument);
	}
}

MFJSONValue *MFParseJSON_Root(MFDocumentJSON *pDocument)
{
	return (MFJSONValue*)&pDocument->doc;
}

MFJSONType MFParseJSON_Type(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return (MFJSONType)pValue->GetType();
}

bool MFParseJSON_IsNull(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return pValue->GetType() == kNullType;
}

bool MFParseJSON_GetBool(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return pValue->GetType() == kTrueType ? true : false;
}

int MFParseJSON_GetInt(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return pValue->GetInt();
}

float MFParseJSON_GetFloat(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return (float)pValue->GetDouble();
}

const char *MFParseJSON_GetString(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return pValue->GetString();
}

size_t MFParseJSON_GetLength(MFJSONValue *_pValue)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return pValue->Size();
}

MFJSONValue *MFParseJSON_GetElement(MFJSONValue *_pValue, size_t index)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return (MFJSONValue*)&(*pValue)[(SizeType)index];
}

MFJSONValue *MFParseJSON_GetMember(MFJSONValue *_pValue, const char *pName)
{
	Document::ValueType *pValue = (Document::ValueType*)_pValue;
	return (MFJSONValue*)&(*pValue)[pName];
}
