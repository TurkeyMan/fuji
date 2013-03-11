/**
 * @file MFModule.h
 * @brief Fuji module registration and management.
 * @author Manu Evans
 * @defgroup MFModule Module related
 * @{
 */

#if !defined(_MFMODULE_H)
#define _MFMODULE_H

/**
 * Module init completion status.
 * Module initialisation completion status.
 */
enum MFInitStatus
{
	MFAIC_Failed = -1,			/**< Module initialisation failed. */
	MFAIC_Pending = 0,			/**< Module initialisation pending. */
	MFAIC_Succeeded = 1,		/**< Module initialisation completed successfully. */

	MFAIC_ForceInt = 0x7FFFFFFF	/**< Force enum to int type. */
};

/**
 * Fuji module initialisation callback prototype.
 */
typedef MFInitStatus (MFInitCallback)();

/**
 * Fuji module destruction callback prototype.
 */
typedef void (MFDeinitCallback)();

/**
 * Register a Fuji module.
 * Registers a Fuji module.
 * @param pSystemName The name of the module.
 * @param pInitFunction Module init function callback.
 * @param pDeinitFunction Module init function callback.
 * @param prerequisites A bitfield of modules that must be loaded prior to this module.
 * @return Returns an ID identifying the module.
 */
MF_API int MFModule_RegisterModule(const char *pModuleName, MFInitCallback *pInitFunction, MFDeinitCallback *pDeinitFunction, uint64 prerequisites = 0);

MF_API uint64 MFModule_RegisterCoreModules();

MF_API uint64 MFModule_RegisterEngineModules();

MF_API int MFModule_GetNumModules();

MF_API int MFModule_GetModuleID(const char *pName);

MF_API const char *MFModule_GetModuleName(int id);

MF_API bool MFModule_IsModuleInitialised(int id);
MF_API bool MFModule_DidModuleInitialisationFail(int id);

MF_API uint64 MFModule_GetModuleMask(const char **ppModuleNames);

#endif // _MFMODULE_H

/** @} */
