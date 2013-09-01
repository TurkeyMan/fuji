module fuji.material;

public import fuji.fuji;
import fuji.texture;
import fuji.matrix;

struct MFIni;
struct MFIniEnumKey;

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
		MFIniEnumKey* pEnumKeys;
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
	int       function(void* pPlatformData) pRegisterMaterial;	/**< Pointer to the RegisterMaterial function */
	void      function() pUnregisterMaterial;					/**< Pointer to the UnregisterMaterial function */

	void      function(MFMaterial* pMaterial) pCreateInstance;	/**< Pointer to the CreateInstance function */
	void      function(MFMaterial* pMaterial) pDestroyInstance;	/**< Pointer to the DestroyInstance function */

	int       function(MFMaterial* pMaterial) pBegin;			/**< Pointer to the Begin function */

	void      function(MFMaterial* pMaterial) pUpdate;			/**< Pointer to the Update function */

	int       function() pGetNumParams;							/**< Pointer to the GetNumParams function */
	MFMaterialParameterInfo* function(int parameterIndex) pGetParameterInfo;	/**< Pointer to the GetParameterInfo function */
	void      function(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value) pSetParameter;		/**< Pointer to a SetParameter function */
	size_t    function(MFMaterial* pMaterial, int parameterIndex, int argIndex, void* pValue) pGetParameter;	/**< Pointer to the GetParameter function */
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
* @return Returns a new instance of the target material.
*/
extern (C) MFMaterial* MFMaterial_Create(const(char)* pName);

/**
* Release an MFMaterial instance.
* Release a reference to an MFMaterial and destroy when the reference count reaches 0.
* @param pMaterial Material instance to destroy.
* @return Returns the new number of references to the material. If 0 is returned, there are no more instances and the material will be destroyed.
*/
extern (C) int MFMaterial_Release(MFMaterial* pMaterial);

/**
* Find a material.
* Finds a material instance.
* @param pName Name of the material to find.
* @return Returns a pointer to the material. If the material was not found, NULL is returned.
* @remarks Note that MFMaterial_Find increments the materials reference count so you must release the reference when finished.
*/
extern (C) MFMaterial* MFMaterial_Find(const(char)* pName);

/**
* Get a pointer to a stock material.
* Gets a pointer to one of the built in 'stock' materials.
* @param materialIdentifier Identifier of the stock material.
* @return Returns a pointer to the specified stock material.
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
extern (C) immutable(char)* MFMaterial_GetMaterialName(MFMaterial* pMaterial) pure;

// material control

/**
* Get the number of parameters the material exposes.
* Gets the number of parameters the material exposes.
* @param pMaterial Pointer to a material instance.
* @return Returns the number of parameters exposed by the material.
*/
extern (C) int MFMaterial_GetNumParameters(MFMaterial* pMaterial);

/**
* Get the name of a parameter.
* Gets the name of a material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return Returns a string representing the parameter name.
*/
extern (C) immutable(char)* MFMaterial_GetParameterName(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get the index of a parameter from a parameter name.
* Gets the index of a parameter from a parameter name.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return Returns the index of the parameter. -1 if the parameter does not exist.
*/
extern (C) int MFMaterial_GetParameterIndexFromName(MFMaterial* pMaterial, const(char)* pParameterName);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return Returns a pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) immutable(MFMaterialParameterInfo)* MFMaterial_GetParameterInfo(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return Returns a pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) immutable(MFMaterialParameterInfo)* MFMaterial_GetParameterInfoFromName(MFMaterial* pMaterial, const(char)* pParameterName);

/**
* Get the value of a parameter.
* Gets the value of a parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pValue Pointer to a buffer to receive non-integer parameter data.
* @return Returns the value of integer or poitner parameters, otherwise returns 0 on success.
*/
extern (C) size_t MFMaterial_GetParameter(MFMaterial* pMaterial, int parameterIndex, int argIndex, void* pValue = null);

/**
* Get the value of an integer parameter.
* Gets the value of an integer parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return Returns the value contained in the integer parameter.
*/
/+inline+/ size_t MFMaterial_GetParameterI(MFMaterial* pMaterial, int parameterIndex, int argIndex)
{
	return MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, null);
}

/**
* Get the value of a float parameter.
* Gets the value of a float parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return Returns the value contained in the float parameter.
*/
/+inline+/ float MFMaterial_GetParameterF(MFMaterial* pMaterial, int parameterIndex, int argIndex)
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
/+inline+/ int MFMaterial_GetParameterV(MFMaterial* pMaterial, int parameterIndex, int argIndex, MFVector* pVector)
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
/+inline+/ int MFMaterial_GetParameterM(MFMaterial* pMaterial, int parameterIndex, int argIndex, MFMatrix* pMatrix)
{
	return cast(int)MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pMatrix);
}

/**
* Get the value of a texture parameter.
* Gets the value of a texture parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return Returns the MFTexture set to the parameter requested.
*/
/+inline+/ MFTexture* MFMaterial_GetParameterT(MFMaterial* pMaterial, int parameterIndex, int argIndex)
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


import std.c.string;
import std.string;

final class Material
{
	static Material create(in string name)								{ return cast(Material)MFMaterial_Create(name.toStringz()); }
	static Material find(in string name)								{ return cast(Material)MFMaterial_Find(name.toStringz()); }
	static Material stockMaterial(MFStockMaterials materialIdentifier)	{ return cast(Material)MFMaterial_GetStockMaterial(materialIdentifier); }

	int release()														{ return MFMaterial_Release(cast(MFMaterial*)this); }

	@property MFMaterial* handle() pure nothrow							{ return cast(MFMaterial*)this; }
	@property const(MFMaterial)* handle() const pure nothrow			{ return cast(const(MFMaterial)*)this; }

	@property string name() const pure
	{
		auto pName = MFMaterial_GetMaterialName(cast(MFMaterial*)this);
		return pName[0..strlen(pName)];
	}

	@property Parameters parameters()	{ return Parameters(cast(MFMaterial*)this, 0, MFMaterial_GetNumParameters(cast(MFMaterial*)this)); }

	struct Parameter
	{
		bool opCast(T)() if(is(T == bool))	{ return index != -1; }

		@property string name() const
		{
			auto pName = MFMaterial_GetParameterName(pMaterial, cast(int)index);
			return pName[0..strlen(pName)];
		}

		@property ref immutable(MFMaterialParameterInfo) info() const
		{
			return *MFMaterial_GetParameterInfo(pMaterial, cast(int)index);
		}

        // assignment operators
        // cast operators

		// setters and getters...

	private:
		MFMaterial* pMaterial;
		size_t index;
	}

	struct Parameters
	{
		@property bool empty() const pure nothrow					{ return offset == count; }
		@property size_t length() const pure nothrow				{ return count - offset; }

		@property Parameters save() pure nothrow					{ return this; }

		@property Parameter front() pure nothrow					{ return Parameter(pMaterial, offset); }
		@property Parameter back() pure nothrow						{ return Parameter(pMaterial, count-1); }

		Parameter opIndex(size_t index) pure nothrow				{ return Parameter(pMaterial, offset + cast(int)index); }
		Parameter opIndex(string name)      				        { return find(name); }
		Parameters opSlice(size_t x, size_t y) pure nothrow			{ return Parameters(pMaterial, offset + x, offset + y); }

		void popFront() pure nothrow								{ ++offset; }
		void popBack() pure nothrow									{ --count; }

		Parameter find(string name)
        {
            int index = MFMaterial_GetParameterIndexFromName(pMaterial, name.toStringz());
            if(index < offset || index >= count)
                return Parameter(pMaterial, -1);
            return Parameter(pMaterial, index);
        }

	private:
		MFMaterial* pMaterial;
		size_t offset, count;
	}
}
