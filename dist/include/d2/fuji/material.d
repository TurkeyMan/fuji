module fuji.material;

public import fuji.c.MFMaterial;

import fuji.resource;
import fuji.texture;
import fuji.matrix;

struct Material
{
	MFMaterial *pMaterial;

	this(this) pure nothrow
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pMaterial);
	}

	this(ref Resource resource) pure nothrow
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Material)
		{
			resource.AddRef();
			pMaterial = cast(MFMaterial*)resource.handle;
		}
	}

	this(const(char)[] name) nothrow
	{
		create(name);
	}

	this(MFStockMaterials materialIdentifier) nothrow
	{
		createStockMaterial(materialIdentifier);
	}

	~this() nothrow
	{
		release();
	}

	void create(const(char)[] name) nothrow
	{
		release();
		pMaterial = MFMaterial_Create(name.toStringz());
	}

	void createExisting(const(char)[] name) nothrow
	{
		release();
		pMaterial = MFMaterial_Find(name.toStringz());
	}

	void createStockMaterial(MFStockMaterials materialIdentifier) nothrow
	{
		release();
		pMaterial = MFMaterial_GetStockMaterial(materialIdentifier);
	}

	int release() nothrow
	{
		int rc = 0;
		if(pMaterial)
		{
			rc = MFMaterial_Release(pMaterial);
			pMaterial = null;
		}
		return rc;
	}

	void setCurrent() nothrow
	{
		MFMaterial_SetMaterial(pMaterial);
	}

	@property inout(MFMaterial)* handle() inout pure nothrow { return pMaterial; }
	@property ref inout(Resource) resource() inout pure nothrow	{ return *cast(inout(Resource)*)&this; }

	@property const(char)[] name() const pure nothrow
	{
		return MFMaterial_GetMaterialName(pMaterial).toDStr;
	}

	@property inout(Parameters) parameters() inout nothrow
	{
		return inout(Parameters)(pMaterial, 0, MFMaterial_GetNumParameters(pMaterial));
	}

	struct Parameter
	{
//		alias asInt this;
//		alias asFloat this;

		this(MFMaterial* pMaterial, size_t index) nothrow
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

		bool opCast(T)() const pure nothrow if(is(T == bool))
		{
			return pParameterInfo != null;
		}

		@property const(char)[] name() const pure nothrow
		{
			return pParameterInfo.pParameterName.toDStr;

		}

		@property ref const(MFMaterialParameterInfo) info() const pure nothrow
		{
			return *pParameterInfo;
		}

		Parameter opIndex(int i) pure nothrow
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

		Parameter opIndex(const(char)[] s) pure nothrow
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

		bool ValidateArg(MFParamType[] types ...) const pure
		{
			assert(pParameterInfo.numValues == 1, "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: struct");
			assert(std.algorithm.canFind(types, pParameterInfo.pValues[0].type), "Material parameter '" ~ pParameterInfo.pParameterName.toDStr ~ "' is incorrect type: " ~ std.conv.to!string(pParameterInfo.pValues[0].type));
			return true;
		}

		// getters...
		@property int asInt() const
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum, MFParamType.Bool);
			return cast(int)GetParameter(null);
		}
		@property float asFloat() const
		{
			debug ValidateArg(MFParamType.Float);
			float value;
			GetParameter(&value);
			return value;
		}
		@property const(char)[] asString() const
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				const(char)* value;
				value = cast(const(char)*)GetParameter(null);
				return value.toDStr;
			}
			else// if(pParameterInfo.pValues[0].type == MFParamType.Enum)
			{
				// TODO: return enum key for value...
				assert(false, "TODO");
			}
		}
		@property bool asBool() const
		{
			debug ValidateArg(MFParamType.Bool);
			return GetParameter(null) != 0;
		}
		@property MFVector asVector() const
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			MFVector v;
			GetParameter(&v);
			return v;
		}
		@property MFMatrix asMatrix() const
		{
			debug ValidateArg(MFParamType.Matrix);
			MFMatrix m;
			GetParameter(&m);
			return m;
		}
		@property Texture asTexture() const
		{
			// HACK: fuji material system needs to be tidied up!
			debug ValidateArg(MFParamType.String); // **String?**
			size_t t = GetParameter(null);
			Texture tex;
			tex.pTexture = cast(MFTexture*)t;
			MFResource_AddRef(cast(fuji.resource.MFResource*)tex.pTexture);
			return tex;
		}

		// setters...
		void opAssign(int i)
		{
			debug ValidateArg(MFParamType.Int, MFParamType.Enum);
			SetParameter(i);
		}
		void opAssign(float f)
		{
			debug ValidateArg(MFParamType.Float);
			SetParameter(cast(size_t)&f);
		}
		void opAssign(const(char)[] s)
		{
			debug ValidateArg(MFParamType.String, MFParamType.Enum);
			if(pParameterInfo.pValues[0].type == MFParamType.String)
			{
				SetParameter(cast(size_t)s.toStringz);
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
		void opAssign(bool b)
		{
			debug ValidateArg(MFParamType.Bool);
			SetParameter(b ? 1 : 0);
		}
		void opAssign(ref in MFVector v)
		{
			debug ValidateArg(MFParamType.Vector3, MFParamType.Vector4);
			SetParameter(cast(size_t)&v);
		}
		void opAssign(ref in MFMatrix m)
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
		size_t GetParameter(void* pValue = null) const nothrow
		{
			assert(pParameterInfo.argIndex.type == MFParamType.None || bLowordSet, "Argument index not specified!");
			assert(pParameterInfo.argIndexHigh.type == MFParamType.None || bHiwordSet, "Argument index not specified!");
			return MFMaterial_GetParameter(pMaterial, pParameterInfo.parameterIndex, parameterArg, pValue);
		}
		void SetParameter(size_t value) nothrow
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
