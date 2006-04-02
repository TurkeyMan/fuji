/**
 * @file MFMaterial.h
 * @brief Provide access to the material system which manages renderer configuration.
 * @author Manu Evans
 * @defgroup MFMaterial Material Interface
 * @{
 */

#if !defined(_MFMATERIAL_H)
#define _MFMATERIAL_H

class MFIni;

/**
 * @struct MFMaterial
 * Represents a Fuji Material.
 */
struct MFMaterial;

/**
 * Stock materials.
 * Enums for stock (built-in) materials.
 */
enum MFStockMaterials
{
	MFMat_White,		/**< White material (This is a flat white material for untextured simulation) */
	MFMat_Unavailable,	/**< Unavailable material (Used when a desired material is unavailable) */
	MFMat_SysLogoSmall,	/**< Small System Logo */
	MFMat_SysLogoLarge,	/**< Large System Logo */

	MFMat_USB,			/**< USB Icon material */
	MFMat_Connected,	/**< Connected Icon material */
	MFMat_Disconnected,	/**< Disconnected Icon material */
	MFMat_Power,		/**< Power Icon material */
	MFMat_Charging,		/**< Charging Icon material */

	MFMat_Max,			/**< Maximum stock material */
	MFMat_ForceInt = 0x7FFFFFFF	/**< Force StockMaterials to an int type */
};

/**
 * Paramater types.
 * Various available material paramater types.
 */
enum MFParamType
{
	MFParamType_Unknown,	/**< Unknown paramater type */

	MFParamType_String,		/**< String paramater type */
	MFParamType_Float,		/**< Floating point paramater type */
	MFParamType_Int,		/**< Integer paramater type */
	MFParamType_Bool,		/**< Boolean paramater type */
	MFParamType_Vector3,	/**< 3D vector paramater type */
	MFParamType_Vector4,	/**< 4D vector paramater type */
	MFParamType_Matrix,		/**< Matrix paramater type */

	MFParamType_Max,		/**< Maximum paramater type */
	MFParamType_ForceInt = 0x7FFFFFFF	/**< Force ParamType to an int type */
};

/**
 * Material paramater information structure.
 * Represents various information about a material paramater. This is exposed for use when writing custom materials.
 */
struct MFMaterialParamaterInfo
{
	const char *pParamaterName;	/**< Name of the paramater */

	MFParamType *pArgTypes;		/**< Pointer to an array of argument types for each argument index */
	int numArgs;				/**< Number of arguments for this paramater */
};

/**
 * Material type registration callbacks.
 * These are a set of callbacks that are defined to register a custom material.
 */
struct MFMaterialCallbacks
{
	int       (*pRegisterMaterial)(void *pPlatformData);	/**< Pointer to the RegisterMaterial function */
	void      (*pUnregisterMaterial)();						/**< Pointer to the UnregisterMaterial function */

	void      (*pCreateInstance)(MFMaterial *pMaterial);	/**< Pointer to the CreateInstance function */
	void      (*pDestroyInstance)(MFMaterial *pMaterial);	/**< Pointer to the DestroyInstance function */

	int       (*pBegin)(MFMaterial *pMaterial);				/**< Pointer to the Begin function */

	void      (*pUpdate)(MFMaterial *pMaterial);			/**< Pointer to the Update function */

	int       (*pGetNumParams)();							/**< Pointer to the GetNumParams function */
	MFMaterialParamaterInfo* (*pGetParamaterInfo)(int paramaterIndex);	/**< Pointer to the GetParamaterInfo function */
	void      (*pSetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex, const void *pValue);	/**< Pointer to a SetParameter function */
	uint32    (*pGetParameter)(MFMaterial *pMaterial, int paramaterIndex, int argIndex, void *pValue);	/**< Pointer to the GetParameter function */
};


// interface functions

/**
 * Add a material definitions file.
 * Add a material definitions file where material definitions can be found.
 * @param pName Name for material definitions.
 * @param pFilename Filename of definitions file.
 * @return If no error occurs, 0 is returned.
 */
int MFMaterial_AddDefinitionsFile(const char *pName, const char *pFilename);

/**
 * Add a material definitions ini.
 * Add a material definitions ini where material definitions can be found.
 * @param pName Name for material definitions.
 * @param pMatDefs Pointer to an ini file containing material definitions.
 * @return If no error occurs, 0 is returned.
 */
int MFMaterial_AddDefinitionsIni(const char *pName, MFIni *pMatDefs);

/**
 * Remove a material definitions file.
 * Removes a material definitions file.
 * @param pName Name of material definitions to remove.
 * @return None.
 */
void MFMaterial_RemoveDefinitions(const char *pName);

/**
 * Create a material instance.
 * Creates an instance of a material.
 * @param pName Name of material to create.
 * @return Returns a new instance of the target material.
 */
MFMaterial*	MFMaterial_Create(const char *pName);

/**
 * Destroy a material instance.
 * Destroys a material instance.
 * @param pMaterial Material instance to destroy.
 * @return Returns the new number of references to the material. If 0 is returned, there are no more instance and the material will be destroyed.
 */
int MFMaterial_Destroy(MFMaterial *pMaterial);

/**
 * Find a material.
 * Finds a material instance.
 * @param pName Name of the material to find.
 * @return Returns a pointer to the material. If the material was not found, NULL is returned.
 * @remarks Note that MFMaterial_Find does NOT increment the materials reference count so you do not need to destroy the pointer returned by MFMaterial_Find.
 */
MFMaterial*	MFMaterial_Find(const char *pName);

/**
 * Get a pointer to a stock material.
 * Gets a pointer to one of the built in 'stock' materials.
 * @param materialIdentifier Identifier of the stock material.
 * @return Returns a pointer to the specified stock material.
 */
MFMaterial*	MFMaterial_GetStockMaterial(MFStockMaterials materialIdentifier);

/**
 * Set the current active material.
 * Sets the current active material.
 * @param pMaterial Pointer to a material to set as the active material.
 * @return None.
 */
void MFMaterial_SetMaterial(MFMaterial *pMaterial);

/**
 * Get the current active material.
 * Gets the current active material.
 * @return Pointer to the current active material.
 */
MFMaterial*	MFMaterial_GetCurrent();

// material control

/**
 * Get the number of paramaters the material exposes.
 * Gets the number of paramaters the material exposes.
 * @param pMaterial Pointer to a material instance.
 * @return Returns the number of paramaters exposed by the material.
 */
int MFMaterial_GetNumParamaters(MFMaterial *pMaterial);

/**
 * Get the name of a paramater.
 * Gets the name of a material paramater.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Paramater index.
 * @return Returns a string representing the paramater name.
 */
const char*	MFMaterial_GetParamaterName(MFMaterial *pMaterial, int paramaterIndex);

/**
 * Get the index of a paramater from a paramater name.
 * Gets the index of a paramater from a paramater name.
 * @param pMaterial Pointer to a material instance.
 * @param pParameterName String representing a paramater name.
 * @return Returns the index of the paramater. -1 if the paramater does not exist.
 */
int MFMaterial_GetParamaterIndexFromName(MFMaterial *pMaterial, const char *pParameterName);

/**
 * Get the number of arguments for a paramater.
 * Gets the number of arguments for a paramater.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Index of the paramater to find the arg count for.
 * @return Returns the number of arguments for a paramater.
 */
int MFMaterial_GetNumParamaterArgs(MFMaterial *pMaterial, int paramaterIndex);

/**
 * Get the ParamType for a paramater argument.
 * Gets the ParamType for a paramater argument.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Index of the paramater.
 * @param argIndex Argument index of the paramater.
 * @return Returns the ParamType of the paramater argument.
 */
MFParamType	MFMaterial_GetParamaterArgType(MFMaterial *pMaterial, int paramaterIndex, int argIndex);

/**
 * Set the value of a paramater.
 * Sets the value of a paramater.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Index of the paramater
 * @param argIndex Argument index of the paramater.
 * @param pValue Pointer to the value to assign to the paramater.
 * @return None.
 */
void MFMaterial_SetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, const void *pValue);

/**
 * Get the value of a paramater.
 * Gets the value of a paramater.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Index of the paramater
 * @param argIndex Argument index of the paramater.
 * @param pValue Pointer to a veriable to receive the paramater data.
 * @return If MFMaterial_GetParamater, the return value is 0.
 */
int MFMaterial_GetParamater(MFMaterial *pMaterial, int paramaterIndex, int argIndex, void *pValue);

/**
 * Set the value of a paramater to an integer value.
 * Sets the value of a paramater to an integer value.
 * @param pMaterial Pointer to a material instance.
 * @param paramaterIndex Index of the paramater
 * @param argIndex Argument index of the paramater.
 * @param paramater Value of the integer paramater being set.
 * @return None.
 */
inline void MFMaterial_SetParamaterI(MFMaterial *pMaterial, int paramaterIndex, int argIndex, size_t paramater)
{
	MFMaterial_SetParamater(pMaterial, paramaterIndex, argIndex, &paramater);
}

/**
 * Register a new material type.
 * Registers a new material type with the material system.
 * @param pName The name of the new material type.
 * @param pCallbacks Pointer to an MFMaterialCallbacks structure which defines a set of callbacks to interact with the material.
 * @return None.
 */
void MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks);

/**
 * Unregister a material type.
 * Unregisters a material type with the material system.
 * @param pName The name of the material type to de-register.
 * @return None.
 */
void MFMaterual_UnregisterMaterialType(const char *pName);

#endif // _MFMATERIAL_H

/** @} */
