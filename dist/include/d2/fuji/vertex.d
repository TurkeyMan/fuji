module fuji.vertex;

public import fuji.c.MFVertex;

import fuji.resource;
import fuji.vector;
import fuji.render;
import fuji.renderstate;
import fuji.material;

nothrow:
@nogc:

// 'standard' vertex structures
struct MFSimpleVertex
{
	@(MFVertexElementType.Position)
	MFVertexFloat3 pos;

	@(MFVertexElementType.Colour)
	MFVertexUByte4N_BGRA colour;
}

struct MFTextureVertex
{
	@(MFVertexElementType.Position)
	MFVertexFloat3 pos;

	@(MFVertexElementType.Colour)
	MFVertexUByte4N_BGRA colour;

	@(MFVertexElementType.TexCoord)
	MFVertexFloat2 uv;
}

struct MFLitVertex
{
	@(MFVertexElementType.Position)
	MFVertexFloat3 pos;

	@(MFVertexElementType.Normal)
	MFVertexFloat3 normal;

	@(MFVertexElementType.Colour)
	MFVertexUByte4N_BGRA colour;

	@(MFVertexElementType.TexCoord)
	MFVertexFloat2 uv;
}

// helpers to make vertex buffers
MFVertexDeclaration* MFVertex_CreateVertexDeclarationForStruct(VertexStruct)() if(is(VertexStruct == struct))
{
	MFVertexElement[VertexStruct.tupleof.length] elements = void;
	int[MFVertexElementType.Max] elementIndex;

	foreach(i, m; __traits(allMembers, VertexStruct))
	{
		alias T = typeof(__traits(getMember, VertexStruct, m));
		static assert(GetAttribute!(T, MFVertexDataFormat) != MFVertexDataFormat.Unknown, VertexStruct.stringof ~ "." ~ VertexStruct.tupleof[i].stringof ~ " (of type " ~ T.stringof ~ ") has no MFVertexDataType attribute.");
		static assert(GetAttribute!(VertexStruct.tupleof[i], MFVertexElementType) != MFVertexElementType.Unknown, VertexStruct.stringof ~ "." ~ VertexStruct.tupleof[i].stringof ~ " has no MFVertexElementType attribute.");

		MFVertexElementType et = GetAttribute!(__traits(getMember, VertexStruct, m), MFVertexElementType);
		MFVertexDataFormat df = GetAttribute!(T, MFVertexDataFormat);

		elements[i].stream = 0;
		elements[i].elementType = et;
		elements[i].elementIndex = elementIndex[et]++;
		elements[i].componentCount = numElements[df];
		elements[i].format = df;
	}

	return MFVertex_CreateVertexDeclaration(elements.ptr, elements.length);
}

MFVertexBuffer* MFVertex_CreateVertexBufferForStruct(VertexStruct)(MFVertexBufferType type, size_t numVertices, string name = null)
{
	auto n = Stringz!(64)(name);
	MFVertexDeclaration* pDecl = MFVertex_CreateVertexDeclarationForStruct!VertexStruct();
	MFVertexBuffer* pVB = MFVertex_CreateVertexBuffer(pDecl, cast(int)numVertices, type, null, n);
	MFVertex_ReleaseVertexDeclaration(pDecl);
	return pVB;
}

MFVertexBuffer* MFVertex_CreateVertexBufferFromData(VertexStruct)(MFVertexBufferType type, VertexStruct[] vertices, string name = null)
{
	auto n = Stringz!(64)(name);
	MFVertexDeclaration* pDecl = MFVertex_CreateVertexDeclarationForStruct!VertexStruct();
	MFVertexBuffer* pVB = MFVertex_CreateVertexBuffer(pDecl, cast(int)vertices.length, type, vertices.ptr, n);
	MFVertex_ReleaseVertexDeclaration(pDecl);
	return pVB;
}


struct VertexBuffer(VertexDataType = void) if(is(VertexDataType == void) || IsValidVertexStructure!VertexDataType)
{
	MFVertexBuffer* pVB;

nothrow:
@nogc:
	this(this) pure
	{
		pVB.resource.AddRef();
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.VertexBuffer)
		{
			resource.AddRef();
			pVB = cast(MFVertexBuffer*)resource.handle;
		}
	}

	this(MFVertexDeclaration* pVertexFormat, int numVerts, MFVertexBufferType type, string name = null)
	{
		Create(pVertexFormat, numVerts, type, name);
	}

	this(int numVerts, MFVertexBufferType type, string name = null)
	{
		Create(numVerts, type, name);
	}

	~this()
	{
		Release();
	}

	void create(MFVertexDeclaration* pVertexFormat, size_t numVertices, MFVertexBufferType type, string name = null)
	{
		Release();
		auto n = Stringz!(64)(name);
		pVB = MFVertex_CreateVertexBuffer(pVertexFormat, cast(int)numVertices, type, null, n);
	}

	void create(VertexStruct = VertexDataType)(size_t numVertices, MFVertexBufferType type, string name = null)
	{
		Release();

		static if(is(VertexStruct == void))
			assert(false, "Unable to create vertex buffer for 'VertexStruct == " ~ VertexStruct.stringof ~ "'");
		else
			pVB = MFVertex_CreateVertexBufferForStruct!VertexStruct(type, numVertices, name);
	}

	int release()
	{
		int rc = 0;
		if(pVB)
		{
			rc = MFVertex_ReleaseVertexBuffer(pVB);
			pVB = null;
		}
		return rc;
	}

	VertexDataType[] lock()
	{
		void* pLock;
		MFVertex_LockVertexBuffer(pVB, &pLock);

		static if(is(VertexDataType == void))
		{
			// TODO: return a slice! HOW BIG?!
			assert(false, "TODO: we need to be able to get the buffer size for untyped vertex buffers to return a slice");
		}
		else
		{
			return (cast(VertexDataType*)pLock)[0..pVB.numVerts];
		}
	}

	void unlock()
	{
		MFVertex_UnlockVertexBuffer(pVB);
	}

	Mesh getMesh(MFPrimType primType) pure
	{
		return Mesh(pVB, primType);
	}

	void draw(MFPrimType primType, MFMaterial* pMaterial, const MFStateBlock* pEntity, const MFStateBlock* pMaterialOverride, const MFStateBlock* pView)
	{
		MFStateBlock *pSB = MFStateBlock_CreateTemporary(64);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexDeclaration, pVB.pVertexDeclatation);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexBuffer0, pVB);

		MFRenderer_AddVertices(pSB, 0, cast(int)pVB.numVerts, primType, pMaterial, pEntity, pMaterialOverride, pView);
	}

	@property inout(MFVertexBuffer)* handle() inout pure	{ return pVB; }
	@property ref inout(Resource) resource() inout pure		{ return *cast(inout(Resource)*)&this; }

	@property size_t length() const pure	{ return pVB ? pVB.numVerts : 0; }
	@property size_t size() const pure
	{
		static if(is(VertexDataType == void))
		{
			assert(false, "TODO: we need to be able to get the buffer size for untyped vertex buffers");
//			return pVB.numVerts * 0;
		}
		else
			return pVB.numVerts * VertexDataType.sizeof;
	}
}

struct Mesh
{
	MFMesh mesh;
	alias mesh this;

nothrow:
@nogc:
	this(MFVertexBuffer *pVB, MFPrimType primType)
	{
		Create(pVB, primType);
	}

	~this()
	{
		Release();
	}

	void Create(MFVertexBuffer *pVB, MFPrimType primType)
	{
		Release();

		MFStateBlock *pSB = pVB.bufferType == MFVertexBufferType.Scratch ? MFStateBlock_CreateTemporary(64) : MFStateBlock_Create(64);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexDeclaration, pVB.pVertexDeclatation);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexBuffer0, pVB);

		mesh.pMeshState = pSB;
		mesh.primType = primType;
		mesh.vertexOffset = 0;
		mesh.numVertices = pVB.numVerts;
		mesh.indexOffset = 0;
		mesh.numIndices = 0;
	}

	void Release()
	{
		// TODO: if it's a temp stateblock, it shouldn't be destroyed!
		if(mesh.pMeshState)
		{
			MFStateBlock_Destroy(mesh.pMeshState);
			mesh.pMeshState = null;
		}
	}

	void Draw(MFMaterial* pMaterial, const MFStateBlock* pEntity, const MFStateBlock* pMaterialOverride, const MFStateBlock* pView)
	{
		MFRenderer_AddMesh(&mesh, pMaterial, pEntity, pMaterialOverride, pView);
	}
}

// make a bunch of types for different common vertex formats

@(MFVertexDataFormat.Float1)
struct MFVertexFloat
{
	this(float f)		{ this.f = f; }
	this(MFVector v)	{ f = v.x; }

	float f;
}
@(MFVertexDataFormat.Float2)
struct MFVertexFloat2
{
	this(float x, float y)	{ this.x = x; this.y = y; }
	this(ref float[2] arr)	{ x = arr[0]; y = arr[1]; }
	this(MFVector v)		{ x = v.x; y = v.y; }

	float x,y;
}
@(MFVertexDataFormat.Float3)
struct MFVertexFloat3
{
	this(float x, float y, float z)	{ this.x = x; this.y = y; this.z = z; }
	this(ref float[3] arr)	{ x = arr[0]; y = arr[1]; z = arr[2]; }
	this(MFVector v)		{ x = v.x; y = v.y; z = v.z; }

	float x,y,z;
}
@(MFVertexDataFormat.Float4)
struct MFVertexFloat4
{
	this(float x, float y, float z, float w)	{ this.x = x; this.y = y; this.z = z; this.w = w; }
	this(ref float[4] arr)	{ x = arr[0]; y = arr[1]; z = arr[2]; w = arr[3]; }
	this(MFVector v)		{ x = v.x; y = v.y; z = v.z; w = v.w; }

	float x,y,z,w;
}

@(MFVertexDataFormat.Float16_2)
struct MFVertexHalf2 // 16 bit float
{
	ushort x,y;
}
@(MFVertexDataFormat.Float16_4)
struct MFVertexHalf4 // 16 bit float
{
	ushort x,y,z,w;
}

@(MFVertexDataFormat.UByte4_RGBA)
struct MFVertexUByte4
{
	this(uint packed)		{ *cast(uint*)&this = packed; }
	this(MFVector v)		{ x = cast(ubyte)v.x; y = cast(ubyte)v.y; z = cast(ubyte)v.z; w = cast(ubyte)v.w; }

	ubyte x,y,z,w;
}
@(MFVertexDataFormat.UByte4N_RGBA)
struct MFVertexUByte4N
{
	this(uint packed)		{ *cast(uint*)&this = packed; }
	this(MFVector v)		{ x = cast(ubyte)(v.x * 255.0f); y = cast(ubyte)(v.y * 255.0f); z = cast(ubyte)(v.z * 255.0f); w = cast(ubyte)(v.w * 255.0f); }

	ubyte x,y,z,w;
}
@(MFVertexDataFormat.UByte4N_BGRA)
struct MFVertexUByte4N_BGRA
{
	ubyte z,y,x,w;
}

@(MFVertexDataFormat.UDec3)
struct MFVertexUDec3
{
	uint udec3;
}
@(MFVertexDataFormat.Dec3N)
struct MFVertexDec3N
{
	uint dec3n;
}

@(MFVertexDataFormat.SShort2)
struct MFVertexShort2
{
	short x,y;
}
@(MFVertexDataFormat.SShort2N)
struct MFVertexShort2N
{
	short x,y;
}
@(MFVertexDataFormat.UShort2)
struct MFVertexUShort2
{
	ushort x,y;
}
@(MFVertexDataFormat.UShort2N)
struct MFVertexUShort2N
{
	ushort x,y;
}

@(MFVertexDataFormat.SShort4)
struct MFVertexShort4
{
	short x,y,z,w;
}
@(MFVertexDataFormat.SShort4N)
struct MFVertexShort4N
{
	short x,y,z,w;
}
@(MFVertexDataFormat.UShort4)
struct MFVertexUShort4
{
	ushort x,y,z,w;
}
@(MFVertexDataFormat.UShort4N)
struct MFVertexUShort4N
{
	ushort x,y,z,w;
}


// private stuff...
private:

enum int[MFVertexDataFormat.Max] numElements =
[
	0, // Auto
	4, // Float4
	3, // Float3
	2, // Float2
	1, // Float1
	4, // UByte4_RGBA
	4, // UByte4N_RGBA
	4, // UByte4N_BGRA
	4, // SShort4
	2, // SShort2
	4, // SShort4N
	2, // SShort2N
	4, // UShort4
	2, // UShort2
	4, // UShort4N
	2, // UShort2N
	4, // Float16_4
	2, // Float16_2
	3, // UDec3
	3 // Dec3N
];

template GetAttribute(alias T, Attr)
{
	template Impl(T...)
	{
		static if(T.length == 0)
			enum Impl = Attr.Unknown;
		else
			enum Impl = is(typeof(T[0]) == Attr) ? T[0] : Impl!(T[1..$]);
	}

	enum GetAttribute = Impl!(__traits(getAttributes, T));
}

template HasVertexAttributes(alias M)
{
	enum HasVertexAttributes = GetAttribute!(M, MFVertexElementType) != MFVertexElementType.Unknown && GetAttribute!(typeof(M), MFVertexDataFormat) != MFVertexDataFormat.Unknown;
}

template IsValidVertexStructure(alias T)
{
	template AllMembersHaveVertexAttributes(alias T, Members...)
	{
		static if(Members.length == 0)
			enum AllMembersHaveVertexAttributes = true;
		else
			enum AllMembersHaveVertexAttributes = HasVertexAttributes!(__traits(getMember, T, Members[0])) && AllMembersHaveVertexAttributes!(T, Members[1..$]);
	}

	enum IsValidVertexStructure = is(T == struct) && AllMembersHaveVertexAttributes!(T, __traits(allMembers, T));
}
