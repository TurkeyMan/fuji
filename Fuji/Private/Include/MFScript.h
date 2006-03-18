/**
 * @file MFScript.h
 * @brief Provide access to the embedded 'pawn' scripting system.
 * @author Manu Evans
 * @defgroup MFScript Scripting Functions
 * @{
 */

#if !defined(_MFSCRIPT_H)
#define _MFSCRIPT_H

#include "MFVector.h"

#define MFEntryPoint_Main -1

/**
 * @struct MFScript
 * Represents a Fuji script.
 */
struct MFScript;

/**
 * Represents an entrypoint into a script.
 * Represents an entrypoint into a script.
 */
typedef int MFEntryPoint;

/**
 * Callback function type for native script functions.
 * Callback function type for native script functions.
 */
typedef uint32 (*ScriptNativeFunction)(MFScript *pScript, uint32 *pParamaters);

/**
 * Represents an string in a script.
 * Represents an string in a script.
 */
typedef uint32 MFScriptString;

/**
 * Represents an array in a script.
 * Represents an array in a script.
 */
typedef uint32 MFScriptArray;

/**
 * Script native function description.
 * Describes a native script function.
 */
struct ScriptNativeInfo
{
  const char *pDecl;	/**< String representing the function declaration as will be known to the script */
  void *pFunc;			/**< Pointer to the native function. */
};

/**
 * Load a fuji script and prepares it for execution.
 * Loads a fuji script and prepares it for execution.
 * @param pFilename Filename of the script to load.
 * @return Returns a handle to the loaded script.
 * @see MFScript_DestroyScript()
 * @see MFScript_Execute()
 */
MFScript* MFScript_LoadScript(const char *pFilename);

/**
 * Find a public function.
 * Finds a public function and returns an MFEntryPoint to that function.
 * @param pScript Script to search.
 * @param pFunctionName Function name to search for in the public exports.
 * @return Returns an MFEntryPoint pointing to the function entry point. If the function was not found, the value of MFEntryPoint_Main (-1) is returned.
 * @see MFScript_Call()
 */
MFEntryPoint MFScript_FindPublicFunction(MFScript *pScript, const char *pFunctionName);

/**
 * Begin execution of a fuji script.
 * Begins execution of a fuji script.
 * @param pScript Pointer to an MFScript to execute.
 * @param pEntryPoint Name of the entrypoint function. NULL specifies the default entry point (main).
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 */
int MFScript_Execute(MFScript *pScript, const char *pEntryPoint);

/**
 * Execute an immediate instruction.
 * Executes an immediate instruction.
 * @param pScript Pointer to an MFScript to execute.
 * @param pCode String containing the code to execute.
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_Execute()
 */
int MFScript_ExecuteImmediate(MFScript *pScript, const char *pCode);

/**
 * Begin execution of a fuji script at a specified entrypoint.
 * Begins execution of a fuji scriptat at a specified entrypoint.
 * @param pScript Pointer to an MFScript to execute.
 * @param entryPoint A valid MFEntryPoint specifying where to begin execution.
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 * @see MFScript_FindPublicFunction()
 */
int MFScript_Call(MFScript *pScript, MFEntryPoint entryPoint);

/**
 * Destroy a fuji script.
 * Destroys a fuji script.
 * @param pScript Pointer to an MFScript to destroy.
 * @return None.
 * @see MFScript_LoadScript()
 */
void MFScript_DestroyScript(MFScript *pScript);

/**
 * Register native functions.
 * Registers an array of native functions that can be accessed by scripts.
 * @param pNativeFunctions Pointer to an array of ScriptNativeInfo structures describing an array of available native functions. Array must terminate with a NULL ScriptNativeInfo structure.
 * @return None.
 * @remarks Note: MFScript_RegisterNativeFunctions does NOT take a copy of the data pointed to by pNativeFunctions. This pointer MUST point to memory that will remain available for the life of the script (usually global memory).
 */
void MFScript_RegisterNativeFunctions(ScriptNativeInfo *pNativeFunctions);

/**
 * Convert a script string into a C string.
 * Converts a script string into a C string.
 * @param pScript MFScript where the string lives.
 * @param scriptString A string in the script.
 * @return Returns a pointer to a the converted C string.
 */
const char* MFScript_GetCString(MFScript *pScript, MFScriptString scriptString);

/**
 * Convert a C string to a script string.
 * Converts a C string into a script string.
 * @param pScript MFScript where the string lives.
 * @param pString C string to be converted.
 * @return Returns an MFScriptString representing the string.
 */
MFScriptString MFScript_MakeScriptString(MFScript *pScript, const char *pString);

/**
 * Get the base pointer to a script array.
 * Gets the base pointer to a script array.
 * @param pScript MFScript where the array lives.
 * @param scriptArray An array in the script.
 * @return Returns a pointer to the base of the array.
 */
const void* MFScript_GetArray(MFScript *pScript, MFScriptArray scriptArray);

/**
 * Get the number of items in a dynamic script array.
 * Gets the number of items in a dynamic script array.
 * @param pScript MFScript where the array lives.
 * @param scriptArray An array in the script.
 * @return Returns the number of items in the array.
 */
int MFScript_GetArraySize(MFScript *pScript, MFScriptArray scriptArray);

/**
 * Get an element from a script array.
 * Gets an element from a script array.
 * @param pScript MFScript where the array lives.
 * @param scriptArray An array in the script.
 * @param item The item to retrieve.
 * @return Returns a pointer to an item in the array.
 */
const void* MFScript_GetArrayItem(MFScript *pScript, MFScriptArray scriptArray, int item);


/**** Pawn related ****/

#if defined(_USE_PAWN_SCRIPT)

/**
 * Cell to float.
 * Convert a script 'cell' to a float.
 * @param cell The cell to be converted to a float.
 * @return The cell as a float.
 */
float MFScript_ctof(uint32 cell);

/**
 * Float to cell.
 * Convert a float to a script 'cell'.
 * @param _float The float to be converted to a cell.
 * @return The float as a cell.
 */
uint32 MFScript_ftoc(float _float);

/**
 * Resolve a script reference to a physical object.
 * Resolve a script reference to a physical object.
 * @param pScript MFScript where the object lives.
 * @param scriptAddress The script address of the object.
 * @param ppPhysicalAddress Address of a pointer to the object which receives the physical address of the object.
 * @return Error code.
 */
int MFScript_GetAddr(MFScript *pScript, uint32 scriptAddress, uint32 **ppPhysicalAddress);

#endif

#endif

/** @} */
