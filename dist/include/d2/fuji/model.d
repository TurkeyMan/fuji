module fuji.model;

public import fuji.c.MFModel;

import fuji.resource;
import fuji.boundingvolume;
import fuji.matrix;
import fuji.animation;

import std.string;

struct Model
{
	MFModel *pModel;
	alias pModel this;

	this(this) pure nothrow
	{
		MFResource_AddRef(cast(fuji.resource.MFResource*)pModel);
	}

	this(ref Resource resource) pure nothrow
	{
		// TODO: should this throw instead?
		if(resource.type == MFResourceType.Model)
		{
			resource.AddRef();
			pModel = cast(MFModel*)resource.handle;
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

	void create(const(char)[] name, const(char)[] animation = null) nothrow
	{
		release();
		pModel = MFModel_CreateWithAnimation(name.toStringz(), animation ? animation.toStringz() : null);
	}

	int release() nothrow
	{
		int rc = 0;
		if(pModel)
		{
			rc = MFModel_Release(pModel);
			pModel = null;
		}
		return rc;
	}

	@property inout(MFModel)* handle() inout pure nothrow					{ return pModel; }
	@property ref inout(Resource) resource() inout pure nothrow				{ return *cast(inout(Resource)*)&this; }

	@property const(char)[] name() const nothrow							{ return MFModel_GetName(pModel).toDStr; }

	@property void worldMatrix(ref const(MFMatrix) world) nothrow			{ MFModel_SetWorldMatrix(pModel, world); }

	@property ref const(MFBoundingVolume) boundingVolume() const nothrow	{ return *MFModel_GetBoundingVolume(pModel); }

	@property inout(MFAnimation)* animation() inout nothrow					{ return MFModel_GetAnimation(pModel); }

	@property inout(Subobjects) subobjects() inout nothrow					{ return inout(Subobjects)(pModel, 0, MFModel_GetNumSubObjects(pModel)); }
	@property inout(Bones) bones() inout nothrow							{ return inout(Bones)(pModel, 0, MFModel_GetNumBones(pModel)); }
	@property inout(Tags) tags() inout nothrow								{ return inout(Tags)(pModel, 0, MFModel_GetNumTags(pModel)); }

	struct Subobject
	{
		this(MFModel* pModel, size_t index) nothrow
		{
			this.pModel = pModel;
			subobject = cast(int)index;
		}

		bool opCast(T)() const pure nothrow if(is(T == bool))				{ return subobject != -1; }

		@property int index() const pure nothrow							{ return subobject; }
		@property const(char)[] name() const nothrow						{ return MFModel_GetSubObjectName(pModel, subobject).toDStr; }

		@property bool enabled() const nothrow								{ return MFModel_IsSubobjectEnabed(pModel, subobject); }
		@property void enabled(bool bEnable) nothrow						{ return MFModel_EnableSubobject(pModel, subobject, bEnable); }

		@property inout(MFMeshChunk)* meshChunk(size_t index) inout nothrow	{ return MFModel_GetMeshChunk(pModel, subobject, cast(int)index); }

	private:
		MFModel* pModel;
		int subobject;
	}

	struct Bone
	{
		this(MFModel* pModel, size_t index) nothrow
		{
			this.pModel = pModel;
			bone = cast(int)index;
		}

		bool opCast(T)() const pure nothrow if(is(T == bool))				{ return bone != -1; }

		@property int index() const pure nothrow							{ return bone; }
		@property const(char)[] name() const nothrow						{ return MFModel_GetBoneName(pModel, bone).toDStr; }

		@property ref const(MFMatrix) origin() const nothrow				{ return MFModel_GetBoneOrigin(pModel, bone); }

	private:
		MFModel* pModel;
		int bone;
	}

	struct Tag
	{
		this(MFModel* pModel, size_t index) nothrow
		{
			this.pModel = pModel;
			tag = cast(int)index;
		}

		bool opCast(T)() const pure nothrow if(is(T == bool))				{ return tag != -1; }

		@property int index() const pure nothrow							{ return tag; }
		@property const(char)[] name() const nothrow						{ return MFModel_GetTagName(pModel, tag).toDStr; }

		@property ref const(MFMatrix) matrix() const nothrow				{ return MFModel_GetTagMatrix(pModel, tag); }

	private:
		MFModel* pModel;
		int tag;
	}

	alias Subobjects = ResourceRange!(MFModel, Subobject, MFModel_GetSubObjectIndex);
	alias Bones = ResourceRange!(MFModel, Bone, MFModel_GetBoneIndex);
	alias Tags = ResourceRange!(MFModel, Tag, MFModel_GetTagIndex);
}
