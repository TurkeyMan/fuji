module fuji.material;

public import fuji.c.MFMaterial;

import fuji.resource;
import fuji.texture;
import fuji.matrix;

nothrow:
@nogc:

struct Material
{
	alias resource this;

	MFMaterial *pMaterial;

nothrow:
@nogc:
	this(this) pure
	{
		addRef();
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Material)
		{
			pMaterial = cast(MFMaterial*)resource.handle;
			addRef();
		}
	}

	this(const(char)[] name)
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

	void opAssign(MFMaterial *pMaterial)
	{
		release();
		this.pMaterial = pMaterial;
		addRef();
	}

	void opAssign(Material material)
	{
		release();
		pMaterial = material.pMaterial;
		addRef();
	}

	void create(const(char)[] name)
	{
		release();
		auto s = Stringz!(64)(name);
		pMaterial = MFMaterial_Create(s);
	}

	void createExisting(const(char)[] name)
	{
		release();
		auto s = Stringz!(64)(name);
		pMaterial = MFMaterial_Find(s);
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

	void setCurrent()
	{
		MFMaterial_SetMaterial(pMaterial);
	}

	@property inout(MFMaterial)* handle() inout pure { return pMaterial; }
	@property ref inout(Resource) resource() inout pure	{ return *cast(inout(Resource)*)&this; }

	@property const(char)[] name() const pure
	{
		return MFMaterial_GetMaterialName(pMaterial).toDStr;
	}

	@property inout(Parameters) parameters() inout
	{
		return inout(Parameters)(pMaterial, 0, MFMaterial_GetNumParameters(pMaterial));
	}

	struct Parameter
	{
//		alias asInt this;
//		alias asFloat this;

	nothrow:
	//@nogc:
		this(MFMaterial* pMaterial, size_t index) @nogc
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

		bool opCast(T)() const pure @nogc if(is(T == bool))
		{
			return pParameterInfo != null;
		}

		@property const(char)[] name() const pure @nogc
		{
			return pParameterInfo.pParameterName.toDStr;

		}

		@property ref const(MFMaterialParameterInfo) info() const pure @nogc
		{
			return *pParameterInfo;
		}

		Parameter opIndex(int i) pure //@nogc
		{
			Parameter p = this;

			if(pParameterInfo.argIndexHigh.type == MFParamType.Enum || pParameterInfo.argIndexHigh.type == MFParamType.Int)
			{
				assert(pParameterInfo.argIndex.type != MFParamType.Enum && pParameterInfo.argIndex.type != MFParamType.Int, "Parameters with 2 arguments are not (yet) supported >_<");
				
				p.parameterArg = (parameterArg & 0xFFFF) | (i << 16);
				p.bHiwordSet = true;
			}
			else if(pParameterInfo.argIndex.type == MFParamType.Enum)
			{
				p.parameterArg = (parameterArg & 0xFFFF0000) | i;
				p.bLowordSet = true;
			}
			else
				assert(false, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is not indexable.");

			return p;
		}

		Parameter opIndex(const(char)[] s) pure //@nogc
		{
			Parameter p = this;

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
					p.parameterArg = bHigh ? (parameterArg & 0xFFFF) | (pKey.value << 16) : (parameterArg & 0xFFFF0000) | pKey.value;
					if(bHigh)
						p.bHiwordSet = true;
					else
						p.bLowordSet = true;
					return p;
				}
				++pKey;
			}
			assert(false, "Invalid enum key '" ~ s ~ "' for material parameter: " ~ pParameterInfo.pParameterName.toDStr);
		}

		bool ValidateArg(MFParamType[] types ...) const pure //@nogc
		{
			assert(pParameterInfo.numValues == 1, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: struct");
// FIXME	assert(std.algorithm.canFind(types, pParameterInfo.pValues[0].type), "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: " ~ std.conv.to!string(pParameterInfo.pValues[0].type));
			assert(std.algorithm.canFind(types, pParameterInfo.pValues[0].type), "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: " ~ std.conv.to!string(cast(int)pParameterInfo.pValues[0].type));
			return true;
		}

		// getters...
		@property int asInt() const //@nogc
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum, MFParamType.Bool);
			return cast(int)GetParameter(null);
		}
		@property float asFloat() const //@nogc
		{
			debug ValidateArg(MFParamType.Float);
			float value;
			GetParameter(&value);
			return value;
		}
		@property const(char)[] asString() const //@nogc
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				const(char)* value = cast(const(char)*)GetParameter(null);
				return value.toDStr;
			}
			else// if(pParameterInfo.pValues[0].type == MFParamType.Enum)
			{
				// TODO: return enum key for value...
				assert(false, "TODO");
			}
		}
		@property bool asBool() const //@nogc
		{
			debug ValidateArg(MFParamType.Bool);
			return GetParameter(null) != 0;
		}
		@property MFVector asVector() const //@nogc
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			MFVector v;
			GetParameter(&v);
			return v;
		}
		@property MFMatrix asMatrix() const //@nogc
		{
			debug ValidateArg(MFParamType.Matrix);
			MFMatrix m;
			GetParameter(&m);
			return m;
		}
		@property Texture asTexture() const //@nogc
		{
			// HACK: fuji material system needs to be tidied up!
			debug ValidateArg(MFParamType.String); // **String?**
			size_t t = GetParameter(null);
			Texture tex;
			tex.pTexture = cast(MFTexture*)t;
			tex.addRef();
			return tex;
		}

		// setters...
		void opAssign(int i) //@nogc
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum);
			SetParameter(i);
		}
		void opAssign(float f) //@nogc
		{
			debug ValidateArg(MFParamType.Float);
			SetParameter(cast(size_t)&f);
		}
		void opAssign(const(char)[] s) //@nogc
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				auto str = Stringz!(32)(s);
				SetParameter(cast(size_t)str);
			}
			else// if(pParameterInfo.pValues[0].type == MFParamType.Enum)
			{
				const(MFEnumKey)* pKey = pParameterInfo.pValues[0].pEnumKeys;
				while(pKey && pKey.pKey)
				{
					// TODO: should this be case insensitive?
					if(pKey.pKey[0..s.length] == s[] && pKey.pKey[s.length] == 0)
					{
						SetParameter(pKey.value);
						return;
					}
					++pKey;
				}
				assert(false, "Invalid enum key '" ~ s ~ "' for material parameter: " ~ pParameterInfo.pParameterName.toDStr);
			}
		}
		void opAssign(bool b) //@nogc
		{
			debug ValidateArg(MFParamType.Bool);
			SetParameter(b ? 1 : 0);
		}
		void opAssign(ref in MFVector v) //@nogc
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			SetParameter(cast(size_t)&v);
		}
		void opAssign(ref in MFMatrix m) //@nogc
		{
			debug ValidateArg(MFParamType.Matrix);
			SetParameter(cast(size_t)&m);
		}
		void opAssign(const Texture t)
		{
			// HACK: fuji material system needs to be tidied up!
			assert(false, "TODO: fuji needs a texture type...");
//			SetParameter(cast(size_t)t.pTexture);
		}

	private:
		size_t GetParameter(void* pValue = null) const //@nogc
		{
			assert(pParameterInfo.argIndex.type == MFParamType.None || bLowordSet, "Argument index not specified!");
			assert(pParameterInfo.argIndexHigh.type == MFParamType.None || bHiwordSet, "Argument index not specified!");
			return MFMaterial_GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, pValue);
		}
		void SetParameter(size_t value) //@nogc
		{
			assert(pParameterInfo.argIndex.type == MFParamType.None || bLowordSet, "Argument index not specified!");
			assert(pParameterInfo.argIndexHigh.type == MFParamType.None || bHiwordSet, "Argument index not specified!");
			MFMaterial_SetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, value);
		}

		MFMaterial* pMaterial;
		const(MFMaterialParameterInfo)* pParameterInfo;
		int parameterArg;
		bool bLowordSet, bHiwordSet;
	}

	alias Parameters = ResourceRange!(MFMaterial, Parameter, MFMaterial_GetParameterIndexFromName, true, true);
}
