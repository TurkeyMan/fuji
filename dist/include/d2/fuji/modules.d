module fuji.modules;

/**
 * Module init completion status.
 * Module initialisation completion status.
 */
enum MFInitStatus
{
	Failed = -1,	/**< Module initialisation failed. */
	Pending = 0,	/**< Module initialisation pending. */
	Succeeded = 1	/**< Module initialisation completed successfully. */
}

/**
 * Fuji module initialisation callback prototype.
 */
alias MFInitStatus function() MFInitCallback;

/**
 * Fuji module destruction callback prototype.
 */
alias void function() MFDeinitCallback;

/**
 * Register a Fuji module.
 * Registers a Fuji module.
 * @param pSystemName The name of the module.
 * @param pInitFunction Module init function callback.
 * @param pDeinitFunction Module init function callback.
 * @param prerequisites A bitfield of modules that must be loaded prior to this module.
 * @return Returns an ID identifying the module.
 */
extern (C) int MFModule_RegisterModule(const(char*) pModuleName, MFInitCallback* pInitFunction, MFDeinitCallback* pDeinitFunction, ulong prerequisites = 0);

extern (C) ulong MFModule_RegisterCoreModules();

extern (C) ulong MFModule_RegisterModules();

extern (C) int MFModule_GetNumModules();

extern (C) int MFModule_GetModuleID(const(char*) pName);

extern (C) const(char*) MFModule_GetModuleName(int id);

extern (C) bool MFModule_IsModuleInitialised(int id);
extern (C) bool MFModule_DidModuleInitialisationFail(int id);

extern (C) ulong MFModule_GetModuleMask(const(char*)* ppModuleNames);

