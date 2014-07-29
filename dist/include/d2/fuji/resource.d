module fuji.resource;

public import fuji.c.MFResource;

import fuji.string;

nothrow:
@nogc:

struct Resource
{
	alias pResource this;

	MFResource *pResource;

nothrow:
@nogc:
	this(this) pure	{ addRef(); }
	~this()			{ release(); }

	void opAssign(MFResource *pResource)
	{
		release();
		this.pResource = pResource;
		addRef();
	}

	void opAssign(Resource r)
	{
		release();
		pResource = r.pResource;
		addRef();
	}

	@property inout(MFResource)* handle() inout pure	{ return pResource; }

	@property MFResourceType type() const pure			{ return cast(MFResourceType)pResource.type; }
	@property uint hash() const pure					{ return pResource.hash; }
	@property int refCount() const pure					{ return pResource.refCount; }
	@property const(char)[] name() const pure			{ return pResource.name; }

	int addRef() pure
	{
		if(pResource)
			return pResource.addRef();
		return 0;
	}
	int release()
	{
		int rc = 0;
		if(pResource)
		{
			rc = pResource.release();
			pResource = null;
		}
		return rc;
	}

	bool isType(MFResourceType type) const pure			{ return this.type == type; }
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
nothrow:
@nogc:
	@property bool empty() const pure					{ return pResource != null; }
	@property size_t length() const pure				{ return count; }

	@property ResourceIterator save() pure				{ return this; }

	@property TypeForResource!type front()
	{
		return cast(TypeForResource!type)MFResource_Get(pResource);
	}

	void popFront()
	{
		pResource = pNext;
		--count;
		if(pResource)
			pNext = MFResource_EnumerateNext(pResource, type);
	}

private:
	MFResourceIterator* pResource, pNext;
	int count;
}

template TypeForResource(MFResourceType type)
{
	static if(type == MFResourceType.Texture)
		alias TypeForResource = Texture;
	else static if(type == MFResourceType.Material)
		alias TypeForResource = Material;
	else static if(type == MFResourceType.VertexDecl)
		alias TypeForResource = VertexDeclaration;
	else static if(type == MFResourceType.VertexBuffer)
		alias TypeForResource = VertexBuffer;
	else static if(type == MFResourceType.IndexBuffer)
		alias TypeForResource = IndexBuffer;
	else static if(type == MFResourceType.BlendState)
		alias TypeForResource = BlendState;
	else static if(type == MFResourceType.SamplerState)
		alias TypeForResource = SamplerState;
	else static if(type == MFResourceType.DepthStencilState)
		alias TypeForResource = DepthStencilState;
	else static if(type == MFResourceType.RasteriserState)
		alias TypeForResource = RasteriserState;
	else static if(type == MFResourceType.Shader)
		alias TypeForResource = Shader;
	else static if(type == MFResourceType.Effect)
		alias TypeForResource = Effect;
	else static if(type == MFResourceType.Model)
		alias TypeForResource = Model;
	else static if(type == MFResourceType.Animation)
		alias TypeForResource = Animation;
	else static if(type == MFResourceType.Sound)
		alias TypeForResource = Sound;
	else static if(type == MFResourceType.Font)
		alias TypeForResource = Font;
	else static if(type == MFResourceType.All || (type >= MFResourceType.User && type < MFResourceType.Max))
		alias TypeForResource = Resource;
	else
		static assert("Invalid resource type!");
}

unittest
{
	foreach(t; enumerateResources!(MFResourceType.Texture))
	{
		// test...?
	}
}

package struct ResourceRange(ResourceType, ItemType, alias findFunc, bool bItemsHaveNames = true, bool bDispatchAssign = false)
{
nothrow:
	static if(bItemsHaveNames)
	{
		ItemType opIndex(const(char)[] name) @nogc						{ return find(name); }

		@property ItemType opDispatch(const(char)[] name)()				{ return find(name); }
		static if(bDispatchAssign)
		{
			@property ItemType opDispatch(const(char)[] name, T)(T t)	{ ItemType p = find(name); p = t; return p; }
		}

		ItemType find(const(char)[] name) @nogc
		{
			auto s = Stringz!(64)(name);
			int index = findFunc(pRes, s);
			if(index < offset || index >= count)
				return ItemType(pRes, -1);
			return ItemType(pRes, index);
		}
	}

@nogc:
	@property bool empty() const pure							{ return offset == count; }
	@property size_t length() const pure						{ return count - offset; }

	@property typeof(this) save() pure							{ return this; }

	@property ItemType front()									{ return ItemType(pRes, offset); }
	@property ItemType back()									{ return ItemType(pRes, count-1); }

	ItemType opIndex(size_t index)								{ return ItemType(pRes, offset + index); }
	typeof(this) opSlice(size_t x, size_t y) pure				{ return typeof(this)(pRes, offset + x, offset + y); }

	void popFront() pure										{ ++offset; }
	void popBack() pure											{ --count; }

private:
	ResourceType* pRes;
	size_t offset, count;
}
