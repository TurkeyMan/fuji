module fuji.model;

public import fuji.c.MFModel;

import fuji.resource;
import fuji.boundingvolume;
import fuji.matrix;
import fuji.animation;
import fuji.string;

nothrow:
@nogc:

struct Model
{
	MFModel *pModel;
	alias pModel this;

nothrow:
@nogc:
	this(const(char)[] name)
	{
		create(name);
	}

	void create(const(char)[] name, const(char)[] animation = null)
	{
		release();
		auto n = Stringz!(64)(name);
		auto a = Stringz!(64)(animation);
		pModel = MFModel_CreateWithAnimation(n, animation ? a : null);
	}

	this(this) pure
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pModel);
	}

	this(ref Resource resource) pure
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Model)
		{
			resource.AddRef();
			pModel = cast(MFModel*)resource.handle;
		}
	}

	~this()
	{
		release();
	}

	int release()
	{
		int rc = 0;
		if(pModel)
		{
			rc = MFModel_Release(pModel);
			pModel = null;
		}
		return rc;
	}

	bool opCast(T)() if(is(T == bool))								{ return pModel != null; }

	@property inout(MFModel)* handle() inout pure					{ return pModel; }
	@property ref inout(Resource) resource() inout pure				{ return *cast(inout(Resource)*)&this; }

	@property const(char)[] name() const							{ return MFModel_GetName(pModel).toDStr; }

	@property void worldMatrix(ref const(MFMatrix) world)			{ MFModel_SetWorldMatrix(pModel, world); }

	@property ref const(MFBoundingVolume) boundingVolume() const	{ return *MFModel_GetBoundingVolume(pModel); }

	@property inout(MFAnimation)* animation() inout					{ return MFModel_GetAnimation(pModel); }

	@property inout(Subobjects) subobjects() inout					{ return inout(Subobjects)(pModel, 0, MFModel_GetNumSubObjects(pModel)); }
	@property inout(Bones) bones() inout							{ return inout(Bones)(pModel, 0, MFModel_GetNumBones(pModel)); }
	@property inout(Tags) tags() inout								{ return inout(Tags)(pModel, 0, MFModel_GetNumTags(pModel)); }

	struct Subobject
	{
	nothrow:
	@nogc:
		this(MFModel* pModel, size_t index)
		{
			this.pModel = pModel;
			subobject = cast(int)index;
		}

		bool opCast(T)() const pure if(is(T == bool))				{ return subobject != -1; }

		@property int index() const pure							{ return subobject; }
		@property const(char)[] name() const						{ return MFModel_GetSubObjectName(pModel, subobject).toDStr; }

		@property bool enabled() const								{ return MFModel_IsSubobjectEnabed(pModel, subobject); }
		@property void enabled(bool bEnable)						{ return MFModel_EnableSubobject(pModel, subobject, bEnable); }

		@property inout(MFMeshChunk)* meshChunk(size_t index) inout	{ return MFModel_GetMeshChunk(pModel, subobject, cast(int)index); }

	private:
		MFModel* pModel;
		int subobject;
	}

	struct Bone
	{
	nothrow:
	@nogc:
		this(MFModel* pModel, size_t index)
		{
			this.pModel = pModel;
			bone = cast(int)index;
		}

		bool opCast(T)() const pure if(is(T == bool))				{ return bone != -1; }

		@property int index() const pure							{ return bone; }
		@property const(char)[] name() const						{ return MFModel_GetBoneName(pModel, bone).toDStr; }

		@property ref const(MFMatrix) origin() const				{ return MFModel_GetBoneOrigin(pModel, bone); }

	private:
		MFModel* pModel;
		int bone;
	}

	struct Tag
	{
	nothrow:
	@nogc:
		this(MFModel* pModel, size_t index)
		{
			this.pModel = pModel;
			tag = cast(int)index;
		}

		bool opCast(T)() const pure if(is(T == bool))				{ return tag != -1; }

		@property int index() const pure							{ return tag; }
		@property const(char)[] name() const						{ return MFModel_GetTagName(pModel, tag).toDStr; }

		@property ref const(MFMatrix) matrix() const				{ return MFModel_GetTagMatrix(pModel, tag); }

	private:
		MFModel* pModel;
		int tag;
	}

	alias Subobjects = ResourceRange!(MFModel, Subobject, MFModel_GetSubObjectIndex);
	alias Bones = ResourceRange!(MFModel, Bone, MFModel_GetBoneIndex);
	alias Tags = ResourceRange!(MFModel, Tag, MFModel_GetTagIndex);
}
