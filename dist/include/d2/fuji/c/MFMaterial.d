module fuji.c.MFMaterial;

import fuji.c.MFResource;
import fuji.c.MFTexture;
import fuji.c.MFMatrix;

nothrow:
@nogc:

struct MFIni;
struct MFEnumKey
{
	const(char)* pKey;
	int value;
};

/**
* @struct MFMaterial
* Represents a Fuji Material.
*/
struct MFMaterial
{
	@disable this();
	@disable this(this);
}

/**
* Stock materials.
* Enums for stock (built-in) materials.
*/
enum MFStockMaterials
{
	White,			/**< White material (This is a flat white material for untextured simulation) */
	Unavailable,	/**< Unavailable material (Used when a desired material is unavailable) */
	SysLogoSmall,	/**< Small System Logo */
	SysLogoLarge,	/**< Large System Logo */

	USB,			/**< USB Icon material */
	Connected,		/**< Connected Icon material */
	Disconnected,	/**< Disconnected Icon material */
	Power,			/**< Power Icon material */
	Charging		/**< Charging Icon material */
}

/**
* Parameter types.
* Various available material parameter types.
*/
enum MFParamType
{
	None,		/**< No parameter */

	Constant,	/**< Constant parameter type */
	String,		/**< String parameter type */
	Float,		/**< Floating point parameter type */
	Int,		/**< Integer parameter type */
	Enum,		/**< Parameter is an enumeration */
	Bool,		/**< Boolean parameter type */
	Vector3,	/**< 3D vector parameter type */
	Vector4,	/**< 4D vector parameter type */
	Colour,		/**< Colour parameter type */
	Matrix		/**< Matrix parameter type */
}

/**
* Material parameter information structure.
* Represents various information about a material parameter. This is exposed for use when writing custom materials.
*/
struct MFMaterialParameterInfo
{
	struct ParameterDetails
	{
		MFParamType type;
		int defaultValue;
		MFEnumKey* pEnumKeys;
	}

	const(char)* pParameterName;	/**< Name of the parameter */

	int parameterIndex;				/**< Index of the parameter */

	ParameterDetails argIndexHigh;	/**< (Optional) Arg index HiWord details */
	ParameterDetails argIndex;		/**< (Optional) Arg index details */

	ParameterDetails* pValues;		/**< Pointer to an array of parameter details describing the value struct */
	int numValues;					/**< Number of elements in the argument struct */
}

/**
* Material type registration callbacks.
* These are a set of callbacks that are defined to register a custom material.
*/
struct MFMaterialCallbacks
{
	extern (C) int       function(void* pPlatformData) nothrow pRegisterMaterial;	/**< Pointer to the RegisterMaterial function */
	extern (C) void      function() nothrow pUnregisterMaterial;					/**< Pointer to the UnregisterMaterial function */

	extern (C) void      function(MFMaterial* pMaterial) nothrow pCreateInstance;	/**< Pointer to the CreateInstance function */
	extern (C) void      function(MFMaterial* pMaterial) nothrow pDestroyInstance;	/**< Pointer to the DestroyInstance function */

	extern (C) int       function(MFMaterial* pMaterial) nothrow pBegin;			/**< Pointer to the Begin function */

	extern (C) void      function(MFMaterial* pMaterial) nothrow pUpdate;			/**< Pointer to the Update function */

	extern (C) int       function() nothrow pGetNumParams;							/**< Pointer to the GetNumParams function */
	extern (C) const(MFMaterialParameterInfo)* function(int parameterIndex) nothrow pGetParameterInfo;	/**< Pointer to the GetParameterInfo function */
	extern (C) void      function(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value) nothrow pSetParameter;	/**< Pointer to a SetParameter function */
	extern (C) size_t    function(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex, void* pValue) nothrow pGetParameter;	/**< Pointer to the GetParameter function */
}


// interface functions

/**
* Add a material definitions file.
* Add a material definitions file where material definitions can be found.
* @param pName Name for material definitions.
* @param pFilename Filename of definitions file.
* @return If no error occurs, 0 is returned.
*/
extern (C) int MFMaterial_AddDefinitionsFile(const(char)* pName, const(char)* pFilename);

/**
* Add a material definitions ini.
* Add a material definitions ini where material definitions can be found.
* @param pName Name for material definitions.
* @param pMatDefs Pointer to an ini file containing material definitions.
* @return If no error occurs, 0 is returned.
*/
extern (C) int MFMaterial_AddDefinitionsIni(const(char)* pName, MFIni* pMatDefs);

/**
* Remove a material definitions file.
* Removes a material definitions file.
* @param pName Name of material definitions to remove.
* @return None.
*/
extern (C) void MFMaterial_RemoveDefinitions(const(char)* pName);

/**
* Create a material instance.
* Creates an instance of a material.
* @param pName Name of material to create.
* @return A new instance of the target material.
*/
extern (C) MFMaterial* MFMaterial_Create(const(char)* pName);

/**
* Release an MFMaterial instance.
* Release a reference to an MFMaterial and destroy when the reference count reaches 0.
* @param pMaterial Material instance to destroy.
* @return The new reference count of the material. If the returned reference count is 0, the material is destroyed.
*/
extern (C) int MFMaterial_Release(MFMaterial* pMaterial);

/**
* Find a material.
* Finds a material instance.
* @param pName Name of the material to find.
* @return A pointer to the material. If the material was not found, NULL is returned.
* @remarks Note that MFMaterial_Find increments the materials reference count so you must release the reference when finished.
*/
extern (C) MFMaterial* MFMaterial_Find(const(char)* pName);

/**
* Get a pointer to a stock material.
* Gets a pointer to one of the built in 'stock' materials.
* @param materialIdentifier Identifier of the stock material.
* @return A pointer to the specified stock material.
*/
extern (C) MFMaterial* MFMaterial_GetStockMaterial(MFStockMaterials materialIdentifier);

/**
* Set the current active material.
* Sets the current active material.
* @param pMaterial Pointer to a material to set as the active material.
* @return None.
*/
extern (C) void MFMaterial_SetMaterial(MFMaterial* pMaterial);

/**
* Get the current active material.
* Gets the current active material.
* @return Pointer to the current active material.
*/
extern (C) MFMaterial* MFMaterial_GetCurrent();

/**
* Get a materials name.
* Gets a materials name.
* @return The name of the specified material.
*/
extern (C) const(char)* MFMaterial_GetMaterialName(const(MFMaterial)* pMaterial) pure;

// material control

/**
* Get the number of parameters the material exposes.
* Gets the number of parameters the material exposes.
* @param pMaterial Pointer to a material instance.
* @return The number of parameters exposed by the material.
*/
extern (C) int MFMaterial_GetNumParameters(const(MFMaterial)* pMaterial);

/**
* Get the name of a parameter.
* Gets the name of a material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return A string representing the parameter name.
*/
extern (C) const(char)* MFMaterial_GetParameterName(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get the index of a parameter from a parameter name.
* Gets the index of a parameter from a parameter name.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return The index of the parameter. -1 if the parameter does not exist.
*/
extern (C) int MFMaterial_GetParameterIndexFromName(const(MFMaterial)* pMaterial, const(char)* pParameterName);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return A pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) const(MFMaterialParameterInfo)* MFMaterial_GetParameterInfo(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return A pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) const(MFMaterialParameterInfo)* MFMaterial_GetParameterInfoFromName(const(MFMaterial)* pMaterial, const(char)* pParameterName);

/**
* Get the value of a parameter.
* Gets the value of a parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pValue Pointer to a buffer to receive non-integer parameter data.
* @return The value of integer or poitner parameters, otherwise returns 0 on success.
*/
extern (C) size_t MFMaterial_GetParameter(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex, void* pValue = null);

/**
* Get the value of an integer parameter.
* Gets the value of an integer parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return The value contained in the integer parameter.
*/
/+inline+/ size_t MFMaterial_GetParameterI(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex)
{
	return MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, null);
}

/**
* Get the value of a float parameter.
* Gets the value of a float parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return The value contained in the float parameter.
*/
/+inline+/ float MFMaterial_GetParameterF(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex)
{
	float value;
	MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, &value);
	return value;
}

/**
* Get the value of a vector parameter.
* Gets the value of a vector parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pValue Pointer to a vector to receive the parameter data.
* @return Returns 0 on success.
*/
/+inline+/ int MFMaterial_GetParameterV(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex, MFVector* pVector)
{
	return cast(int)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pVector);
}

/**
* Get the value of a matrix parameter.
* Gets the value of a matrix parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pMatrix Pointer to a matrix to receive the parameter data.
* @return Returns 0 on success.
*/
/+inline+/ int MFMaterial_GetParameterM(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex, MFMatrix* pMatrix)
{
	return cast(int)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pMatrix);
}

/**
* Get the value of a texture parameter.
* Gets the value of a texture parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return The MFTexture set to the parameter requested.
*/
/+inline+/ MFTexture* MFMaterial_GetParameterT(const(MFMaterial)* pMaterial, int parameterIndex, int argIndex)
{
	return cast(MFTexture*)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, null);
}

/**
* Set the value of a parameter.
* Sets the value of a parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param value The value to assign to the parameter.
* @return None.
*/
extern (C) void MFMaterial_SetParameter(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value);

/**
* Set the value of an integer parameter.
* Sets the value of an integer parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param value Value of the integer parameter being set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterI(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, value);
}

/**
* Set the value of a float parameter.
* Sets the value of a float parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param value Value of the float parameter being set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterF(MFMaterial* pMaterial, int parameterIndex, int argIndex, float value)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, cast(size_t)&value);
}

/**
* Set the value of a vector parameter.
* Sets the value of a vector parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param vector Value of the vector parameter being set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterV(MFMaterial* pMaterial, int parameterIndex, int argIndex, ref const MFVector vector)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, cast(size_t)&vector);
}

/**
* Set the value of a matrix parameter.
* Sets the value of a matrix parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param matrix Value of the matrix parameter being set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterM(MFMaterial* pMaterial, int parameterIndex, int argIndex, ref const MFMatrix matrix)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, cast(size_t)&matrix);
}

/**
* Set the value of a texture parameter.
* Sets the value of a texture parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pTexture Texture to be set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterT(MFMaterial* pMaterial, int parameterIndex, int argIndex, const MFTexture* pTexture)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, cast(size_t)pTexture);
}

/**
* Set the value of a string parameter.
* Sets the value of a string parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pString String to be set.
* @return None.
*/
/+inline+/ void MFMaterial_SetParameterS(MFMaterial* pMaterial, int parameterIndex, int argIndex, const(char)* pString)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, cast(size_t)pString);
}

/**
* Register a new material type.
* Registers a new material type with the material system.
* @param pName The name of the new material type.
* @param pCallbacks Pointer to an MFMaterialCallbacks structure which defines a set of callbacks to interact with the material.
* @return None.
*/
extern (C) void MFMaterial_RegisterMaterialType(const(char)* pName, const(MFMaterialCallbacks)* pCallbacks);

/**
* Unregister a material type.
* Unregisters a material type with the material system.
* @param pName The name of the material type to de-register.
* @return None.
*/
extern (C) void MFMaterial_UnregisterMaterialType(const(char)* pName);
