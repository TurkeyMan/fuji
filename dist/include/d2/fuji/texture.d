module fuji.texture;

public import fuji.c.MFTexture;

public import fuji.image;
import fuji.resource;
import fuji.vector;

nothrow:
@nogc:

struct MFTextureDimensions
{
	int width, height;
}

struct Texture
{
	alias resource this;

	MFTexture *pTexture;

nothrow:
@nogc:
	this(const(char)[] name)
	{
		create(name);
	}

	void create(const(char)[] name, const(MFTextureDesc)* pDesc)
	{
		release();
		auto s = Stringz!(64)(name);
		pTexture = MFTexture_Create(s, pDesc);
	}

	void create(const(char)[] name, uint flags = MFTextureCreateFlags.GenerateMips)
	{
		release();
		auto s = Stringz!(64)(name);
		pTexture = MFTexture_CreateFromFile(s, flags);
	}

	void create2D(const(char)[] name, int width, int height, MFImageFormat format, uint flags = 0)
	{
		auto s = Stringz!(64)(name);
		pTexture = MFTexture_Create2D(s, width, height, format, flags);
	}

	void find(const(char)[] name)
	{
		release();
		auto s = Stringz!(64)(name);
		pTexture = MFTexture_Find(s);
	}

	this(this) pure
	{
		addRef();
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Texture)
		{
			pTexture = cast(MFTexture*)resource.handle;
			addRef();
		}
	}

	~this()
	{
		release();
	}

	void opAssign(MFTexture *pTexture)
	{
		release();
		this.pTexture = pTexture;
		addRef();
	}

	void opAssign(Texture texture)
	{
		release();
		pTexture = texture.pTexture;
		addRef();
	}

	int release()
	{
		int rc = 0;
		if(pTexture)
		{
			rc = MFTexture_Release(pTexture);
			pTexture = null;
		}
		return rc;
	}

	bool update(const(void)* pData, int mipLevel = 0, int element = 0)
	{
		return MFTexture_Update(pTexture, element, mipLevel, pData);
	}

	bool map(out MFLockedTexture surface, int mipLevel = 0, int element = 0)
	{
		return MFTexture_Map(pTexture, element, mipLevel, &surface);
	}

	void unmap(int mipLevel = 0, int element = 0)
	{
		MFTexture_Unmap(pTexture, element, mipLevel);
	}

	@property inout(MFTexture)* handle() inout pure		{ return pTexture; }
	@property ref inout(Resource) resource() inout pure	{ return *cast(inout(Resource)*)&this; }

	@property MFTextureDimensions size() const pure
	{
		MFTextureDimensions d;
		MFTexture_GetTextureDimensions(pTexture, &d.width, &d.height);
		return d;
	}

	@property int width() const pure	{ return size.width; }
	@property int height() const pure	{ return size.height; }
}
