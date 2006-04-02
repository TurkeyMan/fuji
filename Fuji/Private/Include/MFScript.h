/**
 * @file MFScript.h
 * @brief Provide access to the embedded 'AngelScript' scripting system.
 * @author Manu Evans
 * @defgroup MFScript Scripting Functions
 * @{
 */

#if !defined(_MFSCRIPT_H)
#define _MFSCRIPT_H

#include "MFVector.h"

#define MFEntryPoint_Main -1

/**
 * @struct MFScriptContext
 * Represents a Fuji script execution context.
 */
struct MFScriptContext;

/**
 * Represents an entrypoint into a script.
 * Represents an entrypoint into a script.
 */
typedef int MFEntryPoint;

/**
 * Callback function type for native script functions.
 * Callback function type for native script functions.
 */
//typedef uint32 (*ScriptNativeFunction)(MFScript *pScript, uint32 *pParameters);

/**
 * Represents any object in a script.
 * Represents any object in a script.
 */
typedef uint32 MFScriptObject;

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
 * ...
 *
 */
enum MFScriptCallingConvention
{
	MFCC_CDecl = 0,
	MFCC_StdCall,
	MFCC_ThisCall,
	MFCC_CDecl_ObjLast,
	MFCC_CDecl_ObjFirst,
	MFCC_Generic,

	MFCC_Max,
	MFCC_ForceInt = 0x7FFFFFFF
};

/**
 * Load a fuji script and prepares it for execution.
 * Loads a fuji script and prepares it for execution.
 * @param pFilename Filename of the script to load.
 * @return Returns 0 if script was loaded successfully. Negative values indicate an error.
 * @see MFScript_DestroyScript()
 * @see MFScript_Execute()
 */
int MFScript_LoadScript(const char *pFilename);

/**
 * Find a public function.
 * Finds a public function and returns an MFEntryPoint to that function.
 * @param pScript Script to search.
 * @param pFunctionName Function name to search for in the public exports.
 * @return Returns an MFEntryPoint pointing to the function entry point. If the function was not found, the value of MFEntryPoint_Main (-1) is returned.
 * @see MFScript_Call()
 */
MFEntryPoint MFScript_FindPublicFunction(const char *pFunctionName);

/**
 * Create a script execution context.
 * Creates an execution context that can be used to execute script.
 * @return Returns a pointer to a newly created script context.
 * @see MFScript_Execute()
 */
MFScriptContext* MFScript_CreateContext();

/**
 * Begin execution of a fuji script.
 * Begins execution of a fuji script.
 * @param pContext Pointer to an MFScriptContext to execute within.
 * @param pEntryPoint Name of the entrypoint function. NULL specifies the default entry point (main).
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 */
int MFScript_Execute(MFScriptContext *pContext, const char *pEntryPoint);

/**
 * Execute an immediate instruction.
 * Executes an immediate instruction.
 * @param pContext Pointer to an MFScriptContext to execute within.
 * @param pCode String containing the code to execute.
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_Execute()
 */
int MFScript_ExecuteImmediate(const char *pCode, MFScriptContext *pContext = NULL);

/**
 * Begin execution of a fuji script at a specified entrypoint.
 * Begins execution of a fuji scriptat at a specified entrypoint.
 * @param pContext Pointer to an MFScriptContext to execute within.
 * @param entryPoint A valid MFEntryPoint specifying where to begin execution.
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 * @see MFScript_FindPublicFunction()
 */
int MFScript_Call(MFScriptContext *pContext, MFEntryPoint entryPoint);

/**
 * Destroy a fuji script execution context.
 * Destroys a fuji script execution context.
 * @param pContext Pointer to a MFScriptContext to destroy.
 * @return None.
 * @see MFScript_CreateExecutionContext()
 */
void MFScript_DestroyContext(MFScriptContext *pContext);

/**
 * Register native function.
 * Registers a native function that can be accessed by scripts.
 * @param pDeclaration The function declaration, as seen by the script.
 * @param pFunction Pointer to the native function to be registered.
 * @param callingConvention Calling convention to be used when calling the specified function.
 * @return None.
 */
void MFScript_RegisterNativeFunction(const char *pDeclaration, void *pFunction, MFScriptCallingConvention callingConvention);

/**
 * Convert a script string into a C string.
 * Converts a script string into a C string.
 * @param scriptString A string in the script.
 * @return Returns a pointer to a the converted C string.
 */
const char* MFScript_GetCString(MFScriptString scriptString);

/**
 * Convert a C string to a script string.
 * Converts a C string into a script string.
 * @param pString C string to be converted.
 * @return Returns an MFScriptString representing the string.
 */
MFScriptString MFScript_MakeScriptString(const char *pString);

/**
 * Get the base pointer to a script array.
 * Gets the base pointer to a script array.
 * @param scriptArray An array in the script.
 * @return Returns a pointer to the base of the array.
 */
const void* MFScript_GetArray(MFScriptArray scriptArray);

/**
 * Get the number of items in a dynamic script array.
 * Gets the number of items in a dynamic script array.
 * @param scriptArray An array in the script.
 * @return Returns the number of items in the array.
 */
int MFScript_GetArraySize(MFScriptArray scriptArray);

/**
 * Get an element from a script array.
 * Gets an element from a script array.
 * @param scriptArray An array in the script.
 * @param item The item to retrieve.
 * @return Returns a pointer to an item in the array.
 */
const void* MFScript_GetArrayItem(MFScriptArray scriptArray, int item);

/**
 * Increase the reference count of a script object.
 * Increases the reference count of a script object.
 * @param pObject An object belonging to the script to have its ref count increased.
 * @return The internal reference count.
 */
int MFScript_AddRef(MFScriptObject *pObject);

/**
 * Decrease the reference count of a script object.
 * Decrease the reference count of a script object.
 * @param pObject An object belonging to the script to have its ref count decreased.
 * @return The internal reference count.
 * @remarks If the internal reference count is reduced to zero, the object becomes a candidate for garbage collection.
 */
int MFScript_Release(MFScriptObject *pObject);

#endif

/** @} */
