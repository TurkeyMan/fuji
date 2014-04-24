module fuji.texture;

public import fuji.c.MFTexture;

public import fuji.image;
import fuji.resource;
import fuji.vector;


struct MFTextureDimensions
{
	int width, height;
}

struct Texture
{
	MFTexture *pTexture;
	alias pTexture this;

	this(this) pure nothrow
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pTexture);
	}

	this(ref Resource resource) pure nothrow
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Texture)
		{
			resource.AddRef();
			pTexture = cast(MFTexture*)resource.handle;
		}
	}

	this(const(char)[] name) nothrow
	{
		create(name);
	}

	~this() nothrow
	{
		release();
	}

	void create(const(char)[] name, bool generateMipChain = true) nothrow
	{
		release();
		pTexture = MFTexture_Create(name.toStringz(), generateMipChain);
	}

	void createExisting(const(char)[] name) nothrow
	{
		release();
		pTexture = MFTexture_Find(name.toStringz());
	}

	int release() nothrow
	{
		int rc = 0;
		if(pTexture)
		{
			rc = MFTexture_Release(pTexture);
			pTexture = null;
		}
		return rc;
	}

	@property inout(MFTexture)* handle() inout pure nothrow		{ return pTexture; }
	@property ref inout(Resource) resource() inout pure nothrow	{ return *cast(inout(Resource)*)&this; }

	@property MFTextureDimensions size() const pure nothrow
	{
		MFTextureDimensions d;
		MFTexture_GetTextureDimensions(pTexture, &d.width, &d.height);
		return d;
	}

	@property int width() const pure nothrow	{ return size.width; }
	@property int height() const pure nothrow	{ return size.height; }
}
