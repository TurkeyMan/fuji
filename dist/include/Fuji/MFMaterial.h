/**
 * @file MFMaterial.h
 * @brief Provide access to the material system which manages renderer configuration.
 * @author Manu Evans
 * @defgroup MFMaterial Material Interface
 * @{
 */

#pragma once
#if !defined(_MFMATERIAL_H)
#define _MFMATERIAL_H

class MFIni;
struct MFIniEnumKey;
struct MFTexture;
struct MFStateBlock;
struct MFRendererState;
struct MFMaterialType;

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
	MFParamType_None,		/**< No parameter */

	MFParamType_Constant,	/**< Constant parameter type */
	MFParamType_String,		/**< String parameter type */
	MFParamType_Float,		/**< Floating point parameter type */
	MFParamType_Int,		/**< Integer parameter type */
	MFParamType_Enum,		/**< Parameter is an enumeration */
	MFParamType_Bool,		/**< Boolean parameter type */
	MFParamType_Vector3,	/**< 3D vector parameter type */
	MFParamType_Vector4,	/**< 4D vector parameter type */
	MFParamType_Colour,		/**< Colour parameter type */
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
	struct ParameterDetails
	{
		MFParamType type;
		int defaultValue;
		MFIniEnumKey *pEnumKeys;
	};

	const char *pParameterName;		/**< Name of the parameter */

	int parameterIndex;				/**< Index of the parameter */

	ParameterDetails argIndexHigh;	/**< (Optional) Arg index HiWord details */
	ParameterDetails argIndex;		/**< (Optional) Arg index details */

	ParameterDetails *pValues;		/**< Pointer to an array of parameter details describing the value struct */
	int numValues;					/**< Number of elements in the argument struct */
};

/**
 * Material type registration callbacks.
 * These are a set of callbacks that are defined to register a custom material.
 */
struct MFMaterialCallbacks
{
	int       (*pRegisterMaterial)(MFMaterialType *pType);				/**< Pointer to the RegisterMaterial function */
	void      (*pUnregisterMaterial)();									/**< Pointer to the UnregisterMaterial function */

	void      (*pCreateInstance)(MFMaterial *pMaterial);				/**< Pointer to the CreateInstance function */
	void      (*pDestroyInstance)(MFMaterial *pMaterial);				/**< Pointer to the DestroyInstance function */

	void      (*pUpdate)(MFMaterial *pMaterial);						/**< Pointer to the Update function */

	void      (*pBuildStateBlock)(MFMaterial *pMaterial);				/**< Pointer to the BuildStateBlock function */

	int       (*pBegin)(MFMaterial *pMaterial, MFRendererState &state);	/**< Pointer to the Begin function */

	int       (*pGetNumParams)();										/**< Pointer to the GetNumParams function */
	MFMaterialParameterInfo* (*pGetParameterInfo)(int parameterIndex);	/**< Pointer to the GetParameterInfo function */
	void      (*pSetParameter)(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value);	/**< Pointer to a SetParameter function */
	uintp     (*pGetParameter)(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue);	/**< Pointer to the GetParameter function */
};

/**
 * Material type.
 * Describes a material type.
 */
struct MFMaterialType
{
	MFMaterialCallbacks materialCallbacks;	/**< Material type callbacks */
	char *pTypeName;						/**< Material type name */
	size_t instanceDataSize;				/**< Size of the instance data in bytes */
};


// interface functions

/**
 * Add a material definitions file.
 * Add a material definitions file where material definitions can be found.
 * @param pName Name for material definitions.
 * @param pFilename Filename of definitions file.
 * @return If no error occurs, 0 is returned.
 */
MF_API int MFMaterial_AddDefinitionsFile(const char *pName, const char *pFilename);

/**
 * Add a material definitions ini.
 * Add a material definitions ini where material definitions can be found.
 * @param pName Name for material definitions.
 * @param pMatDefs Pointer to an ini file containing material definitions.
 * @return If no error occurs, 0 is returned.
 */
MF_API int MFMaterial_AddDefinitionsIni(const char *pName, MFIni *pMatDefs);

/**
 * Remove a material definitions file.
 * Removes a material definitions file.
 * @param pName Name of material definitions to remove.
 * @return None.
 */
MF_API void MFMaterial_RemoveDefinitions(const char *pName);

/**
 * Create a material instance.
 * Creates an instance of a material.
 * @param pName Name of material to create.
 * @return Returns a new instance of the target material.
 */
MF_API MFMaterial*	MFMaterial_Create(const char *pName);

/**
 * Release an MFMaterial instance.
 * Release a reference to an MFMaterial and destroy when the reference count reaches 0.
 * @param pMaterial Material instance to destroy.
 * @return Returns the new number of references to the material. If 0 is returned, there are no more instances and the material will be destroyed.
 */
MF_API int MFMaterial_Release(MFMaterial *pMaterial);

/**
 * Find a material.
 * Finds a material instance.
 * @param pName Name of the material to find.
 * @return Returns a pointer to the material. If the material was not found, NULL is returned.
 * @remarks Note that MFMaterial_Find increments the materials reference count so you must release the reference when finished.
 */
MF_API MFMaterial*	MFMaterial_Find(const char *pName);

/**
 * Get a pointer to a stock material.
 * Gets a pointer to one of the built in 'stock' materials.
 * @param materialIdentifier Identifier of the stock material.
 * @return Returns a pointer to the specified stock material.
 */
MF_API MFMaterial*	MFMaterial_GetStockMaterial(MFStockMaterials materialIdentifier);

/**
 * Set the current active material.
 * Sets the current active material.
 * @param pMaterial Pointer to a material to set as the active material.
 * @return None.
 */
MF_API void MFMaterial_SetMaterial(const MFMaterial *pMaterial);

/**
 * Get the current active material.
 * Gets the current active material.
 * @return Pointer to the current active material.
 */
MF_API MFMaterial*	MFMaterial_GetCurrent();

/**
 * Get a materials name.
 * Gets a materials name.
 * @return The name of the specified material.
 */
MF_API const char* MFMaterial_GetMaterialName(MFMaterial *pMaterial);

/**
 * Get a materials state block.
 * Gets a materials state block.
 * @return An MFStateBlock representing the given material.
 */
MF_API MFStateBlock* MFMaterial_GetMaterialStateBlock(MFMaterial *pMaterial);

// material control

/**
 * Get the number of parameters the material exposes.
 * Gets the number of parameters the material exposes.
 * @param pMaterial Pointer to a material instance.
 * @return Returns the number of parameters exposed by the material.
 */
MF_API int MFMaterial_GetNumParameters(MFMaterial *pMaterial);

/**
 * Get the name of a parameter.
 * Gets the name of a material parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Parameter index.
 * @return Returns a string representing the parameter name.
 */
MF_API const char*	MFMaterial_GetParameterName(MFMaterial *pMaterial, int parameterIndex);

/**
 * Get the index of a parameter from a parameter name.
 * Gets the index of a parameter from a parameter name.
 * @param pMaterial Pointer to a material instance.
 * @param pParameterName String representing a parameter name.
 * @return Returns the index of the parameter. -1 if the parameter does not exist.
 */
MF_API int MFMaterial_GetParameterIndexFromName(MFMaterial *pMaterial, const char *pParameterName);

/**
 * Get parameter info by name.
 * Gets information about the specified material parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Parameter index.
 * @return Returns a pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
 */
MF_API MFMaterialParameterInfo *MFMaterial_GetParameterInfo(MFMaterial *pMaterial, int parameterIndex);

/**
 * Get parameter info by name.
 * Gets information about the specified material parameter.
 * @param pMaterial Pointer to a material instance.
 * @param pParameterName String representing a parameter name.
 * @return Returns a pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
 */
MF_API MFMaterialParameterInfo *MFMaterial_GetParameterInfoFromName(MFMaterial *pMaterial, const char *pParameterName);

/**
 * Get the value of a parameter.
 * Gets the value of a parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @param pValue Pointer to a buffer to receive non-integer parameter data.
 * @return Returns the value of integer or poitner parameters, otherwise returns 0 on success.
 */
MF_API uintp MFMaterial_GetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, void *pValue = NULL);

/**
 * Get the value of an integer parameter.
 * Gets the value of an integer parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @return Returns the value contained in the integer parameter.
 */
inline uintp MFMaterial_GetParameterI(MFMaterial *pMaterial, int parameterIndex, int argIndex)
{
	return MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex);
}

/**
 * Get the value of a float parameter.
 * Gets the value of a float parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @return Returns the value contained in the float parameter.
 */
inline float MFMaterial_GetParameterF(MFMaterial *pMaterial, int parameterIndex, int argIndex)
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
inline int MFMaterial_GetParameterV(MFMaterial *pMaterial, int parameterIndex, int argIndex, MFVector *pVector)
{
	return (int)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pVector);
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
inline int MFMaterial_GetParameterM(MFMaterial *pMaterial, int parameterIndex, int argIndex, MFMatrix *pMatrix)
{
	return (int)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pMatrix);
}

/**
 * Get the value of a texture parameter.
 * Gets the value of a texture parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @return Returns the MFTexture set to the parameter requested.
 */
inline MFTexture *MFMaterial_GetParameterT(MFMaterial *pMaterial, int parameterIndex, int argIndex)
{
	return (MFTexture*)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex);
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
MF_API void MFMaterial_SetParameter(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value);

/**
 * Set the value of an integer parameter.
 * Sets the value of an integer parameter.
 * @param pMaterial Pointer to a material instance.
 * @param parameterIndex Index of the parameter
 * @param argIndex Argument index of the parameter.
 * @param value Value of the integer parameter being set.
 * @return None.
 */
inline void MFMaterial_SetParameterI(MFMaterial *pMaterial, int parameterIndex, int argIndex, uintp value)
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
inline void MFMaterial_SetParameterF(MFMaterial *pMaterial, int parameterIndex, int argIndex, float value)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, (uintp)&value);
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
inline void MFMaterial_SetParameterV(MFMaterial *pMaterial, int parameterIndex, int argIndex, const MFVector &vector)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, (uintp)&vector);
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
inline void MFMaterial_SetParameterM(MFMaterial *pMaterial, int parameterIndex, int argIndex, const MFMatrix &matrix)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, (uintp)&matrix);
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
inline void MFMaterial_SetParameterT(MFMaterial *pMaterial, int parameterIndex, int argIndex, const MFTexture *pTexture)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, (uintp)pTexture);
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
inline void MFMaterial_SetParameterS(MFMaterial *pMaterial, int parameterIndex, int argIndex, const char *pString)
{
	MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, (uintp)pString);
}

/**
 * Register a new material type.
 * Registers a new material type with the material system.
 * @param pName The name of the new material type.
 * @param pCallbacks Pointer to an MFMaterialCallbacks structure which defines a set of callbacks to interact with the material.
 * @param instanceDataSize Size of the instance data allocation in bytes.
 * @return None.
 */
MF_API void MFMaterial_RegisterMaterialType(const char *pName, const MFMaterialCallbacks *pCallbacks, size_t instanceDataSize);

/**
 * Unregister a material type.
 * Unregisters a material type with the material system.
 * @param pName The name of the material type to de-register.
 * @return None.
 */
MF_API void MFMaterial_UnregisterMaterialType(const char *pName);


// C++ API
#include "Fuji/MFResource.h"

namespace Fuji
{
	class Material : public Resource
	{
	public:
		inline Material()																				{}
		inline Material(const Material &from)															: Resource(from) {}
		inline Material(MFMaterial *pFrom)																: Resource((MFResource*)pFrom) {}
		inline Material(const char *pName)																{ pResource = (MFResource*)MFMaterial_Create(pName); }
//		inline ~Material()																				{ if(pMaterial) MFMaterial_Release(pMaterial); }

		inline Material& operator=(const Material& from)
		{
			Release();
			pResource = from.pResource;
			AddRef();
			return *this;
		}

		inline operator MFMaterial*()																	{ return (MFMaterial*)pResource; }

		inline Material& Create(const char *pName)
		{
			Release();
			pResource = (MFResource*)MFMaterial_Create(pName);
			return *this;
		}

		inline const char *Name() const																	{ return MFMaterial_GetMaterialName((MFMaterial*)pResource); }
		inline MFStateBlock* StateBlock()																{ return MFMaterial_GetMaterialStateBlock((MFMaterial*)pResource); }
		inline const MFStateBlock* StateBlock() const													{ return MFMaterial_GetMaterialStateBlock((MFMaterial*)pResource); }

		inline int NumParameters() const																{ return MFMaterial_GetNumParameters((MFMaterial*)pResource); }
		inline const char* ParameterName(int parameterIndex) const										{ return MFMaterial_GetParameterName((MFMaterial*)pResource, parameterIndex); }
		inline int ParameterIndexFromName(const char *pParameterName) const								{ return MFMaterial_GetParameterIndexFromName((MFMaterial*)pResource, pParameterName); }

		inline const MFMaterialParameterInfo* ParameterInfo(int parameterIndex) const					{ return MFMaterial_GetParameterInfo((MFMaterial*)pResource, parameterIndex); }
		inline const MFMaterialParameterInfo* ParameterInfoFromName(const char *pParameterName) const	{ return MFMaterial_GetParameterInfoFromName((MFMaterial*)pResource, pParameterName); }

		inline uintp GetParameter(int parameterIndex, int argIndex, void *pValue = NULL) const			{ return MFMaterial_GetParameter((MFMaterial*)pResource, parameterIndex, argIndex, pValue); }

		inline void SetParameter(int parameterIndex, int argIndex, uintp value)							{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, value); }
		inline void SetParameter(int parameterIndex, int argIndex, float value)							{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, (uintp)&value); }
		inline void SetParameter(int parameterIndex, int argIndex, const MFVector &vector)				{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, (uintp)&vector); }
		inline void SetParameter(int parameterIndex, int argIndex, const MFMatrix &matrix)				{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, (uintp)&matrix); }
		inline void SetParameter(int parameterIndex, int argIndex, const MFTexture *pTexture)			{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, (uintp)pTexture); }
		inline void SetParameter(int parameterIndex, int argIndex, const char *pString)					{ MFMaterial_SetParameter((MFMaterial*)pResource, parameterIndex, argIndex, (uintp)pString); }
	};
}

#endif // _MFMATERIAL_H

/** @} */
