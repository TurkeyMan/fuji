#include "Fuji.h"

#if defined(_USE_ANGEL_SCRIPT)

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
	pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);	
	MFDebug_Assert(pEngine, "Failed to create script engine.");

#if !defined(_RETAIL)
	pEngine->SetCommonMessageStream((asOUTPUTFUNC_t)MFDebug_Message, 0);
#endif

	RegisterScriptString(pEngine);

	RegisterInternalTypes();
	RegisterInternalFunctions();

	MFMountDataNative native;
	native.cbSize = sizeof(MFMountDataNative);
	native.flags = MFMF_FlattenDirectoryStructure | MFMF_Recursive;
	native.pMountpoint = "script";
	native.priority = MFMP_AboveNormal;
	native.pPath = MFFile_SystemPath("script/");

	MFFileSystem_Mount(MFFileSystem_GetInternalFileSystemHandle(MFFSH_NativeFileSystem), &native);
}

void MFScript_DeinitModule()
{

}

MFScript* MFScript_LoadScript(const char *pFilename)
{
	MFFile *pFile = MFFileSystem_Open(MFStr("%s.as", pFilename));
	int len = MFFile_GetSize(pFile);

	if(!len)
	{
		MFDebug_Warn(2, MFStr("Failed to load script '%s'.", pFilename));
		return NULL;
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
		return NULL;
	}

	return (MFScript*)pEngine->CreateContext();
}

MFEntryPoint MFScript_FindPublicFunction(MFScript *pScript, const char *pFunctionName)
{
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

int MFScript_Execute(MFScript *pScript, const char *pEntryPoint)
{
	asIScriptContext *pContext = (asIScriptContext*)pScript;

	if(!pEntryPoint)
		pEntryPoint = "main";

	MFEntryPoint entryPoint = MFScript_FindPublicFunction(NULL, pEntryPoint);

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

int MFScript_ExecuteImmediate(MFScript *pScript, const char *pCode)
{
	return pEngine->ExecuteString(NULL, pCode, NULL, 0);
}

int MFScript_Call(MFScript *pScript, MFEntryPoint entryPoint)
{
	asIScriptContext *pContext = (asIScriptContext*)pScript;

	pContext->Prepare(entryPoint);
	if(pContext->Execute() < 0)
	{
		MFDebug_Warn(1, MFStr("Failed to execute script."));
		return 0;
	}

	return pContext->GetReturnDWord();
}

void MFScript_DestroyScript(MFScript *pScript)
{
	asIScriptContext *pContext = (asIScriptContext*)pScript;
	pContext->Release();
}

void MFScript_RegisterNativeFunctions(ScriptNativeInfo *pNativeFunctions)
{
	while(pNativeFunctions->pFunc)
	{
		int r = pEngine->RegisterGlobalFunction(pNativeFunctions->pDecl, asFUNCTION(pNativeFunctions->pFunc), asCALL_CDECL);
		MFDebug_Assert(r >= 0, MFStr("Couldnt register function with declaration '%s'", pNativeFunctions->pDecl));

		++pNativeFunctions;
	}
}

const char* MFScript_GetCString(MFScript *pScript, MFScriptString scriptString)
{
	asCScriptString &string = *(asCScriptString*)scriptString;
	return string.buffer.c_str();
}

MFScriptString MFScript_MakeScriptString(MFScript *pScript, const char *pString)
{
	return (MFScriptString)new asCScriptString(pString);
}

const void* MFScript_GetArray(MFScript *pScript, MFScriptArray scriptArray)
{
	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	return array.GetElementPointer(0);
}

int MFScript_GetArraySize(MFScript *pScript, MFScriptArray scriptArray)
{
	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	return array.GetElementCount();
}

const void* MFScript_GetArrayItem(MFScript *pScript, MFScriptArray scriptArray, int item)
{
	asCArrayObject &array = *(asCArrayObject*)scriptArray;
	MFDebug_Assert(item < (int)array.GetElementCount(), "Array index is out of bounds.");
	return array.GetElementPointer(item);
}

#endif // defined(_USE_ANGEL_SCRIPT)
