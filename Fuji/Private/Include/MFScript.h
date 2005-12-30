/**
 * @file MFScript.h
 * @brief Provide access to the embedded 'pawn' scripting system.
 * @author Manu Evans
 * @defgroup MFScript Scripting Functions
 * @{
 */

#if !defined(_MFSCRIPT_H)
#define _MFSCRIPT_H

/**
 * @struct MFScript
 * Represents a Fuji script.
 */
struct MFScript;

/**
 * Callback function type for native script functions.
 * Callback function type for native script functions.
 */
typedef uint32 (*ScriptNativeFunction)(void *pVirtualMachine, uint32 *pParamaters);

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
 * Begin execution of a pawn script.
 * Begins execution of a pawn script.
 * @param pScript Pointer to an MFScript to execute.
 * @param pEntryPoint Name of the entrypoint function NULL specifies the default entry point (main).
 * @return Returns the value returned from the script entrypoint function.
 * @see MFScript_LoadScript()
 */
int MFScript_Execute(MFScript *pScript, const char *pEntryPoint);

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

#endif

/** @} */
