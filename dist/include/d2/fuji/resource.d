module fuji.resource;

public import fuji.fuji;

enum MFResourceType
{
	All = -2,
	Unknown = -1,

	Texture = 0,
	Material,
	VertexDecl,
	VertexBuffer,
	IndexBuffer,
	BlendState,
	SamplerState,
	DepthStencilState,
	RasteriserState,
	Shader,
	Effect,
	Model,
	Animation,
	Sound,
	Font,

	User,

	Max = 32,

	UserCount = Max - User
};

struct MFResource
{
	uint hash;
	short type;
	short refCount;
	const(char)* pName;
};

struct MFResourceIterator;

extern (C) void MFResource_AddResource(MFResource* pResource, int type, uint hash, const(char)* pName = null);
extern (C) void MFResource_RemoveResource(MFResource* pResource);

extern (C) MFResource* MFResource_Find(uint hash);

extern (C) MFResourceType MFResource_GetType(MFResource* pResource);
extern (C) uint MFResource_GetHash(MFResource* pResource);
extern (C) int MFResource_GetRefCount(MFResource* pResource);
extern (C) const(char)* MFResource_GetName(MFResource* pResource);

extern (C) int MFResource_GetNumResources(MFResourceType type = MFResourceType.All);

extern (C) MFResourceIterator* MFResource_EnumerateFirst(MFResourceType type = MFResourceType.All);
extern (C) MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator* pIterator, MFResourceType type = MFResourceType.All);
extern (C) MFResource* MFResource_Get(MFResourceIterator* pIterator);

bool MFResource_IsType(MFResource* pResource, MFResourceType type)
{
	return MFResource_GetType(pResource) == type;
}


import std.c.string;

final class Resource
{
	@property MFResourceType type() const pure nothrow			{ return cast(MFResourceType)(cast(MFResource*)this).type; }
	@property uint hash() const pure nothrow					{ return (cast(MFResource*)this).hash; }
	@property int refCount() const pure nothrow					{ return (cast(MFResource*)this).refCount; }
	@property string name() const pure nothrow
	{
		auto pName = (cast(MFResource*)this).pName;
		return pName ? cast(string)pName[0..strlen(pName)] : null;
	}

	@property MFResource* handle() pure nothrow					{ return cast(MFResource*)this; }
	@property const(MFResource)* handle() const pure nothrow	{ return cast(const(MFResource)*)this; }

	bool isType(MFResourceType type) const pure nothrow			{ return this.type == type; }
}


size_t getNumResources(MFResourceType type = MFResourceType.All)
{
	return MFResource_GetNumResources(type);
}

ResourceIterator!Type enumerateResources(MFResourceType Type)()
{
	int numResources = MFResource_GetNumResources(Type);
	if(numResources == 0)
		return ResourceIterator!Type(NULL, NULL, 0);

	MFResourceIterator* pI = MFResource_EnumerateFirst(Type);
	assert(pI != null);

	return ResourceIterator!Type(pI, MFResource_EnumerateNext(pI, Type), numResources);
}

struct ResourceIterator(MFResourceType type)
{
	@property bool empty() const pure nothrow					{ return count == 0; }
	@property size_t length() const pure nothrow				{ return count; }

	@property ResourceIterator save() pure nothrow				{ return this; }

	@property auto front() pure nothrow
	{
		static if(type == Texture)
			return cast(Texture)MFResource_Get(pResource);
		else static if(type == Material)
			return cast(Material)MFResource_Get(pResource);
		else static if(type == VertexDecl)
			return cast(VertexDeclaration)MFResource_Get(pResource);
		else static if(type == VertexBuffer)
			return cast(VertexBuffer)MFResource_Get(pResource);
		else static if(type == IndexBuffer)
			return cast(IndexBuffer)MFResource_Get(pResource);
		else static if(type == BlendState)
			return cast(BlendState)MFResource_Get(pResource);
		else static if(type == SamplerState)
			return cast(SamplerState)MFResource_Get(pResource);
		else static if(type == DepthStencilState)
			return cast(DepthStencilState)MFResource_Get(pResource);
		else static if(type == RasteriserState)
			return cast(RasteriserState)MFResource_Get(pResource);
		else static if(type == Shader)
			return cast(Shader)MFResource_Get(pResource);
		else static if(type == Effect)
			return cast(Effect)MFResource_Get(pResource);
		else static if(type == Model)
			return cast(Model)MFResource_Get(pResource);
		else static if(type == Animation)
			return cast(Animation)MFResource_Get(pResource);
		else static if(type == Sound)
			return cast(Sound)MFResource_Get(pResource);
		else static if(type == Font)
			return cast(Font)MFResource_Get(pResource);
		else
			return cast(Resource)MFResource_Get(pResource);
	}

	void popFront() pure nothrow
	{
		pResource = pNext;
		pNext = MFResource_EnumerateNext(pResource, type);
		--count;
	}

private:
	MFResourceIterator* pResource, pNext;
	int count;
}
