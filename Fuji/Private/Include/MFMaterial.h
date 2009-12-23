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
 * Parameter types.
 * Various available material parameter types.
 */
enum MFParamType
{
	MFParamType_Unknown,	/**< Unknown parameter type */

	MFParamType_String,		/**< String parameter type */
	MFParamType_Float,		/**< Floating point parameter type */
	MFParamType_Int,		/**< Integer parameter type */
	MFParamType_Bool,		/**< Boolean parameter type */
	MFParamType_Vector3,	/**< 3D vector parameter type */
	MFParamType_Vector4,	/**< 4D vector parameter type */
	MFParamType_Matrix,		/**< Matrix parameter type */

	MFParamType_Max,		/**< Maximum parameter type */
	MFParamType_ForceInt = 0x7FFFFFFF	/**< Force ParamType to an int type */
};

/**
 * Material parameter information structure.
 * Represents various information about a material parameter. This is exposed for use when writing custom materials.
 */
struct MFMaterialParameterInfo
{
	const char *pParameterName;	/**< Name of the parameter */

	MFParamType *pArgTypes;		/**< Pointer to an array of argument types for each argument index */
	int numArgs;				/**< Number of arguments for this parameter */
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
	MFMaterialParameterInfo* (*pGetParameterInfo)(int parameterIndex);	/**< Pointer to the GetParameterInfo function */
	void      (*pSetParameter)(MFMaterial *pMaterial, int parameterIndex, int argIndex, const void *pValue);	/**< Pointer to a SetParameter function */
	uint32    (*pGetParameter)(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue);	/**< Pointer to the GetParameter function */
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
 * @return Returns the new number of references to the material. If 0 is returned, there are no more instances and the material will be destroyed.
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
void MFMaterial_SetMaterial(const MFMaterial *pMaterial);

/**
 * Get the current active material.
 * Gets the current active material.
 * @return Pointer to the current active material.
 */
MFMaterial*	MFMaterial_GetCurrent();

/**
 * Get a materials name.
 * Gets a materials name.
 * @return The name of the specified material.
 */
const char *MFMaterial_GetMaterialName(MFMaterial *pMaterial);

// material control

/**
 * Get the number of parameters the material exposes.
 * Gets the number of parameters the material exposes.
 * @param pMaterial Pointer to a material instance.
 * @return Returns the number of parameters exposed by the material.
 */
int MFMaterial_GetNumParameters(MFMaterial *pMaterial);

/**
 * Get the name of a parameter.
 * Gets the name of a material parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Parameter index.
 * @return Returns a string representing the parameter name.
 */
const char*	MFMaterial_GetParameterName(MFMaterial *pMaterial, int parameterIndex);

/**
 * Get the index of a parameter from a parameter name.
 * Gets the index of a parameter from a parameter name.
 * @param pMaterial Pointer to a material instance.
 * @param pParameterName String representing a parameter name.
 * @return Returns the index of the parameter. -1 if the parameter does not exist.
 */
int MFMaterial_GetParameterIndexFromName(MFMaterial *pMaterial, const char *pParameterName);

/**
 * Get the number of arguments for a parameter.
 * Gets the number of arguments for a parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter to find the arg count for.
 * @return Returns the number of arguments for a parameter.
 */
int MFMaterial_GetNumParameterArgs(MFMaterial *pMaterial, int parameterIndex);

/**
 * Get the ParamType for a parameter argument.
 * Gets the ParamType for a parameter argument.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter.
 * @param argIndex Argument index of the parameter.
 * @return Returns the ParamType of the parameter argument.
 */
MFParamType	MFMaterial_GetParameterArgType(MFMaterial *pMaterial, int parameterIndex, int argIndex);

/**
 * Set the value of a parameter.
 * Sets the value of a parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @param pValue Pointer to the value to assign to the parameter.
 * @return None.
 */
void MFMaterial_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, const void *pValue);

/**
 * Get the value of a parameter.
 * Gets the value of a parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @param pValue Pointer to a veriable to receive the parameter data.
 * @return If MFMaterial_GetParameter, the return value is 0.
 */
int MFMaterial_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue);

/**
 * Set the value of a parameter to an integer value.
 * Sets the value of a parameter to an integer value.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @param parameter Value of the integer parameter being set.
 * @return None.
 */
inline void MFMaterial_SetParameterI(MFMaterial *pMaterial, int parameterIndex, int argIndex, size_t parameter)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, &parameter);
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
void MFMaterial_UnregisterMaterialType(const char *pName);

#endif // _MFMATERIAL_H

/** @} */
