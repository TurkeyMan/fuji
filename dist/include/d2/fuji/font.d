module fuji.font;

public import fuji.c.MFFont;

import fuji.resource;
import fuji.string;
import fuji.vector;
import fuji.matrix;

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

	MFVector getCharPos(const(char)[] text, int charIndex, float height) const
	{
		auto s = Stringz!256(text);
		return MFFont_GetCharPos(pFont, s, charIndex, height);
	}

	float getStringWidth(const(char)[] text, float height, float lineWidth = 0.0f, int maxLen = -1, float* pTotalHeight = null) const
	{
		auto s = Stringz!256(text);
		return MFFont_GetStringWidth(pFont, s, height, lineWidth, maxLen, pTotalHeight);
	}

	int blit(const(char)[] text, int x, int y, ref const(MFVector) colour = MFVector.white)
	{
		auto s = Stringz!256(text);
		return MFFont_BlitText(pFont, x, y, colour, s, -1);
	}

	float draw(const(char)[] text, ref const(MFVector) pos, float height, ref const(MFVector) colour = MFVector.white, ref const(MFMatrix) ltw = MFMatrix.identity)
	{
		auto s = Stringz!256(text);
		return MFFont_DrawText(pFont, pos, height, colour, s, -1, ltw);
	}

	float draw(const(char)[] text, float x, float y, float height, ref const(MFVector) colour = MFVector.white, ref const(MFMatrix) ltw = MFMatrix.identity)
	{
		MFVector pos = MFVector(x, y);
		return draw(text, pos, height, colour, ltw);
	}

	float drawJustified(const(char)[] text, ref const(MFVector) pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, ref const(MFVector) colour = MFVector.white, ref const(MFMatrix) ltw = MFMatrix.identity)
	{
		auto s = Stringz!256(text);
		return MFFont_DrawTextJustified(pFont, s, pos, boxWidth, boxHeight, justification, textHeight, colour, -1, ltw);
	}

	float drawAnchored(const(char)[] text, ref const(MFVector) pos, MFFontJustify justification, float lineWidth, float textHeight, ref const(MFVector) colour = MFVector.white, ref const(MFMatrix) ltw = MFMatrix.identity)
	{
		auto s = Stringz!256(text);
		return MFFont_DrawTextAnchored(pFont, s, pos, justification, lineWidth, textHeight, colour, -1, ltw);
	}
}
