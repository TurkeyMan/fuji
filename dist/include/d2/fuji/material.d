module fuji.material;

public import fuji.fuji;
import fuji.resource;
import fuji.texture;
import fuji.matrix;

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
	extern (C) int       function(void* pPlatformData) pRegisterMaterial;	/**< Pointer to the RegisterMaterial function */
	extern (C) void      function() pUnregisterMaterial;					/**< Pointer to the UnregisterMaterial function */

	extern (C) void      function(MFMaterial* pMaterial) pCreateInstance;	/**< Pointer to the CreateInstance function */
	extern (C) void      function(MFMaterial* pMaterial) pDestroyInstance;	/**< Pointer to the DestroyInstance function */

	extern (C) int       function(MFMaterial* pMaterial) pBegin;			/**< Pointer to the Begin function */

	extern (C) void      function(MFMaterial* pMaterial) pUpdate;			/**< Pointer to the Update function */

	extern (C) int       function() pGetNumParams;							/**< Pointer to the GetNumParams function */
	extern (C) MFMaterialParameterInfo* function(int parameterIndex) pGetParameterInfo;	/**< Pointer to the GetParameterInfo function */
	extern (C) void      function(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value) pSetParameter;	/**< Pointer to a SetParameter function */
	extern (C) size_t    function(MFMaterial* pMaterial, int parameterIndex, int argIndex, void* pValue) pGetParameter;	/**< Pointer to the GetParameter function */
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
* @return The new number of references to the material. If 0 is returned, there are no more instances and the material will be destroyed.
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
extern (C) immutable(char)* MFMaterial_GetMaterialName(MFMaterial* pMaterial) pure;

// material control

/**
* Get the number of parameters the material exposes.
* Gets the number of parameters the material exposes.
* @param pMaterial Pointer to a material instance.
* @return The number of parameters exposed by the material.
*/
extern (C) int MFMaterial_GetNumParameters(MFMaterial* pMaterial);

/**
* Get the name of a parameter.
* Gets the name of a material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return A string representing the parameter name.
*/
extern (C) immutable(char)* MFMaterial_GetParameterName(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get the index of a parameter from a parameter name.
* Gets the index of a parameter from a parameter name.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return The index of the parameter. -1 if the parameter does not exist.
*/
extern (C) int MFMaterial_GetParameterIndexFromName(MFMaterial* pMaterial, const(char)* pParameterName);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Parameter index.
* @return A pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) immutable(MFMaterialParameterInfo)* MFMaterial_GetParameterInfo(const(MFMaterial)* pMaterial, int parameterIndex);

/**
* Get parameter info by name.
* Gets information about the specified material parameter.
* @param pMaterial Pointer to a material instance.
* @param pParameterName String representing a parameter name.
* @return A pointer to a struct containing the associated parameter info or NULL if parameter does not exist.
*/
extern (C) immutable(MFMaterialParameterInfo)* MFMaterial_GetParameterInfoFromName(MFMaterial* pMaterial, const(char)* pParameterName);

/**
* Get the value of a parameter.
* Gets the value of a parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @param pValue Pointer to a buffer to receive non-integer parameter data.
* @return The value of integer or poitner parameters, otherwise returns 0 on success.
*/
extern (C) size_t MFMaterial_GetParameter(MFMaterial* pMaterial, int parameterIndex, int argIndex, void* pValue = null);

/**
* Get the value of an integer parameter.
* Gets the value of an integer parameter.
* @param pMaterial Pointer to a material instance.
* @param parameterIndex Index of the parameter
* @param argIndex Argument index of the parameter.
* @return The value contained in the integer parameter.
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
* @return The value contained in the float parameter.
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
* @return The MFTexture set to the parameter requested.
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


// wrappers for D...

struct Material
{
	alias pMaterial this;

	this(this)
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pMaterial);
	}

	this(Resource resource)
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Material)
		{
			resource.AddRef();
			pMaterial = cast(MFMaterial*)resource.handle;
		}
	}

	this(in string name)
	{
		create(name);
	}

	this(MFStockMaterials materialIdentifier)
	{
		createStockMaterial(materialIdentifier);
	}

	~this()
	{
		release();
	}

	void create(in string name)
	{
		release();
		pMaterial = MFMaterial_Create(name.toStringz());
	}

	void createExisting(in string name)
	{
		release();
		pMaterial = MFMaterial_Find(name.toStringz());
	}

	void createStockMaterial(MFStockMaterials materialIdentifier)
	{
		release();
		pMaterial = MFMaterial_GetStockMaterial(materialIdentifier);
	}

	int release()
	{
		int rc = 0;
		if(pMaterial)
		{
			rc = MFMaterial_Release(pMaterial);
			pMaterial = null;
		}
		return rc;
	}

	@property inout(MFMaterial)* handle() inout pure nothrow { return pMaterial; }

	@property string name() const pure
	{
		auto pName = MFMaterial_GetMaterialName(cast(MFMaterial*)this);
		return pName.toDStr;
	}

	@property Parameters parameters()	{ return Parameters(cast(MFMaterial*)this, 0, MFMaterial_GetNumParameters(cast(MFMaterial*)this)); }

	struct Parameter
	{
//		alias asInt this;
//		alias asFloat this;

		this(MFMaterial* pMaterial, size_t index)
		{
			this.pMaterial = pMaterial;
			pParameterInfo = index != -1 ? MFMaterial_GetParameterInfo(pMaterial, cast(int)index) : null;
			if(pParameterInfo)
			{
				if(pParameterInfo.argIndex.type == MFParamType.Constant)
				{
					parameterArg = pParameterInfo.argIndex.defaultValue;
					bLowordSet = true;
				}
				if(pParameterInfo.argIndexHigh.type == MFParamType.Constant)
				{
					parameterArg |= pParameterInfo.argIndexHigh.defaultValue << 16;
					bHiwordSet = true;
				}
			}
		}

		bool opCast(T)() if(is(T == bool))	{ return pParameterInfo != null; }

		@property const(char)[] name() const
		{
			return pParameterInfo.pParameterName.toDStr;

		}

		@property ref const(MFMaterialParameterInfo) info() const
		{
			return *pParameterInfo;
		}

		Parameter opIndex(int i)
		{
			if(pParameterInfo.argIndexHigh.type == MFParamType.Enum || pParameterInfo.argIndexHigh.type == MFParamType.Int)
			{
				assert(pParameterInfo.argIndex.type != MFParamType.Enum && pParameterInfo.argIndex.type != MFParamType.Int, "Parameters with 2 arguments are not (yet) supported >_<");
				
				parameterArg = (parameterArg & 0xFFFF) | (i << 16);
				bHiwordSet = true;
			}
			else if(pParameterInfo.argIndex.type == MFParamType.Enum)
			{
				parameterArg = (parameterArg & 0xFFFF0000) | i;
				bLowordSet = true;
			}
			else
				assert(false, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is not indexable.");
			return this;
		}

		Parameter opIndex(string s)
		{
			bool bHigh;
			const(MFEnumKey)* pKey;
			if(pParameterInfo.argIndexHigh.type == MFParamType.Enum)
			{
				assert(pParameterInfo.argIndex.type != MFParamType.Enum, "Parameters with 2 arguments are not (yet) supported >_<");
				pKey = pParameterInfo.argIndexHigh.pEnumKeys;
				bHigh = true;
			}
			else if(pParameterInfo.argIndex.type == MFParamType.Enum)
			{
				pKey = pParameterInfo.argIndex.pEnumKeys;
			}
			else
				assert(false, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' does not have an enum index.");
			while(pKey && pKey.pKey)
			{
				// TODO: should this be case insensitive?
				if(pKey.pKey[0..s.length] == s[] && pKey.pKey[s.length] == 0)
				{
					parameterArg = bHigh ? (parameterArg & 0xFFFF) | (pKey.value << 16) : (parameterArg & 0xFFFF0000) | pKey.value;
					if(bHigh)
						bHiwordSet = true;
					else
						bLowordSet = true;
					return this;
				}
				++pKey;
			}
			assert(false, "Invalid enum key '" ~ s ~ "' for material parameter: " ~ pParameterInfo.pParameterName.toDStr);
		}

		bool ValidateArg(MFParamType[] types ...)
		{
			assert(pParameterInfo.numValues == 1, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: struct");
			assert(std.algorithm.canFind(types, pParameterInfo.pValues[0].type), "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: " ~ std.conv.to!string(pParameterInfo.pValues[0].type));
			return true;
		}

		// getters...
		@property int asInt()
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum, MFParamType.Bool);
			return cast(int)GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, null);
		}
		@property float asFloat()
		{
			debug ValidateArg(MFParamType.Float);
			float value;
			GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, &value);
			return value;
		}
		@property const(char)[] asString()
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				const(char)* value;
				value = cast(const(char)*)GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, null);
				return value.toDStr;
			}
			else// if(pParameterInfo.pValues[0].type == MFParamType.Enum)
			{
				// TODO: return enum key for value...
				assert(false, "TODO");
			}
		}
		@property bool asBool()
		{
			debug ValidateArg(MFParamType.Bool);
			return GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, null) != 0;
		}
		@property MFVector asVector()
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			MFVector v;
			GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, &v);
			return v;
		}
		@property MFMatrix asMatrix()
		{
			debug ValidateArg(MFParamType.Matrix);
			MFMatrix m;
			GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, &m);
			return m;
		}
		@property Texture asTexture()
		{
			// HACK: fuji material system needs to be tidied up!
			debug ValidateArg(MFParamType.String); // **String?**
			size_t t = GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, null);
			Texture tex;
			tex.pTexture = cast(MFTexture*)t;
			MFResource_AddRef(cast(fuji.resource.MFResource*)tex.pTexture);
			return tex;
		}

		// setters...
		void opAssign(int i)
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum);
			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, i);
		}
		void opAssign(float f)
		{
			debug ValidateArg(MFParamType.Float);
			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, cast(size_t)&f);
		}
		void opAssign(string s)
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, cast(size_t)s.toStringz);
			}
			else// if(pParameterInfo.pValues[0].type == MFParamType.Enum)
			{
				const(MFEnumKey)* pKey = pParameterInfo.pValues[0].pEnumKeys;
				while(pKey && pKey.pKey)
				{
					// TODO: should this be case insensitive?
					if(pKey.pKey[0..s.length] == s[] && pKey.pKey[s.length] == 0)
					{
						SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, pKey.value);
						return;
					}
					++pKey;
				}
				assert(false, "Invalid enum key '" ~ s ~ "' for material parameter: " ~ pParameterInfo.pParameterName.toDStr);
			}
		}
		void opAssign(bool b)
		{
			debug ValidateArg(MFParamType.Bool);
			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, b ? 1 : 0);
		}
		void opAssign(ref in MFVector v)
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, cast(size_t)&v);
		}
		void opAssign(ref in MFMatrix m)
		{
			debug ValidateArg(MFParamType.Matrix);
			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, cast(size_t)&m);
		}
		void opAssign(const Texture t)
		{
			// HACK: fuji material system needs to be tidied up!
			assert(false, "TODO: fuji needs a texture type...");
//			SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, cast(size_t)t.pTexture);
		}

	private:
		size_t GetParameter(MFMaterial* pMaterial, int parameterIndex, int argIndex, void* pValue = null)
		{
			assert(pParameterInfo.argIndex.type == MFParamType.None || bLowordSet, "Argument index not specified!");
			assert(pParameterInfo.argIndexHigh.type == MFParamType.None || bHiwordSet, "Argument index not specified!");
			return MFMaterial_GetParameter(pMaterial, parameterIndex, argIndex, pValue);
		}
		void SetParameter(MFMaterial* pMaterial, int parameterIndex, int argIndex, size_t value)
		{
			assert(pParameterInfo.argIndex.type == MFParamType.None || bLowordSet, "Argument index not specified!");
			assert(pParameterInfo.argIndexHigh.type == MFParamType.None || bHiwordSet, "Argument index not specified!");
			MFMaterial_SetParameter(pMaterial, parameterIndex, argIndex, value);
		}

		MFMaterial* pMaterial;
		const(MFMaterialParameterInfo)* pParameterInfo;
		int parameterArg;
		bool bLowordSet, bHiwordSet;
	}

	struct Parameters
	{
		@property bool empty() const pure nothrow			{ return offset == count; }
		@property size_t length() const pure nothrow		{ return count - offset; }

		@property Parameters save() pure nothrow			{ return this; }

		@property Parameter front()							{ return Parameter(pMaterial, offset); }
		@property Parameter back()							{ return Parameter(pMaterial, count-1); }

		Parameter opIndex(size_t index)						{ return Parameter(pMaterial, offset + cast(int)index); }
		Parameter opIndex(string name)						{ return find(name); }
		Parameters opSlice(size_t x, size_t y) pure nothrow	{ return Parameters(pMaterial, offset + x, offset + y); }

		@property Parameter opDispatch(string name)()		{ return find(name); }
		@property Parameter opDispatch(string name, T)(T t)	{ Parameter p = find(name); p = t; return p; }

		void popFront() pure nothrow						{ ++offset; }
		void popBack() pure nothrow							{ --count; }

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

	MFMaterial *pMaterial;
}
