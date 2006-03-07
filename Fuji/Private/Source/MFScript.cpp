#include "Fuji.h"

#if defined(_USE_ANGEL_SCRIPT)

#include "MFHeap.h"
#include "MFScript.h"
#include "MFFileSystem.h"

#include "FileSystem/MFFileSystemNative.h"

#include "angelscript.h"
#include "scriptstring.h"

#include <stdio.h>

// structure definitions

// globals
static asIScriptEngine *pEngine = NULL;


/*** Functions ***/

void MFScript_InitModule()
{
	pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);	
	MFDebug_Assert(pEngine, "Failed to create script engine.");

	pEngine->SetCommonMessageStream((asOUTPUTFUNC_t)MFDebug_Message, 0);

	RegisterScriptString(pEngine);

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
	const char *pScript = MFFileSystem_Load(MFStr("%s.as", pFilename));

	if(!pScript)
	{
		MFDebug_Warn(2, MFStr("Failed to load script '%s'.", pFilename));
		return NULL;
	}

	pEngine->AddScriptSection(0, pFilename, pScript, MFString_Length(pScript), 0, false);
	pEngine->Build(NULL);

	return (MFScript*)pEngine->CreateContext();
}

MFEntryPoint MFScript_FindPublicFunction(MFScript *pScript, const char *pFunctionName)
{
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
		int r = pEngine->RegisterGlobalFunction(pNativeFunctions->pName, asFUNCTION(pNativeFunctions->pFunc), asCALL_CDECL);
		MFDebug_Assert(r >= 0, MFStr("Couldnt register function with declaration '%s'", pNativeFunctions->pName));

		++pNativeFunctions;
	}
}

#endif // defined(_USE_ANGEL_SCRIPT)
