#include "Fuji.h"

#if defined(_ENABLE_SCRIPTING)

#include "MFHeap.h"
#include "MFScript_Internal.h"
#include "MFFileSystem.h"
#include "FileSystem/MFFileSystemNative.h"

#include "angelscript.h"
#include "scriptstring.h"
#include "../Middleware/angelscript/source/as_arrayobject.h"

#include <stdio.h>

// structure definitions

// globals
asIScriptEngine *pEngine = NULL;


/*** Functions ***/

void MFScript_InitModule()
{
	MFCALLSTACK;

	pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);	
	MFDebug_Assert(pEngine, "Failed to create script engine.");

#if !defined(_RETAIL)
	pEngine->SetCommonMessageStream((asOUTPUTFUNC_t)MFDebug_Message, 0);
#endif

	RegisterScriptString(pEngine);

	RegisterInternalTypes();
	RegisterInternalFunctions();
}

void MFScript_DeinitModule()
{

}

int MFScript_LoadScript(const char *pFilename)
{
	MFCALLSTACK;

	MFFile *pFile = MFFileSystem_Open(MFStr("%s.as", pFilename));
	int len = MFFile_GetSize(pFile);

	if(!len)
	{
		MFDebug_Warn(2, MFStr("Failed to load script '%s'.", pFilename));
		return -1;
	}

	char *pScript = (char*)MFHeap_TAlloc(len+1);

	MFFile_Read(pFile, pScript, len, false);
	pScript[len] = 0;

	int r;

	r = pEngine->AddScriptSection(0, pFilename, pScript, MFString_Length(pScript), 0, false);
	MFDebug_Assert(r >= 0, "Failed to add code section.");

	r = pEngine->Build(NULL);

	MFHeap_Free(pScript);

	if(r < 0)
	{
		MFDebug_Assert(r >= 0, "Failed compiling the script.");
		return r;
	}

	return 0;
}

MFEntryPoint MFScript_FindPublicFunction(const char *pFunctionName)
{
	MFCALLSTACK;

	if(pFunctionName[0] == 0)
		return -1;

	MFEntryPoint entryPoint = pEngine->GetFunctionIDByName(NULL, pFunctionName);

	if(entryPoint < 0)
	{
		MFDebug_Warn(4, MFStr("Public function '%s' was not found in the script.", pFunctionName));
		return MFEntryPoint_Main;
	}

	return entryPoint;
}

MFScriptContext* MFScript_CreateContext()
{
	MFCALLSTACK;

	return (MFScriptContext*)pEngine->CreateContext();
}

int MFScript_Execute(MFScriptContext *_pContext, const char *pEntryPoint)
{
	MFCALLSTACK;

	asIScriptContext *pContext = (asIScriptContext*)_pContext;

	if(!pEntryPoint)
		pEntryPoint = "main";

	MFEntryPoint entryPoint = MFScript_FindPublicFunction(pEntryPoint);

	if(entryPoint >= 0)
	{
		pContext->Prepare(entryPoint);
		if(pContext->Execute() < 0)
		{
			MFDebug_Warn(1, MFStr("Failed to execute script with entry point '%s'", pEntryPoint));
			return 0;
		}
	}

	return pContext->GetReturnDWord();
}

int MFScript_ExecuteImmediate(const char *pCode, MFScriptContext *pContext)
{
	MFCALLSTACK;

	return pEngine->ExecuteString(NULL, pCode, (asIScriptContext**)&pContext, pContext ? asEXECSTRING_USE_MY_CONTEXT : 0);
}

int MFScript_Call(MFScriptContext *_pContext, MFEntryPoint entryPoint)
{
	MFCALLSTACK;

	asIScriptContext *pContext = (asIScriptContext*)_pContext;

	pContext->Prepare(entryPoint);
	if(pContext->Execute() < 0)
	{
		MFDebug_Warn(1, MFStr("Failed to execute script."));
		return 0;
	}

	return pContext->GetReturnDWord();
}

void MFScript_DestroyContext(MFScriptContext *_pContext)
{
	MFCALLSTACK;

	asIScriptContext *pContext = (asIScriptContext*)_pContext;
	pContext->Release();
}

void MFScript_RegisterNativeFunction(const char *pDeclaration, void *pFunction, MFScriptCallingConvention callingConvention)
{
	MFCALLSTACK;

	int r = pEngine->RegisterGlobalFunction(pDeclaration, asFUNCTION(pFunction), callingConvention);
	MFDebug_Assert(r >= 0, MFStr("Couldnt register function with declaration '%s'", pDeclaration));
}

const char* MFScript_GetCString(MFScriptString scriptString)
{
	MFCALLSTACK;

	asCScriptString &string = *(asCScriptString*)scriptString;
	return string.buffer.c_str();
}

MFScriptString MFScript_MakeScriptString(const char *pString)
{
	MFCALLSTACK;

	return (MFScriptString)new asCScriptString(pString);
}

const void* MFScript_GetArray(MFScriptArray scriptArray)
{
	MFCALLSTACK;

	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	return array.GetElementPointer(0);
}

int MFScript_GetArraySize(MFScriptArray scriptArray)
{
	MFCALLSTACK;

	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	return array.GetElementCount();
}

const void* MFScript_GetArrayItem(MFScriptArray scriptArray, int item)
{
	MFCALLSTACK;

	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	MFDebug_Assert(item < (int)array.GetElementCount(), "Array index is out of bounds.");
	return array.GetElementPointer(item);
}

int MFScript_AddRef(MFScriptObject *pObject)
{
	MFCALLSTACK;

	asIScriptAny &any = *(asIScriptAny*)pObject;
	return any.AddRef();
}

int MFScript_Release(MFScriptObject *pObject)
{
	MFCALLSTACK;

	asIScriptAny &any = *(asIScriptAny*)pObject;
	return any.Release();
}

#endif // _ENABLE_SCRIPTING
