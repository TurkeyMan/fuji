#include "Fuji.h"
#include "MFScript.h"
#include "MFTranslation.h"

#include "angelscript.h"
#include "scriptstring.h"

extern asIScriptEngine *pEngine;

// wapper functions

void ConstructMFVector(MFVector *pVector)
{
	*pVector = MFVector::zero;
}

void ConstructMFMatrix(MFMatrix *pMat)
{
	*pMat = MFMatrix::identity;
}

MFVector MakeVec1(float f)
{
	MFVector t;
	t.Set(f, f, f, f);
	return t;
}

MFVector MakeVec2(float x, float y)
{
	MFVector t;
	t.Set(x, y, 0.0f, 1.0f);
	return t;
}

MFVector MakeVec3(float x, float y, float z)
{
	MFVector t;
	t.Set(x, y, z, 1.0f);
	return t;
}

MFVector MakeVec4(float x, float y, float z, float w)
{
	MFVector t;
	t.Set(x, y, z, w);
	return t;
}

MFStringTable* LoadStringTable(MFScriptString name, MFLanguage language, MFLanguage fallback)
{
	return MFTranslation_LoadStringTable(MFScript_GetCString(name), language, fallback);
}

MFScriptString GetLanguageName(MFLanguage language, bool native)
{
	return MFScript_MakeScriptString(MFTranslation_GetLanguageName(language, native));
}

MFScriptString GetString(MFStringTable *pTable, int stringID)
{
	return MFScript_MakeScriptString(MFTranslation_GetString(pTable, stringID));
}

void RegisterInternalTypes()
{
	pEngine->RegisterObjectType("vector", sizeof(MFVector), asOBJ_CLASS | asOBJ_CLASS_CONSTRUCTOR | asOBJ_CLASS_ASSIGNMENT);
	pEngine->RegisterObjectProperty("vector", "float x", offsetof(MFVector, x));
	pEngine->RegisterObjectProperty("vector", "float y", offsetof(MFVector, y));
	pEngine->RegisterObjectProperty("vector", "float z", offsetof(MFVector, z));
	pEngine->RegisterObjectProperty("vector", "float w", offsetof(MFVector, w));
	pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMFVector), asCALL_CDECL_OBJLAST);
	pEngine->RegisterObjectBehaviour("vector", asBEHAVE_ASSIGNMENT, "vector &f(const vector &in)", asMETHOD(MFVector, operator=), asCALL_THISCALL);

	pEngine->RegisterGlobalFunction("vector Vector(float)", asFUNCTION(MakeVec1), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("vector Vector(float, float)", asFUNCTION(MakeVec2), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("vector Vector(float, float, float)", asFUNCTION(MakeVec3), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("vector Vector(float, float, float, float)", asFUNCTION(MakeVec4), asCALL_CDECL);

	pEngine->RegisterObjectType("matrix", sizeof(MFMatrix), asOBJ_CLASS | asOBJ_CLASS_ASSIGNMENT);
	pEngine->RegisterObjectProperty("matrix", "vector xaxis", 0);
	pEngine->RegisterObjectProperty("matrix", "vector yaxis", 16);
	pEngine->RegisterObjectProperty("matrix", "vector zaxis", 32);
	pEngine->RegisterObjectProperty("matrix", "vector translation", 48);
	pEngine->RegisterObjectBehaviour("matrix", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMFMatrix), asCALL_CDECL_OBJLAST);
	pEngine->RegisterObjectBehaviour("matrix", asBEHAVE_ASSIGNMENT, "matrix &f(const matrix &in)", asMETHOD(MFMatrix, operator=), asCALL_THISCALL);
}

void RegisterInternalFunctions()
{
	pEngine->RegisterGlobalFunction("string& GetLanguageName(int, bool)", asFUNCTION(GetLanguageName), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("int GetDefaultLanguage()", asFUNCTION(MFTranslation_GetDefaultLanguage), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("uint LoadStringTable(const string &in, int, int)", asFUNCTION(LoadStringTable), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("void DestroyStringTable(uint)", asFUNCTION(MFTranslation_DestroyStringTable), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("int GetNumStrings(uint)", asFUNCTION(MFTranslation_GetNumStrings), asCALL_CDECL);
	pEngine->RegisterGlobalFunction("string& GetStrings(uint, int)", asFUNCTION(GetString), asCALL_CDECL);
}
