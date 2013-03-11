#include "Fuji.h"

#if defined(_ENABLE_SCRIPTING)

#include "MFScript.h"
#include "MFTranslation.h"
#include "MFModel.h"
#include "MFAnimation.h"
#include "MFMaterial.h"
#include "MFTexture.h"
#include "MFFont.h"

#include "angelscript.h"
#include "scriptstring.h"

extern asIScriptEngine *pEngine;

typedef void (*funcPTR)();

// wrapper functions

static void ConstructString(asCScriptString *self, const char *pCString)
{
	new(self) asCScriptString(pCString);
}

static void ConstructMFVector(MFVector *pVector)
{
	*pVector = MFVector::zero;
}

static void ConstructMFMatrix(MFMatrix *pMat)
{
	*pMat = MFMatrix::identity;
}

static void ConstructMFQuaternion(MFQuaternion *pQuat)
{
	*pQuat = MFQuaternion::identity;
}

static void ConstructVec1(MFVector *pVector, float f)
{
	pVector->Set(f, f, f, f);
}

static void ConstructVec2(MFVector *pVector, float x, float y)
{
	pVector->Set(x, y);
}

static void ConstructVec3(MFVector *pVector, float x, float y, float z)
{
	pVector->Set(x, y, z);
}

/*
static char* &SetCString(char* &self, const char* other)
{
	self = (char*)other;
	return self;
}
*/
static char* &SetCStringToASString(char* &self, MFScriptString other)
{
	self = (char*)MFScript_GetCString(other);
	return self;
}

static MFScriptString SetASStringToCString(MFScriptString self, char* other)
{
	MFScript_SetString(self, other);
	return self;
}

static void ScriptAssert(asIScriptGeneric *gen)
{
#if !defined(_RETAIL)
	if(!gen->GetArgDWord(0))
	{
		const char *pFunc;

		asIScriptContext *c = asGetActiveContext();
		int f = c->GetCurrentFunction();
		pFunc = gen->GetEngine()->GetFunctionName(f);

		MFDebug_DebugAssert("Script Assert", MFScript_GetCString((MFScriptString)gen->GetArgObject(1)), pFunc, c->GetCurrentLineNumber());
		MFDebug_Breakpoint();
	}
#endif
}
static void ScriptMessage(MFScriptString message)
{
	MFDebug_Message(MFScript_GetCString(message));
}
static void ScriptLog(int level, MFScriptString message)
{
	MFDebug_Log(level, MFScript_GetCString(message));
}
static void ScriptWarn(int level, MFScriptString message)
{
	MFDebug_Warn(level, MFScript_GetCString(message));
}
static void ScriptError(MFScriptString message)
{
	MFDebug_Error(MFScript_GetCString(message));
}
static void ScriptBreak()
{
	MFDebug_Breakpoint();
}

static void DummyFunc() { }

// MFVector wrappers :(
static MFVector& OpDivEq(MFVector &v1, const MFVector &v2) { v1.x /= v2.x; v1.y /= v2.y; v1.z /= v2.z; v1.w /= v2.w; return v1; }
static MFVector& OpDivEqF(MFVector &v, float f) { return v *= MFRcp(f); }
static MFVector OpAdd(const MFVector &v1, const MFVector &v2) { return v1 + v2; }
static MFVector OpSub(const MFVector &v1, const MFVector &v2) { return v1 - v2; }
static MFVector OpMul(const MFVector &v1, const MFVector &v2) { return v1 * v2; }
static MFVector OpMulF(const MFVector &v1, float f) { return v1 * f; }
static MFVector OpDiv(MFVector &v1, const MFVector &v2) { MFVector t; t.x = v1.x / v2.x; t.y = v1.y / v2.y; t.z = v1.z / v2.z; t.w = v1.w / v2.w; return t; }
static MFVector OpDivF(MFVector &v, float f) { return v * MFRcp(f); }
static bool OpEq(const MFVector &v1, const MFVector &v2) { return v1 == v2; }
static bool OpNEq(const MFVector &v1, const MFVector &v2) { return v1 != v2; }
static float OpIndex(const MFVector &v, int i) { return v[i]; }

void RegisterInternalTypes()
{
	int r;

	// used for marshalling strings between code and script
	r = pEngine->RegisterObjectType("cstring", sizeof(char*), asOBJ_PRIMITIVE); MFDebug_Assert(r >= 0, "Failed!");
//	r = pEngine->RegisterObjectBehaviour("cstring",asBEHAVE_ASSIGNMENT, "cstring &f(cstring)", asFUNCTION(SetCString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("cstring", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(SetCStringToASString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("cstring",asBEHAVE_ASSIGNMENT, "cstring &f(const string &in)", asFUNCTION(SetCStringToASString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("string",asBEHAVE_CONSTRUCT, "void f(cstring)", asFUNCTION(ConstructString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("string",asBEHAVE_ASSIGNMENT, "string &f(cstring)", asFUNCTION(SetASStringToCString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");

	r = pEngine->RegisterObjectType("vector", sizeof(MFVector), asOBJ_CLASS | asOBJ_CLASS_CONSTRUCTOR | asOBJ_CLASS_ASSIGNMENT); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("vector", "float x", offsetof(MFVector, x)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("vector", "float y", offsetof(MFVector, y)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("vector", "float z", offsetof(MFVector, z)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("vector", "float w", offsetof(MFVector, w)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const vector zero_vector", (void*)&MFVector::zero); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const vector one_vector", (void*)&MFVector::one); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const vector identity_vector", (void*)&MFVector::identity); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const vector up_vector", (void*)&MFVector::up); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMFVector), asCALL_CDECL_OBJLAST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(const vector &in)", asMETHOD(MFVector, operator=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(ConstructVec1), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(ConstructVec2), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(ConstructVec3), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asMETHOD(MFVector, Set), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_ASSIGNMENT, "vector &f(const vector &in)", asMETHOD(MFVector, operator=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_ADD_ASSIGN, "vector &f(const vector &in)", asMETHOD(MFVector, operator+=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_SUB_ASSIGN, "vector &f(const vector &in)", asMETHOD(MFVector, operator-=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_MUL_ASSIGN, "vector &f(float)", asMETHODPR(MFVector, operator*=, (const MFVector&), MFVector&), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_MUL_ASSIGN, "vector &f(const vector &in)", asMETHODPR(MFVector, operator*=, (const MFVector&), MFVector&), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_DIV_ASSIGN, "vector &f(float)", asFUNCTION(OpDivEqF), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_DIV_ASSIGN, "vector &f(const vector &in)", asFUNCTION(OpDivEq), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_ADD, "vector f(const vector &in, const vector &in)", asFUNCTION(OpAdd), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_SUBTRACT, "vector f(const vector &in, const vector &in)", asFUNCTION(OpSub), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_MULTIPLY, "vector f(const vector &in, float)", asFUNCTION(OpMulF), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_MULTIPLY, "vector f(float, const vector &in)", asFUNCTIONPR(operator*, (float f, const MFVector&), MFVector), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_MULTIPLY, "vector f(const vector &in, const vector &in)", asFUNCTION(OpMul), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_DIVIDE, "vector f(const vector &in, float)", asFUNCTION(OpDivF), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_DIVIDE, "vector f(const vector &in, const vector &in)", asFUNCTION(OpDiv), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_EQUAL, "bool f(const vector &in, const vector &in)", asFUNCTION(OpEq), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalBehaviour(asBEHAVE_NOTEQUAL, "bool f(const vector &in, const vector &in)", asFUNCTION(OpNEq), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_INDEX, "float &f(uint)", asFUNCTION(OpIndex), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_NEGATE, "vector f()", asMETHODPR(MFVector, operator-, () const, MFVector), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Dot2(const vector &in) const", asMETHOD(MFVector, Dot2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Dot3(const vector &in) const", asMETHOD(MFVector, Dot3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float DotH(const vector &in) const", asMETHOD(MFVector, DotH), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Dot4(const vector &in) const", asMETHOD(MFVector, Dot4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Cross2(const vector &in) const", asMETHOD(MFVector, Cross2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector Cross3(const vector &in) const", asMETHODPR(MFVector, Cross3, (const MFVector&) const, MFVector), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Cross3(const vector &in, const vector &in)", asMETHODPR(MFVector, Cross3, (const MFVector&, const MFVector&), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float MagSquared2() const", asMETHOD(MFVector, MagSquared2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float MagSquared3() const", asMETHOD(MFVector, MagSquared3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float MagSquared4() const", asMETHOD(MFVector, MagSquared4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Magnitude2() const", asMETHOD(MFVector, Magnitude2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Magnitude3() const", asMETHOD(MFVector, Magnitude3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float Magnitude4() const", asMETHOD(MFVector, Magnitude4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float InvMagnitude2() const", asMETHOD(MFVector, InvMagnitude2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float InvMagnitude3() const", asMETHOD(MFVector, InvMagnitude3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "float InvMagnitude4() const", asMETHOD(MFVector, InvMagnitude4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise2()", asMETHODPR(MFVector, Normalise2, (), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise3()", asMETHODPR(MFVector, Normalise3, (), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise4()", asMETHODPR(MFVector, Normalise4, (), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise2(const vector &in)", asMETHODPR(MFVector, Normalise2, (const MFVector&), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise3(const vector &in)", asMETHODPR(MFVector, Normalise3, (const MFVector&), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "vector &Normalise4(const vector &in)", asMETHODPR(MFVector, Normalise4, (const MFVector&), MFVector&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "cstring ToString2() const", asMETHOD(MFVector, ToString2), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "cstring ToString3() const", asMETHOD(MFVector, ToString3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("vector", "cstring ToString4() const", asMETHOD(MFVector, ToString4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");

	r = pEngine->RegisterObjectType("quaternion", sizeof(MFQuaternion), asOBJ_CLASS | asOBJ_CLASS_CONSTRUCTOR | asOBJ_CLASS_ASSIGNMENT); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("quaternion", "float x", offsetof(MFQuaternion, x)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("quaternion", "float y", offsetof(MFQuaternion, y)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("quaternion", "float z", offsetof(MFQuaternion, z)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("quaternion", "float w", offsetof(MFQuaternion, w)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const quaternion identity_quaternion", (void*)&MFQuaternion::identity); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMFQuaternion), asCALL_CDECL_OBJLAST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(const vector &in)", asMETHOD(MFQuaternion, operator=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("vector", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asMETHOD(MFQuaternion, Set), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	// TODO: Add the rest...

	r = pEngine->RegisterObjectType("matrix", sizeof(MFMatrix), asOBJ_CLASS | asOBJ_CLASS_ASSIGNMENT); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("matrix", "vector xaxis", 0); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("matrix", "vector yaxis", 16); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("matrix", "vector zaxis", 32); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("matrix", "vector translation", 48); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalProperty("const matrix identity_matrix", (void*)&MFMatrix::identity); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("matrix", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructMFMatrix), asCALL_CDECL_OBJLAST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectBehaviour("matrix", asBEHAVE_ASSIGNMENT, "matrix &f(const matrix &in)", asMETHOD(MFMatrix, operator=), asCALL_THISCALL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetIdentity()", asMETHOD(MFMatrix, SetIdentity), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "const vector &GetXAxis() const", asMETHOD(MFMatrix, GetXAxis), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "const vector &GetYAxis() const", asMETHOD(MFMatrix, GetYAxis), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "const vector &GetZAxis() const", asMETHOD(MFMatrix, GetZAxis), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "const vector &GetTrans() const", asMETHOD(MFMatrix, GetTrans), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetXAxis3(const vector &in)", asMETHOD(MFMatrix, SetXAxis3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetYAxis3(const vector &in)", asMETHOD(MFMatrix, SetYAxis3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetZAxis3(const vector &in)", asMETHOD(MFMatrix, SetZAxis3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetTrans3(const vector &in)", asMETHOD(MFMatrix, SetTrans3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetXAxis4(const vector &in)", asMETHOD(MFMatrix, SetXAxis4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetYAxis4(const vector &in)", asMETHOD(MFMatrix, SetYAxis4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetZAxis4(const vector &in)", asMETHOD(MFMatrix, SetZAxis4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "void SetTrans4(const vector &in)", asMETHOD(MFMatrix, SetTrans4), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Translate(const vector &in)", asMETHOD(MFMatrix, Translate), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Rotate(const vector &in, float)", asMETHOD(MFMatrix, Rotate), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &RotateQ(const quaternion &in)", asMETHOD(MFMatrix, RotateQ), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &RotateYPR(float, float, float)", asMETHOD(MFMatrix, RotateYPR), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &RotateX(float)", asMETHOD(MFMatrix, RotateX), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &RotateY(float)", asMETHOD(MFMatrix, RotateY), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &RotateZ(float)", asMETHOD(MFMatrix, RotateZ), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Scale(const vector &in)", asMETHOD(MFMatrix, Scale), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetTranslation(const vector &in)", asMETHOD(MFMatrix, SetTranslation), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotation(const vector &in, float)", asMETHOD(MFMatrix, SetRotation), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotationQ(const quaternion &in)", asMETHOD(MFMatrix, SetRotationQ), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotationYPR(float, float, float)", asMETHOD(MFMatrix, SetRotationYPR), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotationX(float)", asMETHOD(MFMatrix, SetRotationX), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotationY(float)", asMETHOD(MFMatrix, SetRotationY), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetRotationZ(float)", asMETHOD(MFMatrix, SetRotationZ), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &SetScale(const vector &in)", asMETHOD(MFMatrix, SetScale), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &LookAt(const quaternion &in, const quaternion &in, const quaternion &in)", asMETHOD(MFMatrix, LookAt), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "quaternion GetRotationQ() const", asMETHOD(MFMatrix, GetRotationQ), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Normalise()", asMETHOD(MFMatrix, Normalise), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &OrthoNormalise()", asMETHOD(MFMatrix, OrthoNormalise), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Transpose()", asMETHODPR(MFMatrix, Transpose, (), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Transpose(const matrix &in)", asMETHODPR(MFMatrix, Transpose, (const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Transpose3x3()", asMETHODPR(MFMatrix, Transpose3x3, (), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Transpose3x3(const matrix &in)", asMETHODPR(MFMatrix, Transpose3x3, (const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply()", asMETHODPR(MFMatrix, Multiply, (const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply()", asMETHODPR(MFMatrix, Multiply, (const MFMatrix&, const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply4x4()", asMETHODPR(MFMatrix, Multiply4x4, (const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply4x4()", asMETHODPR(MFMatrix, Multiply4x4, (const MFMatrix&, const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply3x3()", asMETHODPR(MFMatrix, Multiply3x3, (const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Multiply3x3()", asMETHODPR(MFMatrix, Multiply3x3, (const MFMatrix&, const MFMatrix&), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "vector TransformVector(const vector &in) const", asMETHOD(MFMatrix, TransformVector), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "vector TransformVectorH(const vector &in) const", asMETHOD(MFMatrix, TransformVectorH), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "vector TransformVector3(const vector &in) const", asMETHOD(MFMatrix, TransformVector3), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Inverse()", asMETHODPR(MFMatrix, Inverse, (), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Inverse(const matrix &in)", asMETHODPR(MFMatrix, Inverse, (const MFMatrix &), MFMatrix&), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &Tween(const matrix &in, const matrix &in, float)", asMETHOD(MFMatrix, Tween), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &PreciseTween(const matrix &in, const matrix &in, float)", asMETHOD(MFMatrix, PreciseTween), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "matrix &ClearW()", asMETHOD(MFMatrix, ClearW), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("matrix", "cstring ToString() const", asMETHOD(MFMatrix, ToString), asCALL_THISCALL);  MFDebug_Assert(r >= 0, "Failed!");

	// register MFBoundingVolume
	r = pEngine->RegisterObjectType("MFBoundingVolume", sizeof(MFBoundingVolume), asOBJ_CLASS_CDA); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("MFBoundingVolume", "vector boundingSphere", offsetof(MFBoundingVolume, boundingSphere)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("MFBoundingVolume", "vector min", offsetof(MFBoundingVolume, min)); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectProperty("MFBoundingVolume", "vector max", offsetof(MFBoundingVolume, max)); MFDebug_Assert(r >= 0, "Failed!");
}

void RegisterInternalFunctions()
{
	int r;

	// Debug functions
	r = pEngine->RegisterGlobalFunction("void Assert(bool, const string &in)", asFUNCTION(ScriptAssert), asCALL_GENERIC); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("int Message(const string &in)", asFUNCTION(ScriptMessage), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("int Log(int, const string &in)", asFUNCTION(ScriptLog), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("int Warn(int, const string &in)", asFUNCTION(ScriptWarn), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("int Error(const string &in)", asFUNCTION(ScriptError), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("void Breakpoint()", asFUNCTION(ScriptBreak), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");

#define REGISTER_CLASSHANDLE(x) \
	pEngine->RegisterObjectType(#x, 0, asOBJ_CLASS_CDA);\
	pEngine->RegisterObjectBehaviour(#x, asBEHAVE_ADDREF, "void f()", asFUNCTION(DummyFunc), asCALL_CDECL_OBJLAST);\
	pEngine->RegisterObjectBehaviour(#x, asBEHAVE_RELEASE, "void f()", asFUNCTION(DummyFunc), asCALL_CDECL_OBJLAST);

	// register Classes
	REGISTER_CLASSHANDLE(MFMeshChunk)
	REGISTER_CLASSHANDLE(MFFont)
	REGISTER_CLASSHANDLE(MFModel)
	REGISTER_CLASSHANDLE(MFAnimation)
	REGISTER_CLASSHANDLE(MFStringTable)

	// register MFFont functions
	r = pEngine->RegisterGlobalFunction("MFFont @MFFont_Create(cstring fontName)", asFUNCTION(MFFont_Create), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("MFFont @MFFont_GetDebugFont()", asFUNCTION(MFFont_GetDebugFont), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "void Destroy()", asFUNCTION(MFFont_Destroy), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "float GetFontHeight()", asFUNCTION(MFFont_GetFontHeight), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "float GetCharacterWidth(int)", asFUNCTION(MFFont_GetCharacterWidth), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "vector GetCharPos(cstring, int, float)", asFUNCTION(MFFont_GetCharPos), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "float GetStringWidth(cstring, float, float, int, float &out)", asFUNCTION(MFFont_GetStringWidth), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "float DrawText(cstring, const vector &in, float, const vector &in, cstring, int, const matrix &in)", asFUNCTIONPR(MFFont_DrawText, (MFFont*, const MFVector&, float, const MFVector&, const char*, int, const MFMatrix&), float), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFFont", "float DrawText(cstring, float, float, float, const vector &in, cstring, int, const matrix &in)", asFUNCTIONPR(MFFont_DrawText, (MFFont*, float, float, float, const MFVector&, const char*, int, const MFMatrix&), float), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");

/*
int MFFont_BlitText(MFFont *pFont, int x, int y, const MFVector &colour, const char *pText, int maxChars = -1);
int MFFont_BlitTextf(MFFont *pFont, int x, int y, const MFVector &colour, const char *pFormat, ...);
float MFFont_DrawTextf(MFFont *pFont, const MFVector &pos, float height, const MFVector &colour, const char *pFormat, ...);
float MFFont_DrawTextf(MFFont *pFont, float x, float y, float height, const MFVector &colour, const char *pFormat, ...);
float MFFont_DrawTextJustified(MFFont *pFont, const char *pText, const MFVector &pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, const MFVector &colour, int numChars = -1, const MFMatrix &ltw = MFMatrix::identity);
float MFFont_DrawTextAnchored(MFFont *pFont, const char *pText, const MFVector &pos, MFFontJustify justification, float lineWidth, float textHeight, const MFVector &colour, int numChars = -1, const MFMatrix &ltw = MFMatrix::identity);
*/

	// register MFModel functions
	r = pEngine->RegisterGlobalFunction("MFModel @MFModel_Create(cstring modelName)", asFUNCTION(MFModel_Create), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("MFModel @MFModel_CreateWithAnimation(cstring model, cstring animation)", asFUNCTION(MFModel_CreateWithAnimation), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "void Draw()", asFUNCTION(MFModel_Draw), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "void Destroy()", asFUNCTION(MFModel_Destroy), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "void SetWorldMatrix(const matrix &in)", asFUNCTION(MFModel_SetWorldMatrix), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "void SetColour(const vector &in)", asFUNCTION(MFModel_SetColour), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "cstring GetName()", asFUNCTION(MFModel_GetName), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetNumSubObjects()", asFUNCTION(MFModel_GetNumSubObjects), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetSubObjectIndex(cstring)", asFUNCTION(MFModel_GetSubObjectIndex), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "cstring GetSubObjectName(int)", asFUNCTION(MFModel_GetSubObjectName), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "void EnableSubobject(int, bool)", asFUNCTION(MFModel_EnableSubobject), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "bool IsSubobjectEnabed(int)", asFUNCTION(MFModel_IsSubobjectEnabed), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "MFBoundingVolume &GetBoundingVolume()", asFUNCTION(MFModel_GetBoundingVolume), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "MFMeshChunk &GetMeshChunk(int, int)", asFUNCTION(MFModel_GetMeshChunk), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "MFAnimation &GetAnimation()", asFUNCTION(MFModel_GetAnimation), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetNumBones()", asFUNCTION(MFModel_GetNumBones), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "cstring GetBoneName(int)", asFUNCTION(MFModel_GetBoneName), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "const matrix &GetBoneOrigin(int)", asFUNCTION(MFModel_GetBoneOrigin), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetBoneIndex(cstring)", asFUNCTION(MFModel_GetBoneIndex), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetNumTags()", asFUNCTION(MFModel_GetNumTags), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "cstring GetTagName(int)", asFUNCTION(MFModel_GetName), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "const matrix &GetTagMatrix(int)", asFUNCTION(MFModel_GetTagMatrix), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFModel", "int GetTagIndex(cstring)", asFUNCTION(MFModel_GetTagIndex), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");

	// Translation Functions
	r = pEngine->RegisterGlobalFunction("int MFTranslation_GetLanguageName(int, bool)", asFUNCTION(MFTranslation_GetLanguageName), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("cstring MFTranslation_GetDefaultLanguage()", asFUNCTION(MFTranslation_GetDefaultLanguage), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("MFStringTable @MFTranslation_LoadStringTable(cstring, int, int)", asFUNCTION(MFTranslation_LoadStringTable), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterGlobalFunction("MFStringTable @MFTranslation_LoadEnumStringTable(cstring)", asFUNCTION(MFTranslation_LoadEnumStringTable), asCALL_CDECL); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFStringTable", "void DestroyStringTable()", asFUNCTION(MFTranslation_DestroyStringTable), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFStringTable", "int GetNumStrings()", asFUNCTION(MFTranslation_GetNumStrings), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFStringTable", "int FindString(cstring)", asFUNCTION(MFTranslation_FindString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
	r = pEngine->RegisterObjectMethod("MFStringTable", "cstring GetString(int)", asFUNCTION(MFTranslation_GetString), asCALL_CDECL_OBJFIRST); MFDebug_Assert(r >= 0, "Failed!");
}

#endif // _ENABLE_SCRIPTING
