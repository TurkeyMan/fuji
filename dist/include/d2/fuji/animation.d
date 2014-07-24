module fuji.animation;

public import fuji.c.MFAnimation;

import fuji.resource;
import fuji.animation;
import fuji.model;
import fuji.matrix;
import fuji.string;

nothrow:
@nogc:

struct MFFrameRange
{
	float start, end;
}

struct Animation
{
	MFAnimation *pAnimation;
	alias pAnimation this;

nothrow:
@nogc:
	this(this) pure
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pAnimation);
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Animation)
		{
			resource.AddRef();
			pAnimation = cast(MFAnimation*)resource.handle;
		}
	}

	this(const(char)[] name, Model model)
	{
		create(name, model);
	}

	~this()
	{
		release();
	}

	void create(const(char)[] name, Model model)
	{
		release();
		auto n = Stringz!(64)(name);
		pAnimation = MFAnimation_Create(n, model.handle);
	}

	int release()
	{
		int rc = 0;
		if(pAnimation)
		{
			rc = MFAnimation_Release(pAnimation);
			pAnimation = null;
		}
		return rc;
	}

	bool opCast(T)() if(is(T == bool))								{ return pAnimation != null; }

	@property inout(MFAnimation)* handle() inout pure				{ return pAnimation; }
	@property ref inout(Resource) resource() inout pure				{ return *cast(inout(Resource)*)&this; }

	@property void frame(float time)								{ MFAnimation_SetFrame(pAnimation, time); }

	@property float startTime() const								{ return frameRange.start; }
	@property float endTime() const									{ return frameRange.end; }

	@property MFFrameRange frameRange() const
	{
		MFFrameRange r;
		MFAnimation_GetFrameRange(pAnimation, &r.start, &r.end);
		return r;
	}

	MFMatrix* calculateMatrices(MFMatrix* pLocalToWorld)			{ return MFAnimation_CalculateMatrices(pAnimation, pLocalToWorld); }
}
