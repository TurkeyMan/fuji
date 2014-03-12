/**
 * @file MFModule.h
 * @brief Fuji module registration and management.
 * @author Manu Evans
 * @defgroup MFModule Module related
 * @{
 */

#pragma once
#if !defined(_MFMODULE_H)
#define _MFMODULE_H

/**
 * Module init completion status.
 * Module initialisation completion status.
 */
enum MFInitStatus
{
	MFIS_Failed = -1,				/**< Module initialisation failed. */
	MFIS_Pending = 0,				/**< Module initialisation pending. */
	MFIS_Succeeded = 1,				/**< Module initialisation completed successfully. */

	MFIS_ForceInt = 0x7FFFFFFF		/**< Force enum to int type. */
};

/**
 * Fuji module initialisation callback prototype.
 */
typedef MFInitStatus (MFInitCallback)(int moduleId, bool bPerformInitialisation);

/**
 * Fuji module destruction callback prototype.
 */
typedef void (MFDeinitCallback)();

/**
 * Register a Fuji module.
 * Registers a Fuji module to be initialised during the startup process.
 * @param pModuleName The name of the module.
 * @param pInitFunction Module init function callback.
 * @param pDeinitFunction Module deinit function callback.
 * @param prerequisites A bitfield of modules that must be loaded prior to this module.
 * @return An ID identifying the module.
 */
MF_API int MFModule_RegisterModule(const char *pModuleName, MFInitCallback *pInitFunction, MFDeinitCallback *pDeinitFunction, uint64 prerequisites = 0);

MF_API uint64 MFModule_RegisterCoreModules();
MF_API uint64 MFModule_RegisterEngineModules();

MF_API bool MFModule_InitModules();
MF_API void MFModule_DeinitModules();
MF_API bool MFModule_BindModules();

/**
 * Get the number of registered modules.
 * Gets the number of registered modules.
 * @return The number of registered modules.
 */
MF_API int MFModule_GetNumModules();

/**
 * Get a module's id.
 * Gets the id of the requested module by name.
 * @param pName The name of the module.
 * @return The module's id.
 */
MF_API int MFModule_GetModuleID(const char *pName);

/**
 * Get a module's name.
 * Gets the name of the requested module by index.
 * @param id Module id.
 * @return The module's name.
 */
MF_API const char *MFModule_GetModuleName(int id);

/**
 * Check if a module initialised.
 * Check's if a module initialised successfully.
 * @param id Module id.
 * @return \a true if the module initialised successfully.
 */
MF_API bool MFModule_IsModuleInitialised(int id);

/**
 * Check if a module initialisation failed.
 * Check's if a module initialisation failed.
 * @param id Module id.
 * @return \a true if the module initialisation failed.
 */
MF_API bool MFModule_DidModuleInitialisationFail(int id);

/**
 * Get a bit mask for a list of modules.
 * Get's a bit mask for a NULL-terminated list of modules.
 * @param ppModuleNames A NULL-terminated list of module names.
 * @return A mask with bits set for each module listed.
 * @remarks To get a mask including all the 'core' modules (modules that are not optional), use "MFCore".
 */
MF_API uint64 MFModule_GetModuleMask(const char **ppModuleNames);

#endif // _MFMODULE_H

/** @} */
