module fuji.font;

public import fuji.c.MFFont;

import fuji.resource;
import fuji.string;

struct Font
{
	MFFont *pFont;
	alias pFont this;

nothrow:
@nogc:
	static @property Font debugFont()
	{
		Font f;
		f.pFont = MFFont_GetDebugFont();
		MFResource_AddRef(cast(fuji.resource.MFResource*)f.pFont);
		return f;
	}

	this(this) pure
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pFont);
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Font)
		{
			resource.AddRef();
			pFont = cast(MFFont*)resource.handle;
		}
	}

	this(const(char)[] name)
	{
		create(name);
	}

	~this()
	{
		release();
	}

	void create(const(char)[] name)
	{
		release();
		auto n = Stringz!(64)(name);
		pFont = MFFont_Create(n);
	}

	int release()
	{
		int rc = 0;
		if(pFont)
		{
			rc = MFFont_Release(pFont);
			pFont = null;
		}
		return rc;
	}

	bool opCast(T)() if(is(T == bool))								{ return pFont != null; }

	@property inout(MFFont)* handle() inout pure					{ return pFont; }
	@property ref inout(Resource) resource() inout pure				{ return *cast(inout(Resource)*)&this; }

	@property float height() const									{ return MFFont_GetFontHeight(pFont); }

	float characterWidth(dchar character) const						{ return MFFont_GetCharacterWidth(pFont, cast(int)character); }
}
