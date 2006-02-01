/**
 * @file MFScript.h
 * @brief Provide access to the embedded 'pawn' scripting system.
 * @author Manu Evans
 * @defgroup MFScript Scripting Functions
 * @{
 */

#if !defined(_MFSCRIPT_H)
#define _MFSCRIPT_H

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
 * Script native function description.
 * Describes a native script function.
 */
struct ScriptNativeInfo
{
  const char *pName;			/**< String representing the name of the function as will be known to the script */
  ScriptNativeFunction pFunc;	/**< Pointer to the native function. */
};

/**
 * Load a pawn script and prepares it for execution.
 * Loads a pawn script and prepares it for execution.
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
 * @return Returns an MFEntryPoint pointing to the function entry point. If the function was not found, the value of MFEntryPoint_Main is returned.
 * @see MFScript_Call()
 */
MFEntryPoint MFScript_FindPublicFunction(MFScript *pScript, const char *pFunctionName);

/**
 * Begin execution of a pawn script.
 * Begins execution of a pawn script.
 * @param pScript Pointer to an MFScript to execute.
 * @param pEntryPoint Name of the entrypoint function. NULL specifies the default entry point (main).
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 */
int MFScript_Execute(MFScript *pScript, const char *pEntryPoint);

/**
 * Begin execution of a pawn script at a specified entrypoint.
 * Begins execution of a pawn scriptat at a specified entrypoint.
 * @param pScript Pointer to an MFScript to execute.
 * @param entryPoint A valid MFEntryPoint specifying where to begin execution.
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 * @see MFScript_FindPublicFunction()
 */
int MFScript_Call(MFScript *pScript, MFEntryPoint entryPoint);

/**
 * Destroy a pawn script.
 * Destroys a pawn script.
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

/**
 * Convert a script string into a C string.
 * Converts a script string into a C string and returns a pointer to the string in the MFStr buffer.
 * @param pScript MFScript where the string lives.
 * @param scriptString A string in the script.
 * @return Returns a pointer to a the string converted C string in the MFStr buffer.
 */
char* MFScript_GetCString(MFScript *pScript, uint32 scriptString);

#endif

/** @} */
