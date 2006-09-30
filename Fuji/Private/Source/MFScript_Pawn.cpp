#include "Fuji.h"

#if defined(_USE_PAWN_SCRIPT)

#include "MFHeap.h"
#include "MFScript.h"
#include "MFFileSystem.h"
#include "pawn/amx/amx.h"
#include "pawn/amx/amxaux.h"

extern "C" AMX_NATIVE_INFO core_Natives[];
extern "C" AMX_NATIVE_INFO float_Natives[];
extern "C" AMX_NATIVE_INFO string_Natives[];

// structure definitions
struct MFScript
{
	AMX amx;
};

// globals
static const int gMaxNativeFunctionLists = 128;
static ScriptNativeInfo *gNativeFunctionRegistry[gMaxNativeFunctionLists];
static int gNativeFunctionCount = 0;


/*** Functions ***/

void MFScript_InitModule()
{
	
}

void MFScript_DeinitModule()
{

}

int MFScript_LoadProgram(AMX *amx, char *filename, void *memblock)
{
  AMX_HEADER hdr;
  int result, didalloc;

  /* open the file, read and check the header */
  MFFile *pFile = MFFileSystem_Open(filename, MFOF_Read);

  if(!pFile)
    return AMX_ERR_NOTFOUND;

  MFFile_Read(pFile, &hdr, sizeof(hdr));

  amx_Align16(&hdr.magic);
  amx_Align32((uint32_t *)&hdr.size);
  amx_Align32((uint32_t *)&hdr.stp);
  if(hdr.magic != AMX_MAGIC)
  {
    MFFile_Close(pFile);
    return AMX_ERR_FORMAT;
  } /* if */

  /* allocate the memblock if it is NULL */
  didalloc = 0;
  if(memblock == NULL)
  {
    if((memblock = MFHeap_Alloc(hdr.stp)) == NULL)
	{
      MFFile_Close(pFile);
      return AMX_ERR_MEMORY;
    } /* if */
    didalloc = 1;
    /* after amx_Init(), amx->base points to the memory block */
  } /* if */

  /* read in the file */
  MFFile_Seek(pFile, 0, MFSeek_Begin);
  MFFile_Read(pFile, memblock, hdr.size);
  MFFile_Close(pFile);

  /* initialize the abstract machine */
  MFZeroMemory(amx, sizeof(*amx));
  result = amx_Init(amx, memblock);

  /* free the memory block on error, if it was allocated here */
  if(result != AMX_ERR_NONE && didalloc)
  {
    MFHeap_Free(memblock);
    amx->base = NULL;                   /* avoid a double free */
  } /* if */

  return result;
}

MFScript* MFScript_LoadScript(const char *pFilename)
{
	int err;

	const char *pScriptFilename = MFStr("%s.amx", pFilename);

	if(!MFFileSystem_Exists(pScriptFilename))
	{
		const char *pScriptSource = MFStr("%s.p", pFilename);

		if(!MFFileSystem_Exists(pScriptSource))
		{
			// couldnt find the script apparently
			MFDebug_Warn(1, MFStr("Script '%s' does not exist.", pFilename));
			return NULL;
		}

		MFDebug_Warn(4, MFStr("Script source found for script '%s'. Attempting to compile from source.", pFilename));

		// we need to compile and load from source
//		system("pawncc.exe ");
//		MFFileSystem_Load("home:temp.amx", NULL);

		if(0) // failed
		{
			MFDebug_Warn(1, MFStr("Failed to compile script '%s' from source. Cannot load script.", pFilename));
			return NULL;
		}
	}

	MFScript *pScript = (MFScript*)MFHeap_Alloc(sizeof(MFScript));

	err = MFScript_LoadProgram(&pScript->amx, (char*)pScriptFilename, NULL);

	if(err != AMX_ERR_NONE)
	{
		MFHeap_Free(pScript);
		MFDebug_Warn(1, MFStr("Failed to load script '%s'. \"%s\"", pFilename, aux_StrError(err)));
		return NULL;
	}

	amx_Register(&pScript->amx, core_Natives, -1);
	amx_Register(&pScript->amx, float_Natives, -1);
	amx_Register(&pScript->amx, string_Natives, -1);

	// register our internal functions
	for(int a=0; a<gNativeFunctionCount; a++)
	{
		amx_Register(&pScript->amx, (AMX_NATIVE_INFO*)gNativeFunctionRegistry[a], -1);
	}

	return pScript;
}

MFEntryPoint MFScript_FindPublicFunction(MFScript *pScript, const char *pFunctionName)
{
	MFEntryPoint entryPoint;

	if(amx_FindPublic(&pScript->amx, pFunctionName, &entryPoint) == AMX_ERR_NOTFOUND)
	{
		MFDebug_Warn(4, MFStr("Public function '%s' was not found in the script. \"%s\"", pFunctionName, aux_StrError(AMX_ERR_NOTFOUND)));
		return MFEntryPoint_Main;
	}

	return entryPoint;
}

int MFScript_Execute(MFScript *pScript, const char *pEntryPoint)
{
	int entryPoint = AMX_EXEC_MAIN;
	cell returnValue;
	int err;

	if(pEntryPoint)
	{
		if(amx_FindPublic(&pScript->amx, pEntryPoint, &entryPoint) == AMX_ERR_NOTFOUND)
		{
			MFDebug_Warn(3, MFStr("Public function '%s' was not found in the script. \"%s\"", pEntryPoint, aux_StrError(AMX_ERR_NOTFOUND)));
			return 0;
		}
	}

	err = amx_Exec(&pScript->amx, &returnValue, entryPoint);

	if(err)
	{
		MFDebug_Warn(1, MFStr("Failed to execute script with entry point '%s'", pEntryPoint ? pEntryPoint : "main"));
		return 0;
	}

	return returnValue;
}

int MFScript_Call(MFScript *pScript, MFEntryPoint entryPoint)
{
	cell returnValue;
	int err;

	err = amx_Exec(&pScript->amx, &returnValue, entryPoint);

	if(err)
	{
		MFDebug_Warn(1, "Failed to execute script.");
		return 0;
	}

	return returnValue;
}

void MFScript_DestroyScript(MFScript *pScript)
{
	aux_FreeProgram(&pScript->amx);
}

void MFScript_RegisterNativeFunctions(ScriptNativeInfo *pNativeFunctions)
{
	gNativeFunctionRegistry[gNativeFunctionCount] = pNativeFunctions;
	++gNativeFunctionCount;
}

float MFScript_ctof(uint32 cell)
{
	return amx_ctof(cell);
}

uint32 MFScript_ftoc(float _float)
{
	return amx_ftoc(_float);
}

int MFScript_GetAddr(MFScript *pScript, uint32 scriptAddress, uint32 **ppPhysicalAddress)
{
	return amx_GetAddr(&pScript->amx, scriptAddress, (cell**)ppPhysicalAddress);
}

const char* MFScript_GetCString(MFScript *pScript, uint32 scriptString)
{
	cell *amx_cstr_; int amx_length_;
	amx_GetAddr(&pScript->amx, scriptString, &amx_cstr_);
	amx_StrLen(amx_cstr_, &amx_length_);
	if(amx_length_ > 0)
	{
		char *pTemp = (char*)MFStrN("", amx_length_ * sizeof(char));
		amx_GetString(pTemp, amx_cstr_, sizeof(char)>1, amx_length_ + 1);
		return pTemp;
	}

	return NULL;
}

MFVector MFScript_GetVector(MFScript *pScript, uint32 scriptVector, int numComponents)
{
	MFVector t = MFVector::zero;

	uint32 *pArray;
	MFScript_GetAddr(pScript, scriptVector, &pArray);

	for(int a=0; a<numComponents; a++)
	{
		t[a] = amx_ctof(pArray[a]);
	}

	return t;
}

#endif // defined(_USE_PAWN_SCRIPT)
