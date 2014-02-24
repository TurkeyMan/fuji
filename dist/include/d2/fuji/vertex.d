module fuji.vertex;

public import fuji.fuji;
import fuji.resource;
import fuji.vector;
import fuji.renderstate;

struct MFEffectTechnique; // TODO: effect.d will appear and remove this

/**
 * @struct MFVertexDeclaration
 * Represents the layout of a vertex buffer.
 */
struct MFVertexDeclaration;

/**
 * @struct MFVertexBuffer
 * Represents a Fuji vertex buffer.
 */
struct MFVertexBuffer
{
private:
	// Note: This is a mirror of the internal Fuji structure, don't touch!
	MFResource resource;
	alias resource this;

	MFVertexDeclaration* pVertexDeclatation;
	MFVertexBufferType bufferType;
	int numVerts;

	bool bLocked;
	void* pLocked;

	const(char)* pName;
}

/**
 * @struct MFIndexBuffer
 * Represents a Fuji index buffer.
 */
struct MFIndexBuffer;

enum MFVertexDataFormat
{
	Unknown = -1,

	Auto = 0,

	Float4,
	Float3,
	Float2,
	Float1,
	UByte4_RGBA,
	UByte4N_RGBA,
	UByte4N_BGRA,
	SShort4,
	SShort2,
	SShort4N,
	SShort2N,
	UShort4,
	UShort2,
	UShort4N,
	UShort2N,
	Float16_4,
	Float16_2,
	UDec3,
	Dec3N,

	Max
};

enum MFVertexBufferType
{
	Static,
	Dynamic,
	Scratch
};

enum MFVertexElementType
{
	Unknown = -1,

	Position,
	Normal,
	Colour,
	TexCoord,
	Binormal,
	Tangent,
	Indices,
	Weights,

	Max
}

enum MFPrimType
{
	Points,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFan,
	QuadList
}

struct MFVertexElement
{
	int stream;
	MFVertexElementType elementType;
	int elementIndex;
	int componentCount;
	MFVertexDataFormat format;
}

struct MFMesh
{
	MFStateBlock *pMeshState;
	MFPrimType primType;
	int vertexOffset;
	int numVertices;
	int indexOffset;
	int numIndices;
};

extern (C) MFVertexDeclaration* MFVertex_CreateVertexDeclaration(MFVertexElement* pElementArray, int elementCount);
extern (C) int MFVertex_ReleaseVertexDeclaration(MFVertexDeclaration* pDeclaration);
extern (C) MFVertexDeclaration* MFVertex_GetStreamDeclaration(MFVertexDeclaration* pDeclaration, int stream) pure;

extern (C) MFVertexBuffer* MFVertex_CreateVertexBuffer(MFVertexDeclaration* pVertexFormat, int numVerts, MFVertexBufferType type, void* pVertexBufferMemory = null, const(char)* pName = null);
extern (C) int MFVertex_ReleaseVertexBuffer(MFVertexBuffer* pVertexBuffer);
extern (C) void MFVertex_LockVertexBuffer(MFVertexBuffer* pVertexBuffer, void **ppVertices);
extern (C) void MFVertex_CopyVertexData(MFVertexBuffer* pVertexBuffer, MFVertexElementType targetElement, int targetElementIndex, const(void)* pSourceData, MFVertexDataFormat sourceDataFormat, int sourceDataStride, int numVertices);
extern (C) void MFVertex_SetVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, ref const(MFVector) data);
extern (C) void MFVertex_ReadVertexData4v(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, MFVector* pData);
extern (C) void MFVertex_SetVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint data);
extern (C) void MFVertex_ReadVertexData4ub(MFVertexBuffer* pVertexBuffer, MFVertexElementType element, int elementIndex, uint* pData);
extern (C) void MFVertex_UnlockVertexBuffer(MFVertexBuffer* pVertexBuffer);

extern (C) MFIndexBuffer* MFVertex_CreateIndexBuffer(int numIndices, ushort* pIndexBufferMemory = null, const(char)* pName = null);
extern (C) int MFVertex_ReleaseIndexBuffer(MFIndexBuffer* pIndexBuffer);
extern (C) void MFVertex_LockIndexBuffer(MFIndexBuffer* pIndexBuffer, ushort** ppIndices);
extern (C) void MFVertex_UnlockIndexBuffer(MFIndexBuffer* pIndexBuffer);

extern (C) void MFVertex_SetVertexDeclaration(const(MFVertexDeclaration)* pVertexDeclaration);
extern (C) void MFVertex_SetVertexStreamSource(int stream, const(MFVertexBuffer)* pVertexBuffer);
extern (C) void MFVertex_SetIndexBuffer(const(MFIndexBuffer)* pIndexBuffer);
extern (C) void MFVertex_RenderVertices(MFEffectTechnique* pTechnique, MFPrimType primType, int firstVertex, int numVertices);
extern (C) void MFVertex_RenderIndexedVertices(MFEffectTechnique* pTechnique, MFPrimType primType, int vertexOffset, int indexOffset, int numVertices, int numIndices);


// helpers for D...

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
	MFVertexDeclaration* pDecl = MFVertex_CreateVertexDeclarationForStruct!VertexStruct();
	MFVertexBuffer* pVB = MFVertex_CreateVertexBuffer(pDecl, cast(int)numVertices, type, null, name ? name.toStringz : null);
	MFVertex_ReleaseVertexDeclaration(pDecl);
	return pVB;
}

MFVertexBuffer* MFVertex_CreateVertexBufferFromData(VertexStruct)(MFVertexBufferType type, VertexStruct[] vertices, string name = null)
{
	MFVertexDeclaration* pDecl = MFVertex_CreateVertexDeclarationForStruct!VertexStruct();
	MFVertexBuffer* pVB = MFVertex_CreateVertexBuffer(pDecl, cast(int)vertices.length, type, vertices.ptr, name ? name.toStringz : null);
	MFVertex_ReleaseVertexDeclaration(pDecl);
	return pVB;
}


// wrap the vertex buffer nicely

import fuji.render;
import fuji.material;

struct VertexBuffer(VertexDataType = void) if(is(VertexDataType == void) || IsValidVertexStructure!VertexDataType)
{
	alias pVB this;

	this(this)
	{
		pVB.resource.AddRef();
	}

	this(Resource resource)
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

	void Create(MFVertexDeclaration* pVertexFormat, size_t numVertices, MFVertexBufferType type, string name = null)
	{
		Release();
		pVB = MFVertex_CreateVertexBuffer(pVertexFormat, cast(int)numVertices, type, null, name ? name.toStringz : null);
	}

	void Create(VertexStruct = VertexDataType)(size_t numVertices, MFVertexBufferType type, string name = null)
	{
		Release();

		static if(is(VertexStruct == void))
			assert(false, "Unable to create vertex buffer for 'VertexStruct == " ~ VertexStruct.stringof ~ "'");
		else
			pVB = MFVertex_CreateVertexBufferForStruct!VertexStruct(type, numVertices, name);
	}

	int Release()
	{
		int rc = 0;
		if(pVB)
		{
			rc = MFVertex_ReleaseVertexBuffer(pVB);
			pVB = null;
		}
		return rc;
	}

	VertexDataType[] Lock()
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

	void Unlock()
	{
		MFVertex_UnlockVertexBuffer(pVB);
	}

	Mesh GetMesh(MFPrimType primType)
	{
		return Mesh(pVB, primType);
	}

	void Draw(MFPrimType primType, MFMaterial* pMaterial, const MFStateBlock* pEntity, const MFStateBlock* pMaterialOverride, const MFStateBlock* pView)
	{
		MFStateBlock *pSB = MFStateBlock_CreateTemporary(64);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexDeclaration, pVB.pVertexDeclatation);
		MFStateBlock_SetRenderState(pSB, MFStateConstant_RenderState.VertexBuffer0, pVB);

		MFRenderer_AddVertices(pSB, 0, cast(int)pVB.numVerts, primType, pMaterial, pEntity, pMaterialOverride, pView);
	}

	@property size_t length() const { return pVB ? pVB.numVerts : 0; }
	@property size_t size() const
	{
		static if(is(VertexDataType == void))
		{
			assert(false, "TODO: we need to be able to get the buffer size for untyped vertex buffers");
//			return pVB.numVerts * 0;
		}
		else
			return pVB.numVerts * VertexDataType.sizeof;
	}

	MFVertexBuffer* pVB;
}

struct Mesh
{
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

	MFMesh mesh;
	alias mesh this;
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

private enum int[MFVertexDataFormat.Max] numElements =
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

private template GetAttribute(alias T, Attr)
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

private template HasVertexAttributes(alias M)
{
	enum HasVertexAttributes = GetAttribute!(M, MFVertexElementType) != MFVertexElementType.Unknown && GetAttribute!(typeof(M), MFVertexDataFormat) != MFVertexDataFormat.Unknown;
}

private template IsValidVertexStructure(alias T)
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
