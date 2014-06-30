module fuji.animation;

public import fuji.c.MFAnimation;

import fuji.resource;
import fuji.animation;
import fuji.model;
import fuji.matrix;

import std.string;

struct MFFrameRange
{
	float start, end;
}

struct Animation
{
	MFAnimation *pAnimation;
	alias pAnimation this;

	this(this) pure nothrow
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pAnimation);
	}

	this(ref Resource resource) pure nothrow
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Animation)
		{
			resource.AddRef();
			pAnimation = cast(MFAnimation*)resource.handle;
		}
	}

	this(const(char)[] name, Model model) nothrow
	{
		create(name, model);
	}

	~this() nothrow
	{
		release();
	}

	void create(const(char)[] name, Model model) nothrow
	{
		release();
		pAnimation = MFAnimation_Create(name.toStringz(), model.handle);
	}

	int release() nothrow
	{
		int rc = 0;
		if(pAnimation)
		{
			rc = MFAnimation_Release(pAnimation);
			pAnimation = null;
		}
		return rc;
	}

	@property inout(MFAnimation)* handle() inout pure nothrow				{ return pAnimation; }
	@property ref inout(Resource) resource() inout pure nothrow				{ return *cast(inout(Resource)*)&this; }

	@property void frame(float time) nothrow								{ MFAnimation_SetFrame(pAnimation, time); }

	@property float startTime() const nothrow								{ return frameRange.start; }
	@property float endTime() const nothrow									{ return frameRange.end; }

	@property MFFrameRange frameRange() const nothrow
	{
		MFFrameRange r;
		MFAnimation_GetFrameRange(pAnimation, &r.start, &r.end);
		return r;
	}

	MFMatrix* calculateMatrices(MFMatrix* pLocalToWorld) nothrow			{ return MFAnimation_CalculateMatrices(pAnimation, pLocalToWorld); }
}
