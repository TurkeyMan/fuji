module fuji.font;

public import fuji.c.MFFont;

import fuji.resource;
import fuji.string;

struct Font
{
	alias resource this;

	MFFont *pFont;

nothrow:
@nogc:
	static @property Font debugFont()
	{
		Font f;
		f.pFont = MFFont_GetDebugFont();
		f.addRef();
		return f;
	}

	this(this) pure
	{
		addRef();
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Font)
		{
			pFont = cast(MFFont*)resource.handle;
			addRef();
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

	void opAssign(MFFont *pFont)
	{
		release();
		this.pFont = pFont;
		addRef();
	}

	void opAssign(Font font)
	{
		release();
		pFont = font.pFont;
		addRef();
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

	@property inout(MFFont)* handle() inout pure					{ return pFont; }
	@property ref inout(Resource) resource() inout pure				{ return *cast(inout(Resource)*)&this; }

	@property float height() const									{ return MFFont_GetFontHeight(pFont); }

	float characterWidth(dchar character) const						{ return MFFont_GetCharacterWidth(pFont, cast(int)character); }
}
